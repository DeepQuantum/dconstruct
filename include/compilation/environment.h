#include "base.h"
#include "ast/type.h"

#include <unordered_map>

namespace dconstruct::compiler {
    struct environment {
        std::unordered_map<std::string, ast::typed_value> m_values;

        void define(const std::string& name, const ast::typed_value& value);

        [[nodiscard]] std::optional<const ast::typed_value&> get(const std::string& name) const;
    };

}