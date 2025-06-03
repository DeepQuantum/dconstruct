#ifndef BINARYFILE_H
#define BINARYFILE_H

#include "DCHeader.h"
#include "DCScript.h"

#include <unordered_map>
#include <string>
#include <vector>

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
    stringid_64 id;
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
    BinaryFile();

    BinaryFile(const std::string &path);

    DC_Header *m_dcheader = nullptr;
    StateScript *m_dcscript = nullptr;
    std::size_t m_size;
    std::vector<uint8_t> m_bytes;
    std::vector<Symbol> m_symbols;
    void readRelocTable();
    int32_t disassembleFile(std::unordered_map<stringid_64, std::string> &sidbase);
    void disassembleEntry(Entry *entry, std::unordered_map<stringid_64, std::string> &sidbase);

};

#endif // BINARYFILE_H
