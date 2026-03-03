#include "disassembly/mapping_registry.h"
#include <fstream>
#include <algorithm>
#include <array>

namespace dconstruct {

    [[nodiscard]] static std::string get_type_name_from_sid(const sid64 sid, const SIDBase& sidbase) {
        const char* sid_str = sidbase.search(sid);
        if (sid_str == nullptr) {
            return sanitize_dc_string(int_to_string_id<sid64>(sid));
        }
        return sanitize_dc_string(sid_str);
    }

    [[nodiscard]] static std::string member_name_from_offset(const u32 offset) {
        constexpr std::array<char, 16> chars = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
        std::string name = "member_";
        name.reserve(11);
        name.push_back(chars[(offset >> 12) & 0xF]);
        name.push_back(chars[(offset >> 8) & 0xF]);
        name.push_back(chars[(offset >> 4) & 0xF]);
        name.push_back(chars[offset & 0xF]);
        return name;
    }

    [[nodiscard]] static ast::full_type mapping_member_to_type(const mapping_member_type& member, const SIDBase& sidbase) {
        if (member.m_kind == mapping_member_kind::Pointer) {
            if (member.m_pointedStructTypeId.has_value()) {
                ast::struct_type pointed_type;
                pointed_type.m_name = get_type_name_from_sid(*member.m_pointedStructTypeId, sidbase);
                return ast::ptr_type(std::make_shared<ast::full_type>(ast::full_type{pointed_type}));
            }
            return ast::ptr_type{};
        }
        if (member.m_kind == mapping_member_kind::Primitive && member.m_primitive.has_value()) {
            return ast::make_type_from_prim(*member.m_primitive);
        }
        return ast::make_type_from_prim(ast::primitive_kind::U8);
    }

    [[nodiscard]] static ast::struct_type mapping_struct_to_ast_struct(const mapping_struct_type& mapping_struct, const SIDBase& sidbase) {
        ast::struct_type result;
        result.m_name = get_type_name_from_sid(mapping_struct.m_typeId, sidbase);

        for (const auto& [offset, member] : mapping_struct.m_members) {
            result.m_members.emplace(member_name_from_offset(offset), std::make_shared<ast::full_type>(mapping_member_to_type(member, sidbase)));
        }

        return result;
    }

    [[nodiscard]] mapping_struct_type& MappingRegistry::get_or_create_struct(const sid64 type_id) {
        auto [it, inserted] = m_structs.emplace(type_id, mapping_struct_type{});
        if (inserted) {
            it->second.m_typeId = type_id;
        }
        return it->second;
    }

    [[nodiscard]] mapping_member_type& MappingRegistry::get_or_create_member(mapping_struct_type& type, const u32 offset, const u32 size) {
        auto [it, inserted] = type.m_members.emplace(offset, mapping_member_type{});
        if (inserted) {
            it->second.m_offset = offset;
            it->second.m_size = size;
        } else {
            it->second.m_size = std::max(it->second.m_size, size);
        }
        return it->second;
    }

    void MappingRegistry::observe_struct(const sid64 type_id, const u32 total_size) {
        std::scoped_lock lock(m_mutex);
        mapping_struct_type& type = get_or_create_struct(type_id);
        ++type.m_totalObservations;
        type.m_totalSize = std::max(type.m_totalSize, total_size);
    }

    void MappingRegistry::observe_pointer_member(const sid64 type_id, const u32 offset, const u32 size, const std::optional<sid64> pointed_type_id) {
        std::scoped_lock lock(m_mutex);
        mapping_struct_type& type = get_or_create_struct(type_id);
        mapping_member_type& member = get_or_create_member(type, offset, size);

        if (member.m_kind != mapping_member_kind::Pointer && member.m_kind != mapping_member_kind::Unknown) {
            ++member.m_conflicts;
        }

        member.m_kind = mapping_member_kind::Pointer;
        member.m_relocSeen = true;
        member.m_primitive = std::nullopt;
        member.m_primitiveVotes.clear();
        if (pointed_type_id.has_value()) {
            member.m_pointedStructTypeId = pointed_type_id;
        }
    }

    void MappingRegistry::observe_primitive_member(const sid64 type_id, const u32 offset, const u32 size, const ast::primitive_kind kind, const u64 raw_value) {
        std::scoped_lock lock(m_mutex);
        mapping_struct_type& type = get_or_create_struct(type_id);
        mapping_member_type& member = get_or_create_member(type, offset, size);

        if (member.m_relocSeen || member.m_kind == mapping_member_kind::Pointer) {
            ++member.m_conflicts;
            return;
        }

        member.m_kind = mapping_member_kind::Primitive;
        ++member.m_primitiveVotes[kind];

        if (!member.m_primitive.has_value()) {
            member.m_primitive = kind;
        } else if (member.m_primitive.value() != kind) {
            const u64 current_votes = member.m_primitiveVotes[member.m_primitive.value()];
            const u64 candidate_votes = member.m_primitiveVotes[kind];
            if (candidate_votes > current_votes) {
                member.m_primitive = kind;
            }
            ++member.m_conflicts;
        }

        member.m_valueProfile.observe(raw_value);
    }

    void MappingRegistry::observe_unknown_member(const sid64 type_id, const u32 offset, const u32 size) {
        std::scoped_lock lock(m_mutex);
        mapping_struct_type& type = get_or_create_struct(type_id);
        mapping_member_type& member = get_or_create_member(type, offset, size);
        if (member.m_kind != mapping_member_kind::Unknown) {
            ++member.m_conflicts;
        }
    }

    [[nodiscard]] std::optional<mapping_struct_type> MappingRegistry::get_struct(const sid64 type_id) const {
        std::scoped_lock lock(m_mutex);
        auto it = m_structs.find(type_id);
        if (it == m_structs.end()) {
            return std::nullopt;
        }
        return it->second;
    }

    [[nodiscard]] std::vector<mapping_struct_type> MappingRegistry::get_all_structs() const {
        std::scoped_lock lock(m_mutex);
        std::vector<mapping_struct_type> types;
        types.reserve(m_structs.size());
        for (const auto& [sid, type] : m_structs) {
            types.push_back(type);
        }
        return types;
    }

    void MappingRegistry::dump_types_file(const std::filesystem::path& out_path, const SIDBase& sidbase) const {
        std::vector<mapping_struct_type> types = get_all_structs();
        std::sort(types.begin(), types.end(), [](const mapping_struct_type& lhs, const mapping_struct_type& rhs) {
            return lhs.m_typeId < rhs.m_typeId;
        });

        std::ofstream out(out_path);
        for (const auto& type : types) {
            const ast::struct_type mapped_struct = mapping_struct_to_ast_struct(type, sidbase);
            out << ast::type_to_definition_string(ast::full_type{mapped_struct}) << "\n\n";
        }
    }
}
