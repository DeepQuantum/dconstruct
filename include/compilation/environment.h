#include "base.h"
#include "ast/type.h"

#include <unordered_map>

namespace dconstruct::compiler {
    std::unordered_map<std::string, ast::full_type> m_values;
}