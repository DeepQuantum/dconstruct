#pragma once

#ifndef BINARYFILE_H
#define BINARYFILE_H

#include "DCHeader.h"
#include "DCScript.h"
#include "sidbase.h"
#include "instructions.h"

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>

namespace dconstruct {
    enum SymbolType {
        B8,
        I32,
        F32,
        SS,
        HASH,
        LAMBDA,
        UNKNOWN
    };

    struct Symbol {
        SymbolType type;
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

        BinaryFile(const std::string& path);

        DC_Header* m_dcheader = nullptr;
        StateScript* m_dcscript = nullptr;
        std::size_t m_size;
        std::unique_ptr<std::byte[]> m_bytes;
        std::unique_ptr<std::byte[]> m_pointedAtTable;
        location m_strings;
        location m_relocTable;
        std::map<sid64, const std::string> m_sidCache;
        std::set<p64> m_emittedStructs{};
        std::vector<std::unique_ptr<FunctionDisassembly>> m_functions;
        [[nodiscard]] b8 is_file_ptr(const location) const noexcept;
        [[nodiscard]] b8 gets_pointed_at(const location) const noexcept;
        [[nodiscard]] b8 is_string(const location) const noexcept;

    private:
        void read_reloc_table();
        void replace_newlines_in_stringtable() noexcept;
    };
}
#endif // BINARYFILE_H
