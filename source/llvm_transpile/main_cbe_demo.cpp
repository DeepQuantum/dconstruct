#include "binaryfile.h"
#include "disassembly/disassembler.h"
#include "llvm_transpile/llvm_transpile.h"
#include "base.h"
#include "sidbase.h"
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

struct generated_outputs {
    std::string llvm_unopt;
    std::string llvm_opt;
    std::string llvm_lowered;
    std::string llvm_deduped;
    std::string dcvm_asm;
    std::vector<char> dcvm_bytecode;
};

static generated_outputs generate_outputs(llvm::Module& module) {
    generated_outputs outputs;

    outputs.llvm_unopt = llvm_transpile::emit_llvm_ir(module);
    llvm_transpile::optimize_module(module);
    outputs.llvm_opt = llvm_transpile::emit_llvm_ir(module);

    llvm_transpile::lower_calls_to_lookup(module);
    outputs.llvm_lowered = llvm_transpile::emit_llvm_ir(module);

    llvm_transpile::optimize_module(module);
    outputs.llvm_deduped = llvm_transpile::emit_llvm_ir(module);

    outputs.dcvm_asm = llvm_transpile::emit_dcvm_asm(module);
    outputs.dcvm_bytecode = llvm_transpile::emit_dcvm_bytecode(module);

    return outputs;
}

static void print_outputs(const generated_outputs& outputs) {
    const std::filesystem::path out_dir = std::filesystem::path(__FILE__).parent_path();

    const std::array<std::pair<const char*, const std::string*>, 5> text_outputs = {{
        {"output_unopt.ll", &outputs.llvm_unopt},
        {"output_opt.ll", &outputs.llvm_opt},
        {"output_lowered.ll", &outputs.llvm_lowered},
        {"output_deduped.ll", &outputs.llvm_deduped},
        {"output.dcvm.s", &outputs.dcvm_asm},
    }};

    for (const auto& [filename, content] : text_outputs) {
        const std::filesystem::path out_path = out_dir / filename;
        std::ofstream out_file(out_path);
        out_file << *content;
        out_file.close();
        std::println("wrote {}", out_path.string());
    }

    const std::filesystem::path bin_path = out_dir / "output.dcvm.bin";
    std::ofstream bin_file(bin_path, std::ios::binary);
    bin_file.write(outputs.dcvm_bytecode.data(), static_cast<std::streamsize>(outputs.dcvm_bytecode.size()));
    bin_file.close();
    std::println("wrote {} ({} bytes)", bin_path.string(), outputs.dcvm_bytecode.size());
}

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

    for (const auto* function : disassembler.get_all_functions()) {
        resstr<llvm::Function*> res = llvm_transpile::transpile_function_to_llvm(module, *function, sidbase);
        if (!res.has_value()) {
            std::println(stderr, "transpile failed: {}", res.error());
            return 1;
        }
    }

    const generated_outputs outputs = generate_outputs(module);
    print_outputs(outputs);

    return 0;
}
