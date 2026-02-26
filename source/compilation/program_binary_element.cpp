#include "compilation/program_binary_element.h"

namespace dconstruct::compilation {

program_binary_element::program_binary_element(const u64 size) noexcept {
    m_rawData.reserve(size);
    m_relocTable.resize(size / 8 + 1);
}

void program_binary_element::insert_into_reloctable(const u8 bits, const u64 num_bits) noexcept {
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

void program_binary_element::insert_string_offset(const u64 index) noexcept {
    m_stringOffsets.emplace_back(m_rawData.size(), index);
}

void program_binary_element::insert_string_offset(const u64 index, const u64 offset) noexcept {
    m_stringOffsets.emplace_back(m_rawData.size() + offset, index);
}

void program_binary_element::adjust_offsets(const u64 offset) noexcept {
    const u64 chunks = m_rawData.size() / sizeof(u64);
    for (u64 i = 0; i < chunks; ++i) {
        if (m_relocTable[i / 8] & (1 << (i % 8))) {
            *reinterpret_cast<u64*>(m_rawData.data() + i * sizeof(u64)) += offset;
        }
    }
}

}
