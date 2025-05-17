#include "disassembler.h"

#include <fstream>
#include <filesystem>
#include <intrin.h> //bittest

Disassembler::Disassembler() {
}

void Disassembler::loadSidbase(const std::string &path) {
    uint64_t num_entries, loc, hash_value;
    std::ifstream sidfile(path, std::ios::binary);
    std::size_t fsize = std::filesystem::file_size(path);

    if (!sidfile.is_open()) {
        exit(-1);
    }

    sidfile.read(reinterpret_cast<char*>(&num_entries), 8);

    char *bytes = new char[fsize];

    sidfile.read(bytes, fsize);

    for (std::size_t i = 0; i < num_entries; ++i) {
        hash_value = *reinterpret_cast<stringid_64*>(bytes + i * 16 + 0);
        loc = *reinterpret_cast<uint64_t*>(bytes + i * 16 + 8);

        this->m_sidbase.emplace(hash_value, std::string(bytes + loc - 8));
    }
    delete[] bytes;
}


int32_t Disassembler::disassembleFile(Script &script) {
    constexpr uint32_t magic = 0x44433030;
    constexpr uint32_t version = 0x1;

    script.m_dcheader = reinterpret_cast<DC_Header*>(script.m_bytes.data());

    if (script.m_dcheader->m_magic != magic) {
        throw std::runtime_error("Magic number doesn't equal 0x44433030: " + std::to_string(*(uint32_t*)script.m_bytes.data()));
    }

    if (script.m_dcheader->m_versionNumber != version) {
        throw std::runtime_error("Version number doesn't equal 0x00000001: " + std::to_string(*(uint32_t*)(script.m_bytes.data() + 8)));
    }

    bool checked = this->readRelocTable(script.m_bytes.data(), script.m_dcheader);

    if (!checked) {
        throw std::runtime_error("Script data check was not succesfull");
    }

    for (std::size_t i = 0; i < script.m_dcheader->m_numEntries; ++i) {
        this->disassembleEntry(script, &script.m_dcheader->m_pStartOfData[i]);
    }

    return 1;
}

void Disassembler::disassembleEntry(Script &script, Entry *entry) {
    stringid_64 type = entry->m_typeId;

    if (this->m_sidbase[type] == "state-script") {
        this->disassembleStateScript(script, reinterpret_cast<StateScript*>(entry));
    } else if (this->m_sidbase[type] == "int32") {
        script.m_symbols.emplace_back(Symbol<int32_t>{reinterpret_cast<uint8_t*>(entry), entry->m_scriptId, reinterpret_cast<int32_t*>(entry->m_entryPtr)});
    } else {
        throw std::runtime_error("Unknown type: " + this->m_sidbase[type]);
    }
}

void Disassembler::disassembleStateScript(Script &script, StateScript *entry) {
    throw std::runtime_error(entry->m_pDebugFileName);
}

bool Disassembler::readRelocTable(uint8_t *data, DC_Header *header) {
    int32_t size = *(int32_t*)(data + header->m_textSize);
    uint8_t *data_copy = data;
    uint8_t *pUnk = data + header->m_textSize;
    int32_t max = size * 8;
    int32_t iPtr = 0;
    int32_t byteIndex = 0;
    int32_t v1 = 0;
    int8_t r12d = 0x0;
    int32_t v2 = 0x0;
    int32_t r15d = 0x0;
    uint64_t loc = 0;
    uint64_t tmp = 0;

    if (!(size & 0x1FFFFFFF)) {
        return false;
    }

    do {
        if (byteIndex < size)
        {
            r12d = v1 + 1;
            v2 = *(pUnk + byteIndex + 0x4);
            if (r12d == 8)
            {
                r15d = 1;
                r12d = 0;
            }
            else
            {
                r15d = 0;
            }
            if (_bittest(reinterpret_cast<long*>(&v2), v1))
            {
                loc = *(uint64_t*)(data + (iPtr * 0x8));
                tmp = 0;
                if (loc != 0)
                {
                    if ((loc >= 0 && loc < 0xFFFFFFFFFFFFFFFF))
                    {
                        tmp = loc;
                    }
                }
                *(uint64_t*)(data + (iPtr * 0x8)) = (uint64_t)(data_copy + tmp);
            }
            byteIndex += r15d;
            iPtr++;
            v1 = r12d;
        }
    } while (max > iPtr);

    return true;
}


















