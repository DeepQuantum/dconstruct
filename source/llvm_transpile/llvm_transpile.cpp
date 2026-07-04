#include "llvm_transpile/llvm_transpile.h"
#include "DCScript.h"
#include "ast/type.h"
#include "base.h"
#include "binaryfile.h"
#include "decompilation/control_flow_graph.h"
#include "disassembly/instructions.h"
#include "disassembly/opcodes.h"
#include "sidbase.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/IR/Attributes.h"
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
#include <cstdint>
#include <initializer_list>
#include <llvm/IR/Verifier.h>
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Type.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/IR/Value.h"
#include "llvm/Bitcode/BitcodeReader.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/Alignment.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/MemoryBufferRef.h"
#include "llvm/Support/ModRef.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/TargetParser/Triple.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/IntrinsicsDCVM.h"
#include <algorithm>
#include <array>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>
#include <format>
#include <print>
#include <execution>

using namespace std::literals;

extern "C" void LLVMInitializeCBackendTarget();
extern "C" void LLVMInitializeCBackendTargetInfo();
extern "C" void LLVMInitializeCBackendTargetMC();

extern "C" void LLVMInitializeDCVMTargetInfo();
extern "C" void LLVMInitializeDCVMTarget();
extern "C" void LLVMInitializeDCVMTargetMC();
extern "C" void LLVMInitializeDCVMAsmPrinter();
extern "C" void DCVMSetBytecodeSink(llvm::TargetMachine* machine, std::vector<char>* sink);

namespace dconstruct::llvm_transpile {

    struct recompiled_function_info {
        sid64 m_sid = 0;
        std::vector<Instruction> m_instructions;
        std::vector<u64> m_symbols;
        std::vector<bool> m_pointerMap;
    };

    struct recompiled_function_header {
        sid64 m_sid = 0;
        u32 m_sizeInstructions = 0;
        u32 m_sizeSymbols = 0;
    };

    static_assert(sizeof(Instruction) == sizeof(u64));
    static_assert(sizeof(recompiled_function_header) == sizeof(u64) * 2);

    bool range_inside_file(const BinaryFile& binfile, const void* ptr, const u64 size) {
        const p64 begin = reinterpret_cast<p64>(binfile.m_bytes.get());
        const p64 end = begin + binfile.m_size;
        const p64 range_begin = reinterpret_cast<p64>(ptr);
        const p64 range_end = range_begin + size;
        return range_begin >= begin && range_begin <= end && range_end >= range_begin && range_end <= end;
    }

    void edit_unmapped_reloc_table(std::byte* bytes, const u64 text_size, const u64 file_offset, const bool value) {
        u8* reloc_table = reinterpret_cast<u8*>(bytes + text_size + sizeof(u32));
        const u64 offset = file_offset / sizeof(u64);
        u8& reloc_byte = reloc_table[offset / 8];
        const u8 mask = static_cast<u8>(1 << (offset % 8));
        if (value) {
            reloc_byte |= mask;
        } else {
            reloc_byte &= static_cast<u8>(~mask);
        }
    }

    struct function_patch {
        const function_disassembly* m_oldFunc = nullptr;
        const recompiled_function_info* m_newFunc = nullptr;
    };

    template <typename T> bool read_bytecode_value(const std::vector<char>& bytes, u64& offset, T& out) {
        if (bytes.size() - offset < sizeof(T)) {
            return false;
        }
        std::memcpy(&out, bytes.data() + offset, sizeof(T));
        offset += sizeof(T);
        return true;
    }

    errmsg validate_recompiled_symbol_map(const recompiled_function_info& function, const std::string& module_name) {
        if (function.m_symbols.size() != function.m_pointerMap.size()) {
            return std::format("symbol pointer map for sid {:x} in {} has {} entries for {} symbols", function.m_sid, module_name, function.m_pointerMap.size(), function.m_symbols.size());
        }

        return std::nullopt;
    }

    errmsg copy_recompiled_symbols_to_mapped_file(BinaryFile& binfile, u64* destination, const recompiled_function_info& new_func) {
        if (errmsg error = validate_recompiled_symbol_map(new_func, binfile.m_path.string())) {
            return error;
        }

        const u64 text_size = binfile.m_dcheader->m_textSize;
        const p64 mapped_base = reinterpret_cast<p64>(binfile.m_bytes.get());
        for (u64 i = 0; i < new_func.m_symbols.size(); ++i) {
            u64 value = new_func.m_symbols[i];
            const bool is_pointer = new_func.m_pointerMap[i] && value != 0;
            if (is_pointer) {
                if (value >= text_size) {
                    return std::format("recompiled symbol {} for sid {:x} points outside the text segment of {}", i, new_func.m_sid, binfile.m_path.string());
                }
                value += mapped_base;
            }

            destination[i] = value;
            binfile.edit_reloc_table(location(destination + i), is_pointer);
        }

        return std::nullopt;
    }

    errmsg copy_recompiled_symbols_to_unmapped_file(
        std::byte* bytes,
        const u64 new_text_size,
        const u64 symbol_offset,
        const recompiled_function_info& new_func,
        const u64 old_strings_start,
        const u64 old_text_size,
        const u64 inserted_size,
        const std::filesystem::path& file_path
    ) {
        if (errmsg error = validate_recompiled_symbol_map(new_func, file_path.string())) {
            return error;
        }

        for (u64 i = 0; i < new_func.m_symbols.size(); ++i) {
            u64 value = new_func.m_symbols[i];
            const bool is_pointer = new_func.m_pointerMap[i] && value != 0;
            if (is_pointer) {
                if (value >= old_text_size) {
                    return std::format("recompiled symbol {} for sid {:x} points outside the original text segment of {}", i, new_func.m_sid, file_path.string());
                }
                if (value >= old_strings_start) {
                    value += inserted_size;
                }
            }

            const u64 entry_offset = symbol_offset + i * sizeof(u64);
            std::memcpy(bytes + entry_offset, &value, sizeof(value));
            edit_unmapped_reloc_table(bytes, new_text_size, entry_offset, is_pointer);
        }

        return std::nullopt;
    }

    resstr<std::vector<recompiled_function_info>> parse_recompiled_functions(const generated_outputs& outputs) {
        const std::vector<char>& bytes = outputs.m_dcvmBytecode;
        std::vector<recompiled_function_info> functions;
        u64 offset = 0;

        while (offset < bytes.size()) {
            recompiled_function_header header;
            if (!read_bytecode_value(bytes, offset, header)) {
                return std::unexpected(std::format("truncated bytecode function header in {}", outputs.m_moduleName));
            }

            if (header.m_sizeInstructions % sizeof(Instruction) != 0) {
                return std::unexpected(std::format("invalid instruction byte count for sid {:x} in {}", header.m_sid, outputs.m_moduleName));
            }
            if (header.m_sizeSymbols % sizeof(u64) != 0) {
                return std::unexpected(std::format("invalid symbol byte count for sid {:x} in {}", header.m_sid, outputs.m_moduleName));
            }

            const u64 instruction_count = header.m_sizeInstructions / sizeof(Instruction);
            const u64 symbol_count = header.m_sizeSymbols / sizeof(u64);
            const u64 payload_size = static_cast<u64>(header.m_sizeInstructions) + header.m_sizeSymbols + symbol_count;
            if (bytes.size() - offset < payload_size) {
                return std::unexpected(std::format("truncated bytecode payload for sid {:x} in {}", header.m_sid, outputs.m_moduleName));
            }

            recompiled_function_info function;
            function.m_sid = header.m_sid;
            function.m_instructions.resize(instruction_count);
            function.m_symbols.resize(symbol_count);
            function.m_pointerMap.reserve(symbol_count);

            const u64 instruction_bytes = instruction_count * sizeof(Instruction);
            if (instruction_bytes != 0) {
                std::memcpy(function.m_instructions.data(), bytes.data() + offset, instruction_bytes);
                offset += instruction_bytes;
            }

            const u64 symbol_bytes = symbol_count * sizeof(u64);
            if (symbol_bytes != 0) {
                std::memcpy(function.m_symbols.data(), bytes.data() + offset, symbol_bytes);
                offset += symbol_bytes;
            }

            for (u64 i = 0; i < symbol_count; ++i) {
                function.m_pointerMap.push_back(bytes[offset] != 0);
                ++offset;
            }

            if (errmsg error = validate_recompiled_symbol_map(function, outputs.m_moduleName)) {
                return std::unexpected(std::move(*error));
            }

            functions.push_back(std::move(function));
        }

        return functions;
    }

