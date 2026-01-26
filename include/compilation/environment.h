#pragma once

#include "ast/type.h"
#include "sidbase.h"

#include <unordered_map>

namespace dconstruct::compiler {
    template<typename T = ast::typed_value>
    struct environment {
        environment() noexcept : m_enclosing(nullptr) {};
        
        environment(const environment& rhs) = delete;

        environment& operator=(const environment& rhs) = delete;

        environment& operator=(environment&& rhs) = default;
        environment(environment&& rhs) = default;

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

        explicit scope(std::unordered_map<std::string, ast::full_type> names_to_types) noexcept : 
            m_namesToTypes(std::move(names_to_types)){};

        explicit scope(scope* enclosing) noexcept {
            m_enclosing = enclosing;
        }

        scope() noexcept {
            m_enclosing = nullptr;
        };
        
        scope(const scope& rhs) = delete;

        scope& operator=(const scope& rhs) = delete;

        scope& operator=(scope&& rhs) = default;
        scope(scope&& rhs) = default;

        

        std::unordered_map<std::string, ast::full_type> m_namesToTypes;
        std::unordered_map<std::string, sid64_literal> m_sidAliases;
        const ast::full_type* m_returnType;
    };

}