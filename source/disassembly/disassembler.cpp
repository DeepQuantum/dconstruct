#include "custom_structs.h"
#include <cmath>
#include <chrono>
#include "disassembler.h"
#include <string.h>


[[nodiscard]] const char *Disassembler::lookup(const sid64 sid) noexcept {
    auto res = m_currentFile->m_sidCache.find(sid);
    if (res != m_currentFile->m_sidCache.end()) {
        return res->second.c_str();
    }

    const char *hash_string = m_sidbase->search(sid);
    if (hash_string == nullptr) {
        const std::string new_hash_string = int_to_string_id(sid);
        auto [iter, inserted] = m_currentFile->m_sidCache.emplace(sid, new_hash_string);
        hash_string = iter->second.c_str();
    }
    return hash_string;
}

template<typename... Args>
void Disassembler::insert_span_fmt(const char *format, const TextFormat &text_format, Args ...args) {
    char buffer[512] = { 0 };
    snprintf(buffer, sizeof(buffer), format, args...);
    insert_span(buffer, text_format);
}

[[nodiscard]] b8 Disassembler::is_sid(const sid64 value) const noexcept {
    const b8 in_ptr_range = value >= reinterpret_cast<p64>(m_currentFile->m_bytes.get()) 
        && value < reinterpret_cast<p64>(m_currentFile->m_bytes.get()) + m_currentFile->m_size;
    b8 val = !(in_ptr_range && m_currentFile->is_file_ptr(value)) && value > m_sidbase->m_lowestSid;
    return val;
} 

[[nodiscard]] b8 Disassembler::is_empty_array_ptr(const p64 data_ptr) const noexcept {
    const b8 last_int_zero = *(reinterpret_cast<const i32*>(&data_ptr - 1)) == 0;
    const b8 next_loc_sid = is_sid(*reinterpret_cast<const sid64*>(data_ptr));
    const b8 value_after_sid_pointed_at = m_currentFile->location_gets_pointed_at((void*)(data_ptr + 8));
    return last_int_zero && next_loc_sid && value_after_sid_pointed_at;
}

[[nodiscard]] b8 Disassembler::is_possible_float(const f32 *val) const noexcept {
    f32 rounded = roundf(*val * 1e5f) / 1e5f;
    return fabsf(*val - rounded) < 1e-5f && *val > -1e5f && *val < 1e5f && rounded != 0.f;
}

[[nodiscard]] b8 Disassembler::is_possible_i32(const i32 *val) const noexcept {
    return abs(*val) < 50000;
}

/*
    a "struct" is a piece of data that has a type ID and some data following it.
    an "array-like" is essentially just a size + pointer. the thing being pointed at can be anything, either proper structs
    or more array-likes. array-likes may also point at array-structs, which are just blocks of data with the type ID "array"
    followed by some data. array structs don't provide their own size, so the array-like must provide the size.
    array structs may contain arbitrarily sized members though, so we must check each member individually.

*/  
u32 Disassembler::insert_struct_or_arraylike(const p64 *struct_location, const u64 indent) noexcept {
    const p64 struct_data_start = *struct_location;
    if (m_currentFile->is_file_ptr(reinterpret_cast<p64>(struct_location)) && reinterpret_cast<p64>(struct_data_start) > m_currentFile->m_stringsPtr) {
        insert_span_fmt("%*sstring: \"%s\"\n", {.m_color = TYPE_COLOR}, indent, "", reinterpret_cast<const char*>(struct_data_start));
        return 0;
    }
    if (!m_currentFile->is_file_ptr(reinterpret_cast<p64>(struct_location))) {
        if (reinterpret_cast<p64>(struct_location) > m_currentFile->m_stringsPtr) {
            insert_span_fmt("%*sstring: \"%s\"\n", COMMENT_FMT, indent, "", reinterpret_cast<const char*>(struct_location));
        } else {
            //insert_span_fmt("%*sinserting primitive\n", COMMENT_FMT, indent, "");
            return insert_next_struct_member(reinterpret_cast<p64>(struct_location), indent);
        }
        return 0;
    }
    const u64 next_struct_header = *reinterpret_cast<const u64*>(struct_data_start - 8);
    if (next_struct_header == SID("array")) {
        insert_array(struct_location, indent);
    } else {
        if (!m_sidbase->sid_exists(next_struct_header)) {
            const u32 anonymous_array_size = *reinterpret_cast<const u32*>(struct_location + 1);
            insert_span_fmt("%*sanonymous array [0x%x] {size: %d} {\n", COMMENT_FMT, indent, "", get_offset((void*)struct_location), anonymous_array_size);
            for (u64 i = 0; i < anonymous_array_size; ++i) {
                insert_span_fmt("%*s[%d]\n", COMMENT_FMT, indent, "", i);
                insert_struct_or_arraylike(reinterpret_cast<const p64*>(struct_data_start + i * 8), indent + m_options.m_indentPerLevel);
            }
        } else {
            //insert_span_fmt("%*sinserting struct\n", COMMENT_FMT, indent, "");
            const dc_structs::unmapped *_struct = reinterpret_cast<const dc_structs::unmapped*>(struct_data_start - 8);
            insert_struct(_struct, indent);
        }
    }
    return 0;
}

void Disassembler::insert_array(const p64 *struct_location, const u64 indent) {
    const p64 struct_data_start = *struct_location;
    u32 size_array = *(struct_location - 1);
    insert_span_fmt("%*sarray [0x%x] {size: %d} {\n", COMMENT_FMT, indent, "", get_offset((void *)struct_location), size_array);
    if (size_array == 0) {
        insert_span("}\n", COMMENT_FMT, indent);
        return;
    }
    u32 member_offset = 0;
    u32 var_count = 0;
    b8 struct_size_determined = false;
    insert_span_fmt("%*sarray entry {0} anonymous struct [0x%x] {\n", COMMENT_FMT, indent + m_options.m_indentPerLevel, "", get_offset((void *)struct_data_start));
    while (!struct_size_determined) {
        insert_span_fmt("%*s[%d] ", COMMENT_FMT, indent + m_options.m_indentPerLevel * 2, "", var_count++);
        u32 moved = insert_struct_or_arraylike(reinterpret_cast<const p64 *>(struct_data_start + member_offset), 0);
        member_offset += moved ? moved : 8;
        struct_size_determined = m_currentFile->location_gets_pointed_at((void *)(struct_data_start + member_offset * size_array + 8));
    }
    insert_span("}\n", COMMENT_FMT, indent + m_options.m_indentPerLevel);
    u32 struct_size = member_offset;
    for (u32 array_entry_count = 1; array_entry_count < size_array; ++array_entry_count) {
        member_offset = 0;
        insert_span_fmt("%*sarray entry {%d} anonymous struct [0x%x] {\n", COMMENT_FMT, indent + m_options.m_indentPerLevel, "", array_entry_count, get_offset((void *)(struct_data_start + array_entry_count * struct_size)));
        var_count = 0;
        while (member_offset < struct_size) {
            insert_span_fmt("%*s[%d] ", COMMENT_FMT, indent + m_options.m_indentPerLevel * 2, "", var_count++);
            const p64 current_member_location = struct_data_start + array_entry_count * struct_size + member_offset;
            u32 moved = insert_struct_or_arraylike(reinterpret_cast<const p64 *>(current_member_location), 0);
            member_offset += moved ? moved : 8;
        }
        insert_span("}\n", COMMENT_FMT, indent + m_options.m_indentPerLevel);
    }
    insert_span("}\n", COMMENT_FMT, indent);
}

void Disassembler::insert_unmapped_struct(const dc_structs::unmapped *struct_ptr, const u64 indent) {
    u64 offset = 0;
    b8 offset_gets_pointed_at = false;
    u64 move = 0;
    u32 entry_count = 0;
    while (!offset_gets_pointed_at) {
        offset += move;
        const p64 data_ptr = reinterpret_cast<p64>(&struct_ptr->m_data) + offset;
        move = insert_next_struct_member(data_ptr, indent);
        entry_count++;
        offset_gets_pointed_at = m_currentFile->location_gets_pointed_at((void*)(data_ptr + move + 8));
    }
}

