#pragma once

#include "base.h"
#include "DCHeader.h"
#include "DCScript.h"

namespace dconstruct::compilation {
    struct function;
    struct global_state;


    struct program_binary_element {

        program_binary_element(const u64 size) noexcept;

        template<typename T, typename ... bits>
        void push_bytes(const T& data, bits... b) noexcept {
            const std::byte* p = reinterpret_cast<const std::byte*>(std::addressof(data));
            m_rawData.insert(m_rawData.end(), p, p + sizeof(T));
            const std::vector<u8> bits_list = {static_cast<u8>(b)...};
            for (u32 i = 0; i < bits_list.size() - 1; ++i) {
                insert_into_reloctable(bits_list[i], 8);
            }
            insert_into_reloctable(bits_list.back(), (sizeof(T) / 8) % 8);
        }

        void insert_into_reloctable(const u8 bits, const u64 num_bits) noexcept;

        void insert_string_offset() noexcept;
        void insert_string_offset(const u64 offset) noexcept;

        void adjust_offsets(const u64 offset) noexcept;


        Entry m_entry;
        std::vector<std::byte> m_rawData;
        std::vector<u64> m_stringOffsets;
        std::vector<bool> m_relocTable;
    

        u64 m_byteOffset = 0;
        u8 m_bitOffset = 0;
    };
}