#include "binaryfile.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <immintrin.h>
#include <cstring>
#include <chrono>

namespace dconstruct {
    BinaryFile::BinaryFile() {}

    BinaryFile::BinaryFile(const std::filesystem::path &path) {
        m_path = path;
        std::ifstream scriptstream(path, std::ios::binary);

        if (!scriptstream.is_open()) {
            std::cout << "error: coudln't open " << path << '\n';
            exit(-1);
        }

        m_size = std::filesystem::file_size(path);
        std::byte* temp_buffer = new std::byte[m_size];

        scriptstream.read((char*)temp_buffer, m_size);
        m_bytes = std::unique_ptr<std::byte[]>(temp_buffer);
    }

    b8 BinaryFile::dc_setup() {
        constexpr u32 magic = 0x44433030;
        constexpr u32 version = 0x1;

        if (m_size == 0) {
            std::cerr << "error: file is empty.\n";
            return false;
        }

        m_dcheader = reinterpret_cast<DC_Header*>(m_bytes.get());


        if (m_dcheader->m_magic != magic) {
            std::cerr << "error: not a DC-file. magic number doesn't equal 0x44433030: " << *(uint32_t*)m_bytes.get() << '\n';
            return false;
        }

        if (m_dcheader->m_versionNumber != version) {
            std::cerr << "error: not a DC-file. version number doesn't equal 0x00000001: " << *(uint32_t*)(m_bytes.get() + 8) << '\n';
            return false;
        }

        read_reloc_table();

        replace_newlines_in_stringtable();

        return true;
    }

    void BinaryFile::replace_newlines_in_stringtable() noexcept {
        constexpr u8 table_size_offset = 4;
        const u64 table_size = m_relocTable.num() - table_size_offset - m_strings.num();
        char* string_table = const_cast<char*>(m_strings.as<char>());
        for (u64 i = 0; i < table_size; ++i) {
            if (string_table[i] == '\n') {
                string_table[i] = ' ';
            }
        }
    }


    [[nodiscard]] b8 BinaryFile::gets_pointed_at(const location loc) const noexcept {
        const p64 offset = (loc.num() - reinterpret_cast<p64>(m_bytes.get())) / 8;
        return (u8)m_pointedAtTable[offset / 8] & (1 << (offset % 8));
    }

    [[nodiscard]] b8 BinaryFile::is_file_ptr(const location loc) const noexcept {
        p64 offset = (loc.num() - reinterpret_cast<p64>(m_bytes.get()));
        if (offset >= m_size) {
            return false;
        }
        offset /= 8;
        return m_relocTable.get<u8>(offset / 8) & (1 << (offset % 8));
    }

    [[nodiscard]] b8 BinaryFile::is_string(const location loc) const noexcept {
        return loc >= m_strings;
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

        std::byte *reloc_data = m_bytes.get() + m_dcheader->m_textSize;

        const u32 table_size = *reinterpret_cast<u32*>(reloc_data);
        m_pointedAtTable = std::make_unique<std::byte[]>(table_size);

        m_relocTable = location(reloc_data + 4);

#ifdef DERANGED
        const __m512i _indices = _mm512_set_epi64(7, 6, 5, 4, 3, 2, 1, 0);
        const __m512i _1 = _mm512_set1_epi64(0x1);
        const __m512i _7 = _mm512_set1_epi64(0x7);
        const __m512i _3 = _mm512_set1_epi64(0x3);
        const __m512i _base = _mm512_set1_epi64(reinterpret_cast<p64>(m_bytes.get()));

        for (u64 i = 0; i < table_size; ++i) {
            const u8* data_segment_ptr = m_bytes.get() + i * 64;
            __m512i _data_segment = _mm512_loadu_epi64((void*)data_segment_ptr);
            __m512i _reloc_bits = _mm512_set1_epi64(bitmap[i]);
            _reloc_bits = _mm512_srlv_epi64(_reloc_bits, _indices);
            __m512i _bit_mask = _mm512_and_epi64(_reloc_bits, _1);
            __m512i _data_segment_masked = _mm512_mullo_epi64(_bit_mask, _base);
            _data_segment_masked = _mm512_add_epi64(_data_segment_masked, _data_segment);
            _mm512_storeu_epi64((void*)data_segment_ptr, _data_segment_masked);
            _mm512_mask_i64scatter_epi64((void*)m_pointedAtTable.get(), (__mmask8)bitmap[i], _data_segment, _bit_mask, 0x1);
        }
#else
        for (u64 i = 0; i < table_size * 8; ++i) {
            if (m_relocTable.get<u8>(i / 8) & (1 << (i % 8))) {
                u64* entry = reinterpret_cast<u64*>(m_bytes.get() + i * 8);
                u64 offset = *entry;
                *entry = reinterpret_cast<u64>(m_bytes.get() + offset);
                reinterpret_cast<u8*>(m_pointedAtTable.get())[offset / 64] |= (1 << ((offset / 8) % 8));
            }
        }
#endif
        m_strings = location(m_bytes.get() + m_dcheader->m_stringsOffset);
    }

    
    [[nodiscard]] std::unique_ptr<std::byte[]> BinaryFile::get_unmapped() const noexcept { 
        std::byte *unmapped_bytes = new std::byte[m_size];

        std::memcpy(unmapped_bytes, m_bytes.get(), m_size);

        std::byte *reloc_data = unmapped_bytes + m_dcheader->m_textSize;

        const u32 table_size = *reinterpret_cast<u32*>(reloc_data);

        for (u64 i = 0; i < table_size * 8; ++i) {
            if (m_relocTable.get<u8>(i / 8) & (1 << (i % 8))) {
                p64 *entry = reinterpret_cast<p64*>(unmapped_bytes + i * 8);
                p64 offset = *entry;
                *entry = offset - reinterpret_cast<p64>(m_bytes.get());
            }
        }

        return std::unique_ptr<std::byte[]>(unmapped_bytes);
    }
}
