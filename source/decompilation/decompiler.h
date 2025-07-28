#pragma once

#include "base.h"
#include "instructions.h"
#include "control_flow_graph.h"
#include "expression_frame.h"

namespace dconstruct::dcompiler {

struct decompiled_function {
    expression_frame m_frame;
};

class Decompiler { 

public:

    Decompiler() = delete;

    explicit Decompiler(const function_disassembly *func) {
        m_functions.push_back(func);
    };

    explicit Decompiler(const std::vector<const function_disassembly*> &funcs) {
        m_functions = funcs;
    };

    std::vector<decompiled_function> decompile();

private:
    std::vector<const function_disassembly*> m_functions{};

    void parse_basic_block(const control_flow_node&, expression_frame &);
};

}