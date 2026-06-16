#pragma once

#include "base.h"
#include "binaryfile.h"
#include "instructions.h"
#include "custom_structs.h"
#include "ast/state_script/state_script.h"
#include <string>
#include <vector>
#include <variant>
#include <memory>
#include <unordered_map>

namespace dconstruct {
    enum class game_type : u8 {
        T2R,
        T1X,
        UC4,
    };

    struct disassembled_value;

    struct mapped_value;

    struct string_value {
        const char* m_chars = nullptr;
        std::optional<u64> m_pointerOffset;
    };

    using disassembled_value_content = std::variant<disassembled_value, mapped_value, std::shared_ptr<function_disassembly>, const ast::state_script*, const u8*, const u16*, const u32*, const i32*, const u64*, const f32*, string_value, const structs::map*>;

    using disassembled_values_t = std::vector<disassembled_value_content>;

    struct disassembled_value {
        sid64 m_typeId;
        p64 m_offset;
        disassembled_values_t m_values;
        std::optional<u64> m_pointerOffset;
    };

    struct mapped_value {
        std::string m_name;
        std::unique_ptr<disassembled_value_content> m_value;
    };

    struct disassembled_entry {
        sid64 m_nameId;
        sid64 m_typeId;
        p64 m_offset;
        disassembled_values_t m_values;
    };

    class Disassembler {
    public:
        Disassembler(BinaryFile* file, const SIDBase* sidbase, const game_type game = game_type::T2R) noexcept
            : m_currentFile(file), m_sidbase(sidbase), m_game(game) {};

        Disassembler(BinaryFile* file, const SIDBase* sidbase, const std::unordered_map<sid64, ast::full_type>* known_types, const game_type game = game_type::T2R) noexcept
            : m_currentFile(file), m_sidbase(sidbase), m_knownTypes(known_types), m_game(game) {};

        const std::vector<disassembled_entry>& disassemble();
        [[nodiscard]] std::string disassembly_to_string(const std::vector<disassembled_entry>& entries);
        [[nodiscard]] const std::vector<disassembled_entry>& get_disassembled_entries() const noexcept {
            return m_disassembledEntries;
        }
        virtual ~Disassembler() = default;

        template <typename T>
        [[nodiscard]] const T* get_value_ptr_at(const u64 offset) const noexcept {
            return reinterpret_cast<const T*>(&this->m_currentFile->m_bytes[offset]);
        }

        [[nodiscard]] std::shared_ptr<function_disassembly> create_function_disassembly(const ScriptLambda* lambda, function_name_variant name, const bool is_script_function = false);


        [[nodiscard]] static std::shared_ptr<function_disassembly> create_function_disassembly(
            const Instruction* istr_ptr,
            const u64 num_instructions,
            function_name_variant,
            const location& symbol_table,
            const u64 instruction_base_offset,
            const u64 instruction_stride,
            const game_type game,
            const SIDBase& sidbase,
            std::map<sid64, std::string>& sidcache,
            const bool is_script_function,
            const location& strings_offset
        );

        [[nodiscard]] const std::vector<const function_disassembly*> get_all_functions() noexcept;

        [[nodiscard]] std::vector<const function_disassembly*> get_named_functions() const noexcept;

        [[nodiscard]] const function_disassembly* get_function_at_offset(const u64 offset) const noexcept;

        [[nodiscard]] bool has_state_script() const noexcept {
            return m_stateScript.has_value();
        }

        [[nodiscard]] ast::state_script* get_state_script() noexcept {
            return m_stateScript.has_value() ? &*m_stateScript : nullptr;
        }

        void disassemble_functions_from_bin_file();
        std::unordered_map<u64, std::vector<std::string>> m_offsetsToFunctionNames;

    protected:
        inline static const std::unordered_map<sid64, ast::full_type> s_emptyTypes;
        static constexpr u32 INTERPRETED_BUFFER_SIZE = 512;
        static constexpr u32 DISASSEMBLY_BUFFER_SIZE = 256;

        disassembled_entry insert_entry(const Entry* entry);
        disassembled_value insert_struct(const structs::unmapped* entry, const sid64 name_id = 0, const sid64 type_id = 0);
        [[nodiscard]] bool is_unmapped_sid(const location) const noexcept;
        [[nodiscard]] bool is_possible_float(const f32* ptr) const noexcept;
        [[nodiscard]] bool is_possible_i32(const i32* ptr) const noexcept;
        u8 insert_struct_or_arraylike(const location, disassembled_values_t&);
        [[nodiscard]] u32 get_size_array(const location);
        void insert_anonymous_array(const location, disassembled_values_t&);
        [[nodiscard]] disassembled_value insert_array(const location, const u32, u32 struct_size = 0);
        [[nodiscard]] ast::state_script insert_state_script(const StateScript*);
        [[nodiscard]] disassembled_values_t insert_mapped_struct(const location, const std::pair<sid64, ast::full_type>&);
        void insert_unmapped_struct(const structs::unmapped*, disassembled_values_t&);
        u8 insert_next_struct_member(const location, disassembled_values_t&);
        [[nodiscard]] u8 insert_next_struct_member(const location, const ast::full_type&, disassembled_values_t& values);
        [[nodiscard]] std::optional<ast::variable_declaration> insert_variable(const SsDeclaration* var);
        [[nodiscard]] ast::state_script_block insert_on_block(const SsOnBlock* block, state_script_function_id& state_name);
        static void set_register_types(Register&, Register&, const ast::full_type type);
        static void process_instruction(const u32, function_disassembly&, const u64, const game_type, const SIDBase&, std::map<sid64, std::string>&, const location);
        [[nodiscard]] u32 get_offset(const location) const noexcept;
        [[nodiscard]] u32 get_offset(const void*) const noexcept;
        [[nodiscard]] static bool pointer_gets_called(const u32, const u32, const function_disassembly&);

        std::optional<ast::state_script> m_stateScript;
        embedded_function_id m_currentEmbeddedFunctionId;
        std::vector<std::shared_ptr<function_disassembly>> m_functions;
        std::vector<disassembled_entry> m_disassembledEntries;
        std::map<sid64, std::vector<const structs::unmapped*>> m_unmappedEntries;
        const std::unordered_map<sid64, ast::full_type>* m_knownTypes = &s_emptyTypes;
        const SIDBase* m_sidbase = nullptr;
        BinaryFile* m_currentFile = nullptr;
        game_type m_game = game_type::T2R;
    };
}
