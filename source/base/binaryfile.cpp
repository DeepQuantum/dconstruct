#include "binaryfile.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <immintrin.h>
#include <cstring>
#include <chrono>
#include <numeric>

namespace dconstruct {


    [[nodiscard]] std::expected<BinaryFile, std::string> BinaryFile::from_path(const std::filesystem::path &path) noexcept {
        std::ifstream scriptstream(path, std::ios::binary);

        if (!scriptstream.is_open()) {
            return std::unexpected{"couldn't open " + path.string() + '\n'};
        }

        const u64 size = std::filesystem::file_size(path);
        std::byte* temp_buffer = static_cast<std::byte*>(::operator new[](size, std::align_val_t(64)));

        scriptstream.read((char*)temp_buffer, size);
        auto bytes = byte_uptr(temp_buffer);

        if (size == 0) {
            return std::unexpected{path.string() + " is empty.\n"};
        }

        auto* dcheader = reinterpret_cast<DC_Header*>(bytes.get());

        if (dcheader->m_magic != DC_MAGIC) {
            return std::unexpected{"not a DC-file. magic number doesn't equal 0x44433030: " + std::to_string(*(uint32_t*)bytes.get()) + '\n'};
        }

        if (dcheader->m_versionNumber != DC_VERSION) {
            return std::unexpected{"not a DC-file. version number doesn't equal 0x00000001: " + std::to_string(*(uint32_t*)(bytes.get() + 8)) + '\n'};
        }

        auto file = BinaryFile(path, size, std::move(bytes), dcheader);

        file.read_reloc_table();

        file.replace_newlines_in_stringtable();

        return file;
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


    
    [[nodiscard]] bool BinaryFile::gets_pointed_at(const location loc) const noexcept {
        const p64 offset = (loc.num() - reinterpret_cast<p64>(m_bytes.get())) / 8;
        return (u8)m_pointedAtTable[offset / 8] & (1 << (offset % 8));
    }

    
    [[nodiscard]] bool BinaryFile::is_file_ptr(const location loc) const noexcept {
        p64 offset = (loc.num() - reinterpret_cast<p64>(m_bytes.get()));
        if (offset >= m_size) {
            return false;
        }
        offset /= 8;
        return m_relocTable.get<u8>(offset / 8) & (1 << (offset % 8));
    }

    
    [[nodiscard]] bool BinaryFile::is_string(const location loc) const noexcept {
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

    
    void BinaryFile::read_reloc_table() noexcept {
        std::byte *reloc_data = m_bytes.get() + m_dcheader->m_textSize;

        const u32 table_size = *reinterpret_cast<u32*>(reloc_data);
        m_pointedAtTable = byte_uptr(static_cast<std::byte*>(::operator new[](table_size, std::align_val_t(64))));
        std::memset(m_pointedAtTable.get(), 0, table_size);

        m_relocTable = location(reloc_data + 4);

#ifdef AVX512
        const __m512i _1 = _mm512_set1_epi64(0x1);
        const __m512i _base = _mm512_set1_epi64(reinterpret_cast<p64>(m_bytes.get()));

        std::byte* data_segment_ptr = m_bytes.get();
        __m512i _data_segment, _data_segment_masked;
        __mmask8 reloc_byte;

        for (u64 i = 0; i < table_size; ++i) {
            reloc_byte = m_relocTable.get<__mmask8>(i);

            _data_segment        = _mm512_load_epi64((void*)data_segment_ptr); // load the next 8 8-byte lanes
            _data_segment_masked = _mm512_maskz_mov_epi64(reloc_byte, _base); // load only the reloc offsets
            _data_segment_masked = _mm512_add_epi64(_data_segment_masked, _data_segment); // add the base pointer to the lanes that actually are pointers and need to be reloc'd
            _mm512_store_epi64((void*)data_segment_ptr, _data_segment_masked); // store the result back at the start of the current 8x8 byte chunk
            _mm512_mask_i64scatter_epi64((void*)m_pointedAtTable.get(), reloc_byte, _data_segment, _1, sizeof(std::byte)); // store which offsets get pointed at

            data_segment_ptr += 64;
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

    
    [[nodiscard]] BinaryFile::byte_uptr BinaryFile::get_unmapped() const { 
        std::byte *unmapped_bytes = static_cast<std::byte*>(::operator new[](m_size, std::align_val_t(64)));

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

        return byte_uptr(unmapped_bytes);
    }
}
