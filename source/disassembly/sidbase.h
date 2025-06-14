#pragma once
#include "base.h"
#include <cassert>
#include <memory>

struct SIDBaseEntry {
    sid64 hash;
    u64 offset;
};

class SIDBase {

public:
    void load(const char* path = "./sidbase.bin") noexcept;
    [[nodiscard]] const char *search(const sid64 hash) const noexcept;
    [[nodiscard]] b8 sid_exists(const sid64 hash) const noexcept;
    
private:
    u64 m_num_entries = 0;
    std::unique_ptr<u8[]> m_sidbytes;
    SIDBaseEntry *m_entries = nullptr;
};

