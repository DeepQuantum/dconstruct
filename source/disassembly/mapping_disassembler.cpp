#include "disassembly/mapping_disassembler.h"
#include <cmath>

namespace dconstruct {

    [[nodiscard]] bool MappingDisassembler::is_unmapped_sid(const location loc) const noexcept {
        const bool in_range = loc.get<sid64>() >= m_sidbase->m_lowestSid && loc.get<sid64>() <= m_sidbase->m_highestSid;
        const bool is_file_ptr = m_currentFile->is_file_ptr(loc);
        return loc.is_aligned() && in_range && !is_file_ptr;
    }

    [[nodiscard]] bool MappingDisassembler::is_possible_float(const f32* value) const noexcept {
        f32 rounded = std::roundf(*value * 1e4f) / 1e4f;
        return std::fabsf(*value - rounded) < 1e-4f && *value > -1e4f && *value < 1e4f && rounded != 0.f;
    }

    [[nodiscard]] bool MappingDisassembler::is_possible_i32(const i32* value) const noexcept {
        return std::abs(*value) < 50000;
    }

    [[nodiscard]] u32 MappingDisassembler::get_size_array(const location array) const {
        u32 size_array_front = array.get<u32>(8);
        const u16 size_array_front_u16_raw = array.get<u16>(8);
        const u16 size_array_front_u16_swapped = static_cast<u16>((size_array_front_u16_raw << 8) | (size_array_front_u16_raw >> 8));
        u32 size_array_back = array.get<u32>(-8);
        constexpr u32 max_allowed_size_array = 100'000;
        const bool front_u16_raw_plausible = size_array_front_u16_raw > 0 && size_array_front_u16_raw <= max_allowed_size_array;
        const bool front_u16_swapped_plausible = size_array_front_u16_swapped > 0 && size_array_front_u16_swapped <= max_allowed_size_array;
        u32 size_array_front_u16 = size_array_front_u16_raw;
        if (!front_u16_raw_plausible && front_u16_swapped_plausible) {
            size_array_front_u16 = size_array_front_u16_swapped;
        }
        const bool front_u16_plausible = size_array_front_u16 > 0 && size_array_front_u16 <= max_allowed_size_array;
        const bool front_u32_invalid = size_array_front == 0 || size_array_front > max_allowed_size_array;
        const bool front_u32_significantly_larger = front_u16_plausible && size_array_front >= size_array_front_u16 * 8 && (size_array_front - size_array_front_u16) >= 256;
        if (front_u16_plausible && (front_u32_invalid || front_u32_significantly_larger)) {
            size_array_front = size_array_front_u16;
        }
        if (size_array_front > max_allowed_size_array) {
            if (size_array_back > max_allowed_size_array) {
                return 0;
            }
            return size_array_back;
        }
        return size_array_front;
    }

    void MappingDisassembler::discover_anonymous_array(const location anon_array) {
        const u32 anonymous_array_size = anon_array.get<u32>(8);
        if (anonymous_array_size > 100'000) {
            return;
        }
        discover_array(anon_array, anonymous_array_size);
    }

    void MappingDisassembler::discover_array(const location array, const u32 array_size) {
        if (array_size == 0) {
            return;
        }
    }

    [[nodiscard]] u8 MappingDisassembler::discover_struct_or_arraylike(const location struct_location) {
        u8 bytes_inserted = 0;
        if (m_currentFile->is_file_ptr(struct_location)) {
            if (m_currentFile->is_string(location().from(struct_location))) {
                return 8;
            }
            const location next_struct_header = location().from(struct_location, -8);
            if (!next_struct_header.is_aligned() || !is_unmapped_sid(next_struct_header)) {
                discover_anonymous_array(struct_location);
            } else if (next_struct_header.get<sid64>() == SID("array")) {
                discover_array(struct_location, get_size_array(struct_location));
            }
            bytes_inserted = 8;
        } else {
            if (struct_location >= m_currentFile->m_strings) {
                bytes_inserted = 8;
            } else {
                bytes_inserted = discover_next_struct_member(struct_location);
            }
        }
        return bytes_inserted;
    }

