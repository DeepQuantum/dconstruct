#include "ast/unary_expressions/dereference_expression.h"


namespace dconstruct::ast {

    [[nodiscard]] expr_uptr dereference_expr::simplify() const {
        return std::make_unique<ast::dereference_expr>(m_operator, m_rhs->simplify());
    }

    [[nodiscard]] full_type dereference_expr::compute_type_unchecked(const type_environment& env) const noexcept {
        const full_type rhs_type = m_rhs->compute_type_unchecked(env);
        if (std::holds_alternative<ast::ptr_type>(rhs_type)) {
            return *std::get<ast::ptr_type>(rhs_type).m_pointedAt;
        }
        return ast::full_type{std::monostate()};
    }

    [[nodiscard]] semantic_check_res dereference_expr::compute_type_checked(type_environment& env) const noexcept {
        const semantic_check_res rhs_type = m_rhs->get_type_checked(env);
        if (!rhs_type) {
            return rhs_type;
        }

        const std::optional<std::string> invalid_dereference = std::visit([](auto&& rhs_type) -> std::optional<std::string> {
            using T = std::decay_t<decltype(rhs_type)>;

            if constexpr (std::is_same_v<T, ptr_type>) {
                if (is_unknown(*rhs_type.m_pointedAt)) {
                    return "cannot dereference void pointer";
                } else{ 
                    return std::nullopt;
                }
            } else {
                return "cannot dereference non-pointer type " + type_to_declaration_string(rhs_type);
            }
        }, *rhs_type);

        if (!invalid_dereference) {
            return *std::get<ptr_type>(*rhs_type).m_pointedAt;
        }

        return std::unexpected{semantic_check_error{*invalid_dereference, this}};
    }
}