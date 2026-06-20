#include "llvm_transpile/llvm_transpile.h"
#include "base.h"
#include "decompilation/control_flow_graph.h"
#include "disassembly/instructions.h"
#include "disassembly/opcodes.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Type.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/IR/Value.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/TargetParser/Triple.h"
#include "llvm/ADT/SmallString.h"
#include <memory>
#include <string>

using namespace std::literals;

extern "C" void LLVMInitializeCBackendTarget();
extern "C" void LLVMInitializeCBackendTargetInfo();
extern "C" void LLVMInitializeCBackendTargetMC();


namespace dconstruct::llvm_transpile {

    template <std::size_t N>
    struct fixed_string {
        char data[N];

        constexpr fixed_string(const char (&str)[N]) {
            std::copy_n(str, N, data);
        }
    };

    struct sourced_instruction {
        Opcode opcode;
        u8 dest;
        u8 op1;
        u8 op2;
        u32 node;
        u32 line;
    };

    llvm::Function* transpile_function_to_llvm(llvm::Module& module, const function_disassembly& func_disassembly) {

        ControlFlowGraph graph = ControlFlowGraph::build(func_disassembly);
        llvm::LLVMContext& ctx = module.getContext();

        auto* func_type = llvm::FunctionType::get(llvm::Type::getFloatTy(ctx), false);
        auto* func = llvm::Function::Create(func_type, llvm::GlobalValue::LinkageTypes::ExternalLinkage, 0, func_disassembly.get_id(), &module);

        std::vector<llvm::BasicBlock*> blocks;
        blocks.reserve(graph.m_nodes.size());
        for (const control_flow_node& node : graph.m_nodes) {
            llvm::BasicBlock* block = llvm::BasicBlock::Create(ctx, std::format("bb{}", node.m_index), func);
            blocks.push_back(block);
        }

        llvm::IRBuilder<> builder(blocks[0], blocks[0]->begin());

        llvm::IntegerType* default_int_t = builder.getInt64Ty();
        llvm::Type* default_float_t = builder.getFloatTy();

        std::array<llvm::AllocaInst*, MAX_REGISTER> register_file_frame;
        std::array<llvm::Type*, MAX_REGISTER> types;


        for (u64 i = 0; i < register_file_frame.size(); ++i) {
            register_file_frame[i] = builder.CreateAlloca(default_int_t, nullptr, std::format("r{}", i));
            types[i] = default_int_t;
        }


        const location& symbol_table = func_disassembly.m_stackFrame.m_symbolTable.m_location;


        auto load_register = [&](const u8 index) -> llvm::LoadInst* {
            return builder.CreateLoad(types[index], register_file_frame[index]);
        };

        auto make_store = [&](const u8 index, llvm::Value* rhs, llvm::Type* type = nullptr) -> llvm::StoreInst* {
            if (type) {
                types[index] = type;
            }
            return builder.CreateStore(rhs, register_file_frame[index]);
        };

        auto make_symbol_table_load = [&]<typename T>(const Instruction& istr) -> llvm::StoreInst* {
            T val = symbol_table.get<T>(istr.operand1 * sizeof(u64));

            llvm::Constant* constant = nullptr;

            if constexpr (std::is_integral_v<T>) {
                types[istr.destination] = llvm::IntegerType::get(ctx, sizeof(T));
                constant = llvm::ConstantInt::get(types[istr.destination], val, std::is_signed_v<T>);
            } else if constexpr (std::is_same_v<T, f32>) {
                types[istr.destination] = default_float_t;
                constant = llvm::ConstantFP::get(types[istr.destination], val);
            } else {
                static_assert(false, "invalid symbol table load type");
            }

            return builder.CreateStore(constant, register_file_frame[istr.destination]);
        };

        auto make_mem_load = [&]<typename T>(const Instruction& istr) -> llvm::StoreInst* {

            types[istr.operand1] = llvm::PointerType::get(ctx, 0);
            llvm::Value* from = load_register(istr.operand1);

            llvm::Type* type = nullptr;
            if constexpr (std::is_integral_v<T>) {
                type = llvm::IntegerType::get(ctx, sizeof(T) * 8);
            } else if constexpr (std::is_same_v<T, f32>) {
                type = llvm::Type::getFloatTy(ctx);
            } else {
                static_assert(false, "unsupported type for mem load");
            }

            llvm::Value* res = builder.CreateLoad(type, from);
            return make_store(istr.destination, res, type);
        };

        auto make_mem_store = [&]<typename T>(const Instruction& istr) -> llvm::StoreInst* {
            types[istr.operand1] = llvm::PointerType::get(ctx, 0);
            llvm::Value* from = load_register(istr.operand2);
            llvm::Value* to = load_register(istr.operand1);

            llvm::Type* type = nullptr;
            if constexpr (std::is_integral_v<T>) {
                type = llvm::IntegerType::get(ctx, sizeof(T) * 8);
            } else if constexpr (std::is_same_v<T, f32>) {
                type = llvm::Type::getFloatTy(ctx);
            } else {
                static_assert(false, "unsupported type for mem load");
            }

            llvm::Value* stored = builder.CreateStore(from, to);
            llvm::StoreInst* copy = make_store(istr.destination, from, types[istr.operand2]);
            return copy;
        };

        auto make_binary = [&](const Instruction& istr, std::function<llvm::Value*(llvm::Value*, llvm::Value*)> binary_op) -> llvm::StoreInst* {
            llvm::Value* lhs = load_register(istr.operand1);
            llvm::Value* rhs = load_register(istr.operand2);
            llvm::Value* res = binary_op(lhs, rhs);
            return make_store(istr.destination, res);
        };

        auto make_unary = [&](const Instruction& istr, std::function<llvm::Value*(llvm::Value*)> unary_op) -> llvm::StoreInst* {
            llvm::Value* lhs = load_register(istr.operand1);
            llvm::Value* res = unary_op(lhs);
            return make_store(istr.destination, res);
        };

        for (u32 n = 0; n < graph.m_nodes.size(); ++n) {
            const control_flow_node& node = graph.m_nodes[n];
            for (u32 l = 0; l < node.m_lines.size(); ++l) {

                const function_disassembly_line& line = node.m_lines[l];
                Instruction istr = line.m_instruction;

                *reinterpret_cast<u32*>(&istr.m_padding + 0) = n;
                *reinterpret_cast<u32*>(&istr.m_padding + 2) = l;

                switch (line.m_instruction.opcode) {
                    using enum Opcode;
                    case IAdd: {
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateAdd(a, b); });
                        break;
                    }
                    case FAdd: {
                        types[istr.destination] = default_float_t;
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFAdd(a, b); });
                        break;
                    }
                    case ISub:{
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateSub(a, b); });
                        break;
                    }
                    case FSub: {
                        types[istr.destination] = default_float_t;
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFSub(a, b); });
                        break;
                    }
                    case IMul: {
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateMul(a, b); });
                        break;
                    }
                    case FMul: {
                        types[istr.destination] = default_float_t;
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFMul(a, b); });
                        break;
                    }
                    case IDiv: {
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateSDiv(a, b); });
                        break;
                    }
                    case FDiv: {
                        types[istr.destination] = default_float_t;
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFDiv(a, b); });
                        break;
                    }
                    case OpBitAnd: {
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateAnd(a, b); });
                        break;
                    }
                    case OpBitOr: {
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateOr(a, b); });
                        break;
                    }
                    case OpBitXor: {
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateXor(a, b); });
                        break;
                    }
                    case OpBitNot:
                    case OpLogNot: {
                        make_unary(istr, [&](llvm::Value* a) { return builder.CreateNot(a); });
                        break;
                    }
                    case OpBitNor: {
                        make_unary(istr, [&](llvm::Value* a) { return builder.CreateNot(make_binary(istr, [&](llvm::Value* b, llvm::Value* c) { return builder.CreateOr(b, c); })); });
                        break;
                    }
                    case OpLogAnd: {
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateLogicalAnd(a, b); });
                        break;
                    }
                    case OpLogOr: {
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateLogicalOr(a, b); });
                        break;
                    }
                    case IEqual: {
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateICmpEQ(a, b); });
                        break;
                    }
                    case FEqual: {
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFCmpUEQ(a, b); });
                        break;
                    }
                    case INotEqual: {
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateICmpNE(a, b); });
                        break;
                    }
                    case FNotEqual: {
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFCmpUNE(a, b); });
                        break;
                    }
                    case ILessThan: {
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateICmpSLT(a, b); });
                        break;
                    }
                    case FLessThan: {
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFCmpULT(a, b); });
                        break;
                    }
                    case ILessThanEqual: {
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateICmpSLE(a, b); });
                        break;
                    }
                    case FLessThanEqual: {
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFCmpULE(a, b); });
                        break;
                    }
                    case IGreaterThan: {
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateICmpSGT(a, b); });
                        break;
                    }
                    case FGreaterThan: {
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFCmpUGT(a, b); });
                        break;
                    }
                    case IGreaterThanEqual: {
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateICmpSGE(a, b); });
                        break;
                    }
                    case FGreaterThanEqual: {
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFCmpUGE(a, b); });
                        break;
                    }
                    case IMod: {
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateSRem(a, b); });
                    }
                    case FMod: {
                        llvm::Value* divider = load_register(istr.operand2);
                        llvm::Value* op = load_register(istr.operand1);

                        llvm::FunctionType* fmodf_t = llvm::FunctionType::get(llvm::Type::getFloatTy(ctx), {llvm::Type::getFloatTy(ctx), llvm::Type::getFloatTy(ctx)}, false);
                        llvm::FunctionCallee fmodf_func = module.getOrInsertFunction("fmodf", fmodf_t);
                        llvm::CallInst* call = builder.CreateCall(fmodf_func, {op, divider});

                        make_store(istr.destination, call);
                        types[istr.destination] = default_float_t;
                    }
                    case AssertPointer: {
                        #ifdef NDEBUG
                        llvm::Value* ptr = load_register(istr.destination);
                        builder.CreateIsNotNull(ptr);
                        #endif
                    }
                    case LoadU16Imm: {
                        const u16 val = istr.operand1 | (istr.operand2 << (sizeof(u16) / 2));
                        types[istr.destination] = builder.getInt16Ty();
                        llvm::Constant* value = llvm::ConstantInt::get(types[istr.destination], val, std::is_signed_v<u16>);
                        make_store(istr.destination, value);
                        break;
                    }
                    case LoadStaticU8Imm: {
                        make_symbol_table_load.operator()<u8>(istr);
                        break;
                    }
                    case LoadStaticU16Imm: {
                        make_symbol_table_load.operator()<u16>(istr);
                        break;
                    }
                    case LoadStaticU32Imm: {
                        make_symbol_table_load.operator()<u32>(istr);
                        break;
                    }
                    case LoadStaticU64Imm: {
                        make_symbol_table_load.operator()<u64>(istr);
                        break;
                    }
                    case LoadStaticI8Imm: {
                        make_symbol_table_load.operator()<i8>(istr);
                        break;
                    }
                    case LoadStaticI16Imm: {
                        make_symbol_table_load.operator()<i16>(istr);
                        break;
                    }
                    case LoadStaticI32Imm: {
                        make_symbol_table_load.operator()<i32>(istr);
                        break;
                    }
                    case LoadStaticI64Imm: {
                        make_symbol_table_load.operator()<i64>(istr);
                        break;
                    }
                    case LoadStaticFloatImm : {
                        make_symbol_table_load.operator()<f32>(istr);
                        break;
                    }
                    case LoadU8: {
                        make_mem_load.operator()<u8>(istr);
                        break;
                    }
                    case LoadU16: {
                        make_mem_load.operator()<u16>(istr);
                        break;
                    }
                    case LoadU32: {
                        make_mem_load.operator()<u32>(istr);
                        break;
                    }
                    case LoadU64: {
                        make_mem_load.operator()<u64>(istr);
                        break;
                    }
                    case LoadI8: {
                        make_mem_load.operator()<i8>(istr);
                        break;
                    }
                    case LoadI16: {
                        make_mem_load.operator()<i16>(istr);
                        break;
                    }
                    case LoadI32: {
                        make_mem_load.operator()<i32>(istr);
                        break;
                    }
                    case LoadI64: {
                        make_mem_load.operator()<i64>(istr);
                        break;
                    }
                    case StoreI8: {
                        make_mem_store.operator()<i8>(istr);
                        break;
                    }
                    case LoadStaticPointerImm: {
                        break;
                    }
                    case Move: {
                        make_store(istr.destination, load_register(istr.operand1));
                        break;
                    }
                    case Return: {
                        builder.CreateRet(builder.CreateLoad(types[istr.destination], register_file_frame[istr.destination]));
                        break;
                    }
                    default:
                        continue;
                }
            }
        }

        return func;
    }

    [[nodiscard]] std::string emit_c_from_module(llvm::Module& module) {
        LLVMInitializeCBackendTargetInfo();
        LLVMInitializeCBackendTarget();
        LLVMInitializeCBackendTargetMC();

        llvm::Triple triple(llvm::sys::getDefaultTargetTriple());

        std::string error;
        const llvm::Target* target = llvm::TargetRegistry::lookupTarget("c", triple, error);
        if (target == nullptr) {
            return std::format("error: could not find C backend target: {}", error);
        }

        llvm::TargetOptions options;
        std::unique_ptr<llvm::TargetMachine> machine(target->createTargetMachine(triple, "", "", options, std::optional<llvm::Reloc::Model>()));
        if (machine == nullptr) {
            return "error: could not create C backend target machine";
        }

        module.setTargetTriple(triple);
        module.setDataLayout(machine->createDataLayout());

        llvm::SmallString<0> buffer;
        llvm::raw_svector_ostream out(buffer);

        llvm::LoopAnalysisManager LAM;
        llvm::FunctionAnalysisManager FAM;
        llvm::CGSCCAnalysisManager CGAM;
        llvm::ModuleAnalysisManager MAM;

        llvm::PassBuilder PB(machine.get());
        PB.registerModuleAnalyses(MAM);
        PB.registerCGSCCAnalyses(CGAM);
        PB.registerFunctionAnalyses(FAM);
        PB.registerLoopAnalyses(LAM);
        PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

        llvm::ModulePassManager MPM = PB.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O2);
        MPM.run(module, MAM);

        module.print(out, nullptr);

        llvm::SmallString<0> c_buffer;
        llvm::raw_svector_ostream c_out(c_buffer);

        llvm::legacy::PassManager pass_manager;
        pass_manager.add(new llvm::TargetLibraryInfoWrapperPass(triple));
        pass_manager.add(llvm::createTargetTransformInfoWrapperPass(machine->getTargetIRAnalysis()));

        if (machine->addPassesToEmitFile(pass_manager, c_out, nullptr, llvm::CodeGenFileType::AssemblyFile, true)) {
            return "error: C backend does not support emitting this module";
        }
        pass_manager.run(module);

        std::string result;
        result += "/* ===== Optimized LLVM IR ===== */\n";
        result += std::string(buffer.str());
        result += "\n/* ===== C backend ===== */\n";
        result += std::string(c_buffer.str());
        return result;
    }
}
