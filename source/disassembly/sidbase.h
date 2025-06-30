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
        void load(const std::filesystem::path& path) noexcept;
        [[nodiscard]] const char* search(const sid64 hash) const noexcept;
        [[nodiscard]] b8 sid_exists(const sid64 hash) const noexcept;
        sid64 m_lowestSid;
        sid64 m_highestSid;


    private:
        u64 m_numEntries = 0;
        std::unique_ptr<std::byte[]> m_sidbytes;
        SIDBaseEntry* m_entries = nullptr;
    };
}