u32 Disassembler::insert_next_struct_member(const p64 data_ptr, const u64 indent) {
    u32 move;
    const char *str_ptr = nullptr;
    if (m_currentFile->is_file_ptr(data_ptr)) {
        const p64 ptr_value = *reinterpret_cast<const p64 *>(data_ptr);
        if (ptr_value >= m_currentFile->m_stringsPtr) {
            insert_span("string: ", {.m_color = TYPE_COLOR}, indent);
            insert_span_fmt("\"%s", {.m_color = STRING_COLOR}, reinterpret_cast<const char *>(ptr_value));
            insert_span("\"\n", {.m_color = STRING_COLOR});
        }
        else {
            insert_struct_or_arraylike(reinterpret_cast<p64 *>(data_ptr), indent);
        }
        move = 8;
    }
    else if ((str_ptr = m_sidbase->search(*reinterpret_cast<const sid64*>(data_ptr))) != nullptr) {
        insert_span_fmt("%*ssid: %s\n", {.m_color = HASH_COLOR}, indent, "", str_ptr);
        move = 8;
    }
    else if (is_possible_float(reinterpret_cast<const f32 *>(data_ptr))) {
        insert_span_fmt("%*sfloat: %.2f\n", {.m_color = NUM_COLOR}, indent, "", *reinterpret_cast<const f32 *>(data_ptr));
        move = 4;
    }
    else if (is_possible_i32(reinterpret_cast<const i32 *>(data_ptr))) {
        insert_span_fmt("%*sint: %d\n", {.m_color = NUM_COLOR}, indent, "", *reinterpret_cast<const i32 *>(data_ptr));
        move = 4;
    }
    else if (data_ptr % 8 == 0 && *reinterpret_cast<const sid64 *>(data_ptr) > m_sidbase->m_lowestSid && *reinterpret_cast<const sid64 *>(data_ptr)) {
        str_ptr = lookup(*reinterpret_cast<const sid64 *>(data_ptr));
        insert_span_fmt("%*ssid: %s\n", {.m_color = HASH_COLOR}, indent, "", str_ptr);
        move = 8;
    }
    else {
        insert_span_fmt("%*sint: %d\n", {.m_color = NUM_COLOR}, indent, "", *reinterpret_cast<const i32 *>(data_ptr));
        move = 4;
    }
    return move;
}

void Disassembler::disassemble() {
    insert_header_line();
    const std::string sep = std::string(100, '#') + "\n";
    for (i32 i = 0; i < m_currentFile->m_dcheader->m_numEntries; ++i) {
        insert_entry(m_currentFile->m_dcheader->m_pStartOfData + i);
        insert_span(sep.c_str(), {.m_color = COMMENT_COLOR, .m_fontSize = 14});
    }
    complete();
}

void Disassembler::insert_entry(const Entry *entry) {
    const dc_structs::unmapped *struct_ptr = reinterpret_cast<const dc_structs::unmapped*>(reinterpret_cast<const u64*>(entry->m_entryPtr) - 1);
    insert_span_fmt("%s = ", ENTRY_HEADER_FMT, lookup(entry->m_scriptId));
    insert_struct(struct_ptr);
}


void Disassembler::insert_struct(const dc_structs::unmapped *struct_ptr, const u64 indent) {
    //printf("\n%*s%s %x\n", indent, "", lookup(entry->m_typeID), get_offset(entry));

    if (indent > m_options.m_indentPerLevel * 20) {
        printf("wups");
    }
    const u64 offset = get_offset((void*)struct_ptr);
    TextFormat opcode_format = {OPCODE_COLOR, 14};

    insert_span_fmt("%*s%s [0x%05X] {\n", ENTRY_TYPE_FMT, indent, "", lookup(struct_ptr->typeID), offset);

    switch (struct_ptr->typeID) {
        case SID("boolean"): {
            insert_span_fmt("%*sbool: <%s>\n", opcode_format, indent, "", *reinterpret_cast<const i32*>(&struct_ptr->m_data) ? "true" : "false");
            break;
        }
        case SID("int"): {
            insert_span_fmt("%*sint: <%d>\n", opcode_format, indent, "", *reinterpret_cast<const i32*>(&struct_ptr->m_data));
            break;
        }
        case SID("float"): {
            insert_span_fmt("%*sfloat: <%.2f>\n", opcode_format, indent, "", *reinterpret_cast<const f32*>(&struct_ptr->m_data));
            break;
        }
        case SID("sid"): {
            insert_span_fmt("%*ssid: <%s>\n", opcode_format, indent, "", lookup(*reinterpret_cast<const sid64*>(&struct_ptr->m_data)));
            break;
        }
        case SID("state-script"): {
            insert_state_script(reinterpret_cast<const StateScript*>(&struct_ptr->m_data), indent + m_options.m_indentPerLevel);
            break;
        }
        case SID("script-lambda"): {
            std::unique_ptr<FunctionDisassembly> function = std::make_unique<FunctionDisassembly>(std::move(create_function_disassembly(reinterpret_cast<const ScriptLambda*>(&struct_ptr->m_data))));
            insert_function_disassembly_text(*function, indent + m_options.m_indentPerLevel);
            m_currentFile->m_functions.push_back(std::move(function));
            break;
        }
        case SID("symbol-array"): {
            const dc_structs::symbol_array *array = reinterpret_cast<const dc_structs::symbol_array*>(&struct_ptr->m_data);
            for (u64 i = 0; i < array->contents.size; ++i) {
                insert_span_fmt("%*s%d. %s\n", {OPCODE_COLOR, 14}, indent + m_options.m_indentPerLevel, "", i + 1, lookup(array->contents.keys[i]));
            }
            break;
        }
        case SID("map"):
        case SID("map-32"): {
            const dc_structs::map *map = reinterpret_cast<const dc_structs::map*>(&struct_ptr->m_data);
            insert_span_fmt("%*skeys: [0x%05X], values: [0x%05X]\n\n", {HASH_COLOR, 16}, indent + m_options.m_indentPerLevel, "", get_offset(map->keys.data), get_offset(map->values.data));
            for (u64 i = 0; i < map->size; ++i) {
                const char *key_hash = lookup(map->keys[i]);
                insert_span_fmt("%*s%s {\n", opcode_format, indent + m_options.m_indentPerLevel, "", key_hash);
                const dc_structs::unmapped *struct_ptr = reinterpret_cast<const dc_structs::unmapped*>(map->values[i] - 8);
                insert_struct(struct_ptr, indent + m_options.m_indentPerLevel * 2);
                insert_span("}\n", {CONTROL_COLOR, 16}, indent + m_options.m_indentPerLevel);
            }
            break;
        }
        case SID("point-curve"): {
            const dc_structs::point_curve *curve = reinterpret_cast<const dc_structs::point_curve*>(&struct_ptr->m_data);
            insert_span_fmt("%*sint: %u\n", {.m_color = NUM_COLOR}, indent + m_options.m_indentPerLevel, "", curve->int1);
            for (u64 i = 0; i < 3; ++i) {
                insert_span_fmt("%*s%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n", 
                    {.m_color = NUM_COLOR}, indent + m_options.m_indentPerLevel,"",
                    curve->floats[i + 0], curve->floats[i + 1], curve->floats[i + 2],
                    curve->floats[i + 3], curve->floats[i + 4], curve->floats[i + 5],
                    curve->floats[i + 6], curve->floats[i + 7], curve->floats[i + 8],
                    curve->floats[i + 9], curve->floats[i + 10]
                );
            }
            break;
        }
        // case SID("array*"): {
        //     const u32 num_elements = *(reinterpret_cast<const u32*>(&struct_ptr->m_data) + 2);
        //     const p64 *array_entries_start = reinterpret_cast<const u64*>(struct_ptr->m_data);
        //     for (u64 i = 0; i < num_elements; ++i) {
        //         //const dc_structs::unmapped *array_entry_struct_ptr = reinterpret_cast<const dc_structs::unmapped*>(array_entries_start[i] - 8);
        //         insert_struct_or_arraylike(reinterpret_cast<p64*>(array_entries_start[i] - 8), indent + m_options.m_indentPerLevel);
        //     }
        //     break;
        // }
        case SID("string-array"): {
            const dc_structs::string_array* str_a = reinterpret_cast<const dc_structs::string_array*>(&struct_ptr->m_data);
            u64 str_offset = 0;
            for (u64 i = 0; i < str_a->size; ++i) {
                const char* text = reinterpret_cast<const char*>(str_a->contents.data[0] + str_offset);
                printf("%s\n", text);
                str_offset += strlen(text) + 1;
            }
            break;
        }
        default: {
            if (m_options.m_emitOnce && m_currentFile->m_emittedStructs.find(reinterpret_cast<p64>(struct_ptr)) != m_currentFile->m_emittedStructs.end()) {
                insert_span_fmt("%*sALREADY_EMITTED\n%*s}", {.m_color = COMMENT_COLOR}, indent + m_options.m_indentPerLevel, "", indent, "");
                return;
            }
            insert_unmapped_struct(struct_ptr, indent + m_options.m_indentPerLevel);
            break;
        }
    }
    insert_span("}\n", ENTRY_TYPE_FMT, indent);
    if (m_options.m_emitOnce) {
        m_currentFile->m_emittedStructs.emplace(reinterpret_cast<p64>(struct_ptr));
    }
}

