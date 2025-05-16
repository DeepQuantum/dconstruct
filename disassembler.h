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

    bool checkScriptData(uint8_t *data, const DC_Header *header);
    void disassembleEntry(const Entry *entry);

};

#endif // DISASSEMBLER_H
