#pragma once

#include "base.h"
#include "tokens.h"
#include "ast/ast.h"

#include <vector>
#include <unordered_map>

namespace dconstruct::compiler {

    struct parsing_error {
        token m_token;
        std::string m_message;
    };

    class Parser {
    public:
        explicit Parser(const std::vector<token> &tokens) : m_tokens(tokens) {};
        [[nodiscard]] std::vector<stmnt_uptr> parse();
        
        inline const std::vector<parsing_error>& get_errors() const noexcept {
            return m_errors;
        }
    
    private:
        std::vector<token> m_tokens;
        std::vector<parsing_error> m_errors;

        std::unordered_map<std::string, ast::primitive_kind> m_knownTypes{
            {"u8", ast::primitive_kind::U8},
            {"u16", ast::primitive_kind::U16},
            {"u32", ast::primitive_kind::U32},
            {"u64", ast::primitive_kind::U64},
            {"i8", ast::primitive_kind::I8},
            {"i16", ast::primitive_kind::I16},
            {"i32", ast::primitive_kind::I32},
            {"i64", ast::primitive_kind::I64},
            {"f32", ast::primitive_kind::F32},
            {"f64", ast::primitive_kind::F64},
            {"char", ast::primitive_kind::CHAR},
            {"bool", ast::primitive_kind::BOOL},
            {"string", ast::primitive_kind::STRING},
            {"sid", ast::primitive_kind::SID}
        };

        u32 m_current = 0;

        void synchronize();

        const token& advance();
        const token* consume(const token_type, const std::string&);
        [[nodiscard]] const token& peek() const;
        [[nodiscard]] const token& previous() const;
        [[nodiscard]] b8 is_at_end() const;
        [[nodiscard]] b8 check(const token_type) const;
        [[nodiscard]] b8 match(const std::initializer_list<token_type>& types);
        [[nodiscard]] b8 match_type();

        [[nodiscard]] std::unique_ptr<ast::variable_declaration> make_var_declaration();
        [[nodiscard]] stmnt_uptr make_declaration();
        [[nodiscard]] stmnt_uptr make_statement();
        [[nodiscard]] std::unique_ptr<ast::block> make_block();
        [[nodiscard]] std::unique_ptr<ast::if_stmt> make_if();
        [[nodiscard]] std::unique_ptr<ast::while_stmt> make_while();
        [[nodiscard]] stmnt_uptr make_for();
        [[nodiscard]] std::unique_ptr<ast::expression_stmt> make_expression_statement();
        [[nodiscard]] expr_uptr make_or();
        [[nodiscard]] expr_uptr make_and();
        [[nodiscard]] expr_uptr make_assignment();
        [[nodiscard]] expr_uptr make_expression();
        [[nodiscard]] expr_uptr make_equality();
        [[nodiscard]] expr_uptr make_comparison();
        [[nodiscard]] expr_uptr make_term();
        [[nodiscard]] expr_uptr make_factor();
        [[nodiscard]] expr_uptr make_unary();
        [[nodiscard]] expr_uptr make_primary();
    };
}