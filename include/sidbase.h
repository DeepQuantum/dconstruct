#pragma once
#include "base.h"
#include <memory>
#include <filesystem>

namespace dconstruct {
    struct SIDBaseEntry {
        sid64 hash;
        u64 offset;
    };

    class SIDBase {

    public:
        //explicit SIDBase(const std::filesystem::path& path);
        
        SIDBase(const u64 num_entries, std::unique_ptr<std::byte[]>&& bytes, SIDBaseEntry* entries, const sid64 lowest, const sid64 highest) : 
        m_numEntries(num_entries), m_sidbytes(std::move(bytes)), m_entries(entries), m_lowestSid(lowest), m_highestSid(highest) {};

        [[nodiscard]] static SIDBase from_binary(const std::filesystem::path& path) noexcept;
        //[[nodiscard]] static SIDBase from_uc4_binary(const std::filesystem::path& path) noexcept;
        
        [[nodiscard]] const char* search(const sid64 hash) const noexcept;
        [[nodiscard]] bool sid_exists(const sid64 hash) const noexcept;
        sid64 m_lowestSid;
        sid64 m_highestSid;


    private:
        u64 m_numEntries;
        std::unique_ptr<std::byte[]> m_sidbytes;
        SIDBaseEntry* m_entries;
    };
}

