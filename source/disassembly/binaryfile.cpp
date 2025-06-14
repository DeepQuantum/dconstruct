#include "binaryfile.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <immintrin.h>
#include <cstring>
#include <chrono>

BinaryFile::BinaryFile() {}

BinaryFile::BinaryFile(const std::string &path) {
    std::ifstream scriptstream(path, std::ios::binary);

    if (!scriptstream.is_open()) {
        throw std::runtime_error("Coudln't open " + path);
    }

    m_size = std::filesystem::file_size(path);
    u8 *temp_buffer = new u8[m_size];
    
    scriptstream.read((char*)temp_buffer, m_size);
    this->m_bytes = std::unique_ptr<u8[]>(temp_buffer);
}

i32 BinaryFile::disassemble_file() {
    constexpr u32 magic = 0x44433030;
    constexpr u32 version = 0x1;

    this->m_dcheader = std::bit_cast<DC_Header*>(this->m_bytes.get());

    if (this->m_dcheader->m_magic != magic) {
        throw std::runtime_error("Magic number doesn't equal 0x44433030: " + std::to_string(*(uint32_t*)this->m_bytes.get()));
    }

    if (this->m_dcheader->m_versionNumber != version) {
        throw std::runtime_error("Version number doesn't equal 0x00000001: " + std::to_string(*(uint32_t*)(this->m_bytes.get() + 8)));
    }

    this->read_reloc_table();
    
    return 1;
}


[[nodiscard]] b8 BinaryFile::location_gets_pointed_at(const void *ptr) const noexcept {
    p64 proper_offset = (std::bit_cast<p64>(ptr) - std::bit_cast<p64>(this->m_bytes.get())) / 8;
    return this->m_pointedAtTable[proper_offset / 8] & (1 << (proper_offset % 8));
}

[[nodiscard]] b8 BinaryFile::is_file_ptr(const p64 ptr) const noexcept {
    p64 offset = (ptr - std::bit_cast<p64>(this->m_bytes.get())) / 8;
    const u8* table = this->m_bytes.get() + this->m_dcheader->m_textSize + 4;
    return table[offset / 8] & (1 << (offset % 8));
}


// void print_m512i(__m512i *var) {
//     alignas(64) uint64_t val[8];  // 512 bits = 8 * 64 bits
//     _mm512_store_epi64((__m512i*)val, *var);
//     for (int i = 0; i < 8; i++) {
//         printf("%llu  ", val[i]);
//     }
//     printf("\n");
// }

void BinaryFile::read_reloc_table() {

    u8 *reloc_data = this->m_bytes.get() + this->m_dcheader->m_textSize;

    const u32 table_size = *std::bit_cast<u32*>(reloc_data);
    this->m_pointedAtTable = std::make_unique<u8[]>(table_size);
    
    const u8 *bitmap = reloc_data + 4;

#ifdef DERANGED
    const __m512i _indices = _mm512_set_epi64(7,6,5,4,3,2,1,0);
    const __m512i _1 = _mm512_set1_epi64(0x1);
    const __m512i _7 = _mm512_set1_epi64(0x7);
    const __m512i _3 = _mm512_set1_epi64(0x3);
    const __m512i _base = _mm512_set1_epi64(std::bit_cast<p64>(this->m_bytes.get()));

    for (u64 i = 0; i < table_size; ++i) {
        const u8 *data_segment_ptr = this->m_bytes.get() + i * 64;
        __m512i _data_segment = _mm512_loadu_epi64((void *)data_segment_ptr);
        __m512i _reloc_bits = _mm512_set1_epi64(bitmap[i]);
        _reloc_bits = _mm512_srlv_epi64(_reloc_bits, _indices);
        __m512i _bit_mask = _mm512_and_epi64(_reloc_bits, _1);
        __m512i _data_segment_masked = _mm512_mullo_epi64(_bit_mask, _base);
        _data_segment_masked = _mm512_add_epi64(_data_segment_masked, _data_segment);
        _mm512_storeu_epi64((void *)data_segment_ptr, _data_segment_masked);
        _mm512_mask_i64scatter_epi64((void *)this->m_pointedAtTable.get(), (__mmask8)bitmap[i], _data_segment, _bit_mask, 0x1);
    }
#else
    for (u64 i = 0; i < table_size * 8; ++i) {
        if (bitmap[i / 8] & (1 << (i % 8))) {
            u64* entry = std::bit_cast<u64*>(this->m_bytes.get() + i * 8);
            u64 offset = *entry;
            *entry = std::bit_cast<u64>(this->m_bytes.get() + offset);
            this->m_pointedAtTable[offset / 64] |= (1 << ((offset / 8) % 8));
        }
    }
#endif
    this->m_stringsPtr = std::bit_cast<p64>(this->m_bytes.get() + this->m_dcheader->m_stringsOffset);
}