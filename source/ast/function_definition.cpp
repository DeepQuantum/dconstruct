#include "ast/type.h"
#include "ast/function_definition.h"

namespace dconstruct::ast {

    void function_definition::pseudo_c(ast_serialization_buffer& buffer) const {
        if (const std::string* name = std::get_if<std::string>(&m_name)) {
            buffer.append(type_to_declaration_string(*m_type.m_return), ' ');

            const bool func_name_as_pascal = buffer.has_flag(LANGUAGE_FLAGS::FUNCTION_NAMES_PASCAL);
            if (func_name_as_pascal) {
                const auto str_res = try_convert_pascal_case(*name);
                buffer.append(str_res.value_or(*name));
            } else {
                buffer.append(*name);
            }
            if (func_name_as_pascal) {
                buffer.clear_flag(LANGUAGE_FLAGS::IDENTIFIER_PASCAL);
            }
            buffer.append('(');
            bool first = true;
            for (const auto& param : m_parameters) {
                if (!first) {
                    buffer.append(", "sv);
                }
                first = false;
                buffer.append(param);
            }
            buffer.append(") "sv);
        } else {
            buffer.append("lambda "sv);
        }
        buffer.append(m_body);
    }

    void function_definition::to_pseudo_c_colored_string(code_color_serialization_buffer& buffer) const noexcept {
        if (const std::string* name = std::get_if<std::string>(&m_name)) {
            type_to_colored_declaration_string(*m_type.m_return, buffer);
            buffer.append(AST_COLOR::BLANK, ' ');
            buffer.append(AST_COLOR::CALL, *name);
            buffer.append(AST_COLOR::PUNCTUATION, '(');
            bool first = true;
            for (const auto& param : m_parameters) {
                if (!first) {
                    buffer.append(AST_COLOR::PUNCTUATION, ", "sv);
                }
                first = false;
                buffer.append(param);
            }
            buffer.append(AST_COLOR::PUNCTUATION, ") "sv);
        } else {
            buffer.append(AST_COLOR::KEYWORD, "lambda "sv);
        }
        buffer.append(m_body);
    }

    void function_definition::pseudo_py(ast_serialization_buffer& buffer) const {
        if (const std::string* name = std::get_if<std::string>(&m_name)) {
            buffer.append("def "sv, *name, '(');
            bool first = true;
            for (const auto& param : m_parameters) {
                if (!first) {
                    buffer.append(", "sv);
                }
                first = false;
                param.pseudo_py(buffer);
            }
            buffer.append("):"sv, m_body);
        } else {
            buffer.append("lambda "sv, m_body);
        }
    }

    void function_definition::pseudo_racket(ast_serialization_buffer& buffer) const {
        if (const std::string* name = std::get_if<std::string>(&m_name)) {
            buffer.append("(define ("sv, *name);
            for (const auto& param : m_parameters) {
                buffer.append(' ');
                param.pseudo_racket(buffer);
            }
            buffer.append(") "sv, m_body, ')');
        } else {
            buffer.append("(lambda "sv, m_body, ')');
        }
    }

    void function_definition::regex_optimization_pass() noexcept {
        m_body.regex_optimization_pass();
    }

    [[nodiscard]] std::vector<semantic_check_error> function_definition::check_semantics(compilation::scope& scope) const noexcept {
        scope.m_expectedReturnType = m_type.m_return.get();
        scope.m_computedReturnType = false;
        for (const parameter& param : m_parameters) {
            scope.define(param.m_name, param.m_type);
        }
        if (const std::string* name = std::get_if<std::string>(&m_name)) {
            scope.define(*name, m_type);
        }
        std::vector<semantic_check_error> res = m_body.check_semantics(scope);
        if (const auto* p = std::get_if<primitive_type>(&*scope.m_expectedReturnType); p && p->m_type != primitive_kind::NOTHING && !scope.m_computedReturnType) {
            return {semantic_check_error{"function expects a value to be returned"}};
        }
        return res;
    }

    [[nodiscard]] program_binary_result function_definition::emit_dc(compilation::global_state& global) const noexcept {
        compilation::function fn{};
        if (const std::string* name = std::get_if<std::string>(&m_name)) {
            fn.m_name = *name;
        }

        for (u32 i = 0; i < m_parameters.size(); ++i) {
            const parameter& param = m_parameters[i];
            const emission_res new_var_reg = fn.get_next_unused_register();
            if (!new_var_reg) {
                return std::unexpected{new_var_reg.error()};
            }
            assert(!fn.m_varsToRegs.lookup(param.m_name));
            fn.m_varsToRegs.define(param.m_name, *new_var_reg);
            fn.emit_instruction(Opcode::Move, *new_var_reg, ARGUMENT_REGISTERS_IDX + i);
        }

        const emission_err body_err = m_body.emit_dc(fn, global);
        if (body_err) {
            return std::unexpected{*body_err};
        }

        if (fn.m_returnBranchLocations.empty()) {
            fn.emit_instruction(Opcode::Return, 0_r, 0_r);
        } else if (fn.m_returnBranchLocations.back() == fn.m_instructions.size() - 2) {
            assert(fn.m_instructions.back().opcode == Opcode::Branch);
            fn.m_instructions.back() = Instruction(Opcode::Return, 0_r, 0_r);
            fn.m_returnBranchLocations.pop_back();
        }

        const u16 return_location = fn.m_instructions.size() - 1;

        for (const u64 branch_location : fn.m_returnBranchLocations) {
            Instruction& branch = fn.m_instructions[branch_location + 1];
            assert(branch.opcode == Opcode::Branch);
            assert(branch.destination == compilation::function::BRANCH_PLACEHOLDER);
            branch.set_lo_hi(return_location);
        }

        assert(fn.m_instructions.back().opcode == Opcode::Return);
        return fn.to_binary_element();
    }

}
