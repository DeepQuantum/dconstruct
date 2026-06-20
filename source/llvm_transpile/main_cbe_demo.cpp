#include "llvm_transpile/llvm_transpile.h"
#include "base.h"
#include "disassembly/instructions.h"
#include "disassembly/opcodes.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

#include <array>
#include <bit>
#include <filesystem>
#include <fstream>
#include <print>
#include <string>
#include <vector>

using namespace dconstruct;

int main() {
    std::array<u64, 2> symbols = {std::bit_cast<u32>(1.5f), std::bit_cast<u32>(1.5f)};

    std::array<Instruction, 2> code = {{
        // {Opcode::LoadStaticFloatImm, 0, 0, 0, {}},
        // {Opcode::LoadStaticFloatImm, 1, 1, 0, {}},
        // {Opcode::FAdd, 2, 0, 1, {}},
        {Opcode::StoreI8, 1, 2, 3, {}},
        {Opcode::Return, 1, 0, 0, {}},
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

    std::string original_ir;
    {
        llvm::raw_string_ostream os(original_ir);
        module.print(os, nullptr);
    }

    const std::string optimized_and_c = llvm_transpile::emit_c_from_module(module);

    std::string output;
    output += "/* ===== Original LLVM IR ===== */\n";
    output += original_ir;
    output += "\n";
    output += optimized_and_c;

    const std::filesystem::path out_path = std::filesystem::path(__FILE__).parent_path() / "output.c";
    std::ofstream out_file(out_path);
    out_file << output;
    out_file.close();

    std::println("wrote transpile output to {}", out_path.string());

    return 0;
}
