#include "base.h"

#include "lsp/messages.h"
#include "lsp/connection.h"
#include "lsp/io/standardio.h"
#include "lsp/messagehandler.h"

#include "lsp/types.h"
#include "lsp/uri.h"
#include "sidbase.h"
#include "sidbase_lookup.h"
#include <expected>
#include <format>
#include <unordered_map>
#include <fstream>

namespace dconstruct::dcplsp {
    struct server_test_access;

    class server {
    public:
        enum class STOP_REASON : u8 {
            ERROR,
            SHUTDOWN,
        };

        server(SIDBase sidbase) : m_connection(lsp::io::standardIO()), m_messageHandler(m_connection), m_sidbase(std::move(sidbase)) {
            init_callbacks();
            m_logfile = std::ofstream("./dcplsp.log", std::ios::app);
        };

        void run();

        server(const server&) = delete;
        server(const server&&) = delete;
        server& operator=(const server&) = delete;
        server& operator=(const server&&) = delete;

    private:
        lsp::Connection m_connection;
        lsp::MessageHandler m_messageHandler;
        SIDBase m_sidbase;
        std::unordered_map<std::string, lsp::TextDocumentItem> m_documents;
        std::optional<STOP_REASON> m_stopReason;
        std::optional<std::string> m_error = std::nullopt;
        std::vector<u32> m_semanticTokens;
        mutable std::ofstream m_logfile;

        void on_document_did_change(lsp::notifications::TextDocument_DidChange::Params&& params);
        [[nodiscard]] lsp::requests::TextDocument_Completion::Result finish_hash_completion(lsp::requests::TextDocument_Completion::Params&& params);

        void init_callbacks();

        void stop_with_error(std::string_view message);



        template<typename ...Args>
        void log(std::format_string<Args...>,  Args&& ...args) const;

        friend struct server_test_access;

    };

    [[nodiscard]] u64 position_to_string_index(const std::string& text, const lsp::Position& pos);
    [[nodiscard]] std::vector<u32> make_semantic_tokens(std::string text);
}
