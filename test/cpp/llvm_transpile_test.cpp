#include <gtest/gtest.h>

#include "DCScript.h"
#include "ast/type.h"
#include "binaryfile.h"
#include "disassembly/disassembler.h"
#include "disassembly/instructions.h"
#include "llvm_transpile/llvm_transpile.h"
#include "sidbase.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <format>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace dconstruct::testing {

    static const std::filesystem::path TEST_ROOT = DCONSTRUCT_TEST_ROOT;

    struct function_snapshot {
        std::string m_id;
        sid64 m_sid = 0;
        u64 m_lambdaOffset = 0;
        u64 m_instructionOffset = 0;
        u64 m_symbolOffset = 0;
        std::optional<u64> m_stateScriptLambdaPointerOffset;
        std::set<std::string> m_staticStrings;
    };

    struct recompiled_function {
        sid64 m_sid = 0;
        std::vector<Instruction> m_instructions;
        std::vector<u64> m_symbols;
        std::vector<bool> m_pointerMap;
        std::vector<std::optional<std::string>> m_originalPointerStrings;
    };

    std::filesystem::path wave_manager_fixture_path() {
        if (const char* dc1_dir = std::getenv("TLOU2_DC1_DIR")) {
            return std::filesystem::path(dc1_dir) / "ss-rogue" / "wave-manager-funcs.bin";
        }

        return "C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/build/pc/main/bin_unpacked/dc1/ss-rogue/wave-manager-funcs.bin";
    }

    std::filesystem::path assault_manager_fixture_path() {
        if (const char* dc1_dir = std::getenv("TLOU2_DC1_DIR")) {
            return std::filesystem::path(dc1_dir) / "ss-rogue" / "ss-assault-manager.bin";
        }

        return "C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/build/pc/main/bin_unpacked/dc1/ss-rogue/ss-assault-manager.bin";
    }

    std::filesystem::path nd_script_funcs_fixture_path() {
        if (const char* dc1_dir = std::getenv("TLOU2_DC1_DIR")) {
            return std::filesystem::path(dc1_dir) / "nd-script-funcs.bin";
        }

        return "C:/Program Files (x86)/Steam/steamapps/common/The Last of Us Part II/build/pc/main/bin_unpacked/dc1/nd-script-funcs.bin";
    }

    u64 file_offset(const BinaryFile& file, const void* ptr) {
        return reinterpret_cast<p64>(ptr) - reinterpret_cast<p64>(file.m_bytes.get());
    }

    bool reloc_bit_is_set(const BinaryFile& file, const u64 file_offset) {
        const u64 slot = file_offset / sizeof(u64);
        return (file.m_relocTable.get<u8>(slot / 8) & (1 << (slot % 8))) != 0;
    }

    template <typename T>
    bool read_bytecode_value(const std::vector<char>& bytes, std::size_t& offset, T& out) {
        if (bytes.size() - offset < sizeof(T)) {
            return false;
        }
        std::memcpy(&out, bytes.data() + offset, sizeof(T));
        offset += sizeof(T);
        return true;
    }

    resstr<std::vector<recompiled_function>> parse_recompiled_functions(const llvm_transpile::generated_outputs& output) {
        std::vector<recompiled_function> functions;
        std::size_t offset = 0;

        while (offset < output.m_dcvmBytecode.size()) {
            u64 sid = 0;
            u64 sizes = 0;
            if (!read_bytecode_value(output.m_dcvmBytecode, offset, sid) || !read_bytecode_value(output.m_dcvmBytecode, offset, sizes)) {
                return std::unexpected(std::format("truncated bytecode header in {}", output.m_moduleName));
            }

            const u32 instruction_bytes = static_cast<u32>(sizes);
            const u32 symbol_bytes = static_cast<u32>(sizes >> 32);
            if (instruction_bytes % sizeof(Instruction) != 0 || symbol_bytes % sizeof(u64) != 0) {
                return std::unexpected(std::format("invalid bytecode payload sizes for sid {:x}", sid));
            }

            const u64 instruction_count = instruction_bytes / sizeof(Instruction);
            const u64 symbol_count = symbol_bytes / sizeof(u64);
            const u64 payload_size = instruction_bytes + symbol_bytes + symbol_count;
            if (output.m_dcvmBytecode.size() - offset < payload_size) {
                return std::unexpected(std::format("truncated bytecode payload for sid {:x}", sid));
            }

            recompiled_function function;
            function.m_sid = sid;
            function.m_instructions.resize(instruction_count);
            function.m_symbols.resize(symbol_count);
            function.m_pointerMap.reserve(symbol_count);
            function.m_originalPointerStrings.resize(symbol_count);

            if (instruction_bytes != 0) {
                std::memcpy(function.m_instructions.data(), output.m_dcvmBytecode.data() + offset, instruction_bytes);
                offset += instruction_bytes;
            }

            if (symbol_bytes != 0) {
                std::memcpy(function.m_symbols.data(), output.m_dcvmBytecode.data() + offset, symbol_bytes);
                offset += symbol_bytes;
            }

            for (u64 i = 0; i < symbol_count; ++i) {
                function.m_pointerMap.push_back(output.m_dcvmBytecode[offset] != 0);
                ++offset;
            }

            functions.push_back(std::move(function));
        }

        return functions;
    }

    bool is_primitive_string(const ast::full_type& type) {
        const ast::primitive_type* primitive = std::get_if<ast::primitive_type>(&type);
        return primitive != nullptr && primitive->m_type == ast::primitive_kind::STRING;
    }

    std::optional<std::string> static_string_for_instruction(const BinaryFile& file, const function_disassembly& function, const Instruction& instruction) {
        const u32 symbol_index = instruction.operand1;
        const auto& symbol_table = function.m_stackFrame.m_symbolTable;
        if (symbol_index >= symbol_table.m_types.size() || !is_primitive_string(symbol_table.m_types[symbol_index])) {
            return std::nullopt;
        }

        const u64 symbol_entry_offset = symbol_index * sizeof(u64);
        const char* string_pointer = symbol_table.m_location.get<const char*>(symbol_entry_offset);
        if (string_pointer < file.m_strings.as<char>() || string_pointer >= reinterpret_cast<const char*>(file.m_bytes.get() + file.m_size)) {
            return std::nullopt;
        }

        return std::string(string_pointer);
    }

    std::set<std::string> collect_static_strings(const BinaryFile& file, const function_disassembly& function) {
        std::set<std::string> strings;
        for (const function_disassembly_line& line : function.m_lines) {
            if (line.m_instruction.opcode != Opcode::LoadStaticPointerImm) {
                continue;
            }

            std::optional<std::string> string = static_string_for_instruction(file, function, line.m_instruction);
            if (string.has_value()) {
                strings.insert(std::move(*string));
            }
        }

        return strings;
    }

    std::map<std::string, function_snapshot> snapshot_functions(const BinaryFile& file, const std::vector<const function_disassembly*>& functions) {
        std::map<std::string, function_snapshot> snapshots;

        for (const function_disassembly* function : functions) {
            const auto* lambda = reinterpret_cast<const ScriptLambda*>(file.m_bytes.get() + function->m_originalOffset);
            function_snapshot snapshot;
            snapshot.m_id = function->get_id();
            snapshot.m_sid = SID(snapshot.m_id.c_str());
            snapshot.m_lambdaOffset = function->m_originalOffset;
            snapshot.m_instructionOffset = file_offset(file, lambda->m_pInstruction);
            snapshot.m_symbolOffset = file_offset(file, lambda->m_pSymbols);
            snapshot.m_stateScriptLambdaPointerOffset = function->m_stateScriptLambdaPointerOffset;
            snapshot.m_staticStrings = collect_static_strings(file, *function);

            auto [_, inserted] = snapshots.emplace(snapshot.m_id, std::move(snapshot));
            EXPECT_TRUE(inserted) << "duplicate function id in test fixture: " << function->get_id();
        }

        return snapshots;
    }

    const llvm_transpile::generated_outputs* find_output(const std::vector<llvm_transpile::generated_outputs>& outputs, std::string_view module_name) {
        for (const llvm_transpile::generated_outputs& output : outputs) {
            if (output.m_moduleName == module_name) {
                return &output;
            }
        }

        return nullptr;
    }

    void expect_valid_text_and_reloc_layout(const BinaryFile& file) {
        const u32 text_size = file.m_dcheader->m_textSize;
        ASSERT_LE(static_cast<u64>(text_size) + sizeof(u32), file.m_size);

        const u32 reloc_size = *reinterpret_cast<const u32*>(file.m_bytes.get() + text_size);
        const u32 expected_reloc_size = (text_size + 63) / 64;
        EXPECT_EQ(reloc_size, expected_reloc_size);
        EXPECT_EQ(file_offset(file, file.m_relocTable.as<std::byte>()), static_cast<u64>(text_size) + sizeof(u32));
        EXPECT_EQ(file.m_size, static_cast<u64>(text_size) + sizeof(u32) + reloc_size);
        EXPECT_GE(static_cast<u64>(reloc_size) * 64, text_size);
    }

    u64 lambda_symbol_count(const ScriptLambda& lambda) {
        if (lambda.m_sum < 12) {
            return 0;
        }

        const u64 instruction_and_symbol_count = (lambda.m_sum - 12) / 4;
        if (instruction_and_symbol_count < lambda.m_numInstructions) {
            return 0;
        }

        return instruction_and_symbol_count - lambda.m_numInstructions;
    }

    void expect_end_to_end_patch_preserves_functions_and_relocations(
        const std::filesystem::path& fixture_path,
        const std::string_view fixture_name,
        const std::vector<std::filesystem::path>& dependency_paths,
        const bool require_state_script,
        const bool require_moved_and_unmoved_string_symbols
    ) {
        if (!std::filesystem::exists(fixture_path)) {
            GTEST_SKIP() << fixture_name << " fixture not found: " << fixture_path.string()
                         << "; set TLOU2_DC1_DIR to the game's bin_unpacked/dc1 directory";
        }

        resstr<SIDBase> sidbase_result = SIDBase::from_binary(TEST_ROOT / "fixtures" / "dc" / "sidbase_new_fixed.bin");
        ASSERT_TRUE(sidbase_result.has_value()) << sidbase_result.error();
        const SIDBase& sidbase = *sidbase_result;

        resstr<BinaryFile> original_file_result = BinaryFile::from_path(fixture_path);
        ASSERT_TRUE(original_file_result.has_value()) << original_file_result.error();
        BinaryFile file = std::move(*original_file_result);

        Disassembler original_disassembler(&file, &sidbase);
        original_disassembler.disassemble();
        std::vector<const function_disassembly*> original_functions = original_disassembler.get_all_functions();
        ASSERT_FALSE(original_functions.empty());

        u64 standalone_function_count = 0;
        u64 state_script_function_count = 0;
        for (const function_disassembly* function : original_functions) {
            if (function->m_isScriptFunction) {
                ++state_script_function_count;
                ASSERT_TRUE(function->m_stateScriptLambdaPointerOffset.has_value()) << function->get_id();
            } else {
                ++standalone_function_count;
            }
        }
        EXPECT_GT(standalone_function_count, 0) << fixture_name;
        if (require_state_script) {
            ASSERT_TRUE(original_disassembler.has_state_script()) << fixture_name;
            EXPECT_GT(state_script_function_count, 0) << fixture_name;
        }

        const std::string module_name = fixture_path.filename().string();
        const std::map<std::string, function_snapshot> original_snapshots = snapshot_functions(file, original_functions);
        ASSERT_EQ(original_snapshots.size(), original_functions.size());
        const u64 old_text_size = file.m_dcheader->m_textSize;
        const u64 old_strings_start = file.m_dcheader->m_stringsOffset;

        llvm_transpile::llvm_transpiler transpiler(sidbase);
        transpiler.add_module(module_name, &file, original_functions);

        std::vector<std::unique_ptr<BinaryFile>> dependency_files;
        std::vector<std::unique_ptr<Disassembler>> dependency_disassemblers;
        dependency_files.reserve(dependency_paths.size());
        dependency_disassemblers.reserve(dependency_paths.size());
        for (const std::filesystem::path& dependency_path : dependency_paths) {
            if (!std::filesystem::exists(dependency_path)) {
                continue;
            }

            resstr<BinaryFile> dependency_file_result = BinaryFile::from_path(dependency_path);
            ASSERT_TRUE(dependency_file_result.has_value()) << dependency_file_result.error();
            dependency_files.push_back(std::make_unique<BinaryFile>(std::move(*dependency_file_result)));
            dependency_disassemblers.push_back(std::make_unique<Disassembler>(dependency_files.back().get(), &sidbase));
            dependency_disassemblers.back()->disassemble();
            transpiler.add_module(dependency_path.filename().string(), dependency_files.back().get(), dependency_disassemblers.back()->get_all_functions());
        }

        transpiler.enable_runtime_module();

        std::vector<llvm_transpile::generated_outputs> outputs = transpiler.run();
        const llvm_transpile::generated_outputs* module_output = find_output(outputs, module_name);
        ASSERT_NE(module_output, nullptr);
        ASSERT_FALSE(module_output->m_dcvmBytecode.empty());

        resstr<std::vector<recompiled_function>> recompiled_functions_result = parse_recompiled_functions(*module_output);
        ASSERT_TRUE(recompiled_functions_result.has_value()) << recompiled_functions_result.error();
        ASSERT_EQ(recompiled_functions_result->size(), original_functions.size());

        std::unordered_map<sid64, recompiled_function> recompiled_by_sid;
        recompiled_by_sid.reserve(recompiled_functions_result->size());
        for (recompiled_function& function : *recompiled_functions_result) {
            for (u64 i = 0; i < function.m_symbols.size(); ++i) {
                if (!function.m_pointerMap[i] || function.m_symbols[i] == 0) {
                    continue;
                }

                ASSERT_GE(function.m_symbols[i], old_strings_start) << "sid " << function.m_sid << " symbol " << i;
                ASSERT_LT(function.m_symbols[i], old_text_size) << "sid " << function.m_sid << " symbol " << i;
                function.m_originalPointerStrings[i] = std::string(reinterpret_cast<const char*>(file.m_bytes.get() + function.m_symbols[i]));
            }

            auto [_, inserted] = recompiled_by_sid.emplace(function.m_sid, std::move(function));
            EXPECT_TRUE(inserted) << "duplicate recompiled sid: " << function.m_sid;
        }

        std::vector<llvm_transpile::original_binfile> originals = {{
            module_name,
            &file,
            original_functions,
        }};
        if (errmsg error = llvm_transpile::llvm_transpiler::patch_original_binfiles(outputs, originals)) {
            FAIL() << *error;
        }

        expect_valid_text_and_reloc_layout(file);
        const u64 string_delta = file.m_dcheader->m_stringsOffset - old_strings_start;

        bool saw_moved_string_symbol_table = false;
        bool saw_unmoved_string_symbol_table = false;

        for (const auto& [function_id, original] : original_snapshots) {
            const auto recompiled_it = recompiled_by_sid.find(original.m_sid);
            ASSERT_NE(recompiled_it, recompiled_by_sid.end()) << "missing recompiled function after recompilation: " << function_id;
            const recompiled_function& recompiled = recompiled_it->second;

            const auto* lambda = reinterpret_cast<const ScriptLambda*>(file.m_bytes.get() + original.m_lambdaOffset);
            const u64 instruction_offset = file_offset(file, lambda->m_pInstruction);
            const u64 symbol_offset = file_offset(file, lambda->m_pSymbols);
            const u64 instruction_size = recompiled.m_instructions.size() * sizeof(Instruction);
            const u64 symbol_size = recompiled.m_symbols.size() * sizeof(u64);

            EXPECT_TRUE(reloc_bit_is_set(file, original.m_lambdaOffset + offsetof(ScriptLambda, m_pInstruction))) << function_id;
            EXPECT_TRUE(reloc_bit_is_set(file, original.m_lambdaOffset + offsetof(ScriptLambda, m_pSymbols))) << function_id;
            EXPECT_EQ(lambda->m_numInstructions, recompiled.m_instructions.size()) << function_id;
            EXPECT_EQ(lambda_symbol_count(*lambda), recompiled.m_symbols.size()) << function_id;
            EXPECT_EQ(lambda->m_sum, 12 + 4 * (recompiled.m_instructions.size() + recompiled.m_symbols.size())) << function_id;
            EXPECT_EQ(lambda->m_pSymbols, lambda->m_pInstruction + recompiled.m_instructions.size()) << function_id;
            EXPECT_LE(instruction_offset + instruction_size, file.m_dcheader->m_textSize) << function_id;
            EXPECT_LE(symbol_offset + symbol_size, file.m_dcheader->m_textSize) << function_id;
            if (original.m_stateScriptLambdaPointerOffset.has_value()) {
                const u64 slot_offset = *original.m_stateScriptLambdaPointerOffset;
                ASSERT_LE(slot_offset + sizeof(ScriptLambda*), file.m_dcheader->m_textSize) << function_id;
                EXPECT_TRUE(reloc_bit_is_set(file, slot_offset)) << function_id;
                const auto* lambda_slot = reinterpret_cast<ScriptLambda* const*>(file.m_bytes.get() + slot_offset);
                ASSERT_NE(*lambda_slot, nullptr) << function_id;
                EXPECT_EQ(file_offset(file, *lambda_slot), original.m_lambdaOffset) << function_id;
            }
            if (instruction_size != 0) {
                EXPECT_EQ(std::memcmp(lambda->m_pInstruction, recompiled.m_instructions.data(), instruction_size), 0) << function_id;
            }

            const bool moved_storage = instruction_offset != original.m_instructionOffset;
            bool function_has_string_symbol = false;

            for (u64 symbol_index = 0; symbol_index < recompiled.m_symbols.size(); ++symbol_index) {
                const u64 symbol_entry_offset = symbol_offset + symbol_index * sizeof(u64);
                const bool expected_pointer = recompiled.m_pointerMap[symbol_index] && recompiled.m_symbols[symbol_index] != 0;
                EXPECT_EQ(reloc_bit_is_set(file, symbol_entry_offset), expected_pointer) << function_id << " symbol " << symbol_index;

                if (!expected_pointer) {
                    EXPECT_EQ(lambda->m_pSymbols[symbol_index], recompiled.m_symbols[symbol_index]) << function_id << " symbol " << symbol_index;
                    continue;
                }

                function_has_string_symbol = true;
                ASSERT_TRUE(recompiled.m_originalPointerStrings[symbol_index].has_value()) << function_id << " symbol " << symbol_index;
                const char* string_pointer = reinterpret_cast<const char*>(lambda->m_pSymbols[symbol_index]);
                ASSERT_GE(string_pointer, file.m_strings.as<char>()) << function_id << " symbol " << symbol_index;
                ASSERT_LT(string_pointer, reinterpret_cast<const char*>(file.m_bytes.get() + file.m_dcheader->m_textSize)) << function_id << " symbol " << symbol_index;
                EXPECT_EQ(file_offset(file, string_pointer), recompiled.m_symbols[symbol_index] + string_delta) << function_id << " symbol " << symbol_index;
                EXPECT_EQ(std::string(string_pointer), *recompiled.m_originalPointerStrings[symbol_index]) << function_id << " symbol " << symbol_index;
            }

            if (function_has_string_symbol) {
                if (moved_storage) {
                    saw_moved_string_symbol_table = true;
                } else {
                    saw_unmoved_string_symbol_table = true;
                }
            }
        }

        if (require_moved_and_unmoved_string_symbols) {
            EXPECT_TRUE(saw_moved_string_symbol_table);
            EXPECT_TRUE(saw_unmoved_string_symbol_table);
        }
    }

    TEST(LLVM_RECOMPILE, WaveManagerEndToEndPatchPreservesFunctionsAndRelocations) {
        const std::filesystem::path fixture_path = wave_manager_fixture_path();
        expect_end_to_end_patch_preserves_functions_and_relocations(
            fixture_path,
            "wave-manager-funcs.bin",
            {fixture_path.parent_path().parent_path() / "anim-gas-mask-impl.bin"},
            false,
            false
        );
    }

    TEST(LLVM_RECOMPILE, AssaultManagerEndToEndPatchPreservesFunctionsStateScriptAndRelocations) {
        const std::filesystem::path fixture_path = assault_manager_fixture_path();
        expect_end_to_end_patch_preserves_functions_and_relocations(
            fixture_path,
            "ss-assault-manager.bin",
            {fixture_path.parent_path() / "wave-manager-funcs.bin"},
            true,
            false
        );
    }

    TEST(LLVM_RECOMPILE, NdScriptFuncsEndToEndPatchPreservesFunctionsAndRelocations) {
        const std::filesystem::path fixture_path = nd_script_funcs_fixture_path();
        expect_end_to_end_patch_preserves_functions_and_relocations(
            fixture_path,
            "nd-script-funcs.bin",
            {},
            false,
            false
        );
    }

}
