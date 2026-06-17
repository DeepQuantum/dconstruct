#pragma once
#include "base.h"
#include <memory>
#include <filesystem>
#include <expected>
#include <map>
#include <string>
#include <vector>

namespace dconstruct {
    struct SIDBaseEntry {
        sid64 hash;
        u64 offset;
    };

    class SIDBase {
    public:
        SIDBase(const u64 num_entries, std::unique_ptr<std::byte[]>&& bytes, SIDBaseEntry* entries, const sid64 lowest, const sid64 highest, const std::size_t byte_size) : m_numEntries(num_entries), m_sidbytes(std::move(bytes)), m_entries(entries), m_lowestSid(lowest), m_highestSid(highest), m_byteSize(byte_size) {};

        [[nodiscard]] static resstr<SIDBase> from_binary(const std::filesystem::path& path) noexcept;

        [[nodiscard]] static SIDBase from_caches(const std::vector<std::map<sid64, std::string>>& caches);

        bool dump(const std::filesystem::path& path) const;

        [[nodiscard]] const char* search(const sid64 hash) const noexcept;
        [[nodiscard]] bool sid_exists(const sid64 hash) const noexcept;
        [[nodiscard]] const char* lookup(const sid64 hash) const noexcept;
        [[nodiscard]] const char* lookup(const sid32 hash) const noexcept;
        [[nodiscard]] const char* lookup(const sid64 hash, std::map<sid64, std::string>& cache) const;
        [[nodiscard]] const char* lookup(const sid32 hash, std::map<sid64, std::string>& cache) const;
        const std::vector<std::string>& acquire_strings() noexcept;

        [[nodiscard]] DC_ALWAYS_INLINE const std::vector<std::string>& get_normalized_acquired_strings() const noexcept {
            return m_normalizedAcquiredStrings;
        }

        [[nodiscard]] DC_ALWAYS_INLINE const std::vector<std::string>& get_acquired_strings() const noexcept {
            return m_acquiredStrings;
        }

        sid64 m_lowestSid;
        sid64 m_highestSid;

    private:
        u64 m_numEntries;
        std::unique_ptr<std::byte[]> m_sidbytes;
        SIDBaseEntry* m_entries;
        std::size_t m_byteSize;

        // for when we need to actually permanently own the strings in the sidbase.
        std::vector<std::string> m_acquiredStrings;
        std::vector<std::string> m_normalizedAcquiredStrings;
    };
}
