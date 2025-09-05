#pragma once

#include "DCHeader.h"
#include "DCScript.h"
#include "sidbase.h"
#include "disassembly/instructions.h"

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
            b8* b8_ptr;
            StateScript* ss_ptr;
            ScriptLambda* lambda_ptr;
            uint64_t* hash_ptr;
            Entry raw_entry;
        };
    };

    class BinaryFile
    {
    public:

        b8 dc_setup();
        BinaryFile();

        BinaryFile(const std::filesystem::path &path);

        std::filesystem::path m_path;
        DC_Header* m_dcheader = nullptr;
        StateScript* m_dcscript = nullptr;
        std::size_t m_size = 0;
        std::unique_ptr<std::byte[]> m_bytes;
        std::unique_ptr<std::byte[]> m_pointedAtTable;
        location m_strings;
        location m_relocTable;
        std::map<sid64, const std::string> m_sidCache;
        std::set<p64> m_emittedStructs{};
        [[nodiscard]] b8 is_file_ptr(const location) const noexcept;
        [[nodiscard]] b8 gets_pointed_at(const location) const noexcept;
        [[nodiscard]] b8 is_string(const location) const noexcept;
        [[nodiscard]] std::unique_ptr<std::byte[]> get_unmapped() const;

    private:
        void read_reloc_table();
        void replace_newlines_in_stringtable() noexcept;
    };
}
