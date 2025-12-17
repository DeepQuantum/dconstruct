#include "sidbase.h"
#include <fstream>
#include <filesystem>
#include <iostream>

namespace dconstruct {

    [[nodiscard]] std::expected<SIDBase, std::string> SIDBase::from_binary(const std::filesystem::path& path) noexcept {

        std::ifstream sidfile(path, std::ios::binary);

        if (!sidfile.is_open()) {
            return std::unexpected{std::string("couldn't open sidbase at path \'") + path.string() + "'\n"};
        }

        std::size_t fsize = std::filesystem::file_size(path);
        std::byte* temp_buffer = new std::byte[fsize];

        u64 num_entries = 0;
        sidfile.read(reinterpret_cast<char*>(&num_entries), 8);
        sidfile.seekg(0);

        sidfile.read(reinterpret_cast<char*>(temp_buffer), fsize);

        auto entries = reinterpret_cast<SIDBaseEntry*>(temp_buffer + 8);
        auto bytes = std::unique_ptr<std::byte[]>(temp_buffer);
        const sid64 lowest = entries[0].hash;
        const sid64 highest = entries[num_entries - 1].hash;

        return SIDBase{num_entries, std::move(bytes), entries, lowest, highest};
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

    [[nodiscard]] bool SIDBase::sid_exists(const sid64 hash) const noexcept {
        return search(hash) != nullptr;
    }
}