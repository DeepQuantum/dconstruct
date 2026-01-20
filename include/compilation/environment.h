#pragma once

#include "ast/type.h"

#include <unordered_map>

namespace dconstruct::compiler {
    template<typename T = ast::typed_value>
    struct environment {
        environment() noexcept : m_enclosing(nullptr) {};
        
        environment(const environment& rhs) = delete;

        environment& operator=(const environment& rhs) = delete;

        explicit environment(environment* enclosing) noexcept : m_enclosing(enclosing) {};

        std::unordered_map<std::string, T> m_values;

        void define(const std::string& name, T value) {
            m_values[name] = value;
        }

        bool assign(const std::string& name, T value) {
            if (m_values.contains(name)) {
                m_values[name] = std::move(value);
                return true;
            }
            if (m_enclosing != nullptr) {
                m_enclosing->assign(name, std::move(value));
            }
            return false;
        }

        [[nodiscard]] const T* lookup(const std::string& name) const {
            if (auto it = m_values.find(name); it != m_values.end()) {
                return &it->second;
            }
            if (m_enclosing != nullptr) {
                return m_enclosing->lookup(name);
            }
            return nullptr;
        }

        [[nodiscard]] T* lookup(const std::string& name) {
            if (auto it = m_values.find(name); it != m_values.end()) {
                return &it->second;
            }
            if (m_enclosing != nullptr) {
                return m_enclosing->lookup(name);
            }
            return nullptr;
        }

        environment* m_enclosing;
    };

    struct scope : public environment<ast::full_type> {
        std::unordered_map<std::string, ast::full_type> n_namesToTypes;
        const ast::full_type* m_returnType;
    };

}