#include "ast/unary_expressions/increment_expression.h"


namespace dconstruct::ast {

void increment_expression::pseudo_c(std::ostream& os) const {
    os << *m_rhs << "++";
}

void increment_expression::pseudo_py(std::ostream& os) const {
    os << *m_rhs << "++";
}

void increment_expression::pseudo_racket(std::ostream& os) const {
    os << "(++ " << *m_rhs << ')';
}

[[nodiscard]] expr_uptr increment_expression::simplify() const {
    return std::make_unique<ast::increment_expression>(m_operator, m_rhs->simplify());
}

[[nodiscard]] semantic_check_res increment_expression::compute_type_checked(compilation::scope& env) const noexcept {  
    const semantic_check_res rhs_type = m_rhs->get_type_checked(env);

    if (!rhs_type) {
        return rhs_type;
    }

    const std::optional<std::string> invalid_increment = std::visit([](auto&& rhs_type) -> std::optional<std::string> {
        using T = std::decay_t<decltype(rhs_type)>;
        
        if constexpr (std::is_same_v<T, primitive_kind>) {
            if constexpr (is_integral(rhs_type)) {
                return std::nullopt;
            } else {
                return "cannot increment expression with non-integral type " + type_to_declaration_string(rhs_type);
            }
        } else {
            return "cannot increment expression with non-integral type " + type_to_declaration_string(rhs_type);
        }
    }, *rhs_type);


    if (!invalid_increment) {
        return *rhs_type;
    }

    return std::unexpected{semantic_check_error{*invalid_increment, this}};
}


}