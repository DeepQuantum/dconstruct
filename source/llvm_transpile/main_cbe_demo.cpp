#include "DCScript.h"
#include "binaryfile.h"
#include "disassembly/disassembler.h"
#include "disassembly/instructions.h"
#include "llvm_transpile/llvm_transpile.h"
#include "base.h"
#include "sidbase.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/IntrinsicsDCVM.h"
#include "llvm/IR/Module.h"
#include "llvm/Analysis/CFGPrinter.h"
#include "llvm/Support/GraphWriter.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include <array>
#include <cstdlib>
#include <format>
#include <filesystem>
#include <fstream>
#include <memory>
#include <print>
#include <string>
#include <utility>
#include <vector>

using namespace dconstruct;

[[maybe_unused]] static void dump_cfgs(llvm::Module& module, std::string_view variant) {
    const std::filesystem::path repo_root = std::filesystem::path(__FILE__).parent_path().parent_path().parent_path();
    const std::filesystem::path dot_exe = repo_root / "source" / "Graphviz-13.1.0-win64" / "bin" / "dot.exe";
    const std::filesystem::path cfg_dir = repo_root / "test" / "cfg";
    std::filesystem::create_directories(cfg_dir);

    for (llvm::Function& llvm_func : module) {
        if (llvm_func.isDeclaration()) {
            continue;
        }
        std::string safe_name = llvm_func.getName().str();
        std::ranges::replace_if(safe_name, [](char c) { return std::string_view("/\\:*?\"<>|").contains(c); }, '_');
        const std::filesystem::path dot_path = cfg_dir / std::format("cfg_{}_{}.dot", safe_name, variant);
        {
            std::error_code ec;
            llvm::raw_fd_ostream cfg_out(dot_path.string(), ec);
            if (ec) {
                std::println(stderr, "failed to open {}: {}", dot_path.string(), ec.message());
                continue;
            }
            llvm::DOTFuncInfo cfg_info(&llvm_func);
            llvm::WriteGraph(cfg_out, &cfg_info);
        }
        std::println("wrote {}", dot_path.string());

        const std::filesystem::path svg_path = cfg_dir / std::format("cfg_{}_{}.svg", safe_name, variant);
        const std::string render_cmd = std::format("\"\"{}\" -Tsvg \"{}\" -o \"{}\"\"", dot_exe.string(), dot_path.string(), svg_path.string());
        if (std::system(render_cmd.c_str()) != 0) {
            std::println(stderr, "dot failed to render {}", svg_path.string());
        } else {
            std::println("wrote {}", svg_path.string());
        }
    }
}

int main() {
    auto sidbase_result = SIDBase::from_binary("sidbase.bin");
    if (!sidbase_result.has_value()) {
        std::println(stderr, "failed to load sidbase.bin: {}", sidbase_result.error());
        return 1;
    }
    const SIDBase& sidbase = sidbase_result.value();

    std::vector<std::filesystem::path> paths = {"wave-manager-funcs.bin", "anim-gas-mask-impl.bin"};

    llvm_transpile::llvm_transpiler tp(sidbase);
    std::vector<std::unique_ptr<BinaryFile>> binfiles;
    std::vector<std::unique_ptr<Disassembler>> disassemblers;
    binfiles.reserve(paths.size());
    disassemblers.reserve(paths.size());

    for (const auto& path: paths) {
        resstr<BinaryFile> binfile = BinaryFile::from_path(path);
        if (!binfile) {
            std::println(stderr, "failed to load binary file {}: {}", path.filename().string(), binfile.error());
            return 1;
        }

        binfiles.push_back(std::make_unique<BinaryFile>(std::move(*binfile)));
        disassemblers.push_back(std::make_unique<Disassembler>(binfiles.back().get(), &sidbase));
        disassemblers.back()->disassemble();

        tp.add_module(path.filename().string(), disassemblers.back()->get_all_functions());
    }

    const std::filesystem::path out_dir = std::filesystem::path(__FILE__).parent_path();
    const std::vector<llvm_transpile::generated_outputs> outputs = tp.run();

    for (const llvm_transpile::generated_outputs& output : outputs) {
        if (errmsg error = llvm_transpile::llvm_transpiler::write_outputs(output, out_dir)) {
            std::println(stderr, "failed to write outputs for {}: {}", output.m_moduleName, *error);
            return 1;
        }
        std::println("generated {} ({} bytes)", output.m_moduleName, output.m_dcvmBytecode.size());
    }

    return 0;
}
