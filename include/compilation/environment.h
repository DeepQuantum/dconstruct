#include "ast/type.h"

#include <unordered_map>

namespace dconstruct::compiler {
    template<typename T = ast::typed_value>
    struct environment {
        environment() noexcept : m_enclosing(nullptr) {};
        
        environment(const environment& rhs) = delete;

        environment& operator=(const environment& rhs) = delete;

        explicit environment(std::unique_ptr<environment>&& enclosing) noexcept : m_enclosing(std::move(enclosing)) {};

        std::unordered_map<std::string, T> m_values;

        void define(const std::string& name, T value);

        bool assign(const std::string& name, T value);

        [[nodiscard]] T* lookup(const std::string& name) const;

        std::unique_ptr<environment> m_enclosing;
    };

}