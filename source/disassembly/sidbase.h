#pragma once
#include "base.h"
#include <cassert>

struct SIDBaseEntry {
    stringid_64 hash;
    u64 offset;
};

class SIDBase {

public:
    void load(const char* path = "./sidbase.bin") noexcept;
    [[nodiscard]] const char *search(const stringid_64 hash) const noexcept;
    [[nodiscard]] const b8 sid_exists(const stringid_64 hash) const noexcept;
    ~SIDBase() noexcept;
    
private:
    u64 m_num_entries = 0;
    u8 *m_sidbytes = nullptr;
    SIDBaseEntry *m_entries = nullptr;
};

