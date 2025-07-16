#pragma once

#include "base.h"
#include "instructions.h"
#include "control_flow_graph.h"
#include "expressions.h"

namespace dconstruct::dcompiler {

struct decompiled_function {};

class Decompiler { 

public:

    Decompiler() = delete;

    explicit Decompiler(const FunctionDisassembly *func) {
        m_functions.push_back(func);
    };

    explicit Decompiler(const std::vector<const FunctionDisassembly*> &funcs) {
        m_functions = funcs;
    };

    std::vector<decompiled_function> decompile() noexcept;

private:
    std::vector<const FunctionDisassembly*> m_functions{};

    void parse_basic_block(const control_flow_node&, expression_frame &) noexcept;
};

}