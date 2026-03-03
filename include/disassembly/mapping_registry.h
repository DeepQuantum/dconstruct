#pragma once

#include "disassembly/mapping_types.h"
#include "sidbase.h"
#include <unordered_map>
#include <optional>
#include <mutex>
#include <filesystem>

namespace dconstruct {

    class MappingRegistry {
    public:
        void observe_struct(const sid64 type_id, const u32 total_size);
        void observe_pointer_member(const sid64 type_id, const u32 offset, const u32 size, const std::optional<sid64> pointed_type_id);
        void observe_primitive_member(const sid64 type_id, const u32 offset, const u32 size, const ast::primitive_kind kind, const u64 raw_value);
        void observe_unknown_member(const sid64 type_id, const u32 offset, const u32 size);

        [[nodiscard]] std::optional<mapping_struct_type> get_struct(const sid64 type_id) const;
        [[nodiscard]] std::vector<mapping_struct_type> get_all_structs() const;
        void dump_types_file(const std::filesystem::path& out_path, const SIDBase& sidbase) const;

    private:
        [[nodiscard]] mapping_struct_type& get_or_create_struct(const sid64 type_id);
        [[nodiscard]] mapping_member_type& get_or_create_member(mapping_struct_type& type, const u32 offset, const u32 size);

        mutable std::mutex m_mutex;
        std::unordered_map<sid64, mapping_struct_type> m_structs;
    };
}
