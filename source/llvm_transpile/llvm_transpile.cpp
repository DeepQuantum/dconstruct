#include "llvm_transpile/llvm_transpile.h"
#include "ast/type.h"
#include "decompilation/control_flow_graph.h"
#include "disassembly/disassembler.h"
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
#include <filesystem>
#include <fstream>
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

    llvm_transpiler::llvm_transpiler(const SIDBase& sidbase)
        :   m_sidbase(sidbase),
            m_ctx(llvm::LLVMContext()),
            m_builder(m_ctx) {

        m_defaultIntT = m_builder.getInt64Ty();
        m_defaultFloatT = m_builder.getFloatTy();
        m_boolT = m_builder.getInt1Ty();
        m_defaultPointerT = m_builder.getPtrTy();
    }

    void llvm_transpiler::add_module(std::string_view name, const std::vector<const function_disassembly*>& funcs) {
        std::vector<lift_function> functions;
        functions.reserve(funcs.size());
        for (const function_disassembly* disassembly_function : funcs) {
            functions.emplace_back(disassembly_function, &disassembly_function->m_stackFrame.m_symbolTable.m_location);
        }
        m_translationUnits.emplace_back(std::make_unique<llvm::Module>(name, m_ctx), std::move(functions));
    }

    void llvm_transpiler::prepare_function(llvm::Module& module, lift_function& function) {
        const function_disassembly* m_disasm = function.m_disasm;
        const location* m_symbolTable = function.m_symbolTable;
        function.m_graph = ControlFlowGraph::build(*m_disasm);
        ControlFlowGraph* m_graph = &*function.m_graph;
        auto& m_blocks = function.m_blocks;
        auto& m_types = function.m_types;
        auto& m_registerFrame = function.m_registerFrame;
        m_blocks.clear();
        m_builder.ClearInsertionPoint();

        std::vector<llvm::Type*> arg_types(m_disasm->m_stackFrame.m_registerArgs.size());
        std::transform(m_disasm->m_stackFrame.m_registerArgs.begin(), m_disasm->m_stackFrame.m_registerArgs.end(), arg_types.begin(), [&](const ast::full_type& type) {
            return ast_type_to_llvm_type(m_ctx, type);
        });

        auto* func_type = llvm::FunctionType::get(llvm::Type::getInt64Ty(m_ctx), llvm::ArrayRef(arg_types.data(), arg_types.size()), false);
        llvm::Function* m_function = llvm::Function::Create(func_type, llvm::GlobalValue::LinkageTypes::ExternalLinkage, 0, m_disasm->get_id(), &module);
        function.m_llvmFunc = m_function;

        const sid64 self_sid = SID(m_disasm->get_id().c_str());
        m_function->setMetadata("dcvm.sid_distance", llvm::MDNode::get(m_ctx, {
            llvm::MDString::get(m_ctx, "sid"),
            llvm::ConstantAsMetadata::get(m_builder.getInt64(self_sid)),
        }));

        for (const control_flow_node& node : m_graph->m_nodes) {
            llvm::BasicBlock* block = llvm::BasicBlock::Create(m_ctx, std::format("bb{}", node.m_index), m_function);
            m_blocks.emplace(node.m_startLine, block);
        }

        m_builder.SetInsertPoint(m_blocks[0], m_blocks[0]->begin());

        for (u64 i = 0; i < m_registerFrame.size(); ++i) {
            m_registerFrame[i] = m_builder.CreateAlloca(m_defaultIntT, nullptr, std::format("r{}", i));
            m_types[i] = m_defaultIntT;
        }

        const u64 symbol_table_size = m_disasm->m_stackFrame.m_symbolTable.m_types.size();
        function.m_symbolTableArrayT = llvm::ArrayType::get(m_defaultIntT, symbol_table_size);
        llvm::Constant* symbol_table_array = llvm::ConstantDataArray::get(m_ctx, llvm::ArrayRef(m_symbolTable->as<u64>(), symbol_table_size));
        function.m_symbolTableGlobal = new llvm::GlobalVariable(module, function.m_symbolTableArrayT, true, llvm::GlobalValue::PrivateLinkage, symbol_table_array, std::format("{}_symbol_table_src", m_disasm->get_id()));
        function.m_symbolTableGlobal->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
        function.m_symbolTableGlobal->setAlignment(llvm::Align(sizeof(u64)));
        llvm::appendToUsed(module, {function.m_symbolTableGlobal});

        for (u64 i = 0; i < m_function->arg_size(); ++i) {
            llvm::Argument* arg = m_function->getArg(i);
            arg->setName(std::format("arg_{}", i));
            make_store(function, ARGUMENT_REGISTERS_IDX + i, arg, arg->getType());
        }
    }

    resstr<llvm::Function*> llvm_transpiler::transpile_function(llvm::Module& module, const function_disassembly& func_disassembly) {
        lift_function function{&func_disassembly, &func_disassembly.m_stackFrame.m_symbolTable.m_location};
        prepare_function(module, function);
        return lift_prepared_function(module, function);
    }

    resstr<llvm::Function*> llvm_transpiler::lift_prepared_function(llvm::Module& module, lift_function& function) {
        const function_disassembly* m_disasm = function.m_disasm;
        const location* m_symbolTable = function.m_symbolTable;
        ControlFlowGraph* m_graph = &*function.m_graph;
        auto& m_blocks = function.m_blocks;
        auto& m_types = function.m_types;
        auto& m_registerFrame = function.m_registerFrame;
        llvm::Function* m_function = function.m_llvmFunc;
        llvm::Module& m_module = module;

        m_builder.SetInsertPoint(m_blocks.at(0));
        const control_flow_node* current_node = &m_graph->m_nodes[0];
        for (u32 l = 0; l < m_disasm->m_lines.size(); ++l) {
            const function_disassembly_line& line = m_disasm->m_lines[l];
            Instruction istr = line.m_instruction;
            std::string istr_id = std::format("__{}_{}__", istr.opcode_to_string(), l);

            std::string op = std::format("{}op", istr_id);
            switch (line.m_instruction.opcode) {
                using enum Opcode;
                case IAdd: {
                    if (m_types[istr.operand1]->isPointerTy()) {
                        make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreatePtrAdd(a, b, op); }, m_types[istr.operand1], istr_id);
                    } else if (m_types[istr.operand2]->isPointerTy()) {
                        make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreatePtrAdd(b, a, op); }, m_types[istr.operand1], istr_id);
                    } else {
                        make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateAdd(a, b, op); }, m_defaultIntT, istr_id);
                    }
                    break;
                }
                case FAdd: {
                    make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateFAdd(a, b, op); }, m_defaultFloatT, istr_id);
                    break;
                }
                case ISub: {
                    make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateSub(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case FSub: {
                    make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateFSub(a, b, op); }, m_defaultFloatT, istr_id);
                    break;
                }
                case IMul: {
                    make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateMul(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case FMul: {
                    make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateFMul(a, b, op); }, m_defaultFloatT, istr_id);
                    break;
                }
                case IDiv: {
                    make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateSDiv(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case FDiv: {
                    make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateFDiv(a, b, op); }, m_defaultFloatT, istr_id);
                    break;
                }
                case OpBitAnd: {
                    make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateAnd(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case OpBitOr: {
                    make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateOr(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case OpBitXor: {
                    make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateXor(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case OpBitNot: {
                    make_unary(function, istr, [&](llvm::Value* a) { return m_builder.CreateNot(a, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case OpLogNot: {
                    make_unary(function, istr, [&](llvm::Value* a) { return m_builder.CreateICmpEQ(a, llvm::ConstantInt::get(a->getType(), 0), op); }, m_boolT, istr_id);
                    break;
                }
                case OpBitNor: {
                    make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateNot(m_builder.CreateOr(a, b, op)); },m_defaultIntT, istr_id);
                    break;
                }
                case OpLogAnd: {
                    make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateLogicalAnd(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case OpLogOr: {
                    make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateLogicalOr(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case INeg: {
                    make_unary(function, istr, [&](llvm::Value* a) { return m_builder.CreateNeg(a, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case FNeg: {
                    make_unary(function, istr, [&](llvm::Value* a) { return m_builder.CreateFNeg(a, op); }, m_defaultFloatT, istr_id);
                    break;
                }
                case IAddImm: {
                    if (m_types[istr.operand1]->isPointerTy()) {
                        make_binary_imm(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreatePtrAdd(a, b, op); }, m_types[istr.operand1], istr_id);
                    } else {
                        make_binary_imm(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateAdd(a, b, op); }, m_defaultIntT, istr_id);
                    }
                    break;
                }
                case ISubImm: {
                    make_binary_imm(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateSub(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case IMulImm: {
                    make_binary_imm(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateMul(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case IDivImm: {
                    make_binary_imm(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateSDiv(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case IntAsh: {
                    make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateIntrinsic(llvm::Intrinsic::dcvm_intash, {a, b}); }, m_defaultIntT, istr_id);
                    break;
                }
                case IEqual: {
                    make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateICmpEQ(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case FEqual: {
                    make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateFCmpUEQ(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case INotEqual: {
                    make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateICmpNE(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case FNotEqual: {
                    make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateFCmpUNE(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case ILessThan: {
                    make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateICmpSLT(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case FLessThan: {
                    make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateFCmpULT(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case ILessThanEqual: {
                    make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateICmpSLE(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case FLessThanEqual: {
                    make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateFCmpULE(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case IGreaterThan: {
                    make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateICmpSGT(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case FGreaterThan: {
                    make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateFCmpUGT(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case IGreaterThanEqual: {
                    make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateICmpSGE(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case FGreaterThanEqual: {
                    make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateFCmpUGE(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case IMod: {
                    make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateSRem(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case FMod: {
                    make_binary(function, istr, [&](llvm::Value* a, llvm::Value* b) { return m_builder.CreateFRem(a, b, op); }, m_defaultFloatT, istr_id);
                    break;
                }
                case IAbs: {
                    make_unary(function, istr, [&](llvm::Value* a) { return m_builder.CreateIntrinsic(llvm::Intrinsic::abs, a); }, m_defaultIntT, istr_id);
                    break;
                }
                case FAbs: {
                    make_unary(function, istr, [&](llvm::Value* a) { return m_builder.CreateIntrinsic(llvm::Intrinsic::fabs, a); }, m_defaultFloatT, istr_id);
                    break;
                }
                case AssertPointer: {
                    m_types[istr.destination] = m_defaultPointerT;
                    llvm::Value* ptr = load_register(function, istr.destination, std::format("{}_ptr", istr_id));
                    m_builder.CreateIsNotNull(ptr, op);
                    break;
                }
                case LoadU16Imm: {
                    const u16 val = istr.operand1 | (istr.operand2 << 8);
                    m_types[istr.destination] = m_defaultIntT;
                    llvm::Constant* value = llvm::ConstantInt::get(m_defaultIntT, val, false);
                    make_store(function, istr.destination, value);
                    break;
                }
                case LoadStaticU8Imm: {
                    make_symbol_table_load<u8>(function, istr, istr_id);
                    break;
                }
                case LoadStaticU16Imm: {
                    make_symbol_table_load<u16>(function, istr, istr_id);
                    break;
                }
                case LoadStaticU32Imm: {
                    make_symbol_table_load<u32>(function, istr, istr_id);
                    break;
                }
                case LoadStaticU64Imm: {
                    make_symbol_table_load<u64>(function, istr, istr_id);
                    break;
                }
                case LoadStaticI8Imm: {
                    make_symbol_table_load<i8>(function, istr, istr_id);
                    break;
                }
                case LoadStaticI16Imm: {
                    make_symbol_table_load<i16>(function, istr, istr_id);
                    break;
                }
                case LoadStaticI32Imm: {
                    make_symbol_table_load<i32>(function, istr, istr_id);
                    break;
                }
                case LoadStaticI64Imm: {
                    make_symbol_table_load<i64>(function, istr, istr_id);
                    break;
                }
                case LoadStaticFloatImm: {
                    make_symbol_table_load<f32>(function, istr, istr_id);
                    break;
                }
                case LoadStaticInt: {
                    llvm::Value* idx = load_register(function, istr.operand1, std::format("{}_idx", istr_id));
                    llvm::Value* value = m_builder.CreateIntrinsic(llvm::Intrinsic::dcvm_load_static_int, {idx}, {}, std::format("{}static_int", istr_id));
                    make_store(function, istr.destination, value, m_defaultIntT);
                    break;
                }
                case LoadStaticFloat: {
                    llvm::Value* idx = load_register(function, istr.operand1, std::format("{}_idx", istr_id));
                    llvm::Value* value = m_builder.CreateIntrinsic(llvm::Intrinsic::dcvm_load_static_float, {idx}, {}, std::format("{}static_float", istr_id));
                    make_store(function, istr.destination, value, m_defaultFloatT);
                    break;
                }
                case LoadStaticPointer: {
                    llvm::Value* idx = load_register(function, istr.operand1, std::format("{}_idx", istr_id));
                    llvm::Value* value = m_builder.CreateIntrinsic(llvm::Intrinsic::dcvm_load_static_pointer, {idx}, {}, std::format("{}static_pointer", istr_id));
                    make_store(function, istr.destination, value, m_defaultPointerT);
                    break;
                }
                case LoadU8: {
                    make_mem_load<u8>(function, istr, istr_id);
                    break;
                }
                case LoadU16: {
                    make_mem_load<u16>(function, istr, istr_id);
                    break;
                }
                case LoadU32: {
                    make_mem_load<u32>(function, istr, istr_id);
                    break;
                }
                case LoadU64: {
                    make_mem_load<u64>(function, istr, istr_id);
                    break;
                }
                case LoadI8: {
                    make_mem_load<i8>(function, istr, istr_id);
                    break;
                }
                case LoadI16: {
                    make_mem_load<i16>(function, istr, istr_id);
                    break;
                }
                case LoadI32: {
                    make_mem_load<i32>(function, istr, istr_id);
                    break;
                }
                case LoadI64: {
                    make_mem_load<i64>(function, istr, istr_id);
                    break;
                }
                case LoadFloat: {
                    make_mem_load<f32>(function, istr, istr_id);
                    break;
                }
                case LoadPointer: {
                    make_mem_load<const void*>(function, istr, istr_id);
                    break;
                }
                case StoreI8: {
                    make_mem_store<i8>(function, istr, istr_id);
                    break;
                }
                case StoreU8: {
                    make_mem_store<u8>(function, istr, istr_id);
                    break;
                }
                case StoreI16: {
                    make_mem_store<i16>(function, istr, istr_id);
                    break;
                }
                case StoreU16: {
                    make_mem_store<u16>(function, istr, istr_id);
                    break;
                }
                case StoreI32: {
                    make_mem_store<i32>(function, istr, istr_id);
                    break;
                }
                case StoreU32: {
                    make_mem_store<u32>(function, istr, istr_id);
                    break;
                }
                case StoreI64: {
                    make_mem_store<i64>(function, istr, istr_id);
                    break;
                }
                case StoreU64: {
                    make_mem_store<u64>(function, istr, istr_id);
                    break;
                }
                case StorePointer: {
                    make_mem_store<const void*>(function, istr, istr_id);
                    break;
                }
                case CastInteger: {
                    llvm::Value* current_float = load_register(function, istr.operand1);

                    assert(m_types[istr.operand1] == m_defaultFloatT);

                    llvm::Value* cast = m_builder.CreateFPToUI(current_float, m_defaultIntT);
                    m_types[istr.destination] = m_defaultIntT;
                    break;
                }
                case CastFloat: {
                    llvm::Value* current_int = load_register(function, istr.operand1);

                    assert(m_types[istr.operand1] == m_defaultIntT);

                    llvm::Value* cast = m_builder.CreateUIToFP(current_int, m_defaultFloatT);
                    m_types[istr.destination] = m_defaultFloatT;
                    break;
                }
                case LoadStaticPointerImm: {
                    const ast::full_type& type = m_disasm->m_stackFrame.m_symbolTable.m_types[istr.operand1];
                    if (const ast::primitive_type* string_value = std::get_if<ast::primitive_type>(&type)) {
                        assert(string_value->m_type == ast::primitive_kind::STRING);

                        llvm::Value* string_pointer = m_builder.CreateIntrinsic(llvm::Intrinsic::dcvm_static_pointer, {m_builder.getInt64(istr.operand1)});
                        make_store(function, istr.destination, string_pointer, m_defaultPointerT);
                    } else {
                        make_symbol_table_load<const void*>(function, istr, istr_id);
                    }
                    break;
                }
                case Move: {
                    make_store(function, istr.destination, load_register(function, istr.operand1, std::format("{}_load_op1", istr_id)), m_types[istr.operand1]);
                    break;
                }
                case LookupPointer: {
                    const ast::full_type& symbol_table_funtion_t = m_disasm->m_stackFrame.m_symbolTable.m_types[istr.operand1];
                    assert(std::holds_alternative<ast::function_type>(symbol_table_funtion_t));
                    const ast::function_type& ast_function_t = std::get<ast::function_type>(symbol_table_funtion_t);
                    llvm::FunctionType* function_type = llvm::cast<llvm::FunctionType>(ast_type_to_llvm_type(m_ctx, symbol_table_funtion_t));

                    const sid64 func_sid = m_symbolTable->get<sid64>(istr.operand1 * sizeof(u64));
                    const char* function_name = m_sidbase.lookup(func_sid);
                    const ast::function_type::DISTANCE distance = ast_function_t.m_distanceType;

                    llvm::FunctionCallee callee_function = m_module.getOrInsertFunction(function_name, function_type);
                    llvm::Function* F = llvm::cast<llvm::Function>(callee_function.getCallee());
                    F->setMetadata("dcvm.sid_distance", llvm::MDNode::get(m_ctx, {
                        llvm::MDString::get(m_ctx, "sid"),
                        llvm::ConstantAsMetadata::get(m_builder.getInt64(func_sid)),
                        llvm::MDString::get(m_ctx, "distance"),
                        llvm::MDString::get(m_ctx, distance == ast::function_type::DISTANCE::NEAR ? "near" : "far"),
                    }));
                    F->setDoesNotThrow();
                    F->setWillReturn();
                    F->setNoSync();

                    make_store(function, istr.destination, callee_function.getCallee(), function_type);
                    break;
                }
                case CallFf:
                case Call: {
                    llvm::FunctionType* func_type = llvm::cast<llvm::FunctionType>(m_types[istr.operand1]);
                    llvm::Value* callee = load_register(function, istr.operand1, std::format("{}_callee", istr_id));
                    std::vector<llvm::Value*> arg_values;
                    arg_values.reserve(istr.operand2);
                    for (u64 i = 0; i < istr.operand2; ++i) {
                        arg_values.push_back(load_register(function, ARGUMENT_REGISTERS_IDX + i, std::format("{}_arg{}", istr_id, i)));
                        if (i < func_type->getNumParams() && arg_values.back()->getType() != func_type->getParamType(i)) {
                            func_type = exchange_function_types(function, arg_values.back()->getType(), istr.operand1, i);
                        }
                    }
                    llvm::CallInst* call = m_builder.CreateCall(func_type, callee, llvm::ArrayRef(arg_values.data(), arg_values.size()), std::format("{}_call", istr_id));
                    if (istr.opcode == CallFf) {
                        call->setMetadata("dcvm.distance", llvm::MDNode::get(m_ctx, {llvm::MDString::get(m_ctx, "far")}));
                    }
                    make_store(function, istr.destination, call, func_type->getReturnType());
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
                        current_node = m_graph->get_node_with_start_line(l + 1);
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
                        current_node = m_graph->get_node_with_start_line(l + 1);
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
                        current_node = m_graph->get_node_with_start_line(l + 1);
                        assert(current_node != nullptr);
                        break;
                    }
                    case Return: {
                        llvm::Value* ret_value = load_register(function, istr.destination, std::format("{}_retval", istr_id));
                        if (ret_value->getType() != m_function->getReturnType() && ret_value->getType()->isIntegerTy() && m_function->getReturnType()->isIntegerTy()) {
                            ret_value = m_builder.CreateIntCast(ret_value, m_function->getReturnType(), false, std::format("{}_retcast", istr_id));
                        }
                        m_builder.CreateRet(ret_value);
                        break;
                    }
                    default: {
                        m_builder.CreateBr(m_blocks.at(l + 1));
                        m_builder.SetInsertPoint(m_blocks.at(l + 1));
                        current_node = m_graph->get_node_with_start_line(l + 1);
                        assert(current_node != nullptr);
                        break;
                    }
                }
            }
        }

        return m_function;
    }

    std::vector<generated_outputs> llvm_transpiler::run() {
        std::vector<generated_outputs> outputs;
        outputs.reserve(m_translationUnits.size());

        for (translation_unit& unit : m_translationUnits) {
            llvm::Module& module = *unit.m_module;

            for (lift_function& function : unit.m_functions) {
                prepare_function(module, function);
            }

            for (lift_function& function : unit.m_functions) {
                (void)lift_prepared_function(module, function);
            }

            outputs.push_back(generate_outputs(module));
        }

        return outputs;
    }

    generated_outputs llvm_transpiler::generate_outputs(llvm::Module& module) {
        generated_outputs outputs;
        outputs.m_moduleName = module.getName().str();
        outputs.m_llvmUnopt = emit_llvm_ir(module);

        optimize_module(module);
        lower_calls_to_lookup(module);
        optimize_module(module);

        outputs.m_llvmOpt = emit_llvm_ir(module);
        outputs.m_dcvmAsm = emit_dcvm_asm(module);
        return outputs;
    }

    static std::string safe_output_name(std::string name) {
        if (name.empty()) {
            return "module";
        }
        std::ranges::replace_if(name, [](char c) { return std::string_view("/\\:*?\"<>|").contains(c); }, '_');
        return name;
    }

    errmsg llvm_transpiler::write_outputs(const generated_outputs& outputs, const std::filesystem::path& output_dir) {
        std::error_code ec;
        std::filesystem::create_directories(output_dir, ec);
        if (ec) {
            return std::format("couldn't create output directory {}: {}", output_dir.string(), ec.message());
        }

        const std::string module_name = safe_output_name(outputs.m_moduleName);
        const std::array<std::pair<std::filesystem::path, const std::string*>, 3> text_outputs = {{
            {output_dir / std::format("{}_unopt.ll", module_name), &outputs.m_llvmUnopt},
            {output_dir / std::format("{}_opt.ll", module_name), &outputs.m_llvmOpt},
            {output_dir / std::format("{}.dcvm.s", module_name), &outputs.m_dcvmAsm},
        }};

        for (const auto& [path, content] : text_outputs) {
            std::ofstream file(path);
            if (!file) {
                return "couldn't open output file " + path.string();
            }
            file << *content;
        }

        const std::filesystem::path bytecode_path = output_dir / std::format("{}.dcvm.bin", module_name);
        std::ofstream bytecode_file(bytecode_path, std::ios::binary);
        if (!bytecode_file) {
            return "couldn't open output file " + bytecode_path.string();
        }
        bytecode_file.write(outputs.m_dcvmBytecode.data(), static_cast<std::streamsize>(outputs.m_dcvmBytecode.size()));

        return std::nullopt;
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

    llvm::Value* llvm_transpiler::load_register(lift_function& function, const u8 index, std::string_view istr_name) {
        llvm::Value* raw = m_builder.CreateLoad(m_defaultIntT, function.m_registerFrame[index], istr_name);
        return coerce_from_storage(raw, function.m_types[index]);
    }

    llvm::StoreInst* llvm_transpiler::make_store(lift_function& function, const u8 index, llvm::Value* rhs, llvm::Type* type) {
        if (type) {
            function.m_types[index] = type;
        }
        return m_builder.CreateStore(coerce_to_storage(rhs), function.m_registerFrame[index]);
    }

    llvm::StoreInst* llvm_transpiler::make_binary(lift_function& function, const Instruction& istr, std::function<llvm::Value*(llvm::Value*, llvm::Value*)> binary_op, llvm::Type* dest_type, std::string_view istr_id) {
        llvm::Value* lhs = load_register(function, istr.operand1, std::format("{}_load_lhs", istr_id));
        llvm::Value* rhs = load_register(function, istr.operand2, std::format("{}_load_rhs", istr_id));
        if (lhs->getType() != rhs->getType() && (lhs->getType() != m_defaultPointerT && rhs->getType() != m_defaultPointerT)) {
            rhs = m_builder.CreateZExtOrTrunc(rhs, lhs->getType());
        }
        llvm::Value* res = binary_op(lhs, rhs);
        function.m_types[istr.destination] = dest_type;
        return make_store(function, istr.destination, res);
    }

    llvm::StoreInst* llvm_transpiler::make_binary_imm(lift_function& function, const Instruction& istr, std::function<llvm::Value*(llvm::Value*, llvm::Value*)> binary_op, llvm::Type* dest_type, std::string_view istr_id) {
        llvm::Value* lhs = load_register(function, istr.operand1, std::format("{}_load_lhs", istr_id));
        llvm::Type* imm_type = lhs->getType()->isPointerTy() ? m_builder.getInt8Ty() : lhs->getType();
        llvm::Value* imm =  llvm::ConstantInt::get(imm_type, istr.operand2);
        llvm::Value* res = binary_op(lhs, imm);
        function.m_types[istr.destination] = dest_type;
        return make_store(function, istr.destination, res);
    }

    llvm::StoreInst* llvm_transpiler::make_unary(lift_function& function, const Instruction& istr, std::function<llvm::Value*(llvm::Value*)> unary_op, llvm::Type* dest_type, std::string_view istr_id) {
        llvm::Value* lhs = load_register(function, istr.operand1, std::format("{}_load_op1", istr_id));
        llvm::Value* res = unary_op(lhs);
        function.m_types[istr.destination] = dest_type;
        return make_store(function, istr.destination, res);
    }

    llvm::FunctionType* llvm_transpiler::exchange_function_types(lift_function& function, llvm::Type* new_arg_type, const u8 type_location, const u8 arg_idx) {
        llvm::FunctionType* old_f_type = llvm::cast<llvm::FunctionType>(function.m_types[type_location]);
        std::vector<llvm::Type*> new_arg_types;
        new_arg_types.reserve(old_f_type->getNumParams());
        for (u32 i = 0; i < old_f_type->getNumParams(); ++i) {
            new_arg_types.push_back(old_f_type->getParamType(i));
        }
        new_arg_types[arg_idx] = new_arg_type;
        llvm::FunctionType* new_f_type = llvm::FunctionType::get(old_f_type->getReturnType(), llvm::ArrayRef(new_arg_types.data(), new_arg_types.size()), old_f_type->isVarArg());
        function.m_types[type_location] = new_f_type;
        return new_f_type;
    }

    errmsg transpile_functions_to_llvm(llvm::Module& module, const std::vector<const function_disassembly*>& funcs, const SIDBase& sidbase) {
        llvm_transpiler t(sidbase);
        for (const function_disassembly* func : funcs) {
            resstr<llvm::Function*> res = t.transpile_function(module, *func);
            if (!res.has_value()) {
                return res.error();
            }
        }
        return std::nullopt;
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

        llvm::Triple triple("dcvm-tlou2-llvm_transpiler100");
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

    std::unique_ptr<llvm::Module> llvm_transpiler::create_runtime_module(llvm::LLVMContext& ctx) {
        std::unique_ptr module = std::make_unique<llvm::Module>("__dcvm_runtime", ctx);

        llvm::IRBuilder<> builder(ctx);
        { // is-final-build?()
            constexpr bool is_final_build_val = true;
            llvm::FunctionType* is_final_build_t = llvm::FunctionType::get(llvm::Type::getInt1Ty(ctx), {}, false);
            llvm::Function* is_final_build_f = llvm::Function::Create(is_final_build_t, llvm::GlobalValue::LinkageTypes::ExternalLinkage, 0, "is-final-build?", module.get());
            llvm::BasicBlock* block = llvm::BasicBlock::Create(ctx, "bb", is_final_build_f);
            builder.SetInsertPoint(block, block->begin());
            llvm::Value* ret_value = builder.getInt1(is_final_build_val);
            builder.CreateRet(ret_value);
        }

        return module;
    }
}
