#include "binaryfile.h"
#include "disassembly/disassembler.h"
#include "llvm_transpile/llvm_transpile.h"
#include "base.h"
#include "sidbase.h"
#include "llvm/IR/LLVMContext.h"
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

    const std::filesystem::path repo_root = std::filesystem::path(__FILE__).parent_path().parent_path().parent_path();
    const std::filesystem::path dot_exe = repo_root / "source" / "Graphviz-13.1.0-win64" / "bin" / "dot.exe";
    const std::filesystem::path cfg_dir = repo_root / "test" / "cfg";
    std::filesystem::create_directories(cfg_dir);

    const auto dump_cfgs = [&](std::string_view variant) {
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
    };

    for (const auto* function : disassembler.get_all_functions()) {
        resstr<llvm::Function*> res = llvm_transpile::transpile_function_to_llvm(module, *function, sidbase);
        if (!res.has_value()) {
            std::println(stderr, "transpile failed: {}", res.error());
            return 1;
        }
    }

   // dump_cfgs("unopt");

    const std::string llvm_unopt = llvm_transpile::emit_llvm_ir(module);
    const std::string c_unopt = llvm_transpile::emit_c_from_module(module);

    llvm_transpile::optimize_module(module);

    //dump_cfgs("opt");

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
