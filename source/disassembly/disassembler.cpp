#include "ast/type.h"
#include "disassembly/custom_structs.h"
#include <cmath>
#include "disassembly/disassembler.h"
#include <cstring>
#include <functional>
#include <sstream>
#include <iostream>

namespace dconstruct {

    [[nodiscard]] const std::vector<const function_disassembly*> Disassembler::get_all_functions() noexcept {
        std::vector<const function_disassembly*> funcs;
        for (auto& func : m_functions) {
            if (func->m_isEmbeddedFunction) {
                const auto& ids = m_offsetsToFunctionNames.at(func->m_originalOffset);
                std::ostringstream final_id;
                for (const auto& id : ids) {
                    final_id << id << "\n";
                }
                final_id << "embedded@" << std::hex << func->m_originalOffset;
                func->m_id = final_id.str();
            }
            funcs.push_back(func.get());
        }
        return funcs;
    }

    [[nodiscard]] std::vector<const function_disassembly*> Disassembler::get_named_functions() const noexcept {
        std::vector<const function_disassembly*> funcs;
        for (const auto& func : m_functions) {
            if (!func->get_id().starts_with("anonymous")) {
                funcs.push_back(func.get());
            }
        }
        return funcs;
    }

    [[nodiscard]] const function_disassembly* Disassembler::get_function_at_offset(const u64 offset) const noexcept {
        for (const auto& func : m_functions) {
            if (func->m_originalOffset == offset) {
                return func.get();
            }
        }
        return nullptr;
    }

    [[nodiscard]] bool Disassembler::is_unmapped_sid(const location loc) const noexcept {
        const bool in_range = loc.get<sid64>() >= m_sidbase->m_lowestSid && loc.get<sid64>() <= m_sidbase->m_highestSid;
        const bool is_fileptr = m_currentFile->is_file_ptr(loc);
        return loc.is_aligned() && in_range && !is_fileptr;
    }

    [[nodiscard]] bool Disassembler::is_possible_float(const f32* val) const noexcept {
        f32 rounded = std::roundf(*val * 1e4f) / 1e4f;
        return std::fabsf(*val - rounded) < 1e-4f && *val > -1e4f && *val < 1e4f && rounded != 0.f;
    }

    [[nodiscard]] bool Disassembler::is_possible_i32(const i32* val) const noexcept {
        return std::abs(*val) < 50000;
    }

    /*
        a "struct" is a piece of data that has a type ID and members following it.
        an "array-like" is essentially just a size + pointer. the thing being pointed at can be anything, either proper structs
        or more array-likes. array-likes may also point at array-structs, which are just blocks of data with the type ID "array"
        followed by structs of undertermined size. array structs don't provide their own size, so the array-like must provide the size.
        array structs may contain arbitrarily sized members though, so we must check each member individually.

    */

    u8 Disassembler::insert_struct_or_arraylike(const location struct_location, disassembled_values_t& values) {
        u8 bytes_inserted = 0;
        if (m_currentFile->is_file_ptr(struct_location)) {
            const location ptr_value = location().from(struct_location);
            if (get_offset(ptr_value) >= m_currentFile->m_size) {
                return 8;
            }
            if (m_currentFile->is_string(ptr_value)) {
                values.emplace_back(string_value{ptr_value.as<char>(), get_offset(struct_location)});
                bytes_inserted = 8;
                return bytes_inserted;
            }
            const location next_struct_header = location().from(struct_location, -8);
            if (!next_struct_header.is_aligned() || !is_unmapped_sid(next_struct_header)) {
                insert_anonymous_array(struct_location, values);
            } else if (next_struct_header.get<sid64>() == SID("array")) {
                values.emplace_back(insert_array(struct_location, get_size_array(struct_location)));
            } else {
                disassembled_value nested = insert_struct(next_struct_header.as<structs::unmapped>());
                nested.m_pointerOffset = get_offset(struct_location);
                values.emplace_back(std::move(nested));
            }
            bytes_inserted = 8;
        } else {
            if (struct_location >= m_currentFile->m_strings) {
                values.emplace_back(string_value{struct_location.as<char>(), std::nullopt});
                bytes_inserted = 8;
            } else {
                bytes_inserted = insert_next_struct_member(struct_location, values);
            }
        }
        return bytes_inserted;
    }

    // anonymous arrays are arrays that have no clear type ID and can be indexed at any position with any length.
    // typically they are indexed via (u32 offset, u32 length, i32 const -1)
    // right now i think they can only contain pointers to other structs, which means we can pass the element size 8
    void Disassembler::insert_anonymous_array(const location anon_array, disassembled_values_t& values) {
        const u32 anonymous_array_size = anon_array.get<u32>(8);
        if (anonymous_array_size > 50'000) {
            return;
        }

        values.emplace_back(insert_array(anon_array, anonymous_array_size, sizeof(p64)));
    }

    [[nodiscard]] u32 Disassembler::get_size_array(const location array) {
        u32 size_array_front = array.get<u32>(-8);
        u32 size_array_back = array.get<u32>(8);
        u32 size_array = 0;
        constexpr u32 max_allowed_size_array_u32 = 100'000;
        constexpr u16 max_allowed_size_array_u16 = 1'000;
        constexpr u16 bit_mask_u16 = 0xFFFF;
        if (size_array_front > max_allowed_size_array_u32 || size_array_front == 0) {
            const u16 small_size_front = size_array_front & bit_mask_u16;
            if (small_size_front < max_allowed_size_array_u16 && size_array_front > 0) {
                size_array = size_array_front & bit_mask_u16;
            } else if (size_array_back > max_allowed_size_array_u32 || size_array_back == 0) {
                if ((size_array_back & bit_mask_u16) < max_allowed_size_array_u16 && size_array_back > 0) {
                    size_array = size_array_back & bit_mask_u16;
                } else {
                    return 0;
                }
            } else {
                size_array = size_array_back;
            }
        } else {
            size_array = size_array_front;
        }
        return size_array;
    }

    disassembled_value Disassembler::insert_array(const location array, const u32 array_size, u32 struct_size) {
        disassembled_value array_value{
            .m_typeId = SID("array"),
            .m_offset = get_offset(location().from(array)),
            .m_values = {},
        };

        if (array_size == 0) {
            return array_value;
        }

        u32 member_offset = 8;
        const location member = location().from(array);

        while (!m_currentFile->is_string(member + member_offset) && !m_currentFile->gets_pointed_at(member + member_offset) && member_offset <= m_currentFile->m_size) {
            member_offset += 8;
        }


        if (struct_size == 0) {
            const u8 type_id_padding = m_currentFile->is_string(member + member_offset) ? 0 : 8;
            f32 struct_size_f = f32(member_offset - type_id_padding) / f32(array_size);
            if (struct_size_f != std::trunc(struct_size_f) || struct_size_f == .0) {
                struct_size_f = f32(member_offset) / f32(array_size);
                if (struct_size_f != std::trunc(struct_size_f)) {
                    struct_size_f = std::round(struct_size_f / 8.f) * 8.f;
                }
            }
            // assert(struct_size_f != 0.0);
            struct_size = (u32)struct_size_f;
        }


        for (u32 array_entry_count = 0; array_entry_count < array_size; ++array_entry_count) {
            member_offset = 0;
            disassembled_value array_entry{
                .m_typeId = 0,
                .m_offset = get_offset(member + array_entry_count * struct_size),
                .m_values = {},
            };

            while (member_offset < struct_size) {
                const location current_member_location = member + (array_entry_count * struct_size + member_offset);
                const u32 last_member_size = insert_struct_or_arraylike(current_member_location, array_entry.m_values);
                member_offset += last_member_size ? last_member_size : 8;
            }

            array_value.m_values.emplace_back(std::move(array_entry));
        }
        return array_value;
    }

    void Disassembler::insert_unmapped_struct(const structs::unmapped* struct_ptr, disassembled_values_t& values) {
        u64 member_offset = 0;
        bool offset_gets_pointed_at = false;
        u64 last_member_size = 0;
        const location member_start = location(&struct_ptr->m_data);
        location member_location = member_start;
        while (!offset_gets_pointed_at) {
            member_offset += last_member_size;
            last_member_size = insert_next_struct_member(member_start + member_offset, values);
            member_location = member_start + (member_offset + last_member_size);
            offset_gets_pointed_at = m_currentFile->gets_pointed_at(member_location + 8) || m_currentFile->is_string(member_location);
        }
    }

    u8 Disassembler::insert_next_struct_member(const location member, disassembled_values_t& values) {
        u8 member_size;
        if (m_currentFile->is_file_ptr(member)) {
            if (member >= m_currentFile->m_strings) {
                values.emplace_back(string_value{member.as<char>(), std::nullopt});
            } else {
                insert_struct_or_arraylike(member, values);
            }
            member_size = 8;
        } else if (m_sidbase->search(member.get<sid64>()) != nullptr) {
            values.emplace_back(member.as<sid64>());
            member_size = 8;
        } else if (is_possible_float(member.as<f32>())) {
            values.emplace_back(member.as<f32>());
            member_size = 4;
        } else if (is_possible_i32(member.as<i32>())) {
            values.emplace_back(member.as<i32>());
            member_size = 4;
        } else if (is_unmapped_sid(member)) {
            values.emplace_back(member.as<u64>());
            member_size = 8;
        } else {
            values.emplace_back(member.as<i32>());
            member_size = 4;
        }
        return member_size;
    }

