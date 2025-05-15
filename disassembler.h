#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <map>
#include <string>
#include <cstdint>

class Disassembler
{
public:
    Disassembler();

    void loadSidbase(const std::string &path = "sidbase.bin");
    std::map<uint64_t, std::string> m_sidbase;

private:

};

#endif // DISASSEMBLER_H
