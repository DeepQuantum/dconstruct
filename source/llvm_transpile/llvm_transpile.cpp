#include "llvm_transpile/llvm_transpile.h"
#include "ast/type.h"
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
#include "llvm/IR/GlobalVariable.h"
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
#include "llvm/Support/Alignment.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/TargetParser/Triple.h"
#include "llvm/ADT/SmallString.h"
#include <string>
#include <variant>

using namespace std::literals;

extern "C" void LLVMInitializeCBackendTarget();
extern "C" void LLVMInitializeCBackendTargetInfo();
extern "C" void LLVMInitializeCBackendTargetMC();

namespace dconstruct::llvm_transpile {

    template <std::size_t N> struct fixed_string {
        char data[N];

        constexpr fixed_string(const char (&str)[N]) { std::copy_n(str, N, data); }
    };

    struct sourced_instruction {
        Opcode opcode;
        u8 dest;
        u8 op1;
        u8 op2;
        u32 node;
        u32 line;
    };

    resstr<llvm::Function*> transpile_function_to_llvm(llvm::Module& module, const function_disassembly& func_disassembly) {
        ControlFlowGraph graph = ControlFlowGraph::build(func_disassembly);
        llvm::LLVMContext& ctx = module.getContext();

        auto* func_type = llvm::FunctionType::get(llvm::Type::getFloatTy(ctx), false);
        auto* func = llvm::Function::Create(func_type, llvm::GlobalValue::LinkageTypes::ExternalLinkage, 0, func_disassembly.get_id(), &module);

        std::map<istr_line, llvm::BasicBlock*> blocks;
        for (const control_flow_node& node : graph.m_nodes) {
            llvm::BasicBlock* block = llvm::BasicBlock::Create(ctx, std::format("bb{}", node.m_index), func);
            blocks.emplace(node.m_startLine, block);
        }

        llvm::IRBuilder<> builder(blocks[0], blocks[0]->begin());

        llvm::IntegerType* default_int_t = builder.getInt64Ty();
        llvm::Type* default_float_t = builder.getFloatTy();
        llvm::Type* bool_t = builder.getInt1Ty();
        llvm::Type* default_pointer_t = builder.getPtrTy();

        std::array<llvm::AllocaInst*, MAX_REGISTER> register_file_frame;
        std::array<llvm::Type*, MAX_REGISTER> types;

        for (u64 i = 0; i < register_file_frame.size(); ++i) {
            register_file_frame[i] = builder.CreateAlloca(default_int_t, nullptr, std::format("r{}", i));
            types[i] = default_int_t;
        }

        const location& symbol_table = func_disassembly.m_stackFrame.m_symbolTable.m_location;
        const u64 symbol_table_size = func_disassembly.m_stackFrame.m_symbolTable.m_types.size();
        llvm::ArrayType* array_t = llvm::ArrayType::get(default_int_t, symbol_table_size);
        llvm::Constant* symbol_table_array = llvm::ConstantDataArray::get(ctx, llvm::ArrayRef(symbol_table.as<u64>(), symbol_table_size));
        llvm::GlobalVariable* symbol_table_global = new llvm::GlobalVariable(module, array_t, true, llvm::GlobalValue::PrivateLinkage, symbol_table_array, "symbol_table");
        symbol_table_global->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
        symbol_table_global->setAlignment(llvm::Align(sizeof(u64)));
        llvm::appendToUsed(module, {symbol_table_global});

        auto load_register = [&](const u8 index, std::string_view istr_name = "") -> llvm::LoadInst* { return builder.CreateLoad(types[index], register_file_frame[index], istr_name); };

        auto make_store = [&](const u8 index, llvm::Value* rhs, llvm::Type* type = nullptr) -> llvm::StoreInst* {
            if (type) {
                types[index] = type;
            }
            return builder.CreateStore(rhs, register_file_frame[index]);
        };

        auto make_symbol_table_load = [&]<typename T>(const Instruction& istr, std::string_view istr_id, llvm::Value* idx = nullptr) -> llvm::StoreInst* {
            if constexpr (std::is_integral_v<T>) {
                types[istr.destination] = llvm::IntegerType::get(ctx, sizeof(T) * 8);
            } else if constexpr (std::is_same_v<T, f32>) {
                types[istr.destination] = default_float_t;
            } else if constexpr (std::is_pointer_v<T>) {
                types[istr.destination] = default_pointer_t;
            } else {
                static_assert(false, "invalid symbol table load type");
            }

            if (!idx) {
                idx = builder.getInt64(istr.operand1);
            }

            llvm::Value* st_value = builder.CreateInBoundsGEP(array_t, symbol_table_global, {builder.getInt64(0), idx}, std::format("{}_gep", istr_id));
            llvm::Value* load = builder.CreateLoad(types[istr.destination], st_value, std::format("{}_st_load", istr_id));

            return builder.CreateStore(load, register_file_frame[istr.destination]);
        };

        auto make_mem_load = [&]<typename T>(const Instruction& istr, std::string_view istr_id) -> llvm::StoreInst* {
            types[istr.operand1] = llvm::PointerType::get(ctx, 0);
            llvm::Value* from = load_register(istr.operand1, std::format("{}_addr", istr_id));

            llvm::Type* type = nullptr;
            if constexpr (std::is_integral_v<T>) {
                type = llvm::IntegerType::get(ctx, sizeof(T) * 8);
            } else if constexpr (std::is_same_v<T, f32>) {
                type = llvm::Type::getFloatTy(ctx);
            } else {
                static_assert(false, "unsupported type for mem load");
            }

            llvm::Value* res = builder.CreateLoad(type, from, std::format("{}_load", istr_id));
            return make_store(istr.destination, res, type);
        };

        auto make_mem_store = [&]<typename T>(const Instruction& istr, std::string_view istr_id) -> llvm::StoreInst* {
            types[istr.operand1] = llvm::PointerType::get(ctx, 0);
            llvm::Value* from = load_register(istr.operand2, std::format("{}_value", istr_id));
            llvm::Value* to = load_register(istr.operand1, std::format("{}_addr", istr_id));

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

        auto make_binary = [&](const Instruction& istr, std::function<llvm::Value*(llvm::Value*, llvm::Value*)> binary_op, llvm::Type* dest_type, std::string_view istr_id = "") -> llvm::StoreInst* {
            llvm::Value* lhs = load_register(istr.operand1, std::format("{}_load_lhs", istr_id));
            llvm::Value* rhs = load_register(istr.operand2, std::format("{}_load_rhs", istr_id));
            llvm::Value* res = binary_op(lhs, rhs);
            types[istr.destination] = dest_type;
            return make_store(istr.destination, res);
        };

        auto make_binary_imm = [&](const Instruction& istr, std::function<llvm::Value*(llvm::Value*, llvm::Value*)> binary_op, llvm::Type* dest_type, std::string_view istr_id = "") -> llvm::StoreInst* {
            llvm::Value* lhs = load_register(istr.operand1, std::format("{}_load_lhs", istr_id));
            llvm::Constant* imm = llvm::ConstantInt::get(bool_t, istr.operand2);
            llvm::Value* res = binary_op(lhs, imm);
            types[istr.destination] = dest_type;
            return make_store(istr.destination, res);
        };

        auto make_unary = [&](const Instruction& istr, std::function<llvm::Value*(llvm::Value*)> unary_op, llvm::Type* dest_type, std::string_view istr_id = "") -> llvm::StoreInst* {
            llvm::Value* lhs = load_register(istr.operand1, std::format("{}_load_op1", istr_id));
            llvm::Value* res = unary_op(lhs);
            types[istr.destination] = dest_type;
            return make_store(istr.destination, res);
        };


        const control_flow_node* current_node = &graph.m_nodes[0];
        for (u32 l = 0; l < func_disassembly.m_lines.size(); ++l) {
            const function_disassembly_line& line = func_disassembly.m_lines[l];
            Instruction istr = line.m_instruction;
            std::string istr_id = std::format("__{}_{}__", istr.opcode_to_string(), l);

            std::string op = std::format("{}_op", istr_id);
            switch (line.m_instruction.opcode) {
                using enum Opcode;
                case IAdd: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateAdd(a, b, op); }, default_int_t, istr_id);
                    break;
                }
                case FAdd: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFAdd(a, b, op); }, default_float_t, istr_id);
                    break;
                }
                case ISub: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateSub(a, b, op); }, default_int_t, istr_id);
                    break;
                }
                case FSub: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFSub(a, b, op); }, default_float_t, istr_id);
                    break;
                }
                case IMul: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateMul(a, b, op); }, default_int_t, istr_id);
                    break;
                }
                case FMul: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFMul(a, b, op); }, default_float_t, istr_id);
                    break;
                }
                case IDiv: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateSDiv(a, b, op); }, default_int_t, istr_id);
                    break;
                }
                case FDiv: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFDiv(a, b, op); }, default_float_t, istr_id);
                    break;
                }
                case OpBitAnd: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateAnd(a, b, op); }, default_int_t, istr_id);
                    break;
                }
                case OpBitOr: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateOr(a, b, op); }, default_int_t, istr_id);
                    break;
                }
                case OpBitXor: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateXor(a, b, op); }, default_int_t, istr_id);
                    break;
                }
                case OpBitNot:
                case OpLogNot: {
                    make_unary(istr, [&](llvm::Value* a) { return builder.CreateNot(a, op); }, default_int_t, istr_id);
                    break;
                }
                case OpBitNor: {
                    make_unary(
                        istr,
                        [&](llvm::Value* a) { return builder.CreateNot(make_binary(istr, [&](llvm::Value* b, llvm::Value* c) { return builder.CreateOr(b, c, op); }, default_int_t, istr_id), op); },
                        default_int_t,
                        istr_id
                    );
                    break;
                }
                case OpLogAnd: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateLogicalAnd(a, b, op); }, bool_t, istr_id);
                    break;
                }
                case OpLogOr: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateLogicalOr(a, b, op); }, bool_t, istr_id);
                    break;
                }
                case INeg: {
                    make_unary(istr, [&](llvm::Value* a) { return builder.CreateNeg(a, op); }, default_int_t, istr_id);
                    break;
                }
                case FNeg: {
                    make_unary(istr, [&](llvm::Value* a) { return builder.CreateFNeg(a, op); }, default_float_t, istr_id);
                    break;
                }
                case IAddImm: {
                    make_binary_imm(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateAdd(a, b, op); }, default_int_t, istr_id);
                    break;
                }
                case ISubImm: {
                    make_binary_imm(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateSub(a, b, op); }, default_int_t, istr_id);
                    break;
                }
                case IMulImm: {
                    make_binary_imm(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateMul(a, b, op); }, default_int_t, istr_id);
                    break;
                }
                case IDivImm: {
                    make_binary_imm(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateSDiv(a, b, op); }, default_int_t, istr_id);
                    break;
                }
                case IntAsh: {
                    // llvm::Value* shift_amount = load_register(istr.operand2);
                    // llvm::Constant* zero = llvm::ConstantInt::get(default_int_t, 0);
                    // builder.CreateICmpSLT(shift_amount, zero);
                    break;
                }
                case IEqual: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateICmpEQ(a, b, op); }, bool_t, istr_id);
                    break;
                }
                case FEqual: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFCmpUEQ(a, b, op); }, bool_t, istr_id);
                    break;
                }
                case INotEqual: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateICmpNE(a, b, op); }, bool_t, istr_id);
                    break;
                }
                case FNotEqual: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFCmpUNE(a, b, op); }, bool_t, istr_id);
                    break;
                }
                case ILessThan: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateICmpSLT(a, b, op); }, bool_t, istr_id);
                    break;
                }
                case FLessThan: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFCmpULT(a, b, op); }, bool_t, istr_id);
                    break;
                }
                case ILessThanEqual: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateICmpSLE(a, b, op); }, bool_t, istr_id);
                    break;
                }
                case FLessThanEqual: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFCmpULE(a, b, op); }, bool_t, istr_id);
                    break;
                }
                case IGreaterThan: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateICmpSGT(a, b, op); }, bool_t, istr_id);
                    break;
                }
                case FGreaterThan: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFCmpUGT(a, b, op); }, bool_t, istr_id);
                    break;
                }
                case IGreaterThanEqual: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateICmpSGE(a, b, op); }, bool_t, istr_id);
                    break;
                }
                case FGreaterThanEqual: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFCmpUGE(a, b, op); }, bool_t, istr_id);
                    break;
                }
                case IMod: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateSRem(a, b, op); }, default_int_t, istr_id);
                    break;
                }
                case FMod: {
                    llvm::Value* divider = load_register(istr.operand2, std::format("{}_divider", istr_id));
                    llvm::Value* dividend = load_register(istr.operand1, std::format("{}_dividend", istr_id));

                    llvm::Function* fmodf_func = module.getFunction("fmodf");
                    if (!fmodf_func) {
                        return std::unexpected{"couldn't find fmodf"};
                    }
                    llvm::CallInst* call = builder.CreateCall(fmodf_func, {dividend, divider}, op);

                    make_store(istr.destination, call);
                    types[istr.destination] = default_float_t;
                    break;
                }
                case AssertPointer: {
                    types[istr.destination] = default_pointer_t;
                    llvm::Value* ptr = load_register(istr.destination, std::format("{}_ptr", istr_id));
                    builder.CreateIsNotNull(ptr, op);
                    break;
                }
                case LoadU16Imm: {
                    const u16 val = istr.operand1 | (istr.operand2 << (sizeof(u16) / 2));
                    types[istr.destination] = builder.getInt16Ty();
                    llvm::Constant* value = llvm::ConstantInt::get(types[istr.destination], val, std::is_signed_v<u16>);
                    make_store(istr.destination, value);
                    break;
                }
                case LoadStaticU8Imm: {
                    make_symbol_table_load.operator()<u8>(istr, istr_id);
                    break;
                }
                case LoadStaticU16Imm: {
                    make_symbol_table_load.operator()<u16>(istr, istr_id);
                    break;
                }
                case LoadStaticU32Imm: {
                    make_symbol_table_load.operator()<u32>(istr, istr_id);
                    break;
                }
                case LoadStaticU64Imm: {
                    make_symbol_table_load.operator()<u64>(istr, istr_id);
                    break;
                }
                case LoadStaticI8Imm: {
                    make_symbol_table_load.operator()<i8>(istr, istr_id);
                    break;
                }
                case LoadStaticI16Imm: {
                    make_symbol_table_load.operator()<i16>(istr, istr_id);
                    break;
                }
                case LoadStaticI32Imm: {
                    make_symbol_table_load.operator()<i32>(istr, istr_id);
                    break;
                }
                case LoadStaticI64Imm: {
                    make_symbol_table_load.operator()<i64>(istr, istr_id);
                    break;
                }
                case LoadStaticFloatImm: {
                    make_symbol_table_load.operator()<f32>(istr, istr_id);
                    break;
                }
                case LoadStaticInt: {
                    make_symbol_table_load.operator()<u64>(istr, istr_id, load_register(istr.operand1, std::format("{}_idx", istr_id)));
                    break;
                }
                case LoadStaticFloat: {
                    make_symbol_table_load.operator()<f32>(istr, istr_id, load_register(istr.operand1, std::format("{}_idx", istr_id)));
                    break;
                }
                case LoadStaticPointer: {
                    make_symbol_table_load.operator()<void*>(istr, istr_id, load_register(istr.operand1, std::format("{}_idx", istr_id)));
                    break;
                }
                case LoadU8: {
                    make_mem_load.operator()<u8>(istr, istr_id);
                    break;
                }
                case LoadU16: {
                    make_mem_load.operator()<u16>(istr, istr_id);
                    break;
                }
                case LoadU32: {
                    make_mem_load.operator()<u32>(istr, istr_id);
                    break;
                }
                case LoadU64: {
                    make_mem_load.operator()<u64>(istr, istr_id);
                    break;
                }
                case LoadI8: {
                    make_mem_load.operator()<i8>(istr, istr_id);
                    break;
                }
                case LoadI16: {
                    make_mem_load.operator()<i16>(istr, istr_id);
                    break;
                }
                case LoadI32: {
                    make_mem_load.operator()<i32>(istr, istr_id);
                    break;
                }
                case LoadI64: {
                    make_mem_load.operator()<i64>(istr, istr_id);
                    break;
                }
                case StoreI8: {
                    make_mem_store.operator()<i8>(istr, istr_id);
                    break;
                }
                case StoreU8: {
                    make_mem_store.operator()<u8>(istr, istr_id);
                    break;
                }
                case StoreI16: {
                    make_mem_store.operator()<i16>(istr, istr_id);
                    break;
                }
                case StoreU16: {
                    make_mem_store.operator()<u16>(istr, istr_id);
                    break;
                }
                case StoreI32: {
                    make_mem_store.operator()<i32>(istr, istr_id);
                    break;
                }
                case StoreU32: {
                    make_mem_store.operator()<u32>(istr, istr_id);
                    break;
                }
                case StoreI64: {
                    make_mem_store.operator()<i64>(istr, istr_id);
                    break;
                }
                case StoreU64: {
                    make_mem_store.operator()<u64>(istr, istr_id);
                    break;
                }
                case LoadStaticPointerImm: {
                    break;
                }
                case Move: {
                    make_store(istr.destination, load_register(istr.operand1, std::format("{}_load_op1", istr_id)));
                    break;
                }
                case Return:
                case Branch:
                case BranchIf:
                case BranchIfNot: {
                    // explicitly handled by the last line code
                    break;
                }
                case LookupPointer: {;
                    const auto& disasm_function_type = func_disassembly.m_stackFrame.m_symbolTable.m_types[istr.operand1];
                    assert(std::holds_alternative<ast::function_type>(disasm_function_type));
                    
                }
                default:
                    continue;
            }

            if (l == current_node->m_endLine) {
                switch (istr.opcode) {
                    using enum Opcode;
                    case Branch: {
                        istr_line target = istr.destination | (istr.operand2 << (sizeof(u16) / 2));
                        builder.CreateBr(blocks.at(target));
                        builder.SetInsertPoint(blocks.at(l + 1));
                        current_node = graph.get_node_with_start_line(l + 1);
                        assert(current_node != nullptr);
                        break;
                    }
                    case BranchIf: {
                        const istr_line destination = istr.destination | (istr.operand2 << (sizeof(u16) / 2));
                        llvm::Value* condition_reg = load_register(istr.operand1, std::format("{}_cond", istr_id));
                        llvm::BasicBlock* target_block = blocks.at(destination);
                        llvm::BasicBlock* fallthrough = blocks.at(l + 1);
                        builder.CreateCondBr(condition_reg, target_block, fallthrough);
                        builder.SetInsertPoint(blocks.at(l + 1));
                        current_node = graph.get_node_with_start_line(l + 1);
                        assert(current_node != nullptr);
                        break;
                    }
                    case BranchIfNot: {
                        const istr_line destination = istr.destination | (istr.operand2 << (sizeof(u16) / 2));
                        llvm::Value* condition_reg = load_register(istr.operand1, std::format("{}_cond", istr_id));
                        llvm::BasicBlock* target_block = blocks.at(destination);
                        llvm::BasicBlock* fallthrough = blocks.at(l + 1);
                        builder.CreateCondBr(condition_reg, fallthrough, target_block);
                        builder.SetInsertPoint(blocks.at(l + 1));
                        current_node = graph.get_node_with_start_line(l + 1);
                        assert(current_node != nullptr);
                        break;
                    }
                    case Return: {
                        builder.CreateRet(load_register(istr.destination, std::format("{}_retval", istr_id)));
                        break;
                    }
                    default: {
                        builder.CreateBr(blocks.at(l + 1));
                        builder.SetInsertPoint(blocks.at(l + 1));
                        current_node = graph.get_node_with_start_line(l + 1);
                        assert(current_node != nullptr);
                        break;
                    }
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
