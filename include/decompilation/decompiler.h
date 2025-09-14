#pragma once

#include "base.h"
#include "disassembly/instructions.h"
#include "control_flow_graph.h"
#include "decompilation/expression_frame.h"
#include "binaryfile.h"
#include <set>


namespace dconstruct::dcompiler {

struct decompiled_function {
    std::string m_id;
    std::set<node_id> m_parsedNodes;
    expression_frame m_frame;
    ControlFlowGraph m_graph;

    [[nodiscard]] std::string to_string() const;
};


class Decompiler { 

public:

    Decompiler() = delete;

    explicit Decompiler(const function_disassembly *func, const BinaryFile &currentFile) : m_currentFile(currentFile) {
        m_functions.push_back(func);
    };

    explicit Decompiler(const std::vector<const function_disassembly*> &funcs, const BinaryFile &currentFile) : m_currentFile(currentFile) {
        m_functions = funcs;
    };

    [[nodiscard]] std::unordered_map<std::string, decompiled_function> decompile();

    [[nodiscard]] ast::full_type try_find_argument_type(const std::vector<function_disassembly_line>& lines, const u32 argument_register);

private:
    std::vector<const function_disassembly*> m_functions{};
    const BinaryFile& m_currentFile;

    void emit_node(const control_flow_node&, decompiled_function&);
    void parse_basic_block(const control_flow_node&, expression_frame &);
};

}