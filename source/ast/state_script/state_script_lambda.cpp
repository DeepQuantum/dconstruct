#include "ast/state_script/state_script_lambda.h"

namespace dconstruct::ast {

void state_script_lambda::pseudo_c(std::ostream& os) const {
    os << "lambda {\n";
    os << indent_more;
    if (m_body) {
        for (const auto& stmnt : m_body->m_statements) {
            os << indent << *stmnt << "\n";
        }
    }
    os << indent_less;
    os << indent << "}";
}

void state_script_lambda::pseudo_py(std::ostream& os) const {
    if (m_body) {
        os << *m_body;
    } else {
        os << "pass";
    }
}

void state_script_lambda::pseudo_racket(std::ostream& os) const {
    if (m_body) {
        os << *m_body;
    } else {
        os << "(begin)";
    }
}

[[nodiscard]] bool state_script_lambda::equals(const state_script_lambda& rhs) const noexcept {
    if (m_body == nullptr && rhs.m_body == nullptr) {
        return true;
    }
    if (m_body == nullptr || rhs.m_body == nullptr) {
        return false;
    }
    return m_body->equals(*rhs.m_body);
}

}
