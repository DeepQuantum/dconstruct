#include "base.h"
#include <vector>
#include "tokens.h"

namespace dconstruct::compiler {

struct lexing_error {
    u32 m_line;
    std::string m_message;

    [[nodiscard]] b8 operator==(const lexing_error &rhs) const noexcept {
        return m_line == rhs.m_line && m_message == rhs.m_message;
    }
};


class Lexer {
public:
    Lexer(const std::string &source) : m_source(source) {};
    std::vector<token>& scan_tokens() noexcept;
    const std::vector<lexing_error>& get_errors() const noexcept;

private:
    std::vector<token> m_tokens;
    std::vector<lexing_error> m_errors;
    std::string m_source;

    u32 m_start = 0;
    u32 m_current = 0;
    u32 m_line = 1;

    b8 m_hadError = false;

    token scan_token() noexcept;
    b8 reached_eof() const noexcept;
    token make_current_token(const token_type, const token::t_literal& = 0ULL) const noexcept;
    token make_string() noexcept;
    token make_number() noexcept;
    char advance() noexcept;
    b8 match(const char) noexcept;
    char peek() const noexcept;
    char peek_next() const noexcept;
};

}