    const generated_outputs* find_outputs_for_module(const std::vector<generated_outputs>& outputs, const std::string& module_name) {
        for (const generated_outputs& output : outputs) {
            if (output.m_moduleName == module_name) {
                return &output;
            }
        }
        return nullptr;
    }

    errmsg patch_recompiled_function(BinaryFile& binfile, const function_disassembly& old_func, const recompiled_function_info& new_func) {
        const u64 old_symbol_count = old_func.m_stackFrame.m_symbolTable.m_types.size();

        if (old_func.m_originalOffset + sizeof(ScriptLambda) > binfile.m_size) {
            return std::format("original lambda header for {} is outside {}", old_func.get_id(), binfile.m_path.string());
        }

        ScriptLambda& original_function = *reinterpret_cast<ScriptLambda*>(binfile.m_bytes.get() + old_func.m_originalOffset);

        const u64 instruction_bytes = new_func.m_instructions.size() * sizeof(Instruction);
        const u64 symbol_bytes = new_func.m_symbols.size() * sizeof(u64);
        if (!range_inside_file(binfile, original_function.m_pInstruction, instruction_bytes)) {
            return std::format("instruction storage for {} is outside {}", old_func.get_id(), binfile.m_path.string());
        }

        original_function.m_pSymbols = original_function.m_pInstruction + new_func.m_instructions.size();
        if (!range_inside_file(binfile, original_function.m_pSymbols, symbol_bytes)) {
            return std::format("symbol storage for {} is outside {}", old_func.get_id(), binfile.m_path.string());
        }

        const location old_symbols = old_func.m_stackFrame.m_symbolTable.m_location;
        if (!range_inside_file(binfile, old_symbols.as<std::byte>(), old_symbol_count * sizeof(u64))) {
            return std::format("old symbol table for {} is outside {}", old_func.get_id(), binfile.m_path.string());
        }

        original_function.m_numInstructions = static_cast<u32>(new_func.m_instructions.size());
        original_function.m_sum = 12 + 4 * (new_func.m_instructions.size() + new_func.m_symbols.size());

        location old_symbols_reloc = old_symbols;
        for (u64 i = 0; i < old_symbol_count; ++i) {
            binfile.edit_reloc_table(old_symbols_reloc, false);
            old_symbols_reloc += sizeof(u64);
        }

        if (instruction_bytes != 0) {
            std::memcpy(original_function.m_pInstruction, new_func.m_instructions.data(), instruction_bytes);
        }
        if (symbol_bytes != 0) {
            if (errmsg error = copy_recompiled_symbols_to_mapped_file(binfile, original_function.m_pSymbols, new_func)) {
                return error;
            }
        }

        return std::nullopt;
    }

    errmsg patch_large_recompiled_functions(BinaryFile& binfile, const std::vector<function_patch>& large_patches) {
        if (large_patches.empty()) {
            return std::nullopt;
        }

        struct scheduled_patch {
            const function_disassembly* m_oldFunc = nullptr;
            const recompiled_function_info* m_newFunc = nullptr;
            u64 m_instructionOffset = 0;
            u64 m_symbolOffset = 0;
        };

        const u64 old_text_size = binfile.m_dcheader->m_textSize;
        const u32 old_reloc_size = *reinterpret_cast<const u32*>(binfile.m_bytes.get() + old_text_size);
        const u64 old_reloc_end = old_text_size + sizeof(u32) + old_reloc_size;
        if (old_reloc_end > binfile.m_size) {
            return std::format("reloc table for {} is outside the file", binfile.m_path.string());
        }

        const u64 old_strings_start = binfile.m_dcheader->m_stringsOffset;
        if (old_strings_start < sizeof(DC_Header) || old_strings_start > old_text_size) {
            return std::format("string table for {} is outside the text segment", binfile.m_path.string());
        }

        const u64 old_slot_count = u64{old_reloc_size} * 8;
        for (u64 slot = old_strings_start / sizeof(u64); slot < old_slot_count; ++slot) {
            if (slot * sizeof(u64) >= old_strings_start && binfile.m_relocTable.get<u8>(slot / 8) & (1 << (slot % 8))) {
                return std::format("string table region for {} contains a relocated pointer at {:#x}", binfile.m_path.string(), slot * sizeof(u64));
            }
        }

        u64 payload_cursor = old_strings_start;
        std::vector<scheduled_patch> scheduled;
        scheduled.reserve(large_patches.size());

        for (const function_patch& patch : large_patches) {
            const u64 instruction_size = patch.m_newFunc->m_instructions.size() * sizeof(Instruction);
            const u64 symbol_size = patch.m_newFunc->m_symbols.size() * sizeof(u64);
            scheduled.push_back({
                patch.m_oldFunc,
                patch.m_newFunc,
                payload_cursor,
                payload_cursor + instruction_size,
            });
            payload_cursor += instruction_size + symbol_size;
        }

        const u64 inserted_size = payload_cursor - old_strings_start;
        const u64 new_strings_start = old_strings_start + inserted_size;
        const u64 old_strings_size = old_text_size - old_strings_start;
        const u64 new_text_size = new_strings_start + old_strings_size;
        if (new_text_size > std::numeric_limits<u32>::max()) {
            return std::format("expanded text section for {} is too large", binfile.m_path.string());
        }
        const u32 new_reloc_size = static_cast<u32>((new_text_size + 63) / 64);
        const u64 tail_size = binfile.m_size - old_reloc_end;
        const u64 new_file_size = new_text_size + sizeof(u32) + new_reloc_size + tail_size;

        BinaryFile::byte_uptr old_unmapped = binfile.get_unmapped();
        std::byte* new_bytes_raw = static_cast<std::byte*>(::operator new[](new_file_size, std::align_val_t(64)));
        BinaryFile::byte_uptr new_unmapped(new_bytes_raw);
        std::memset(new_unmapped.get(), 0, new_file_size);

        std::memcpy(new_unmapped.get(), old_unmapped.get(), old_strings_start);
        std::memcpy(new_unmapped.get() + new_strings_start, old_unmapped.get() + old_strings_start, old_strings_size);
        std::memcpy(new_unmapped.get() + new_text_size + sizeof(u32), old_unmapped.get() + old_text_size + sizeof(u32), old_reloc_size);
        if (tail_size != 0) {
            std::memcpy(new_unmapped.get() + new_text_size + sizeof(u32) + new_reloc_size, old_unmapped.get() + old_reloc_end, tail_size);
        }

        DC_Header* header = reinterpret_cast<DC_Header*>(new_unmapped.get());
        header->m_textSize = static_cast<u32>(new_text_size);
        header->m_stringsOffset = static_cast<u32>(new_strings_start);
        *reinterpret_cast<u32*>(new_unmapped.get() + new_text_size) = new_reloc_size;

        const u8* old_reloc = reinterpret_cast<const u8*>(old_unmapped.get() + old_text_size + sizeof(u32));
        for (u64 slot = 0; slot < old_strings_start / sizeof(u64); ++slot) {
            if (!(old_reloc[slot / 8] & (1 << (slot % 8)))) {
                continue;
            }

            u64 value = 0;
            std::memcpy(&value, new_unmapped.get() + slot * sizeof(u64), sizeof(value));
            if (value >= old_strings_start && value < old_text_size) {
                value += inserted_size;
                std::memcpy(new_unmapped.get() + slot * sizeof(u64), &value, sizeof(value));
            }
        }

        for (const scheduled_patch& patch : scheduled) {
            const function_disassembly& old_func = *patch.m_oldFunc;
            const recompiled_function_info& new_func = *patch.m_newFunc;
            const u64 instruction_size = new_func.m_instructions.size() * sizeof(Instruction);
            const u64 symbol_size = new_func.m_symbols.size() * sizeof(u64);

            if (instruction_size != 0) {
                std::memcpy(new_unmapped.get() + patch.m_instructionOffset, new_func.m_instructions.data(), instruction_size);
            }
            if (symbol_size != 0) {
                if (errmsg error =
                        copy_recompiled_symbols_to_unmapped_file(new_unmapped.get(), new_text_size, patch.m_symbolOffset, new_func, old_strings_start, old_text_size, inserted_size, binfile.m_path)) {
                    return error;
                }
            }

            ScriptLambda& lambda = *reinterpret_cast<ScriptLambda*>(new_unmapped.get() + old_func.m_originalOffset);
            lambda.m_pInstruction = reinterpret_cast<u64*>(patch.m_instructionOffset);
            lambda.m_pSymbols = reinterpret_cast<u64*>(patch.m_symbolOffset);
            lambda.m_numInstructions = static_cast<u32>(new_func.m_instructions.size());
            lambda.m_sum = 12 + 4 * (new_func.m_instructions.size() + new_func.m_symbols.size());

            edit_unmapped_reloc_table(new_unmapped.get(), new_text_size, old_func.m_originalOffset, true);
            edit_unmapped_reloc_table(new_unmapped.get(), new_text_size, old_func.m_originalOffset + sizeof(u64), true);

            const u64 old_symbol_offset = reinterpret_cast<p64>(old_func.m_stackFrame.m_symbolTable.m_location.as<std::byte>()) - reinterpret_cast<p64>(binfile.m_bytes.get());
            const u64 old_symbol_count = old_func.m_stackFrame.m_symbolTable.m_types.size();
            for (u64 i = 0; i < old_symbol_count; ++i) {
                edit_unmapped_reloc_table(new_unmapped.get(), new_text_size, old_symbol_offset + i * sizeof(u64), false);
            }
        }

        binfile.replace_with_unmapped(new_file_size, std::move(new_unmapped));
        return std::nullopt;
    }

