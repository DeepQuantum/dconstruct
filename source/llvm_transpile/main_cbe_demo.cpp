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

    llvm_transpile::optimize_module(module);

    //dump_cfgs("opt");

    const std::string llvm_opt = llvm_transpile::emit_llvm_ir(module);

    llvm_transpile::lower_calls_to_lookup(module);
    const std::string llvm_lowered = llvm_transpile::emit_llvm_ir(module);

    llvm_transpile::optimize_module(module);
    const std::string llvm_deduped = llvm_transpile::emit_llvm_ir(module);

    llvm::Module dcvm_smoke("dcvm_smoke", ctx);
    {
        llvm::IRBuilder<> b(ctx);
        llvm::Type* i64t = b.getInt64Ty();
        llvm::Type* f32t = b.getFloatTy();
        llvm::FunctionType* binop_ty = llvm::FunctionType::get(i64t, {i64t, i64t}, false);
        llvm::FunctionType* unop_ty = llvm::FunctionType::get(i64t, {i64t}, false);
        llvm::FunctionType* fcmp_ty = llvm::FunctionType::get(i64t, {f32t, f32t}, false);

        const auto emit_binop = [&](const char* name, auto&& make) {
            llvm::Function* f = llvm::Function::Create(binop_ty, llvm::GlobalValue::ExternalLinkage, name, &dcvm_smoke);
            b.SetInsertPoint(llvm::BasicBlock::Create(ctx, "entry", f));
            b.CreateRet(make(f->getArg(0), f->getArg(1)));
        };
        const auto emit_unop = [&](const char* name, auto&& make) {
            llvm::Function* f = llvm::Function::Create(unop_ty, llvm::GlobalValue::ExternalLinkage, name, &dcvm_smoke);
            b.SetInsertPoint(llvm::BasicBlock::Create(ctx, "entry", f));
            b.CreateRet(make(f->getArg(0)));
        };
        const auto emit_icmp = [&](const char* name, auto&& make) {
            llvm::Function* f = llvm::Function::Create(binop_ty, llvm::GlobalValue::ExternalLinkage, name, &dcvm_smoke);
            b.SetInsertPoint(llvm::BasicBlock::Create(ctx, "entry", f));
            b.CreateRet(b.CreateZExt(make(f->getArg(0), f->getArg(1)), i64t));
        };
        const auto emit_fcmp = [&](const char* name, auto&& make) {
            llvm::Function* f = llvm::Function::Create(fcmp_ty, llvm::GlobalValue::ExternalLinkage, name, &dcvm_smoke);
            b.SetInsertPoint(llvm::BasicBlock::Create(ctx, "entry", f));
            b.CreateRet(b.CreateZExt(make(f->getArg(0), f->getArg(1)), i64t));
        };

        emit_binop("add",  [&](llvm::Value* x, llvm::Value* y) { return b.CreateAdd(x, y); });
        emit_binop("sub",  [&](llvm::Value* x, llvm::Value* y) { return b.CreateSub(x, y); });
        emit_binop("mul",  [&](llvm::Value* x, llvm::Value* y) { return b.CreateMul(x, y); });
        emit_binop("sdiv", [&](llvm::Value* x, llvm::Value* y) { return b.CreateSDiv(x, y); });
        emit_binop("band", [&](llvm::Value* x, llvm::Value* y) { return b.CreateAnd(x, y); });
        emit_binop("bor",  [&](llvm::Value* x, llvm::Value* y) { return b.CreateOr(x, y); });
        emit_binop("bxor", [&](llvm::Value* x, llvm::Value* y) { return b.CreateXor(x, y); });
        emit_binop("bnor", [&](llvm::Value* x, llvm::Value* y) { return b.CreateNot(b.CreateOr(x, y)); });
        emit_unop("bnot",  [&](llvm::Value* x) { return b.CreateNot(x); });
        emit_unop("ineg",  [&](llvm::Value* x) { return b.CreateNeg(x); });

        emit_icmp("ieq", [&](llvm::Value* x, llvm::Value* y) { return b.CreateICmpEQ(x, y); });
        emit_icmp("ine", [&](llvm::Value* x, llvm::Value* y) { return b.CreateICmpNE(x, y); });
        emit_icmp("igt", [&](llvm::Value* x, llvm::Value* y) { return b.CreateICmpSGT(x, y); });
        emit_icmp("ige", [&](llvm::Value* x, llvm::Value* y) { return b.CreateICmpSGE(x, y); });
        emit_icmp("ilt", [&](llvm::Value* x, llvm::Value* y) { return b.CreateICmpSLT(x, y); });
        emit_icmp("ile", [&](llvm::Value* x, llvm::Value* y) { return b.CreateICmpSLE(x, y); });

        emit_fcmp("feq", [&](llvm::Value* x, llvm::Value* y) { return b.CreateFCmpUEQ(x, y); });
        emit_fcmp("fne", [&](llvm::Value* x, llvm::Value* y) { return b.CreateFCmpUNE(x, y); });
        emit_fcmp("fgt", [&](llvm::Value* x, llvm::Value* y) { return b.CreateFCmpUGT(x, y); });
        emit_fcmp("fge", [&](llvm::Value* x, llvm::Value* y) { return b.CreateFCmpUGE(x, y); });
        emit_fcmp("flt", [&](llvm::Value* x, llvm::Value* y) { return b.CreateFCmpULT(x, y); });
        emit_fcmp("fle", [&](llvm::Value* x, llvm::Value* y) { return b.CreateFCmpULE(x, y); });

        llvm::FunctionType* iconst_ty = llvm::FunctionType::get(i64t, {}, false);
        llvm::FunctionType* fconst_ty = llvm::FunctionType::get(f32t, {}, false);
        const auto emit_const = [&](const char* name, llvm::FunctionType* ty, llvm::Constant* c) {
            llvm::Function* f = llvm::Function::Create(ty, llvm::GlobalValue::ExternalLinkage, name, &dcvm_smoke);
            b.SetInsertPoint(llvm::BasicBlock::Create(ctx, "entry", f));
            b.CreateRet(c);
        };
        emit_const("small_const", iconst_ty, b.getInt64(42));
        emit_const("big_const",   iconst_ty, b.getInt64(0x123456789ABCDEF0ULL));
        emit_const("dup_const",   iconst_ty, b.getInt64(0x123456789ABCDEF0ULL));
        emit_const("float_const", fconst_ty, llvm::ConstantFP::get(f32t, 3.14159));

        llvm::FunctionType* fbinop_ty = llvm::FunctionType::get(f32t, {f32t, f32t}, false);
        llvm::FunctionType* funop_ty = llvm::FunctionType::get(f32t, {f32t}, false);
        const auto emit_fbinop = [&](const char* name, auto&& make) {
            llvm::Function* f = llvm::Function::Create(fbinop_ty, llvm::GlobalValue::ExternalLinkage, name, &dcvm_smoke);
            b.SetInsertPoint(llvm::BasicBlock::Create(ctx, "entry", f));
            b.CreateRet(make(f->getArg(0), f->getArg(1)));
        };
        const auto emit_funop = [&](const char* name, auto&& make) {
            llvm::Function* f = llvm::Function::Create(funop_ty, llvm::GlobalValue::ExternalLinkage, name, &dcvm_smoke);
            b.SetInsertPoint(llvm::BasicBlock::Create(ctx, "entry", f));
            b.CreateRet(make(f->getArg(0)));
        };

        emit_binop("imod",  [&](llvm::Value* x, llvm::Value* y) { return b.CreateSRem(x, y); });
        emit_fbinop("fmod", [&](llvm::Value* x, llvm::Value* y) { return b.CreateFRem(x, y); });
        emit_unop("iabs",   [&](llvm::Value* x) { return b.CreateBinaryIntrinsic(llvm::Intrinsic::abs, x, b.getFalse()); });
        emit_funop("fabs",  [&](llvm::Value* x) { return b.CreateUnaryIntrinsic(llvm::Intrinsic::fabs, x); });

        llvm::Function* lookup_demo = llvm::Function::Create(iconst_ty, llvm::GlobalValue::ExternalLinkage, "lookup_demo", &dcvm_smoke);
        b.SetInsertPoint(llvm::BasicBlock::Create(ctx, "entry", lookup_demo));
        llvm::Value* demo_fp = b.CreateIntrinsic(llvm::Intrinsic::dcvm_lookup, {b.getInt64(0xAABBCCDDEEFF0011ULL)});
        b.CreateRet(b.CreatePtrToInt(demo_fp, i64t));

        llvm::FunctionType* call_ty = llvm::FunctionType::get(i64t, {i64t}, false);
        const auto make_callee = [&](const char* name, uint64_t sid, const char* dist) {
            llvm::Function* f = llvm::Function::Create(call_ty, llvm::GlobalValue::ExternalLinkage, name, &dcvm_smoke);
            f->setMetadata("dcvm.sid_distance", llvm::MDNode::get(ctx, {
                llvm::MDString::get(ctx, "sid"),
                llvm::ConstantAsMetadata::get(b.getInt64(sid)),
                llvm::MDString::get(ctx, "distance"),
                llvm::MDString::get(ctx, dist),
            }));
            return f;
        };
        llvm::Function* near_target = make_callee("near_target", 0x1234123412341234ULL, "near");
        llvm::Function* far_target = make_callee("far_target", 0x5678567856785678ULL, "far");
        const auto make_caller = [&](const char* name, llvm::Function* callee) {
            llvm::Function* f = llvm::Function::Create(call_ty, llvm::GlobalValue::ExternalLinkage, name, &dcvm_smoke);
            b.SetInsertPoint(llvm::BasicBlock::Create(ctx, "entry", f));
            b.CreateRet(b.CreateCall(callee, {f->getArg(0)}));
        };
        make_caller("call_near", near_target);
        make_caller("call_far", far_target);
    }

    llvm_transpile::lower_calls_to_lookup(dcvm_smoke);
    const std::string dcvm_asm = llvm_transpile::emit_dcvm_asm(dcvm_smoke);

    llvm::Module call_smoke("call_smoke", ctx);
    {
        llvm::IRBuilder<> b(ctx);
        llvm::Type* i64t = b.getInt64Ty();
        llvm::FunctionType* fn_ty = llvm::FunctionType::get(i64t, {i64t}, false);

        const auto set_sid = [&](llvm::Function* f, uint64_t sid) {
            f->setMetadata("dcvm.sid_distance", llvm::MDNode::get(ctx, {
                llvm::MDString::get(ctx, "sid"),
                llvm::ConstantAsMetadata::get(b.getInt64(sid)),
                llvm::MDString::get(ctx, "distance"),
                llvm::MDString::get(ctx, "far"),
            }));
        };

        llvm::Function* helper_inlined = llvm::Function::Create(fn_ty, llvm::GlobalValue::InternalLinkage, "helper_inlined", &call_smoke);
        helper_inlined->addFnAttr(llvm::Attribute::AlwaysInline);
        set_sid(helper_inlined, 0x1111111111111111ULL);
        b.SetInsertPoint(llvm::BasicBlock::Create(ctx, "entry", helper_inlined));
        b.CreateRet(b.CreateAdd(helper_inlined->getArg(0), b.getInt64(1)));

        llvm::Function* helper_kept = llvm::Function::Create(fn_ty, llvm::GlobalValue::ExternalLinkage, "helper_kept", &call_smoke);
        helper_kept->addFnAttr(llvm::Attribute::NoInline);
        set_sid(helper_kept, 0x2222222222222222ULL);
        b.SetInsertPoint(llvm::BasicBlock::Create(ctx, "entry", helper_kept));
        b.CreateRet(b.CreateMul(helper_kept->getArg(0), b.getInt64(3)));

        llvm::Function* caller = llvm::Function::Create(fn_ty, llvm::GlobalValue::ExternalLinkage, "caller", &call_smoke);
        b.SetInsertPoint(llvm::BasicBlock::Create(ctx, "entry", caller));
        llvm::Value* inlined_res = b.CreateCall(helper_inlined, {caller->getArg(0)}, "a");
        llvm::Value* kept_res0 = b.CreateCall(helper_kept, {inlined_res}, "b");
        llvm::Value* kept_res1 = b.CreateCall(helper_kept, {kept_res0}, "c");
        b.CreateRet(b.CreateAdd(kept_res0, kept_res1));
    }

    const std::string calls_initial = llvm_transpile::emit_llvm_ir(call_smoke);
    llvm_transpile::optimize_module(call_smoke);
    const std::string calls_inlined = llvm_transpile::emit_llvm_ir(call_smoke);
    llvm_transpile::lower_calls_to_lookup(call_smoke);
    const std::string calls_lowered = llvm_transpile::emit_llvm_ir(call_smoke);
    llvm_transpile::optimize_module(call_smoke);
    const std::string calls_deduped = llvm_transpile::emit_llvm_ir(call_smoke);

    const std::filesystem::path out_dir = std::filesystem::path(__FILE__).parent_path();

    const std::array<std::pair<const char*, const std::string*>, 9> outputs = {{
        {"output_unopt.ll", &llvm_unopt},
        {"output_opt.ll", &llvm_opt},
        {"output_lowered.ll", &llvm_lowered},
        {"output_deduped.ll", &llvm_deduped},
        {"output.dcvm.s", &dcvm_asm},
        {"calls_0_initial.ll", &calls_initial},
        {"calls_1_inlined.ll", &calls_inlined},
        {"calls_2_lowered.ll", &calls_lowered},
        {"calls_3_deduped.ll", &calls_deduped},
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
