#include "ast/state_script/state_script_lambda.h"

namespace dconstruct::ast {

[[nodiscard]] bool operator==(const state_script_lambda& lhs, const state_script_lambda& rhs) noexcept {
    return lhs.equals(rhs);
}

void state_script_lambda::pseudo_c(std::ostream& os) const {
    os << "lambda" << m_body;
}

void state_script_lambda::pseudo_py(std::ostream& os) const {
    os << m_body;
}

void state_script_lambda::pseudo_racket(std::ostream& os) const {
    os << m_body;
}

[[nodiscard]] bool state_script_lambda::equals(const state_script_lambda& rhs) const noexcept {
    return m_body.equals(rhs.m_body);
}

}
