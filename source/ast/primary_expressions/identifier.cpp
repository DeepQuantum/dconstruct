#include "ast/primary_expressions/identifier.h"
#include "ast/assign.h"


namespace dconstruct::ast {

    
void identifier::pseudo_c(std::ostream& os) const {
    os << m_name.m_lexeme;
}

void identifier::pseudo_py(std::ostream& os) const {
    os << m_name.m_lexeme;
}

void identifier::pseudo_racket(std::ostream& os) const {
    os << m_name.m_lexeme;
}

[[nodiscard]] bool identifier::equals(const expression &rhs) const noexcept {
    const identifier* rhs_id = dynamic_cast<const identifier*>(&rhs);
    if (rhs_id == nullptr) {
        return false;
    }
    return m_name == rhs_id->m_name;
}

[[nodiscard]] expr_uptr identifier::simplify() const {
    return std::make_unique<identifier>(*this);
}

[[nodiscard]] expr_uptr identifier::clone() const {
    auto expr = std::make_unique<identifier>(m_name);
    if (!is_unknown(m_type)) expr->set_type(m_type);
    return expr;
}

[[nodiscard]] full_type identifier::compute_type(const type_environment& env) const {
    if (auto opt = env.lookup(m_name.m_lexeme))
        return *opt;
    return std::monostate();
}

[[nodiscard]] u16 identifier::complexity() const noexcept {
    return 1;
}

[[nodiscard]] std::unique_ptr<identifier> identifier::copy() const noexcept {
    return std::unique_ptr<ast::identifier>{ static_cast<ast::identifier*>(this->clone().release()) };
}

OPTIMIZATION_ACTION expression::check_optimization(std::unique_ptr<ast::expression>* expr, optimization_pass_context& optimization_ctx) {
    const auto pass_action = expr->get()->decomp_optimization_pass(optimization_ctx);
    switch (pass_action) {
        case OPTIMIZATION_ACTION::VAR_READ: {
            optimization_ctx.m_variables.lookup(static_cast<identifier&>(**expr).m_name.m_lexeme)->m_reads.push_back(expr);
            break;
        } 
        case OPTIMIZATION_ACTION::VAR_WRITE: {
            auto* assign = static_cast<assign_expr*>(expr->get());
            assert(dynamic_cast<identifier*>(assign->m_lhs.get()));
            optimization_ctx.m_variables.lookup(static_cast<identifier&>(*assign->m_lhs).m_name.m_lexeme)->m_writes.push_back(expr);
            break;
        }
        case OPTIMIZATION_ACTION::BEGIN_FOREACH: {
            return OPTIMIZATION_ACTION::BEGIN_FOREACH;   
        }
        case OPTIMIZATION_ACTION::END_FOREACH: {
            return OPTIMIZATION_ACTION::END_FOREACH;
        }
        default: {
            break;
        }
    }
    return OPTIMIZATION_ACTION::NONE;
}

OPTIMIZATION_ACTION identifier::decomp_optimization_pass(optimization_pass_context& optimization_ctx) noexcept {
    if (!m_name.m_lexeme.starts_with("var")) {
        return OPTIMIZATION_ACTION::NONE;
    }
    auto* ctx = optimization_ctx.m_variables.lookup(m_name.m_lexeme); 
    if (!ctx) {
        return OPTIMIZATION_ACTION::NONE;
    }
    return OPTIMIZATION_ACTION::VAR_READ;
}

}