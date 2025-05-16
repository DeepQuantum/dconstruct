#include "script.h"

#include <iostream>
#include <fstream>
#include <filesystem>

Script::Script() {}

Script::Script(const std::string &path) {
    std::ifstream scriptstream(path, std::ios::binary);

    if (!scriptstream.is_open()) {
        throw std::runtime_error("Coudln't open " + path);
    }

    m_size = std::filesystem::file_size(path);
    m_bytes.resize(m_size);

    scriptstream.read((char*)m_bytes.data(), m_size);
}