    [[nodiscard]] u8 Disassembler::insert_next_struct_member(
        const location member,
        const ast::full_type& type,
        disassembled_values_t& values
    ) {
        return std::visit([this, member, &values](auto&& type) -> u8 {
            using member_type = std::decay_t<decltype(type)>;

            if constexpr (std::is_same_v<member_type, ast::primitive_type>) {
                switch (type.m_type) {
                    case ast::primitive_kind::SID:
                    case ast::primitive_kind::U64: {
                        values.emplace_back(member.as<sid64>());
                        return sizeof(sid64);
                    }
                    case ast::primitive_kind::STRING: {
                        values.emplace_back(string_value{member.as<char>(), std::nullopt});
                        return sizeof(const char*);
                    }
                    case ast::primitive_kind::U8: {
                        values.emplace_back(member.as<u8>());
                        return sizeof(u8);
                    }
                    case ast::primitive_kind::U16: {
                        values.emplace_back(member.as<u16>());
                        return sizeof(u16);
                    }
                    case ast::primitive_kind::U32: {
                        values.emplace_back(member.as<u32>());
                        return sizeof(u32);
                    }
                    default: {
                        values.emplace_back(member.as<i32>());
                        return sizeof(i32);
                    }
                }
            } else {
                return insert_struct_or_arraylike(member, values);
            }
        }, type);
    }

    const std::vector<disassembled_entry>& Disassembler::disassemble() {
        m_disassembledEntries.clear();
        m_functions.clear();
        m_offsetsToFunctionNames.clear();
        m_stateScript.reset();
        m_disassembledEntries.reserve(m_currentFile->m_dcheader->m_numEntries);
        for (i32 i = 0; i < m_currentFile->m_dcheader->m_numEntries; ++i) {
            m_disassembledEntries.emplace_back(insert_entry(m_currentFile->m_dcheader->m_pStartOfData + i));
        }
        return m_disassembledEntries;
    }

    std::string Disassembler::disassembly_to_string(const std::vector<disassembled_entry>& entries) {
        static constexpr char ENTRY_SEP[] = "##############################";
        constexpr u8 indent_per_level = 2;

        std::string out;
        out.reserve(0x2FFFFFULL);

        const auto append_format = [&out](const char* format, auto... args) {
            char buffer[1024];
            std::snprintf(buffer, sizeof(buffer), format, args...);
            out += buffer;
        };

        const auto append_indented_text = [&out](const std::string& text, const u32 indent) {
            size_t start = 0;
            while (start < text.size()) {
                const size_t end = text.find('\n', start);
                out.append(indent, ' ');
                if (end == std::string::npos) {
                    out.append(text, start, std::string::npos);
                    break;
                }
                out.append(text, start, end - start + 1);
                start = end + 1;
            }
        };

        const auto append_goto_label = [&out, append_format](const std::vector<u32>& labels, const function_disassembly_line& line, const u32 func_size, const std::vector<function_disassembly_line>& lines) {
            if (line.m_target == std::numeric_limits<u16>::max()) {
                return;
            }
            const u32 target = static_cast<u32>(std::distance(labels.begin(), std::find(labels.begin(), labels.end(), line.m_target)));
            if (line.m_target == func_size) {
                out += "=> L_RETURN";
            } else if (line.m_target == func_size - 1 && lines[line.m_target].m_instruction.opcode == Opcode::LoadU16Imm) {
                append_format("=> L_RETURN_%d", lines[line.m_target].m_instruction.operand1);
            } else {
                append_format("=> L_%d", target);
            }
        };

        const auto append_label = [this, &out, append_format](const std::vector<u32>& labels, const function_disassembly_line& line, const u32 func_size, const u32 indent) {
            auto label_location = std::find(labels.begin(), labels.end(), line.m_location);
            if (label_location == labels.end()) {
                return;
            }
            const u32 label_indent = indent > indent_per_level ? indent - indent_per_level : 0;
            const u32 label_index = static_cast<u32>(std::distance(labels.begin(), label_location));
            out.append(indent, ' ');
            if (line.m_location == func_size) {
                out += "L_RETURN:\n";
            } else if (line.m_location == func_size - 1 && line.m_instruction.opcode == Opcode::LoadU16Imm) {
                append_format("L_RETURN_%d:\n", line.m_instruction.operand1);
            } else {
                append_format("L_%d:\n", label_index);
            }
        };

        const auto append_function_disassembly = [&, this](const function_disassembly& function_disassembly, const u32 indent) {
            const auto labels = function_disassembly.m_stackFrame.m_labels;

            if (!function_disassembly.m_stackFrame.m_registerArgs.empty()) {
                out.append(indent, ' ');
                append_format("[%zu args]\n", function_disassembly.m_stackFrame.m_registerArgs.size());
            }

            for (const auto& line : function_disassembly.m_lines) {
                const u32 line_offset = static_cast<u32>(std::max(67 - static_cast<i32>(line.m_text.length()), 0));
                append_label(labels, line, static_cast<u32>(function_disassembly.m_lines.size() - 1), indent);
                out.append(indent, ' ');
                out += line.m_text;
                out.append(line_offset, ' ');
                out += line.m_comment;
                append_goto_label(labels, line, static_cast<u32>(function_disassembly.m_lines.size() - 1), function_disassembly.m_lines);
                out += '\n';
            }

            out += '\n';
            out.append(indent, ' ');
            out += "SYMBOL TABLE: \n";

            const auto& [table_location, types] = function_disassembly.m_stackFrame.m_symbolTable;
            for (u32 i = 0; i < types.size(); ++i) {
                const auto& type = types[i];
                location value_location = table_location + i * 8;
                out.append(indent, ' ');
                append_format("%04X   0x%06X    ", i, get_offset(value_location));
                std::visit([&](auto&& entry) -> void {
                    using T = std::decay_t<decltype(entry)>;
                    if constexpr (std::is_same_v<T, ast::primitive_type>) {
                        switch (entry.m_type) {
                            case ast::primitive_kind::I32:
                                append_format("int: %i\n", value_location.get<i32>());
                                break;
                            case ast::primitive_kind::I64:
                                append_format("int64: %lli\n", value_location.get<i64>());
                                break;
                            case ast::primitive_kind::U64:
                                append_format("uint64: %llu\n", value_location.get<u64>());
                                break;
                            case ast::primitive_kind::F32:
                                append_format("float: %f\n", value_location.get<f32>());
                                break;
                            case ast::primitive_kind::STRING:
                                append_format("string: \"%s\"\n", value_location.get<const char*>());
                                break;
                            case ast::primitive_kind::SID:
                                append_format("sid: %s\n", m_sidbase->lookup(value_location.get<sid64>(), m_currentFile->m_sidCache));
                                break;
                            default:
                                append_format("unknown: %llu\n", value_location.get<u64>());
                                break;
                        }
                    } else if constexpr (std::is_same_v<T, ast::function_type>) {
                        if (entry.m_return != nullptr && !is_unknown(*entry.m_return)) {
                            append_format("function: %s%s\n", m_sidbase->lookup(value_location.get<sid64>(), m_currentFile->m_sidCache), ast::type_to_declaration_string(entry).c_str());
                        } else {
                            append_format("function: %s\n", m_sidbase->lookup(value_location.get<sid64>(), m_currentFile->m_sidCache));
                        }
                    } else if constexpr (std::is_same_v<T, ast::ptr_type>) {
                        append_format("pointer: \"%s\" (%s)\n", m_sidbase->lookup(value_location.get<sid64>(), m_currentFile->m_sidCache), ast::type_to_declaration_string(entry).c_str());
                    } else if constexpr (std::is_same_v<T, std::monostate>) {
                        append_format("unknown: %llu\n", value_location.get<u64>());
                    } else {
                        append_format("unknown: %llu\n", value_location.get<u64>());
                    }
                },type);
            }
        };

        const auto append_state_script_lambda = [&](const ast::state_script_lambda& lam, const u32 indent) {
            std::visit([&](const auto& lambda) {
                using L = std::decay_t<decltype(lambda)>;
                if constexpr (std::is_same_v<L, std::shared_ptr<function_disassembly>>) {
                    out.append(indent, ' ');
                    append_format("lambda %s {\n", lambda->get_id().c_str());
                    append_function_disassembly(*lambda, indent + indent_per_level);
                    out.append(indent, ' ');
                    out += "}\n";
                } else {
                    ast::ast_serialization_buffer buffer;
                    buffer.m_indentWidth = indent_per_level;
                    buffer.append(lambda);
                    append_indented_text(buffer.str(), indent);
                    out += '\n';
                }
            }, lam);
        };

        const auto append_state_script = [&](const ast::state_script& script, const u32 indent) {
            const u32 i1 = indent + indent_per_level;
            const u32 i2 = i1 + indent_per_level;
            const u32 i3 = i2 + indent_per_level;
            const u32 i4 = i3 + indent_per_level;

            out.append(indent, ' ');
            append_format("statescript #%s {\n", script.m_name.c_str());

            out.append(i1, ' ');
            out += "options {\n";
            for (const auto& opt : script.m_options) {
                out.append(i2, ' ');
                out += opt.to_pseudo_c_string();
                out += '\n';
            }
            out.append(i1, ' ');
            out += "}\n";

            out.append(i1, ' ');
            out += "declarations {\n";
            for (const auto& decl : script.m_declarations) {
                out.append(i2, ' ');
                out += decl.m_identifier;
                out += '\n';
            }
            out.append(i1, ' ');
            out += "}\n";

            for (const auto& state : script.m_states) {
                out.append(i1, ' ');
                append_format("state %s {\n", state.m_name.c_str());
                for (const auto& block : state.m_blocks) {
                    out.append(i2, ' ');
                    append_format("block %s {\n", block.block_type_to_string().c_str());
                    for (const auto& track : block.m_tracks) {
                        out.append(i3, ' ');
                        append_format("track %s {\n", track.m_name.c_str());
                        for (const auto& lam : track.m_lambdas) {
                            append_state_script_lambda(lam, i4);
                        }
                        out.append(i3, ' ');
                        out += "}\n";
                    }
                    out.append(i2, ' ');
                    out += "}\n";
                }
                out.append(i1, ' ');
                out += "}\n";
            }

            out.append(indent, ' ');
            out += "}\n";
        };

        std::function<void(const disassembled_values_t&, u32)> append_values;
        std::function<void(const disassembled_values_t::value_type&, u32)> append_value;
        std::function<void(sid64, p64, const disassembled_values_t&, u32)> append_struct_like;

        append_values = [&](const disassembled_values_t& values, const u32 indent) {
            u32 member_count = 0;
            for (const auto& value : values) {
                out.append(indent, ' ');
                append_format("[%u] ", member_count++);
                append_value(value, indent);
            }
        };

        append_struct_like = [&](const sid64 type_id, const p64 offset, const disassembled_values_t& values, const u32 indent) {
            if (type_id == SID("array")) {
                append_format("array [0x%x] {size: %zu} {\n", offset, values.size());
                for (u32 i = 0; i < values.size(); ++i) {
                    out.append(indent + indent_per_level, ' ');
                    if (const auto* entry = std::get_if<disassembled_value>(&values[i]); entry != nullptr && entry->m_typeId == 0) {
                        append_format("[%u] anonymous struct [0x%x] {\n", i, entry->m_offset);
                        append_values(entry->m_values, indent + indent_per_level * 2);
                        out.append(indent + indent_per_level, ' ');
                        out += "}\n";
                    } else {
                        append_format("[%u] ", i);
                        append_value(values[i], indent + indent_per_level);
                    }
                }
                out.append(indent, ' ');
                out += "}\n";
                return;
            }

            if (type_id == 0) {
                append_format("anonymous struct [0x%05X] {\n", offset);
            } else {
                append_format("%s [0x%05X] {\n", m_sidbase->lookup(type_id, m_currentFile->m_sidCache), offset);
            }

            if (type_id == SID("script-lambda")) {
                for (const auto& value : values) {
                    if (const auto* function = std::get_if<std::shared_ptr<function_disassembly>>(&value)) {
                        append_function_disassembly(**function, indent + indent_per_level * 2);
                    }
                }
            } else if (type_id == SID("state-script")) {
                for (const auto& value : values) {
                    if (const auto* state_script = std::get_if<const ast::state_script*>(&value)) {
                        append_state_script(**state_script, indent + indent_per_level);
                    }
                }
            } else if (type_id == SID("map") || type_id == SID("map-32") || type_id == SID("render-settings-map") || type_id == SID("hash-table")) {
                bool seen_keys = false;
                for (const auto& value : values) {
                    if (const auto* map = std::get_if<const structs::map*>(&value); map != nullptr && *map != nullptr) {
                        out.append(indent + indent_per_level, ' ');
                        append_format("keys: [0x%05X], values: [0x%05X]\n", get_offset((*map)->keys.data), get_offset((*map)->values.data));
                    } else {
                        out.append(indent + indent_per_level, ' ');
                        out += seen_keys ? "values " : "keys ";
                        seen_keys = true;
                        append_value(value, indent + indent_per_level);
                    }
                }
            } else {
                append_values(values, indent + indent_per_level);
            }

            out.append(indent, ' ');
            out += "}\n";
        };

        append_value = [&](const disassembled_value_content& value, const u32 indent) {
            const disassembled_value_content* value_to_use = &value;
            if (const mapped_value* mapped_value_ref = std::get_if<mapped_value>(&value)) {
                append_format("%s ", mapped_value_ref->m_name.c_str());
                value_to_use = mapped_value_ref->m_value.get();
            }

            std::visit([&](auto&& entry) {
                using T = std::decay_t<decltype(entry)>;

                if constexpr (std::is_same_v<T, disassembled_value>) {
                    append_struct_like(entry.m_typeId, entry.m_offset, entry.m_values, indent);
                } else if constexpr (std::is_same_v<T, std::shared_ptr<function_disassembly>>) {
                    out += '\n';
                    append_function_disassembly(*entry, indent + indent_per_level);
                } else if constexpr (std::is_same_v<T, const ast::state_script*>) {
                    out += '\n';
                    append_state_script(*entry, indent + indent_per_level);
                } else if constexpr (std::is_same_v<T, const u8*>) {
                    append_format("u8: %u\n", *entry);
                } else if constexpr (std::is_same_v<T, const u16*>) {
                    append_format("u16: %u\n", *entry);
                } else if constexpr (std::is_same_v<T, const i32*> || std::is_same_v<T, const u32*>) {
                    append_format("int: %d\n", *entry);
                } else if constexpr (std::is_same_v<T, const u64*>) {
                    append_format("sid: %s\n", m_sidbase->lookup(*entry, m_currentFile->m_sidCache));
                } else if constexpr (std::is_same_v<T, const f32*>) {
                    append_format("float: %.2f\n", *entry);
                } else if constexpr (std::is_same_v<T, string_value>) {
                    append_format("string: \"%s\"\n", entry.m_chars != nullptr ? entry.m_chars : "");
                } else if constexpr (std::is_same_v<T, const structs::map*>) {
                    append_format("keys: [0x%05X], values: [0x%05X]\n\n", get_offset(entry->keys.data), get_offset(entry->values.data));
                }
            }, *value_to_use);
        };

        for (u32 i = 0; i < entries.size(); ++i) {
            const disassembled_entry& entry = entries[i];
            out += "\n\n";
            out += ENTRY_SEP;
            append_format("  ENTRY %u  ", i);
            out += ENTRY_SEP;
            out += "\n\n";
            append_format("%s = ", m_sidbase->lookup(entry.m_nameId, m_currentFile->m_sidCache));
            append_struct_like(entry.m_typeId, entry.m_offset, entry.m_values, 0);
        }

        if (entries.empty()) {
            for (const auto& function : m_functions) {
                append_function_disassembly(*function, indent_per_level * 2);
            }
        }

        return out;
    }

