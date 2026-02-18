#include "compilation/tokens.h"

namespace dconstruct::compilation {


[[nodiscard]] bool token::operator==(const token &rhs) const {
    return m_type == rhs.m_type && m_lexeme == rhs.m_lexeme && m_literal == rhs.m_literal && m_line == rhs.m_line;
}

}
