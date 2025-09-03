#pragma once

#include "base.h"
#include "disassembly/instructions.h"
#include "control_flow_graph.h"
#include "decompilation/expression_frame.h"
#include "sidbase.h"
#include <set>


namespace dconstruct::dcompiler {

struct decompiled_function {
    std::set<node_id> m_parsedNodes;
    expression_frame m_frame;
    ControlFlowGraph m_graph;

    [[nodiscard]] std::string to_string() const;
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

    [[nodiscard]] std::vector<std::pair<std::string, decompiled_function>> decompile();

private:
    std::vector<const function_disassembly*> m_functions{};
    const SIDBase& m_sidbase;

    void emit_node(const control_flow_node&, decompiled_function&);
    void parse_basic_block(const control_flow_node&, expression_frame &);
};

}