    [[nodiscard]] u8 MappingDisassembler::discover_next_struct_member(const location member_loc) {
        if (m_currentFile->is_file_ptr(member_loc)) {
            if (member_loc >= m_currentFile->m_strings) {
                return 8;
            }
            return discover_struct_or_arraylike(member_loc);
        }

        if (m_sidbase->search(member_loc.get<sid64>()) != nullptr) {
            return 8;
        }

        if (is_possible_float(member_loc.as<f32>())) {
            return 4;
        }

        if (is_possible_i32(member_loc.as<i32>())) {
            return 4;
        }

        if (is_unmapped_sid(member_loc)) {
            return 8;
        }

        return 4;
    }

    [[nodiscard]] std::optional<sid64> MappingDisassembler::try_get_pointed_struct_type(const location ptr_value) const noexcept {
        if (ptr_value.num() < reinterpret_cast<p64>(m_currentFile->m_bytes.get()) || ptr_value.num() >= reinterpret_cast<p64>(m_currentFile->m_bytes.get() + m_currentFile->m_size)) {
            return std::nullopt;
        }

        const location struct_header = ptr_value - 8;
        if (!struct_header.is_aligned()) {
            return std::nullopt;
        }

        if (!is_unmapped_sid(struct_header)) {
            return std::nullopt;
        }

        return struct_header.get<sid64>();
    }

    void MappingDisassembler::map_temporary_members(const sid64 struct_type_id, const location member_start, const u32 inferred_size) {
        m_registry.observe_struct(struct_type_id, inferred_size);

        if (inferred_size == 0 || inferred_size > 5'000) {
            return;
        }

        u32 offset = 0;
        while (offset < inferred_size) {
            const location current = member_start + offset;
            if (offset % 8 == 0 && m_currentFile->is_file_ptr(current)) {
                const location pointed_to = location(current.get<const void*>());
                const std::optional<sid64> pointed_type_id = try_get_pointed_struct_type(pointed_to);
                m_registry.observe_pointer_member(struct_type_id, offset, 8, pointed_type_id);
                if (pointed_type_id.has_value()) {
                    const auto* pointed_struct = (pointed_to - 8).as<structs::unmapped>();
                    observe_unmapped_struct(pointed_struct, *pointed_type_id);
                }
                offset += 8;
            } else {
                m_registry.observe_primitive_member(struct_type_id, offset, 2, ast::primitive_kind::U16, current.get<u16>());
                offset += 2;
            }
        }
    }

    void MappingDisassembler::observe_unmapped_struct(const structs::unmapped* struct_ptr, const sid64 type_id) {
        if (type_id == 0) {
            return;
        }

        const p64 struct_ptr_num = reinterpret_cast<p64>(struct_ptr);
        if (m_registry.get_struct(type_id).has_value() || m_visitedStructs.contains(struct_ptr_num)) {
            return;
        }
        m_visitedStructs.emplace(struct_ptr_num);

        const sid64 struct_type_id = type_id != 0 ? type_id : struct_ptr->typeID;
        u64 member_offset = 0;
        u64 last_member_size = 0;
        bool offset_gets_pointed_at = false;
        const location member_start = location(&struct_ptr->m_data);
        location member_location = member_start;

        while (!offset_gets_pointed_at) {
            member_offset += last_member_size;
            const location current_member = member_start + member_offset;
            last_member_size = discover_next_struct_member(current_member);
            member_location = member_start + (member_offset + last_member_size);
            offset_gets_pointed_at = m_currentFile->gets_pointed_at(member_location + 8) || m_currentFile->is_string(member_location);
        }

        const u32 inferred_size = static_cast<u32>((member_location.num() - member_start.num()));
        map_temporary_members(struct_type_id, member_start, inferred_size);
    }

    void MappingDisassembler::ingest() {
        for (i32 i = 0; i < m_currentFile->m_dcheader->m_numEntries; ++i) {
            const Entry* entry = m_currentFile->m_dcheader->m_pStartOfData + i;
            const auto* struct_ptr = reinterpret_cast<const structs::unmapped*>(reinterpret_cast<const u64*>(entry->m_entryPtr) - 1);
            if (m_registry.get_struct(entry->m_typeId).has_value() || m_registry.get_struct(struct_ptr->typeID).has_value()) {
                continue;
            }
            observe_unmapped_struct(struct_ptr, entry->m_typeId);
        }
    }
}
