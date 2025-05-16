#ifndef SCRIPT_H
#define SCRIPT_H

#include "DCHeader.h"
#include "DCScript.h"

#include <string>
#include <vector>

class Script
{
public:
    Script();

    Script(const std::string &path);

    DC_Header *m_dcheader;
    StateScript *m_dcscript;
    std::size_t m_size;
    std::vector<uint8_t> m_bytes;

};

#endif // SCRIPT_H
