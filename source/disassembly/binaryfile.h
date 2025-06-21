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
        i32 *i32_ptr;
        f32 *f32_ptr;
        b8 *b8_ptr;
        StateScript *ss_ptr;
        ScriptLambda *lambda_ptr;
        uint64_t *hash_ptr;
        Entry raw_entry;
    };
};

class BinaryFile
{
public:

    i32 dc_setup();
    BinaryFile();

    BinaryFile(const char *path);

    DC_Header *m_dcheader = nullptr;
    StateScript *m_dcscript = nullptr;
    std::size_t m_size;
    std::unique_ptr<u8[]> m_bytes;
    std::unique_ptr<u8[]> m_pointedAtTable;
    p64 m_stringsPtr;
    u8 *m_relocTable;
    std::map<sid64, const std::string> m_sidCache;
    std::set<p64> m_emittedStructs{};
    std::vector<std::unique_ptr<FunctionDisassembly>> m_functions;
    [[nodiscard]] b8 is_file_ptr(const p64 ptr) const noexcept;
    [[nodiscard]] b8 location_gets_pointed_at(const void *ptr) const noexcept;
    //void apply_edit()


private:
    void read_reloc_table();
    void replace_newlines_in_stringtable() noexcept;
};

#endif // BINARYFILE_H
