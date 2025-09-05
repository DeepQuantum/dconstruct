#include "compilation/environment.h"


namespace dconstruct::compiler {

void environment::define(const std::string& name, ast::typed_value value) {
    m_values[name] = value;
}

b8 environment::assign(const std::string& name, ast::typed_value value) {
    if (m_values.contains(name)) {
        m_values[name] = std::move(value);
        return true;
    }
    if (m_enclosing != nullptr) {
        m_enclosing->assign(name, std::move(value));
    }
    return false;
}

[[nodiscard]] opt_ref<const ast::typed_value> environment::lookup(const std::string& name) const {
    if (auto it = m_values.find(name); it != m_values.end()) {
        return it->second;
    }
    if (m_enclosing != nullptr) {
        return m_enclosing->lookup(name);
    }
    return std::nullopt;
}

}