    disassembled_entry Disassembler::insert_entry(const Entry* entry) {
        m_currentEmbeddedFunctionId = embedded_function_id{};
        const structs::unmapped* struct_ptr = reinterpret_cast<const structs::unmapped*>(reinterpret_cast<const u64*>(entry->m_entryPtr) - 1);
        const char* entry_name = m_sidbase->lookup(entry->m_nameID, m_currentFile->m_sidCache);
        m_currentEmbeddedFunctionId.m_entry = entry_name;
        disassembled_value entry_value = insert_struct(struct_ptr, entry->m_nameID, entry->m_typeId);
        return disassembled_entry{
            .m_nameId = entry->m_nameID,
            .m_typeId = entry_value.m_typeId,
            .m_offset = entry_value.m_offset,
            .m_values = std::move(entry_value.m_values),
        };
    }

    disassembled_value Disassembler::insert_struct(
        const structs::unmapped* struct_ptr,
        const sid64 name_id,
        const sid64 type_id
    ) {
        const u64 offset = get_offset(&struct_ptr->m_data);

        sid64 effective_type_id = type_id != 0 ? type_id : struct_ptr->typeID;
        const char* struct_name = m_sidbase->lookup(effective_type_id, m_currentFile->m_sidCache);
        disassembled_value value{
            .m_typeId = effective_type_id,
            .m_offset = offset,
            .m_values = {},
            .m_pointerOffset = std::nullopt,
        };

        m_currentEmbeddedFunctionId.m_outerStructs.emplace_back(struct_name, offset);

        switch (effective_type_id) {
            case SID("state-script") :
                {
                const StateScript* script;
                switch (m_game) {
                    case game_type::T2R:
                        script = reinterpret_cast<const StateScript*>(&struct_ptr->m_data);
                        break;
                    case game_type::T1X:
                        script = reinterpret_cast<const StateScript*>(&struct_ptr->m_data + 8);
                        break;
                    default:
                        script = nullptr;
                }
                m_currentFile->m_dcscript = script;
                m_stateScript = insert_state_script(script);
                value.m_values.emplace_back(&*m_stateScript);
                break;
            }
            case SID("script-lambda") :
                {
                const ScriptLambda* script_lambda = reinterpret_cast<const ScriptLambda*>(&struct_ptr->m_data);
                bool already_emitted_func = m_offsetsToFunctionNames.contains(offset);
                std::string name;
                if (name_id == 0) {
                    m_currentEmbeddedFunctionId.m_outerStructs.pop_back();
                    name = m_currentEmbeddedFunctionId.to_string();
                    m_offsetsToFunctionNames[offset].push_back(name);
                    if (already_emitted_func) {
                        break;
                    }
                } else {
                    name = m_sidbase->lookup(name_id, m_currentFile->m_sidCache);
                }
                std::shared_ptr<function_disassembly> function = create_function_disassembly(script_lambda, std::move(name));
                function->m_isEmbeddedFunction = name_id == 0;
                function->m_originalOffset = offset;
                value.m_values.emplace_back(function);
                m_functions.push_back(std::move(function));
                break;
            }
            case SID("map"):
            case SID("map-32"):
            case SID("render-settings-map"):
            case SID("hash-table") :
                {
                const structs::map* map = reinterpret_cast<const structs::map*>(&struct_ptr->m_data);
                value.m_values.emplace_back(map);
                if (map->size > 0 && map->keys.data != nullptr && map->values.data != nullptr) {
                    const auto build_entries = [this](const location data_start, const u32 count) {
                        disassembled_value entries{
                            .m_typeId = SID("array"),
                            .m_offset = get_offset(data_start),
                            .m_values = {},
                            .m_pointerOffset = std::nullopt,
                        };
                        for (u32 i = 0; i < count; ++i) {
                            insert_struct_or_arraylike(data_start + i * sizeof(u64), entries.m_values);
                        }
                        return entries;
                    };
                    value.m_values.emplace_back(build_entries(location(map->keys.data), map->size));
                    value.m_values.emplace_back(build_entries(location(map->values.data), map->size));
                }
                break;
            }
            default: {
                if (auto entry = m_knownTypes->find(effective_type_id); entry != m_knownTypes->end()) {
                    value.m_values = insert_mapped_struct(&struct_ptr->m_data, *entry);
                } else {
                    insert_unmapped_struct(struct_ptr, value.m_values);
                }
                break;
            }
        }
        if (!m_currentEmbeddedFunctionId.m_outerStructs.empty()) {
            m_currentEmbeddedFunctionId.m_outerStructs.pop_back();
        }
        return value;
    }

