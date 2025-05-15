#include "disassembler.h"

#include <fstream>
#include <filesystem>

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

    for (std::size_t i = 0; i < num_entries; i += 16) {
        hash_value = *reinterpret_cast<uint64_t*>(bytes + i + 0);
        loc = *reinterpret_cast<uint64_t*>(bytes + i + 8);

        this->m_sidbase.emplace(hash_value, std::string(bytes + loc - 8));
    }
    delete[] bytes;
}
