#pragma once

#include "base.h"
#include "DCHeader.h"
#include "DCScript.h"

namespace dconstruct::compilation {
    struct function;
    struct global_state;


    struct program_binary_element {

        program_binary_element(const u64 size) noexcept {
            m_rawData.reserve(size);
            m_relocTable.resize(size / 8 + 1);
        }

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

        void insert_into_reloctable(const u8 bits, const u64 num_bits) noexcept {
            const u8 bit_space_remaining = (8 - m_bitOffset % 8);
            if (bit_space_remaining >= num_bits) {
                m_relocTable[m_byteOffset] |= bits << m_bitOffset;
                m_bitOffset += num_bits;
                assert(m_bitOffset <= 8);
                if (m_bitOffset == 8) {
                    m_bitOffset = 0;
                    m_byteOffset++;
                }
            } else {
                m_relocTable[m_byteOffset++] |= bits << m_bitOffset;
                m_relocTable[m_byteOffset] |= bits >> bit_space_remaining;
                m_bitOffset = num_bits - bit_space_remaining;
            }
        }

        void insert_string_offset(const u64 index) noexcept {
            m_stringOffsets.emplace_back(m_currentSize, index);
        }


        Entry m_entry;
        std::vector<std::byte> m_rawData;
        std::vector<std::pair<u64, u64>> m_stringOffsets;
        std::vector<u8> m_relocTable;
    

        u64 m_currentSize = 0;
        u64 m_byteOffset = 0;
        u8 m_bitOffset = 0;
    };
}