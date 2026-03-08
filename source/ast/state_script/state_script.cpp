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

        std::unordered_set<std::string> block_names;
        for (const auto& block : state.m_blocks) {
            if (!block_names.insert(block.m_name).second) {
                errors.push_back(semantic_check_error{"duplicate block name: '" + block.m_name + "' in state '" + state.m_name + "'"});
            }

            std::unordered_set<std::string> track_names;
            for (const auto& track : block.m_tracks) {
                if (!track_names.insert(track.m_name).second) {
                    errors.push_back(semantic_check_error{"duplicate track name: '" + track.m_name + "' in block '" + block.m_name + "'"});
                }
            }
        }
    }

    for (const auto& state : m_states) {
        for (const auto& block : state.m_blocks) {
            for (const auto& track : block.m_tracks) {
                for (size_t i = 0; i < track.m_lambdas.size(); ++i) {
                    const auto& lambda = track.m_lambdas[i];
                    std::vector<semantic_check_error> lambda_errors = lambda.m_body.check_semantics(decl_scope);
                    const std::string path = "in state '" + state.m_name + "' block '" + block.m_name + "' track '" + track.m_name + "' lambda " + std::to_string(i);
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
    
    u64 current_offset = sizeof(state_script_sid);
    current_offset += sizeof(StateScript);

    u64 options_offset = 0;
    u64 symbol_array_offset = 0;
    u64 symbol_data_offset = 0;
    if (!m_options.empty()) {
        current_offset += sizeof(ss_options_sid);
        options_offset = current_offset;
        current_offset += sizeof(SsOptions);
        current_offset += sizeof(symbol_array_sid);
        symbol_array_offset = current_offset;
        current_offset += sizeof(SymbolArray);
        symbol_data_offset = current_offset;
        current_offset += sizeof(sid64) * m_options.size();
    }

    u64 declaration_list_offset = 0;
    u64 declaration_data_offset = 0;
    std::vector<u64> declaration_data_offsets;

    if (!m_declarations.empty()) {
        current_offset += sizeof(ss_decl_list_sid);
        declaration_list_offset = current_offset;
        current_offset += sizeof(SsDeclarationList);
        current_offset += sizeof(array_sid);
        declaration_data_offset = current_offset;
        current_offset += sizeof(SsDeclaration) * m_declarations.size();
        for (const auto& decl : m_declarations) {
            if (decl.m_init == nullptr) {
                continue;
            }
            current_offset += sizeof(sid64);
            declaration_data_offsets.push_back(current_offset);
            current_offset += sizeof(void*);
        }
    }

    current_offset += sizeof(array_sid);
    const u64 state_offset = current_offset;
    current_offset += sizeof(SsState) * m_states.size();

    current_offset += sizeof(array_sid);
    const u64 block_data_offset = current_offset;

    std::vector<u64> block_offsets;
    for (const auto& state : m_states) {
        block_offsets.push_back(current_offset);
        current_offset += sizeof(SsOnBlock);
    }


    std::vector<u64> track_offsets;
    for (const auto& state : m_states) {
        for (const auto& block : state.m_blocks) {
            current_offset += sizeof(array_sid);
            for (const auto& track : block.m_tracks) {
                track_offsets.push_back(current_offset);
                current_offset += sizeof(SsTrack);           
            }
        }
    }

    std::vector<u64> ss_lambda_offsets;
    for (const auto& state : m_states) {
        for (const auto& block : state.m_blocks) {
            for (const auto& track : block.m_tracks) {
                current_offset += sizeof(array_sid);
                for (const auto& lambda : track.m_lambdas) {
                    ss_lambda_offsets.push_back(current_offset);
                    current_offset += sizeof(SsLambda);
                }
            }
        }
    }

    std::vector<u64> script_lambda_offsets;
    for (const auto& state : m_states) {
        for (const auto& block : state.m_blocks) {
            for (const auto& track : block.m_tracks) {
                for (const auto& lambda : track.m_lambdas) {
                    current_offset += sizeof(script_lambda_sid);
                    script_lambda_offsets.push_back(current_offset);
                    current_offset += sizeof(ScriptLambda);
                }
            }
        }
    }

    std::vector<compilation::program_binary_element> lambda_elements;
    for (const auto& state : m_states) {
        for (const auto& block : state.m_blocks) {
            for (const auto& track : block.m_tracks) {
                current_offset += sizeof(array_sid);
                for (u32 lambda_idx = 0; lambda_idx < track.m_lambdas.size(); ++lambda_idx) {
                    const auto& lambda = track.m_lambdas[lambda_idx];
                    compilation::function fn;
                    const emission_err error = lambda.m_body.emit_dc(fn, global);
                    if (error) {
                        return std::unexpected{"failed to emit lambda in state '" + state.m_name + "' block '" + block.m_name + "' track '" + track.m_name + "' lambda " + std::to_string(lambda_idx) + ": " + *error};
                    }
                    compilation::program_binary_element fn_element = fn.to_binary_element();
                    fn_element.adjust_offsets(current_offset - sizeof(ScriptLambda) - 2 * sizeof(sid64));

                    assert(fn_element.m_rawData.size() % 8 == 0);

                    current_offset += fn_element.m_rawData.size();
                    lambda_elements.push_back(std::move(fn_element));
                }
            }
        }
    }

    const u64 total_declarations_size = std::accumulate(m_declarations.begin(), m_declarations.end(), 0ull, [](u64 sum, const variable_declaration& decl) {
        return sum + get_size(decl.m_type);
    });

    compilation::program_binary_element element{current_offset};

    element.m_entry = Entry {
        script_name,
        state_script_sid,
        nullptr
    };

    element.push_bytes(state_script_sid, 0b0);

    const u64 debug_str_idx = global.add_string("debug_file_name_placeholder");
    element.insert_string_offset(offsetof(StateScript, m_pDebugFileName));
    const StateScript ss = {
        script_name,
        !m_declarations.empty() ? reinterpret_cast<SsDeclarationList*>(declaration_list_offset) : nullptr,
        m_initialStateIdx,
        reinterpret_cast<SsOptions*>(options_offset),
        0x0,
        reinterpret_cast<SsState*>(state_offset),
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
        u64 options_string_idx = 0;
        if (!m_optionsString.empty()) {
            options_string_idx = global.add_string(m_optionsString);
            element.insert_string_offset();
        }

        const SsOptions ss_options = {
            reinterpret_cast<const char*>(options_string_idx),
            0x0,
            0x0,
            reinterpret_cast<SymbolArray*>(symbol_array_offset),
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
    
        const SymbolArray symbol_array = {
            static_cast<u32>(m_options.size()),
            0,
            reinterpret_cast<sid64*>(symbol_data_offset),
        };
    
        element.push_bytes(symbol_array, 0b10);

        for (const auto& opt : m_options) {
            const std::string opt_str = opt.get_raw_string();
            const u64 opt_sid = SID(opt_str.c_str());
            element.push_bytes(opt_sid, 0b0);
        }
    }


    if (!m_declarations.empty()) {
        element.push_bytes(ss_decl_list_sid, 0b0);

        const SsDeclarationList decl_list = {
            static_cast<u32>(total_declarations_size),
            static_cast<u32>(m_declarations.size()),
            reinterpret_cast<SsDeclaration*>(declaration_data_offset),
        };

        element.push_bytes(decl_list, 0b10);

        element.push_bytes(array_sid, 0b0);

        u16 var_size_sum = 0;
        for (u32 i = 0; i < m_declarations.size(); ++i) {
            const auto& decl = m_declarations[i];
            const auto& ptype = std::get<primitive_type>(decl.m_type);
            sid64 decl_type_sid = 0;
            switch (ptype.m_type) {
                case primitive_kind::BOOL:   decl_type_sid = SID("boolean"); break;
                case primitive_kind::I32:    decl_type_sid = SID("int32");   break;
                case primitive_kind::F32:    decl_type_sid = SID("float");   break;
                case primitive_kind::STRING: decl_type_sid = SID("string");  break;
                case primitive_kind::SID:    decl_type_sid = SID("symbol");  break;
                default: assert(false && "unsupported declaration type"); break;
            }

            var_size_sum += static_cast<u16>(get_size(decl.m_type));
            const SsDeclaration ss_decl = {
                SID(decl.m_identifier.c_str()),
                nullptr,
                decl_type_sid,
                var_size_sum,
                1,
                0,
                reinterpret_cast<void*>(declaration_data_offsets[i]),
                0x80,
            };
            element.push_bytes(ss_decl, 0b01'0010);
        }

        for (u32 i = 0; i < m_declarations.size(); ++i) {
            const auto& decl = m_declarations[i];
            const auto& ptype = std::get<primitive_type>(decl.m_type);
            sid64 decl_type_sid = 0;
            switch (ptype.m_type) {
                case primitive_kind::BOOL:   decl_type_sid = SID("boolean"); break;
                case primitive_kind::I32:    decl_type_sid = SID("int32");   break;
                case primitive_kind::F32:    decl_type_sid = SID("float");   break;
                case primitive_kind::STRING: decl_type_sid = SID("string");  break;
                case primitive_kind::SID:    decl_type_sid = SID("symbol");  break;
                default:                     decl_type_sid = SID("int32");   break;
            }
            element.push_bytes(decl_type_sid, 0b0);

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

    for (u32 i = 0; i < m_states.size(); ++i) {
        const state_script_state& state = m_states[i];
        const SsState ss_state = {
            SID(state.m_name.c_str()),
            static_cast<i64>(state.m_blocks.size()),
            reinterpret_cast<SsOnBlock*>(block_offsets[i])
        };
        element.push_bytes(ss_state, 0b100);
    }

    element.push_bytes(array_sid, 0b0);

    u32 track_flat_idx = 0;
    for (u32 i = 0; i < m_states.size(); ++i) {
        const auto& state = m_states[i];
        const auto& block = state.m_blocks[0];
        const auto [block_id, block_event_id]  = block.get_block_struct_info();

        i32 block_type = static_cast<i32>(block_id);

        u16 block_lambda_count = 0;
        for (const auto& t : block.m_tracks) {
            block_lambda_count += static_cast<u16>(t.m_lambdas.size());
        }

        const u64 tg_name_idx = global.add_string(m_name + " " + state.m_name + " (on (" + block.m_name + "))");
        constexpr u64 offset = offsetof(SsOnBlock, m_trackGroup.m_name);
        element.insert_string_offset(offset);

        const SsOnBlock on_block = {
            block_type,
            0,
            block_event_id,
            nullptr,
            {
                0x0,
                block_lambda_count,
                static_cast<i16>(block.m_tracks.size()),
                0,
                reinterpret_cast<SsTrack*>(track_offsets.empty() ? 0 : track_offsets[track_flat_idx]),
                reinterpret_cast<const char*>(tg_name_idx),
                0x0,
                0x0,
                nullptr,
            }
        };
        element.push_bytes(on_block, 0b0110'0100, 0b10);
        track_flat_idx += static_cast<u32>(block.m_tracks.size());
    }

    

    u32 ss_lambda_flat_idx = 0;
    for (const auto& state : m_states) {
        for (const auto& block : state.m_blocks) {
            element.push_bytes(array_sid, 0b0);
            for (u32 ti = 0; ti < block.m_tracks.size(); ++ti) {
                const auto& track = block.m_tracks[ti];
                const SsTrack ss_track = {
                    SID(track.m_name.c_str()),
                    static_cast<u16>(ti),
                    static_cast<i16>(track.m_lambdas.size()),
                    0,
                    reinterpret_cast<SsLambda*>(ss_lambda_offsets.empty() ? 0 : ss_lambda_offsets[ss_lambda_flat_idx]),
                };
                element.push_bytes(ss_track, 0b100);
                ss_lambda_flat_idx += static_cast<u32>(track.m_lambdas.size());
            }
        }
    }

    u32 script_lambda_flat_idx = 0;
    u64 lambda_counter = 0;
    for (const auto& state : m_states) {
        for (const auto& block : state.m_blocks) {
            for (const auto& track : block.m_tracks) {
                element.push_bytes(array_sid, 0b0);
                for (u32 li = 0; li < track.m_lambdas.size(); ++li) {
                    const SsLambda ss_lam = {
                        reinterpret_cast<ScriptLambda*>(script_lambda_offsets[script_lambda_flat_idx]),
                        lambda_counter++,
                    };
                    element.push_bytes(ss_lam, 0b01);
                    script_lambda_flat_idx++;
                }
            }
        }
    }

    u32 lambda_element_flat_idx = 0;
    for (const auto& state : m_states) {
        for (const auto& block : state.m_blocks) {
            for (const auto& track : block.m_tracks) {
                for (u32 lambda_idx = 0; lambda_idx < track.m_lambdas.size(); ++lambda_idx) {
                    const compilation::program_binary_element& fn_element = lambda_elements[lambda_element_flat_idx++];

                    element.m_rawData.insert(element.m_rawData.end(), fn_element.m_rawData.begin(), fn_element.m_rawData.end());
                    element.m_relocTable.insert(element.m_relocTable.end(), fn_element.m_relocTable.begin(), fn_element.m_relocTable.end());
                }
            }
        }
    }

    return element;

}

}
