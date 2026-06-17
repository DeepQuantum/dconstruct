#include "dcclsp/server.h"
#include "dcclsp/sidbase_lookup.h"
#include "lsp/messages.h"
#include "lsp/types.h"
#include <string_view>

using namespace std::string_view_literals;

namespace dconstruct::dcclsp {

    void server::init_callbacks() {

        {
            using req = lsp::requests::Initialize;
            m_messageHandler.add<req>([](req::Params&&) -> req::Result {
                return {
                    .capabilities = {
                        .positionEncoding = lsp::PositionEncodingKind::UTF16,
                        .textDocumentSync = lsp::TextDocumentSyncOptions{
                            .openClose = true,
                            .change    = lsp::TextDocumentSyncKind::Full,
                            .save      = true
                        },
                        .completionProvider = lsp::CompletionOptions {
                            .triggerCharacters = lsp::Array<lsp::String>{ "#" },
                        },
                    },
                    .serverInfo = lsp::InitializeResultServerInfo {
                        .name = "dcpl language server",
                        .version = "1.0.0",
                    },
                };
            });
        }

        {
            using noti = lsp::notifications::TextDocument_DidOpen;
            m_messageHandler.add<noti>([this](noti::Params&& params) -> void {
                m_documents[params.textDocument.uri.data()] = std::move(params.textDocument);
            });
        }

        {
            using noti = lsp::notifications::TextDocument_DidChange;
            m_messageHandler.add<noti>([this](noti::Params&& params) {
                this->on_document_did_change(std::move(params));
            });
        }

        {
            using req = lsp::requests::TextDocument_Completion;
            m_messageHandler.add<req>([this](req::Params&& params) -> req::Result {
                if (params.context) {
                    if (std::optional<lsp::String> starting_char = params.context->triggerCharacter; (*starting_char)[0] == '#') {
                        return lsp::CompletionList{ .isIncomplete = true, .items = {}};
                    } else {
                        return this->finish_hash_completion(std::move(params));
                    }
                }

                stop_with_error("undefined completion function"sv);
                return {};
            });
        }
    }

    void server::run() {
        while (!m_stopReason) {
            m_messageHandler.processIncomingMessages();
        }
        switch (*m_stopReason) {
            using enum STOP_REASON;
            case ERROR: {
                m_messageHandler.sendNotification<lsp::notifications::Window_LogMessage>(lsp::notifications::Window_LogMessage::Params {
                    .type = lsp::MessageType::Error,
                    .message = std::move(*m_error),
                });
                break;
            }
            case SHUTDOWN: {
                m_messageHandler.sendNotification<lsp::notifications::Window_LogMessage>(lsp::notifications::Window_LogMessage::Params {
                    .type = lsp::MessageType::Info,
                    .message = "shutting down language server",
                });
                break;
            }
        }
    }

    void server::on_document_did_change(lsp::notifications::TextDocument_DidChange::Params&& params) {
        lsp::TextDocumentItem& doc = m_documents.at(params.textDocument.uri.data());
        doc.version = params.textDocument.version;

        for (const auto& change : params.contentChanges) {
            const auto* ranged_change = std::get_if<lsp::TextDocumentContentChangeEvent_Range_Text>(&change);
            if (!ranged_change) {
                stop_with_error("we only ever expect ranged changes, but the server received a change for the whole document."sv);
                return;
            }

            const u64 start = position_to_string_index(doc.text, ranged_change->range.start);
            const u64 stop = position_to_string_index(doc.text, ranged_change->range.end);

            doc.text.replace(doc.text.begin() + start, doc.text.begin() + stop, ranged_change->text);
        }
    }

    [[nodiscard]] lsp::requests::TextDocument_Completion::Result server::finish_hash_completion(lsp::requests::TextDocument_Completion::Params&& params) {
        const lsp::TextDocumentItem& doc = m_documents.at(params.textDocument.uri.data());
        const u64 completion_pos = position_to_string_index(doc.text, params.position);

        if (completion_pos > doc.text.size()) {
            return {};
        }
        auto completion_iter = doc.text.begin() + completion_pos - 1;

        if (*completion_iter == ' ' || *completion_iter == '\n') {
            return lsp::CompletionList { .isIncomplete = false, .items = {} };
        }

        const u64 hash = doc.text.rfind('#', completion_pos);
        if (hash == std::string::npos) {
            stop_with_error("oopsie, so this is awkward UwU. we are never supposed to have a isIncomplete completion without a '#' before!!");
            return {};
        }

        std::string needle = doc.text.substr(hash + 1, completion_pos - hash);

        return get_sidbase_matches(needle, m_sidbase);
    }

    void server::stop_with_error(std::string_view message) {
        m_stopReason = STOP_REASON::ERROR;
        m_error.emplace(message);
    }

    [[nodiscard]] u64 position_to_string_index(const std::string& text, const lsp::Position& pos) {

        if (pos.line == 0) {
            return pos.character;
        }

        u64 line = 0;

        for (u64 i = 0; i < text.size(); ++i) {
            char c = text[i];

            if (c != '\r' && c != '\n') {
                continue;
            }

            if (c == '\r' && i + 1 < text.size() && text[i + 1] == '\n') {
                i++;
            }

            line++;
            u64 line_start = i + 1;

            if (line == pos.line) {
                return line_start + pos.character;
            }
        }

        return text.size();
    }

}
