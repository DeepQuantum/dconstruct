#ifndef BINARYFILE_H
#define BINARYFILE_H

#include "DCHeader.h"
#include "DCScript.h"

#include <unordered_map>
#include <string>
#include <vector>

union SymbolValue {
    i32 i32;
    float f32;
    StateScript ss;
    bool b8;
    stringid_64 hash;
};

struct Symbol {
    u8 *offset;
    SymbolValue value;
};

class BinaryFile
{
public:
    BinaryFile();

    BinaryFile(const std::string &path);

    DC_Header *m_dcheader;
    StateScript *m_dcscript;
    std::size_t m_size;
    std::vector<uint8_t> m_bytes;
    std::vector<Symbol> m_symbols;
    bool readRelocTable();
    int32_t disassembleFile(std::unordered_map<stringid_64, std::string> &sidbase);
    void disassembleEntry(Entry *entry, std::unordered_map<stringid_64, std::string> &sidbase);

};

#endif // BINARYFILE_H
