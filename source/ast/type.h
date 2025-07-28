#pragma once

#include "base.h"
#include "tokens.h"

namespace dconstruct::compiler {
struct type {
    type(const compiler::token &type) : m_type(type) {};
private:
    compiler::token m_type;
};
}
