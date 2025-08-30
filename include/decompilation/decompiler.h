#pragma once

#include "base.h"
#include "disassembly/instructions.h"
#include "control_flow_graph.h"
#include "decompilation/expression_frame.h"
#include "sidbase.h"
#include "control_flow_graph.h"

namespace dconstruct::dcompiler {

struct decompiled_function {
    std::set<u32>
    expression_frame m_frame;
    ControlFlowGraph m_graph;
    std::ostream& m_text;
};

class Decompiler { 

public:

    Decompiler() = delete;

    explicit Decompiler(const function_disassembly *func, const SIDBase& sidbase) : m_sidbase(sidbase) {
        m_functions.push_back(func);  
    };

    explicit Decompiler(const std::vector<const function_disassembly*> &funcs, const SIDBase& sidbase) : m_sidbase(sidbase) {
        m_functions = funcs;
    };

    std::vector<decompiled_function> decompile(std::ostream& out);

private:
    std::vector<const function_disassembly*> m_functions{};
    const SIDBase& m_sidbase;

    void emit_node(const control_flow_node&, decompiled_function&);
    void parse_basic_block(const control_flow_node&, expression_frame &);
};

}