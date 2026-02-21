#include "ast/state_script/state_script.h"
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


    const u64 num_states = m_states.size();
    if (num_states == 0) {
        return std::unexpected("state script code emission requires at least one state");
    }


    std::vector<SsDeclaration> declarations;
    declarations.reserve(m_declarations.size());
    i32 total_declaration_size = 0;
    for (const auto& decl : m_declarations) {
        SsDeclaration ss_decl{};
        ss_decl.m_declId = SID(decl.m_identifier.c_str());
        ss_decl.m_declIdString = nullptr;
        const auto& prim_type = std::get<primitive_type>(decl.m_type);
        switch (prim_type.m_type) {
            case primitive_kind::I32:
                ss_decl.m_declTypeId = SID("int32");
                ss_decl.m_varSizeSum = sizeof(i32);
                break;
            case primitive_kind::U32:
                ss_decl.m_declTypeId = SID("u32");
                ss_decl.m_varSizeSum = sizeof(u32);
                break;
            case primitive_kind::F32:
                ss_decl.m_declTypeId = SID("float");
                ss_decl.m_varSizeSum = sizeof(f32);
                break;
            case primitive_kind::BOOL:
                ss_decl.m_declTypeId = SID("boolean");
                ss_decl.m_varSizeSum = sizeof(bool);
                break;
        }
        ss_decl.m_isVar = 1;
        ss_decl.m_always0 = 0;
        ss_decl.m_pDeclValue = nullptr;
        ss_decl.m_always0x80 = 0x80;
        total_declaration_size += ss_decl.m_varSizeSum;
        declarations.push_back(ss_decl);
    }

    const u64 total_blocks = std::accumulate(m_states.begin(), m_states.end(), u64{0}, [](const u64 acc, const state_script_state& s) {
        return acc + s.m_blocks.size();
    });
    u64 total_tracks = 0;
    u64 total_lambdas = 0;
    for (const auto& state : m_states) {
        for (const auto& block : state.m_blocks) {
            total_tracks += block.m_tracks.size();
            for (const auto& track : block.m_tracks) {
                total_lambdas += track.m_lambdas.size();
            }
        }
    }

    std::vector<sid64> option_symbols;
    option_symbols.reserve(m_options.size());
    for (const auto& opt : m_options) {
        option_symbols.push_back(opt.first);
    }

    u64 current_offset = 0;
    current_offset += sizeof(sid64);
    const u64 state_script_struct_offset = current_offset;
    current_offset += sizeof(StateScript);
    current_offset += sizeof(sid64);
    const u64 options_struct_offset = current_offset;
    current_offset += sizeof(SsOptions);
    const u64 symbol_array1_offset = current_offset;
    current_offset += sizeof(SymbolArray);
    const u64 symbol_array2_offset = current_offset;
    current_offset += sizeof(SymbolArray);
    const u64 symbol_array3_offset = current_offset;
    current_offset += sizeof(SymbolArray);
    const u64 symbol_array4_offset = current_offset;
    current_offset += sizeof(SymbolArray);
    const u64 option_symbols_offset = current_offset;
    current_offset += option_symbols.size() * sizeof(sid64);
    current_offset += sizeof(sid64);
    const u64 declaration_list_offset = current_offset;
    current_offset += sizeof(SsDeclarationList);
    const u64 declarations_offset = current_offset;
    current_offset += declarations.size() * sizeof(SsDeclaration);
    current_offset += sizeof(sid64);
    const u64 states_offset = current_offset;
    current_offset += m_states.size() * sizeof(SsState);
    const u64 blocks_offset = current_offset;
    current_offset += total_blocks * sizeof(SsOnBlock);
    const u64 tracks_offset = current_offset;
    current_offset += total_tracks * sizeof(SsTrack);
    const u64 lambdas_offset = current_offset;
    current_offset += total_lambdas * sizeof(SsLambda);

    compilation::program_binary_element element{current_offset};

    const u64 option_string_idx = global.add_string(m_options.empty() ? std::string{"ss-option-placeholder"} : m_options.front().second);
    const u64 debug_name_idx = global.add_string("ss-debug-placeholder");
    const u64 error_name_idx = global.add_string("ss-error-placeholder");

    StateScript script{};
    script.m_stateScriptId = script_name;
    script.m_pSsDeclList = reinterpret_cast<SsDeclarationList*>(declaration_list_offset);
    const u64 initial_state_idx = m_initialStateIdx < m_states.size() ? m_initialStateIdx : 0;
    script.m_initialStateId = SID(m_states[initial_state_idx].m_name.c_str());
    script.m_pSsOptions = reinterpret_cast<SsOptions*>(options_struct_offset);
    script.m_always0_1 = 0;
    script.m_pSsStateTable = reinterpret_cast<SsState*>(states_offset);
    script.m_stateCount = static_cast<i16>(m_states.size());
    script.m_line = 0x10;
    script.m_always0_2 = 0;
    script.m_pDebugFileName = nullptr;
    script.m_pErrorName = nullptr;
    script.m_padding = 0;

    SsOptions options{};
    options.m_optionString = nullptr;
    options.m_unknownFlags = 0;
    options.m_always0_1 = 0;
    options.m_pSymbolArray = reinterpret_cast<SymbolArray*>(symbol_array1_offset);
    options.m_symbolArray2 = reinterpret_cast<SymbolArray*>(symbol_array2_offset);
    options.m_symbolArray3 = reinterpret_cast<SymbolArray*>(symbol_array3_offset);
    options.m_symbolArray4 = reinterpret_cast<SymbolArray*>(symbol_array4_offset);
    options.m_always5 = 5;
    options.m_mostly0 = 0;
    options.m_mostly0Rarely1 = 0;
    options.m_always0_2 = 0;

    SymbolArray symbol_array1{};
    symbol_array1.m_numEntries = static_cast<i32>(option_symbols.size());
    symbol_array1.m_unk = 0;
    symbol_array1.m_pSymbols = reinterpret_cast<sid64*>(option_symbols_offset);

    SymbolArray symbol_array2{};
    SymbolArray symbol_array3{};
    SymbolArray symbol_array4{};

    SsDeclarationList declaration_list{};
    declaration_list.m_totalDeclarationSize = total_declaration_size;
    declaration_list.m_numDeclarations = static_cast<i32>(declarations.size());
    declaration_list.m_pDeclarations = reinterpret_cast<SsDeclaration*>(declarations_offset);

    std::vector<SsState> states;
    states.resize(m_states.size());
    std::vector<SsOnBlock> blocks;
    blocks.resize(total_blocks);
    std::vector<SsTrack> tracks;
    tracks.resize(total_tracks);
    std::vector<SsLambda> lambdas;
    lambdas.resize(total_lambdas);

    u64 block_index = 0;
    u64 track_index = 0;
    u64 lambda_index = 0;

    std::vector<std::pair<u64, u64>> block_name_string_offsets;
    block_name_string_offsets.reserve(total_blocks);
    std::vector<std::pair<u64, u64>> declaration_name_string_offsets;
    declaration_name_string_offsets.reserve(declarations.size());

    for (u64 state_idx = 0; state_idx < m_states.size(); ++state_idx) {
        const auto& source_state = m_states[state_idx];
        SsState& out_state = states[state_idx];

        out_state.m_stateId = SID(source_state.m_name.c_str());
        out_state.m_numSsOnBlocks = source_state.m_blocks.size();
        out_state.m_pSsOnBlocks = source_state.m_blocks.empty() ? nullptr : reinterpret_cast<SsOnBlock*>(blocks_offset + block_index * sizeof(SsOnBlock));

        for (const auto& source_block : source_state.m_blocks) {
            const u64 block_start_track_index = track_index;
            u64 block_lambda_count = 0;

            for (const auto& source_track : source_block.m_tracks) {
                SsTrack& out_track = tracks[track_index];
                out_track.m_trackId = SID(source_track.m_name.c_str());
                out_track.m_trackIdx = static_cast<u16>(track_index - block_start_track_index);
                out_track.m_totalLambdaCount = static_cast<i16>(source_track.m_lambdas.size());
                out_track.m_padding = 0;
                out_track.m_pSsLambda = source_track.m_lambdas.empty() ? nullptr : reinterpret_cast<SsLambda*>(lambdas_offset + lambda_index * sizeof(SsLambda));

                for (u64 local_lambda_idx = 0; local_lambda_idx < source_track.m_lambdas.size(); ++local_lambda_idx) {
                    SsLambda& out_lambda = lambdas[lambda_index++];
                    out_lambda.m_pScriptLambda = nullptr;
                    out_lambda.m_someSortOfCounter = lambda_index;
                }

                block_lambda_count += source_track.m_lambdas.size();
                ++track_index;
            }

            SsOnBlock& out_block = blocks[block_index];
            out_block.m_blockType = 0;
            out_block.m_always0 = 0;
            out_block.m_blockEventId = source_block.m_name.empty() ? sid64{0} : SID(source_block.m_name.c_str());
            out_block.m_pScriptLambda = nullptr;
            out_block.m_trackGroup.m_always0 = 0;
            out_block.m_trackGroup.m_totalLambdaCount = static_cast<u16>(block_lambda_count);
            out_block.m_trackGroup.m_numTracks = static_cast<i16>(source_block.m_tracks.size());
            out_block.m_trackGroup.m_padding = 0;
            out_block.m_trackGroup.m_aTracks = source_block.m_tracks.empty() ? nullptr : reinterpret_cast<SsTrack*>(tracks_offset + block_start_track_index * sizeof(SsTrack));
            out_block.m_trackGroup.m_name = nullptr;
            out_block.m_trackGroup.m_always0_1 = 0;
            out_block.m_trackGroup.m_always0_2 = 0;
            out_block.m_trackGroup.m_rareScriptLambda = nullptr;

            const u64 block_name_string_idx = global.add_string(source_block.m_name.empty() ? std::string{"ss-block-placeholder"} : source_block.m_name);
            const u64 block_name_offset = blocks_offset + block_index * sizeof(SsOnBlock) + offsetof(SsOnBlock, m_trackGroup) + offsetof(SsTrackGroup, m_name);
            block_name_string_offsets.emplace_back(block_name_offset, block_name_string_idx);

            ++block_index;
        }
    }

    for (u64 decl_idx = 0; decl_idx < declarations.size(); ++decl_idx) {
        const u64 decl_name_idx = global.add_string(m_declarations[decl_idx].m_identifier);
        const u64 decl_name_offset = declarations_offset + decl_idx * sizeof(SsDeclaration) + offsetof(SsDeclaration, m_declIdString);
        declaration_name_string_offsets.emplace_back(decl_name_offset, decl_name_idx);
    }

    auto push = [&element](const auto& value, auto... reloc_bits) {
        element.push_bytes(value, reloc_bits...);
        element.m_currentSize = element.m_rawData.size();
    };

    push(state_script_sid, 0b0);
    push(script, 0b1010'1010, 0b01);
    push(ss_options_sid, 0b0);
    push(options, 0b0111'1001, 0b00);
    push(symbol_array1, 0b10);
    push(symbol_array2, 0b00);
    push(symbol_array3, 0b00);
    push(symbol_array4, 0b00);
    for (const sid64 option_sid : option_symbols) {
        push(option_sid, 0b0);
    }
    push(ss_declarations_sid, 0b0);
    push(declaration_list, 0b10);
    for (const SsDeclaration& declaration : declarations) {
        push(declaration, 0b000010, 0b00);
    }
    push(array_sid, 0b0);
    for (const SsState& state : states) {
        const u8 reloc_bits = state.m_pSsOnBlocks ? 0b100 : 0b000;
        push(state, reloc_bits);
    }
    for (const SsOnBlock& block : blocks) {
        const u8 first_bits = static_cast<u8>((block.m_pScriptLambda ? 0b0000'0100 : 0) |
                                              (block.m_trackGroup.m_aTracks ? 0b0001'0000 : 0) |
                                              0b0010'0000 |
                                              (block.m_trackGroup.m_rareScriptLambda ? 0b1000'0000 : 0));
        const u8 second_bits = 0b00;
        push(block, first_bits, second_bits);
    }
    for (const SsTrack& track : tracks) {
        const u8 reloc_bits = track.m_pSsLambda ? 0b100 : 0b000;
        push(track, reloc_bits);
    }
    for (const SsLambda& lambda : lambdas) {
        const u8 reloc_bits = lambda.m_pScriptLambda ? 0b01 : 0b00;
        push(lambda, reloc_bits);
    }

    element.m_stringOffsets.emplace_back(options_struct_offset + offsetof(SsOptions, m_optionString), option_string_idx);
    element.m_stringOffsets.emplace_back(state_script_struct_offset + offsetof(StateScript, m_pDebugFileName), debug_name_idx);
    element.m_stringOffsets.emplace_back(state_script_struct_offset + offsetof(StateScript, m_pErrorName), error_name_idx);
    for (const auto& [offset, str_idx] : declaration_name_string_offsets) {
        element.m_stringOffsets.emplace_back(offset, str_idx);
    }
    for (const auto& [offset, str_idx] : block_name_string_offsets) {
        element.m_stringOffsets.emplace_back(offset, str_idx);
    }

    element.m_entry = Entry{
        script_name,
        state_script_sid,
        reinterpret_cast<const void*>(state_script_struct_offset)
    };

    return element;

}

}
