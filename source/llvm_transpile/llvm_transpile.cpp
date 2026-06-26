#include "llvm_transpile/llvm_transpile.h"
#include "ast/type.h"
#include "decompilation/control_flow_graph.h"
#include "disassembly/instructions.h"
#include "disassembly/opcodes.h"
#include "sidbase.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/GlobalIFunc.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Type.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/IR/Value.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/Alignment.h"
#include "llvm/Support/Casting.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/TargetParser/Triple.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/IntrinsicsDCVM.h"
#include <algorithm>
#include <array>
#include <functional>
#include <map>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

#include <print>
#include <format>

using namespace std::literals;

extern "C" void LLVMInitializeCBackendTarget();
extern "C" void LLVMInitializeCBackendTargetInfo();
extern "C" void LLVMInitializeCBackendTargetMC();

extern "C" void LLVMInitializeDCVMTargetInfo();
extern "C" void LLVMInitializeDCVMTarget();
extern "C" void LLVMInitializeDCVMTargetMC();
extern "C" void LLVMInitializeDCVMAsmPrinter();
extern "C" const char* DCVMBytecodeData();
extern "C" size_t DCVMBytecodeSize();
extern "C" void DCVMBytecodeReset();

namespace dconstruct::llvm_transpile {

    llvm_transpiler::llvm_transpiler(llvm::Module& module, const function_disassembly& func_disassembly, const SIDBase& sidbase)
        : m_module(module),
          m_disasm(func_disassembly),
          m_sidbase(sidbase),
          m_ctx(module.getContext()),
          m_symbolTable(func_disassembly.m_stackFrame.m_symbolTable.m_location),
          m_graph(ControlFlowGraph::build(func_disassembly)),
          m_builder(m_ctx) {

        std::vector<llvm::Type*> arg_types(m_disasm.m_stackFrame.m_registerArgs.size());
        std::transform(m_disasm.m_stackFrame.m_registerArgs.begin(), m_disasm.m_stackFrame.m_registerArgs.end(), arg_types.begin(), [&](const ast::full_type& type) {
            return ast_type_to_llvm_type(m_ctx, type);
        });

        auto* func_type = llvm::FunctionType::get(llvm::Type::getInt64Ty(m_ctx), llvm::ArrayRef(arg_types.data(), arg_types.size()), false);
        m_function = llvm::Function::Create(func_type, llvm::GlobalValue::LinkageTypes::ExternalLinkage, 0, m_disasm.get_id(), &m_module);

        const sid64 self_sid = SID(m_disasm.get_id().c_str());
        m_function->setMetadata("dcvm.sid_distance", llvm::MDNode::get(m_ctx, {
            llvm::MDString::get(m_ctx, "sid"),
            llvm::ConstantAsMetadata::get(m_builder.getInt64(self_sid)),
        }));

        for (const control_flow_node& node : m_graph.m_nodes) {
            llvm::BasicBlock* block = llvm::BasicBlock::Create(m_ctx, std::format("bb{}", node.m_index), m_function);
            m_blocks.emplace(node.m_startLine, block);
        }

        m_builder.SetInsertPoint(m_blocks[0], m_blocks[0]->begin());

        m_defaultIntT = m_builder.getInt64Ty();
        m_defaultFloatT = m_builder.getFloatTy();
        m_boolT = m_builder.getInt1Ty();
        m_defaultPointerT = m_builder.getPtrTy();

        for (u64 i = 0; i < m_registerFrame.size(); ++i) {
            m_registerFrame[i] = m_builder.CreateAlloca(m_defaultIntT, nullptr, std::format("r{}", i));
            m_types[i] = m_defaultIntT;
        }

        const u64 symbol_table_size = m_disasm.m_stackFrame.m_symbolTable.m_types.size();
        m_arrayT = llvm::ArrayType::get(m_defaultIntT, symbol_table_size);
        llvm::Constant* symbol_table_array = llvm::ConstantDataArray::get(m_ctx, llvm::ArrayRef(m_symbolTable.as<u64>(), symbol_table_size));
        m_symbolTableGlobal = new llvm::GlobalVariable(m_module, m_arrayT, true, llvm::GlobalValue::PrivateLinkage, symbol_table_array, std::format("{}_symbol_table_src", m_disasm.get_id()));
        m_symbolTableGlobal->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
        m_symbolTableGlobal->setAlignment(llvm::Align(sizeof(u64)));
        llvm::appendToUsed(m_module, {m_symbolTableGlobal});

        for (u64 i = 0; i < m_function->arg_size(); ++i) {
            llvm::Argument* arg = m_function->getArg(i);
            arg->setName(std::format("arg_{}", i));
            make_store(ARGUMENT_REGISTERS_IDX + i, arg, arg->getType());
        }
    }

