#include "llvm_transpile/llvm_transpile.h"
#include "base.h"
#include "disassembly/instructions.h"
#include "disassembly/opcodes.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#include <array>
#include <print>
#include <string>
#include <vector>

using namespace dconstruct;

int main() {
    alignas(8) std::array<u32, 2> symbols = {10u, 32u};

    std::array<Instruction, 4> code = {{
        {Opcode::LoadStaticU32Imm, 0, 0, 0, {}},
        {Opcode::LoadStaticU32Imm, 1, 4, 0, {}},
        {Opcode::IAdd, 2, 0, 1, {}},
        {Opcode::Return, 2, 0, 0, {}},
    }};

    std::vector<function_disassembly_line> lines;
    lines.reserve(code.size());
    for (u64 i = 0; i < code.size(); ++i) {
        lines.emplace_back(i, code.data(), true);
    }

    StackFrame frame(location(symbols.data()));

    function_disassembly func(std::move(lines), std::move(frame), std::string("demo_add"), false);

    llvm::LLVMContext ctx;
    llvm::Module module("cbe_demo", ctx);

    llvm_transpile::transpile_function_to_llvm(module, func);

    const std::string c_code = llvm_transpile::emit_c_from_module(module);

    std::println("{}", c_code);

    return 0;
}
