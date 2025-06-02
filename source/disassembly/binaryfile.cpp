#include "binaryfile.h"

#include <iostream>
#include <fstream>
#include <filesystem>

#include <intrin.h> // for _bittest

BinaryFile::BinaryFile() {}

BinaryFile::BinaryFile(const std::string &path) {
    std::ifstream scriptstream(path, std::ios::binary);

    if (!scriptstream.is_open()) {
        throw std::runtime_error("Coudln't open " + path);
    }

    m_size = std::filesystem::file_size(path);
    m_bytes.resize(m_size);

    scriptstream.read((char*)m_bytes.data(), m_size);
}

i32 BinaryFile::disassembleFile(std::unordered_map<stringid_64, std::string> &sidbase) {
    constexpr u32 magic = 0x44433030;
    constexpr u32 version = 0x1;

    this->m_dcheader = reinterpret_cast<DC_Header*>(this->m_bytes.data());

    if (this->m_dcheader->m_magic != magic) {
        throw std::runtime_error("Magic number doesn't equal 0x44433030: " + std::to_string(*(uint32_t*)this->m_bytes.data()));
    }

    if (this->m_dcheader->m_versionNumber != version) {
        throw std::runtime_error("Version number doesn't equal 0x00000001: " + std::to_string(*(uint32_t*)(this->m_bytes.data() + 8)));
    }

    bool checked = this->readRelocTable();

    if (!checked) {
        throw std::runtime_error("Script data check was not succesfull");
    }

    for (std::size_t i = 0; i < this->m_dcheader->m_numEntries; ++i) {
        this->disassembleEntry(this->m_dcheader->m_pStartOfData + i, sidbase);
    }
    return 1;
}

void BinaryFile::disassembleEntry(Entry *entry, std::unordered_map<stringid_64, std::string> &sidbase) {
    Symbol symbol = Symbol{};

    std::string typeName = sidbase[entry->m_typeId];
    symbol.id = entry->m_scriptId;

    if (typeName == "state-script") {
        this->m_dcscript = reinterpret_cast<StateScript*>(entry->m_entryPtr);
        symbol.type = SymbolType::SS;
        symbol.ss_ptr = this->m_dcscript;
    } else if (typeName == "int32") {
        symbol.type = SymbolType::I32;
        symbol.i32_ptr = reinterpret_cast<i32*>(entry->m_entryPtr);
    } else if (typeName == "float") {
        symbol.type = SymbolType::F32;
        symbol.f32_ptr = reinterpret_cast<f32*>(entry->m_entryPtr);
    } else if (typeName == "boolean") {
        symbol.type = SymbolType::B8;
        symbol.b8_ptr = reinterpret_cast<b8*>(entry->m_entryPtr);
    } else if (typeName == "script-lambda") {
        symbol.type = SymbolType::LAMBDA;
        symbol.lambda_ptr = reinterpret_cast<ScriptLambda*>(entry->m_entryPtr);
    }
    else {
        symbol.type = SymbolType::UNKNOWN;
        symbol.raw_entry = *entry;
    }

    this->m_symbols.push_back(symbol);
}


bool BinaryFile::readRelocTable() {
    u8* data = this->m_bytes.data();
    DC_Header *header = this->m_dcheader;
    int32_t size = *(i32*)(data + this->m_dcheader->m_textSize);
    uint8_t *data_copy = this->m_bytes.data();
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
