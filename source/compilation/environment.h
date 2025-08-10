#include "compilation/environment.h"


namespace dconstruct::compiler {

void environment::define(const std::string& name, const ast::full_type& value) {
    m_values[name] = value;
}

[[nodiscard]] std::optional<const ast::full_type&> environment::get(const std::string& name) const {
    if (!m_values.contains(name)) {
        return std::nullopt;
    }
    return m_values.at(name);
}

}