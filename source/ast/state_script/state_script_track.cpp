#include "ast/state_script/state_script_track.h"

namespace dconstruct::ast {

    namespace {

        void pseudo_c_lambda(ast_serialization_buffer& buffer, const function_definition& lambda) {
            buffer.append(lambda);
        }

        void pseudo_c_lambda(ast_serialization_buffer& buffer, const function_disassembly& lambda) {
            buffer.append("lambda "sv, lambda.get_id(), " {\n"sv);
            buffer.indent_more();
            for (const function_disassembly_line& line : lambda.m_lines) {
                const size_t line_offset = line.m_text.length() < 67 ? 67 - line.m_text.length() : 0;
                buffer.append_indent();
                buffer.append(line.m_text, std::string(line_offset, ' '), line.m_comment, '\n');
            }
            buffer.indent_less();
            buffer.append_indent();
            buffer.append('}');
        }

        void pseudo_c_lambda(ast_serialization_buffer& buffer, const std::shared_ptr<function_disassembly>& lambda) {
            pseudo_c_lambda(buffer, *lambda);
        }

        void pseudo_py_lambda(ast_serialization_buffer& buffer, const function_definition& lambda) {
            buffer.append(lambda);
        }

        void pseudo_py_lambda(ast_serialization_buffer& buffer, const function_disassembly& lambda) {
            buffer.append("lambda "sv, lambda.get_id(), ':');
            for (const function_disassembly_line& line : lambda.m_lines) {
                buffer.append("\n    "sv, line.m_text);
                if (!line.m_comment.empty()) {
                    buffer.append("  # "sv, line.m_comment);
                }
            }
        }

        void pseudo_py_lambda(ast_serialization_buffer& buffer, const std::shared_ptr<function_disassembly>& lambda) {
            pseudo_py_lambda(buffer, *lambda);
        }

        void pseudo_racket_lambda(ast_serialization_buffer& buffer, const function_definition& lambda) {
            buffer.append(lambda);
        }

        void pseudo_racket_lambda(ast_serialization_buffer& buffer, const function_disassembly& lambda) {
            buffer.append("(lambda "sv, lambda.get_id());
            for (const function_disassembly_line& line : lambda.m_lines) {
                buffer.append(" ("sv, line.m_text);
                if (!line.m_comment.empty()) {
                    buffer.append("  "sv, line.m_comment);
                }
                buffer.append(')');
            }
            buffer.append(')');
        }

        void pseudo_racket_lambda(
            ast_serialization_buffer& buffer,
            const std::shared_ptr<function_disassembly>& lambda
        ) {
            pseudo_racket_lambda(buffer, *lambda);
        }

        [[nodiscard]] bool equals_function_disassembly(
            const function_disassembly& lhs,
            const function_disassembly& rhs
        ) noexcept {
            if (lhs.get_id() != rhs.get_id() || lhs.m_lines.size() != rhs.m_lines.size()) {
                return false;
            }
            for (size_t i = 0; i < lhs.m_lines.size(); ++i) {
                const function_disassembly_line& l = lhs.m_lines[i];
                const function_disassembly_line& r = rhs.m_lines[i];
                if (!(l.m_instruction == r.m_instruction) ||
                    l.m_location != r.m_location ||
                    l.m_text != r.m_text ||
                    l.m_comment != r.m_comment ||
                    l.m_target != r.m_target ||
                    l.m_isArgMove != r.m_isArgMove) {
                    return false;
                }
            }
            return true;
        }

        [[nodiscard]] bool equals_lambda(const state_script_lambda& lhs, const state_script_lambda& rhs) noexcept {
            if (lhs.index() != rhs.index()) {
                return false;
            }
            if (const auto* lhs_ast = std::get_if<function_definition>(&lhs)) {
                const auto* rhs_ast = std::get_if<function_definition>(&rhs);
                return rhs_ast != nullptr && lhs_ast->m_body.equals(rhs_ast->m_body);
            }
            const auto* lhs_disassembly = std::get_if<std::shared_ptr<function_disassembly>>(&lhs);
            const auto* rhs_disassembly = std::get_if<std::shared_ptr<function_disassembly>>(&rhs);
            return lhs_disassembly != nullptr && rhs_disassembly != nullptr && *lhs_disassembly && *rhs_disassembly && equals_function_disassembly(**lhs_disassembly, **rhs_disassembly);
        }

    }

    [[nodiscard]] bool operator==(const state_script_track& lhs, const state_script_track& rhs) noexcept {
        return lhs.equals(rhs);
    }

    void state_script_track::pseudo_c(ast_serialization_buffer& buffer) const {
        buffer.append("track "sv, m_name, " {\n"sv);
        buffer.indent_more();
        for (const auto& lam : m_lambdas) {
            buffer.append_indent();
            std::visit(
                [&buffer](const auto& lambda) {
                    pseudo_c_lambda(buffer, lambda);
                },
                lam
            );
            buffer.append('\n');
        }
        buffer.indent_less();
        buffer.append_indent();
        buffer.append('}');
    }

    void state_script_track::pseudo_py(ast_serialization_buffer& buffer) const {
        buffer.append("track "sv, m_name, ':');
        for (const auto& lam : m_lambdas) {
            buffer.append("\n  "sv);
            std::visit(
                [&buffer](const auto& lambda) {
                    pseudo_py_lambda(buffer, lambda);
                },
                lam
            );
        }
    }

    void state_script_track::pseudo_racket(ast_serialization_buffer& buffer) const {
        buffer.append("(track "sv, m_name);
        for (const auto& lam : m_lambdas) {
            buffer.append(' ');
            std::visit(
                [&buffer](const auto& lambda) {
                    pseudo_racket_lambda(buffer, lambda);
                },
                lam
            );
        }
        buffer.append(')');
    }

    void state_script_track::regex_optimization_pass() noexcept {
        for (auto& lambda : m_lambdas) {
            if (auto* function = std::get_if<function_definition>(&lambda)) {
                function->regex_optimization_pass();
            }
        }
    }

    [[nodiscard]] bool state_script_track::equals(const state_script_track& rhs) const noexcept {
        if (m_name != rhs.m_name || m_lambdas.size() != rhs.m_lambdas.size()) {
            return false;
        }
        for (size_t i = 0; i < m_lambdas.size(); ++i) {
            if (!equals_lambda(m_lambdas[i], rhs.m_lambdas[i])) {
                return false;
            }
        }
        return true;
    }

}
