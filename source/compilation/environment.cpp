#include "compilation/environment.h"


namespace dconstruct::compiler {

void environment::define(const std::string& name, const ast::typed_value& value) {
    m_values[name] = value;
}

[[nodiscard]] std::optional<std::reference_wrapper<const ast::typed_value>> environment::lookup(const std::string& name) const {
    if (auto it = m_values.find(name); it != m_values.end()) {
        return it->second;
    }
    return std::nullopt;
}

}