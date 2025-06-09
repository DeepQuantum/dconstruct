#include "sidbase.h"
#include <fstream>
#include <filesystem>


void SIDBase::load(const char *path) noexcept {
    std::ifstream sidfile(path, std::ios::binary);
    std::size_t fsize = std::filesystem::file_size(path);

    if (!sidfile.is_open()) {
        exit(-1);
    }

    this->m_sidbytes = std::make_unique<u8[]>(fsize);

    sidfile.read(reinterpret_cast<char*>(&this->m_num_entries), 8);
    sidfile.seekg(0);
    sidfile.read(reinterpret_cast<char*>(this->m_sidbytes.get()), fsize);

    this->m_num_entries = m_num_entries;
    this->m_entries = reinterpret_cast<SIDBaseEntry*>(this->m_sidbytes.get() + 8);
}

[[nodiscard]] const char *SIDBase::search(const stringid_64 hash) const noexcept {
    u64 low = 0;
    u64 high = m_num_entries - 1;
    u64 mid = 0;
    while (low <= high) {
        mid = low + (high - low) / 2;
        const SIDBaseEntry* current = m_entries + mid;
        if (current->hash == hash) [[unlikely]]
            return reinterpret_cast<const char*>(this->m_sidbytes.get() + current->offset);
        if (current->hash < hash) {
            low = mid + 1;
        } else {
            if (mid == 0) break;
            high = mid - 1;
        }
    }
    return nullptr;
}

[[nodiscard]] const b8 SIDBase::sid_exists(const stringid_64 hash) const noexcept {
    return this->search(hash) != nullptr;
}