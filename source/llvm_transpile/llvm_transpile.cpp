#include "llvm_transpile/llvm_transpile.h"
#include "base.h"
#include "binaryfile.h"
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
#include "llvm/IR/Type.h"
#include <memory>


namespace dconstruct::llvm_transpile {

    [[nodiscard]] std::unique_ptr<llvm::Function> transpile_function_to_llvm(
        llvm::LLVMContext& ctx,
        llvm::Module& module,
        const function_disassembly& func_disassembly
    ) {
        ControlFlowGraph graph = ControlFlowGraph::build(func_disassembly);

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
                }
            }
        }

        return nullptr;
    }
}
