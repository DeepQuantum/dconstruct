#pragma once
#include "base.h"
#include <cassert>

struct SIDBaseEntry {
    stringid_64 hash;
    u64 offset;
};

struct SIDBase {
    u64 m_num_entries;
    u8 *m_sidbytes;
    SIDBaseEntry *m_entries;

    [[nodiscard]] const char *search(const stringid_64 hash) const noexcept {
        u64 low = 0;
        u64 high = m_num_entries - 1;
        u64 mid = 0;

        while (low <= high) {
            mid = low + (high - low) / 2;
            const SIDBaseEntry* current = m_entries + mid;

            if (current->hash == hash) [[unlikely]]
                return reinterpret_cast<const char*>(this->m_sidbytes + current->offset);

            if (current->hash < hash) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }
        return nullptr;
    }

    ~SIDBase() {
        delete[] this->m_sidbytes;
    }
};

