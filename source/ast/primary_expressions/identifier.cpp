#include "ast/primary_expressions/identifier.h"


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

void identifier::decomp_optimization_pass(second_pass_env& env) noexcept {
    if (!m_name.m_lexeme.starts_with("var")) return;
    second_pass_context* ctx = env->lookup(m_name.m_lexeme);
    assert(ctx);
    ctx->m_firstUsageSite = this;
    ctx->m_uses++;
} 

}