#pragma once
#include "base.h"

struct SIDBaseEntry {
    stringid_64 hash;
    u64 offset;
};

struct SIDBase {
    u64 m_num_entries;
    u8 *m_sidbytes;
    SIDBaseEntry *m_entries;

    [[nodiscard]] const char *search(const stringid_64 hash) const noexcept {
        const SIDBaseEntry &middle = m_entries[this->m_num_entries / 2];
        if (this->m_num_entries == 1 && middle.hash != hash) {
            return nullptr;
        }
        if (middle.hash == hash) {
            return reinterpret_cast<const char*>(this->m_sidbytes + middle.offset);
        } else if (hash > middle.hash) {
            return this->search(hash, m_entries + this->m_num_entries / 2, m_num_entries / 2);
        } else {
            return this->search(hash, m_entries, this->m_num_entries / 2);
        }
    }

    [[nodiscard]] const char *search(const stringid_64 hash, const SIDBaseEntry *begin, const u64 num_entries) const noexcept {
        const SIDBaseEntry &middle = begin[num_entries / 2];
        if (num_entries == 1 && middle.hash != hash) {
            return nullptr;
        }
        if (middle.hash == hash) {
            return reinterpret_cast<const char*>(this->m_sidbytes + middle.offset);
        } else if (hash > middle.hash) {
            return this->search(hash, begin + num_entries / 2, num_entries / 2);
        } else {
            return this->search(hash, begin, num_entries / 2);
        }
    }

    ~SIDBase() {
        delete[] this->m_sidbytes;
    }
};

