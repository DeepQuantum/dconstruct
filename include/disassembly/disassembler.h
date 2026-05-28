#pragma once

#include "base.h"
#include "binaryfile.h"
#include "instructions.h"
#include "custom_structs.h"
#include <vector>
#include <unordered_map>

namespace dconstruct {
    struct DisassemblerOptions {
        u8 m_indentPerLevel = 2;
        bool m_emitOnce = false;
        bool m_verbose = false;
    };


    enum class game_type : u8 {
        T2R,
        T1X,
        UC4,
    };
    

    class Disassembler {
    public:

        Disassembler(BinaryFile* file, const SIDBase* sidbase, const game_type game = game_type::T2R) noexcept : m_currentFile(file), m_sidbase(sidbase), m_game(game) {}

        void disassemble();
        virtual ~Disassembler() {};
        [[nodiscard]] function_disassembly create_function_disassembly(const ScriptLambda* lambda, function_name_variant name, const bool is_script_function = false);
        [[nodiscard]] function_disassembly create_function_disassembly(std::vector<Instruction>&&, function_name_variant, const location& symbol_table, const bool is_script_function = false);

        [[nodiscard]] const std::vector<const function_disassembly*> get_all_functions() noexcept;

        [[nodiscard]] std::vector<const function_disassembly*> get_named_functions() const noexcept;
        [[nodiscard]] bool has_state_script() const noexcept {
            return m_hasStateScript;
        }

        void disassemble_functions_from_bin_file();
        std::unordered_map<u64, std::vector<std::string>> m_offsetsToFunctionNames;

    protected:
        virtual void insert_span(const char* text, const u32 indent = 0) {};
        
        std::map<sid64, std::vector<const structs::unmapped*>> m_unmappedEntries;
        BinaryFile* m_currentFile = nullptr;
        const SIDBase* m_sidbase = nullptr;
        DisassemblerOptions m_options;
        std::vector<function_disassembly> m_functions;
        embedded_function_id m_currentEmbeddedFunctionId;
        game_type m_game = game_type::T2R;
        bool m_hasStateScript = false;


        FILE* m_perfFile = nullptr;

        void insert_entry(const Entry* entry);
        void insert_struct(const structs::unmapped* entry, const u32 indent = 0, const sid64 name_id = 0, const sid64 type_id = 0);
        template<typename... Args>
        void insert_span_fmt(const char* format, Args ...args);
        template<typename... Args>
        void insert_span_indent(const char*, const u32, Args ...);
        [[nodiscard]] const char* lookup(const sid64 hash);
        [[nodiscard]] const char* lookup(const sid32 hash);
        [[nodiscard]] bool is_unmapped_sid(const location) const noexcept;
        [[nodiscard]] bool is_possible_float(const f32* ptr) const noexcept;
        [[nodiscard]] bool is_possible_i32(const i32* ptr) const noexcept;
        u8 insert_struct_or_arraylike(const location, const u32);
        [[nodiscard]] u32 get_size_array(const location, const u32);
        void insert_anonymous_array(const location, const u32);
        void insert_array(const location, const u32, const u32);
        void insert_state_script(const StateScript*, const u32);
        void insert_state_script_verbose_fields(const StateScript*, const u32);
        void insert_ss_declaration_list_verbose_fields(const SsDeclarationList*, const u32);
        void insert_ss_declaration_verbose_fields(const SsDeclaration*, const u32);
        void insert_ss_options_verbose_fields(const SsOptions*, const u32);
        void insert_symbol_array_verbose_fields(const SymbolArray*, const u32);
        void insert_ss_state_verbose_fields(const SsState*, const u32);
        void insert_ss_track_group_verbose_fields(const SsTrackGroup*, const u32);
        void insert_ss_on_block_verbose_fields(const SsOnBlock*, const u32);
        void insert_ss_track_verbose_fields(const SsTrack*, const u32);
        void insert_ss_lambda_verbose_fields(const SsLambda*, const u32);
        void insert_script_lambda_verbose_fields(const ScriptLambda*, const u32);
        void insert_unmapped_struct(const structs::unmapped*, const u32);
        u8 insert_next_struct_member(const location, const u32);
        void insert_variable(const SsDeclaration* var, const u32);
        void insert_on_block(const SsOnBlock* block, const u32, state_script_function_id& state_name);
        void set_register_types(Register&, Register&, const ast::full_type type);
        void process_instruction(const u32, function_disassembly &);
        void insert_function_disassembly_text(const function_disassembly& functionDisassembly, const u32 indent);
        void insert_label(const std::vector<u32>& labels, const function_disassembly_line& line, const u32 func_size, const u32 indent);
        void insert_goto_label(const std::vector<u32>& labels, const function_disassembly_line& line, const u32 func_size, const std::vector<function_disassembly_line>& lines);
        [[nodiscard]] u32 get_offset(const location) const noexcept;
        [[nodiscard]] u32 get_offset(const void*) const noexcept;
        [[nodiscard]] bool pointer_gets_called(const u32, const u32, const function_disassembly&) const;
        

        static constexpr u32 INTERPRETED_BUFFER_SIZE = 512;
        static constexpr u32 DISASSEMBLY_BUFFER_SIZE = 256;
    };

    const static std::unordered_map<sid64, ast::function_type> builtinFunctions = {
        {SID("get-int32"), ast::make_function(make_type_from_prim(ast::primitive_kind::I32), ast::function_type::DISTANCE::NEAR, {
            {"symbol", make_type_from_prim(ast::primitive_kind::SID)},
            {"location", make_type_from_prim(ast::primitive_kind::SID)}
        })},
        {SID("get-boolean"), ast::make_function(make_type_from_prim(ast::primitive_kind::BOOL), ast::function_type::DISTANCE::NEAR, {
            {"symbol", make_type_from_prim(ast::primitive_kind::SID)},
            {"location", make_type_from_prim(ast::primitive_kind::SID)}
        })}
    };
}
