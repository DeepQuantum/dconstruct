#include "ast/type.h"
#include "ast/function_definition.h"

namespace dconstruct::ast {

void function_definition::pseudo_c(std::ostream& os) const {
    if (const std::string* name = std::get_if<std::string>(&m_name)) {
        os << type_to_declaration_string(*m_type.m_return) << " ";

        const bool func_name_as_pascal = os.iword(get_flag_index()) & static_cast<i32>(LANGUAGE_FLAGS::FUNCTION_NAMES_PASCAL);
        if (func_name_as_pascal) {
            const auto str_res = try_convert_pascal_case(*name);
            os << str_res.value_or(*name);
        } else {
            os << *name;
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
    } else {
        os << "lambda ";
    }
    os << m_body;
}

void function_definition::pseudo_py(std::ostream& os) const {
    if (const std::string* name = std::get_if<std::string>(&m_name)) {
        os << "def " << *name << "(";
        bool first = true;
        for (const auto& param : m_parameters) {
            if (!first) {
                os << ", ";
            }
            first = false;
            param.pseudo_py(os);
        }
        os << "):";
        os << m_body;
    } else {
        os << "lambda " << m_body;
    }
}

void function_definition::pseudo_racket(std::ostream& os) const {
    if (const std::string* name = std::get_if<std::string>(&m_name)) {
        os << "(define (" << *name;
        for (const auto& param : m_parameters) {
            os << " ";
            param.pseudo_racket(os);
        }
        os << ") " << m_body << ")";
    } else {
        os << "(lambda " << m_body << ")";
    }
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
