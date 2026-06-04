#pragma once

#include "compilation/tokens.h"
#include "compilation/environment.h"

#include <format>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

using namespace std::literals;

namespace dconstruct::ast {

    struct ast_element;

    enum class FOREACH_OPTIMIZATION_ACTION : u8 {
        BEGIN_FOREACH,
        END_FOREACH,
        ITERABLE_COUNT,
        ITERABLE_AT,
        FOR,
        NONE,
    };

    enum class VAR_OPTIMIZATION_ACTION : u8 {
        VAR_DECLARATION,
        VAR_READ,
        VAR_WRITE,
        VAR_DEREFERENCE,
        NONE
    };

    enum class MATCH_OPTIMIZATION_ACTION : u8 {
        RESULT_VAR_DECLARATION,
        RESULT_VAR_WRITE,
        RESULT_VAR_ASSIGNMENT,
        CHECK_VAR_SET,
        CHECK_VAR_READ,
        MATCH_CONDITION_COMPARISON,
        MATCH_CONDITION_CHECKED_COMPARISON,
        LITERAL,
        NONE,
    };

    enum class LANGUAGE_FLAGS : u8 {
        C = 0x1,
        PY = 0x2,
        RACKET = 0x4,
        FUNCTION_NAMES_PASCAL = 0x8,
        IDENTIFIER_PASCAL = 0x10,
        COMPILER = 0x20,
    };

    enum class AST_COLOR : u8 {
        BLANK,
        NUMBER,
        SID,
        IDENTIFIER,
        MEMBER,
        TYPE,
        CALL,
        KEYWORD,
        STRING,
        COMMENT,
        OPERATOR,
        PUNCTUATION,
    };

    using code_color_buffer = std::vector<std::pair<AST_COLOR, std::string>>;

    [[nodiscard]] constexpr LANGUAGE_FLAGS operator|(const LANGUAGE_FLAGS lhs, const LANGUAGE_FLAGS rhs) noexcept {
        return static_cast<LANGUAGE_FLAGS>(static_cast<u8>(lhs) | static_cast<u8>(rhs));
    }

    [[nodiscard]] constexpr LANGUAGE_FLAGS operator&(const LANGUAGE_FLAGS lhs, const LANGUAGE_FLAGS rhs) noexcept {
        return static_cast<LANGUAGE_FLAGS>(static_cast<u8>(lhs) & static_cast<u8>(rhs));
    }

    [[nodiscard]] constexpr LANGUAGE_FLAGS operator~(const LANGUAGE_FLAGS flags) noexcept {
        return static_cast<LANGUAGE_FLAGS>(~static_cast<u8>(flags));
    }

    template<typename T> 
    concept reservable = requires(T& t, const u64 size) {
        { t.reserve(size) } -> std::same_as<void>;
    };

    template<reservable T>
    struct ast_buffer_wrapper {
        explicit ast_buffer_wrapper(const u64 size = 0) {
            m_buffer.reserve(size);
        }

        void reserve(const u64 size) {
            m_buffer.reserve(size);
        }

         void indent_more() noexcept {
            ++m_currentIndent;
        }

        void indent_less() noexcept {
            if (m_currentIndent != 0) {
                --m_currentIndent;
            }
        }

        [[nodiscard]] bool has_flag(const LANGUAGE_FLAGS flag) const noexcept {
            return static_cast<u8>(m_flags & flag) != 0;
        }

        void set_flag(const LANGUAGE_FLAGS flag) noexcept {
            m_flags = m_flags | flag;
        }

        void clear_flag(const LANGUAGE_FLAGS flag) noexcept {
            m_flags = static_cast<LANGUAGE_FLAGS>(static_cast<u8>(m_flags) & ~static_cast<u8>(flag));
        }

        [[nodiscard]] const T& str() const noexcept {
            return m_buffer;
        }

        [[nodiscard]] T take() noexcept {
            return std::move(m_buffer);
        }

        u8 m_currentIndent = 0;
        u8 m_indentWidth = 4;
        LANGUAGE_FLAGS m_flags = LANGUAGE_FLAGS::C;
        T m_buffer;
    };

    struct ast_serialization_buffer : ast_buffer_wrapper<std::string> {
        using ast_buffer_wrapper::ast_buffer_wrapper;

        void _append(const char value) {
            m_buffer.push_back(value);
        }

        void _append(const std::string_view value) {
            m_buffer.append(value);
        }

        template <typename T> requires(std::is_arithmetic_v<std::remove_cvref_t<T>> && !std::is_same_v<std::remove_cvref_t<T>, char>)
        void _append(T value) {
            append_format("{}", value);
        }

        void _append(std::string&& value) {
            if (m_buffer.empty()) {
                m_buffer = std::move(value);
            } else {
                m_buffer.append(value);
            }
        }

        void _append(const ast_element& element);

        template <typename... Args>
        void append(Args&&... args) {
            (_append(std::forward<Args>(args)), ...);
        }

