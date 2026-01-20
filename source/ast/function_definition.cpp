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


[[nodiscard]] std::vector<semantic_check_error> function_definition::check_semantics(compiler::scope& scope) const noexcept {
    scope.m_returnType = m_type.m_return.get();
    return m_body.check_semantics(scope);
}

}