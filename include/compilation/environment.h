#include "base.h"
#include "ast/type.h"

#include <unordered_map>

namespace dconstruct::compiler {
    struct environment {
        std::unordered_map<std::string, ast::full_type> m_values;

        void define(const std::string& name, const ast::full_type& value);

        [[nodiscard]] std::optional<const ast::full_type&> get(const std::string& name) const;
    };

}