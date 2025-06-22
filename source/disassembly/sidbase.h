#pragma once
#include "base.h"
#include <cassert>
#include <memory>
#include <filesystem>

struct SIDBaseEntry {
    sid64 hash;
    u64 offset;
};

class SIDBase {

public:
    void load(const std::filesystem::path &path = "./sidbase.bin") noexcept;
    [[nodiscard]] const char *search(const sid64 hash) const noexcept;
    [[nodiscard]] b8 sid_exists(const sid64 hash) const noexcept;
    sid64 m_lowestSid;

    
private:
    u64 m_numEntries = 0;
    std::unique_ptr<u8[]> m_sidbytes;
    SIDBaseEntry *m_entries = nullptr;
};

