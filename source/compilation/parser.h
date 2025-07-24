#pragma once

#include "base.h"
#include "tokens.h"
#include "binary_expressions.h"

#include <vector>

namespace dconstruct::compiler {

    struct parser_error {
        
    };

    class Parser {
    public:
        explicit Parser(const std::vector<token> &tokens) : m_tokens(tokens) {};
    
    
    private:
        std::vector<token> m_tokens;
        std::vector<parser_error> m_errors;
        u32 m_current = 0;

        const token& advance() noexcept;
        [[nodiscard]] const token& peek() const noexcept;
        [[nodiscard]] const token& previous() const noexcept;
        [[nodiscard]] b8 is_at_end() const noexcept;
        [[nodiscard]] b8 check(const token_type) const noexcept;
        [[nodiscard]] b8 match(const std::initializer_list<token_type>& types) noexcept;

        [[nodiscard]] std::unique_ptr<ast::expression> make_expression() noexcept;
        [[nodiscard]] std::unique_ptr<ast::expression> make_equality() noexcept;
        [[nodiscard]] std::unique_ptr<ast::expression> make_comparison() noexcept;
        [[nodiscard]] std::unique_ptr<ast::expression> make_term() noexcept;
    };
}