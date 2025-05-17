#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <string>
#include <map>
#include "base.h"

#include "script.h"



class Disassembler
{
public:
    Disassembler();
    void loadSidbase(const std::string &path = "sidbase.bin");
    int32_t disassembleFile(Script &script);
    std::map<stringid_64, std::string> m_sidbase;

private:

    bool readRelocTable(uint8_t *data, DC_Header *header);
    void disassembleEntry(Script &script, Entry *entry);
    void disassembleStateScript(Script &script, StateScript *entry);

};

#endif // DISASSEMBLER_H
