#include "binaryfile.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <immintrin.h>
#include <cstring>
#include <chrono>
#include <numeric>

namespace dconstruct {

    template class BinaryFile<true>;
    template class BinaryFile<false>;

    template<bool is_64_bit>
    [[nodiscard]] std::expected<BinaryFile<is_64_bit>, std::string> BinaryFile<is_64_bit>::from_path(const std::filesystem::path &path) noexcept {
        std::ifstream scriptstream(path, std::ios::binary);

        if (!scriptstream.is_open()) {
            return std::unexpected{"couldn't open " + path.string() + '\n'};
        }

        const u64 size = std::filesystem::file_size(path);
        std::byte* temp_buffer = new std::byte[size];

        scriptstream.read((char*)temp_buffer, size);
        auto bytes = std::unique_ptr<std::byte[]>(temp_buffer);

        

        if (size == 0) {
            return std::unexpected{path.string() + " is empty.\n"};
        }

        auto* dcheader = reinterpret_cast<DC_Header*>(bytes.get());

        if (dcheader->m_magic != MAGIC) {
            return std::unexpected{"not a DC-file. magic number doesn't equal 0x44433030: " + std::to_string(*(uint32_t*)bytes.get()) + '\n'};
        }

        if (dcheader->m_versionNumber != VERSION) {
            return std::unexpected{"not a DC-file. version number doesn't equal 0x00000001: " + std::to_string(*(uint32_t*)(bytes.get() + 8)) + '\n'};
        }

        auto file = BinaryFile<is_64_bit>(path, size, std::move(bytes), dcheader);

        file.read_reloc_table();

        if constexpr (is_64_bit) {
            file.replace_newlines_in_stringtable();
        }

        return file;
    }

    template<bool is_64_bit>
    template<typename T>
    void BinaryFile<is_64_bit>::insert_into_bytestream(std::unique_ptr<std::byte[]>& out, u64& size, const T& obj) noexcept {
        const std::byte* p = reinterpret_cast<const std::byte*>(std::addressof(obj));
        std::memcpy(out.get() + size, p, sizeof(T));
        size += sizeof(T);
    }

    template<bool is_64_bit>
    [[nodiscard]] std::expected<BinaryFile<is_64_bit>, std::string> BinaryFile<is_64_bit>::from_codegen(const std::vector<compiler::function>& funcs, const compiler::global_state& global) noexcept {
        constexpr sid64 script_lambda_sid = SID("script-lambda");
        constexpr sid64 array_sid         = SID("array");
        constexpr sid64 global_sid        = SID("global");
        constexpr sid64 function_sid      = SID("function");
        constexpr u64   deadbeef          = 0xDEAD'BEEF'1337'F00D;
        
        constexpr u8  text_size_offset        = 0xC;
        constexpr u64 reloc_table_size_offset = 0x4;
        constexpr u64 first_entry_offset      = 0x28;

        constexpr u32 header_size = sizeof(DC_Header) + sizeof(array_sid);
        
        const u64 num_funcs = funcs.size();

        const u32 entries_size = sizeof(Entry) * num_funcs;

        const u64 raw_function_size = std::accumulate(funcs.begin(), funcs.end(), u64{0}, [](u64 acc, const compiler::function& fn) {
            return acc + fn.get_size_in_bytes();
        });

        const u64 stringtable_size = std::accumulate(global.m_strings.begin(), global.m_strings.end(), u64{0}, [](u64 acc, const std::string& s) {
            return acc + s.size() + 1;
        });

        const u32 functions_size = (sizeof(script_lambda_sid) + sizeof(ScriptLambda)) * num_funcs + raw_function_size;

        std::vector<char> stringtable;
        stringtable.reserve(stringtable_size);

        for (const auto& s : global.m_strings) {
            stringtable.insert(stringtable.end(), s.begin(), s.end());
            stringtable.push_back('\0');
        }

        const u32 data_size = header_size + entries_size + functions_size;
        const u32 stringtable_reloctable_padding = stringtable_size % 4 == 0 ? 0 : 4 - (stringtable_size % 4);

        const u32 relocatable_data_size = data_size + stringtable_size;
        const u32 reloc_table_size = static_cast<u32>(std::ceil(relocatable_data_size / 64.f));
        const u32 non_reloctable_size = stringtable_reloctable_padding + reloc_table_size;
        const u32 total_size = relocatable_data_size + reloc_table_size_offset + non_reloctable_size;

        std::unique_ptr<std::byte[]> out = std::make_unique<std::byte[]>(total_size);
        u64 current_size = 0;
        
        const p64 base_ptr = reinterpret_cast<p64>(out.get());

        DC_Header header {
            MAGIC,
            VERSION,
            relocatable_data_size + stringtable_reloctable_padding,
            data_size,
            0x1,
            static_cast<u32>(funcs.size()),
            reinterpret_cast<Entry*>(first_entry_offset)
        };

        insert_into_bytestream(out, current_size, header);
        insert_into_bytestream(out, current_size, array_sid);

        const u64 first_function_start = header_size + funcs.size() * sizeof(Entry);

        u64 prev_function_size = sizeof(function_sid);

        for (const auto& fn : funcs) {
            const sid64 function_name = SID(fn.m_name.c_str());
            Entry entry{function_name, script_lambda_sid, reinterpret_cast<const Entry*>(first_function_start + prev_function_size)};
            insert_into_bytestream(out, current_size, entry);
            prev_function_size = sizeof(function_sid) + fn.get_size_in_bytes();
        }

        for (const auto& fn : funcs) {
            insert_into_bytestream(out, current_size, script_lambda_sid);
            const ScriptLambda lambda = {
                reinterpret_cast<u64*>(current_size + sizeof(ScriptLambda)) ,
                reinterpret_cast<u64*>(current_size + sizeof(ScriptLambda) + fn.m_instructions.size() * sizeof(Instruction)),
                function_sid,
                0xB0,
                0x0,
                deadbeef,
                0ul,
                static_cast<u32>(fn.m_instructions.size()),
                -1,
                global_sid,
                0ull
            };
            insert_into_bytestream(out, current_size, lambda);
            for (const Instruction& istr : fn.m_instructions) {
                insert_into_bytestream(out, current_size, istr);
            }
            for (const u64 symb : fn.m_symbolTable) {
                insert_into_bytestream(out, current_size, symb);
            }
        }

        std::memcpy(out.get() + current_size, stringtable.data(), stringtable.size());
        current_size += stringtable.size();

        std::memset(out.get() + current_size, 0, stringtable_reloctable_padding);
        current_size += stringtable_reloctable_padding;

        insert_into_bytestream(out, current_size, reloc_table_size);

        assert(current_size == total_size);

        std::ofstream of("out.bin", std::ios_base::binary);
        of.write(reinterpret_cast<const char*>(out.get()), total_size);

        return BinaryFile::from_path("out.bin");
    }

