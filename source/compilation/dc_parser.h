#pragma once

#include "base.h"
#include "tokens.h"
#include "ast.h"

#include <vector>

namespace dconstruct::compiler {

    struct parsing_error {
        token m_token;
        std::string m_message;
    };

    class Parser {
    public:
        explicit Parser(const std::vector<token> &tokens) : m_tokens(tokens) {};
        [[nodiscard]] std::vector<std::unique_ptr<ast::statement>> parse();
        
        inline const std::vector<parsing_error>& get_errors() const noexcept {
            return m_errors;
        }
    
    private:
        std::vector<token> m_tokens;
        std::vector<parsing_error> m_errors;
        u32 m_current = 0;

        const token& advance();
        const token* consume(const token_type, const std::string&);
        [[nodiscard]] const token& peek() const;
        [[nodiscard]] const token& previous() const;
        [[nodiscard]] b8 is_at_end() const;
        [[nodiscard]] b8 check(const token_type) const;
        [[nodiscard]] b8 match(const std::initializer_list<token_type>& types);

        [[nodiscard]] std::unique_ptr<ast::statement> make_statement();
        [[nodiscard]] std::unique_ptr<ast::statement> make_expression_statement();
        [[nodiscard]] std::unique_ptr<ast::expression> make_expression();
        [[nodiscard]] std::unique_ptr<ast::expression> make_equality();
        [[nodiscard]] std::unique_ptr<ast::expression> make_comparison();
        [[nodiscard]] std::unique_ptr<ast::expression> make_term();
        [[nodiscard]] std::unique_ptr<ast::expression> make_factor();
        [[nodiscard]] std::unique_ptr<ast::expression> make_unary();
        [[nodiscard]] std::unique_ptr<ast::expression> make_primary();
    };
}