#include "dcplsp/server.h"
#include "dcplsp/sidbase_lookup.h"
#include "compilation/lexer.h"
#include "compilation/dc_parser.h"
#include "lsp/messages.h"
#include "lsp/types.h"
#include "compilation/dc_parser.h"
#include <format>
#include <string_view>
#include <print>
#include <chrono>
#include <ostream>


using namespace std::string_view_literals;

namespace dconstruct::dcplsp {

    template<typename ...Args>
    void server::log(std::format_string<Args...> fmt, Args&& ...args) const {
        std::ofstream log("dcplsp.log", std::ios::app);

        const auto now = std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now());
        std::println(stderr, "[{:%Y-%m-%d %H:%M:%S}] {}", now, std::format(fmt, std::forward<Args>(args)...));
        std::println(log, "[{:%Y-%m-%d %H:%M:%S}] {}", now, std::format(fmt, std::forward<Args>(args)...));

        log.flush();
    }

    void server::init_callbacks() {

        {
            using req = lsp::requests::Initialize;
            m_messageHandler.add<req>([this](req::Params&&) -> req::Result {
                log("initialized");
                return {
                    .capabilities = {
                        .positionEncoding = lsp::PositionEncodingKind::UTF16,
                        .textDocumentSync = lsp::TextDocumentSyncOptions{
                            .openClose = true,
                            .change    = lsp::TextDocumentSyncKind::Incremental,
                            .save      = true,
                        },
                        .completionProvider = lsp::CompletionOptions {
                            .triggerCharacters = lsp::Array<lsp::String>{ "#" },
                        },
                        .semanticTokensProvider = lsp::SemanticTokensOptions {
                            .legend = lsp::SemanticTokensLegend {
                                .tokenTypes = compilation::Parser::semantic_token_ctx::TOKEN_TYPES,
                            },
                            .full = true,
                        }
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
                log("opened document");
                m_documents[std::string(params.textDocument.uri.data())] = std::move(params.textDocument);
            });
        }

        {
            using noti = lsp::notifications::TextDocument_DidChange;
            m_messageHandler.add<noti>([this](noti::Params&& params) {
                log("changed document");
                this->on_document_did_change(std::move(params));
            });
        }

        {
            using req = lsp::requests::TextDocument_Completion;
            m_messageHandler.add<req>([this](req::Params&& params) -> req::Result {
                log("started completion");
                if (params.context) {
                    if (std::optional<lsp::String> starting_char = params.context->triggerCharacter; starting_char && (*starting_char)[0] == '#') {
                        log("got starting char");
                        return lsp::CompletionList{ .isIncomplete = true, .items = {}};
                    } else {
                        log("finishing completion");
                        return this->finish_hash_completion(std::move(params));
                    }
                }

                stop_with_error("undefined completion function"sv);
                return {};
            });
        }

        {
            using req = lsp::requests::TextDocument_SemanticTokens_Full;
            m_messageHandler.add<req>([this](req::Params&& params) -> req::Result {
                log("started semantic token stuff");
                const lsp::TextDocumentItem& doc = m_documents.at(params.textDocument.uri.toString());
                std::vector<u32> new_tokens = make_semantic_tokens(doc.text);
                if (!new_tokens.empty()) {
                    m_semanticTokens = std::move(new_tokens);
                }
                return lsp::SemanticTokens { m_semanticTokens };
            });
        }
    }

    void server::run() {
        log("started language server");
        while (!m_stopReason) {
            m_messageHandler.processIncomingMessages();
        }
        switch (*m_stopReason) {
            using enum STOP_REASON;
            case ERROR: {
                log("[ERROR]: {}", *m_error);
                m_messageHandler.sendNotification<lsp::notifications::Window_LogMessage>(lsp::notifications::Window_LogMessage::Params {
                    .type = lsp::MessageType::Error,
                    .message = std::move(*m_error),
                });
                break;
            }
            case SHUTDOWN: {
                log("shutting down language server");
                m_messageHandler.sendNotification<lsp::notifications::Window_LogMessage>(lsp::notifications::Window_LogMessage::Params {
                    .type = lsp::MessageType::Info,
                    .message = "shutting down language server",
                });
                break;
            }
        }
    }

    void server::on_document_did_change(lsp::notifications::TextDocument_DidChange::Params&& params) {
        lsp::TextDocumentItem& doc = m_documents.at(std::string(params.textDocument.uri.data()));
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
        const lsp::TextDocumentItem& doc = m_documents.at(std::string(params.textDocument.uri.data()));
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

        std::string needle = doc.text.substr(hash + 1, completion_pos - hash - 1);
        log("needle: {}", needle);
        auto matches = get_sidbase_matches(needle, m_sidbase);

        const lsp::Range replace_range {
            .start = lsp::Position{ .line = params.position.line, .character = params.position.character - static_cast<lsp::uint>(needle.size()) },
            .end   = params.position,
        };
        for (lsp::CompletionItem& item : matches.items) {
            item.filterText = item.label;
            item.textEdit = lsp::TextEdit{ .range = replace_range, .newText = item.label };
        }

        log("found {} matches", matches.items.size());
        return matches;
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

    [[nodiscard]] std::vector<u32> make_semantic_tokens(std::string text) {
        if (text.empty()) {
            return {};
        }

        compilation::Lexer lexer{std::move(text)};

        const auto& [tokens, lex_errors] = lexer.get_results();
        if (!lex_errors.empty()) {
            return {};
        }

        compilation::Parser parser{std::move(tokens)};
        auto program = parser.parse();

        if (!parser.get_errors().empty()) {
            return {};
        }

        return parser.get_semantic_tokens();
    }

}