        void append_padded(const std::string_view value, const size_t width) {
            _append(value);
            if (value.size() < width) {
                m_buffer.append(width - value.size(), ' ');
            }
        }

        template <typename... Args>
        void append_format(std::format_string<Args...> fmt, Args&&... args) {
            std::format_to(std::back_inserter(m_buffer), fmt, std::forward<Args>(args)...);
        }

        void append_indent() {
            m_buffer.append(static_cast<size_t>(m_currentIndent) * m_indentWidth, ' ');
        }

        [[nodiscard]] const std::string& str() const noexcept {
            return m_buffer;
        }
    };

    struct code_color_serialization_buffer : ast_buffer_wrapper<code_color_buffer>  {
        using ast_buffer_wrapper::ast_buffer_wrapper;

        void _append(const AST_COLOR color, const char value) {
            m_buffer.emplace_back(color, std::string(1, value));
        }

        void _append(const AST_COLOR color, const char* value) {
            m_buffer.emplace_back(color, std::string(value));
        }

        void _append(const AST_COLOR color, const std::string_view value) {
            m_buffer.emplace_back(color, std::string(value));
        }

        void _append(const AST_COLOR color, const std::string& value) {
            m_buffer.emplace_back(color, value);
        }

        void _append(const AST_COLOR color, std::string&& value) {
            m_buffer.emplace_back(color, std::move(value));
        }

        template <typename T> requires(std::is_arithmetic_v<std::remove_cvref_t<T>> && !std::is_same_v<std::remove_cvref_t<T>, char>)
        void _append(const AST_COLOR color, const T value) {
            m_buffer.emplace_back(color, std::format("{}", value));
        }

        void _append(const ast_element& element);

        template <typename... Args>
        void append(const AST_COLOR color, Args&&... args) {
            (_append(color, std::forward<Args>(args)), ...);
        }

        void append(const ast_element& element) {
            _append(element);
        }

        void append_padded(const AST_COLOR color, const std::string_view value, const size_t width) {
            if (value.size() < width) {
                std::string padded;
                padded.reserve(width);
                padded.append(value);
                padded.append(width - value.size(), ' ');
                m_buffer.emplace_back(color, std::move(padded));
            } else {
                m_buffer.emplace_back(color, std::string(value));
            }
        }

        template <typename... Args>
        void append_format(const AST_COLOR color, std::format_string<Args...> fmt, Args&&... args) {
            m_buffer.emplace_back(color, std::format(fmt, std::forward<Args>(args)...));
        }

        void append_indent() {
            if (m_currentIndent != 0) {
                m_buffer.emplace_back(AST_COLOR::BLANK, std::string(static_cast<size_t>(m_currentIndent) * m_indentWidth, ' '));
            }
        }
    };

    struct ast_element {
        virtual ~ast_element() = default;
        virtual void pseudo_c(ast_serialization_buffer& buffer) const = 0;
        virtual void pseudo_py(ast_serialization_buffer& buffer) const = 0;
        virtual void pseudo_racket(ast_serialization_buffer& buffer) const = 0;
        virtual void pseudo_c_for_compiler(ast_serialization_buffer& buffer) const;

        virtual void regex_optimization_pass() noexcept = 0;
        

        virtual bool is_dead_code() const noexcept { return false; }
        [[nodiscard]] virtual std::optional<compilation::source_location> source_location() const noexcept { return std::nullopt; }

        [[nodiscard]] std::string to_pseudo_c_string() const noexcept {
            ast_serialization_buffer buffer;
            pseudo_c(buffer);
            return buffer.take();
        }

        virtual void to_pseudo_c_colored_string(code_color_serialization_buffer& buffer) const noexcept {
            buffer.append(AST_COLOR::BLANK, to_pseudo_c_string());
        }

        [[nodiscard]] std::string to_c_for_compiler_string() const noexcept {
            ast_serialization_buffer buffer;
            pseudo_c_for_compiler(buffer);
            return buffer.take();
        }

    };

    inline void ast_element::pseudo_c_for_compiler(ast_serialization_buffer& buffer) const {
        buffer.set_flag(LANGUAGE_FLAGS::COMPILER);
        pseudo_c(buffer);
    }

    inline void serialize_ast(ast_serialization_buffer& buffer, const ast_element& element) {
    }

    inline void ast_serialization_buffer::_append(const ast_element& element) {
        if (has_flag(LANGUAGE_FLAGS::COMPILER)) {
            element.pseudo_c_for_compiler(*this);
        } else if (has_flag(LANGUAGE_FLAGS::RACKET)) {
            element.pseudo_racket(*this);
        } else if (has_flag(LANGUAGE_FLAGS::PY)) {
            element.pseudo_py(*this);
        } else {
            element.pseudo_c(*this);
        }
    }

    inline void code_color_serialization_buffer::_append(const ast_element& element) {
        element.to_pseudo_c_colored_string(*this);
    }
}
