#include "ast/type.h"
#include "ast/function_definition.h"

namespace dconstruct::ast {

void function_definition::pseudo_c(std::ostream& os) const {
    if (!std::holds_alternative<state_script_function_id>(m_name)) {
        os << type_to_declaration_string(*m_type.m_return) << " ";

        const bool func_name_as_pascal = os.iword(get_flag_index()) & static_cast<i32>(LANGUAGE_FLAGS::FUNCTION_NAMES_PASCAL);
        const std::string orig = std::get<std::string>(m_name);
        if (func_name_as_pascal) {
            const auto str_res = try_convert_pascal_case(orig);
            os << str_res.value_or(orig);
        } else {
            os << orig;
        }
        if (func_name_as_pascal) {
            os << remove_id_pascal_case;
        }
        os << "(";
        bool first = true;
        for (const auto& param : m_parameters) {
            if (!first) {
                os << ", ";
            }
            first = false;
            os << param;
        } 
        os << ") ";
    }
    os << m_body;
}

void function_definition::pseudo_py(std::ostream& os) const {

}

void function_definition::pseudo_racket(std::ostream& os) const {
}


[[nodiscard]] std::vector<semantic_check_error> function_definition::check_semantics(compilation::scope& scope) const noexcept {
    scope.m_returnType = m_type.m_return.get();
    for (const parameter& param : m_parameters) {
        scope.define(param.m_name, param.m_type);
    }
    return m_body.check_semantics(scope);
}

[[nodiscard]] emission_err function_definition::emit_dc(compilation::function& fn, compilation::global_state& global) const noexcept {
    for (u32 i = 0; i < m_parameters.size(); ++i) {
        const parameter& param = m_parameters[i];
        const emission_res new_var_reg = fn.get_next_unused_register();
        if (!new_var_reg) {
            return new_var_reg.error();
        }
        assert(!fn.m_varsToRegs.lookup(param.m_name));
        fn.m_varsToRegs.define(param.m_name, *new_var_reg);
        fn.emit_instruction(Opcode::Move, *new_var_reg, ARGUMENT_REGISTERS_IDX + i);
    }

    if (!std::holds_alternative<std::monostate>(*m_type.m_return)) {
        const emission_res return_reg = fn.get_next_unused_register();
        if (!return_reg) {
            return return_reg.error();
        }
        fn.m_returnRegister = *return_reg;
    }

    const emission_err body_err = m_body.emit_dc(fn, global);
    if (body_err) {
        return body_err;
    }

    for (const reg_idx return_branch_location : fn.m_returnBranchLocations) {
        fn.m_instructions[return_branch_location].set_lo_hi(fn.m_instructions.size());
    }

    if (fn.m_instructions.back().opcode != Opcode::Return) {
        fn.emit_instruction(Opcode::Return, 00);
    }
    return std::nullopt;
}

}