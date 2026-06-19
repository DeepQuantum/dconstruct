#include "llvm_transpile/llvm_transpile.h"
#include "base.h"
#include "decompilation/control_flow_graph.h"
#include "disassembly/instructions.h"
#include "disassembly/opcodes.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constant.h"
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

extern "C" void LLVMInitializeCBackendTarget();
extern "C" void LLVMInitializeCBackendTargetInfo();
extern "C" void LLVMInitializeCBackendTargetMC();


namespace dconstruct::llvm_transpile {

    llvm::Function* transpile_function_to_llvm(llvm::Module& module, const function_disassembly& func_disassembly) {

        ControlFlowGraph graph = ControlFlowGraph::build(func_disassembly);
        llvm::LLVMContext& ctx = module.getContext();

        auto* func_type = llvm::FunctionType::get(llvm::IntegerType::get(ctx, 64), false);
        auto* func = llvm::Function::Create(func_type, llvm::GlobalValue::LinkageTypes::ExternalLinkage, 0, func_disassembly.get_id(), &module);

        std::vector<llvm::BasicBlock*> blocks;
        blocks.reserve(graph.m_nodes.size());
        for (const control_flow_node& node : graph.m_nodes) {
            llvm::BasicBlock* block = llvm::BasicBlock::Create(ctx, std::format("bb{}", node.m_index), func);
            blocks.push_back(block);
        }

        llvm::IRBuilder<> builder(blocks[0], blocks[0]->begin());

        llvm::IntegerType* default_int_t = builder.getInt64Ty();

        std::array<llvm::AllocaInst*, MAX_REGISTER> register_file_frame;
        for (u64 i = 0; i < register_file_frame.size(); ++i) {
            register_file_frame[i] = builder.CreateAlloca(default_int_t, nullptr, std::format("r{}", i));
        }

        auto make_load = [&](const u8 index) -> llvm::LoadInst* {
            return builder.CreateLoad(default_int_t, register_file_frame[index]);
        };

        auto make_store = [&](const u8 index, llvm::Value* rhs) -> llvm::StoreInst* {
            return builder.CreateStore(rhs, register_file_frame[index]);
        };

        auto make_constant_store = [&]<typename T>(const Instruction& istr, T val) -> llvm::StoreInst* {
            llvm::Constant* constant = llvm::ConstantInt::get(default_int_t, val, std::is_signed_v<T>);
            return builder.CreateStore(constant, register_file_frame[istr.destination]);
        };

        auto make_mem_load = [&]<typename T>(const Instruction& istr) -> llvm::StoreInst* {
            llvm::Value* from = make_load(istr.operand1);
            llvm::PointerType* pointer_t = llvm::PointerType::get(ctx, 0);
            llvm::Value* to_pointer = builder.CreateIntToPtr(from, pointer_t);
            llvm::Value* res = builder.CreateLoad(llvm::IntegerType::get(ctx, sizeof(T) * 8), to_pointer);
            return make_store(istr.destination, res);
        };

        auto make_binary = [&](const Instruction& istr, std::function<llvm::Value*(llvm::Value*, llvm::Value*)> binary_op) -> llvm::StoreInst* {
            llvm::Value* lhs = make_load(istr.operand1);
            llvm::Value* rhs = make_load(istr.operand2);
            llvm::Value* add = binary_op(lhs, rhs);
            return make_store(istr.destination, add);
        };

        const location& symbol_table = func_disassembly.m_stackFrame.m_symbolTable.m_location;

        for (const control_flow_node& node : graph.m_nodes) {
            for (const function_disassembly_line& line : node.m_lines) {
                const Instruction& istr = line.m_instruction;
                switch (line.m_instruction.opcode) {
                    using enum Opcode;
                    case IAdd: {
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateAdd(a, b); });
                        break;
                    }
                    case ISub:{
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateSub(a, b); });
                        break;
                    }
                    case IMul: {
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateMul(a, b); });
                        break;
                    }
                    case IDiv: {
                        make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateSDiv(a, b); });
                        break;
                    }
                    case LoadU16Imm: {
                        const u16 val = istr.operand1 | (istr.operand2 << 8);
                        make_constant_store(istr, val);
                        break;
                    }
                    case LoadStaticU8Imm: {
                        make_constant_store(istr, symbol_table.get<u8>(istr.operand1));
                        break;
                    }
                    case LoadStaticU16Imm: {
                        make_constant_store(istr, symbol_table.get<u16>(istr.operand1));
                        break;
                    }
                    case LoadStaticU32Imm: {
                        make_constant_store(istr, symbol_table.get<u32>(istr.operand1));
                        break;
                    }
                    case LoadStaticU64Imm: {
                        make_constant_store(istr, symbol_table.get<u64>(istr.operand1));
                        break;
                    }
                    case LoadStaticI8Imm: {
                        make_constant_store(istr, symbol_table.get<i8>(istr.operand1));
                        break;
                    }
                    case LoadStaticI16Imm: {
                        make_constant_store(istr, symbol_table.get<i16>(istr.operand1));
                        break;
                    }
                    case LoadStaticI32Imm: {
                        make_constant_store(istr, symbol_table.get<i32>(istr.operand1));
                        break;
                    }
                    case LoadStaticI64Imm: {
                        make_constant_store(istr, symbol_table.get<i64>(istr.operand1));
                        break;
                    }
                    case LoadU8: {
                        make_mem_load.operator()<u8>(istr);
                        break;
                    }
                    case LoadStaticPointerImm: {
                        break;
                    }
                    case Move: {
                        make_store(istr.destination, make_load(istr.operand1));
                        break;
                    }
                    case Return: {
                        builder.CreateRet(builder.CreateLoad(default_int_t, register_file_frame[istr.destination]));
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
        result += "/* ===== LLVM IR ===== */\n";
        result += std::string(buffer.str());
        result += "\n/* ===== C backend ===== */\n";
        result += std::string(c_buffer.str());
        return result;
    }
}
