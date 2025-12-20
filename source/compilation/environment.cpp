#include "compilation/environment.h"


namespace dconstruct::compiler {

template<typename T>
void environment<T>::define(const std::string& name, T value) {
    m_values[name] = value;
}

template<typename T>
bool environment<T>::assign(const std::string& name, T value) {
    if (m_values.contains(name)) {
        m_values[name] = std::move(value);
        return true;
    }
    if (m_enclosing != nullptr) {
        m_enclosing->assign(name, std::move(value));
    }
    return false;
}

template<typename T>
[[nodiscard]] T* environment<T>::lookup(const std::string& name) const {
    if (auto it = m_values.find(name); it != m_values.end()) {
        return &it->second;
    }
    if (m_enclosing != nullptr) {
        return m_enclosing->lookup(name);
    }
    return std::nullopt;
}

}