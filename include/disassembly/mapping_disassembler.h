#pragma once

#include "disassembly/disassembler.h"
#include "disassembly/custom_structs.h"
#include "disassembly/mapping_registry.h"
#include <set>

namespace dconstruct {

    class MappingDisassembler : public Disassembler {
    public:
        MappingDisassembler(BinaryFile* file, const SIDBase* sidbase, MappingRegistry& registry, const DisassemblerOptions& options = DisassemblerOptions{}, const game_type game = game_type::T2R) noexcept : 
        Disassembler(file, sidbase, game), m_registry(registry) {
            m_options = options;
        }

        ~MappingDisassembler() override = default;

        void ingest();

    private:
        [[nodiscard]] bool is_unmapped_sid(const location loc) const noexcept;
        [[nodiscard]] bool is_possible_float(const f32* value) const noexcept;
        [[nodiscard]] bool is_possible_i32(const i32* value) const noexcept;
        [[nodiscard]] u32 get_size_array(const location array) const;
        [[nodiscard]] u8 discover_next_struct_member(const location member_loc);
        [[nodiscard]] u8 discover_struct_or_arraylike(const location struct_location);
        void discover_anonymous_array(const location anon_array);
        void discover_array(const location array, const u32 array_size);
        [[nodiscard]] std::optional<sid64> try_get_pointed_struct_type(const location ptr_value) const noexcept;
        void map_temporary_members(const sid64 struct_type_id, const location member_start, const u32 inferred_size);
        void observe_unmapped_struct(const structs::unmapped* struct_ptr, const sid64 type_id = 0);
        void insert_span(const char* text, const u32 indent = 0, const TextFormat& text_format = TextFormat{}) override {}

        MappingRegistry& m_registry;
        std::set<p64> m_visitedStructs;
    };
}
