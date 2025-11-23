#include "ast/type.h"

#include <unordered_map>

namespace dconstruct::compiler {
    struct environment {
        explicit environment() noexcept : m_enclosing(nullptr) {};

        explicit environment(std::unique_ptr<environment>&& enclosing) noexcept : m_enclosing(std::move(enclosing)) {};

        std::unordered_map<std::string, ast::typed_value> m_values;

        void define(const std::string& name, ast::typed_value value);

        bool assign(const std::string& name, ast::typed_value value);

        [[nodiscard]] const ast::typed_value* lookup(const std::string& name) const;

        std::unique_ptr<environment> m_enclosing;
    };

}