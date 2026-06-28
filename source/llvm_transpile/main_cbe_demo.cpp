#include "DCScript.h"
#include "binaryfile.h"
#include "compilation/compiler_funcs.h"
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
#include <expected>
#include <format>
#include <filesystem>
#include <fstream>
#include <memory>
#include <print>
#include <string>
#include <utility>
#include <vector>

using namespace dconstruct;

namespace {
    const std::filesystem::path SIDBASE_PATH = "sidbase.bin";
    const std::filesystem::path GAME_ROOT = "C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II";
    const std::filesystem::path GAME_DC1_DIR = GAME_ROOT / "build" / "pc" / "main" / "bin_unpacked" / "dc1";
    const std::filesystem::path GAME_MODS_DIR = GAME_ROOT / "mods";
    const std::filesystem::path RECOMPILED_MOD_DIR = GAME_MODS_DIR / "recompiled";
    const std::filesystem::path RECOMPILED_DC1_DIR = RECOMPILED_MOD_DIR / "bin" / "dc1";
    const std::filesystem::path SOURCE_MODULES = GAME_DC1_DIR / "modules.bin";
    const std::filesystem::path STAGED_MODULES = RECOMPILED_DC1_DIR / "modules.bin";

    const std::array RECOMPILE_INPUTS = {
        GAME_DC1_DIR / "ss-rogue" / "wave-manager-funcs.bin",
        GAME_DC1_DIR / "anim-gas-mask-impl.bin",
    };

    resstr<std::filesystem::path> staged_output_path(const std::filesystem::path& source_path) {
        std::filesystem::path relative = source_path.lexically_relative(GAME_DC1_DIR);
        if (relative.empty() || *relative.begin() == "..") {
            return std::unexpected{"input file is not under GAME_DC1_DIR: " + source_path.string()};
        }
        return RECOMPILED_DC1_DIR / relative;
    }

    errmsg prepare_recompiled_workspace() {
        std::error_code ec;
        std::filesystem::remove_all(RECOMPILED_MOD_DIR, ec);
        if (ec) {
            return "couldn't remove old recompiled staging directory " + RECOMPILED_MOD_DIR.string() + ": " + ec.message();
        }

        std::filesystem::create_directories(RECOMPILED_DC1_DIR, ec);
        if (ec) {
            return "couldn't create recompiled dc1 directory " + RECOMPILED_DC1_DIR.string() + ": " + ec.message();
        }

        if (!std::filesystem::exists(SOURCE_MODULES)) {
            return "missing source modules.bin: " + SOURCE_MODULES.string();
        }

        std::filesystem::copy_file(SOURCE_MODULES, STAGED_MODULES, std::filesystem::copy_options::overwrite_existing, ec);
        if (ec) {
            return "couldn't copy fresh modules.bin from " + SOURCE_MODULES.string() + " to " + STAGED_MODULES.string() + ": " + ec.message();
        }

        const std::filesystem::path psarc_path = compilation::repackage_psarc_path(RECOMPILED_MOD_DIR);
        std::filesystem::remove(psarc_path, ec);
        if (ec) {
            return "couldn't remove old psarc " + psarc_path.string() + ": " + ec.message();
        }

        return std::nullopt;
    }
}

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
    auto sidbase_result = SIDBase::from_binary(SIDBASE_PATH);
    if (!sidbase_result.has_value()) {
        std::println(stderr, "failed to load {}: {}", SIDBASE_PATH.string(), sidbase_result.error());
        return 1;
    }
    const SIDBase& sidbase = sidbase_result.value();

    if (errmsg error = prepare_recompiled_workspace()) {
        std::println(stderr, "failed to prepare recompiled mod workspace: {}", *error);
        return 1;
    }

    llvm_transpile::llvm_transpiler tp(sidbase);
    std::vector<std::unique_ptr<BinaryFile>> binfiles;
    std::vector<std::unique_ptr<Disassembler>> disassemblers;
    std::vector<llvm_transpile::original_binfile> original_binfiles;
    std::vector<std::filesystem::path> staged_outputs;
    binfiles.reserve(RECOMPILE_INPUTS.size());
    disassemblers.reserve(RECOMPILE_INPUTS.size());
    original_binfiles.reserve(RECOMPILE_INPUTS.size());
    staged_outputs.reserve(RECOMPILE_INPUTS.size());

    for (const auto& path: RECOMPILE_INPUTS) {
        resstr<BinaryFile> binfile = BinaryFile::from_path(path);
        if (!binfile) {
            std::println(stderr, "failed to load binary file {}: {}", path.filename().string(), binfile.error());
            return 1;
        }

        resstr<std::filesystem::path> output_path = staged_output_path(path);
        if (!output_path) {
            std::println(stderr, "failed to stage {}: {}", path.string(), output_path.error());
            return 1;
        }

        binfiles.push_back(std::make_unique<BinaryFile>(std::move(*binfile)));
        disassemblers.push_back(std::make_unique<Disassembler>(binfiles.back().get(), &sidbase));
        disassemblers.back()->disassemble();

        std::vector<const function_disassembly*> functions = disassemblers.back()->get_all_functions();
        original_binfiles.push_back({path.filename().string(), binfiles.back().get(), functions});
        staged_outputs.push_back(std::move(*output_path));
        tp.add_module(path.filename().string(), binfiles.back().get(), functions);
    }

    tp.enable_runtime_module();

    const std::vector<llvm_transpile::generated_outputs> outputs = tp.run();

    if (errmsg error = llvm_transpile::llvm_transpiler::patch_original_binfiles(outputs, original_binfiles)) {
        std::println(stderr, "failed to patch original binfiles: {}", *error);
        return 1;
    }

    std::vector<compilation::modules_patch_request> module_patches;
    module_patches.reserve(original_binfiles.size());

    for (u64 i = 0; i < original_binfiles.size(); ++i) {
        const llvm_transpile::original_binfile& original = original_binfiles[i];
        const std::filesystem::path& patched_path = staged_outputs[i];
        std::filesystem::create_directories(patched_path.parent_path());
        if (errmsg error = original.m_binfile->dump_to_file(patched_path)) {
            std::println(stderr, "failed to write patched binary {}: {}", patched_path.string(), *error);
            return 1;
        }

        const u64 patched_size = std::filesystem::file_size(patched_path);
        module_patches.push_back(compilation::modules_patch_request{patched_path, patched_size, {}});
        std::println("staged {} ({} bytes)", patched_path.string(), patched_size);
    }

    resstr<std::vector<compilation::modules_patch_summary>> modules_result = compilation::patch_modules_sizes(STAGED_MODULES, module_patches);
    if (!modules_result) {
        std::println(stderr, "failed to patch modules.bin: {}", modules_result.error());
        return 1;
    }
    for (const compilation::modules_patch_summary& summary : *modules_result) {
        std::println("modules.bin: {} size {} -> {}", summary.m_targetName, summary.m_oldSize, summary.m_newSize);
    }

    resstr<std::filesystem::path> psarc_result = compilation::repackage_psarc(RECOMPILED_MOD_DIR);
    if (!psarc_result) {
        std::println(stderr, "failed to create recompiled psarc: {}", psarc_result.error());
        return 1;
    }

    std::println("created {}", psarc_result->string());

    return 0;
}