    llvm_transpiler::llvm_transpiler(const SIDBase& sidbase, const std::filesystem::path* excluded) : m_sidbase(sidbase), m_ctx(llvm::LLVMContext()), m_excludedPath(excluded) {
        m_defaultIntT = llvm::Type::getInt64Ty(m_ctx);
        m_defaultFloatT = llvm::Type::getFloatTy(m_ctx);
        m_boolT = llvm::Type::getInt1Ty(m_ctx);
        m_defaultPointerT = llvm::PointerType::get(m_ctx, 0);
    }

    void map_referenced_globals_for_clone(llvm::Module& dst_module, const llvm::Function& src_function, llvm::ValueToValueMapTy& vmap) {
        const auto map_value = [&](const auto& self, const llvm::Value* value) -> void {
            if (const auto* function = llvm::dyn_cast<llvm::Function>(value)) {
                if (!vmap.lookup(function)) {
                    llvm::FunctionCallee callee = dst_module.getOrInsertFunction(function->getName(), function->getFunctionType());
                    auto* dst_function = llvm::cast<llvm::Function>(callee.getCallee()->stripPointerCasts());
                    vmap[function] = dst_function;
                    dst_function->copyAttributesFrom(function);
                    dst_function->setCallingConv(function->getCallingConv());
                }
                return;
            }

            if (const auto* global = llvm::dyn_cast<llvm::GlobalVariable>(value)) {
                if (!vmap.lookup(global)) {
                    llvm::GlobalVariable* dst_global = dst_module.getNamedGlobal(global->getName());
                    if (!dst_global) {
                        dst_global = new llvm::GlobalVariable(dst_module, global->getValueType(), global->isConstant(), global->getLinkage(), nullptr, global->getName());
                        if (global->hasInitializer()) {
                            dst_global->setInitializer(const_cast<llvm::Constant*>(global->getInitializer()));
                        }
                        dst_global->setUnnamedAddr(global->getUnnamedAddr());
                        dst_global->setAlignment(global->getAlign());
                    }
                    vmap[global] = dst_global;
                }
                return;
            }

            const auto* constant = llvm::dyn_cast<llvm::Constant>(value);
            if (!constant) {
                return;
            }

            for (const llvm::Use& operand : constant->operands()) {
                self(self, operand.get());
            }
        };

        for (const llvm::BasicBlock& block : src_function) {
            for (const llvm::Instruction& instruction : block) {
                for (const llvm::Use& operand : instruction.operands()) {
                    map_value(map_value, operand.get());
                }
            }
        }
    }

    void llvm_transpiler::add_module(std::string_view name, const BinaryFile* binary_file, const std::vector<const function_disassembly*>& funcs) {
        std::vector<prepared_function_ctx> functions;
        functions.reserve(funcs.size());
        for (const function_disassembly* disassembly_function : funcs) {
            functions.emplace_back(disassembly_function, &disassembly_function->m_stackFrame.m_symbolTable.m_location);
        }
        m_translationUnits.emplace_back(std::make_unique<llvm::Module>(name, m_ctx), binary_file, std::move(functions));
    }

    void llvm_transpiler::prepare_function(translation_unit& unit, prepared_function_ctx& to_lift) {
        llvm::Module& module = unit.module();
        const function_disassembly* disasm = to_lift.m_disasm;
        const location* symbol_table = to_lift.m_symbolTable;
        to_lift.m_graph = ControlFlowGraph::build(*disasm);
        ControlFlowGraph* m_graph = &*to_lift.m_graph;
        auto& blocks = to_lift.m_blocks;
        auto& types = to_lift.m_types;
        auto& register_frame = to_lift.m_registerFrame;
        blocks.clear();
        llvm::IRBuilder<> builder(m_ctx);

        std::vector<llvm::Type*> arg_types(disasm->m_stackFrame.m_registerArgs.size(), m_defaultIntT);

        auto* func_type = llvm::FunctionType::get(llvm::Type::getInt64Ty(m_ctx), llvm::ArrayRef(arg_types.data(), arg_types.size()), false);
        llvm::Function* new_function = llvm::Function::Create(func_type, llvm::GlobalValue::LinkageTypes::ExternalLinkage, 0, disasm->get_id(), &module);
        to_lift.m_llvmFunc = new_function;

        m_lookupCallDistances[SID(disasm->get_id().c_str())] = ast::function_type::DISTANCE::NEAR;

        llvm::BasicBlock* entry_block = llvm::BasicBlock::Create(m_ctx, "entry", new_function);
        for (const control_flow_node& node : m_graph->m_nodes) {
            llvm::BasicBlock* block = llvm::BasicBlock::Create(m_ctx, std::format("bb{}", node.m_index), new_function);
            blocks.emplace(node.m_startLine, block);
        }

        builder.SetInsertPoint(entry_block);

        for (u64 i = 0; i < register_frame.size(); ++i) {
            register_frame[i] = builder.CreateAlloca(m_defaultIntT, nullptr, std::format("r{}", i));
            types[i] = m_defaultIntT;
        }

        const u64 symbol_table_size = disasm->m_stackFrame.m_symbolTable.m_types.size();
        to_lift.m_symbolTableArrayT = llvm::ArrayType::get(m_defaultIntT, symbol_table_size);
        llvm::Constant* symbol_table_array = llvm::ConstantDataArray::get(m_ctx, llvm::ArrayRef(symbol_table->as<u64>(), symbol_table_size));
        to_lift.m_symbolTableGlobal =
            new llvm::GlobalVariable(module, to_lift.m_symbolTableArrayT, true, llvm::GlobalValue::PrivateLinkage, symbol_table_array, std::format("{}_symbol_table_src", disasm->get_id()));
        to_lift.m_symbolTableGlobal->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
        to_lift.m_symbolTableGlobal->setAlignment(llvm::Align(sizeof(u64)));
        llvm::appendToUsed(module, {to_lift.m_symbolTableGlobal});

        for (u64 i = 0; i < new_function->arg_size(); ++i) {
            llvm::Argument* arg = new_function->getArg(i);
            arg->setName(std::format("arg_{}", i));
            make_store(builder, to_lift, ARGUMENT_REGISTERS_IDX + i, arg, arg->getType());
        }

        builder.CreateBr(blocks.at(0));

        if (!disasm->m_isScriptFunction) {
            m_funcLocations[disasm->get_id()] = new_function;
        }
    }

    errmsg llvm_transpiler::transpile_function(translation_unit& unit, const function_disassembly& func_disassembly) {
        prepared_function_ctx function{&func_disassembly, &func_disassembly.m_stackFrame.m_symbolTable.m_location};
        prepare_function(unit, function);
        if (errmsg error = lift_prepared_function(unit, function)) {
            return error;
        }
        unit.m_liftedFunctions.push_back(function.m_llvmFunc);
        return std::nullopt;
    }