    [[nodiscard]] disassembled_values_t Disassembler::insert_mapped_struct(
        const location _struct,
        const std::pair<sid64, ast::full_type>& mapped_type
    ) {
        return std::visit([this, _struct, type_id = mapped_type.first](auto&& t) -> disassembled_values_t {
            using outer_type = std::decay_t<decltype(t)>;

            disassembled_values_t mapped_values{};
            if constexpr (std::is_same_v<outer_type, ast::struct_type>) {
                location member_location = _struct;
                for (const auto& [member_name, member_type] : t.m_members) {
                    const auto inserted_size = insert_next_struct_member(member_location, *member_type, mapped_values);
                    mapped_values.back() = mapped_value{member_name, std::make_unique<disassembled_value_content>(std::move(mapped_values.back()))};
                    member_location = member_location + inserted_size;
                }
            }

            return mapped_values;
        }, mapped_type.second);
    }

    [[nodiscard]] u32 Disassembler::get_offset(const location loc) const noexcept {
        return loc.num() - reinterpret_cast<p64>(m_currentFile->m_dcheader);
    }

    [[nodiscard]] u32 Disassembler::get_offset(const void* loc) const noexcept {
        return reinterpret_cast<p64>(loc) - reinterpret_cast<p64>(m_currentFile->m_dcheader);
    }

    [[nodiscard]] static std::string sid_lexeme(const char* name) {
        if (name == nullptr || name[0] == '\0') {
            return "#0";
        }
        if (name[0] == '#') {
            return name;
        }
        return std::string("#") + name;
    }

    [[nodiscard]] static std::string raw_sid_name(const char* name) {
        if (name == nullptr) {
            return "";
        }
        if (name[0] == '#') {
            return name + 1;
        }
        return name;
    }

    [[nodiscard]] static ast::full_type state_script_decl_type(const sid64 type_id, const char* type_name) {
        switch (type_id) {
            case SID("boolean"):
                return ast::make_type_from_prim(ast::primitive_kind::BOOL);
            case SID("float"):
            case SID("timer"):
            case SID("bound-frame"):
                return ast::make_type_from_prim(ast::primitive_kind::F32);
            case SID("string"):
                return ast::make_type_from_prim(ast::primitive_kind::STRING);
            case SID("symbol"):
                return ast::make_type_from_prim(ast::primitive_kind::SID);
            case SID("int32"):
                return ast::make_type_from_prim(ast::primitive_kind::I32);
            case SID("uint64"):
                return ast::make_type_from_prim(ast::primitive_kind::U64);
            default:
                return ast::struct_type{type_name != nullptr ? type_name : ast::UNKNOWN_TYPE_NAME, {}};
        }
    }

    std::optional<ast::variable_declaration> Disassembler::insert_variable(const SsDeclaration* var) {
        if (var == nullptr || !var->m_isVar) {
            return std::nullopt;
        }

        ast::full_type type = state_script_decl_type(var->m_declTypeId, m_sidbase->lookup(var->m_declTypeId, m_currentFile->m_sidCache));
        std::string name = sid_lexeme(m_sidbase->lookup(var->m_declId, m_currentFile->m_sidCache));
        if (var->m_pDeclValue == nullptr) {
            return ast::variable_declaration(std::move(type), std::move(name));
        }

        switch (var->m_declTypeId) {
            case SID("boolean") :
                {
                return ast::variable_declaration(std::move(type), std::move(name), *reinterpret_cast<bool*>(var->m_pDeclValue));
            }
            case SID("float"):
            case SID("timer"):
            case SID("bound-frame") :
                {
                return ast::variable_declaration(std::move(type), std::move(name), *reinterpret_cast<f32*>(var->m_pDeclValue));
            }
            case SID("string") :
                {
                const char* str = *reinterpret_cast<const char**>(var->m_pDeclValue);
                return ast::variable_declaration(std::move(type), std::move(name), std::string(str != nullptr ? str : ""));
            }
            case SID("symbol") :
                {
                expr_uptr sid = std::make_unique<ast::sid_identifier>(sid_lexeme(m_sidbase->lookup(*reinterpret_cast<sid64*>(var->m_pDeclValue), m_currentFile->m_sidCache)));
                return ast::variable_declaration(std::move(type), std::move(name), std::move(sid));
            }
            case SID("int32") :
                {
                return ast::variable_declaration(std::move(type), std::move(name), *reinterpret_cast<i32*>(var->m_pDeclValue));
            }
            case SID("uint64") :
                {
                return ast::variable_declaration(std::move(type), std::move(name), *reinterpret_cast<u64*>(var->m_pDeclValue));
            }
            default: {
                return ast::variable_declaration(std::move(type), std::move(name));
            }
        }
    }

    ast::state_script_block Disassembler::insert_on_block(
        const SsOnBlock* block,
        state_script_function_id& function_name
    ) {
        std::vector<ast::state_script_track> tracks;
        tracks.reserve(block->m_trackGroup.m_numTracks);

        switch (block->m_blockType) {
            using enum BLOCK_TYPE;
            case START: {
                function_name.m_event.m_name = "start";
                break;
            }
            case END: {
                function_name.m_event.m_name = "end";
                break;
            }
            case EVENT: {
                function_name.m_event.m_name = std::string("event_").append(raw_sid_name(m_sidbase->lookup(block->m_blockEventId, m_currentFile->m_sidCache)));
                break;
            }
            case UPDATE: {
                function_name.m_event.m_name = "update";
                break;
            }
            case VIRTUAL: {
                function_name.m_event.m_name = "virtual";
                break;
            }
            default: {
                function_name.m_event.m_name = "unknown";
                break;
            }
        }

        for (i16 i = 0; i < block->m_trackGroup.m_numTracks; ++i) {
            SsTrack* track_ptr = block->m_trackGroup.m_aTracks + i;
            function_name.m_track = {raw_sid_name(m_sidbase->lookup(track_ptr->m_trackId, m_currentFile->m_sidCache)), static_cast<u32>(i)};
            std::vector<ast::state_script_lambda> lambdas;
            lambdas.reserve(track_ptr->m_totalLambdaCount);

            for (i16 j = 0; j < track_ptr->m_totalLambdaCount; ++j) {
                function_name.m_idx = j;
                SsLambda* ss_lambda = &track_ptr->m_pSsLambda[j];
                std::shared_ptr function = create_function_disassembly(ss_lambda->m_pScriptLambda, function_name, true);
                function->m_originalOffset = get_offset(ss_lambda->m_pScriptLambda);
                function->m_stateScriptLambdaPointerOffset = get_offset(&ss_lambda->m_pScriptLambda);
                lambdas.push_back(function);
                m_functions.push_back(function);
            }

            tracks.emplace_back(function_name.m_track.m_name, std::move(lambdas));
        }

        if (block->m_blockType == BLOCK_TYPE::EVENT) {
            return ast::state_script_block(raw_sid_name(m_sidbase->lookup(block->m_blockEventId, m_currentFile->m_sidCache)), std::move(tracks));
        }
        return ast::state_script_block(block->m_blockType, std::move(tracks));
    }

    ast::state_script Disassembler::insert_state_script(const StateScript* stateScript) {
        std::vector<ast::sid_identifier> options;
        if (stateScript->m_pSsOptions != nullptr && stateScript->m_pSsOptions->m_pSymbolArray != nullptr) {
            const SymbolArray* s_array = stateScript->m_pSsOptions->m_pSymbolArray;
            options.reserve(s_array->m_numEntries);
            for (i32 i = 0; i < s_array->m_numEntries; ++i) {
                options.emplace_back(sid_lexeme(m_sidbase->lookup(s_array->m_pSymbols[i], m_currentFile->m_sidCache)));
            }
        }

        std::vector<ast::variable_declaration> declarations;
        SsDeclarationList* decl_table = stateScript->m_pSsDeclList;
        if (decl_table != nullptr) {
            declarations.reserve(decl_table->m_numDeclarations);
            for (i32 i = 0; i < decl_table->m_numDeclarations; ++i) {
                std::optional<ast::variable_declaration> declaration = insert_variable(&decl_table->m_pDeclarations[i]);
                if (declaration) {
                    declarations.push_back(std::move(*declaration));
                }
            }
        }

        std::vector<ast::state_script_state> states;
        states.reserve(stateScript->m_stateCount);
        state_script_function_id anon_name;
        for (i16 i = 0; i < stateScript->m_stateCount; ++i) {
            SsState* state_ptr = stateScript->m_pSsStateTable + i;
            anon_name.m_state = {raw_sid_name(m_sidbase->lookup(state_ptr->m_stateId, m_currentFile->m_sidCache)), static_cast<u32>(i)};
            std::vector<ast::state_script_block> blocks;
            blocks.reserve(state_ptr->m_numSsOnBlocks);
            for (i64 j = 0; j < state_ptr->m_numSsOnBlocks; ++j) {
                anon_name.m_event.m_idx = j;
                blocks.push_back(insert_on_block(state_ptr->m_pSsOnBlocks + j, anon_name));
            }
            states.emplace_back(anon_name.m_state.m_name, std::move(blocks));
        }

        ast::state_script script{
            raw_sid_name(m_sidbase->lookup(stateScript->m_stateScriptId, m_currentFile->m_sidCache)),
            std::move(options),
            std::move(declarations),
            std::move(states)};
        script.m_rawStateScript = stateScript;
        for (i16 i = 0; i < stateScript->m_stateCount; ++i) {
            if (stateScript->m_pSsStateTable[i].m_stateId == stateScript->m_initialStateId) {
                script.m_initialStateIdx = static_cast<u64>(i);
                break;
            }
        }
        return script;
    }