[[nodiscard]] u32 Disassembler::get_offset(const void *symbol) const noexcept {
    return reinterpret_cast<p64>(symbol) - reinterpret_cast<p64>(m_currentFile->m_dcheader);
}

void Disassembler::insert_variable(const SsDeclaration *var, const u32 indent) {
    b8 is_nullptr = var->m_pDeclValue == nullptr;

    constexpr TextFormat var_format = {.m_color = VAR_COLOR, .m_fontSize = 14};

    insert_span_fmt("%*s[0x%06X] ", {.m_color = NUM_COLOR}, indent, "", get_offset(var));
    insert_span_fmt("%-8s ", {.m_color = TYPE_COLOR, .m_fontSize = 14}, lookup(var->m_declTypeId));
    insert_span_fmt("%-20s = ", {.m_color = VAR_COLOR, .m_fontSize = 14},  lookup(var->m_declId));

    switch (var->m_declTypeId) {
        case SID("boolean"): {
            if (!is_nullptr) 
                insert_span(*reinterpret_cast<b8*>(var->m_pDeclValue) ? "true" : "false", TextFormat{.m_color = HASH_COLOR});
            break;
        }
        case SID("vector"): {
            if (!is_nullptr) {
                f32 *val = reinterpret_cast<f32*>(var->m_pDeclValue);
                insert_span_fmt("(%.2f, %.2f, %.2f, %.2f)", var_format, val[0], val[1], val[2], val[3]);
            }
            break;
        }
        case SID("quat"): {
            if (!is_nullptr) {
                f32 *val = reinterpret_cast<f32*>(var->m_pDeclValue);
                insert_span_fmt("(%.2f, %.2f, %.2f, %.2f)", var_format, val[0], val[1], val[2], val[3]);
            }
            break;
        }
        case SID("float"): {
            if (!is_nullptr) {
                insert_span_fmt("%.2f", var_format, *reinterpret_cast<f32*>(var->m_pDeclValue));
            }
            break;
        }
        case SID("string"): {
            if (!is_nullptr) {
                insert_span_fmt("%s", var_format, *reinterpret_cast<const char**>(var->m_pDeclValue));
            }
            break;
        }
        case SID("symbol"): {
            if (!is_nullptr) {
                insert_span(lookup(*reinterpret_cast<sid64*>(var->m_pDeclValue)), var_format);
            }
            break;
        }
        case SID("int32"): {
            if (!is_nullptr) {
                insert_span_fmt("%i", var_format, *reinterpret_cast<i32*>(var->m_pDeclValue));
            }
            break;
        }
        case SID("uint64"): {
            if (!is_nullptr) {
                insert_span_fmt("%llx", var_format, *reinterpret_cast<u64*>(var->m_pDeclValue));
            }
            break;
        }
        case SID("timer"): {
            if (!is_nullptr) {
                insert_span_fmt("%f", var_format, *reinterpret_cast<f32*>(var->m_pDeclValue));
            }
            break;
        }
        case SID("point"): {
            if (!is_nullptr) {
                f32 *val = reinterpret_cast<f32*>(var->m_pDeclValue);
                insert_span_fmt("(%.2f, %.2f, %.2f)", var_format, val[0], val[1], val[2]);
            }
            break;
        }
        case SID("bound-frame"): {
            if (!is_nullptr) {
                insert_span_fmt("%f", var_format, *reinterpret_cast<f32*>(var->m_pDeclValue));
            }
            break;
        }
        default: {
            insert_span("???", var_format);
            break;
        }
    }
    if (is_nullptr) {
        insert_span("uninitialized");
    }
    insert_span("\n");
   // insert_span_fmt("%*s<%s>\n", TextFormat{OPCODE_COLOR, 12}, 8, "", var_contents);
}

void Disassembler::insert_on_block(const SsOnBlock *block, const u32 indent) {
    TextFormat block_format{COMMENT_COLOR, 12};
    switch (block->m_blockType) {
        case 0: {
            insert_span("ON start {\n", block_format, indent);
            break;
        }
        case 1: {
            insert_span("ON end {\n", block_format, indent);
            break;
        }
        case 2: {
            insert_span_fmt("%*sON event %s {\n", block_format, indent, "", lookup(block->m_blockEventId));
            break;
        }
        case 3: {
            insert_span("ON update {\n", block_format, indent);
            break;
        }
        case 4: {
            insert_span("ON virtual {\n", block_format, indent);
            break;
        }
        default: {
            insert_span_fmt("%*sON UNKNOWN [%d] {\n", block_format, indent, "", block->m_blockType);
            break;
        }
    }

    for (i16 i = 0; i < block->m_trackGroup.m_numTracks; ++i) {
        SsTrack *track_ptr = block->m_trackGroup.m_aTracks + i;
        insert_span_fmt("%*sTRACK %s {\n", {COMMENT_COLOR, 12}, indent + m_options.m_indentPerLevel, "", lookup(track_ptr->m_trackId));
        for (i16 j = 0; j < track_ptr->m_totalLambdaCount; ++j) {
            insert_span("{\n", COMMENT_FMT, indent + m_options.m_indentPerLevel * 2);
            std::unique_ptr<FunctionDisassembly> function = std::make_unique<FunctionDisassembly>(std::move(create_function_disassembly(track_ptr->m_pSsLambda[j].m_pScriptLambda)));
            insert_function_disassembly_text(*function, indent + m_options.m_indentPerLevel * 3);
            m_currentFile->m_functions.push_back(std::move(function));
            insert_span("}\n", COMMENT_FMT, indent + m_options.m_indentPerLevel * 2);
        }
        insert_span("}\n\n", COMMENT_FMT, indent + m_options.m_indentPerLevel);
    }

    insert_span("}\n", COMMENT_FMT, indent);
}

void Disassembler::insert_state_script(const StateScript *stateScript, const u32 indent) {
    TextFormat header_format{COMMENT_COLOR, 14};

    if (stateScript->m_pSsOptions != nullptr && stateScript->m_pSsOptions->m_pSymbolArray != nullptr) {
        SymbolArray *s_array = stateScript->m_pSsOptions->m_pSymbolArray;
        insert_span("OPTIONS: ", header_format, indent);
        for (i32 i = 0; i < s_array->m_numEntries; ++i) {
            insert_span(lookup(s_array->m_pSymbols[i]), header_format, indent + m_options.m_indentPerLevel);
            insert_span("\n");
        }
    }

    SsDeclarationList *decl_table = stateScript->m_pSsDeclList;
    if (decl_table != nullptr) {
        insert_span("DECLARATIONS: \n", header_format, indent);
        for (i32 i = 0; i < decl_table->m_numDeclarations; ++i) {
            SsDeclaration *decl = decl_table->m_pDeclarations + i;
            if (decl->m_isVar) {
                insert_variable(decl, indent + m_options.m_indentPerLevel);
            }
        }
    }
    for (i16 i = 0; i < stateScript->m_stateCount; ++i) {
        SsState *state_ptr = stateScript->m_pSsStateTable + i;
        const char *state_name = lookup(state_ptr->m_stateId);
        insert_span_fmt("%*sSTATE %s {\n", header_format, indent + m_options.m_indentPerLevel, "", state_name);
        for (i64 j = 0; j < state_ptr->m_numSsOnBlocks; ++j) {
            insert_on_block(state_ptr->m_pSsOnBlocks + j, indent + m_options.m_indentPerLevel * 2);
        }
        insert_span_fmt("%*s} END STATE %s\n\n", COMMENT_FMT, indent + m_options.m_indentPerLevel, "", state_name);
    }
}

