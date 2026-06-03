#pragma once

#include "base.h"
#include "ast/type.h"
#include <array>
#include <map>
#include <optional>
#include <unordered_set>

namespace dconstruct {

    enum class mapping_member_kind {
        Unknown,
        Primitive,
        Pointer
    };

    struct mapping_value_profile {
        static constexpr u32 MAX_DISTINCT_VALUES = 16;

        void observe(const u64 raw_value) {
            ++m_totalObservations;
            if (m_distinctValues.size() >= MAX_DISTINCT_VALUES && !m_distinctValues.contains(raw_value)) {
                ++m_overflowObservations;
                return;
            }
            m_distinctValues.emplace(raw_value);
        }

        std::unordered_set<u64> m_distinctValues;
        u64 m_totalObservations = 0;
        u64 m_overflowObservations = 0;
    };

    struct mapping_member_type {
        u32 m_offset = 0;
        u32 m_size = 0;
        mapping_member_kind m_kind = mapping_member_kind::Unknown;
        bool m_relocSeen = false;
        u64 m_conflicts = 0;
        std::optional<ast::primitive_kind> m_primitive;
        std::optional<sid64> m_pointedStructTypeId;
        std::map<ast::primitive_kind, u64> m_primitiveVotes;
        mapping_value_profile m_valueProfile;
    };

    struct mapping_struct_type {
        sid64 m_typeId = 0;
        u32 m_totalSize = 0;
        u64 m_totalObservations = 0;
        std::map<u32, mapping_member_type> m_members;
    };
}