    [[nodiscard]] bool Disassembler::pointer_gets_called(
        const u32 dst,
        const u32 start_idx,
        const function_disassembly& fn
    ) {
        for (auto it = fn.m_lines.begin() + start_idx; it != fn.m_lines.end(); ++it) {
            if (it->m_instruction.destination == dst) {
                return it->m_instruction.opcode == Opcode::Call || it->m_instruction.opcode == Opcode::CallFf;
            }
        }
        return false;
    }

    [[nodiscard]] std::shared_ptr<function_disassembly> Disassembler::create_function_disassembly(
        const ScriptLambda* lambda,
        function_name_variant name,
        const bool is_script_function
    ) {
        Instruction* instructionPtr = reinterpret_cast<Instruction*>(lambda->m_pInstruction);

        // const u32 instruction_count = m_game == game_type::T2R ? lambda->m_numInstructions : lambda->m_numInstructions / 2 - 1;

        const u32 instruction_count = lambda->m_numInstructions;

        std::vector<function_disassembly_line> lines;
        lines.reserve(instruction_count);

        std::shared_ptr func = std::make_shared<function_disassembly>(
            std::move(lines),
            StackFrame(location(lambda->m_pSymbols)),
            std::move(name),
            is_script_function);

        for (u64 i = 0; i < instruction_count; ++i) {
            func->m_lines.emplace_back(i, instructionPtr);
        }

        bool counting_args = true;

        for (u64 i = 0; i < instruction_count; ++i) {
            const p64 offset = get_offset(reinterpret_cast<const void*>(func->m_lines[i].m_globalPointer + func->m_lines[i].m_location));
            process_instruction(i, *func, offset, m_game, *m_sidbase, m_currentFile->m_sidCache, m_currentFile->m_strings);
            if (counting_args) {
                if (func->m_lines[i].m_instruction.operand1 >= ARGUMENT_REGISTERS_IDX) {
                    func->m_stackFrame.m_registerArgs.push_back(std::monostate());
                } else {
                    counting_args = false;
                }
            }
        }

        return func;
    }

    [[nodiscard]] std::shared_ptr<function_disassembly> Disassembler::create_function_disassembly(
        const Instruction* istr_ptr,
        const u64 num_instructions,
        function_name_variant name,
        const location& symbol_table,
        const u64 instruction_base_offset,
        const u64 instruction_stride,
        const game_type game,
        const SIDBase& sidbase,
        std::map<sid64, std::string>& sidcache,
        const bool is_script_function,
        const location& strings_offset
    ) {
        std::vector<function_disassembly_line> lines;
        lines.reserve(num_instructions);

        for (u64 i = 0; i < num_instructions; ++i) {
            lines.emplace_back(i, istr_ptr);
        }

        std::shared_ptr func = std::make_shared<function_disassembly>(
            std::move(lines),
            StackFrame(symbol_table),
            std::move(name),
            is_script_function
        );

        bool counting_args = true;

        for (u64 i = 0; i < num_instructions; ++i) {
            const u64 offset = instruction_base_offset + i * instruction_stride;
            process_instruction(i, *func, offset, game, sidbase, sidcache, strings_offset);
            if (counting_args) {
                if (func->m_lines[i].m_instruction.operand1 >= ARGUMENT_REGISTERS_IDX) {
                    func->m_stackFrame.m_registerArgs.push_back(std::monostate());
                } else {
                    counting_args = false;
                }
            }
        }
        return func;
    }

    void Disassembler::set_register_types(Register& op1, Register& op2, ast::full_type type) {
        if (is_unknown(op1.m_type) && is_unknown(op2.m_type)) {
            op1.set_first_type(std::move(type));
            op2.set_first_type(std::move(type));
        } else {
            auto temp = op1.m_type;
            auto copy = op2.m_type;
            op1.set_first_type(std::move(copy));
            op2.set_first_type(std::move(temp));
        }
    }

    void set_symbol_table_type(SymbolTable& symbol_table, const u32 idx, ast::full_type type) {
        if (idx >= symbol_table.m_types.size()) {
            symbol_table.m_types.resize(idx + 1);
        }
        if (is_unknown(symbol_table.m_types[idx])) {
            symbol_table.m_types[idx] = std::move(type);
        }
    }

