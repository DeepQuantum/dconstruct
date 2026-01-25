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
        [[nodiscard]] std::vector<ast::global_decl_uptr> parse();

        [[nodiscard]] std::tuple<std::vector<ast::global_decl_uptr>, std::unordered_map<std::string, ast::full_type>, std::vector<compiler::parsing_error>> get_results() {
            return { parse(), get_known_types(), get_errors() };
        }
        
        inline const std::vector<parsing_error>& get_errors() const noexcept {
            return m_errors;
        }

        inline const std::unordered_map<std::string, ast::full_type>& get_known_types() const noexcept {
            return m_knownTypes;
        }
    
    private:
        std::vector<token> m_tokens;
        std::vector<parsing_error> m_errors;

        std::unordered_map<std::string, ast::full_type> m_knownTypes {
            {"u8", make_type_from_prim(ast::primitive_kind::U8)},
            {"u16", make_type_from_prim(ast::primitive_kind::U16)},
            {"u32", make_type_from_prim(ast::primitive_kind::U32)},
            {"u64", make_type_from_prim(ast::primitive_kind::U64)},
            {"i8", make_type_from_prim(ast::primitive_kind::I8)},
            {"i16", make_type_from_prim(ast::primitive_kind::I16)},
            {"i32", make_type_from_prim(ast::primitive_kind::I32)},
            {"i64", make_type_from_prim(ast::primitive_kind::I64)},
            {"f32", make_type_from_prim(ast::primitive_kind::F32)},
            {"f64", make_type_from_prim(ast::primitive_kind::F64)},
            {"char", make_type_from_prim(ast::primitive_kind::CHAR)},
            {"bool", make_type_from_prim(ast::primitive_kind::BOOL)},
            {"string", make_type_from_prim(ast::primitive_kind::STRING)},
            {"sid", make_type_from_prim(ast::primitive_kind::SID)},
            {"void", make_type_from_prim(ast::primitive_kind::NOTHING)}
        };

        u32 m_current = 0;

        void synchronize_statements();
        void synchronize_external_definitions();

        const token& advance();
        const token* consume(const token_type, const std::string&);
        [[nodiscard]] const token& peek() const;
        [[nodiscard]] const token& previous() const;
        [[nodiscard]] bool is_at_end() const;
        [[nodiscard]] bool check(const token_type) const;
        [[nodiscard]] bool match(const std::initializer_list<token_type>& types);
        [[nodiscard]] std::optional<ast::full_type> make_type();
        [[nodiscard]] std::optional<ast::full_type> peek_type();
        [[nodiscard]] std::optional<ast::function_type> match_function_type();


        [[nodiscard]] std::unique_ptr<ast::variable_declaration> make_var_declaration();
        [[nodiscard]] std::unique_ptr<ast::variable_declaration> make_var_declaration(ast::full_type type);
        [[nodiscard]] stmnt_uptr make_declaration();
        [[nodiscard]] stmnt_uptr make_statement();
        [[nodiscard]] std::unique_ptr<ast::block> make_block();
        [[nodiscard]] std::unique_ptr<ast::if_stmt> make_if();
        [[nodiscard]] std::unique_ptr<ast::while_stmt> make_while();
        [[nodiscard]] std::unique_ptr<ast::return_stmt> make_return();
        [[nodiscard]] stmnt_uptr make_for();
        [[nodiscard]] stmnt_uptr make_foreach();
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
        [[nodiscard]] expr_uptr make_literal();
        [[nodiscard]] expr_uptr make_match();
        [[nodiscard]] expr_uptr make_call();
        [[nodiscard]] std::unique_ptr<ast::call_expr> finish_call(expr_uptr&& expr);
        [[nodiscard]] std::unique_ptr<ast::subscript_expr> finish_subscript(expr_uptr&& expr);
        [[nodiscard]] std::optional<ast::struct_type> make_struct_type();
        [[nodiscard]] std::unique_ptr<ast::using_declaration> make_using_declaration(); 
        [[nodiscard]] std::optional<ast::enum_type> make_enum_type();
        [[nodiscard]] std::unique_ptr<ast::function_definition> make_function_definition();
        [[nodiscard]] std::optional<global> make_global();
        [[nodiscard]] ast::full_type make_type_from_string(const std::string&);
    };

    [[nodiscard]] inline bool operator==(const parsing_error& lhs, const parsing_error& rhs) noexcept {
        return lhs.m_token == rhs.m_token && lhs.m_message == rhs.m_message;
    }
}