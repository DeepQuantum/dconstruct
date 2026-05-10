#include "ast/state_script/state_script.h"
#include "ast/primary_expressions/literal.h"
#include "compilation/function.h"
#include <array>
#include <numeric>
#include <unordered_set>

namespace dconstruct::ast {

[[nodiscard]] bool operator==(const state_script& lhs, const state_script& rhs) noexcept {
    return lhs.equals(rhs);
}

void state_script::pseudo_c(std::ostream& os) const {
    os << "statescript {\n";
    os << indent_more;
    os << indent << "options {\n";
    os << indent_more;
    for (const auto& opt : m_options) {
        os << indent << opt << "\n";
    }
    os << indent_less;
    os << indent << "}\n";
    os << indent << "declarations {\n";
    os << indent_more;
    for (const auto& decl : m_declarations) {
        os << indent << decl.m_identifier << "\n";
    }
    os << indent_less;
    os << indent << "}\n";
    for (const auto& s : m_states) {
        os << indent << s << "\n";
    }
    os << indent_less;
    os << indent << "}";
}

void state_script::pseudo_py(std::ostream& os) const {
    os << "statescript:\n  options: ";
    for (const auto& opt : m_options) {
        os << opt << " ";
    }
    os << "\n  declarations: ";
    for (const auto& decl : m_declarations) {
        os << decl.m_identifier << " ";
    }
    os << "\n  states: ";
    for (const auto& s : m_states) {
        os << s << " ";
    }
}

void state_script::pseudo_racket(std::ostream& os) const {
    os << "(statescript (options";
    for (const auto& opt : m_options) {
        os << " " << opt;
    }
    os << ") (declarations";
    for (const auto& decl : m_declarations) {
        os << " " << decl.m_identifier;
    }
    os << ") (states";
    for (const auto& s : m_states) {
        os << " " << s;
    }
    os << "))";
}

[[nodiscard]] std::vector<semantic_check_error> state_script::check_semantics(compilation::scope& scope) const noexcept {
    std::vector<semantic_check_error> errors;

    compilation::scope decl_scope(&scope);
    for (const auto& decl : m_declarations) {
        std::vector<semantic_check_error> decl_errors = decl.check_semantics(decl_scope);
        errors.insert(errors.end(), decl_errors.begin(), decl_errors.end());

        if (!std::holds_alternative<primitive_type>(decl.m_type)) {
            errors.push_back(semantic_check_error{"state script declaration '" + decl.m_identifier + "' must have a primitive type"});
        }
    }

    std::unordered_set<std::string> state_names;
    for (const auto& state : m_states) {
        if (!state_names.insert(state.m_name).second) {
            errors.push_back(semantic_check_error{"duplicate state name: '" + state.m_name + "'"});
        }

        decl_scope.define("#" + state.m_name, make_type_from_prim(ast::primitive_kind::U64));
        decl_scope.m_sidAliases[state.m_name] = {SID(state.m_name.c_str()), state.m_name};

        std::unordered_set<std::string> block_names;
        for (const auto& block : state.m_blocks) {
            if (!block_names.insert(block.block_type_to_string()).second) {
                errors.push_back(semantic_check_error{"duplicate block name: '" + block.block_type_to_string() + "' in state '" + state.m_name + "'"});
            }

            std::unordered_set<std::string> track_names;
            for (const auto& track : block.m_tracks) {
                if (!track_names.insert(track.m_name).second) {
                    errors.push_back(semantic_check_error{"duplicate track name: '" + track.m_name + "' in block '" + block.block_type_to_string() + "'"});
                }
            }
        }
    }


    for (const auto& state : m_states) {
        for (const auto& block : state.m_blocks) {
            for (const auto& track : block.m_tracks) {
                for (size_t i = 0; i < track.m_lambdas.size(); ++i) {
                    const auto& lambda = track.m_lambdas[i];
                    std::vector<semantic_check_error> lambda_errors = lambda.check_semantics(decl_scope);
                    const std::string path = "in state '" + state.m_name + "' block '" + block.block_type_to_string() + "' track '" + track.m_name + "' lambda " + std::to_string(i);
                    for (auto& err : lambda_errors) {
                        err.m_message = path + ": " + err.m_message;
                    }
                    errors.insert(errors.end(), lambda_errors.begin(), lambda_errors.end());
                }
            }
        }
    }

    return errors;
}

[[nodiscard]] bool state_script::equals(const state_script& rhs) const noexcept {
    if (m_options != rhs.m_options || m_declarations.size() != rhs.m_declarations.size() ||
        m_states.size() != rhs.m_states.size()) {
        return false;
    }
    for (size_t i = 0; i < m_declarations.size(); ++i) {
        if (!m_declarations[i].equals(rhs.m_declarations[i])) {
            return false;
        }
    }
    for (size_t i = 0; i < m_states.size(); ++i) {
        if (!m_states[i].equals(rhs.m_states[i])) {
            return false;
        }
    }
    return true;
}


[[nodiscard]] program_binary_result state_script::emit_dc(compilation::global_state& global) const noexcept {
    constexpr sid64 state_script_sid = SID("state-script");
    constexpr sid64 array_sid = SID("array");
    constexpr sid64 ss_options_sid = SID("ss-options");
    constexpr sid64 ss_decl_list_sid = SID("ss-decl-list");
    constexpr sid64 symbol_array_sid = SID("symbol-array");
    constexpr sid64 script_lambda_sid = SID("script-lambda");

    const sid64 script_name = SID(m_name.c_str());

    const u64 total_declarations_size = std::accumulate(m_declarations.begin(), m_declarations.end(), 0ull, [](u64 sum, const variable_declaration& decl) {
        return sum + get_size(decl.m_type);
    });

    const auto declaration_type_sid = [](const primitive_type& ptype) noexcept -> sid64 {
        switch (ptype.m_type) {
            case primitive_kind::BOOL:   return SID("boolean");
            case primitive_kind::I32:    return SID("int32");
            case primitive_kind::F32:    return SID("float");
            case primitive_kind::STRING: return SID("string");
            case primitive_kind::SID:    return SID("symbol");
            default: assert(false && "unsupported declaration type"); return SID("int32");
        }
    };

    compilation::program_binary_element element{0};

    const auto patch_u64 = [&element](const u64 offset, const u64 value) noexcept {
        assert(offset + sizeof(u64) <= element.m_rawData.size());
        std::memcpy(element.m_rawData.data() + offset, &value, sizeof(value));
    };

    const auto append_lambda_element = [&element](compilation::program_binary_element&& fn_element) noexcept {
        const u64 base_offset = element.m_rawData.size();
        std::unordered_set<u64> string_offset_chunks;
        string_offset_chunks.reserve(fn_element.m_stringOffsets.size());

        for (const u64 string_offset : fn_element.m_stringOffsets) {
            string_offset_chunks.insert(string_offset / sizeof(u64));
            element.m_stringOffsets.push_back(base_offset + string_offset);
        }

        const u64 chunks = fn_element.m_rawData.size() / sizeof(u64);
        for (u64 i = 0; i < chunks; ++i) {
            if (fn_element.m_relocTable[i] && !string_offset_chunks.contains(i)) {
                u64* ptr = reinterpret_cast<u64*>(fn_element.m_rawData.data() + i * sizeof(u64));
                if (*ptr != 0) {
                    *ptr += base_offset;
                }
            }
        }

        element.m_rawData.insert(element.m_rawData.end(), fn_element.m_rawData.begin(), fn_element.m_rawData.end());
        element.m_relocTable.insert(element.m_relocTable.end(), fn_element.m_relocTable.begin(), fn_element.m_relocTable.end());
    };

    element.m_entry = Entry {
        script_name,
        state_script_sid,
        nullptr
    };

    element.push_bytes(state_script_sid, 0b0);

    const u64 debug_str_idx = global.add_string("debug_file_name_placeholder");
    const u64 state_script_offset = element.m_rawData.size();
    element.insert_string_offset(offsetof(StateScript, m_pDebugFileName));
    const StateScript ss = {
        script_name,
        nullptr,
        SID(m_states[0].m_name.c_str()),
        nullptr,
        0x0,
        nullptr,
        static_cast<i16>(m_states.size()),
        0x0,
        0x0,
        reinterpret_cast<const char*>(debug_str_idx),
        nullptr,
        0x0,
    };
    element.push_bytes(ss, 0b1010'1010, 0b01);

    if (!m_options.empty()) {
        element.push_bytes(ss_options_sid, 0b0);
        const u64 options_offset = element.m_rawData.size();
        patch_u64(state_script_offset + offsetof(StateScript, m_pSsOptions), options_offset);

        u64 options_string_idx = 0;
        if (!m_optionsString.empty()) {
            options_string_idx = global.add_string(m_optionsString);
            element.insert_string_offset();
        }

        const SsOptions ss_options = {
            reinterpret_cast<const char*>(options_string_idx),
            0x0,
            0x0,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            5,
            0,
            0x0,
            0x0,
        };
        element.push_bytes(ss_options, 0b0111'1001, 0b00);

        element.push_bytes(symbol_array_sid, 0b0);
        const u64 symbol_array_offset = element.m_rawData.size();
        patch_u64(options_offset + offsetof(SsOptions, m_pSymbolArray), symbol_array_offset);

        const SymbolArray symbol_array = {
            static_cast<u32>(m_options.size()),
            0,
            nullptr,
        };
        element.push_bytes(symbol_array, 0b10);

        element.push_bytes(array_sid, 0b0);

        const u64 symbol_data_offset = element.m_rawData.size();
        patch_u64(symbol_array_offset + offsetof(SymbolArray, m_pSymbols), symbol_data_offset);
        
        for (const auto& opt : m_options) {
            const std::string opt_str = opt.get_raw_string();
            const u64 opt_sid = SID(opt_str.c_str());
            element.push_bytes(opt_sid, 0b0);
        }
    }

    if (!m_declarations.empty()) {
        element.push_bytes(ss_decl_list_sid, 0b0);
        const u64 declaration_list_offset = element.m_rawData.size();
        patch_u64(state_script_offset + offsetof(StateScript, m_pSsDeclList), declaration_list_offset);

        const SsDeclarationList decl_list = {
            static_cast<u32>(total_declarations_size),
            static_cast<u32>(m_declarations.size()),
            nullptr,
        };
        element.push_bytes(decl_list, 0b10);

        element.push_bytes(array_sid, 0b0);
        const u64 declaration_data_offset = element.m_rawData.size();
        patch_u64(declaration_list_offset + offsetof(SsDeclarationList, m_pDeclarations), declaration_data_offset);

        u16 var_size_sum = 0;
        std::vector<u64> declaration_value_ptr_offsets;
        declaration_value_ptr_offsets.reserve(m_declarations.size());
        for (const auto& decl : m_declarations) {
            const auto& ptype = std::get<primitive_type>(decl.m_type);
            var_size_sum += static_cast<u16>(get_size(decl.m_type));
            const SsDeclaration ss_decl = {
                SID(decl.m_identifier.c_str()),
                nullptr,
                declaration_type_sid(ptype),
                var_size_sum,
                1,
                0,
                nullptr,
                0x80,
            };
            declaration_value_ptr_offsets.push_back(element.m_rawData.size() + offsetof(SsDeclaration, m_pDeclValue));
            element.push_bytes(ss_decl, 0b01'0010);
        }

        for (u32 i = 0; i < m_declarations.size(); ++i) {
            const auto& decl = m_declarations[i];
            const auto& ptype = std::get<primitive_type>(decl.m_type);
            element.push_bytes(declaration_type_sid(ptype), 0b0);

            const u64 declaration_value_offset = element.m_rawData.size();
            patch_u64(declaration_value_ptr_offsets[i], declaration_value_offset);

            u64 init_value = 0;
            if (decl.m_init) {
                const literal* lit = decl.m_init->as_literal();
                assert(lit && "declaration initializer must be a literal");
                std::visit([&](auto&& val) {
                    using T = std::decay_t<decltype(val)>;
                    if constexpr (std::is_arithmetic_v<T>) {
                        std::memcpy(&init_value, &val, sizeof(val));
                    }
                }, lit->m_value);
            }
            element.push_bytes(init_value, 0b0);
        }
    }

    element.push_bytes(array_sid, 0b0);
    const u64 state_offset = element.m_rawData.size();
    patch_u64(state_script_offset + offsetof(StateScript, m_pSsStateTable), state_offset);

    std::vector<u64> state_block_ptr_offsets;
    state_block_ptr_offsets.reserve(m_states.size());
    for (const state_script_state& state : m_states) {
        const SsState ss_state = {
            SID(state.m_name.c_str()),
            static_cast<i64>(state.m_blocks.size()),
            nullptr
        };
        state_block_ptr_offsets.push_back(element.m_rawData.size() + offsetof(SsState, m_pSsOnBlocks));
        element.push_bytes(ss_state, 0b100);
    }

    element.push_bytes(array_sid, 0b0);

    std::vector<u64> block_track_ptr_offsets;
    for (u32 i = 0; i < m_states.size(); ++i) {
        const auto& state = m_states[i];
        if (!state.m_blocks.empty()) {
            patch_u64(state_block_ptr_offsets[i], element.m_rawData.size());
        }

        for (const auto& block : state.m_blocks) {
            u16 block_lambda_count = 0;
            for (const auto& t : block.m_tracks) {
                block_lambda_count += static_cast<u16>(t.m_lambdas.size());
            }

            const u64 tg_name_idx = global.add_string(m_name + " " + state.m_name + " (on (" + block.block_type_to_string() + "))");
            element.insert_string_offset(offsetof(SsOnBlock, m_trackGroup.m_name));

            const SsOnBlock on_block = {
                block.m_blockType,
                0,
                block.m_blockType == BLOCK_TYPE::EVENT ? SID(block.m_eventName->c_str()) : 0,
                nullptr,
                {
                    0x0,
                    block_lambda_count,
                    static_cast<i16>(block.m_tracks.size()),
                    0,
                    nullptr,
                    reinterpret_cast<const char*>(tg_name_idx),
                    0x0,
                    0x0,
                    nullptr,
                }
            };
            block_track_ptr_offsets.push_back(element.m_rawData.size() + offsetof(SsOnBlock, m_trackGroup.m_aTracks));
            element.push_bytes(on_block, 0b0110'0100, 0b10);
        }
    }

    u32 block_flat_idx = 0;
    std::vector<u64> track_lambda_ptr_offsets;
    for (const auto& state : m_states) {
        for (const auto& block : state.m_blocks) {
            element.push_bytes(array_sid, 0b0);
            if (!block.m_tracks.empty()) {
                patch_u64(block_track_ptr_offsets[block_flat_idx], element.m_rawData.size());
            }
            block_flat_idx++;

            for (u32 ti = 0; ti < block.m_tracks.size(); ++ti) {
                const auto& track = block.m_tracks[ti];
                const SsTrack ss_track = {
                    SID(track.m_name.c_str()),
                    static_cast<u16>(ti),
                    static_cast<i16>(track.m_lambdas.size()),
                    0,
                    nullptr,
                };
                track_lambda_ptr_offsets.push_back(element.m_rawData.size() + offsetof(SsTrack, m_pSsLambda));
                element.push_bytes(ss_track, 0b100);
            }
        }
    }

    u32 track_flat_idx = 0;
    u64 lambda_counter = 0;
    std::vector<u64> ss_lambda_script_ptr_offsets;
    for (const auto& state : m_states) {
        for (const auto& block : state.m_blocks) {
            for (const auto& track : block.m_tracks) {
                element.push_bytes(array_sid, 0b0);
                if (!track.m_lambdas.empty()) {
                    patch_u64(track_lambda_ptr_offsets[track_flat_idx], element.m_rawData.size());
                }
                track_flat_idx++;

                for (u32 li = 0; li < track.m_lambdas.size(); ++li) {
                    const SsLambda ss_lam = {
                        nullptr,
                        lambda_counter++,
                    };
                    ss_lambda_script_ptr_offsets.push_back(element.m_rawData.size() + offsetof(SsLambda, m_pScriptLambda));
                    element.push_bytes(ss_lam, 0b01);
                }
            }
        }
    }

    u32 lambda_flat_idx = 0;
    for (const auto& state : m_states) {
        for (const auto& block : state.m_blocks) {
            for (const auto& track : block.m_tracks) {
                for (u32 lambda_idx = 0; lambda_idx < track.m_lambdas.size(); ++lambda_idx) {
                    const auto& lambda = track.m_lambdas[lambda_idx];
                    program_binary_result lambda_element_res = lambda.emit_dc(global);
                    if (!lambda_element_res) {
                        return std::unexpected{"failed to emit lambda in state '" + state.m_name + "' block '" + block.block_type_to_string() + "' track '" + track.m_name + "' lambda " + std::to_string(lambda_idx) + ": " + lambda_element_res.error()};
                    }

                    compilation::program_binary_element fn_element = std::move(*lambda_element_res);
                    assert(fn_element.m_rawData.size() % 8 == 0);

                    patch_u64(ss_lambda_script_ptr_offsets[lambda_flat_idx++], element.m_rawData.size() + sizeof(script_lambda_sid));
                    append_lambda_element(std::move(fn_element));
                }
            }
        }
    }

    return element;

}

}