    void Disassembler::process_instruction(
        const u32 istr_idx,
        function_disassembly& fn,
        const u64 offset,
        const game_type game,
        const SIDBase& sidbase,
        std::map<sid64, std::string>& sidcache,
        const location string_location
    ) {
        function_disassembly_line& line = fn.m_lines[istr_idx];
        StackFrame& frame = fn.m_stackFrame;

        constexpr u32 interpreted_buffer_size = 512;
        constexpr u32 disassembly_buffer_size = 256;

        char disassembly_text[disassembly_buffer_size] = {0};
        char interpreted[interpreted_buffer_size] = {0};
        const Instruction& istr = line.m_instruction;
        const u32 dest = istr.destination;
        const u32 op1 = istr.operand1;
        const u32 op2 = istr.operand2;
        ast::full_type table_entry;

        std::snprintf(
            disassembly_text,
            disassembly_buffer_size,
            "%04X   0x%06llX   %02X %02X %02X %02X   %-21s",
            line.m_location,
            static_cast<unsigned long long>(offset),
            static_cast<u32>(istr.opcode),
            dest,
            op1,
            op2,
            istr.opcode_to_string()
        );
        char* varying = disassembly_text + strlen(disassembly_text);
        const u32 disassembly_text_size = disassembly_buffer_size - strlen(disassembly_text);

        char dst_str[interpreted_buffer_size] = {0};
        char op1_str[interpreted_buffer_size] = {0};
        char op2_str[interpreted_buffer_size] = {0};

        auto load_nonstatic = [&](ast::primitive_kind kind, const char* type_format) -> void {
            frame[op1].m_type = ast::ptr_type{kind};
            std::snprintf(varying, disassembly_text_size, "r%d, [r%d]", dest, op1);
            frame[dest].m_type = make_type_from_prim(kind);
            frame[dest].m_value = Register::UNKNOWN_VAL;
            std::snprintf(interpreted, interpreted_buffer_size, type_format, dest, op1_str);
        };

        auto store_nonstatic = [&](ast::primitive_kind kind, const char* type_format) -> void {
            frame[dest].m_type = ast::ptr_type{kind};
            frame[dest].m_value = Register::UNKNOWN_VAL;
            std::snprintf(varying, disassembly_text_size, "r%d, [r%d], r%d", dest, op1, op2);
            frame[op1].m_type = make_type_from_prim(kind);
            std::snprintf(interpreted, interpreted_buffer_size, type_format, dest, op1_str, op2_str);
        };

        auto load_static_imm = [&]<typename T, ast::primitive_kind kind>(const char* type_format) -> void {
            std::snprintf(varying, disassembly_text_size, "r%d, %d", dest, op1);
            const T value = frame.m_symbolTable.get<T>(op1);
            const auto new_type = make_type_from_prim(kind);
            frame[dest].m_type = new_type;
            frame[dest].m_fromSymbolTable = op1;
            table_entry = new_type;
            if constexpr (std::is_same_v<f32, T>) {
                frame[dest].m_value = std::bit_cast<u32>(value);
            } else {
                frame[dest].m_value = value;
            }
            std::snprintf(interpreted, interpreted_buffer_size, type_format, dest, op1, value);
        };

        if (!istr.destination_is_immediate()) {
            frame.to_string(dst_str, interpreted_buffer_size, dest, sidbase.lookup(frame[dest].m_value, sidcache));
            if (!is_unknown(frame[dest].m_type) && frame[dest].m_containsArg) {
                if (const auto* ft_ptr = std::get_if<ast::function_type>(&frame[dest].m_type)) {
                    frame.m_registerArgs[frame[dest].m_argNum] = *ft_ptr->m_return;
                } else {
                    frame.m_registerArgs[frame[dest].m_argNum] = frame[dest].m_type;
                }
            }
            if (dest != op1 && dest != op2 && istr.operand1_is_used()) {
                frame[dest].m_containsArg = false;
                frame[dest].m_isReturn = false;
            }
        }
        if (!istr.operand1_is_immediate()) {
            frame.to_string(op1_str, interpreted_buffer_size, op1, sidbase.lookup(frame[op1].m_value, sidcache));
        }
        if (!istr.operand2_is_immediate()) {
            frame.to_string(op2_str, interpreted_buffer_size, op2, sidbase.lookup(frame[op2].m_value, sidcache));
        }


        const Opcode opcode = istr.opcode;


        switch (opcode) {
            using enum Opcode;
            case Return: {
                std::snprintf(varying, disassembly_text_size, "r%d", dest);
                std::snprintf(interpreted, interpreted_buffer_size, "Return %s", dst_str);
                for (const auto& reg : fn.m_stackFrame.m_registers) {
                    if (reg.m_containsArg && !is_unknown(reg.m_type)) {
                        fn.m_stackFrame.m_registerArgs[reg.m_argNum] = reg.m_type;
                    }
                }
                fn.m_stackFrame.m_returnType = frame[dest].m_type;
                break;
            }
            case IAdd: {
                std::snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", dest, op1, op2);
                if (frame[op1].is_pointer()) {
                    frame[dest].m_type = frame[op1].m_type;
                    frame[dest].m_value = frame[op1].m_value;
                    frame[dest].m_pointerOffset += frame[op2].m_pointerOffset;
                }
                frame.to_string(dst_str, interpreted_buffer_size, dest, sidbase.lookup(frame[dest].m_value, sidcache));
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = %s + %s", dest, op1_str, op2_str);
                break;
            }
            case ISub:
            case IMul:
            case IDiv:
            case FAdd:
            case FSub:
            case FMul:
            case FDiv: {
                char op = 0;
                if (opcode == FAdd) {
                    op = '+';
                } else if (opcode == ISub || opcode == FSub) {
                    op = '-';
                } else if (opcode == IMul || opcode == FMul) {
                    op = '*';
                } else if (opcode == IDiv || opcode == FDiv) {
                    op = '/';
                }
                std::snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", dest, op1, op2);
                frame.to_string(dst_str, interpreted_buffer_size, dest);
                std::snprintf(interpreted, interpreted_buffer_size, "%s = %s %c %s", dst_str, op1_str, op, op2_str);
                break;
            }
            case LoadStaticInt: {
                // const i64 table_value = frame.m_symbolTable.first.get<i64>(op1 * 8);
                const i32 table_value = frame.m_symbolTable.get<i32>(op1);
                table_entry = make_type_from_prim(ast::primitive_kind::I32);
                std::snprintf(varying, disassembly_text_size, "r%d, %d", dest, op1);
                frame[dest].m_value = table_value;
                frame.to_string(dst_str, interpreted_buffer_size, dest);
                std::snprintf(interpreted, interpreted_buffer_size, "%s = ST[%d] -> <%s>", dst_str, op1, op1_str);
                break;
            }
            case LoadStaticFloat: {
                const f32 table_value = frame.m_symbolTable.get<f32>(op1 * 8);
                table_entry = make_type_from_prim(ast::primitive_kind::F32);
                std::snprintf(varying, disassembly_text_size, "r%d, %d", dest, op1);
                frame[dest].m_value = std::bit_cast<u32>(table_value);
                frame.to_string(dst_str, interpreted_buffer_size, dest);
                std::snprintf(interpreted, interpreted_buffer_size, "%s = ST[%d] -> <%s>", dst_str, op1, op1_str);
                break;
            }
            case LoadStaticPointer: {
                const p64 table_value = frame.m_symbolTable.get<p64>(op1);
                table_entry = ast::ptr_type{};
                std::snprintf(varying, disassembly_text_size, "r%d, %d", dest, op1);
                frame[dest].m_type = ast::ptr_type();
                frame.to_string(dst_str, interpreted_buffer_size, dest);
                std::snprintf(interpreted, interpreted_buffer_size, "%s = ST[%d] -> <%s>", dst_str, op1, op1_str);
                break;
            }
            case LoadU16Imm: {
                const u16 value = op1 | (op2 << 8);
                std::snprintf(varying, disassembly_text_size, "r%d, %d", dest, value);
                frame[dest].m_type = make_type_from_prim(ast::primitive_kind::U16);
                frame[dest].m_value = value;
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = %d", dest, value);
                break;
            }
            case LoadU32: {
                load_nonstatic(ast::primitive_kind::U32, "r%d = *(u32*)%s");
                break;
            }
            case LoadFloat: {
                load_nonstatic(ast::primitive_kind::F32, "r%d = *(f32*)%s");
                break;
            }
            case LoadPointer: {
                frame[op1].set_first_type(ast::ptr_type{ast::ptr_type{}});
                std::snprintf(varying, disassembly_text_size, "r%d, [r%d]", dest, op1);
                frame[dest].m_type = ast::ptr_type{};
                frame[dest].m_value = frame[op1].m_value;
                frame[dest].m_pointerOffset = frame[op1].m_pointerOffset;
                frame.to_string(dst_str, interpreted_buffer_size, op1, sidbase.lookup(frame[op1].m_value, sidcache));
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = *(p64*)%s", dest, dst_str);
                break;
            }
            case LoadI64: {
                load_nonstatic(ast::primitive_kind::I64, "r%d = *(i64*)%s");
                break;
            }
            case LoadU64: {
                load_nonstatic(ast::primitive_kind::U64, "r%d = *(u64*)%s");
                break;
            }
            case StoreInt: {
                store_nonstatic(ast::primitive_kind::I32, "r%d, *(i32*)%s = %s");
                break;
            }
            case StoreFloat: {
                store_nonstatic(ast::primitive_kind::F32, "r%d, *(f32*)%s = %s");
                break;
            }
            case StorePointer: {
                frame[dest].set_first_type(ast::ptr_type{ast::ptr_type{}});
                std::snprintf(varying, disassembly_text_size, "r%d, [r%d], r%d", dest, op1, op2);
                std::snprintf(interpreted, interpreted_buffer_size, "r%d, *(p64*)%s = %s", dest, op1_str, op2_str);
                frame[op1].m_type = ast::ptr_type{};
                break;
            }
            case LookupInt: {
                std::snprintf(varying, disassembly_text_size, "r%d, %d", dest, op1);
                if (game != game_type::UC4) {
                    const i64 value = frame.m_symbolTable.get<i64>(op1);
                    frame[dest].m_type = make_type_from_prim(ast::primitive_kind::I64);
                    table_entry = make_type_from_prim(ast::primitive_kind::I64);
                    std::snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%s>", dest, op1, sidbase.lookup(static_cast<sid64>(value), sidcache));
                } else {
                    const i32 value = frame.m_symbolTable.get<i32>(op1);
                    frame[dest].m_type = make_type_from_prim(ast::primitive_kind::I32);
                    table_entry = make_type_from_prim(ast::primitive_kind::I32);
                    std::snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%s>", dest, op1, sidbase.lookup(static_cast<sid32>(value), sidcache));
                }
                break;
            }
            case LookupFloat: {
                std::snprintf(varying, disassembly_text_size, "r%d, %d", dest, op1);
                const f32 value = frame.m_symbolTable.get<f32>(op1);
                frame[dest].m_type = make_type_from_prim(ast::primitive_kind::F32);
                table_entry = make_type_from_prim(ast::primitive_kind::F32);
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%s>", dest, op1, sidbase.lookup(static_cast<sid64>(value), sidcache));
                break;
            }
            case LookupPointer: {
                std::snprintf(varying, disassembly_text_size, "r%d, %d", dest, op1);
                p64 value = 0;
                if (game != game_type::UC4) {
                    value = frame.m_symbolTable.get<p64>(op1);
                } else {
                    value = frame.m_symbolTable.get<u32>(op1);
                }
                frame[dest].m_value = value;
                frame[dest].m_fromSymbolTable = op1;
                const bool is_function = pointer_gets_called(dest, istr_idx + 1, fn);
                const ast::full_type existing_type = ast::function_type{};
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%s>", dest, op1, sidbase.lookup(value, sidcache));
                if (is_function) {
                    table_entry = existing_type;
                    frame[dest].m_type = existing_type;
                } else {
                    table_entry = ast::make_type_from_prim(ast::primitive_kind::SID);
                    frame[dest].m_type = ast::ptr_type{};
                    frame[dest].m_pointerOffset = 0;
                }
                break;
            }
            case MoveInt: {
                frame[op1].set_first_type(ast::primitive_kind::I64);
                std::snprintf(varying, disassembly_text_size, "r%d, %d", dest, op1);
                frame[dest].m_type = make_type_from_prim(ast::primitive_kind::I64);
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d <%lli>", dest, op1, frame[op1].m_value);
                break;
            }
            case MoveFloat: {
                frame[op1].set_first_type(ast::primitive_kind::F32);
                std::snprintf(varying, disassembly_text_size, "r%d, %d", dest, op1);
                frame[dest].m_type = make_type_from_prim(ast::primitive_kind::F32);
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d <%f>", dest, op1, static_cast<f32>(std::bit_cast<f64>(frame[op1].m_value)));
                break;
            }
            case MovePointer: {
                frame[op1].set_first_type(ast::ptr_type{});
                std::snprintf(varying, disassembly_text_size, "r%d, %d", dest, op1);
                frame[dest].m_type = ast::ptr_type{};
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d <%s>", dest, op1, sidbase.lookup(frame[op1].m_value, sidcache));
                break;
            }
            case CastInteger: {
                std::snprintf(varying, disassembly_text_size, "r%d", dest);
                frame[dest].m_type = make_type_from_prim(ast::primitive_kind::I32);
                frame[dest].m_value = frame[op1].m_value;
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = int(r%d)", dest, dest);
                break;
            }
            case CastFloat: {
                std::snprintf(varying, disassembly_text_size, "r%d", dest);
                frame[dest].m_type = make_type_from_prim(ast::primitive_kind::F32);
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = float(r%d)", dest, dest);
                break;
            }
            case Call:
            case CallFf: {
                std::snprintf(varying, disassembly_text_size, "r%d, r%d, %d", dest, op1, op2);
                char comment_str[300];
                const char* function_name = sidbase.lookup(frame[op1].m_value, sidcache);
                u8 offset = std::snprintf(comment_str, sizeof(comment_str), "r%d = %s(", dest, function_name);
                auto& ftype = frame.m_symbolTable.get_type(frame[op1].m_fromSymbolTable);
                if (!std::holds_alternative<ast::function_type>(ftype)) {
                    ftype = ast::function_type{};
                }
                ast::function_type &func_type = std::get<ast::function_type>(ftype);
                func_type.m_distanceType = opcode == Call ? ast::function_type::DISTANCE::NEAR : ast::function_type::DISTANCE::FAR;
                if (!func_type.m_arguments.empty() && func_type.m_arguments.size() != op2) {
                    func_type.m_isVariadic = true;
                    if (op2 < func_type.m_arguments.size()) {
                        func_type.m_arguments.resize(op2);
                    }
                }
                for (u64 i = 0; i < op2; ++i) {
                    if (i != 0) {
                        offset += std::snprintf(comment_str + offset, sizeof(comment_str) - offset, ", ");
                    }
                    const auto arg_type = std::make_shared<ast::full_type>(frame[ARGUMENT_REGISTERS_IDX + i].m_type);
                    if (i < func_type.m_arguments.size()) {
                        if (ast::is_unknown(*func_type.m_arguments[i].second)) {
                            *func_type.m_arguments[i].second = *arg_type;
                        }
                    } else if (!func_type.m_isVariadic) {
                        func_type.m_arguments.emplace_back(std::format("arg_{}", i), arg_type);
                    }
                    frame.to_string(dst_str, interpreted_buffer_size, ARGUMENT_REGISTERS_IDX + i, sidbase.lookup(frame[i + ARGUMENT_REGISTERS_IDX].m_value, sidcache));
                    offset += std::snprintf(comment_str + offset, sizeof(comment_str) - offset, "%s", dst_str);
                }
                if (auto* ft_ptr = std::get_if<ast::function_type>(&frame[dest].m_type)) {
                    frame[dest].m_type = *ft_ptr->m_return;
                }
                frame[dest].m_isReturn = true;
                frame[dest].m_pointerOffset = 0;
                std::snprintf(interpreted, interpreted_buffer_size, "%s)", comment_str);
                break;
            }
            case IEqual:
            case IGreaterThan:
            case IGreaterThanEqual:
            case ILessThan:
            case ILessThanEqual:
            case FEqual:
            case FGreaterThan:
            case FGreaterThanEqual:
            case FLessThan:
            case FLessThanEqual:
            case INotEqual:
            case FNotEqual: {
                if (opcode == IEqual || opcode == IGreaterThan || opcode == IGreaterThanEqual || opcode == ILessThan || opcode == ILessThanEqual || opcode == INotEqual) {
                    set_register_types(frame[op1], frame[op2], make_type_from_prim(ast::primitive_kind::I64));
                } else {
                    set_register_types(frame[op1], frame[op2], make_type_from_prim(ast::primitive_kind::F32));
                }
                std::snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", dest, op1, op2);
                if (op1 != dest) {
                    frame[dest].m_type = make_type_from_prim(ast::primitive_kind::BOOL);
                }
                const char* op = "";
                if (opcode == IEqual || opcode == FEqual) {
                    op = "==";
                } else if (opcode == IGreaterThan || opcode == FGreaterThan) {
                    op = ">";
                } else if (opcode == IGreaterThanEqual || opcode == FGreaterThanEqual) {
                    op = ">=";
                } else if (opcode == ILessThan || opcode == FLessThan) {
                    op = "<";
                } else if (opcode == ILessThanEqual || opcode == FLessThanEqual) {
                    op = "<=";
                } else if (opcode == INotEqual || opcode == FNotEqual) {
                    op = "!=";
                }
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d %s r%d", dest, op1, op, op2);
                break;
            }
            case IMod:
            case FMod: {
                auto type = opcode == IMod ? ast::primitive_kind::I64 : ast::primitive_kind::F32;
                set_register_types(frame[op1], frame[op2], make_type_from_prim(type));
                std::snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", dest, op1, op2);
                frame[dest].m_type = make_type_from_prim(type);
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d %% r%d", dest, op1, op2);
                break;
            }
            case IAbs:
            case FAbs: {
                auto type = opcode == IAbs ? ast::primitive_kind::I64 : ast::primitive_kind::F32;
                frame[op1].set_first_type(type);
                std::snprintf(varying, disassembly_text_size, "r%d, r%d", dest, op1);
                frame[dest].m_type = make_type_from_prim(type);
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = ABS(r%d)", dest, op1);
                break;
            }
            case Branch: {
                u32 target = dest | (op2 << 8);
                while (fn.m_lines[target].m_instruction.opcode == Branch) {
                    const u32 new_target = fn.m_lines[target].m_instruction.destination | (fn.m_lines[target].m_instruction.operand2 << 8);
                    if (new_target > target) {
                        target = new_target;
                    } else {
                        break;
                    }
                }
                std::snprintf(varying, disassembly_text_size, "0x%X", target);
                std::snprintf(interpreted, interpreted_buffer_size, "GOTO ");
                frame.add_target_label(target);
                line.m_target = target;
                if (target < line.m_location) {
                    frame.m_backwardsJumpLocs.push_back(line);
                }
                break;
            }
            case BranchIf:
            case BranchIfNot: {
                u32 target = dest | (op2 << 8);
                bool is_branching_target = true;
                while (is_branching_target) {
                    const auto& target_line = fn.m_lines[target];
                    const auto& target_op = target_line.m_instruction.opcode;
                    if (target_op == istr.opcode && target_line.m_instruction.operand1 == op1) {
                        target = target_line.m_instruction.destination | (target_line.m_instruction.operand2 << 8);
                    } else if (istr.opcode == BranchIf && target_op == BranchIfNot && target_line.m_instruction.operand1 == op1) {
                        target = target_line.m_location + 1;
                    } else if (istr.opcode == BranchIfNot && target_op == BranchIf && target_line.m_instruction.operand1 == op1) {
                        target = target_line.m_location + 1;
                    } else if (target_op == OpLogNot && target_line.m_instruction.operand1 == op1) {
                        const auto& flipped_target_opcode = fn.m_lines[target + 1].m_instruction.opcode;
                        if (istr.opcode == BranchIf && flipped_target_opcode == BranchIfNot) {
                            target = fn.m_lines[target + 1].m_instruction.destination | (fn.m_lines[target + 1].m_instruction.operand2 << 8);
                        } else if (istr.opcode == BranchIfNot && flipped_target_opcode == BranchIfNot || istr.opcode == BranchIf && flipped_target_opcode == BranchIf) {
                            target = target + 2;
                        } else {
                            is_branching_target = false;
                        }
                    } else {
                        is_branching_target = false;
                    }
                }
                std::snprintf(varying, disassembly_text_size, "r%d, 0x%X", op1, target);
                const char* comment = istr.opcode == BranchIf ? "IF r%d " : "IF NOT r%d ";
                std::snprintf(interpreted, interpreted_buffer_size, comment, op1);
                line.m_target = target;
                frame.add_target_label(target);
                if (target < line.m_location) {
                    frame.m_backwardsJumpLocs.push_back(line);
                }
                break;
            }
            case OpLogNot: {
                frame[op1].set_first_type(ast::primitive_kind::BOOL);
                std::snprintf(varying, disassembly_text_size, "r%d, r%d", dest, op1);
                frame[dest].m_type = make_type_from_prim(ast::primitive_kind::BOOL);
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = !%s", dest, op1_str);
                break;
            }
            case OpBitAnd: {
                frame[op1].set_first_type(ast::primitive_kind::I64);
                std::snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", dest, op1, op2);
                frame[dest].m_type = frame[op1].m_type;
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d & r%d", dest, op1, op2);
                break;
            }
            case OpBitNot: {
                frame[op1].set_first_type(ast::primitive_kind::I64);
                std::snprintf(varying, disassembly_text_size, "r%d, r%d", dest, op1);
                frame[dest].m_type = frame[op1].m_type;
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = ~r%d", dest, op1);
                break;
            }
            case OpBitOr: {
                frame[op1].set_first_type(ast::primitive_kind::I64);
                std::snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", dest, op1, op2);
                frame[dest].m_type = frame[op1].m_type;
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d | r%d", dest, op1, op2);
                break;
            }
            case OpBitXor: {
                frame[op1].set_first_type(ast::primitive_kind::I64);
                std::snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", dest, op1, op2);
                frame[dest].m_type = frame[op1].m_type;
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d ^ r%d", dest, op1, op2);
                break;
            }
            case OpBitNor: {
                frame[op1].set_first_type(ast::primitive_kind::I64);
                std::snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", dest, op1, op2);
                frame[dest].m_type = frame[op1].m_type;
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = ~(r%d | r%d) -> <%llu>", dest, op1, op2, frame[dest].m_value);
                break;
            }
            case OpLogAnd: {
                frame[op1].set_first_type(ast::primitive_kind::BOOL);
                std::snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", dest, op1, op2);
                frame[dest].m_type = make_type_from_prim(ast::primitive_kind::BOOL);
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d && r%d", dest, op1, op2);
                break;
            }
            case OpLogOr: {
                frame[op1].set_first_type(ast::primitive_kind::BOOL);
                std::snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", dest, op1, op2);
                frame[dest].m_type = make_type_from_prim(ast::primitive_kind::BOOL);
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d || r%d", dest, op1, op2);
                break;
            }
            case INeg: {
                frame[op1].set_first_type(ast::primitive_kind::I64);
                std::snprintf(varying, disassembly_text_size, "r%d, r%d", dest, op1);
                frame[dest].m_type = make_type_from_prim(ast::primitive_kind::I64);
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = -r%d", dest, op1);
                break;
            }
            case FNeg: {
                frame[op1].set_first_type(ast::primitive_kind::F32);
                std::snprintf(varying, disassembly_text_size, "r%d, r%d", dest, op1);
                frame[dest].m_type = make_type_from_prim(ast::primitive_kind::F32);
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = -r%d", dest, op1);
                break;
            }
            case LoadParamCnt: {
                break;
            }
            case IAddImm: {
                frame[op1].set_first_type(ast::primitive_kind::I64);
                std::snprintf(varying, disassembly_text_size, "r%d, r%d, %d", dest, op1, op2);
                const char* resolved = nullptr;
                if (frame[op1].is_pointer()) {
                    frame[dest].m_value = frame[op1].m_value;
                    frame[dest].m_pointerOffset = frame[op1].m_pointerOffset + op2;
                    resolved = sidbase.lookup(frame[dest].m_value, sidcache);
                }
                frame[dest].m_type = frame[op1].m_type;
                frame.to_string(dst_str, interpreted_buffer_size, op1, resolved);
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = %s + %d -> <%s>", dest, op1_str, op2, dst_str);
                break;
            }
            case ISubImm:
            case IMulImm:
            case IDivImm: {
                frame[op1].set_first_type(ast::primitive_kind::I64);
                std::snprintf(varying, disassembly_text_size, "r%d, r%d, %d", dest, op1, op2);
                frame[dest].m_type = make_type_from_prim(ast::primitive_kind::I64);
                frame[dest].m_value = frame[op1].m_value;
                const char* op = opcode == ISubImm ? "-" : (opcode == IMulImm ? "*" : "/");
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d %s %d", dest, op1, op, op2);
                break;
            }
            case LoadStaticI32Imm: {
                load_static_imm.operator()<i32, ast::primitive_kind::I32>("r%d = ST[%d] -> <%d>");
                break;
            }
            case LoadStaticFloatImm: {
                load_static_imm.operator()<f32, ast::primitive_kind::F32>("r%d = ST[%d] -> <%f>");
                break;
            }
            case LoadStaticPointerImm: {
                if (location{frame.m_symbolTable.get<const std::byte*>(op1)} >= string_location) [[likely]] {
                    load_static_imm.operator()<p64, ast::primitive_kind::STRING>("r%d = ST[%d] -> \"%s\"");
                } else [[unlikely]] {
                    std::snprintf(varying, disassembly_text_size, "r%d, %d", dest, op1);
                    table_entry = ast::ptr_type{};
                    frame[dest].m_fromSymbolTable = op1;
                    frame[dest].m_type = ast::ptr_type{};
                    frame[dest].m_value = frame.m_symbolTable.get<p64>(op1);
                    std::snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> PTR @ %llu", dest, op1, frame[dest].m_value);
                }
                break;
            }
            case LoadStaticI64Imm: {
                load_static_imm.operator()<i64, ast::primitive_kind::I64>("r%d = ST[%d] -> <%lli>");
                break;
            }
            case LoadStaticU64Imm: {
                std::snprintf(varying, disassembly_text_size, "r%d, %d", dest, op1);
                const u64 value = frame.m_symbolTable.get<u64>(op1);
                const char* hash_str = sidbase.lookup(value, sidcache);
                frame[dest].m_type = make_type_from_prim(ast::primitive_kind::SID);
                frame[dest].m_value = value;
                table_entry = make_type_from_prim(ast::primitive_kind::SID);
                frame.to_string(dst_str, interpreted_buffer_size, dest, hash_str);
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%s>", dest, op1, dst_str);
                break;
            }
            case IntAsh: {
                frame[op1].set_first_type(ast::primitive_kind::I64);
                frame[op2].set_first_type(ast::primitive_kind::I8);
                std::snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", dest, op1, op2);
                frame[dest].m_type = frame[op1].m_type;
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = %s <<>> %s", dest, op1_str, op2_str);
                break;
            }
            case Move: {
                std::snprintf(varying, disassembly_text_size, "r%d, r%d", dest, op1);
                frame[dest] = frame[op1];
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = %s", dest, op1_str);
                break;
            }
            case LoadStaticU32Imm: {
                if (game == game_type::UC4) {
                    std::snprintf(varying, disassembly_text_size, "r%d, %d", dest, op1);
                    const sid32 value = frame.m_symbolTable.get<sid32>(op1);
                    const char* hash_str = sidbase.lookup(value, sidcache);
                    frame[dest].m_type = make_type_from_prim(ast::primitive_kind::SID);
                    frame[dest].m_value = value;
                    table_entry = make_type_from_prim(ast::primitive_kind::SID);
                    frame.to_string(dst_str, interpreted_buffer_size, dest, hash_str);
                    std::snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%s>", dest, op1, dst_str);
                    break;
                } else {
                    load_static_imm.operator()<u32, ast::primitive_kind::U32>("r%d = ST[%d] -> <%u>");
                }
                break;
            }
            case LoadStaticI8Imm: {
                load_static_imm.operator()<i8, ast::primitive_kind::I8>("r%d = ST[%d] -> <%d>");
                break;
            }
            case LoadStaticI16Imm: {
                load_static_imm.operator()<i16, ast::primitive_kind::I16>("r%d = ST[%d] -> <%d>");
                break;
            }
            case LoadStaticU16Imm: {
                load_static_imm.operator()<u16, ast::primitive_kind::U16>("r%d = ST[%d] -> <%u>");
                break;
            }
            case LoadI8: {
                load_nonstatic(ast::primitive_kind::I8, "r%d = *(i8*)%s");
                break;
            }
            case LoadU8: {
                load_nonstatic(ast::primitive_kind::U8, "r%d = *(u8*)%s");
                break;
            }
            case LoadI16: {
                load_nonstatic(ast::primitive_kind::I16, "r%d = *(i16*)%s");
                break;
            }
            case LoadU16: {
                load_nonstatic(ast::primitive_kind::U16, "r%d = *(u16*)%s");
                break;
            }
            case LoadI32: {
                load_nonstatic(ast::primitive_kind::I32, "r%d = *(i32*)%s");
                break;
            }
            case StoreI8: {
                store_nonstatic(ast::primitive_kind::I8, "r%d, *(i8*)%s = %s");
                break;
            }
            case StoreU8: {
                store_nonstatic(ast::primitive_kind::U8, "r%d, *(u8*)%s = %s");
                break;
            }
            case StoreI16: {
                store_nonstatic(ast::primitive_kind::I16, "r%d, *(i16*)%s = %s");
                break;
            }
            case StoreU16: {
                store_nonstatic(ast::primitive_kind::U16, "r%d, *(u16*)%s = %s");
                break;
            }
            case StoreI32: {
                store_nonstatic(ast::primitive_kind::I32, "r%d, *(i32*)%s = %s");
                break;
            }
            case StoreU32: {
                store_nonstatic(ast::primitive_kind::U32, "r%d, *(u32*)%s = %s");
                break;
            }
            case StoreI64: {
                store_nonstatic(ast::primitive_kind::I64, "r%d, *(i64*)%s = %s");
                break;
            }
            case StoreU64: {
                store_nonstatic(ast::primitive_kind::U64, "r%d, *(u64*)%s = %s");
                break;
            }
            case AssertPointer: {
                frame[dest].set_first_type(ast::ptr_type{});
                std::snprintf(varying, disassembly_text_size, "r%d", dest);
                std::snprintf(interpreted, interpreted_buffer_size, "r%d != nullptr", dest);
                break;
            }
            case StoreArray: {
                std::snprintf(varying, disassembly_text_size, " ");
                std::snprintf(interpreted, interpreted_buffer_size, "DCC BREAKPOINT");
                break;
            }
            case QEX_InRangeI: {
                const i64 min = frame.m_symbolTable.get<i64>(op2);
                const i64 max = frame.m_symbolTable.get<i64>(op2 + 1);
                frame[op1].set_first_type(ast::primitive_kind::I64);
                frame[dest].m_type = make_type_from_prim(ast::primitive_kind::BOOL);
                frame[dest].m_value = Register::UNKNOWN_VAL;
                set_symbol_table_type(frame.m_symbolTable, op2, make_type_from_prim(ast::primitive_kind::I64));
                set_symbol_table_type(frame.m_symbolTable, op2 + 1, make_type_from_prim(ast::primitive_kind::I64));
                std::snprintf(varying, disassembly_text_size, "r%d, r%d, %d", dest, op1, op2);
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = in_range(%s, %lli, %lli)", dest, op1_str, min, max);
                break;
            }
            default: {
                std::snprintf(varying, disassembly_text_size, "???");
                std::snprintf(interpreted, interpreted_buffer_size, "UNKNOWN INSTRUCTION");
                break;
            }
        }

        if (!is_unknown(table_entry)) {
            set_symbol_table_type(frame.m_symbolTable, op1, std::move(table_entry));
        }
        line.m_text = std::string(disassembly_text);
        line.m_comment = std::string(interpreted);
    }

