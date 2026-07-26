#pragma once

#include "base.h"
#include "ast/type.h"
#include <memory>
#include <filesystem>
#include <unordered_map>
#include <variant>
#include <vector>

namespace dconstruct::compilation {
    [[nodiscard]] sid64 get_identifier_sid(const std::string& identifier);

    class source_file_table {
    public:
        source_file_table() = default;
        source_file_table(const source_file_table&) = delete;
        source_file_table& operator=(const source_file_table&) = delete;
        source_file_table(source_file_table&&) noexcept = default;
        source_file_table& operator=(source_file_table&&) noexcept = default;

        [[nodiscard]] const std::filesystem::path* intern(std::filesystem::path path);
        void clear() noexcept;

    private:
        std::vector<std::unique_ptr<std::filesystem::path>> m_files;
        std::unordered_map<std::string, const std::filesystem::path*> m_filesByPath;
    };

    struct source_location {
        const std::filesystem::path* m_file;
        u32 m_line = INT_MAX;
        u32 m_char  = INT_MAX;
    };

    [[nodiscard]] std::string format_source_location(const source_location& location);

    enum class token_type {
        LEFT_PAREN,
        RIGHT_PAREN,
        LEFT_BRACE,
        RIGHT_BRACE,
        LEFT_SQUARE,
        RIGHT_SQUARE,
        COMMA,
        DOT,
        SEMICOLON,
        COLON,
        ARROW,
        DOT_DOT_DOT,
        DOLLAR,

        PLUS,
        PLUS_EQUAL,
        PLUS_PLUS,
        MINUS,
        MINUS_EQUAL,
        MINUS_MINUS,
        STAR,
        STAR_EQUAL,
        SLASH,
        SLASH_EQUAL,
        PERCENT,
        PERCENT_EQUAL,
        BANG,
        BANG_EQUAL,
        EQUAL,
        EQUAL_EQUAL,
        EQUAL_GREATER,
        GREATER,
        GREATER_EQUAL,
        GREATER_GREATER,
        LESS,
        LESS_EQUAL,
        LESS_LESS,
        TILDE,
        PIPE,
        PIPE_PIPE,
        CARET,
        CARET_CARET,
        AMPERSAND,
        AMPERSAND_AMPERSAND,

        SIZEOF,

        AND,
        OR,

        IDENTIFIER,

        AUTO,
        USING,
        AS,
        FAR,
        NEAR,

        BREAK,
        CONTINUE,

        STRING,
        INT,
        DOUBLE,
        HEX,
        SID_NUM,
        SID,
        _NULL,

        ELSE,
        FALSE,
        IF,
        RETURN,
        STRUCT,
        ENUM,
        TRUE,
        WHILE,
        FOR,
        FOREACH,
        MATCH,

        BREAKPOINT,

        STATESCRIPT,
        DECLARATIONS,
        OPTIONS,
        STATE,
        BLOCK,
        TRACK,
        LAMBDA,

        TYPEMAP,

        START,
        END,
        EVENT,
        UPDATE,
        VIRTUAL,

        _EOF,
        EMPTY
    };

    struct token {
        token(
            token_type type,
            std::string lexeme,
            ast::primitive_value literal = 0,
            u32 line = INT_MAX,
            u32 column = INT_MAX,
            const  std::filesystem::path* file = nullptr
        ) noexcept
            : m_literal(std::move(literal)),
              m_lexeme(std::move(lexeme)),
              m_type(type),
              m_line(line),
              m_char(column),
              m_file(file) {}

        [[nodiscard]] bool operator==(const token& rhs) const;

        ast::primitive_value m_literal;
        std::string m_lexeme;
        token_type m_type;
        u32 m_line;
        u32 m_char;
        const std::filesystem::path* m_file;
    };
}
