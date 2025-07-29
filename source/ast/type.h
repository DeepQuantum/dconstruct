#pragma once

#include "base.h"
#include "tokens.h"


namespace dconstruct::ast {

    enum primitive : u8 {
        E_UNKNOWN,
        E_INT,
        E_FLOAT,
        E_STRING,
        E_PTR,
        E_HASH,
    };


    struct type {
        type(const compiler::token& type) : m_value(type.m_lexeme) {};
        type(const primitive &type) : m_value(type) {};

        std::variant<std::string, primitive> m_value;

        std::string to_string() const noexcept {
            if (m_value.index() == 0) {
                return std::get<std::string>(m_value);
            }
            else {
                switch (std::get<primitive>(m_value)) {
                    case E_UNKNOWN: return "var";
                    case E_INT: return "i32";
                    case E_FLOAT: return "f32";
                    case E_STRING: return "string";
                    case E_PTR: return "p64";
                    case E_HASH: return "sid64";
                    default: return "var";
                }
            }
        }
    };
}