    resstr<llvm::Function*> llvm_transpiler::run() {
        const control_flow_node* current_node = &m_graph.m_nodes[0];
        for (u32 l = 0; l < m_disasm.m_lines.size(); ++l) {
            const function_disassembly_line& line = m_disasm.m_lines[l];
            Instruction istr = line.m_instruction;
            std::string istr_id = std::format("__{}_{}__", istr.opcode_to_string(), l);

            std::string op = std::format("{}op", istr_id);
            switch (line.m_instruction.opcode) {
                using enum Opcode;
                case IAdd: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateAdd(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case FAdd: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateFAdd(a, b, op); }, m_defaultFloatT, istr_id);
                    break;
                }
                case ISub: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateSub(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case FSub: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateFSub(a, b, op); }, m_defaultFloatT, istr_id);
                    break;
                }
                case IMul: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateMul(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case FMul: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateFMul(a, b, op); }, m_defaultFloatT, istr_id);
                    break;
                }
                case IDiv: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateSDiv(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case FDiv: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateFDiv(a, b, op); }, m_defaultFloatT, istr_id);
                    break;
                }
                case OpBitAnd: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateAnd(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case OpBitOr: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateOr(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case OpBitXor: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateXor(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case OpBitNot: {
                    make_unary(istr, [&](llvm::Value* a) { return m_builder.CreateNot(a, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case OpLogNot: {
                    make_unary(istr, [&](llvm::Value* a) { return m_builder.CreateICmpEQ(a, llvm::ConstantInt::get(a->getType(), 0), op); }, m_boolT, istr_id);
                    break;
                }
                case OpBitNor: {
                    make_unary(
                        istr,
                        [&](llvm::Value* a) { return m_builder.CreateNot(make_binary(istr, [&](llvm::Value* b, llvm::Value* c) { return m_builder.CreateOr(b, c, op); }, m_defaultIntT, istr_id), op); },
                        m_defaultIntT,
                        istr_id
                    );
                    break;
                }
                case OpLogAnd: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateLogicalAnd(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case OpLogOr: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateLogicalOr(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case INeg: {
                    make_unary(istr, [&](llvm::Value* a) { return m_builder.CreateNeg(a, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case FNeg: {
                    make_unary(istr, [&](llvm::Value* a) { return m_builder.CreateFNeg(a, op); }, m_defaultFloatT, istr_id);
                    break;
                }
                case IAddImm: {
                    if (m_types[istr.operand1]->isPointerTy()) {
                        make_binary_imm(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreatePtrAdd(a, b, op); }, m_types[istr.operand1], istr_id);
                    } else {
                        make_binary_imm(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateAdd(a, b, op); }, m_defaultIntT, istr_id);
                    }
                    break;
                }
                case ISubImm: {
                    make_binary_imm(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateSub(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case IMulImm: {
                    make_binary_imm(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateMul(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case IDivImm: {
                    make_binary_imm(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateSDiv(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case IntAsh: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateIntrinsic(llvm::Intrinsic::dcvm_intash, {a, b}); }, m_defaultIntT, istr_id);
                    break;
                }
                case IEqual: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateICmpEQ(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case FEqual: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateFCmpUEQ(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case INotEqual: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateICmpNE(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case FNotEqual: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateFCmpUNE(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case ILessThan: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateICmpSLT(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case FLessThan: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateFCmpULT(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case ILessThanEqual: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateICmpSLE(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case FLessThanEqual: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateFCmpULE(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case IGreaterThan: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateICmpSGT(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case FGreaterThan: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateFCmpUGT(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case IGreaterThanEqual: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateICmpSGE(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case FGreaterThanEqual: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateFCmpUGE(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case IMod: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateSRem(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case FMod: {
                    make_binary(istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateFRem(a, b, op); }, m_defaultFloatT, istr_id);
                    break;
                }
                case IAbs: {
                    make_unary(istr, [&](llvm::Value* a) { return m_builder.CreateIntrinsic(llvm::Intrinsic::abs, a); }, m_defaultIntT, istr_id);
                    break;
                }
                case FAbs: {
                    make_unary(istr, [&](llvm::Value* a) { return m_builder.CreateIntrinsic(llvm::Intrinsic::fabs, a); }, m_defaultFloatT, istr_id);
                    break;
                }
                case AssertPointer: {
                    m_types[istr.destination] = m_defaultPointerT;
                    llvm::Value* ptr = load_register(istr.destination, std::format("{}_ptr", istr_id));
                    m_builder.CreateIsNotNull(ptr, op);
                    break;
                }
                case LoadU16Imm: {
                    const u16 val = istr.operand1 | (istr.operand2 << 8);
                    m_types[istr.destination] = m_defaultIntT;
                    llvm::Constant* value = llvm::ConstantInt::get(m_defaultIntT, val, false);
                    make_store(istr.destination, value);
                    break;
                }
                case LoadStaticU8Imm: {
                    make_symbol_table_load<u8>(istr, istr_id);
                    break;
                }
                case LoadStaticU16Imm: {
                    make_symbol_table_load<u16>(istr, istr_id);
                    break;
                }
                case LoadStaticU32Imm: {
                    make_symbol_table_load<u32>(istr, istr_id);
                    break;
                }
                case LoadStaticU64Imm: {
                    make_symbol_table_load<u64>(istr, istr_id);
                    break;
                }
                case LoadStaticI8Imm: {
                    make_symbol_table_load<i8>(istr, istr_id);
                    break;
                }
                case LoadStaticI16Imm: {
                    make_symbol_table_load<i16>(istr, istr_id);
                    break;
                }
                case LoadStaticI32Imm: {
                    make_symbol_table_load<i32>(istr, istr_id);
                    break;
                }
                case LoadStaticI64Imm: {
                    make_symbol_table_load<i64>(istr, istr_id);
                    break;
                }
                case LoadStaticFloatImm: {
                    make_symbol_table_load<f32>(istr, istr_id);
                    break;
                }
                case LoadStaticInt: {
                    make_symbol_table_load<u64>(istr, istr_id, load_register(istr.operand1, std::format("{}_idx", istr_id)));
                    break;
                }
                case LoadStaticFloat: {
                    make_symbol_table_load<f32>(istr, istr_id, load_register(istr.operand1, std::format("{}_idx", istr_id)));
                    break;
                }
                case LoadStaticPointer: {
                    make_symbol_table_load<void*>(istr, istr_id, load_register(istr.operand1, std::format("{}_idx", istr_id)));
                    break;
                }
                case LoadU8: {
                    make_mem_load<u8>(istr, istr_id);
                    break;
                }
                case LoadU16: {
                    make_mem_load<u16>(istr, istr_id);
                    break;
                }
                case LoadU32: {
                    make_mem_load<u32>(istr, istr_id);
                    break;
                }
                case LoadU64: {
                    make_mem_load<u64>(istr, istr_id);
                    break;
                }
                case LoadI8: {
                    make_mem_load<i8>(istr, istr_id);
                    break;
                }
                case LoadI16: {
                    make_mem_load<i16>(istr, istr_id);
                    break;
                }
                case LoadI32: {
                    make_mem_load<i32>(istr, istr_id);
                    break;
                }
                case LoadI64: {
                    make_mem_load<i64>(istr, istr_id);
                    break;
                }
                case LoadPointer: {
                    make_mem_load<const void*>(istr, istr_id);
                }
                case StoreI8: {
                    make_mem_store<i8>(istr, istr_id);
                    break;
                }
                case StoreU8: {
                    make_mem_store<u8>(istr, istr_id);
                    break;
                }
                case StoreI16: {
                    make_mem_store<i16>(istr, istr_id);
                    break;
                }
                case StoreU16: {
                    make_mem_store<u16>(istr, istr_id);
                    break;
                }
                case StoreI32: {
                    make_mem_store<i32>(istr, istr_id);
                    break;
                }
                case StoreU32: {
                    make_mem_store<u32>(istr, istr_id);
                    break;
                }
                case StoreI64: {
                    make_mem_store<i64>(istr, istr_id);
                    break;
                }
                case StoreU64: {
                    make_mem_store<u64>(istr, istr_id);
                    break;
                }
                case StorePointer: {
                    make_mem_store<const void*>(istr, istr_id);
                }
                case LoadStaticPointerImm: {
                    const ast::full_type& type = m_disasm.m_stackFrame.m_symbolTable.m_types[istr.operand1];
                    if (const ast::primitive_type* string_value = std::get_if<ast::primitive_type>(&type)) {
                        assert(string_value->m_type == ast::primitive_kind::STRING);

                        llvm::Value* string_pointer = m_builder.CreateIntrinsic(llvm::Intrinsic::dcvm_static_pointer, {m_builder.getInt64(istr.operand1)});
                        make_store(istr.destination, string_pointer, m_defaultPointerT);
                    } else {
                        make_symbol_table_load<const void*>(istr, istr_id);
                    }
                    break;
                }
                case Move: {
                    make_store(istr.destination, load_register(istr.operand1, std::format("{}_load_op1", istr_id)), m_types[istr.operand1]);
                    break;
                }
                case LookupPointer: {
                    const ast::full_type& symbol_table_funtion_t = m_disasm.m_stackFrame.m_symbolTable.m_types[istr.operand1];
                    assert(std::holds_alternative<ast::function_type>(symbol_table_funtion_t));
                    const ast::function_type& ast_function_t = std::get<ast::function_type>(symbol_table_funtion_t);
                    llvm::FunctionType* function_type = llvm::cast<llvm::FunctionType>(ast_type_to_llvm_type(m_ctx, symbol_table_funtion_t));

                    const sid64 func_sid = m_symbolTable.get<sid64>(istr.operand1 * sizeof(u64));
                    const char* function_name = m_sidbase.lookup(func_sid);
                    const ast::function_type::DISTANCE distance = ast_function_t.m_distanceType;

                    llvm::FunctionCallee function = m_module.getOrInsertFunction(function_name, function_type);
                    llvm::Function* F = llvm::cast<llvm::Function>(function.getCallee());
                    F->setMetadata("dcvm.sid_distance", llvm::MDNode::get(m_ctx, {
                        llvm::MDString::get(m_ctx, "sid"),
                        llvm::ConstantAsMetadata::get(m_builder.getInt64(func_sid)),
                        llvm::MDString::get(m_ctx, "distance"),
                        llvm::MDString::get(m_ctx, distance == ast::function_type::DISTANCE::NEAR ? "near" : "far"),
                    }
                    ));
                    F->setDoesNotThrow();
                    F->setWillReturn();
                    F->setNoSync();

                    make_store(istr.destination, function.getCallee(), function_type);
                    break;
                }
                case CallFf:
                case Call: {
                    llvm::FunctionType* func_type = llvm::cast<llvm::FunctionType>(m_types[istr.operand1]);
                    llvm::Value* callee = load_register(istr.operand1, std::format("{}_callee", istr_id));
                    std::vector<llvm::Value*> arg_values;
                    arg_values.reserve(istr.operand2);
                    for (u64 i = 0; i < istr.operand2; ++i) {
                        arg_values.push_back(load_register(ARGUMENT_REGISTERS_IDX + i, std::format("{}_arg{}", istr_id, i)));
                        if (i < func_type->getNumParams() && arg_values.back()->getType() != func_type->getParamType(i)) {
                            func_type = exchange_function_types(arg_values.back()->getType(), istr.operand1, i);
                        }
                    }
                    llvm::CallInst* call = m_builder.CreateCall(func_type, callee, llvm::ArrayRef(arg_values.data(), arg_values.size()), std::format("{}_call", istr_id));
                    if (istr.opcode == CallFf) {
                        call->setMetadata("dcvm.distance", llvm::MDNode::get(m_ctx, {llvm::MDString::get(m_ctx, "far")}));
                    }
                    make_store(istr.destination, call, func_type->getReturnType());
                    break;
                }
                case Return:
                case Branch:
                case BranchIf:
                case BranchIfNot: {
                    // explicitly handled by the last line code
                    break;
                }
                default:
                    std::println(stderr, "unhandled opcode {}", istr.opcode_to_string());
                    continue;
            }

            if (l == current_node->m_endLine) {
                switch (istr.opcode) {
                    using enum Opcode;
                    case Branch: {
                        istr_line target = current_node->m_lines.back().m_target;
                        m_builder.CreateBr(m_blocks.at(target));
                        m_builder.SetInsertPoint(m_blocks.at(l + 1));
                        current_node = m_graph.get_node_with_start_line(l + 1);
                        assert(current_node != nullptr);
                        break;
                    }
                    case BranchIf: {
                        const istr_line destination = current_node->m_lines.back().m_target;
                        llvm::Value* condition_reg = m_builder.CreateLoad(m_defaultIntT, m_registerFrame[istr.operand1], std::format("{}_cond", istr_id));
                        llvm::Value* bool_condition_reg = m_builder.CreateICmpNE(condition_reg, llvm::ConstantInt::get(condition_reg->getType(), 0), std::format("{}_bool", istr_id));
                        llvm::BasicBlock* target_block = m_blocks.at(destination);
                        llvm::BasicBlock* fallthrough = m_blocks.at(l + 1);
                        m_builder.CreateCondBr(bool_condition_reg, target_block, fallthrough);
                        m_builder.SetInsertPoint(m_blocks.at(l + 1));
                        current_node = m_graph.get_node_with_start_line(l + 1);
                        assert(current_node != nullptr);
                        break;
                    }
                    case BranchIfNot: {
                        const istr_line destination = current_node->m_lines.back().m_target;
                        llvm::Value* condition_reg = m_builder.CreateLoad(m_defaultIntT, m_registerFrame[istr.operand1], std::format("{}_cond", istr_id));
                        llvm::Value* bool_condition_reg = m_builder.CreateICmpNE(condition_reg, llvm::ConstantInt::get(condition_reg->getType(), 0), std::format("{}_bool", istr_id));
                        llvm::BasicBlock* target_block = m_blocks.at(destination);
                        llvm::BasicBlock* fallthrough = m_blocks.at(l + 1);
                        m_builder.CreateCondBr(bool_condition_reg, fallthrough, target_block);
                        m_builder.SetInsertPoint(m_blocks.at(l + 1));
                        current_node = m_graph.get_node_with_start_line(l + 1);
                        assert(current_node != nullptr);
                        break;
                    }
                    case Return: {
                        llvm::Value* ret_value = load_register(istr.destination, std::format("{}_retval", istr_id));
                        if (ret_value->getType() != m_function->getReturnType() && ret_value->getType()->isIntegerTy() && m_function->getReturnType()->isIntegerTy()) {
                            ret_value = m_builder.CreateIntCast(ret_value, m_function->getReturnType(), false, std::format("{}_retcast", istr_id));
                        }
                        m_builder.CreateRet(ret_value);
                        break;
                    }
                    default: {
                        m_builder.CreateBr(m_blocks.at(l + 1));
                        m_builder.SetInsertPoint(m_blocks.at(l + 1));
                        current_node = m_graph.get_node_with_start_line(l + 1);
                        assert(current_node != nullptr);
                        break;
                    }
                }
            }
        }

        return m_function;
    }

    llvm::Value* llvm_transpiler::coerce_to_storage(llvm::Value* value) {
        llvm::Type* type = value->getType();
        if (type == m_defaultIntT) {
            return value;
        }
        if (type->isPointerTy()) {
            return m_builder.CreatePtrToInt(value, m_defaultIntT);
        }
        if (type->isFloatingPointTy()) {
            value = m_builder.CreateBitCast(value, llvm::IntegerType::get(m_ctx, type->getPrimitiveSizeInBits()));
            type = value->getType();
        }
        return type == m_defaultIntT ? value : m_builder.CreateZExt(value, m_defaultIntT);
    }

    llvm::Value* llvm_transpiler::coerce_from_storage(llvm::Value* raw, llvm::Type* logical) {
        if (logical == m_defaultIntT) {
            return raw;
        }
        if (logical->isFloatingPointTy()) {
            llvm::Type* int_type = llvm::IntegerType::get(m_ctx, logical->getPrimitiveSizeInBits());
            llvm::Value* bits = int_type == m_defaultIntT ? raw : m_builder.CreateTrunc(raw, int_type);
            return m_builder.CreateBitCast(bits, logical);
        }
        if (logical->isIntegerTy()) {
            return m_builder.CreateTrunc(raw, logical);
        }
        return m_builder.CreateIntToPtr(raw, m_defaultPointerT);
    }

    llvm::Value* llvm_transpiler::load_register(const u8 index, std::string_view istr_name) {
        llvm::Value* raw = m_builder.CreateLoad(m_defaultIntT, m_registerFrame[index], istr_name);
        return coerce_from_storage(raw, m_types[index]);
    }

    llvm::StoreInst* llvm_transpiler::make_store(const u8 index, llvm::Value* rhs, llvm::Type* type) {
        if (type) {
            m_types[index] = type;
        }
        return m_builder.CreateStore(coerce_to_storage(rhs), m_registerFrame[index]);
    }

    llvm::StoreInst* llvm_transpiler::make_binary(const Instruction& istr, std::function<llvm::Value*(llvm::Value*, llvm::Value*)> binary_op, llvm::Type* dest_type, std::string_view istr_id) {
        llvm::Value* lhs = load_register(istr.operand1, std::format("{}_load_lhs", istr_id));
        llvm::Value* rhs = load_register(istr.operand2, std::format("{}_load_rhs", istr_id));
        if (lhs->getType() != rhs->getType()) {
            rhs = m_builder.CreateZExtOrTrunc(rhs, lhs->getType());
        }
        llvm::Value* res = binary_op(lhs, rhs);
        m_types[istr.destination] = dest_type;
        return make_store(istr.destination, res);
    }

    llvm::StoreInst* llvm_transpiler::make_binary_imm(const Instruction& istr, std::function<llvm::Value*(llvm::Value*, llvm::Value*)> binary_op, llvm::Type* dest_type, std::string_view istr_id) {
        llvm::Value* lhs = load_register(istr.operand1, std::format("{}_load_lhs", istr_id));
        llvm::Type* imm_type = lhs->getType()->isPointerTy() ? m_builder.getInt8Ty() : lhs->getType();
        llvm::Value* imm =  llvm::ConstantInt::get(imm_type, istr.operand2);
        llvm::Value* res = binary_op(lhs, imm);
        m_types[istr.destination] = dest_type;
        return make_store(istr.destination, res);
    }

    llvm::StoreInst* llvm_transpiler::make_unary(const Instruction& istr, std::function<llvm::Value*(llvm::Value*)> unary_op, llvm::Type* dest_type, std::string_view istr_id) {
        llvm::Value* lhs = load_register(istr.operand1, std::format("{}_load_op1", istr_id));
        llvm::Value* res = unary_op(lhs);
        m_types[istr.destination] = dest_type;
        return make_store(istr.destination, res);
    }

    llvm::FunctionType* llvm_transpiler::exchange_function_types(llvm::Type* new_arg_type, const u8 type_location, const u8 arg_idx) {
        llvm::FunctionType* old_f_type = llvm::cast<llvm::FunctionType>(m_types[type_location]);
        std::vector<llvm::Type*> new_arg_types;
        new_arg_types.reserve(old_f_type->getNumParams());
        for (u32 i = 0; i < old_f_type->getNumParams(); ++i) {
            new_arg_types.push_back(old_f_type->getParamType(i));
        }
        new_arg_types[arg_idx] = new_arg_type;
        llvm::FunctionType* new_f_type = llvm::FunctionType::get(old_f_type->getReturnType(), llvm::ArrayRef(new_arg_types.data(), new_arg_types.size()), old_f_type->isVarArg());
        m_types[type_location] = new_f_type;
        return new_f_type;
    }

    resstr<llvm::Function*> transpile_function_to_llvm(llvm::Module& module, const function_disassembly& func_disassembly, const SIDBase& sidbase) {
        return llvm_transpiler(module, func_disassembly, sidbase).run();
    }

    [[nodiscard]] llvm::Type* ast_type_to_llvm_type(llvm::LLVMContext& ctx, const ast::full_type& type) {
        return std::visit([&ctx](auto&& arg) -> llvm::Type* {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, ast::primitive_type>) {
                if (arg.m_type == ast::primitive_kind::BOOL) {
                    return llvm::Type::getInt1Ty(ctx);
                } else if (ast::is_integral(arg.m_type) || arg.m_type == ast::primitive_kind::SID32 || arg.m_type == ast::primitive_kind::SID) {
                    return llvm::Type::getInt64Ty(ctx);
                } else if (arg.m_type == ast::primitive_kind::F32) {
                    return llvm::Type::getFloatTy(ctx);
                } else if (arg.m_type == ast::primitive_kind::F64) {
                    return llvm::Type::getDoubleTy(ctx);
                } else if (arg.m_type == ast::primitive_kind::STRING || arg.m_type == ast::primitive_kind::NULLPTR) {
                    return llvm::PointerType::getUnqual(ctx);
                } else {
                    assert(false);
                    return nullptr;
                }
            } else if constexpr (std::is_same_v<T, ast::ptr_type>) {
                return llvm::PointerType::getUnqual(ctx);
            } else if constexpr (std::is_same_v<T, ast::enum_type>) {
                return llvm::Type::getInt64Ty(ctx);
            } else if constexpr (std::is_same_v<T, ast::struct_type>) {
                std::vector<llvm::Type*> llvm_types(arg.m_members.size());
                std::transform(arg.m_members.begin(), arg.m_members.end(), llvm_types.begin(), [&ctx](const std::pair<std::string, ast::ref_full_type>& member_pair) {
                    return ast_type_to_llvm_type(ctx, *member_pair.second);
                });
                return llvm::StructType::create(ctx, llvm::ArrayRef(llvm_types.data(), llvm_types.size()), arg.m_name);
            } else if constexpr (std::is_same_v<T, ast::function_type>) {
                std::vector<llvm::Type*> llvm_arg_types(arg.m_arguments.size());
                std::transform(arg.m_arguments.begin(), arg.m_arguments.end(), llvm_arg_types.begin(), [&ctx](const std::pair<std::string, ast::ref_full_type>& paramater_pair) {
                    return ast_type_to_llvm_type(ctx, *paramater_pair.second);
                });
                llvm::Type* return_type = ast_type_to_llvm_type(ctx, *arg.m_return);
                return llvm::FunctionType::get(return_type, llvm::ArrayRef(llvm_arg_types.data(), llvm_arg_types.size()), arg.m_isVariadic);
            } else {
                return llvm::IntegerType::getInt64Ty(ctx);
            }
        }, type);
    }

    static void ensure_c_backend_initialized() {
        static const bool initialized = [] {
            LLVMInitializeCBackendTargetInfo();
            LLVMInitializeCBackendTarget();
            LLVMInitializeCBackendTargetMC();
            return true;
        }();
        (void)initialized;
    }

    static std::unique_ptr<llvm::TargetMachine> create_c_target_machine(std::string& error) {
        ensure_c_backend_initialized();

        llvm::Triple triple(llvm::sys::getDefaultTargetTriple());
        const llvm::Target* target = llvm::TargetRegistry::lookupTarget("c", triple, error);
        if (target == nullptr) {
            error = std::format("could not find C backend target: {}", error);
            return nullptr;
        }

        llvm::TargetOptions options;
        std::unique_ptr<llvm::TargetMachine> machine(target->createTargetMachine(triple, "", "", options, std::optional<llvm::Reloc::Model>()));
        if (machine == nullptr) {
            error = "could not create C backend target machine";
        }
        return machine;
    }

    static void ensure_dcvm_backend_initialized() {
        static const bool initialized = [] {
            LLVMInitializeDCVMTargetInfo();
            LLVMInitializeDCVMTarget();
            LLVMInitializeDCVMTargetMC();
            LLVMInitializeDCVMAsmPrinter();
            return true;
        }();
        (void)initialized;
    }

    static std::unique_ptr<llvm::TargetMachine> create_dcvm_target_machine(std::string& error) {
        ensure_dcvm_backend_initialized();

        llvm::Triple triple("dcvm-unknown-unknown");
        const llvm::Target* target = llvm::TargetRegistry::lookupTarget("dcvm", triple, error);
        if (target == nullptr) {
            error = std::format("could not find DCVM target: {}", error);
            return nullptr;
        }

        llvm::TargetOptions options;
        std::unique_ptr<llvm::TargetMachine> machine(target->createTargetMachine(triple, "", "", options, std::optional<llvm::Reloc::Model>()));
        if (machine == nullptr) {
            error = "could not create DCVM target machine";
        }
        return machine;
    }

    [[nodiscard]] std::string emit_llvm_ir(const llvm::Module& module) {
        std::string ir;
        llvm::raw_string_ostream os(ir);
        module.print(os, nullptr);
        return ir;
    }

    void optimize_module(llvm::Module& module) {
        std::string error;
        std::unique_ptr<llvm::TargetMachine> machine = create_c_target_machine(error);
        if (machine) {
            module.setTargetTriple(machine->getTargetTriple());
            module.setDataLayout(machine->createDataLayout());
        }

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

        llvm::ModulePassManager MPM = PB.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O3);
        MPM.run(module, MAM);
    }

    void lower_calls_to_lookup(llvm::Module& module) {
        llvm::IRBuilder<> builder(module.getContext());

        std::vector<llvm::CallInst*> direct_calls;
        for (llvm::Function& func : module) {
            for (llvm::BasicBlock& block : func) {
                for (llvm::Instruction& inst : block) {
                    auto* call = llvm::dyn_cast<llvm::CallInst>(&inst);
                    if (call == nullptr) {
                        continue;
                    }
                    const llvm::Function* callee = llvm::dyn_cast<llvm::Function>(call->getCalledOperand()->stripPointerCasts());
                    if (callee == nullptr || callee->isIntrinsic() || !callee->hasMetadata("dcvm.sid_distance")) {
                        continue;
                    }
                    direct_calls.push_back(call);
                }
            }
        }

        for (llvm::CallInst* call : direct_calls) {
            llvm::Function* callee = llvm::cast<llvm::Function>(call->getCalledOperand()->stripPointerCasts());
            const llvm::MDNode* sid_md = callee->getMetadata("dcvm.sid_distance");
            llvm::Value* sid = llvm::cast<llvm::ConstantAsMetadata>(sid_md->getOperand(1))->getValue();
            builder.SetInsertPoint(call);
            llvm::CallInst* func_ptr = builder.CreateIntrinsic(llvm::Intrinsic::dcvm_lookup, {sid});
            func_ptr->setName(std::format("__LookupPointer_{}_fptr", callee->getName().str()));
            call->setCalledOperand(func_ptr);
            call->setMetadata(llvm::LLVMContext::MD_callees, llvm::MDNode::get(module.getContext(), llvm::ValueAsMetadata::get(callee)));
        }
    }

    [[nodiscard]] std::string emit_c_from_module(const llvm::Module& module) {
        std::string error;
        std::unique_ptr<llvm::TargetMachine> machine = create_c_target_machine(error);
        if (machine == nullptr) {
            return std::format("error: {}", error);
        }

        std::unique_ptr<llvm::Module> clone = llvm::CloneModule(module);
        clone->setTargetTriple(machine->getTargetTriple());
        clone->setDataLayout(machine->createDataLayout());

        llvm::SmallString<0> c_buffer;
        llvm::raw_svector_ostream c_out(c_buffer);

        llvm::legacy::PassManager pass_manager;
        pass_manager.add(new llvm::TargetLibraryInfoWrapperPass(machine->getTargetTriple()));
        pass_manager.add(llvm::createTargetTransformInfoWrapperPass(machine->getTargetIRAnalysis()));

        if (machine->addPassesToEmitFile(pass_manager, c_out, nullptr, llvm::CodeGenFileType::AssemblyFile, true)) {
            return "error: C backend does not support emitting this module";
        }
        pass_manager.run(*clone);

        return std::string(c_buffer.str());
    }

    [[nodiscard]] std::string emit_dcvm_asm(const llvm::Module& module) {
        std::string error;
        std::unique_ptr<llvm::TargetMachine> machine = create_dcvm_target_machine(error);
        if (machine == nullptr) {
            return std::format("error: {}", error);
        }

        std::unique_ptr<llvm::Module> clone = llvm::CloneModule(module);
        clone->setTargetTriple(machine->getTargetTriple());
        clone->setDataLayout(machine->createDataLayout());

        llvm::SmallString<0> asm_buffer;
        llvm::raw_svector_ostream asm_out(asm_buffer);

        llvm::legacy::PassManager pass_manager;
        pass_manager.add(new llvm::TargetLibraryInfoWrapperPass(machine->getTargetTriple()));
        pass_manager.add(llvm::createTargetTransformInfoWrapperPass(machine->getTargetIRAnalysis()));

        if (machine->addPassesToEmitFile(pass_manager, asm_out, nullptr, llvm::CodeGenFileType::AssemblyFile, true)) {
            return "error: DCVM backend does not support emitting this module";
        }
        pass_manager.run(*clone);

        return std::string(asm_buffer.str());
    }

    [[nodiscard]] std::vector<char> emit_dcvm_bytecode(const llvm::Module& module) {
        std::string error;
        std::unique_ptr<llvm::TargetMachine> machine = create_dcvm_target_machine(error);
        if (machine == nullptr) {
            return {};
        }

        std::unique_ptr<llvm::Module> clone = llvm::CloneModule(module);
        clone->setTargetTriple(machine->getTargetTriple());
        clone->setDataLayout(machine->createDataLayout());

        llvm::SmallString<0> asm_buffer;
        llvm::raw_svector_ostream asm_out(asm_buffer);

        llvm::legacy::PassManager pass_manager;
        pass_manager.add(new llvm::TargetLibraryInfoWrapperPass(machine->getTargetTriple()));
        pass_manager.add(llvm::createTargetTransformInfoWrapperPass(machine->getTargetIRAnalysis()));

        if (machine->addPassesToEmitFile(pass_manager, asm_out, nullptr, llvm::CodeGenFileType::AssemblyFile, true)) {
            return {};
        }

        DCVMBytecodeReset();
        pass_manager.run(*clone);

        const char* data = DCVMBytecodeData();
        const std::size_t size = DCVMBytecodeSize();
        return std::vector<char>(data, data + size);
    }
}