    errmsg llvm_transpiler::lift_prepared_function(translation_unit& unit, prepared_function_ctx& function) {
        llvm::Module& m_module = unit.module();
        const function_disassembly* m_disasm = function.m_disasm;
        const location* m_symbolTable = function.m_symbolTable;
        ControlFlowGraph* m_graph = &*function.m_graph;
        auto& m_blocks = function.m_blocks;
        auto& m_types = function.m_types;
        auto& m_registerFrame = function.m_registerFrame;
        llvm::Function* m_function = function.m_llvmFunc;

        llvm::IRBuilder<> builder(m_ctx);
        builder.SetInsertPoint(m_blocks.at(0));
        const control_flow_node* current_node = &m_graph->m_nodes[0];
        for (u32 l = 0; l < m_disasm->m_lines.size(); ++l) {
            const function_disassembly_line& line = m_disasm->m_lines[l];
            Instruction istr = line.m_instruction;
            std::string istr_id = std::format("__{}_{}__", istr.opcode_to_string(), l);
            bool terminated = false;
            std::string op = std::format("{}op", istr_id);

            switch (line.m_instruction.opcode) {
                using enum Opcode;
                case IAdd: {
                    if (m_types[istr.operand1]->isPointerTy()) {
                        make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreatePtrAdd(a, b, op); }, m_types[istr.operand1], istr_id);
                    } else if (m_types[istr.operand2]->isPointerTy()) {
                        make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreatePtrAdd(b, a, op); }, m_types[istr.operand1], istr_id);
                    } else {
                        make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateAdd(a, b, op); }, m_defaultIntT, istr_id);
                    }
                    break;
                }
                case FAdd: {
                    make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFAdd(a, b, op); }, m_defaultFloatT, istr_id, m_defaultFloatT);
                    break;
                }
                case ISub: {
                    make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateSub(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case FSub: {
                    make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFSub(a, b, op); }, m_defaultFloatT, istr_id, m_defaultFloatT);
                    break;
                }
                case IMul: {
                    make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateMul(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case FMul: {
                    make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFMul(a, b, op); }, m_defaultFloatT, istr_id, m_defaultFloatT);
                    break;
                }
                case IDiv: {
                    make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateSDiv(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case FDiv: {
                    make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFDiv(a, b, op); }, m_defaultFloatT, istr_id, m_defaultFloatT);
                    break;
                }
                case OpBitAnd: {
                    make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateAnd(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case OpBitOr: {
                    make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateOr(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case OpBitXor: {
                    make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateXor(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case OpBitNot: {
                    make_unary(builder, function, istr, [&](llvm::Value* a) { return builder.CreateNot(a, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case OpLogNot: {
                    make_unary(builder, function, istr, [&](llvm::Value* a) { return builder.CreateICmpEQ(a, llvm::ConstantInt::get(a->getType(), 0), op); }, m_boolT, istr_id, m_defaultIntT);
                    break;
                }
                case OpBitNor: {
                    make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateNot(builder.CreateOr(a, b, op)); }, m_defaultIntT, istr_id);
                    break;
                }
                case OpLogAnd: {
                    make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateLogicalAnd(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case OpLogOr: {
                    make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateLogicalOr(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case INeg: {
                    make_unary(builder, function, istr, [&](llvm::Value* a) { return builder.CreateNeg(a, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case FNeg: {
                    make_unary(builder, function, istr, [&](llvm::Value* a) { return builder.CreateFNeg(a, op); }, m_defaultFloatT, istr_id, m_defaultFloatT);
                    break;
                }
                case IAddImm: {
                    if (m_types[istr.operand1]->isPointerTy()) {
                        make_binary_imm(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreatePtrAdd(a, b, op); }, m_types[istr.operand1], istr_id);
                    } else {
                        make_binary_imm(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateAdd(a, b, op); }, m_defaultIntT, istr_id);
                    }
                    break;
                }
                case ISubImm: {
                    make_binary_imm(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateSub(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case IMulImm: {
                    make_binary_imm(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateMul(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case IDivImm: {
                    make_binary_imm(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateSDiv(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case IntAsh: {
                    make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateIntrinsic(llvm::Intrinsic::dcvm_intash, {a, b}); }, m_defaultIntT, istr_id);
                    break;
                }
                case IEqual: {
                    make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateICmpEQ(a, b, op); }, m_defaultIntT, istr_id, m_defaultIntT);
                    break;
                }
                case FEqual: {
                    make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFCmpUEQ(a, b, op); }, m_boolT, istr_id, m_defaultFloatT);
                    break;
                }
                case INotEqual: {
                    make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateICmpNE(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case FNotEqual: {
                    make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFCmpUNE(a, b, op); }, m_boolT, istr_id, m_defaultFloatT);
                    break;
                }
                case ILessThan: {
                    make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateICmpSLT(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case FLessThan: {
                    make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFCmpULT(a, b, op); }, m_boolT, istr_id, m_defaultFloatT);
                    break;
                }
                case ILessThanEqual: {
                    make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateICmpSLE(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case FLessThanEqual: {
                    make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFCmpULE(a, b, op); }, m_boolT, istr_id, m_defaultFloatT);
                    break;
                }
                case IGreaterThan: {
                    make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateICmpSGT(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case FGreaterThan: {
                    make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFCmpUGT(a, b, op); }, m_boolT, istr_id, m_defaultFloatT);
                    break;
                }
                case IGreaterThanEqual: {
                    make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateICmpSGE(a, b, op); }, m_boolT, istr_id);
                    break;
                }
                case FGreaterThanEqual: {
                    make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFCmpUGE(a, b, op); }, m_boolT, istr_id, m_defaultFloatT);
                    break;
                }
                case IMod: {
                    make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateSRem(a, b, op); }, m_defaultIntT, istr_id);
                    break;
                }
                case FMod: {
                    make_binary(builder, function, istr, [&](llvm::Value* a, llvm::Value* b) { return builder.CreateFRem(a, b, op); }, m_defaultFloatT, istr_id, m_defaultFloatT);
                    break;
                }
                case IAbs: {
                    make_unary(builder, function, istr, [&](llvm::Value* a) { return builder.CreateIntrinsic(llvm::Intrinsic::abs, {a->getType()}, {a, builder.getFalse()}); }, m_defaultIntT, istr_id);
                    break;
                }
                case FAbs: {
                    make_unary(builder, function, istr, [&](llvm::Value* a) { return builder.CreateUnaryIntrinsic(llvm::Intrinsic::fabs, a); }, m_defaultFloatT, istr_id, m_defaultFloatT);
                    break;
                }
                case AssertPointer: {
                    m_types[istr.destination] = m_defaultPointerT;
                    llvm::Value* ptr = load_register(builder, function, istr.destination, std::format("{}_ptr", istr_id));
                    builder.CreateIsNotNull(ptr, op);
                    break;
                }
                case LoadU16Imm: {
                    const u16 val = istr.operand1 | (istr.operand2 << 8);
                    m_types[istr.destination] = m_defaultIntT;
                    llvm::Constant* value = llvm::ConstantInt::get(m_defaultIntT, val, false);
                    make_store(builder, function, istr.destination, value);
                    break;
                }
                case LoadStaticU8Imm: {
                    make_symbol_table_load<u8>(builder, function, istr, istr_id);
                    break;
                }
                case LoadStaticU16Imm: {
                    make_symbol_table_load<u16>(builder, function, istr, istr_id);
                    break;
                }
                case LoadStaticU32Imm: {
                    make_symbol_table_load<u32>(builder, function, istr, istr_id);
                    break;
                }
                case LoadStaticU64Imm: {
                    make_symbol_table_load<u64>(builder, function, istr, istr_id);
                    break;
                }
                case LoadStaticI8Imm: {
                    make_symbol_table_load<i8>(builder, function, istr, istr_id);
                    break;
                }
                case LoadStaticI16Imm: {
                    make_symbol_table_load<i16>(builder, function, istr, istr_id);
                    break;
                }
                case LoadStaticI32Imm: {
                    make_symbol_table_load<i32>(builder, function, istr, istr_id);
                    break;
                }
                case LoadStaticI64Imm: {
                    make_symbol_table_load<i64>(builder, function, istr, istr_id);
                    break;
                }
                case LoadStaticFloatImm: {
                    make_symbol_table_load<f32>(builder, function, istr, istr_id);
                    break;
                }
                case LoadStaticInt: {
                    llvm::Value* idx = load_register(builder, function, istr.operand1, std::format("{}_idx", istr_id));
                    llvm::Value* value = builder.CreateIntrinsic(llvm::Intrinsic::dcvm_load_static_int, {idx}, {}, std::format("{}static_int", istr_id));
                    make_store(builder, function, istr.destination, value, m_defaultIntT);
                    break;
                }
                case LoadStaticFloat: {
                    llvm::Value* idx = load_register(builder, function, istr.operand1, std::format("{}_idx", istr_id));
                    llvm::Value* value = builder.CreateIntrinsic(llvm::Intrinsic::dcvm_load_static_float, {idx}, {}, std::format("{}static_float", istr_id));
                    make_store(builder, function, istr.destination, value, m_defaultFloatT);
                    break;
                }
                case LoadStaticPointer: {
                    llvm::Value* idx = load_register(builder, function, istr.operand1, std::format("{}_idx", istr_id));
                    llvm::Value* value = builder.CreateIntrinsic(llvm::Intrinsic::dcvm_load_static_pointer, {idx}, {}, std::format("{}static_pointer", istr_id));
                    make_store(builder, function, istr.destination, value, m_defaultPointerT);
                    break;
                }
                case LoadU8: {
                    make_mem_load<u8>(builder, function, istr, istr_id);
                    break;
                }
                case LoadU16: {
                    make_mem_load<u16>(builder, function, istr, istr_id);
                    break;
                }
                case LoadU32: {
                    make_mem_load<u32>(builder, function, istr, istr_id);
                    break;
                }
                case LoadU64: {
                    make_mem_load<u64>(builder, function, istr, istr_id);
                    break;
                }
                case LoadI8: {
                    make_mem_load<i8>(builder, function, istr, istr_id);
                    break;
                }
                case LoadI16: {
                    make_mem_load<i16>(builder, function, istr, istr_id);
                    break;
                }
                case LoadI32: {
                    make_mem_load<i32>(builder, function, istr, istr_id);
                    break;
                }
                case LoadI64: {
                    make_mem_load<i64>(builder, function, istr, istr_id);
                    break;
                }
                case LoadFloat: {
                    make_mem_load<f32>(builder, function, istr, istr_id);
                    break;
                }
                case LoadPointer: {
                    make_mem_load<void*>(builder, function, istr, istr_id);
                    break;
                }
                case StoreI8: {
                    make_mem_store<i8>(builder, function, istr, istr_id);
                    break;
                }
                case StoreU8: {
                    make_mem_store<u8>(builder, function, istr, istr_id);
                    break;
                }
                case StoreI16: {
                    make_mem_store<i16>(builder, function, istr, istr_id);
                    break;
                }
                case StoreU16: {
                    make_mem_store<u16>(builder, function, istr, istr_id);
                    break;
                }
                case StoreI32: {
                    make_mem_store<i32>(builder, function, istr, istr_id);
                    break;
                }
                case StoreU32: {
                    make_mem_store<u32>(builder, function, istr, istr_id);
                    break;
                }
                case StoreI64: {
                    make_mem_store<i64>(builder, function, istr, istr_id);
                    break;
                }
                case StoreU64: {
                    make_mem_store<u64>(builder, function, istr, istr_id);
                    break;
                }
                case StoreFloat: {
                    make_mem_store<f32>(builder, function, istr, istr_id);
                    break;
                }
                case StorePointer: {
                    make_mem_store<const void*>(builder, function, istr, istr_id);
                    break;
                }
                case CastInteger: {
                    llvm::Value* current_float = load_register(builder, function, istr.operand1, "cast_load", m_defaultFloatT);

                    llvm::Value* cast = builder.CreateFPToUI(current_float, m_defaultIntT);
                    m_types[istr.destination] = m_defaultIntT;
                    make_store(builder, function, istr.destination, cast, m_defaultIntT);
                    break;
                }
                case CastFloat: {
                    llvm::Value* current_int = load_register(builder, function, istr.operand1);

                    assert(m_types[istr.operand1] == m_defaultIntT);

                    llvm::Value* cast = builder.CreateUIToFP(current_int, m_defaultFloatT);
                    m_types[istr.destination] = m_defaultFloatT;

                    make_store(builder, function, istr.destination, cast, m_defaultIntT);
                    break;
                }
                case LoadStaticPointerImm: {
                    // temporary until i implment cross-module string insertion.
                    function.m_llvmFunc->setMetadata("NonInlineableAcrossModules", llvm::MDNode::get(m_ctx, {}));
                    const ast::full_type& type = m_disasm->m_stackFrame.m_symbolTable.m_types[istr.operand1];
                    const p64 mapped_string_offset = function.m_symbolTable->get<p64>(istr.operand1 * sizeof(u64));
                    const p64 binary_file_byte_start = p64(unit.m_binaryFile->m_bytes.get());
                    const p64 string_file_offset = mapped_string_offset - binary_file_byte_start;
                    llvm::Value* string_pointer = builder.CreateIntrinsic(llvm::Intrinsic::dcvm_static_pointer, {builder.getInt64(string_file_offset)});
                    make_store(builder, function, istr.destination, string_pointer, m_defaultPointerT);
                    break;
                }
                case Move: {
                    make_store(builder, function, istr.destination, load_register(builder, function, istr.operand1, std::format("{}_load_op1", istr_id)), m_types[istr.operand1]);
                    break;
                }
                case LookupPointer: {
                    const ast::full_type& symbol_table_t = m_disasm->m_stackFrame.m_symbolTable.m_types[istr.operand1];
                    if (const ast::function_type* ast_function_t = std::get_if<ast::function_type>(&symbol_table_t)) {
                        std::vector<llvm::Type*> arg_types(ast_function_t->m_arguments.size(), m_defaultIntT);
                        llvm::FunctionType* function_type = llvm::FunctionType::get(m_defaultIntT, arg_types, ast_function_t->m_isVariadic);

                        const sid64 func_sid = m_symbolTable->get<sid64>(istr.operand1 * sizeof(u64));
                        const char* function_name = m_sidbase.lookup(func_sid);
                        m_lookupCallDistances[func_sid] = ast_function_t->m_distanceType;

                        llvm::FunctionCallee callee_function = m_module.getOrInsertFunction(function_name, function_type);
                        llvm::Function* F = llvm::cast<llvm::Function>(callee_function.getCallee());
                        F->setDoesNotThrow();
                        F->setWillReturn();
                        F->setNoSync();

                        unit.m_calledGlobals.emplace_back(function_name);

                        make_store(builder, function, istr.destination, callee_function.getCallee(), function_type);
                    } else {
                        assert(std::holds_alternative<ast::primitive_type>(symbol_table_t));
                        const ast::primitive_type ast_sid_t = std::get<ast::primitive_type>(symbol_table_t);
                        assert(ast_sid_t.m_type == ast::primitive_kind::SID);
                        const sid64 sid_id = m_symbolTable->get<sid64>(istr.operand1 * sizeof(u64));
                        make_symbol_table_load<sid64>(builder, function, istr, istr_id);
                        llvm::Value* sid = load_register(builder, function, istr.destination, "", m_defaultIntT);
                        llvm::CallInst* func_ptr = builder.CreateIntrinsic(llvm::Intrinsic::dcvm_lookup, {sid}, {}, std::format("{}_{}_lookup", istr_id, sid_id));

                        make_store(builder, function, istr.destination, func_ptr, m_defaultPointerT);
                    }
                    break;
                }
                case CallFf:
                case Call: {
                    llvm::FunctionType* func_type = llvm::cast<llvm::FunctionType>(m_types[istr.operand1]);
                    llvm::Value* callee = load_register(builder, function, istr.operand1, std::format("{}_callee", istr_id));
                    std::vector<llvm::Value*> arg_values;
                    arg_values.reserve(istr.operand2);
                    for (u64 i = 0; i < istr.operand2; ++i) {
                        arg_values.push_back(load_register(builder, function, ARGUMENT_REGISTERS_IDX + i, std::format("{}_arg{}", istr_id, i), m_defaultIntT));
                        if (i < func_type->getNumParams() && arg_values.back()->getType() != func_type->getParamType(i)) {
                            func_type = exchange_function_types(function, arg_values.back()->getType(), istr.operand1, i);
                        }
                    }
                    llvm::CallInst* call = builder.CreateCall(func_type, callee, llvm::ArrayRef(arg_values.data(), arg_values.size()), std::format("{}_call", istr_id));
                    make_store(builder, function, istr.destination, call, func_type->getReturnType());
                    break;
                }
                case Return: {
                    llvm::Value* ret_value = builder.CreateLoad(m_defaultIntT, m_registerFrame[istr.destination], std::format("{}_retval", istr_id));
                    builder.CreateRet(ret_value);
                    terminated = true;
                    break;
                }
                case Branch: {
                    istr_line target = current_node->m_lines.back().m_target;
                    builder.CreateBr(m_blocks.at(target));
                    builder.SetInsertPoint(m_blocks.at(l + 1));
                    current_node = m_graph->get_node_with_start_line(l + 1);
                    assert(current_node != nullptr);
                    terminated = true;
                    break;
                }
                case BranchIf: {
                    const istr_line destination = current_node->m_lines.back().m_target;
                    llvm::Value* condition_reg = builder.CreateLoad(m_defaultIntT, m_registerFrame[istr.operand1], std::format("{}_cond", istr_id));
                    llvm::Value* bool_condition_reg = builder.CreateICmpNE(condition_reg, llvm::ConstantInt::get(condition_reg->getType(), 0), std::format("{}_bool", istr_id));
                    llvm::BasicBlock* target_block = m_blocks.at(destination);
                    llvm::BasicBlock* fallthrough = m_blocks.at(l + 1);
                    builder.CreateCondBr(bool_condition_reg, target_block, fallthrough);
                    builder.SetInsertPoint(m_blocks.at(l + 1));
                    current_node = m_graph->get_node_with_start_line(l + 1);
                    assert(current_node != nullptr);
                    terminated = true;
                    break;
                }
                case BranchIfNot: {
                    const istr_line destination = current_node->m_lines.back().m_target;
                    llvm::Value* condition_reg = builder.CreateLoad(m_defaultIntT, m_registerFrame[istr.operand1], std::format("{}_cond", istr_id));
                    llvm::Value* bool_condition_reg = builder.CreateICmpNE(condition_reg, llvm::ConstantInt::get(condition_reg->getType(), 0), std::format("{}_bool", istr_id));
                    llvm::BasicBlock* target_block = m_blocks.at(destination);
                    llvm::BasicBlock* fallthrough = m_blocks.at(l + 1);
                    builder.CreateCondBr(bool_condition_reg, fallthrough, target_block);
                    builder.SetInsertPoint(m_blocks.at(l + 1));
                    current_node = m_graph->get_node_with_start_line(l + 1);
                    assert(current_node != nullptr);
                    terminated = true;
                    break;
                }
                default:
                    return std::format("unhandled opcode {}", istr.opcode_to_string());
            }

            if (!terminated && l == current_node->m_endLine) {
                builder.CreateBr(m_blocks.at(l + 1));
                builder.SetInsertPoint(m_blocks.at(l + 1));
                current_node = m_graph->get_node_with_start_line(l + 1);
                assert(current_node != nullptr);
            }
        }

        return std::nullopt;
    }

    std::vector<generated_outputs> llvm_transpiler::run() {
        if (m_excludedPath) {
            if (auto res = read_excluded_functions(*m_excludedPath)) {
                m_exludedFunctions = *res;
            } else {
                std::println(stderr, "failed to read excluded functions: {}", res.error());
                return {};
            }
            for (translation_unit& unit : m_translationUnits) {
                std::erase_if(unit.m_preparedFunctions, [this](const prepared_function_ctx& function) {
                    return m_exludedFunctions.contains(SID(function.m_disasm->get_id().c_str()));
                });
            }
        }
        std::vector<generated_outputs> outputs(m_translationUnits.size());

        prepare_functions();
        link_module_optimization();

        struct serialized_module {
            std::string m_name;
            std::string m_bitcode;
        };

        std::vector<serialized_module> serialized(m_translationUnits.size());
        std::transform(m_translationUnits.begin(), m_translationUnits.end(), serialized.begin(), [](const translation_unit& unit) {
            serialized_module result{unit.module().getName().str(), {}};
            llvm::raw_string_ostream os(result.m_bitcode);
            llvm::WriteBitcodeToFile(unit.module(), os);
            return result;
        });

        std::transform(std::execution::par, serialized.begin(), serialized.end(), outputs.begin(), [this] (const serialized_module& input) {
            generated_outputs unit_outputs;
            unit_outputs.m_moduleName = input.m_name;

            llvm::LLVMContext local_ctx;
            llvm::Expected<std::unique_ptr<llvm::Module>> parsed = llvm::parseBitcodeFile(llvm::MemoryBufferRef(input.m_bitcode, input.m_name), local_ctx);
            if (!parsed) {
                std::println(stderr, "failed to reparse module {}: {}", input.m_name, llvm::toString(parsed.takeError()));
                return unit_outputs;
            }
            llvm::Module& module = **parsed;
            unit_outputs.m_llvmUnopt = emit_llvm_ir(module);

            std::println("running first pass optimization for module {}", module.getName().str());
            optimize_module(module);
            lower_calls_to_lookup(module);
            std::println("running second pass optimization for module {}", module.getName().str());
            optimize_module(module, true);
            annotate_lookup_call_distances(module);

            unit_outputs.m_llvmOpt = emit_llvm_ir(module);
            unit_outputs.m_dcvmAsm = emit_dcvm_asm(module);
            unit_outputs.m_dcvmBytecode = emit_dcvm_bytecode(module);

            return unit_outputs;
        });

        return outputs;
    }

    void llvm_transpiler::prepare_functions() {
        for (translation_unit& unit : m_translationUnits) {
            std::println("preparing {} functions inside module {}", unit.m_preparedFunctions.size(), unit.m_module->getName().str());
            for (prepared_function_ctx& function : unit.m_preparedFunctions) {
                prepare_function(unit, function);
            }

            unit.m_liftedFunctions.clear();
            unit.m_liftedFunctions.reserve(unit.m_preparedFunctions.size());
            for (prepared_function_ctx& function : unit.m_preparedFunctions) {
                if (!lift_prepared_function(unit, function)) {
                    unit.m_liftedFunctions.push_back(function.m_llvmFunc);
                }
            }
        }
    }

    void llvm_transpiler::link_module_optimization() {
        for (translation_unit& unit : m_translationUnits) {
            std::println("linking module {}", unit.m_module->getName().str());
            for (const auto& global_name : unit.m_calledGlobals) {
                llvm::Function* existing = unit.module().getFunction(global_name);

                if (!existing || !existing->isDeclaration()) {
                    continue;
                }

                auto it = m_funcLocations.find(global_name);
                if (it == m_funcLocations.end()) {
                    continue;
                }
                const llvm::Function* runtime_function = it->second;

                assert(runtime_function->getParent() != &unit.module());
                if (runtime_function->isDeclaration() || runtime_function->hasMetadata("NonInlineableAcrossModules")) {
                    existing->setAttributes(runtime_function->getAttributes());
                } else {
                    llvm::Function* dst_func = unit.module().getFunction(global_name);
                    assert(dst_func);
                    dst_func->copyAttributesFrom(runtime_function);
                    dst_func->setCallingConv(runtime_function->getCallingConv());
                    dst_func->setLinkage(llvm::GlobalValue::AvailableExternallyLinkage);

                    llvm::ValueToValueMapTy vmap;
                    vmap[runtime_function] = dst_func;
                    map_referenced_globals_for_clone(unit.module(), *runtime_function, vmap);

                    auto dst_arg = dst_func->arg_begin();
                    for (const llvm::Argument& src_arg : runtime_function->args()) {
                        assert(dst_arg != dst_func->arg_end());
                        dst_arg->setName(src_arg.getName());
                        vmap[&src_arg] = &*dst_arg;
                        ++dst_arg;
                    }

                    llvm::SmallVector<llvm::ReturnInst*, 8> returns;

                    llvm::CloneFunctionInto(dst_func, runtime_function, vmap, llvm::CloneFunctionChangeType::DifferentModule, returns);
                    assert(!llvm::verifyFunction(*dst_func, &llvm::errs()));
                }
            }
        }
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
            if (content->empty()) {
                continue;
            }
            std::ofstream file(path);
            if (!file) {
                return "couldn't open output file " + path.string();
            }
            file << *content;
        }

        if (outputs.m_dcvmBytecode.empty()) {
            return std::nullopt;
        }

        const std::filesystem::path bytecode_path = output_dir / std::format("{}.dcvm.bin", module_name);
        std::ofstream bytecode_file(bytecode_path, std::ios::binary);
        if (!bytecode_file) {
            return "couldn't open output file " + bytecode_path.string();
        }
        bytecode_file.write(outputs.m_dcvmBytecode.data(), static_cast<std::streamsize>(outputs.m_dcvmBytecode.size()));

        return std::nullopt;
    }

    errmsg llvm_transpiler::patch_original_binfiles(const std::vector<generated_outputs>& outputs, const std::vector<original_binfile>& originals) {
        for (const original_binfile& original : originals) {
            if (original.m_binfile == nullptr) {
                return std::format("original binfile for {} is null", original.m_moduleName);
            }

            const generated_outputs* module_outputs = find_outputs_for_module(outputs, original.m_moduleName);
            if (module_outputs == nullptr) {
                return std::format("missing generated outputs for {}", original.m_moduleName);
            }
            if (module_outputs->m_dcvmBytecode.empty()) {
                continue;
            }

            resstr<std::vector<recompiled_function_info>> recompiled_functions = parse_recompiled_functions(*module_outputs);
            if (!recompiled_functions) {
                return recompiled_functions.error();
            }

            std::unordered_map<sid64, const recompiled_function_info*> functions_by_sid;
            functions_by_sid.reserve(recompiled_functions->size());
            for (const recompiled_function_info& function : *recompiled_functions) {
                functions_by_sid[function.m_sid] = &function;
            }

            std::vector<function_patch> small_patches;
            std::vector<function_patch> large_patches;

            for (const function_disassembly* old_func : original.m_functions) {
                if (old_func == nullptr) {
                    continue;
                }

                const sid64 func_sid = SID(old_func->get_id().c_str());
                const auto recompiled_it = functions_by_sid.find(func_sid);
                if (recompiled_it == functions_by_sid.end()) {
                    continue;
                }

                const recompiled_function_info& new_func = *recompiled_it->second;
                const u64 new_func_size = new_func.m_instructions.size() * sizeof(Instruction) + new_func.m_symbols.size() * sizeof(u64);
                const u64 old_func_size = old_func->m_lines.size() * sizeof(Instruction) + old_func->m_stackFrame.m_symbolTable.m_types.size() * sizeof(u64);
                if (new_func_size > old_func_size) {
                    large_patches.push_back({old_func, &new_func});
                } else {
                    small_patches.push_back({old_func, &new_func});
                }
            }

            for (const function_patch& patch : small_patches) {
                if (errmsg error = patch_recompiled_function(*original.m_binfile, *patch.m_oldFunc, *patch.m_newFunc)) {
                    return error;
                }
            }

            if (errmsg error = patch_large_recompiled_functions(*original.m_binfile, large_patches)) {
                return error;
            }
        }

        return std::nullopt;
    }

    llvm::Value* llvm_transpiler::coerce_to_storage(llvm::IRBuilder<>& builder, llvm::Value* value) const {
        llvm::Type* type = value->getType();
        if (type == m_defaultIntT) {
            return value;
        }
        if (type->isPointerTy()) {
            return builder.CreatePtrToInt(value, m_defaultIntT);
        }
        if (type->isFloatingPointTy()) {
            value = builder.CreateBitCast(value, llvm::IntegerType::get(builder.getContext(), type->getPrimitiveSizeInBits()));
            type = value->getType();
        }
        return type == m_defaultIntT ? value : builder.CreateZExt(value, m_defaultIntT);
    }

    llvm::Value* llvm_transpiler::coerce_from_storage(llvm::IRBuilder<>& builder, llvm::Value* raw, llvm::Type* logical) const {
        if (logical == m_defaultIntT) {
            return raw;
        }
        if (logical->isFloatingPointTy()) {
            llvm::Type* int_type = llvm::IntegerType::get(builder.getContext(), logical->getPrimitiveSizeInBits());
            llvm::Value* bits = int_type == m_defaultIntT ? raw : builder.CreateTrunc(raw, int_type);
            return builder.CreateBitCast(bits, logical);
        }
        if (logical->isIntegerTy()) {
            return builder.CreateTrunc(raw, logical);
        }
        return builder.CreateIntToPtr(raw, m_defaultPointerT);
    }

    llvm::Value* llvm_transpiler::load_register(llvm::IRBuilder<>& builder, prepared_function_ctx& function, const u8 index, std::string_view istr_name, llvm::Type* as_type) const {
        llvm::Value* raw = builder.CreateLoad(m_defaultIntT, function.m_registerFrame[index], istr_name);
        return coerce_from_storage(builder, raw, as_type ? as_type : function.m_types[index]);
    }

    llvm::StoreInst* llvm_transpiler::make_store(llvm::IRBuilder<>& builder, prepared_function_ctx& function, const u8 index, llvm::Value* rhs, llvm::Type* type) const {
        if (type) {
            function.m_types[index] = type;
        }
        return builder.CreateStore(coerce_to_storage(builder, rhs), function.m_registerFrame[index]);
    }

    llvm::StoreInst* llvm_transpiler::make_binary(
        llvm::IRBuilder<>& builder,
        prepared_function_ctx& function,
        const Instruction& istr,
        std::function<llvm::Value*(llvm::Value*, llvm::Value*)> binary_op,
        llvm::Type* dest_type,
        std::string_view istr_id,
        llvm::Type* coerced_type
    ) const {
        llvm::Value* lhs = load_register(builder, function, istr.operand1, std::format("{}_load_lhs", istr_id), coerced_type);
        llvm::Value* rhs = load_register(builder, function, istr.operand2, std::format("{}_load_rhs", istr_id), coerced_type);
        if (lhs->getType() != rhs->getType() && (lhs->getType() != m_defaultPointerT && rhs->getType() != m_defaultPointerT)) {
            rhs = builder.CreateZExtOrTrunc(rhs, lhs->getType());
        }

        llvm::Value* res = binary_op(lhs, rhs);
        function.m_types[istr.destination] = dest_type;
        return make_store(builder, function, istr.destination, res);
    }

    llvm::StoreInst* llvm_transpiler::make_binary_imm(
        llvm::IRBuilder<>& builder, prepared_function_ctx& function, const Instruction& istr, std::function<llvm::Value*(llvm::Value*, llvm::Value*)> binary_op, llvm::Type* dest_type, std::string_view istr_id
    ) const {
        llvm::Value* lhs = load_register(builder, function, istr.operand1, std::format("{}_load_lhs", istr_id));
        llvm::Type* imm_type = lhs->getType()->isPointerTy() ? m_defaultIntT : lhs->getType();
        llvm::Value* imm = llvm::ConstantInt::get(imm_type, istr.operand2);
        llvm::Value* res = binary_op(lhs, imm);
        function.m_types[istr.destination] = dest_type;
        return make_store(builder, function, istr.destination, res);
    }

    llvm::StoreInst* llvm_transpiler::make_unary(
        llvm::IRBuilder<>& builder, prepared_function_ctx& function, const Instruction& istr, std::function<llvm::Value*(llvm::Value*)> unary_op, llvm::Type* dest_type, std::string_view istr_id, llvm::Type* coerced_type
    ) const {
        llvm::Value* lhs = load_register(builder, function, istr.operand1, std::format("{}_load_op1", istr_id), coerced_type);
        llvm::Value* res = unary_op(lhs);
        function.m_types[istr.destination] = dest_type;
        return make_store(builder, function, istr.destination, res);
    }

    llvm::FunctionType* llvm_transpiler::exchange_function_types(prepared_function_ctx& function, llvm::Type* new_arg_type, const u8 type_location, const u8 arg_idx) const {
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

    [[nodiscard]] llvm::Type* ast_type_to_llvm_type(llvm::LLVMContext& ctx, const ast::full_type& type) {
        return std::visit(
            [&ctx](auto&& arg) -> llvm::Type* {
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
            },
            type
        );
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

            // the greedy allocator's last-chance recoloring gives up at fixed
            // depth/interference cutoffs and aborts compilation. on a target
            // that cannot spill, recoloring is the last resort before failure,
            // so let it search exhaustively instead of stopping at heuristics.
            const char* args[] = {"llvm_transpile", "-exhaustive-register-search"};
            llvm::cl::ParseCommandLineOptions(2, args, "", nullptr, nullptr);
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
        std::println("emitting llvm ir for module {}", module.getName().str());
        std::string ir;
        llvm::raw_string_ostream os(ir);
        module.print(os, nullptr);
        return ir;
    }

    void optimize_module(llvm::Module& module, bool hoist_redundant_calls) {
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
        if (hoist_redundant_calls) {
            // O3 only removes dominated duplicates of readnone calls (EarlyCSE/GVN).
            // dcvm.lookup duplicates that sit in sibling branches are partial
            // redundancies, which no default pass merges for calls. GVNHoist moves
            // identical computations to the nearest common dominator, so lookups
            // combine without extending their live range to the function entry.
            llvm::FunctionPassManager FPM;
            FPM.addPass(llvm::GVNHoistPass());
            FPM.addPass(llvm::SimplifyCFGPass());
            MPM.addPass(llvm::createModuleToFunctionPassAdaptor(std::move(FPM)));
        }
        MPM.run(module, MAM);
    }

    const char* distance_metadata_name(const ast::function_type::DISTANCE distance) {
        switch (distance) {
            case ast::function_type::DISTANCE::NEAR:
                return "near";
            case ast::function_type::DISTANCE::FAR:
                return "far";
        }
        assert(false);
        return "near";
    }

    std::optional<sid64> lookup_sid_from_value(llvm::Value* value) {
        value = value->stripPointerCasts();

        if (auto* lookup = llvm::dyn_cast<llvm::CallInst>(value)) {
            if (lookup->getIntrinsicID() != llvm::Intrinsic::dcvm_lookup) {
                return std::nullopt;
            }

            auto* sid = llvm::dyn_cast<llvm::ConstantInt>(lookup->getArgOperand(0));
            if (sid == nullptr) {
                return std::nullopt;
            }

            return sid->getZExtValue();
        }

        if (auto* phi = llvm::dyn_cast<llvm::PHINode>(value)) {
            if (phi->getNumIncomingValues() == 0) {
                return std::nullopt;
            }
            return lookup_sid_from_value(phi->getIncomingValue(0));
        }

        if (auto* select = llvm::dyn_cast<llvm::SelectInst>(value)) {
            return lookup_sid_from_value(select->getTrueValue());
        }

        return std::nullopt;
    }

    void llvm_transpiler::lower_calls_to_lookup(llvm::Module& module) const {
        llvm::IRBuilder<> builder(module.getContext());

        std::println("lowering calls to lookup for module {}", module.getName().str());
        std::vector<llvm::CallInst*> direct_calls;
        for (llvm::Function& func : module) {
            for (llvm::BasicBlock& block : func) {
                for (llvm::Instruction& inst : block) {
                    auto* call = llvm::dyn_cast<llvm::CallInst>(&inst);
                    if (call == nullptr) {
                        continue;
                    }
                    const llvm::Function* callee = llvm::dyn_cast<llvm::Function>(call->getCalledOperand()->stripPointerCasts());
                    if (callee == nullptr || callee->isIntrinsic()) {
                        continue;
                    }
                    direct_calls.push_back(call);
                }
            }
        }

        for (llvm::CallInst* call : direct_calls) {
            llvm::Function* callee = llvm::cast<llvm::Function>(call->getCalledOperand()->stripPointerCasts());
            const std::string name = callee->getName().str();
            llvm::Value* sid = builder.getInt64(SID(name.c_str()));
            builder.SetInsertPoint(call);
            llvm::CallInst* func_ptr = builder.CreateIntrinsic(llvm::Intrinsic::dcvm_lookup, {sid});
            func_ptr->setName(std::format("__LookupPointer_{}_fptr", name));
            call->setCalledOperand(func_ptr);
            call->setMetadata(llvm::LLVMContext::MD_callees, llvm::MDNode::get(module.getContext(), llvm::ValueAsMetadata::get(callee)));
        }
    }

    void llvm_transpiler::annotate_lookup_call_distances(llvm::Module& module) const {
        llvm::LLVMContext& ctx = module.getContext();

        std::println("adding lookup distances to functions inside module {}", module.getName().str());
        for (llvm::Function& func : module) {
            for (llvm::BasicBlock& block : func) {
                for (llvm::Instruction& inst : block) {
                    auto* call = llvm::dyn_cast<llvm::CallInst>(&inst);
                    if (call == nullptr) {
                        continue;
                    }

                    const std::optional<sid64> sid = lookup_sid_from_value(call->getCalledOperand());
                    if (!sid) {
                        continue;
                    }

                    const auto distance = m_lookupCallDistances.find(*sid);
                    if (distance == m_lookupCallDistances.end()) {
                        continue;
                    }

                    call->setMetadata("dcvm.distance", llvm::MDNode::get(ctx, {llvm::MDString::get(ctx, distance_metadata_name(distance->second))}));
                }
            }
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
        std::println("emitting dcvm asm for module {}", module.getName().str());
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
        std::println("emitting dcvm byte code for module {}", module.getName().str());
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

        std::vector<char> bytecode;
        DCVMSetBytecodeSink(machine.get(), &bytecode);
        pass_manager.run(*clone);

        return bytecode;
    }

    void llvm_transpiler::init_runtime_module() {
        std::unique_ptr module = std::make_unique<llvm::Module>("__dcvm_runtime", m_ctx);
        llvm::IRBuilder<> builder(m_ctx);

        auto add_runtime_function_overwrite = [&](std::string_view name, u64 num_args, u64 custom_return_val = 0) {
            std::vector<llvm::Type*> arg_types(num_args, m_defaultIntT);
            llvm::FunctionType* func_t = llvm::FunctionType::get(m_defaultIntT, arg_types, false);
            llvm::Function* func = llvm::Function::Create(func_t, llvm::GlobalValue::LinkageTypes::ExternalLinkage, 0, name, module.get());
            func->addFnAttr(llvm::Attribute::AlwaysInline);
            llvm::BasicBlock* block = llvm::BasicBlock::Create(m_ctx, "bb", func);
            builder.SetInsertPoint(block, block->begin());
            llvm::Value* ret_value = builder.getInt64(custom_return_val);
            builder.CreateRet(ret_value);
            m_funcLocations[name.data()] = func;
        };

        auto add_runtime_function_declaration = [&](std::string_view name, u64 num_args, std::initializer_list<llvm::Attribute::AttrKind> attrs) {
            std::vector<llvm::Type*> arg_types(num_args, m_defaultIntT);
            llvm::FunctionType* func_t = llvm::FunctionType::get(m_defaultIntT, arg_types, false);
            llvm::Function* func = llvm::Function::Create(func_t, llvm::GlobalValue::LinkageTypes::ExternalLinkage, 0, name, module.get());
            for (const auto& attr : attrs) {
                func->addFnAttr(attr);
            }
            func->setMemoryEffects(llvm::MemoryEffects::none());
            m_funcLocations[name.data()] = func;
        };

        constexpr bool is_final_build_val = true;
        add_runtime_function_overwrite("is-final-build?", 0, is_final_build_val);

        add_runtime_function_overwrite("dc-assert-enabled?", 0, false);

        add_runtime_function_overwrite("debug-draw-line", 10);
        add_runtime_function_overwrite("debug-draw-locator", 6);
        add_runtime_function_overwrite("debug-draw-line-2d", 9);
        add_runtime_function_overwrite("debug-draw-string", 8);
        add_runtime_function_overwrite("debug-draw-string-2d", 9);
        add_runtime_function_overwrite("debug-draw-sphere", 6);
        add_runtime_function_overwrite("debug-draw-cross", 7);
        add_runtime_function_overwrite("debug-draw-box-2d", 8);

        const std::initializer_list optmized_attrs = {
            llvm::Attribute::Speculatable,
            llvm::Attribute::NoUnwind,
            llvm::Attribute::WillReturn,
            llvm::Attribute::NoFree,
        };

        // add_runtime_function_declaration("vector-dot", 2, optmized_attrs);

        assert(!llvm::verifyModule(*module, &llvm::errs()));

        m_translationUnits.emplace_back(std::move(module), nullptr);
    }

    [[nodiscard]] resstr<std::set<sid64>> llvm_transpiler::read_excluded_functions(const std::filesystem::path& path) const noexcept {
        std::ifstream fs(path);
        std::set<sid64> res;

        if (!fs) {
            return std::unexpected(std::format("couldn't open filepath {}", path.string()));
        }

        std::string line;
        while (std::getline(fs, line)) {
            const u64 first = line.find_first_not_of(" \t\r\n");
            if (first == std::string::npos) {
                continue;
            }
            const u64 last = line.find_last_not_of(" \t\r\n");
            res.insert(SID(line.substr(first, last - first + 1).c_str()));
        }

        return res;
    }
}
