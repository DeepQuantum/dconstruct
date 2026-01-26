#pragma once

#include "DCHeader.h"
#include "DCScript.h"
#include "sidbase.h"
#include "disassembly/instructions.h"
#include "compilation/global_state.h"
#include "compilation/function.h"

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>

namespace dconstruct {
    enum class symbol_type {
        B8,
        I32,
        F32,
        SS,
        HASH,
        LAMBDA,
        UNKNOWN
    };

    struct symbol {
        symbol_type type;
        sid64 id;
        union {
            i32* i32_ptr;
            f32* f32_ptr;
            bool* b8_ptr;
            StateScript* ss_ptr;
            ScriptLambda* lambda_ptr;
            uint64_t* hash_ptr;
            Entry raw_entry;
        };
    };

    template<bool is_64_bit = true>
    class BinaryFile
    {
    public:

        BinaryFile(std::filesystem::path path, const u64 size, std::unique_ptr<std::byte[]>&& bytes, DC_Header* dcheader) noexcept : 
        m_path(std::move(path)), m_size(size), m_bytes(std::move(bytes)), m_dcheader(dcheader) {};

        [[nodiscard]] static std::expected<BinaryFile<is_64_bit>, std::string> from_path(const std::filesystem::path& path) noexcept;

        std::filesystem::path m_path;
        const DC_Header* m_dcheader = nullptr;
        const StateScript* m_dcscript = nullptr;
        std::size_t m_size = 0;
        std::unique_ptr<std::byte[]> m_bytes;
        std::unique_ptr<std::byte[]> m_pointedAtTable;
        location m_strings;
        location m_relocTable;
        std::map<std::conditional_t<is_64_bit, sid64, sid32>, const std::string> m_sidCache;
        std::set<p64> m_emittedStructs;
        [[nodiscard]] bool is_file_ptr(const location) const noexcept;
        [[nodiscard]] bool gets_pointed_at(const location) const noexcept;
        [[nodiscard]] bool is_string(const location) const noexcept;
        [[nodiscard]] std::unique_ptr<std::byte[]> get_unmapped() const;

        

    private:
        void read_reloc_table() noexcept;
        void replace_newlines_in_stringtable() noexcept;
    };

    extern template class BinaryFile<true>;
    extern template class BinaryFile<false>;
}
