#include "compilation/target_patcher.h"
#include <algorithm>
#include <cstring>
#include <format>

namespace dconstruct::compilation {

    resstr<std::pair<std::unique_ptr<std::byte[]>, u64>> patch_target_binary(
        const BinaryFile& file,
        std::vector<program_binary_element> elements,
        const global_state& global
    ) noexcept {
        constexpr u64 SLOT_SIZE = sizeof(u64);
        constexpr u64 RELOC_BYTE_COVERAGE = 64;

        const auto align_up = [](const u64 value, const u64 alignment) noexcept -> u64 {
            return (value + alignment - 1) & ~(alignment - 1);
        };

        const auto reloc_bit_set = [](const u8* table, const u64 slot) noexcept -> bool {
            return (table[slot / 8] & (1 << (slot % 8))) != 0;
        };

        const auto set_reloc_bit = [](u8* table, const u64 slot) noexcept {
            table[slot / 8] |= static_cast<u8>(1 << (slot % 8));
        };

        const auto read_u64 = [](const std::byte* bytes, const u64 offset) noexcept -> u64 {
            u64 value;
            std::memcpy(&value, bytes + offset, sizeof(value));
            return value;
        };

        const auto write = []<typename T>(std::byte* bytes, const u64 offset, const T value) noexcept {
            std::memcpy(bytes + offset, &value, sizeof(value));
        };

        if (elements.empty()) {
            return std::unexpected{"patching requires at least one compiled element"};
        }

        const BinaryFile::byte_uptr unmapped = file.get_unmapped();
        const std::byte* old_bytes = unmapped.get();
        const auto* old_header = reinterpret_cast<const DC_Header*>(old_bytes);

        const u64 old_text_size = old_header->m_textSize;
        const u64 old_strings_start = old_header->m_stringsOffset;
        const u32 num_entries = old_header->m_numEntries;

        if (old_strings_start < sizeof(DC_Header) || old_strings_start > old_text_size) {
            return std::unexpected{std::format("the target's string table offset {:#x} doesn't lie within the text segment of size {:#x}", old_strings_start, old_text_size)};
        }
        if (old_text_size + sizeof(u32) > file.m_size) {
            return std::unexpected{"the target's text segment size exceeds the file size"};
        }
        u32 old_reloc_size;
        std::memcpy(&old_reloc_size, old_bytes + old_text_size, sizeof(old_reloc_size));
        if (old_text_size + sizeof(u32) + old_reloc_size > file.m_size) {
            return std::unexpected{"the target's reloc table size exceeds the file size"};
        }
        const u8* old_reloc = reinterpret_cast<const u8*>(old_bytes + old_text_size + sizeof(u32));

        const u64 old_slot_count = u64{old_reloc_size} * 8;
        for (u64 slot = old_strings_start / SLOT_SIZE; slot < old_slot_count; ++slot) {
            if (slot * SLOT_SIZE >= old_strings_start && reloc_bit_set(old_reloc, slot)) {
                return std::unexpected{std::format("the target's string table region contains a relocated pointer at {:#x}, refusing to move it", slot * SLOT_SIZE)};
            }
        }

        const u64 entry_table_offset = read_u64(old_bytes, offsetof(DC_Header, m_pStartOfData));
        if (entry_table_offset + u64{num_entries} * sizeof(Entry) > old_strings_start) {
            return std::unexpected{"the target's entry table doesn't lie within the data region"};
        }

        std::vector<u64> element_bases;
        element_bases.reserve(elements.size());
        u64 running_offset = align_up(old_strings_start, SLOT_SIZE);
        for (const program_binary_element& element : elements) {
            element_bases.push_back(running_offset);
            running_offset += align_up(element.m_rawData.size(), SLOT_SIZE);
        }

        const u64 tombstone_base = running_offset;
        running_offset += elements.size() * SLOT_SIZE;

        const u64 new_strings_start = running_offset;
        const u64 old_strings_size = old_text_size - old_strings_start;
        u64 new_strings_size = 0;
        for (const std::string& string : global.m_strings) {
            new_strings_size += string.size() + 1;
        }

        const u64 unpadded_text_size = new_strings_start + old_strings_size + new_strings_size;
        const u64 new_text_size = align_up(unpadded_text_size, 4);
        const u32 new_reloc_size = static_cast<u32>((unpadded_text_size + RELOC_BYTE_COVERAGE - 1) / RELOC_BYTE_COVERAGE);
        const u64 total_size = new_text_size + sizeof(u32) + new_reloc_size;

        auto out = std::make_unique<std::byte[]>(total_size);
        std::memcpy(out.get(), old_bytes, old_strings_start);

        const u64 string_delta = new_strings_start - old_strings_start;
        const u64 data_slot_count = std::min(old_slot_count, old_strings_start / SLOT_SIZE);
        for (u64 slot = 0; slot < data_slot_count; ++slot) {
            if (!reloc_bit_set(old_reloc, slot)) {
                continue;
            }
            const u64 value = read_u64(out.get(), slot * SLOT_SIZE);
            if (value >= old_strings_start) {
                write(out.get(), slot * SLOT_SIZE, value + string_delta);
            }
        }

        std::memcpy(out.get() + new_strings_start, old_bytes + old_strings_start, old_strings_size);
        std::vector<u64> string_locations;
        string_locations.reserve(global.m_strings.size());
        u64 string_cursor = new_strings_start + old_strings_size;
        for (const std::string& string : global.m_strings) {
            string_locations.push_back(string_cursor);
            std::memcpy(out.get() + string_cursor, string.c_str(), string.size() + 1);
            string_cursor += string.size() + 1;
        }

        write(out.get(), new_text_size, new_reloc_size);
        u8* new_reloc = reinterpret_cast<u8*>(out.get() + new_text_size + sizeof(u32));
        std::memcpy(new_reloc, old_reloc, std::min<u64>(old_reloc_size, new_reloc_size));

        for (u64 element_index = 0; element_index < elements.size(); ++element_index) {
            program_binary_element& element = elements[element_index];
            const u64 base = element_bases[element_index];
            std::byte* raw = element.m_rawData.data();

            for (const u64 offset : element.m_stringOffsets) {
                const u64 string_index = read_u64(raw, offset);
                if (string_index >= string_locations.size()) {
                    return std::unexpected{std::format("compiled entry #{:016X} references an out-of-range string index {}", element.m_entry.m_nameID, string_index)};
                }
                write(raw, offset, string_locations[string_index] - base);
            }
            for (const u64 offset : element.m_absoluteOffsets) {
                write(raw, offset, read_u64(raw, offset) - base);
            }
            element.adjust_offsets(base);
            std::memcpy(out.get() + base, raw, element.m_rawData.size());

            const u64 chunk_count = std::min<u64>(element.m_rawData.size() / SLOT_SIZE, element.m_relocTable.size());
            for (u64 chunk = 0; chunk < chunk_count; ++chunk) {
                if (element.m_relocTable[chunk] && read_u64(out.get(), base + chunk * SLOT_SIZE) != 0) {
                    set_reloc_bit(new_reloc, base / SLOT_SIZE + chunk);
                }
            }

            bool repointed = false;
            for (u32 entry_index = 0; entry_index < num_entries; ++entry_index) {
                const u64 entry_offset = entry_table_offset + entry_index * sizeof(Entry);
                if (read_u64(out.get(), entry_offset + offsetof(Entry, m_nameID)) != element.m_entry.m_nameID) {
                    continue;
                }
                const u64 entry_type = read_u64(out.get(), entry_offset + offsetof(Entry, m_typeId));
                if (element.m_entry.m_typeId != 0 && entry_type != element.m_entry.m_typeId) {
                    return std::unexpected{std::format("entry #{:016X} has type #{:016X} but the compiled replacement has type #{:016X}", element.m_entry.m_nameID, entry_type, element.m_entry.m_typeId)};
                }
                const u64 old_entry_ptr = read_u64(out.get(), entry_offset + offsetof(Entry, m_entryPtr));
                write(out.get(), entry_offset + offsetof(Entry, m_entryPtr), base + sizeof(sid64));
                if (old_entry_ptr != 0) {
                    const u64 tombstone_offset = tombstone_base + element_index * SLOT_SIZE;
                    write(out.get(), tombstone_offset, old_entry_ptr);
                    set_reloc_bit(new_reloc, tombstone_offset / SLOT_SIZE);
                }
                repointed = true;
                break;
            }
            if (!repointed) {
                return std::unexpected{std::format("the target has no entry named #{:016X}, only existing entries can be replaced", element.m_entry.m_nameID)};
            }
        }

        write(out.get(), offsetof(DC_Header, m_textSize), static_cast<u32>(new_text_size));
        write(out.get(), offsetof(DC_Header, m_stringsOffset), static_cast<u32>(new_strings_start));

        resstr<std::pair<std::unique_ptr<std::byte[]>, u64>> result;
        result.emplace(std::move(out), total_size);
        return result;
    }

}