    void Disassembler::disassemble_functions_from_bin_file() {
        constexpr sid32 function_sid = 0xAB3EB31F;
        m_disassembledEntries.clear();
        m_functions.clear();
        m_offsetsToFunctionNames.clear();
        m_stateScript.reset();
        const location start = location(m_currentFile->m_bytes.get());
        for (u64 i = 0; i < m_currentFile->m_size; i += 4) {
            if (start.get<u32>(i) == function_sid) {
                std::vector<Instruction> istrs;
                const ShortInstruction* instr_ptr = start.get<const ShortInstruction*>(i - 16);
                const std::byte* symbol_table = start.get<const std::byte*>(i - 8);
                const ptrdiff_t func_size = (symbol_table - reinterpret_cast<const std::byte*>(instr_ptr)) / sizeof(ShortInstruction);

                istrs.reserve(func_size);
                for (u64 j = 0; j < func_size; ++j) {
                    istrs.emplace_back(instr_ptr->opcode, instr_ptr->destination, instr_ptr->operand1, instr_ptr->operand2);
                    instr_ptr++;
                }

                auto function_disassembly = create_function_disassembly(
                    istrs.data(),
                    func_size,
                    "0x" + std::to_string(i),
                    location(instr_ptr),
                    get_offset(instr_ptr) - func_size * sizeof(ShortInstruction),
                    sizeof(ShortInstruction),
                    m_game,
                    *m_sidbase,
                    m_currentFile->m_sidCache,
                    false,
                    m_currentFile->m_strings
                );
                m_functions.push_back(std::move(function_disassembly));
            }
        }
    }
}
