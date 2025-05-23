#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <string>
#include <unordered_map>
#include "base.h"

#include "binaryfile.h"

class Disassembler
{
public:
    Disassembler();
    void loadSidbase(const std::string &path = "sidbase.bin");
    i32 disassembleFile(BinaryFile &script);
    std::unordered_map<stringid_64, std::string> m_sidbase;
    std::vector<BinaryFile> m_scripts;

private:
    bool readRelocTable(uint8_t *data, DC_Header *header);
    void disassembleEntry(BinaryFile &script, Entry *entry);
    StateScript disassembleStateScript(BinaryFile &script, Entry *entry);
};

#endif // DISASSEMBLER_H
