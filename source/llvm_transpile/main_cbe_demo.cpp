#include "binaryfile.h"
#include "disassembly/disassembler.h"
#include "llvm_transpile/llvm_transpile.h"
#include "base.h"
#include "sidbase.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <array>
#include <filesystem>
#include <fstream>
#include <print>
#include <string>
#include <utility>

using namespace dconstruct;

int main() {
    auto sidbase_result = SIDBase::from_binary("sidbase.bin");
    if (!sidbase_result.has_value()) {
        std::println(stderr, "failed to load sidbase.bin: {}", sidbase_result.error());
        return 1;
    }
    const SIDBase& sidbase = sidbase_result.value();

    const std::filesystem::path filepath = "anim-gas-mask-impl.bin";
    resstr<BinaryFile> binfile = BinaryFile::from_path(filepath);
    if (!binfile) {
        std::println(stderr, "failed to load binary file {}: {}", filepath.filename().string(), binfile.error());
        return 1;
    }

    llvm::LLVMContext ctx;
    llvm::Module module(filepath.string(), ctx);

    Disassembler disassembler(&*binfile, &sidbase);
    disassembler.disassemble();

    const auto& funcs = disassembler.get_all_functions();



    for (const auto* function : disassembler.get_all_functions()) {
        resstr<llvm::Function*> res = llvm_transpile::transpile_function_to_llvm(module, *function, sidbase);
        if (!res.has_value()) {
            std::println(stderr, "transpile failed: {}", res.error());
            return 1;
        }
    }


    const std::string llvm_unopt = llvm_transpile::emit_llvm_ir(module);
    const std::string c_unopt = llvm_transpile::emit_c_from_module(module);

    llvm_transpile::optimize_module(module);

    const std::string llvm_opt = llvm_transpile::emit_llvm_ir(module);
    const std::string c_opt = llvm_transpile::emit_c_from_module(module);

    const std::filesystem::path out_dir = std::filesystem::path(__FILE__).parent_path();

    const std::array<std::pair<const char*, const std::string*>, 4> outputs = {{
        {"output_unopt.ll", &llvm_unopt},
        {"output_opt.ll", &llvm_opt},
        {"output_unopt.c", &c_unopt},
        {"output_opt.c", &c_opt},
    }};

    for (const auto& [filename, content] : outputs) {
        const std::filesystem::path out_path = out_dir / filename;
        std::ofstream out_file(out_path);
        out_file << *content;
        out_file.close();
        std::println("wrote {}", out_path.string());
    }

    return 0;
}
