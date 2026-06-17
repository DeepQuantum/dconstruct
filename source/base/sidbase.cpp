#include "sidbase.h"
#include "base.h"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <cstring>
#include <format>

namespace dconstruct {

    [[nodiscard]] resstr<SIDBase> SIDBase::from_binary(const std::filesystem::path& path) noexcept {
        std::ifstream sidfile(path, std::ios::binary);

        if (!sidfile.is_open()) {
            return std::unexpected{std::format("couldn't open sidbase at path \'{}\'", path.string())};
        }

        std::size_t fsize = std::filesystem::file_size(path);
        std::byte* temp_buffer = new std::byte[fsize];

        u64 num_entries = 0;
        sidfile.read(reinterpret_cast<char*>(&num_entries), sizeof(u64));
        sidfile.seekg(0);

        sidfile.read(reinterpret_cast<char*>(temp_buffer), fsize);

        auto entries = reinterpret_cast<SIDBaseEntry*>(temp_buffer + sizeof(u64));
        auto bytes = std::unique_ptr<std::byte[]>(temp_buffer);
        const sid64 lowest = entries[0].hash;
        const sid64 highest = entries[num_entries - 1].hash;

        return SIDBase{num_entries, std::move(bytes), entries, lowest, highest, fsize};
    }

    [[nodiscard]] SIDBase SIDBase::from_caches(const std::vector<std::map<sid64, std::string>>& caches) {
        std::map<sid64, std::string> merged;
        for (const auto& cache : caches) {
            for (const auto& [hash, name] : cache) {
                merged.emplace(hash, name);
            }
        }

        const u64 entry_count = merged.size();
        const std::size_t table_size = entry_count * sizeof(SIDBaseEntry);
        std::size_t string_size = 0;
        for (const auto& [hash, name] : merged) {
            string_size += name.size() + 1;
        }
        const std::size_t total_size = sizeof(u64) + table_size + string_size;

        auto buffer = std::make_unique<std::byte[]>(total_size);
        std::byte* base = buffer.get();

        const u64 header = entry_count + 1;
        std::memcpy(base, &header, sizeof(u64));

        auto* entries = reinterpret_cast<SIDBaseEntry*>(base + sizeof(u64));
        std::size_t string_off = sizeof(u64) + table_size;
        u64 i = 0;
        for (const auto& [hash, name] : merged) {
            entries[i].hash = hash;
            entries[i].offset = string_off;
            std::memcpy(base + string_off, name.c_str(), name.size() + 1);
            string_off += name.size() + 1;
            ++i;
        }

        const sid64 lowest = entry_count > 0 ? merged.begin()->first : 0;
        const sid64 highest = entry_count > 0 ? merged.rbegin()->first : 0;

        return SIDBase{header, std::move(buffer), entries, lowest, highest, total_size};
    }

    bool SIDBase::dump(const std::filesystem::path& path) const {
        std::ofstream out(path, std::ios::binary);
        if (!out.is_open()) {
            return false;
        }
        out.write(reinterpret_cast<const char*>(m_sidbytes.get()), static_cast<std::streamsize>(m_byteSize));
        return out.good();
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
            } else {
                if (mid == 0)
                    break;
                high = mid - 1;
            }
        }
        return nullptr;
    }

    [[nodiscard]] bool SIDBase::sid_exists(const sid64 hash) const noexcept {
        return search(hash) != nullptr;
    }

    [[nodiscard]] const char* SIDBase::lookup(const sid64 hash) const noexcept {
        return search(hash);
    }

    [[nodiscard]] const char* SIDBase::lookup(const sid32 hash) const noexcept {
        return search(hash);
    }

    [[nodiscard]] const char* SIDBase::lookup(const sid64 hash, std::map<sid64, std::string>& cache) const {
        if (auto res = cache.find(hash); res != cache.end()) {
            return res->second.c_str();
        }
        if (const char* hash_string = search(hash)) {
            return cache.emplace(hash, hash_string).first->second.c_str();
        }
        return cache.emplace(hash, int_to_string_id<sid64>(hash)).first->second.c_str();
    }

    [[nodiscard]] const char* SIDBase::lookup(const sid32 hash, std::map<sid64, std::string>& cache) const {
        if (auto res = cache.find(hash); res != cache.end()) {
            return res->second.c_str();
        }
        if (const char* hash_string = search(hash)) {
            return cache.emplace(hash, hash_string).first->second.c_str();
        }
        return cache.emplace(hash, int_to_string_id<sid32>(hash)).first->second.c_str();
    }

    [[nodiscard]] const std::vector<std::string>& SIDBase::acquire_strings() noexcept {
        m_acquiredStrings.reserve(m_numEntries);
        m_normalizedAcquiredStrings.reserve(m_numEntries);
        for (u64 i = 0; i < m_numEntries; ++i) {
            const SIDBaseEntry& entry = m_entries[i];
            m_acquiredStrings.emplace_back(reinterpret_cast<const char*>(&m_sidbytes[entry.offset]));
            m_normalizedAcquiredStrings.push_back(m_acquiredStrings.back());
            for (char& ch : m_normalizedAcquiredStrings.back()) {
                ch = std::tolower(ch);
            }
        }
        return m_acquiredStrings;
    }

}
