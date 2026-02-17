#include "ast/program.h"
#include "DCHeader.h"
#include "DCScript.h"

namespace dconstruct::ast {

void program::pseudo_c(std::ostream& os) const {
    for (const auto& decl : m_declarations) {
        os << *decl << "\n\n";
    }
}
void program::pseudo_py(std::ostream& os) const {
    for (const auto& decl : m_declarations) {
        os << *decl << '\n';
    }
}
void program::pseudo_racket(std::ostream& os) const {
    for (const auto& decl : m_declarations) {
        os << *decl << "\n\n";
    }
}

[[nodiscard]] std::vector<semantic_check_error> program::check_semantics(compilation::scope& env) const noexcept {
    std::vector<semantic_check_error> res;
    for (const auto& decl : m_declarations) {
        const std::vector<semantic_check_error> new_errors = decl->check_semantics(env);
        res.insert(res.end(), new_errors.begin(), new_errors.end());
    }
    if (std::holds_alternative<primitive_type>(*env.m_expectedReturnType) && std::get<primitive_type>(*env.m_expectedReturnType).m_type == primitive_kind::NOTHING && !env.m_computedReturnType) {
        return {semantic_check_error{"function expects a value to be returned"}};
    }
    return res;
}

template<typename T>
void program::insert_into_bytestream(std::unique_ptr<std::byte[]>& out, u64& size, const T& obj) noexcept {
    const std::byte* p = reinterpret_cast<const std::byte*>(std::addressof(obj));
    std::memcpy(out.get() + size, p, sizeof(T));
    size += sizeof(T);
}

void program::insert_into_bytestream(std::unique_ptr<std::byte[]>& out, u64& size, const std::vector<std::byte>& data) noexcept {
    std::memcpy(out.get() + size, data.data(), data.size());
    size += data.size();
}

void program::insert_into_reloctable(u8* out, u64& byte_offset, u64& bit_offset, const u8 bits, const u64 num_bits) noexcept {
    const u8 bit_space_remaining = (8 - bit_offset % 8);
    if (bit_space_remaining >= num_bits) {
        out[byte_offset] |= bits << bit_offset;
        bit_offset += num_bits;
        assert(bit_offset <= 8);
        if (bit_offset == 8) {
            bit_offset = 0;
            byte_offset++;
        }
    } else {
        out[byte_offset++] |= bits << bit_offset;
        out[byte_offset] |= bits >> bit_space_remaining;
        bit_offset = num_bits - bit_space_remaining;
    }
}

[[nodiscard]] std::expected<std::pair<std::unique_ptr<std::byte[]>, u64>, std::string> program::make_binary(std::vector<compilation::program_binary_element> program_element, const compilation::global_state& global) noexcept {
    constexpr sid64     script_lambda_sid    = SID("script-lambda");
    constexpr sid64     array_sid            = SID("array");
    constexpr sid64     global_sid           = SID("global");
    constexpr sid64     function_sid         = SID("function");
    constexpr u64       deadbeef             = 0xDEAD'BEEF'1337'F00D;
    constexpr u8        first_reloc_bits     = 0b1000'1000;
    
    constexpr u64 reloc_table_size_offset = 0x4;
    constexpr u64 reloctable_bit_offset   = 0x8;
    constexpr u8  text_size_offset        = 0xC;
    constexpr u64 first_entry_offset      = 0x28;


    constexpr u32 header_size = sizeof(DC_Header) + sizeof(array_sid);
    const u64 num_entries = program_element.size();
    const u32 entries_size = sizeof(Entry) * num_entries;

    const u64 entries_data_size = std::accumulate(program_element.begin(), program_element.end(), u64{0}, [](u64 acc, const compilation::program_binary_element& program_element) {
        return acc + program_element.get_size_in_bytes();
    });

    const u64 stringtable_size = std::accumulate(global.m_strings.begin(), global.m_strings.end(), u64{0}, [](u64 acc, const std::string& s) {
        return acc + s.size() + 1;
    });

    std::vector<char> stringtable;
    stringtable.reserve(stringtable_size);

    for (const auto& s : global.m_strings) {
        stringtable.insert(stringtable.end(), s.begin(), s.end());
        stringtable.push_back('\0');
    }

    const u32 data_size = header_size + entries_size + entries_data_size;
    const u32 stringtable_reloctable_padding = stringtable_size % 4 == 0 ? 0 : 4 - (stringtable_size % 4);
    const u32 relocatable_data_size = data_size + stringtable_size;
    const u32 reloc_table_size = static_cast<u32>(std::ceil(relocatable_data_size / 64.f));
    const u32 non_relocatable_size = stringtable_reloctable_padding + reloc_table_size;
    const u32 total_size = relocatable_data_size + reloc_table_size_offset + non_relocatable_size;

    std::unique_ptr<std::byte[]> out = std::make_unique<std::byte[]>(total_size);
    u64 current_size = 0;
    const u32 reloc_table_start = relocatable_data_size + stringtable_reloctable_padding + reloc_table_size_offset;
    u8* reloc_table_ptr = reinterpret_cast<u8*>(out.get() + reloc_table_start);
    u64 byte_offset = 0, bit_offset = 0;

    auto push_bytes = [&out, &current_size, &reloc_table_ptr, &byte_offset, &bit_offset](auto&& arg, auto&& ...bits) -> void {
        insert_into_bytestream(out, current_size, arg);
        const std::vector<u8> bits_list = {static_cast<u8>(bits)...};
        for (u32 i = 0; i < bits_list.size() - 1; ++i) {
            insert_into_reloctable(reloc_table_ptr, byte_offset, bit_offset, bits_list[i], 8);
        }
        insert_into_reloctable(reloc_table_ptr, byte_offset, bit_offset, bits_list.back(), sizeof(arg) % 8);
    };

    auto get_string_offset = [data_size, &global](const u32 index) -> u64 {
        u64 res = data_size;
        for (u32 i = 0; i < index; ++i) {
            res += global.m_strings[i].size() + 1;
        }
        return res;
    };
    DC_Header header {
        DC_MAGIC,
        DC_VERSION,
        relocatable_data_size + stringtable_reloctable_padding,
        data_size,
        0x1,
        static_cast<u32>(num_entries),
        reinterpret_cast<Entry*>(first_entry_offset)
    };
    push_bytes(header, 0b1000);
    push_bytes(array_sid, 0b0);
    const u64 first_function_start = header_size + num_entries * sizeof(Entry);
    u64 prev_entry_size = sizeof(function_sid);

    for (const auto& element : program_element) {
        push_bytes(element.m_entry, 0b100);
        prev_entry_size += element.get_size_in_bytes();
    }

    
    u64 string_index = 0;
    for (auto& fn : program_element) {
        for (const auto& [offset, str_index] : fn.m_stringOffsets) {
            const u64 relative_offset = get_string_offset(str_index);
            *reinterpret_cast<u64*>(&fn.m_rawData[offset]) = relative_offset;
        }
        insert_into_bytestream(out, current_size, fn.m_rawData);
        for (const auto& bits : fn.m_relocTable) {
            insert_into_reloctable(reloc_table_ptr, byte_offset, bit_offset, bits, sizeof(bits));
        }
    }


    std::memcpy(out.get() + current_size, stringtable.data(), stringtable.size());
    current_size += stringtable.size();
    std::memset(out.get() + current_size, 0, stringtable_reloctable_padding);
    current_size += stringtable_reloctable_padding;
    std::memcpy(out.get() + current_size, &reloc_table_size, sizeof(reloc_table_size));
    current_size += sizeof(reloc_table_size);
    current_size += reloc_table_size;
    assert(current_size == total_size);
    std::expected<std::pair<std::unique_ptr<std::byte[]>, u64>, std::string> result;
    result.emplace(std::move(out), total_size);
    return result;
}

[[nodiscard]] std::expected<std::pair<std::unique_ptr<std::byte[]>, u64>, std::string> program::compile_to_file(const compilation::scope& scope) const noexcept {
    compilation::global_state global{};
    auto functions = compile_binary_elements(scope, global);
    if (!functions) {
        return std::unexpected{functions.error()};
    }
    return make_binary(std::move(*functions), global);
}

[[nodiscard]] std::expected<std::pair<std::unique_ptr<std::byte[]>, u64>, std::string> program::compile_to_file(const compilation::scope& scope, compilation::global_state& global) const noexcept {
    auto functions = compile_binary_elements(scope, global);
    if (!functions) {
        return std::unexpected{functions.error()};
    }
    return make_binary(std::move(*functions), global);
}

[[nodiscard]] std::expected<std::vector<compilation::program_binary_element>, std::string> program::compile_binary_elements(const compilation::scope& scope, compilation::global_state& global) const noexcept {     
    for (const auto& [name, sid_literal] : scope.m_sidAliases) {
        const full_type* type = scope.lookup(name);
        global.m_sidAliases.emplace(name, std::pair{*type, sid_literal.first});
    }

    std::vector<compilation::program_binary_element> functions;
    functions.reserve(m_declarations.size());
    for (u32 i = 0; i < m_declarations.size(); ++i) {
        program_binary_result res = m_declarations[i]->emit_dc(global);

        if (!res) {
            return std::unexpected{res.error()};
        }
        functions.push_back(std::move(*res));
    }

    return functions;
}

[[nodiscard]] std::expected<std::vector<compilation::program_binary_element>, std::string> program::compile_binary_elements(const compilation::scope& scope) const noexcept {
    compilation::global_state global{};
    return compile_binary_elements(scope, global);
}

} 