    template<bool is_64_bit>
    void BinaryFile<is_64_bit>::replace_newlines_in_stringtable() noexcept {
        constexpr u8 table_size_offset = 4;
        const u64 table_size = m_relocTable.num() - table_size_offset - m_strings.num();
        char* string_table = const_cast<char*>(m_strings.as<char>());
        for (u64 i = 0; i < table_size; ++i) {
            if (string_table[i] == '\n') {
                string_table[i] = ' ';
            }
        }
    }


    template<bool is_64_bit>
    [[nodiscard]] bool BinaryFile<is_64_bit>::gets_pointed_at(const location loc) const noexcept {
        const p64 offset = (loc.num() - reinterpret_cast<p64>(m_bytes.get())) / 8;
        return (u8)m_pointedAtTable[offset / 8] & (1 << (offset % 8));
    }

    template<bool is_64_bit>
    [[nodiscard]] bool BinaryFile<is_64_bit>::is_file_ptr(const location loc) const noexcept {
        p64 offset = (loc.num() - reinterpret_cast<p64>(m_bytes.get()));
        if (offset >= m_size) {
            return false;
        }
        offset /= 8;
        return m_relocTable.get<u8>(offset / 8) & (1 << (offset % 8));
    }

    template<bool is_64_bit>
    [[nodiscard]] bool BinaryFile<is_64_bit>::is_string(const location loc) const noexcept {
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

    template<bool is_64_bit>
    void BinaryFile<is_64_bit>::read_reloc_table() {

        std::byte *reloc_data = m_bytes.get() + m_dcheader->m_textSize;

        const u32 table_size = *reinterpret_cast<u32*>(reloc_data);
        m_pointedAtTable = std::make_unique<std::byte[]>(table_size);

        m_relocTable = location(reloc_data + 4);

#ifdef false
        const __m512i _indices = _mm512_set_epi64(7, 6, 5, 4, 3, 2, 1, 0);
        const __m512i _1 = _mm512_set1_epi64(0x1);
        const __m512i _7 = _mm512_set1_epi64(0x7);
        const __m512i _3 = _mm512_set1_epi64(0x3);
        const __m512i _base = _mm512_set1_epi64(reinterpret_cast<p64>(m_bytes.get()));

        for (u64 i = 0; i < table_size; ++i) {
            const u8* data_segment_ptr   = m_bytes.get() + i * 64;
            __m512i _data_segment        = _mm512_loadu_epi64((void*)data_segment_ptr);
            __m512i _reloc_bits          = _mm512_set1_epi64(bitmap[i]);
            _reloc_bits                  = _mm512_srlv_epi64(_reloc_bits, _indices);
            __m512i _bit_mask            = _mm512_and_epi64(_reloc_bits, _1);
            __m512i _data_segment_masked = _mm512_mullo_epi64(_bit_mask, _base);
            _data_segment_masked         = _mm512_add_epi64(_data_segment_masked, _data_segment);

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

    template<bool is_64_bit>
    [[nodiscard]] std::unique_ptr<std::byte[]> BinaryFile<is_64_bit>::get_unmapped() const { 
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
