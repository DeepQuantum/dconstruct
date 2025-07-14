#define GVDLL

#include "decompiler.h"
#include <fstream>
#include <iostream>
#include <graphviz/gvc.h>
#include <graphviz/cgraph.h>
#include <sstream>
#include <set>
#include <mutex>
#include <chrono>


namespace dconstruct {

std::vector<DecompiledFunction> Decompiler::decompile() noexcept {
    for (const auto &func : m_functions) {
        ControlFlowGraph cfg = ControlFlowGraph(func);
        cfg.find_loops();
        cfg.write_image("C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/build/images/" + func->m_id + ".svg");
    }
    return{};
}
}