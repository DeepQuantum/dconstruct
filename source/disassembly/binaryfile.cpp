#include "binaryfile.h"

#include <iostream>
#include <fstream>
#include <filesystem>

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

i32 BinaryFile::disassemble_file(SIDBase &sidbase) {
    constexpr u32 magic = 0x44433030;
    constexpr u32 version = 0x1;

    this->m_dcheader = reinterpret_cast<DC_Header*>(this->m_bytes.data());

    if (this->m_dcheader->m_magic != magic) {
        throw std::runtime_error("Magic number doesn't equal 0x44433030: " + std::to_string(*(uint32_t*)this->m_bytes.data()));
    }

    if (this->m_dcheader->m_versionNumber != version) {
        throw std::runtime_error("Version number doesn't equal 0x00000001: " + std::to_string(*(uint32_t*)(this->m_bytes.data() + 8)));
    }

    this->read_reloc_table();

    this->m_entries.reserve(this->m_dcheader->m_numEntries);
    
    return 1;
}

void BinaryFile::read_reloc_table() {
    u8 *data = this->m_bytes.data();
    u8 *reloc_data = data + this->m_dcheader->m_textSize;

    const u32 table_size = *reinterpret_cast<u32*>(reloc_data);
    u8 *bitmap = reloc_data + 4;

    for (u64 i = 0; i < table_size * 8; ++i) {
        if (bitmap[i / 8] & (1 << (i % 8))) {
            u64* entry = reinterpret_cast<u64*>(data + i * 8);
            u64 offset = *entry;
            *entry = reinterpret_cast<u64>(data + offset);
        }
    }
}

BinaryFile::~BinaryFile() {
    for (const auto &[sid, string] : this->sid_cache) {
        delete[] string;
    }
}