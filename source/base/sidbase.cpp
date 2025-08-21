#include "sidbase.h"
#include <fstream>
#include <filesystem>
#include <iostream>

namespace dconstruct {

    SIDBase::SIDBase(const std::filesystem::path& path) {

        std::ifstream sidfile(path, std::ios::binary);

        if (!sidfile.is_open()) {
            std::cerr << "couldn't open sidbase at path \'" << path << "'\n";
            exit(-1);
        }

        std::size_t fsize = std::filesystem::file_size(path);
        std::byte* temp_buffer = new std::byte[fsize];

        sidfile.read(reinterpret_cast<char*>(&m_numEntries), 8);
        sidfile.seekg(0);

        sidfile.read(reinterpret_cast<char*>(temp_buffer), fsize);

        m_entries = reinterpret_cast<SIDBaseEntry*>(temp_buffer + 8);
        m_sidbytes = std::unique_ptr<std::byte[]>(temp_buffer);
        m_lowestSid = m_entries[0].hash;
        m_highestSid = m_entries[m_numEntries - 1].hash;
    }

    [[nodiscard]] const char* SIDBase::search(const sid64 hash) const noexcept {
        u64 low = 0;
        u64 high = m_numEntries - 1;
        u64 mid = 0;
        while (low <= high) {
            mid = low + (high - low) / 2;
            const SIDBaseEntry* current = m_entries + mid;
            if (current->hash == hash) [[unlikely]]
                return reinterpret_cast<const char*>(m_sidbytes.get() + current->offset);
            if (current->hash < hash) {
                low = mid + 1;
            }
            else {
                if (mid == 0) break;
                high = mid - 1;
            }
        }
        return nullptr;
    }

    [[nodiscard]] b8 SIDBase::sid_exists(const sid64 hash) const noexcept {
        return search(hash) != nullptr;
    }
}