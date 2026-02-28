#include "compilation/program_binary_element.h"

namespace dconstruct::compilation {

program_binary_element::program_binary_element(const u64 size) noexcept {
    m_rawData.reserve(size);
    m_relocTable.reserve(size / 64);
}

void program_binary_element::insert_into_reloctable(const u8 bits, const u64 num_bits) noexcept {
    // const u8 bit_space_remaining = (8 - m_bitOffset % 8);
    // if (bit_space_remaining >= num_bits) {
    //     m_relocTable[m_byteOffset] |= bits << m_bitOffset;
    //     m_bitOffset += num_bits;
    //     assert(m_bitOffset <= 8);
    //     if (m_bitOffset == 8) {
    //         m_bitOffset = 0;
    //         m_byteOffset++;
    //     }
    // } else {
    //     m_relocTable[m_byteOffset++] |= bits << m_bitOffset;
    //     m_relocTable[m_byteOffset] |= bits >> bit_space_remaining;
    //     m_bitOffset = num_bits - bit_space_remaining;
    // }
    for (u64 i = 0; i < num_bits; ++i) {
        m_relocTable.push_back((bits >> i) & 0x1);
    }
}

void program_binary_element::insert_string_offset() noexcept {
    m_stringOffsets.emplace_back(m_rawData.size());
}

void program_binary_element::insert_string_offset(const u64 offset) noexcept {
    m_stringOffsets.emplace_back(m_rawData.size() + offset);
}

void program_binary_element::adjust_offsets(const u64 offset) noexcept {
    const u64 chunks = m_rawData.size() / sizeof(u64);
    for (u64 i = 0; i < chunks; ++i) {
        if (m_relocTable[i]) {
            u64* ptr = reinterpret_cast<u64*>(m_rawData.data() + i * sizeof(u64));
            if (*ptr != 0) {
                *ptr += offset;
            }
        }
    }
}

}