[[nodiscard]] FunctionDisassembly Disassembler::create_function_disassembly(const ScriptLambda *lambda) {
    Instruction *instructionPtr = reinterpret_cast<Instruction*>(lambda->m_pOpcode);
    const u64 instructionCount = reinterpret_cast<Instruction*>(lambda->m_pSymbols) - instructionPtr;

    std::vector<FunctionDisassemblyLine> lines;
    lines.reserve(instructionCount);

    FunctionDisassembly functionDisassembly {
        std::move(lines),
        StackFrame(),
    };

    functionDisassembly.m_stackFrame.m_symbolTablePtr = lambda->m_pSymbols;

    for (u64 i = 0; i < instructionCount; ++i) {
        functionDisassembly.m_lines.emplace_back(i, instructionPtr);
    }

    b8 counting_args = true;

    for (u64 i = 0; i < instructionCount; ++i) {
        process_instruction(functionDisassembly.m_stackFrame, functionDisassembly.m_lines[i]);
        if (counting_args) {
            if (functionDisassembly.m_lines[i].m_instruction.operand1 >= 49) {
                functionDisassembly.m_stackFrame.m_argCount++;
            } else {
                counting_args = false;
            }
        }
    }
    return functionDisassembly;
}

void Disassembler::process_instruction(StackFrame &stackFrame, FunctionDisassemblyLine &line) {
    constexpr u32 interpreted_buffer_size = 512;
    constexpr u32 disassembly_buffer_size = 256;

    char disassembly_text[disassembly_buffer_size] = {0};
    char interpreted[interpreted_buffer_size] = {0};
    const Instruction istr = line.m_instruction;
    SymbolTableEntry table_entry;
    table_entry.m_type = NONE;
    snprintf(disassembly_text, disassembly_buffer_size, "%04llX   0x%06X   %02X %02X %02X %02X   %-21s",
            line.m_location,
            get_offset((void*)(line.m_globalPointer + line.m_location)),
            istr.opcode,
            istr.destination,
            istr.operand1,
            istr.operand2,
            istr.opcode_to_string()
    );
    char *varying = disassembly_text + strlen(disassembly_text);
    const u32 disassembly_text_size = disassembly_buffer_size - strlen(disassembly_text);

    Register &dest = stackFrame[istr.destination < 128 ? istr.destination : 0];
    Register &op1 = stackFrame[istr.operand1 < 128 ? istr.operand1 : 0];
    Register &op2 = stackFrame[istr.operand2 < 128 ? istr.operand2 : 0];

    char dst_str[interpreted_buffer_size] = {0}; 
    char op1_str[interpreted_buffer_size] = {0}; 
    char op2_str[interpreted_buffer_size] = {0}; 

    stackFrame.to_string(dst_str, interpreted_buffer_size, istr.destination < 128 ? istr.destination : 0, lookup(dest.m_type == R_POINTER ? dest.m_PTR.m_sid : dest.m_SID));
    stackFrame.to_string(op1_str, interpreted_buffer_size, istr.operand1 < 128 ? istr.operand1 : 0, lookup(op1.m_type == R_POINTER ? op1.m_PTR.m_sid : op1.m_SID));
    stackFrame.to_string(op2_str, interpreted_buffer_size, istr.operand2 < 128 ? istr.operand2 : 0, lookup(op2.m_type == R_POINTER ? op2.m_PTR.m_sid : op2.m_SID));

    dest.isReturn = false;
    dest.isArg = false;

    switch (istr.opcode) {
        case Return: {
            snprintf(varying, disassembly_text_size,"r%d", istr.destination);
            snprintf(interpreted, interpreted_buffer_size, "Return %s", dst_str);
            break;
        }
        case IAdd: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            if (op1.m_type == RegisterValueType::R_POINTER) {
                dest.m_type = RegisterValueType::R_POINTER;
                dest.m_PTR = op1.m_PTR;
            } else {
                dest.m_type = RegisterValueType::R_I64;
                dest.m_I64 = op1.m_I64 + op2.m_I64;
            }
            stackFrame.to_string(dst_str, interpreted_buffer_size, istr.destination);
            snprintf(interpreted, interpreted_buffer_size, "%s = %s + %s", dst_str, op1_str, op2_str);
            break;
        }
        case ISub: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = RegisterValueType::R_I64;
            dest.m_I64 = op1.m_I64 - op2.m_I64;
            stackFrame.to_string(dst_str, interpreted_buffer_size, istr.destination);
            snprintf(interpreted, interpreted_buffer_size, "%s = %s - %s", dst_str, op1_str, op2_str);
            break;
        }
        case IMul: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = RegisterValueType::R_I64;
            dest.m_I64 = op1.m_I64 * op2.m_I64;
            stackFrame.to_string(dst_str, interpreted_buffer_size, istr.destination);
            snprintf(interpreted, interpreted_buffer_size, "%s = %s * %s", dst_str, op1_str, op2_str);
            break;
        }
        case IDiv: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = RegisterValueType::R_I64;
            if (op2.m_I64 == 0) {
                dest.m_I64 = op1.m_I64 / 1;
            } else {
                dest.m_I64 = op1.m_I64 / op2.m_I64;
            }
            stackFrame.to_string(dst_str, interpreted_buffer_size, istr.destination);
            snprintf(interpreted, interpreted_buffer_size, "%s = %s / %s", dst_str, op1_str, op2_str);
            break;
        }
        case FAdd: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = RegisterValueType::R_F32;
            dest.m_I64 = op1.m_F32 + op2.m_F32;
            stackFrame.to_string(dst_str, interpreted_buffer_size, istr.destination);
            snprintf(interpreted, interpreted_buffer_size, "%s = %s + %s", dst_str, op1_str, op2_str);
            break;
        }
        case FSub: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = RegisterValueType::R_F32;
            dest.m_I64 = op1.m_F32 - op2.m_F32;
            stackFrame.to_string(dst_str, interpreted_buffer_size, istr.destination);
            snprintf(interpreted, interpreted_buffer_size, "%s = %s - %s", dst_str, op1_str, op2_str);
            break;
        }
        case FMul: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = RegisterValueType::R_F32;
            dest.m_I64 = op1.m_F32 * op2.m_F32;
            stackFrame.to_string(dst_str, interpreted_buffer_size, istr.destination);
            snprintf(interpreted, interpreted_buffer_size, "%s = %s * %s", dst_str, op1_str, op2_str);
            break;
        }
        case FDiv: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = RegisterValueType::R_F32;
            if (op2.m_F32 == 0) {
                dest.m_F32 = op1.m_F32 / 1;
               // snprintf(interpreted + 13, interpreted_buffer_size, "%s", " divide by 0");
            } else {
                dest.m_F32 = op1.m_F32 / op2.m_F32;
            }
            stackFrame.to_string(dst_str, interpreted_buffer_size, istr.destination);
            snprintf(interpreted, interpreted_buffer_size, "%s = %s / %s", dst_str, op1_str, op2_str);
            break;
        }
        case LoadStaticInt: {
            const i64 table_value = reinterpret_cast<i64*>(stackFrame.m_symbolTablePtr)[istr.operand1];
            table_entry.m_type = SymbolTableEntryType::INT;
            table_entry.m_i64 = table_value;
            snprintf(varying, disassembly_text_size,"r%d, %d", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_I64;
            dest.m_I64 = table_value;
            stackFrame.to_string(dst_str, interpreted_buffer_size, istr.destination);
            snprintf(interpreted, interpreted_buffer_size, "%s = ST[%d] -> <%s>", dst_str, istr.operand1, op1_str); 
            break;
        }
        case LoadStaticFloat: {
            const f32 table_value = reinterpret_cast<f32*>(stackFrame.m_symbolTablePtr)[istr.operand1];
            table_entry.m_type = SymbolTableEntryType::FLOAT;
            table_entry.m_f32 = table_value;
            snprintf(varying, disassembly_text_size,"r%d, %d", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_F32;
            dest.m_F32 = table_value;
            stackFrame.to_string(dst_str, interpreted_buffer_size, istr.destination);
            snprintf(interpreted, interpreted_buffer_size, "%s = ST[%d] -> <%s>", dst_str, istr.operand1, op1_str); 
            break;
        }
        case LoadStaticPointer: {
            const p64 table_value = reinterpret_cast<p64*>(stackFrame.m_symbolTablePtr)[istr.operand1];
            table_entry.m_type = SymbolTableEntryType::POINTER;
            table_entry.m_pointer = table_value;
            snprintf(varying, disassembly_text_size,"r%d, %d", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_POINTER;
            stackFrame.to_string(dst_str, interpreted_buffer_size, istr.destination);
            snprintf(interpreted, interpreted_buffer_size, "%s = ST[%d] -> <%s>", dst_str, istr.operand1, op1_str); 
            break;
        }
        case LoadU16Imm: {
            const u16 value = istr.operand1 | (istr.operand2 << 8);
            snprintf(varying, disassembly_text_size,"r%d, %d", istr.destination, value);
            dest.m_type = RegisterValueType::R_U16;
            dest.m_U64 = value;
            snprintf(interpreted, interpreted_buffer_size, "r%d = %d", istr.destination, value);
            break;
        }
        case LoadU32: {
            snprintf(varying, disassembly_text_size,"r%d, [r%d]", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_U32;
            dest.m_I32 = 0;
            snprintf(interpreted, interpreted_buffer_size, "r%d = *(u32*)%s", istr.destination, op1_str);
            break;
        }
        case LoadFloat: {
            snprintf(varying, disassembly_text_size,"r%d, [r%d]", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_F32;
            dest.m_F32 = 0.f;
            snprintf(interpreted, interpreted_buffer_size, "r%d = *(f32*)%s", istr.destination, op1_str);
            break;
        }
        case LoadPointer: {
            snprintf(varying, disassembly_text_size, "r%d, [r%d]", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_POINTER;
            dest.m_PTR = RegisterPointer{op1.m_PTR.m_base, op1.m_PTR.m_offset, op1.m_PTR.m_sid};
            stackFrame.to_string(dst_str, interpreted_buffer_size, istr.operand1, lookup(op1.m_PTR.m_sid));
            snprintf(interpreted, interpreted_buffer_size, "r%d = *(p64*)%s", istr.destination, dst_str);
            break;
        }
        case LoadI64: {
            snprintf(varying, disassembly_text_size,"r%d, [r%d]", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_I64;
            dest.m_I64 = 0;
            snprintf(interpreted, interpreted_buffer_size, "r%d = *(i64*)%s", istr.destination, op1_str);
            break;
        }
        case LoadU64: {
            snprintf(varying, disassembly_text_size,"r%d, [r%d]", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_U64;
            dest.m_U64 = 0;
            snprintf(interpreted, interpreted_buffer_size, "r%d = *(u64*)%s", istr.destination, op1_str);
            break;
        }
        case StoreInt: {
            snprintf(varying, disassembly_text_size,"[r%d], r%d", istr.destination, istr.operand1);
            break;
        }
        case LookupInt: {
            snprintf(varying, disassembly_text_size,"r%d, %d", istr.destination, istr.operand1);
            i64 value = reinterpret_cast<i64*>(stackFrame.m_symbolTablePtr)[istr.operand1]; 
            dest.m_type = RegisterValueType::R_I64;
            dest.m_I64 = value;
            table_entry.m_type = SymbolTableEntryType::POINTER;
            table_entry.m_i64 = value;
            snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%s>", istr.destination, istr.operand1, lookup(value));
            break;
        }
        case LookupFloat: {
            snprintf(varying, disassembly_text_size,"r%d, %d", istr.destination, istr.operand1);
            f32 value = *reinterpret_cast<f32*>(stackFrame.m_symbolTablePtr + istr.operand1); 
            dest.m_type = RegisterValueType::R_F32;
            dest.m_F32 = value;
            table_entry.m_type = SymbolTableEntryType::FLOAT;
            table_entry.m_f32 = value;
            snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%.2f>", istr.destination, istr.operand1, value);
            break;
        }
        case LookupPointer: {
            snprintf(varying, disassembly_text_size,"r%d, %d", istr.destination, istr.operand1);
            const p64 value = reinterpret_cast<p64*>(stackFrame.m_symbolTablePtr)[istr.operand1];
            dest.m_type = RegisterValueType::R_POINTER;
            dest.m_PTR.m_base = 0;
            dest.m_PTR.m_offset = 0;
            dest.m_PTR.m_sid = value;
            table_entry.m_type = SymbolTableEntryType::POINTER;
            table_entry.m_pointer = value;
            if (m_currentFile->is_file_ptr(reinterpret_cast<p64>(stackFrame.m_symbolTablePtr + istr.operand1))) {
               snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%s>", istr.destination, istr.operand1, reinterpret_cast<const char*>(value));
            } else {
                snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%s>", istr.destination, istr.operand1, lookup(value));
            }
            break;
        }
        case MoveInt: {
            snprintf(varying, disassembly_text_size,"r%d, %d", istr.destination, istr.operand1);
            dest.m_type = R_I64;
            dest.m_I64 = op1.m_I64;
            snprintf(interpreted, interpreted_buffer_size, "r%d = r%d <%lli>", istr.destination, istr.operand1, op1.m_I64);
            break;
        }
        case MoveFloat: {
            snprintf(varying, disassembly_text_size,"r%d, %d", istr.destination, istr.operand1);
            dest.m_type = R_F32;
            dest.m_F32 = op1.m_F32;
            snprintf(interpreted, interpreted_buffer_size, "r%d = r%d <%f>", istr.destination, istr.operand1, op1.m_F32);
            break;
        }
        case MovePointer: {
            snprintf(varying, disassembly_text_size,"r%d, %d", istr.destination, istr.operand1);
            dest.m_type = R_POINTER;
            dest.m_PTR = op1.m_PTR;
            snprintf(interpreted, interpreted_buffer_size, "r%d = r%d <%s>", istr.destination, istr.operand1, lookup(op1.m_PTR.get()));
            break;
        }
        case CastInteger: {
            snprintf(varying, disassembly_text_size,"r%d, r%d", istr.destination, istr.operand1);
            dest.m_type = R_I32;
            dest.m_I32 = (i32)op1.m_F32;
            snprintf(interpreted, interpreted_buffer_size, "r%d = int(r%d) -> <%f> => <%d>", istr.destination, istr.operand1, op1.m_F32, dest.m_I32);
            break;
        }
        case CastFloat: {
            snprintf(varying, disassembly_text_size,"r%d, r%d", istr.destination, istr.operand1);
            dest.m_type = R_F32;
            dest.m_F32 = (f32)op1.m_I32;
            snprintf(interpreted, interpreted_buffer_size, "r%d = float(r%d) -> <%d> => <%f>", istr.destination, istr.operand1, op1.m_I32, dest.m_F32);
            break;
        }
        case Call: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, %d", istr.destination, istr.operand1, istr.operand2);
            char comment_str[200] = {0};
            snprintf(comment_str, sizeof(comment_str), "r%d = %s(", istr.destination, lookup(op1.m_PTR.m_sid));
            for (u64 i = 0; i < istr.operand2; ++i) {
                if (i != 0) {
                    strncat(comment_str, ", ", sizeof(comment_str) - strlen(comment_str));
                }
                stackFrame.to_string(dst_str, interpreted_buffer_size, 49 + i, lookup(stackFrame[i + 49].m_SID));
                strncat(comment_str, dst_str, sizeof(comment_str) - strlen(comment_str));
            }
            dest.m_type = R_POINTER;
            dest.isReturn = true;
            dest.m_PTR = {0, 0, op1.m_PTR.m_sid};
            snprintf(interpreted, interpreted_buffer_size, "%s)", comment_str);
            break;
        }
        case CallFf: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, %d", istr.destination, istr.operand1, istr.operand2);
            char comment_str[200] = {0};
            snprintf(comment_str, sizeof(comment_str), "r%d = %s(", istr.destination, lookup(op1.m_PTR.m_sid));
            for (u64 i = 0; i < istr.operand2; ++i) {
                if (i != 0) {
                    strncat(comment_str, ", ", sizeof(comment_str) - strlen(comment_str));
                }
                stackFrame.to_string(dst_str, interpreted_buffer_size, 49 + i, lookup(stackFrame[i + 49].m_type == R_POINTER ? stackFrame[i + 49].m_PTR.m_sid : stackFrame[i + 49].m_SID));
                strncat(comment_str, dst_str, sizeof(comment_str) - strlen(comment_str));
            }
            dest.m_type = R_POINTER;
            dest.isReturn = true;
            snprintf(interpreted, interpreted_buffer_size, "%s)", comment_str);
            break;
        }
        case IEqual: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_I64 == op2.m_I64;
            snprintf(interpreted, interpreted_buffer_size, "r%d = %s == %s", 
                istr.destination, 
                op1_str,
                op2_str
            );
            break;
        }
        case IGreaterThan: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_I64 > op2.m_I64;
            snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%lld] > r%d [%lld]", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_I64, 
                istr.operand2, 
                stackFrame[istr.operand2].m_I64
            );
            break;
        }
        case IGreaterThanEqual: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_I64 >= op2.m_I64;
            snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%lli] >= r%d [%lli]", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_I64, 
                istr.operand2, 
                stackFrame[istr.operand2].m_I64
            );
            break;
        }
        case ILessThan: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_I64 < op2.m_I64;
            snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%s] < r%d [%s]", 
                istr.destination, 
                istr.operand1, 
                op1_str, 
                istr.operand2, 
                op2_str
            );
            break;
        }
        case ILessThanEqual: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_I64 <= op2.m_I64;
            snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%lli] <= r%d [%lli]", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_I64, 
                istr.operand2, 
                stackFrame[istr.operand2].m_I64
            );
            break;
        }
        case FEqual: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_F32 == op2.m_F32;
            snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%f] == r%d [%f]", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_F32, 
                istr.operand2, 
                stackFrame[istr.operand2].m_F32
            );
            break;
        }
        case FGreaterThan: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_F32 > op2.m_F32;
            snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%f] > r%d [%f]", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_F32, 
                istr.operand2, 
                stackFrame[istr.operand2].m_F32
            );
            break;
        }
        case FGreaterThanEqual: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_F32 >= op2.m_F32;
            snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%f] >= r%d [%f]", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_F32, 
                istr.operand2, 
                stackFrame[istr.operand2].m_F32
            );
            break;
        }
        case FLessThan: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_F32 < op2.m_F32;
            snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%f] < r%d [%f]", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_F32, 
                istr.operand2, 
                stackFrame[istr.operand2].m_F32
            );
            break;
        }
        case FLessThanEqual: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_F32 <= op2.m_F32;
            snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%f] <= r%d [%f]", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_F32, 
                istr.operand2, 
                stackFrame[istr.operand2].m_F32
            );
            break;
        }
        case IMod: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_I32;
            if (op2.m_I64 == 0) {
                dest.m_I32 = op1.m_I64 % 1;
            } else {
                dest.m_I32 = op1.m_I64 % op2.m_I64;
            }
            snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%lli] %% r%d [%lli] -> <%d>", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_I64, 
                istr.operand2, 
                stackFrame[istr.operand2].m_I64,
                dest.m_I32
            );
            break;
        }
        case FMod: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_F32;
            if (op2.m_F32 == 0.f) {
                dest.m_F32 = fmodf(op1.m_F32, 1.f);
            } else {
                dest.m_F32 = fmodf(op1.m_F32, op2.m_F32);
            }
            snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%f] %% r%d [%f] -> <%f>", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_F32, 
                istr.operand2, 
                stackFrame[istr.operand2].m_F32,
                dest.m_F32
            );
            break;
        }
        case IAbs: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_I32;
            dest.m_I64 = abs(op1.m_I64);
            snprintf(interpreted, interpreted_buffer_size, "r%d = ABS(r%d) [%lli] -> <%lli>", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_I64,
                dest.m_I64
            );
            break;
        }
        case FAbs: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_I32;
            dest.m_F32 = abs(op1.m_F32);
            snprintf(interpreted, interpreted_buffer_size, "r%d = ABS(r%d) [%f] -> <%f>", 
                istr.destination, 
                istr.operand1,
                stackFrame[istr.operand1].m_F32,
                dest.m_F32
            );
            break;
        }
        case Branch: {
            u32 target = istr.destination | (istr.operand2 << 8);
            snprintf(varying, disassembly_text_size,"0x%X", target);
            snprintf(interpreted, interpreted_buffer_size, "GOTO ");
            stackFrame.add_target_label(target);
            line.m_label = target;
            break;
        }
        case BranchIf: {
            u32 target = istr.destination | (istr.operand2 << 8);
            snprintf(varying, disassembly_text_size,"r%d, 0x%X", istr.operand1, target);
            snprintf(interpreted, interpreted_buffer_size, "IF r%d [%s] ", istr.operand1, op1_str);
            line.m_label = target;
            stackFrame.add_target_label(target);
            break;
        }
        case BranchIfNot: {
            u32 target = istr.destination | (istr.operand2 << 8);
            snprintf(varying, disassembly_text_size,"r%d, 0x%X", istr.operand1, target);
            snprintf(interpreted, interpreted_buffer_size, "IF NOT r%d [%s] ", istr.operand1, op1_str);
            line.m_label = target;
            stackFrame.add_target_label(target);
            break;
        }
        case OpLogNot: {
            snprintf(varying, disassembly_text_size,"r%d, r%d", istr.destination, istr.operand1);
            dest.m_type = R_BOOL;
            //dest.m_BOOL = !op1.m_BOOL;
            snprintf(interpreted, interpreted_buffer_size, "r%d = !%s", istr.destination, op1_str);
            break;
        }
        case OpBitAnd: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = op1.m_type;
            dest.m_U64 = op1.m_U64 & op2.m_U64;
            snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%llu] & r%d [%llu] -> <%llu>", istr.destination, istr.operand1, op1.m_U64, istr.operand2, op2.m_U64, dest.m_U64);
            break;
        }
        case OpBitNot: {
            snprintf(varying, disassembly_text_size,"r%d, r%d", istr.destination, istr.operand1);
            dest.m_type = op1.m_type;
            dest.m_U64 = ~op1.m_U64;
            snprintf(interpreted, interpreted_buffer_size, "r%d = ~r%d [%llu] -> <%llu>", istr.destination, istr.operand1, op1.m_U64, dest.m_U64);
            break;
        }
        case OpBitOr: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = op1.m_type;
            dest.m_U64 = op1.m_U64 | op2.m_U64;
            snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%llu] | r%d [%llu] -> <%llu>", istr.destination, istr.operand1, op1.m_U64, istr.operand2, op2.m_U64, dest.m_U64);
            break;
        }
        case OpBitXor: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = op1.m_type;
            dest.m_U64 = op1.m_U64 ^ op2.m_U64;
            snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%llu] ^ r%d [%llu] -> <%llu>", istr.destination, istr.operand1, op1.m_U64, istr.operand2, op2.m_U64, dest.m_U64);
            break;
        }
        case OpBitNor: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = op1.m_type;
            dest.m_U64 = ~(op1.m_U64 | op2.m_U64);
            snprintf(interpreted, interpreted_buffer_size, "r%d = ~(r%d [%llu] | r%d [%llu]) -> <%llu>", istr.destination, istr.operand1, op1.m_U64, istr.operand2, op2.m_U64, dest.m_U64);
            break;
        }
        case OpLogAnd: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_BOOL && op2.m_BOOL;
            snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%d] && r%d [%d]", istr.destination, istr.operand1, op1.m_BOOL, istr.operand2, op2.m_BOOL);
            break;
        } 
        case OpLogOr: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_BOOL || op2.m_BOOL;
            snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%d] || r%d [%d]", istr.destination, istr.operand1, op1.m_BOOL, istr.operand2, op2.m_BOOL);
            break;
        }
        case INeg: {
            snprintf(varying, disassembly_text_size,"r%d, r%d", istr.destination, istr.operand1);
            dest.m_type = R_I64;
            dest.m_I64 = -op1.m_I64;
            snprintf(interpreted, interpreted_buffer_size, "r%d = -r%d [%lli] -> <%lli>", istr.destination, istr.operand1, op1.m_I64, dest.m_I64);
            break;
        }
        case FNeg: {
            snprintf(varying, disassembly_text_size,"r%d, r%d", istr.destination, istr.operand1);
            dest.m_type = R_F32;
            dest.m_F32 = -op1.m_F32;
            snprintf(interpreted, interpreted_buffer_size, "r%d = -r%d [%f] -> <%f>", istr.destination, istr.operand1, op1.m_F32, dest.m_F32);
            break;
        }
        case LoadParamCnt: {
            // wat
            break;
        }
        case IAddImm: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, %d", istr.destination, istr.operand1, istr.operand2);
            const char* resolved = nullptr;
            if (op1.m_type == R_POINTER) {
                dest.m_type = R_POINTER;
                dest.m_PTR = {op1.m_PTR.m_base, op1.m_PTR.m_offset + istr.operand2, op1.m_PTR.m_sid};
                resolved = lookup(dest.m_PTR.m_sid);
            } else {
                dest.m_type = R_I64;
                dest.m_I64 = op1.m_I64 + istr.operand2;
            }
            stackFrame.to_string(dst_str, interpreted_buffer_size, istr.operand1, resolved);
            snprintf(interpreted, interpreted_buffer_size, "r%d = %s + %d -> <%s>", istr.destination, op1_str, istr.operand2, dst_str);
            break;
        }
        case ISubImm: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, %d", istr.destination, istr.operand1, istr.operand2);
            i64 value = op1.m_I64 - istr.operand2;
            dest.m_type = R_I64;
            dest.m_I64 = value;
            snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%lli] - %d -> <%lli>", istr.destination, istr.operand1, op1.m_I64, istr.operand2, dest.m_I64);
            break;
        }
        case IMulImm: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, %d", istr.destination, istr.operand1, istr.operand2);
            i64 value = op1.m_I64 * istr.operand2;
            dest.m_type = R_I64;
            dest.m_I64 = value;
            snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%lli] * %d -> <%lli>", istr.destination, istr.operand1, op1.m_I64, istr.operand2, dest.m_I64);
            break;
        }
        case IDivImm: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, %d", istr.destination, istr.operand1, istr.operand2);
            i64 value;
            if (istr.operand2 == 0) {
                value = op1.m_I64 / 1;
            } else {
                value = op1.m_I64 / istr.operand2;
            }
            dest.m_type = R_I64;
            dest.m_I64 = value;
            snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%lli] / %d -> <%lli>", istr.destination, istr.operand1, op1.m_I64, istr.operand2, dest.m_I64);
            break;
        }
        case LoadStaticI32Imm: {
            snprintf(varying, disassembly_text_size,"r%d, %d", istr.destination, istr.operand1);
            i32 value = *reinterpret_cast<i32*>(stackFrame.m_symbolTablePtr + istr.operand1); 
            dest.m_type = RegisterValueType::R_I32;
            dest.m_I32 = value;
            table_entry.m_type = SymbolTableEntryType::INT;
            table_entry.m_i64 = value;
            snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%d>", istr.destination, istr.operand1, value);
            break;
        }
        case LoadStaticFloatImm: {
            snprintf(varying, disassembly_text_size,"r%d, %d", istr.destination, istr.operand1);
            f32 value = *reinterpret_cast<f32*>(stackFrame.m_symbolTablePtr + istr.operand1); 
            dest.m_type = RegisterValueType::R_F32;
            dest.m_F32 = value;
            table_entry.m_type = SymbolTableEntryType::FLOAT;
            table_entry.m_f32 = value;
            snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%.2f>", istr.destination, istr.operand1, value);
            break;
        }
        case LoadStaticPointerImm: {
            snprintf(varying, disassembly_text_size,"r%d, %d", istr.destination, istr.operand1);
            p64 value = reinterpret_cast<p64*>(stackFrame.m_symbolTablePtr)[istr.operand1]; 
            if (value >= m_currentFile->m_stringsPtr) {
                snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> \"%s\"", istr.destination, istr.operand1, reinterpret_cast<const char*>(value));
                dest.m_type = RegisterValueType::R_STRING;
                dest.m_PTR = {value, 0, 0};
                table_entry.m_type = SymbolTableEntryType::STRING;
                table_entry.m_pointer = value;
            } else {
                snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <0x%d>", istr.destination, istr.operand1, get_offset((void*)value));
                dest.m_type = RegisterValueType::R_POINTER;
                dest.m_PTR = {0, 0, value};
                table_entry.m_type = SymbolTableEntryType::POINTER;
                table_entry.m_pointer = value;
            }
            break;
        }
        case LoadStaticI64Imm: {
            snprintf(varying, disassembly_text_size,"r%d, %d", istr.destination, istr.operand1);
            i64 value = reinterpret_cast<i64*>(stackFrame.m_symbolTablePtr)[istr.operand1]; 
            dest.m_type = RegisterValueType::R_I64;
            dest.m_I64 = value;
            table_entry.m_type = SymbolTableEntryType::INT;
            table_entry.m_i64 = value;
            snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%llu>", istr.destination, istr.operand1, value);
            break;
        }
        case LoadStaticU64Imm: {
            snprintf(varying, disassembly_text_size,"r%d, %d", istr.destination, istr.operand1);
            u64 value = reinterpret_cast<u64*>(stackFrame.m_symbolTablePtr)[istr.operand1];
            if (value >= 0x000FFFFFFFFFFFFF) {
                const char *hash_str = lookup(value);
                dest.m_type = RegisterValueType::R_HASH;
                dest.m_SID = value;
                table_entry.m_type = SymbolTableEntryType::STRINGID_64;
                table_entry.m_hash = value;
                stackFrame.to_string(dst_str, interpreted_buffer_size, istr.destination, hash_str);
                snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%s>", istr.destination, istr.operand1, dst_str);
            } else if (m_currentFile->is_file_ptr(reinterpret_cast<p64>(stackFrame.m_symbolTablePtr + istr.operand1))) {
                printf("here");
            }
            break;
        }
        case IntAsh: {
            snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);

            u64 hash = op1.m_I64 >> -(char)op2.m_I8;
            if (op2.m_I64 == 0) {
                hash = op1.m_I64 << op2.m_I8;
            }
            dest.m_type = RegisterValueType::R_I64;
            dest.m_I64 = hash;
            snprintf(interpreted, interpreted_buffer_size, "r%d = %s <<>> %s", istr.destination, op1_str, op2_str);
            break;
        }
        case Move: {
            snprintf(varying, disassembly_text_size,"r%d, r%d", istr.destination, istr.operand1);

            dest = op1;
            
            snprintf(interpreted, interpreted_buffer_size, "r%d = %s", istr.destination, op1_str);
            break;
        }
        case LoadStaticU32Imm: {
            snprintf(varying, disassembly_text_size, "r%d, r%d", istr.destination, istr.operand1);
            const u32 value = reinterpret_cast<u32*>(stackFrame.m_symbolTablePtr)[istr.operand1];
            dest.m_type = RegisterValueType::R_U32;
            dest.m_U32 = value;
            snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%d>", istr.destination, istr.operand1, value);
            break;
        }
        case LoadStaticI8Imm: {
            snprintf(varying, disassembly_text_size, "r%d, r%d", istr.destination, istr.operand1);
            const i8 value = reinterpret_cast<i8*>(stackFrame.m_symbolTablePtr)[istr.operand1];
            dest.m_type = RegisterValueType::R_I8;
            dest.m_I8 = value;
            snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%d>", istr.destination, istr.operand1, value);
            break;
        }
        case LoadStaticI16Imm: {
            snprintf(varying, disassembly_text_size, "r%d, r%d", istr.destination, istr.operand1);
            const i16 value = reinterpret_cast<i16*>(stackFrame.m_symbolTablePtr)[istr.operand1];
            dest.m_type = RegisterValueType::R_I16;
            dest.m_I16 = value;
            snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%d>", istr.destination, istr.operand1, value);
            break;
        }
        case LoadStaticU16Imm: {
            snprintf(varying, disassembly_text_size, "r%d, r%d", istr.destination, istr.operand1);
            const u16 value = reinterpret_cast<u16*>(stackFrame.m_symbolTablePtr)[istr.operand1];
            dest.m_type = RegisterValueType::R_U16;
            dest.m_U16 = value;
            snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%d>", istr.destination, istr.operand1, value);
            break;
        }
        case LoadI8: {
            snprintf(varying, disassembly_text_size, "r%d, r%d", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_I8;
            dest.m_I8 = 0;
            snprintf(interpreted, interpreted_buffer_size, "r%d = *(i8*)%s", istr.destination, op1_str);
            break;
        }
        case LoadU8: {
            snprintf(varying, disassembly_text_size, "r%d, r%d", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_U8;
            dest.m_U8 = 0;
            snprintf(interpreted, interpreted_buffer_size, "r%d = *(u8*)%s", istr.destination, op1_str);
            break;
        }
        case LoadI16: {
            snprintf(varying, disassembly_text_size, "r%d, r%d", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_I16;
            dest.m_I16 = 0;
            snprintf(interpreted, interpreted_buffer_size, "r%d = *(i16*)%s", istr.destination, op1_str);
            break;
        }
        case LoadU16: {
            snprintf(varying, disassembly_text_size, "r%d, r%d", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_U16;
            dest.m_U16 = 0;
            snprintf(interpreted, interpreted_buffer_size, "r%d = *(u16*)%s", istr.destination, op1_str);
            break;
        }
        case LoadI32: {
            snprintf(varying, disassembly_text_size, "r%d, r%d", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_I32;
            dest.m_I32 = 0;
            snprintf(interpreted, interpreted_buffer_size, "r%d = *(i32*)%s", istr.destination, op1_str);
            break;
        }
        case StoreI8: {
            snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            snprintf(interpreted, interpreted_buffer_size, "r%d, *(i8*)%s = %s", istr.destination, op1_str, op2_str);
            dest.m_type = RegisterValueType::R_I8;
            dest.m_I8 = 0;
            break;
        }
        case StoreU8: {
            snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            snprintf(interpreted, interpreted_buffer_size, "r%d, *(u8*)%s = %s", istr.destination, op1_str, op2_str);
            dest.m_type = RegisterValueType::R_U8;
            dest.m_U8 = 0;
            break;
        }
        case StoreI16: {
            snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            snprintf(interpreted, interpreted_buffer_size, "r%d, *(i16*)%s = %s", istr.destination, op1_str, op2_str);
            dest.m_type = RegisterValueType::R_I16;
            dest.m_I16 = 0;
            break;
        }
        case StoreU16: {
            snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            snprintf(interpreted, interpreted_buffer_size, "r%d, *(u16*)%s = %s", istr.destination, op1_str, op2_str);
            dest.m_type = RegisterValueType::R_U16;
            dest.m_U16 = 0;
            break;
        }
        case StoreI32: {
            snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            snprintf(interpreted, interpreted_buffer_size, "r%d, *(i32*)%s = %s", istr.destination, op1_str, op2_str);
            dest.m_type = RegisterValueType::R_I32;
            dest.m_I32 = 0;
            break;
        }
        case StoreU32: {
            snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            snprintf(interpreted, interpreted_buffer_size, "r%d, *(u32*)%s = %s", istr.destination, op1_str, op2_str);
            dest.m_type = RegisterValueType::R_U32;
            dest.m_U32 = 0;
            break;
        }
        case StoreI64: {
            snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            snprintf(interpreted, interpreted_buffer_size, "r%d, *(i64*)%s = %s", istr.destination, op1_str, op2_str);
            dest.m_type = RegisterValueType::R_I64;
            dest.m_I64 = 0;
            break;
        }
        case StoreU64: {
            snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            snprintf(interpreted, interpreted_buffer_size, "r%d, *(u64*)%s = %s", istr.destination, op1_str, op2_str);
            dest.m_type = RegisterValueType::R_U64;
            dest.m_U64 = 0;
            break;
        }
        case INotEqual:
        case FNotEqual: {
            snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = RegisterValueType::R_BOOL;
            dest.m_BOOL = op1.m_U64 != op2.m_U64;
            snprintf(interpreted, interpreted_buffer_size, "r%d = %s != %s", istr.destination, op1_str, op2_str);
            break;
        }
        case StoreArray: {
            // Not used.
        }
        case AssertPointer: {
            snprintf(varying, disassembly_text_size,"r%d", istr.destination);
            snprintf(interpreted, interpreted_buffer_size, "r%d?", istr.destination);
            break;
        }
        default: {
            snprintf(varying, disassembly_text_size, "???");
            snprintf(interpreted, interpreted_buffer_size, "UNKNOWN INSTRUCTION");
            break;
        }
    }
    
    if (table_entry.m_type != NONE) {
        stackFrame.m_symbolTable.emplace(istr.operand1, table_entry);
    }
    line.m_text = std::string(disassembly_text);
    line.m_comment = std::string(interpreted);
}

void Disassembler::insert_label(const std::vector<u32> &labels, const FunctionDisassemblyLine &line, const u32 func_size, const u32 indent) noexcept {
    constexpr TextFormat label_format = {COMMENT_COLOR, 14};
    auto label_location = std::find(labels.begin(), labels.end(), line.m_location);
    if (label_location != labels.end()) {
        const u32 label_index = std::distance(labels.begin(), label_location);
        if (line.m_location == func_size) {
            insert_span("L_RETURN:\n", label_format, indent - m_options.m_indentPerLevel);
        } else if (line.m_location == func_size - 1 && line.m_instruction.opcode == Opcode::LoadU16Imm) {
            insert_span_fmt("%*sL_RETURN_%d:\n", label_format, indent - m_options.m_indentPerLevel, "", line.m_instruction.operand1);  
        } else {
            insert_span_fmt("%*sL_%d:\n", label_format, indent - m_options.m_indentPerLevel, "", label_index);  
        }
    }
}

void Disassembler::insert_goto_label(const std::vector<u32> &labels, const FunctionDisassemblyLine &line, const u32 func_size, const std::vector<FunctionDisassemblyLine> &lines) noexcept {
    if (line.m_label != -1) {
        u32 target = std::distance(labels.begin(), std::find(labels.begin(), labels.end(), line.m_label));
        if (line.m_label == func_size) {
            insert_span("=> L_RETURN", COMMENT_FMT);
        } else if (line.m_label == func_size - 1 && lines[line.m_label].m_instruction.opcode == Opcode::LoadU16Imm) {
            insert_span_fmt("=> L_RETURN_%d", COMMENT_FMT, lines[line.m_label].m_instruction.operand1);
        } else {
            insert_span_fmt("=> L_%d", COMMENT_FMT, target);
        }
    }
}

void Disassembler::insert_function_disassembly_text(const FunctionDisassembly &functionDisassembly, const u32 indent) {
    auto labels = functionDisassembly.m_stackFrame.m_labels;
    char buffer[512] = {0};
    constexpr TextFormat text_format = {OPCODE_COLOR, 12};
    
    if (functionDisassembly.m_stackFrame.m_argCount > 0) {
        insert_span_fmt("%*s[%d args]\n", COMMENT_FMT, indent, "", functionDisassembly.m_stackFrame.m_argCount);
    }
    
    for (const auto &line : functionDisassembly.m_lines) {
        u32 line_offset = std::max(67ull - line.m_text.length(), 0ull);
        insert_label(labels, line, functionDisassembly.m_lines.size() - 1, indent);
        insert_span(line.m_text.c_str(), text_format, indent);
        const std::string comment = std::string(line_offset, ' ') + line.m_comment;
        insert_span(comment.c_str(), {STRING_COLOR, 12});
        insert_goto_label(labels, line, functionDisassembly.m_lines.size() - 1, functionDisassembly.m_lines);
        insert_span("\n");
    }
    insert_span("\n");
    insert_span("SYMBOL TABLE: \n", {COMMENT_COLOR, 12}, indent);

    char line_start[64] = {0};
    char type[512] = {0};

    u64 *table_ptr = functionDisassembly.m_stackFrame.m_symbolTablePtr;

    for (const auto &[i, entry] : functionDisassembly.m_stackFrame.m_symbolTable) {
        snprintf(line_start, sizeof(line_start), "%04X   0x%06X   ", i, get_offset(table_ptr + i));
        switch (entry.m_type) {
            case SymbolTableEntryType::FLOAT: {
                snprintf(type, sizeof(type), "float: <%f>\n", entry.m_f32);
                break;
            }
            case SymbolTableEntryType::INT: {
                snprintf(type, sizeof(type), "int: <%lli>\n", entry.m_i64);
                break;
            }
            case SymbolTableEntryType::STRING: {
                snprintf(type, sizeof(type), "string: \"%s\"\n", reinterpret_cast<const char*>(entry.m_pointer));
                break;
            }
            case SymbolTableEntryType::POINTER:{
                snprintf(type, sizeof(type), "pointer: <%s>\n", lookup(entry.m_pointer));
                break;
            }
            case SymbolTableEntryType::STRINGID_64: {
                snprintf(type, sizeof(type), "sid: <%s>\n", lookup(entry.m_hash));
                break;
            }
            case SymbolTableEntryType::NONE:
            case SymbolTableEntryType::UNKNOWN_TYPE: {
                snprintf(type, sizeof(type), "UNKNOWN: <%llX>", entry.m_i64);
                break;
            } 
        }
        snprintf(buffer, sizeof(buffer), "%s %s", line_start, type);
        insert_span(buffer, {COMMENT_COLOR, 12}, indent);
    }
}

void Disassembler::insert_header_line() {
    const char* current_script_name;
    const char* current_script_id;
    const TextFormat header_format = {STRING_COLOR, 14};
    if (m_currentFile->m_dcscript != nullptr) {
        current_script_name = lookup(m_currentFile->m_dcscript->m_stateScriptId);
        current_script_id = int_to_string_id(m_currentFile->m_dcscript->m_stateScriptId).c_str();
    } else {
        current_script_name = "UNKNOWN SCRIPT";
        current_script_id = "UNKNOWN SCRIPT ID";
    }
    insert_span_fmt("%*sDeepQuantum's DC Disassembler ver. %d\n", header_format, 14, "", m_versionNumber);
    insert_span_fmt("%*sListing for script: %s\n", header_format, 14, "", current_script_name);
    insert_span_fmt("%*sScript ID: %s\n", header_format, 14, "", current_script_id);
    insert_span_fmt("%*sFilesize: %d bytes\n", header_format, 14, "", m_currentFile->m_size);
    insert_span("START OF DISASSEMBLY\n", header_format, 14);
    insert_span("--------------------------------------------------\n", header_format, 14);
}