#include "ast/state_script/state_script.h"
#include "ast/primary_expressions/literal.h"
#include "compilation/function.h"
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
        os << indent << opt.second << "\n";
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
        os << opt.second << " ";
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
        os << " " << opt.second;
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
    constexpr sid64 script_name = SID("ss-compiled-test");
    constexpr sid64 state_script_sid = SID("state-script");
    constexpr sid64 array_sid = SID("array");
    constexpr sid64 ss_options_sid = SID("ss-options");
    constexpr sid64 ss_declarations_sid = SID("ss-declarations");

    u64 current_offset = 0;
    current_offset += sizeof(StateScript);
    const u64 declaration_list_offset = current_offset;
    current_offset += sizeof(SsDeclarationList);
    current_offset += sizeof(array_sid);
    const u64 state_offset = current_offset;
    current_offset += sizeof(SsState) * m_states.size();    
    current_offset += sizeof(ss_options_sid);
    const u64 options_offset = current_offset;
    current_offset += sizeof(SsOptions);
    current_offset += sizeof(u64) * 4; // unknown padding

    current_offset += sizeof(array_sid);
    const u64 declaration_data_offset = current_offset;
    current_offset += sizeof(SsDeclaration) * m_declarations.size();
    current_offset += sizeof(array_sid);
    const u64 block_data_offset = current_offset;

    std::vector<u64> block_offsets;
    for (const auto& state : m_states) {
        block_offsets.push_back(current_offset);
        current_offset += sizeof(SsOnBlock);
    }

    std::vector<u64> declaration_value_offset;
    for (const auto& decl : m_declarations) {
        declaration_value_offset.push_back(current_offset);
        current_offset += sizeof(sid64) + sizeof(void*);
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
                current_offset += sizeof(array_sid);
                for (const auto& lambda : track.m_lambdas) {
                    script_lambda_offsets.push_back(current_offset);
                    current_offset += sizeof(ScriptLambda);
                }
            }
        }
    }

    const u64 total_declarations_size = std::accumulate(m_declarations.begin(), m_declarations.end(), 0ull, [](u64 sum, const variable_declaration& decl) {
        return sum + get_size(decl.m_type);
    });

    compilation::program_binary_element element{current_offset};

    element.push_bytes(state_script_sid, 0b0);


    const u64 debug_str_idx = global.add_string("debug_file_name_placeholder");
    element.insert_string_offset(current_offset + offsetof(StateScript, m_pDebugFileName));
    const StateScript ss = {
        script_name,
        reinterpret_cast<SsDeclarationList*>(declaration_list_offset),
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

    element.push_bytes(ss_declarations_sid, 0b0);
    
    const SsDeclarationList decl_list = {
        static_cast<u32>(total_declarations_size),
        static_cast<u32>(m_declarations.size()),
        reinterpret_cast<SsDeclaration*>(declaration_data_offset)
    };

    element.push_bytes(decl_list, 0b01);


    element.push_bytes(array_sid, 0b0);

    for (u32 i = 0; i < m_states.size(); ++i) {
        const state_script_state& state = m_states[i];
        const SsState ss_state = {
            SID(state.m_name.c_str()),
            static_cast<i64>(state.m_blocks.size()),
            reinterpret_cast<SsOnBlock*>(block_offsets[i])
        };
        element.push_bytes(ss_state, 0b0);
    }

    element.push_bytes(ss_options_sid, 0b0);

    const u64 options_string_idx = global.add_string(m_optionsString);
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
    element.push_bytes(ss_options, 0b0000'0001, 0b00);

    for (u32 i = 0; i < 4; ++i) {
        element.push_bytes(u64{0}, 0b0);
    }

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

        const SsDeclaration ss_decl = {
            SID(decl.m_identifier.c_str()),
            nullptr,
            decl_type_sid,
            var_size_sum,
            1,
            0,
            reinterpret_cast<void*>(declaration_value_offset[i]),
            0x80,
        };
        element.push_bytes(ss_decl, 0b01'0010);
        var_size_sum += static_cast<u16>(get_size(decl.m_type));
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
            if (lit) {
                std::visit([&](auto&& val) {
                    using T = std::decay_t<decltype(val)>;
                    if constexpr (std::is_arithmetic_v<T>) {
                        std::memcpy(&init_value, &val, sizeof(val));
                    }
                }, lit->m_value);
            }
        }
        element.push_bytes(init_value, 0b0);
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

    constexpr sid64 function_sid = SID("function");
    constexpr u64 deadbeef = 0xDEAD'BEEF'1337'F00D;

    for (const auto& state : m_states) {
        for (const auto& block : state.m_blocks) {
            for (const auto& track : block.m_tracks) {
                element.push_bytes(array_sid, 0b0);
                for (const auto& lambda : track.m_lambdas) {
                    compilation::function fn;
                    (void)lambda.m_body.emit_dc(fn, global);

                    const ScriptLambda script_lam = {
                        nullptr,
                        nullptr,
                        function_sid,
                        fn.get_scriptlambda_sum(),
                        0x0,
                        deadbeef,
                        0x0,
                        static_cast<u32>(fn.m_instructions.size()),
                        -1,
                        script_name,
                        0x0,
                    };
                    element.push_bytes(script_lam, 0b0000'0011, 0b00);
                }
            }
        }
    }

    return element;

}

}
