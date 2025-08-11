#include "ast/primary_expressions/identifier.h"


namespace dconstruct::ast {

void identifier::ast(std::ostream& os) const {
    os << "identifier[" << m_idx << ", " << m_name << ']';
}

void identifier::pseudo(std::ostream& os) const {
    os << m_name;
}

[[nodiscard]] b8 identifier::equals(const expression &rhs) const noexcept {
    const identifier* rhs_id = dynamic_cast<const identifier*>(&rhs);
    if (rhs_id == nullptr) {
        return false;
    }
    return m_name == rhs_id->m_name && m_idx == rhs_id->m_idx;
}

[[nodiscard]] std::unique_ptr<expression> identifier::simplify() const {
    return std::make_unique<identifier>(*this);
}

[[nodiscard]] std::unique_ptr<expression> identifier::clone() const {
    return std::make_unique<identifier>(m_name, m_idx);
}

[[nodiscard]] std::optional<full_type> identifier::compute_type(const compiler::environment& env) const {
    if (auto opt = env.get(m_name))
        return opt->type;
    return std::nullopt;    
}

}