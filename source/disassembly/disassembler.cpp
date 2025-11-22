#include "disassembly/custom_structs.h"
#include <cmath>
#include <chrono>
#include "disassembly/disassembler.h"
#include <cstring>
#include <fstream>
#include <sstream>

static constexpr char ENTRY_SEP[] = "##############################";


namespace dconstruct {
[[nodiscard]] const char *Disassembler::lookup(const sid64 sid) {
    auto res = m_currentFile->m_sidCache.find(sid);
    if (res != m_currentFile->m_sidCache.end()) {
        return res->second.c_str();
    }

    const char *hash_string = m_sidbase->search(sid);
    if (hash_string == nullptr) {
        auto [iter, inserted] = m_currentFile->m_sidCache.emplace(sid, int_to_string_id(sid));
        hash_string = iter->second.c_str();
    } else {
        m_currentFile->m_sidCache.emplace(sid, hash_string);
    }
    return hash_string;
}


template<TextFormat text_format, typename... Args>
void Disassembler::insert_span_fmt(const char *format, Args ...args) {
    char buffer[512];
    std::snprintf(buffer, sizeof(buffer), format, args...);
    insert_span(buffer, 0, text_format);
}

template<TextFormat text_format, typename... Args>
void Disassembler::insert_span_indent(const char* format, const u32 indent, Args ...args) {
    char buffer[512];
    std::snprintf(buffer, sizeof(buffer), format, indent, "", args...);
    insert_span(buffer, 0, text_format);
}

[[nodiscard]] bool Disassembler::is_unmapped_sid(const location loc) const noexcept {
    const bool in_range = loc.get<sid64>() >= m_sidbase->m_lowestSid && loc.get<sid64>() <= m_sidbase->m_highestSid;
    const bool is_fileptr = m_currentFile->is_file_ptr(loc);
    return loc.is_aligned() && in_range && !is_fileptr;
}

[[nodiscard]] bool Disassembler::is_possible_float(const f32 *val) const noexcept {
    f32 rounded = std::roundf(*val * 1e4f) / 1e4f;
    return std::fabsf(*val - rounded) < 1e-4f && *val > -1e4f && *val < 1e4f && rounded != 0.f;
}

[[nodiscard]] bool Disassembler::is_possible_i32(const i32 *val) const noexcept {
    return std::abs(*val) < 50000;
}

/*
    a "struct" is a piece of data that has a type ID and members following it.
    an "array-like" is essentially just a size + pointer. the thing being pointed at can be anything, either proper structs
    or more array-likes. array-likes may also point at array-structs, which are just blocks of data with the type ID "array"
    followed by structs of undertermined size. array structs don't provide their own size, so the array-like must provide the size.
    array structs may contain arbitrarily sized members though, so we must check each member individually.

*/  
u8 Disassembler::insert_struct_or_arraylike(const location struct_location, const u32 indent) {
    u8 bytes_inserted = 0;
    if (m_currentFile->is_file_ptr(struct_location)) {
        if (m_currentFile->is_string(location().from(struct_location))) {
            insert_span_fmt("string: \"%s\"\n", struct_location.get<char*>());
            bytes_inserted = 8;
            return bytes_inserted;
        }
        const location next_struct_header = location().from(struct_location, -8);
        if (!next_struct_header.is_aligned() || !is_unmapped_sid(next_struct_header)) {
            insert_anonymous_array(struct_location, indent);
        } else if (next_struct_header.get<sid64>() == SID("array")) {
            insert_array(struct_location, get_size_array(struct_location, indent), indent);
        } else {
            insert_struct(next_struct_header.as<structs::unmapped>(), indent);
        }
        bytes_inserted = 8;
    }
    else {
        if (struct_location >= m_currentFile->m_strings) {
            insert_span_fmt("string: \"%s\"\n", struct_location.get<char*>());
            bytes_inserted = 8;
        }
        else {
            bytes_inserted = insert_next_struct_member(struct_location, indent);
        }
    }
    return bytes_inserted;
}

void Disassembler::insert_anonymous_array(const location anon_array, const u32 indent) {
    const u32 anonymous_array_size = anon_array.get<u32>(8);
    if (anonymous_array_size > 1024) {
        insert_span_fmt("anonymous array with invalid size %u, returning\n", anonymous_array_size);
        return;
    }

    insert_span_fmt("anonymous array [0x%x] {size: %u} {\n", get_offset(anon_array), anonymous_array_size);
    insert_array(anon_array, anonymous_array_size, indent);
}

[[nodiscard]] u32 Disassembler::get_size_array(const location array, const u32 indent) {
    u32 size_array_front = array.get<u32>(8);
    u32 size_array_back = array.get<u32>(-8);
    u32 size_array;
    constexpr u32 max_allowed_size_array = 512;
    if (size_array_front > max_allowed_size_array) {
        if (size_array_back > max_allowed_size_array) {
            insert_span_fmt("array [0x%x] {size: (%u, %u) (*invalid*)} {", 
                get_offset(array), 
                size_array_front,
                size_array_back
            );
            return 0;
        } else {
            size_array = size_array_back;
        }
    } else {
        size_array = size_array_front;
    }
    insert_span_fmt("array [0x%x] {size: %d} {\n", get_offset(array), size_array);
    return size_array;
}

void Disassembler::insert_array(const location array, const u32 array_size, const u32 indent) {
    
    if (array_size == 0) {
        insert_span("}\n", indent);
        return;
    }

    u32 member_offset = 8;
    u32 member_count = 0;
    const location member = location().from(array);

    while (!m_currentFile->is_string(member + member_offset) && !m_currentFile->gets_pointed_at(member + member_offset)) {
        member_offset += 8;
    }

    const u8 type_id_padding = m_currentFile->is_string(member + member_offset) ? 0 : 8;
    u32 struct_size = (member_offset - type_id_padding) / array_size;

    for (u32 array_entry_count = 0; array_entry_count < array_size; ++array_entry_count) {
        member_offset = member_count = 0;
        insert_span_indent("%*s[%u] anonymous struct [0x%x] {\n", 
            indent + m_options.m_indentPerLevel, 
            array_entry_count,
            get_offset(member + array_entry_count * struct_size)
        );

        while (member_offset < struct_size) {
            insert_span_indent("%*s[%d] ", indent + m_options.m_indentPerLevel * 2, member_count++);
            const location current_member_location = member + (array_entry_count * struct_size + member_offset);
            const u32 last_member_size = insert_struct_or_arraylike(current_member_location.aligned(), indent + m_options.m_indentPerLevel * 2);
            member_offset += last_member_size ? last_member_size : 8;
        }

        insert_span("}\n", indent + m_options.m_indentPerLevel);
    }
    insert_span("}\n", indent);
}

void Disassembler::insert_unmapped_struct(const structs::unmapped *struct_ptr, const u32 indent) {
    u64 member_offset = 0;
    bool offset_gets_pointed_at = false;
    u64 last_member_size = 0;
    u32 member_count = 0;
    const location member_start = location(&struct_ptr->m_data);
    location member_location = member_start;
    while (!offset_gets_pointed_at) {
        member_offset += last_member_size;
        insert_span_indent("%*s[%d] ", indent, member_count++);
        last_member_size = insert_next_struct_member(member_start + member_offset, indent);
        member_location = member_start + (member_offset + last_member_size);
        offset_gets_pointed_at = m_currentFile->gets_pointed_at(member_location + 8) || m_currentFile->is_string(member_location);
    }
}

u8 Disassembler::insert_next_struct_member(const location member, const u32 indent) {
    u8 member_size;
    const char *str_ptr = nullptr;
    if (m_currentFile->is_file_ptr(member)) {
        if (member >= m_currentFile->m_strings) {
            insert_span_fmt("string: \"%s\"\n", member.as<char>());
        }
        else {
            insert_struct_or_arraylike(member, indent);
        }
        member_size = 8;
    }
    else if ((str_ptr = m_sidbase->search(member.get<sid64>())) != nullptr) {
        insert_span_fmt("sid: %s\n", str_ptr);
        member_size = 8;
    }
    else if (is_possible_float(member.as<f32>())) {
        insert_span_fmt("float: %.2f\n", member.get<f32>());
        member_size = 4;
    }
    else if (is_possible_i32(member.as<i32>())) {
        insert_span_fmt("int: %d\n", member.get<i32>());
        member_size = 4;
    }
    else if (is_unmapped_sid(member)) {
        str_ptr = lookup(member.get<sid64>());
        insert_span_fmt("sid: %s\n", str_ptr);
        member_size = 8;
    }
    else {
        insert_span_fmt("int: %d\n", member.get<i32>());
        member_size = 4;
    }
    return member_size;
}

void Disassembler::disassemble() {
    insert_header_line();
    for (i32 i = 0; i < m_currentFile->m_dcheader->m_numEntries; ++i) {
        insert_span("\n\n");
        insert_span_fmt(ENTRY_SEP);
        insert_span_fmt("  ENTRY %u  ", i);
        insert_span(ENTRY_SEP);
        insert_span("\n\n");
        insert_entry(m_currentFile->m_dcheader->m_pStartOfData + i);
    }
}

void Disassembler::insert_entry(const Entry *entry) {
    const structs::unmapped *struct_ptr = reinterpret_cast<const structs::unmapped*>(reinterpret_cast<const u64*>(entry->m_entryPtr) - 1);
    insert_span_fmt("%s = ", lookup(entry->m_nameID));
    insert_struct(struct_ptr, 0, entry->m_nameID);
}


void Disassembler::insert_struct(const structs::unmapped *struct_ptr, const u32 indent, const sid64 name_id) {

    const u64 offset = get_offset(&struct_ptr->m_data);

    insert_span_fmt("%s [0x%05X] {\n", lookup(struct_ptr->typeID), offset);

    switch (struct_ptr->typeID) {
        case SID("state-script"): {
            insert_state_script(reinterpret_cast<const StateScript*>(&struct_ptr->m_data), indent + m_options.m_indentPerLevel);
            break;
        }
        case SID("script-lambda"): {
            std::string name;
            if (name_id == 0) {
                std::stringstream ss;
                ss << "anonymous@";
                ss << std::hex << offset;
                name = ss.str();
            }
            else {
                name = lookup(name_id);
            }
            function_disassembly function = create_function_disassembly(reinterpret_cast<const ScriptLambda*>(&struct_ptr->m_data), name);
            insert_function_disassembly_text(function, indent + m_options.m_indentPerLevel * 2);
            m_functions.push_back(std::move(function));
            break;
        }
        case SID("map"):
        case SID("map-32"): {
            const structs::map *map = reinterpret_cast<const structs::map*>(&struct_ptr->m_data);
            insert_span_indent("%*skeys: [0x%05X], values: [0x%05X]\n\n", indent + m_options.m_indentPerLevel, get_offset(map->keys.data), get_offset(map->values.data));
            for (u64 i = 0; i < map->size; ++i) {
                const char *key_hash = lookup(map->keys[i]);
                insert_span_indent("%*s%s {\n%*s", indent + m_options.m_indentPerLevel, key_hash, indent + m_options.m_indentPerLevel * 2, "");
                const structs::unmapped *struct_ptr = reinterpret_cast<const structs::unmapped*>(map->values[i] - 8);
                insert_struct(struct_ptr, indent + m_options.m_indentPerLevel * 2);
                insert_span("}\n", indent + m_options.m_indentPerLevel);
            }
            break;
        }
        default: {
            if (m_options.m_emitOnce && m_currentFile->m_emittedStructs.find(reinterpret_cast<p64>(struct_ptr)) != m_currentFile->m_emittedStructs.end()) {
                insert_span_indent("%*sALREADY EMITTED\n%*s}\n", indent + m_options.m_indentPerLevel, indent, "");
                return;
            }
            insert_unmapped_struct(struct_ptr, indent + m_options.m_indentPerLevel);
            break;
        }
    }
    insert_span("}\n", indent);
    if (m_options.m_emitOnce) {
        m_currentFile->m_emittedStructs.emplace(reinterpret_cast<p64>(struct_ptr));
    }
}

[[nodiscard]] u32 Disassembler::get_offset(const location loc) const noexcept {
    return loc.num() - reinterpret_cast<p64>(m_currentFile->m_dcheader);
}

[[nodiscard]] u32 Disassembler::get_offset(const void* loc) const noexcept {
    return reinterpret_cast<p64>(loc) - reinterpret_cast<p64>(m_currentFile->m_dcheader);
}

void Disassembler::insert_variable(const SsDeclaration *var, const u32 indent) {
    bool is_nullptr = var->m_pDeclValue == nullptr;


    insert_span_indent("%*s[0x%06X] ",  indent, get_offset(var));
    insert_span_fmt("%-8s ", lookup(var->m_declTypeId));
    insert_span_fmt("%-20s = ", lookup(var->m_declId));

    switch (var->m_declTypeId) {
        case SID("boolean"): {
            if (!is_nullptr) 
                insert_span(*reinterpret_cast<bool*>(var->m_pDeclValue) ? "true" : "false");
            break;
        }
        case SID("vector"): {
            if (!is_nullptr) {
                f32 *val = reinterpret_cast<f32*>(var->m_pDeclValue);
                insert_span_fmt("(%.2f, %.2f, %.2f, %.2f)", val[0], val[1], val[2], val[3]);
            }
            break;
        }
        case SID("quat"): {
            if (!is_nullptr) {
                f32 *val = reinterpret_cast<f32*>(var->m_pDeclValue);
                insert_span_fmt("(%.2f, %.2f, %.2f, %.2f)", val[0], val[1], val[2], val[3]);
            }
            break;
        }
        case SID("float"): {
            if (!is_nullptr) {
                insert_span_fmt("%.2f", *reinterpret_cast<f32*>(var->m_pDeclValue));
            }
            break;
        }
        case SID("string"): {
            if (!is_nullptr) {
                insert_span_fmt("%s", *reinterpret_cast<const char**>(var->m_pDeclValue));
            }
            break;
        }
        case SID("symbol"): {
            if (!is_nullptr) {
                insert_span(lookup(*reinterpret_cast<sid64*>(var->m_pDeclValue)));
            }
            break;
        }
        case SID("int32"): {
            if (!is_nullptr) {
                insert_span_fmt("%i", *reinterpret_cast<i32*>(var->m_pDeclValue));
            }
            break;
        }
        case SID("uint64"): {
            if (!is_nullptr) {
                insert_span_fmt("%llx", *reinterpret_cast<u64*>(var->m_pDeclValue));
            }
            break;
        }
        case SID("timer"): {
            if (!is_nullptr) {
                insert_span_fmt("%f", *reinterpret_cast<f32*>(var->m_pDeclValue));
            }
            break;
        }
        case SID("point"): {
            if (!is_nullptr) {
                f32 *val = reinterpret_cast<f32*>(var->m_pDeclValue);
                insert_span_fmt("(%.2f, %.2f, %.2f)", val[0], val[1], val[2]);
            }
            break;
        }
        case SID("bound-frame"): {
            if (!is_nullptr) {
                insert_span_fmt("%f", *reinterpret_cast<f32*>(var->m_pDeclValue));
            }
            break;
        }
        default: {
            insert_span("???");
            break;
        }
    }
    if (is_nullptr) {
        insert_span("uninitialized");
    }
    insert_span("\n");
}

void Disassembler::insert_on_block(const SsOnBlock *block, const u32 indent, anonymous_function_name& function_name) {
    switch (block->m_blockType) {
        case 0: {
            function_name.m_event = "start";
            break;
        }
        case 1: {
            function_name.m_event = "end";
            break;
        }
        case 2: {
            function_name.m_event = std::string("event_").append(lookup(block->m_blockEventId));
            break;
        }
        case 3: {
            function_name.m_event = "update";
            break;
        }
        case 4: {
            function_name.m_event = "virtual";
            break;
        }
        default: {
            function_name.m_event = "unknown";
            break;
        }
    }
    insert_span_indent("%*sON %s {\n", indent, function_name.m_event.c_str());

    for (i16 i = 0; i < block->m_trackGroup.m_numTracks; ++i) {
        SsTrack *track_ptr = block->m_trackGroup.m_aTracks + i;
        function_name.m_track = lookup(track_ptr->m_trackId);
        insert_span_indent("%*sTRACK %s {\n", indent + m_options.m_indentPerLevel, function_name.m_track.c_str());
        for (i16 j = 0; j < track_ptr->m_totalLambdaCount; ++j) {
            insert_span("{\n", indent + m_options.m_indentPerLevel * 2);
            function_name.m_idx = std::to_string(j);
            function_disassembly function = create_function_disassembly(track_ptr->m_pSsLambda[j].m_pScriptLambda, function_name.get(), true);
            insert_function_disassembly_text(function, indent + m_options.m_indentPerLevel * 3);
            m_functions.push_back(std::move(function));
            insert_span("}\n", indent + m_options.m_indentPerLevel * 2);
        }
        insert_span("}\n\n", indent + m_options.m_indentPerLevel);
    }

    insert_span("}\n", indent);
}

void Disassembler::insert_state_script(const StateScript *stateScript, const u32 indent) {

    if (stateScript->m_pSsOptions != nullptr && stateScript->m_pSsOptions->m_pSymbolArray != nullptr) {
        SymbolArray *s_array = stateScript->m_pSsOptions->m_pSymbolArray;
        insert_span("OPTIONS: ", indent);
        for (i32 i = 0; i < s_array->m_numEntries; ++i) {
            insert_span(lookup(s_array->m_pSymbols[i]), indent + m_options.m_indentPerLevel);
            insert_span("\n");
        }
    }

    SsDeclarationList *decl_table = stateScript->m_pSsDeclList;
    if (decl_table != nullptr) {
        insert_span("DECLARATIONS: \n", indent);
        for (i32 i = 0; i < decl_table->m_numDeclarations; ++i) {
            SsDeclaration *decl = decl_table->m_pDeclarations + i;
            if (decl->m_isVar) {
                insert_variable(decl, indent + m_options.m_indentPerLevel);
            }
        }
    }
    anonymous_function_name anon_name;
    for (i16 i = 0; i < stateScript->m_stateCount; ++i) {
        SsState *state_ptr = stateScript->m_pSsStateTable + i;
        anon_name.m_state = lookup(state_ptr->m_stateId);
        insert_span_indent("%*sSTATE %s {\n", indent + m_options.m_indentPerLevel, anon_name.m_state.c_str());
        for (i64 j = 0; j < state_ptr->m_numSsOnBlocks; ++j) {
            insert_on_block(state_ptr->m_pSsOnBlocks + j, indent + m_options.m_indentPerLevel * 2, anon_name);
        }
        insert_span_indent("%*s} END STATE %s\n\n", indent + m_options.m_indentPerLevel, anon_name.m_state.c_str());
    }
}

[[nodiscard]] bool Disassembler::pointer_gets_called(const u32 dst, const u32 start_idx, const function_disassembly& fn) const {
    for (auto it = fn.m_lines.begin() + start_idx; it != fn.m_lines.end(); ++it) {
        if (it->m_instruction.destination == dst) {
            return it->m_instruction.opcode == Opcode::Call || it->m_instruction.opcode == Opcode::CallFf;
        }
    }
    return false;
}

[[nodiscard]] function_disassembly Disassembler::create_function_disassembly(const ScriptLambda *lambda, const std::string& name, const bool is_script_function) {
    Instruction *instructionPtr = reinterpret_cast<Instruction*>(lambda->m_pOpcode);
    const u64 instructionCount = reinterpret_cast<Instruction*>(lambda->m_pSymbols) - instructionPtr;

    std::vector<function_disassembly_line> lines;
    lines.reserve(instructionCount);

    function_disassembly functionDisassembly {
        std::move(lines),
        StackFrame(location(lambda->m_pSymbols)),
        name,
        is_script_function
    };


    for (u64 i = 0; i < instructionCount; ++i) {
        functionDisassembly.m_lines.emplace_back(i, instructionPtr);
    }

    bool counting_args = true;

    for (u64 i = 0; i < instructionCount; ++i) {
        process_instruction(i, functionDisassembly);
        if (counting_args) {
            if (functionDisassembly.m_lines[i].m_instruction.operand1 >= ARGUMENT_REGISTERS_IDX) {
                functionDisassembly.m_stackFrame.m_registerArgs.push_back(std::monostate());
            } else {
                counting_args = false;
            }
        }
    }
    return functionDisassembly;
}

[[nodiscard]] function_disassembly Disassembler::create_function_disassembly(std::vector<Instruction>&& instructions, const std::string &name, const location& symbol_table, const bool is_script_function) {
    std::vector<function_disassembly_line> lines;
    lines.reserve(instructions.size());

    for (u64 i = 0; i < instructions.size(); ++i) {
        lines.emplace_back(i, instructions.data());
    }

    function_disassembly functionDisassembly {
        std::move(lines),
        StackFrame(symbol_table),
        name,
        is_script_function
    };

    bool counting_args = true;

    for (u64 i = 0; i < instructions.size(); ++i) {
        process_instruction(i, functionDisassembly);
        if (counting_args) {
            if (functionDisassembly.m_lines[i].m_instruction.operand1 >= 49) {
                functionDisassembly.m_stackFrame.m_registerArgs.push_back(std::monostate());
            } else {
                counting_args = false;
            }
        }
    }
    return functionDisassembly;
}

void Disassembler::set_register_types(Register &op1, Register &op2, ast::full_type type) {
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

template<typename T, ast::primitive_kind kind>
void load_static_imm(
    const u32 dest, 
    const u32 op1, 
    StackFrame &frame, 
    ast::full_type& table_entry,
    char *varying, 
    const u32 disassembly_text_size, 
    char *interpreted, 
    const u32 interpreted_buffer_size, 
    const char* type_str) 
{
    std::snprintf(varying, disassembly_text_size, "r%d, %d", dest, op1);
    const T value = frame.m_symbolTable.first.get<T>(op1 * 8);
    const auto new_type = make_type(kind);
    frame[dest].m_type = new_type;
    frame[dest].m_fromSymbolTable = op1;
    table_entry = new_type;
    if constexpr (std::is_same_v<f32, T>) {
        frame[dest].m_value = std::bit_cast<u32>(value);
    } else {
        frame[dest].m_value = value;
    }
    std::snprintf(interpreted, interpreted_buffer_size, type_str, dest, op1, value);
}

void load_nonstatic(
    const u32 dest, 
    const u32 op1, 
    StackFrame &frame, 
    char *varying, 
    const u32 disassembly_text_size, 
    ast::primitive_kind kind,
    char* interpreted,
    const u32 interpreted_buffer_size,
    const char* type_format,
    const char* op1_str
) {
    frame[op1].m_type = ast::ptr_type{kind};
    std::snprintf(varying, disassembly_text_size, "r%d, [r%d]", dest, op1);
    frame[dest].m_type = make_type(kind);
    std::snprintf(interpreted, interpreted_buffer_size, type_format, dest, op1_str);
}

void store_nonstatic(
    const u32 dest, 
    const u32 op1, 
    StackFrame &frame, 
    char *varying, 
    const u32 disassembly_text_size, 
    ast::primitive_kind kind,
    char* interpreted,
    const u32 interpreted_buffer_size,
    const char* type_format,
    const char* op1_str,
    const char* op2_str
) {
    frame[dest].m_type = ast::ptr_type{kind};
    std::snprintf(varying, disassembly_text_size, "[r%d], r%d", dest, op1);
    frame[op1].m_type = make_type(kind);
    std::snprintf(interpreted, interpreted_buffer_size, type_format, dest, op1_str, op2_str);
}

void Disassembler::process_instruction(const u32 istr_idx, function_disassembly &fn) {
    function_disassembly_line& line = fn.m_lines[istr_idx];
    StackFrame& frame = fn.m_stackFrame;

    constexpr u32 interpreted_buffer_size = 512;
    constexpr u32 disassembly_buffer_size = 256;

    char disassembly_text[disassembly_buffer_size] = {0};
    char interpreted[interpreted_buffer_size] = {0};
    const Instruction istr = line.m_instruction;
    const u32 dest = istr.destination;
    const u32 op1 = istr.operand1;
    const u32 op2 = istr.operand2;
    ast::full_type table_entry;

    std::snprintf(disassembly_text, disassembly_buffer_size, "%04llX   0x%06X   %02X %02X %02X %02X   %-21s",
        line.m_location,
        get_offset(reinterpret_cast<const void*>(line.m_globalPointer + line.m_location)),
        static_cast<u32>(istr.opcode),
        dest,
        op1,
        op2,
        istr.opcode_to_string()
    );
    char *varying = disassembly_text + strlen(disassembly_text);
    const u32 disassembly_text_size = disassembly_buffer_size - strlen(disassembly_text);

    char dst_str[interpreted_buffer_size] = {0}; 
    char op1_str[interpreted_buffer_size] = {0}; 
    char op2_str[interpreted_buffer_size] = {0}; 

    if (!istr.destination_is_immediate()) {
        frame.to_string(dst_str, interpreted_buffer_size, dest, lookup(frame[dest].m_value));
        if (!is_unknown(frame[dest].m_type) && frame[dest].m_containsArg) {
            frame.m_registerArgs[frame[dest].m_argNum] = frame[dest].m_type;
        }
        if (dest != op1 && dest != op2 && istr.operand1_is_used()) {
            frame[dest].m_containsArg = false;
            frame[dest].m_isReturn = false;
        }
    }
    if (!istr.operand1_is_immediate()) {
        frame.to_string(op1_str, interpreted_buffer_size, op1, lookup(frame[op1].m_value));
    }
    if (!istr.operand2_is_immediate()) {
        frame.to_string(op2_str, interpreted_buffer_size, op2, lookup(frame[op2].m_value));
    }
    const Opcode opcode = istr.opcode;
    switch (opcode) {
        case Opcode::Return: {
            std::snprintf(varying, disassembly_text_size,"r%d", dest);
            std::snprintf(interpreted, interpreted_buffer_size, "Return %s", dst_str);
            for (const auto& reg : fn.m_stackFrame.m_registers) {
                if (reg.m_containsArg && !is_unknown(reg.m_type)) {
                    fn.m_stackFrame.m_registerArgs[reg.m_argNum] = reg.m_type;
                }
            }
            fn.m_stackFrame.m_returnType = frame[dest].m_type;
            break;
        }
        case Opcode::IAdd: {
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            if (frame[op1].is_pointer()) {
                frame[dest].m_type = frame[op1].m_type;
                frame[dest].m_value = frame[op1].m_value;
                frame[dest].m_pointerOffset += frame[op2].m_pointerOffset;
            }
            frame.to_string(dst_str, interpreted_buffer_size, dest, lookup(frame[dest].m_value));
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = %s + %s", dest, op1_str, op2_str);
            break;
        }
        case Opcode::ISub:
        case Opcode::IMul:
        case Opcode::IDiv:
        case Opcode::FAdd:
        case Opcode::FSub:
        case Opcode::FMul:
        case Opcode::FDiv: {
            char op = 0;
            if (opcode == Opcode::FAdd) {
                op = '+';
            } else if (opcode == Opcode::ISub || opcode == Opcode::FSub) {
                op = '-';
            } else if (opcode == Opcode::IMul || opcode == Opcode::FMul) {
                op = '*';
            } else if (opcode == Opcode::IDiv || opcode == Opcode::FDiv) {
                op = '/';
            }
            std::snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", dest, op1, op2);
            frame.to_string(dst_str, interpreted_buffer_size, dest);
            std::snprintf(interpreted, interpreted_buffer_size, "%s = %s %c %s", dst_str, op1_str, op, op2_str);
            break;
        }
        case Opcode::LoadStaticInt: {
            const i64 table_value = frame.m_symbolTable.first.get<i64>(op1 * 8);
            table_entry = make_type(ast::primitive_kind::I32);
            std::snprintf(varying, disassembly_text_size,"r%d, %d", dest, op1);
            frame[dest].m_value = table_value;
            frame.to_string(dst_str, interpreted_buffer_size, dest);
            std::snprintf(interpreted, interpreted_buffer_size, "%s = ST[%d] -> <%s>", dst_str, op1, op1_str); 
            break;
        }
        case Opcode::LoadStaticFloat: {
            const f32 table_value = frame.m_symbolTable.first.get<f32>(op1 * 8);
            table_entry = make_type(ast::primitive_kind::F32);
            std::snprintf(varying, disassembly_text_size,"r%d, %d", dest, op1);
            frame[dest].m_value = std::bit_cast<u32>(table_value);
            frame.to_string(dst_str, interpreted_buffer_size, dest);
            std::snprintf(interpreted, interpreted_buffer_size, "%s = ST[%d] -> <%s>", dst_str, op1, op1_str); 
            break;
        }
        case Opcode::LoadStaticPointer: {
            const p64 table_value = frame.m_symbolTable.first.get<p64>(op1 * 8);
            table_entry = ast::ptr_type{};
            std::snprintf(varying, disassembly_text_size,"r%d, %d", dest, op1);
            frame[dest].m_type = ast::ptr_type();
            frame.to_string(dst_str, interpreted_buffer_size, dest);
            std::snprintf(interpreted, interpreted_buffer_size, "%s = ST[%d] -> <%s>", dst_str, op1, op1_str); 
            break;
        }
        case Opcode::LoadU16Imm: {
            const u16 value = op1 | (op2 << 8);
            std::snprintf(varying, disassembly_text_size,"r%d, %d", dest, value);
            frame[dest].m_type = make_type(ast::primitive_kind::U16);
            frame[dest].m_value = value;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = %d", dest, value);
            break;
        }
        case Opcode::LoadU32: {
            load_nonstatic(dest, op1, frame, varying, disassembly_text_size, ast::primitive_kind::U32, interpreted, interpreted_buffer_size, "r%d = *(u32*)%s", op1_str);
            break;
        }
        case Opcode::LoadFloat: {
            load_nonstatic(dest, op1, frame, varying, disassembly_text_size, ast::primitive_kind::F32, interpreted, interpreted_buffer_size, "r%d = *(f32*)%s", op1_str);
            break;
        }
        case Opcode::LoadPointer: {
            frame[op1].set_first_type(ast::ptr_type{ast::ptr_type{}});
            std::snprintf(varying, disassembly_text_size, "r%d, [r%d]", dest, op1);
            frame[dest].m_type = ast::ptr_type{};
            frame[dest].m_value = frame[op1].m_value;
            frame[dest].m_pointerOffset = frame[op1].m_pointerOffset;
            frame.to_string(dst_str, interpreted_buffer_size, op1, lookup(frame[op1].m_value));
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = *(p64*)%s", dest, dst_str);
            break;
        }
        case Opcode::LoadI64: {
            load_nonstatic(dest, op1, frame, varying, disassembly_text_size, ast::primitive_kind::I64, interpreted, interpreted_buffer_size, "r%d = *(i64*)%s", op1_str);
            break;
        }
        case Opcode::LoadU64: {
            load_nonstatic(dest, op1, frame, varying, disassembly_text_size, ast::primitive_kind::U64, interpreted, interpreted_buffer_size, "r%d = *(u64*)%s", op1_str);
            break;
        }
        case Opcode::StoreInt: {
            store_nonstatic(dest, op1, frame, varying, disassembly_text_size, ast::primitive_kind::I32, interpreted, interpreted_buffer_size, "r%d, *(i32*)%s = %s", op1_str, op2_str);
            break;
        }
        case Opcode::StoreFloat: {
            store_nonstatic(dest, op1, frame, varying, disassembly_text_size, ast::primitive_kind::F32, interpreted, interpreted_buffer_size, "r%d, *(f32*)%s = %s", op1_str, op2_str);
            break;
        }
        case Opcode::StorePointer: {
            frame[dest].set_first_type(ast::ptr_type{ast::ptr_type{}});
            std::snprintf(varying, disassembly_text_size,"[r%d], r%d", dest, op1);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d, *(p64*)%s = %s", dest, op1_str, op2_str);
            frame[op1].m_type = ast::ptr_type{};
            break;
        }
        case Opcode::LookupInt: {
            std::snprintf(varying, disassembly_text_size,"r%d, %d", dest, op1);
            const i64 value = frame.m_symbolTable.first.get<i64>(op1 * 8);
            frame[dest].m_type = make_type(ast::primitive_kind::I64);
            table_entry = make_type(ast::primitive_kind::I64);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%s>", dest, op1, lookup(value));
            break;
        }
        case Opcode::LookupFloat: {
            std::snprintf(varying, disassembly_text_size,"r%d, %d", dest, op1);
            const i64 value = frame.m_symbolTable.first.get<i64>(op1 * 8);
            frame[dest].m_type = make_type(ast::primitive_kind::F32);
            table_entry = make_type(ast::primitive_kind::F32);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%s>", dest, op1, lookup(value));
            break;
        }
        case Opcode::LookupPointer: {
            std::snprintf(varying, disassembly_text_size,"r%d, %d", dest, op1);
            const p64 value = frame.m_symbolTable.first.get<p64>(op1 * 8);
            frame[dest].m_value = value;
            frame[dest].m_fromSymbolTable = op1;
            const bool is_function = pointer_gets_called(dest, istr_idx + 1, fn);
            const ast::full_type existing_type = builtinFunctions.contains(value) ? builtinFunctions.at(value) : ast::function_type{};
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%s>", dest, op1, lookup(value));
            if (is_function) {
                table_entry = existing_type;
                frame[dest].m_type = existing_type;
            } else {
                table_entry = ast::ptr_type{};
                frame[dest].m_type = ast::ptr_type{};
                frame[dest].m_pointerOffset = 0;
            }
            break;
        }
        case Opcode::MoveInt: {
            frame[op1].set_first_type(ast::primitive_kind::I64);
            std::snprintf(varying, disassembly_text_size,"r%d, %d", dest, op1);
            frame[dest].m_type = make_type(ast::primitive_kind::I64);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d <%lli>", dest, op1, frame[op1].m_value);
            break;
        }
        case Opcode::MoveFloat: {
            frame[op1].set_first_type(ast::primitive_kind::F32);
            std::snprintf(varying, disassembly_text_size,"r%d, %d", dest, op1);
            frame[dest].m_type = make_type(ast::primitive_kind::F32);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d <%f>", dest, op1, static_cast<f32>(std::bit_cast<f64>(frame[op1].m_value)));
            break;
        }
        case Opcode::MovePointer: {
            frame[op1].set_first_type(ast::ptr_type{});
            std::snprintf(varying, disassembly_text_size,"r%d, %d", dest, op1);
            frame[dest].m_type = ast::ptr_type{};
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d <%s>", dest, op1, lookup(frame[op1].m_value));
            break;
        }
        case Opcode::CastInteger: {
            std::snprintf(varying, disassembly_text_size,"r%d, r%d", dest, op1);
            frame[dest].m_type = make_type(ast::primitive_kind::I32);
            frame[dest].m_value = frame[op1].m_value;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = int(r%d) -> <%f> => <%llu>", dest, op1, static_cast<f32>(std::bit_cast<f64>(frame[op1].m_value)), frame[dest].m_value);
            break;
        }
        case Opcode::CastFloat: {
            std::snprintf(varying, disassembly_text_size,"r%d, r%d", dest, op1);
            frame[dest].m_type = make_type(ast::primitive_kind::F32);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = float(r%d) -> <%llu> => <%f>", dest, op1, frame[op1].m_value, static_cast<f32>(std::bit_cast<f64>(frame[dest].m_value)));
            break;
        }
        case Opcode::Call: 
        case Opcode::CallFf: {
            std::snprintf(varying, disassembly_text_size, "r%d, r%d, %d", dest, op1, op2);
            char comment_str[300];
            const char* function_name = lookup(frame[op1].m_value);
            const auto builtin = builtinFunctions.find(frame[op1].m_value);
            u8 offset = std::snprintf(comment_str, sizeof(comment_str), "r%d = %s(", dest, function_name);
            for (u64 i = 0; i < op2; ++i) {
                if (i != 0) {
                    offset += std::snprintf(comment_str + offset, sizeof(comment_str) - offset, ", ");
                }
                if (builtin != builtinFunctions.end()) {
                    offset += std::snprintf(comment_str + offset, sizeof(comment_str) - offset, "%s: ", builtin->second.m_arguments[i].first.c_str());
                }
                else {
                    const auto arg_type = std::make_shared<ast::full_type>(frame[49 + i].m_type);
                    std::get<ast::function_type>(frame.m_symbolTable.second[frame[dest].m_fromSymbolTable]).m_arguments.emplace_back("deduced", arg_type);
                }
                frame.to_string(dst_str, interpreted_buffer_size, 49 + i, lookup(frame[i + 49].m_value));
                offset += std::snprintf(comment_str + offset, sizeof(comment_str) - offset, "%s", dst_str);
            }
            if (builtin != builtinFunctions.end()) {
                frame[dest].m_type = *builtin->second.m_return;
                u8 i = 0;
                for (const auto& [name, type] : builtin->second.m_arguments) {
                    frame[49 + i].m_type = *type;
                    ++i;
                }
            }
            frame[dest].m_isReturn = true;
            frame[dest].m_pointerOffset = 0;
            std::snprintf(interpreted, interpreted_buffer_size, "%s)", comment_str);
            break;
        }
        case Opcode::IEqual:
        case Opcode::IGreaterThan: 
        case Opcode::IGreaterThanEqual: 
        case Opcode::ILessThan: 
        case Opcode::ILessThanEqual: 
        case Opcode::FEqual: 
        case Opcode::FGreaterThan:
        case Opcode::FGreaterThanEqual: 
        case Opcode::FLessThan: 
        case Opcode::FLessThanEqual:
        case Opcode::INotEqual:
        case Opcode::FNotEqual: {
            if (opcode == Opcode::IEqual || opcode == Opcode::IGreaterThan || opcode == Opcode::IGreaterThanEqual || opcode == Opcode::ILessThan || opcode == Opcode::ILessThanEqual || opcode == Opcode::INotEqual) {
                set_register_types(frame[op1], frame[op2], make_type(ast::primitive_kind::I64));
            } else {
                set_register_types(frame[op1], frame[op2], make_type(ast::primitive_kind::F32));
            }
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            if (op1 != dest) {
                frame[dest].m_type = make_type(ast::primitive_kind::BOOL);
            }
            const char* op = "";
            if (opcode == Opcode::IEqual || opcode == Opcode::FEqual) {
                op = "==";
            } else if (opcode == Opcode::IGreaterThan || opcode == Opcode::FGreaterThan) {
                op = ">";
            } else if (opcode == Opcode::IGreaterThanEqual || opcode == Opcode::FGreaterThanEqual) {
                op = ">=";
            } else if (opcode == Opcode::ILessThan || opcode == Opcode::FLessThan) {
                op = "<";
            } else if (opcode == Opcode::ILessThanEqual || opcode == Opcode::FLessThanEqual) {
                op = "<=";
            } else if (opcode == Opcode::INotEqual || opcode == Opcode::FNotEqual) {
                op = "!=";
            }
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d %s r%d", dest, op1, op, op2);
            break;
        }
        case Opcode::IMod:
        case Opcode::FMod: {
            auto type = opcode == Opcode::IMod ? ast::primitive_kind::I64 : ast::primitive_kind::F32;
            set_register_types(frame[op1], frame[op2], make_type(type));
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = make_type(type);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d %% r%d", dest, op1, op2);
            break;
        }
        case Opcode::IAbs:
        case Opcode::FAbs: {
            auto type = opcode == Opcode::IAbs ? ast::primitive_kind::I64 : ast::primitive_kind::F32;
            frame[op1].set_first_type(type);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d", dest, op1);
            frame[dest].m_type = make_type(type);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = ABS(r%d)", dest, op1);
            break;
        }
        case Opcode::Branch: {
            u32 target = dest | (op2 << 8);
            std::snprintf(varying, disassembly_text_size,"0x%X", target);
            std::snprintf(interpreted, interpreted_buffer_size, "GOTO ");
            frame.add_target_label(target);
            line.m_target = target;
            if (target < line.m_location) {
                frame.m_backwardsJumpLocs.push_back(line);
            }
            break;
        }
        case Opcode::BranchIf: 
        case Opcode::BranchIfNot: {
            u32 target = dest | (op2 << 8);
            bool is_branching_target = true;
            while (is_branching_target) {
                const auto& target_line = fn.m_lines[target];
                const auto& target_op = target_line.m_instruction.opcode;
                if (target_op == istr.opcode && target_line.m_instruction.operand1 == op1) {
                    target = target_line.m_instruction.destination | (target_line.m_instruction.operand2 << 8);
                }
                else if (istr.opcode == Opcode::BranchIf && target_op == Opcode::BranchIfNot && target_line.m_instruction.operand1 == op1) {
                    target = target_line.m_location + 1;
                }
                else if (istr.opcode == Opcode::BranchIfNot && target_op == Opcode::BranchIf && target_line.m_instruction.operand1 == op1) {
                    target = target_line.m_location + 1;
                }
                else if (target_op == Opcode::OpLogNot && target_line.m_instruction.operand1 == op1) {
                    const auto& flipped_target_opcode = fn.m_lines[target + 1].m_instruction.opcode;
                    if (istr.opcode == Opcode::BranchIf && flipped_target_opcode == Opcode::BranchIfNot) {
                        target = fn.m_lines[target + 1].m_instruction.destination | (fn.m_lines[target + 1].m_instruction.operand2 << 8);
                    }
                    else if (istr.opcode == Opcode::BranchIfNot && flipped_target_opcode == Opcode::BranchIfNot || istr.opcode == Opcode::BranchIf && flipped_target_opcode == Opcode::BranchIf) {
                        target = target + 2;
                    }
                    else {
                        is_branching_target = false;
                    }
                }
                else {
                    is_branching_target = false;
                }
            }
            std::snprintf(varying, disassembly_text_size, "r%d, 0x%X", op1, target);
            const char* comment = istr.opcode == Opcode::BranchIf ? "IF r%d [%s] " : "IF NOT r%d [%s] ";
            std::snprintf(interpreted, interpreted_buffer_size, comment, op1, op1_str);
            line.m_target = target;
            frame.add_target_label(target);
            if (target < line.m_location) {
                frame.m_backwardsJumpLocs.push_back(line);
            }
            break;
        }
        case Opcode::OpLogNot: {
            frame[op1].set_first_type(ast::primitive_kind::BOOL);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d", dest, op1);
            frame[dest].m_type = make_type(ast::primitive_kind::BOOL);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = !%s", dest, op1_str);
            break;
        }
        case Opcode::OpBitAnd: {
            frame[op1].set_first_type(ast::primitive_kind::I64);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = frame[op1].m_type;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d & r%d", dest, op1, op2);
            break;
        }
        case Opcode::OpBitNot: {
            frame[op1].set_first_type(ast::primitive_kind::I64);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d", dest, op1);
            frame[dest].m_type = frame[op1].m_type;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = ~r%d", dest, op1);
            break;
        }
        case Opcode::OpBitOr: {
            frame[op1].set_first_type(ast::primitive_kind::I64);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = frame[op1].m_type;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d | r%d", dest, op1, op2);
            break;
        }
        case Opcode::OpBitXor: {
            frame[op1].set_first_type(ast::primitive_kind::I64);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = frame[op1].m_type;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d ^ r%d", dest, op1, op2);
            break;
        }
        case Opcode::OpBitNor: {
            frame[op1].set_first_type(ast::primitive_kind::I64);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = frame[op1].m_type;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = ~(r%d | r%d) -> <%llu>", dest, op1, op2, frame[dest].m_value);
            break;
        }
        case Opcode::OpLogAnd: {
            frame[op1].set_first_type(ast::primitive_kind::BOOL);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = make_type(ast::primitive_kind::BOOL);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d && r%d", dest, op1, op2);
            break;
        } 
        case Opcode::OpLogOr: {
            frame[op1].set_first_type(ast::primitive_kind::BOOL);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = make_type(ast::primitive_kind::BOOL);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d || r%d", dest, op1, op2);
            break;
        }
        case Opcode::INeg: {
            frame[op1].set_first_type(ast::primitive_kind::I64);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d", dest, op1);
            frame[dest].m_type = make_type(ast::primitive_kind::I64);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = -r%d", dest, op1);
            break;
        }
        case Opcode::FNeg: {
            frame[op1].set_first_type(ast::primitive_kind::F32);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d", dest, op1);
            frame[dest].m_type = make_type(ast::primitive_kind::F32);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = -r%d", dest, op1);
            break;
        }
        case Opcode::LoadParamCnt: {
            break;
        }
        case Opcode::IAddImm: {
            frame[op1].set_first_type(ast::primitive_kind::I64);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, %d", dest, op1, op2);
            const char* resolved = nullptr;
            if (frame[op1].is_pointer()) {
                frame[dest].m_value = frame[op1].m_value;
                frame[dest].m_pointerOffset =  frame[op1].m_pointerOffset + op2;
                resolved = lookup(frame[dest].m_value);
            }
            frame[dest].m_type = frame[op1].m_type;
            frame.to_string(dst_str, interpreted_buffer_size, op1, resolved);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = %s + %d -> <%s>", dest, op1_str, op2, dst_str);
            break;
        }
        case Opcode::ISubImm:
        case Opcode::IMulImm:
        case Opcode::IDivImm: {
            frame[op1].set_first_type(ast::primitive_kind::I64);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, %d", dest, op1, op2);
            frame[dest].m_type = make_type(ast::primitive_kind::I64);
            frame[dest].m_value = frame[op1].m_value;
            const char* op = opcode == Opcode::ISubImm ? "-" : (opcode == Opcode::IMulImm ? "*" : "/");
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d %s %d", dest, op1, op, op2);
            break;
        }
        case Opcode::LoadStaticI32Imm: {
            load_static_imm<i32, ast::primitive_kind::I32>(dest, op1, frame, table_entry, varying, disassembly_text_size, interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%d>");
            break;
        }
        case Opcode::LoadStaticFloatImm: {
            load_static_imm<f32, ast::primitive_kind::F32>(dest, op1, frame, table_entry, varying, disassembly_text_size, interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%f>");
            break;
        }
        case Opcode::LoadStaticPointerImm: {
            load_static_imm<p64, ast::primitive_kind::STRING>(dest, op1, frame, table_entry, varying, disassembly_text_size, interpreted, interpreted_buffer_size, "r%d = ST[%d] -> \"%s\"");
            break;
        }
        case Opcode::LoadStaticI64Imm: {
            load_static_imm<i64, ast::primitive_kind::I64>(dest, op1, frame, table_entry, varying, disassembly_text_size, interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%lli>");
            break;
        }
        case Opcode::LoadStaticU64Imm: {
            std::snprintf(varying, disassembly_text_size,"r%d, %d", dest, op1);
            const u64 value = frame.m_symbolTable.first.get<u64>(op1 * 8);
            const char *hash_str = lookup(value);
            frame[dest].m_type = make_type(ast::primitive_kind::SID);
            frame[dest].m_value = value;
            table_entry = make_type(ast::primitive_kind::SID);
            frame.to_string(dst_str, interpreted_buffer_size, dest, hash_str);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%s>", dest, op1, dst_str);
            break;
        }
        case Opcode::IntAsh: {
            frame[op1].set_first_type(ast::primitive_kind::I64);
            frame[op2].set_first_type(ast::primitive_kind::I8);
            std::snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = frame[op1].m_type;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = %s <<>> %s", dest, op1_str, op2_str);
            break;
        }
        case Opcode::Move: {
            std::snprintf(varying, disassembly_text_size,"r%d, r%d", dest, op1);
            frame[dest] = frame[op1];
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = %s", dest, op1_str);
            break;
        }
        case Opcode::LoadStaticU32Imm: {
            load_static_imm<u32, ast::primitive_kind::U32>(dest, op1, frame, table_entry, varying, disassembly_text_size, interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%u>");
            break;
        }
        case Opcode::LoadStaticI8Imm: {
            load_static_imm<i8, ast::primitive_kind::I8>(dest, op1, frame, table_entry, varying, disassembly_text_size, interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%d>");
            break;
        }
        case Opcode::LoadStaticI16Imm: {
            load_static_imm<i16, ast::primitive_kind::I16>(dest, op1, frame, table_entry, varying, disassembly_text_size, interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%d>");
            break;
        }
        case Opcode::LoadStaticU16Imm: {
            load_static_imm<u16, ast::primitive_kind::U16>(dest, op1, frame, table_entry, varying, disassembly_text_size, interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%u>");
            break;
        }
        case Opcode::LoadI8: {
            load_nonstatic(dest, op1, frame, varying, disassembly_text_size, ast::primitive_kind::I8, interpreted, interpreted_buffer_size, "r%d = *(i8*)%s", op1_str);
            break;
        }
        case Opcode::LoadU8: {
            load_nonstatic(dest, op1, frame, varying, disassembly_text_size, ast::primitive_kind::U8, interpreted, interpreted_buffer_size, "r%d = *(u8*)%s", op1_str);
            break;
        }
        case Opcode::LoadI16: {
            load_nonstatic(dest, op1, frame, varying, disassembly_text_size, ast::primitive_kind::I16, interpreted, interpreted_buffer_size, "r%d = *(i16*)%s", op1_str);
            break;
        }
        case Opcode::LoadU16: {
            load_nonstatic(dest, op1, frame, varying, disassembly_text_size, ast::primitive_kind::U16, interpreted, interpreted_buffer_size, "r%d = *(u16*)%s", op1_str);
            break;
        }
        case Opcode::LoadI32: {
            load_nonstatic(dest, op1, frame, varying, disassembly_text_size, ast::primitive_kind::I32, interpreted, interpreted_buffer_size, "r%d = *(i32*)%s", op1_str);
            break;
        }
        case Opcode::StoreI8: {
            store_nonstatic(dest, op1, frame, varying, disassembly_text_size, ast::primitive_kind::I8, interpreted, interpreted_buffer_size, "r%d, *(i8*)%s = %s", op1_str, op2_str);
            break;
        }
        case Opcode::StoreU8: {
            store_nonstatic(dest, op1, frame, varying, disassembly_text_size, ast::primitive_kind::U8, interpreted, interpreted_buffer_size, "r%d, *(u8*)%s = %s", op1_str, op2_str);
            break;
        }
        case Opcode::StoreI16: {
            store_nonstatic(dest, op1, frame, varying, disassembly_text_size, ast::primitive_kind::I16, interpreted, interpreted_buffer_size, "r%d, *(i16*)%s = %s", op1_str, op2_str);
            break;
        }
        case Opcode::StoreU16: {
            store_nonstatic(dest, op1, frame, varying, disassembly_text_size, ast::primitive_kind::U16, interpreted, interpreted_buffer_size, "r%d, *(u16*)%s = %s", op1_str, op2_str);
            break;
        }
        case Opcode::StoreI32: {
            store_nonstatic(dest, op1, frame, varying, disassembly_text_size, ast::primitive_kind::I32, interpreted, interpreted_buffer_size, "r%d, *(i32*)%s = %s", op1_str, op2_str);
            break;
        }
        case Opcode::StoreU32: {
            store_nonstatic(dest, op1, frame, varying, disassembly_text_size, ast::primitive_kind::U32, interpreted, interpreted_buffer_size, "r%d, *(u32*)%s = %s", op1_str, op2_str);
            break;
        }
        case Opcode::StoreI64: {
            store_nonstatic(dest, op1, frame, varying, disassembly_text_size, ast::primitive_kind::I64, interpreted, interpreted_buffer_size, "r%d, *(i64*)%s = %s", op1_str, op2_str);
            break;
        }
        case Opcode::StoreU64: {
            store_nonstatic(dest, op1, frame, varying, disassembly_text_size, ast::primitive_kind::U64, interpreted, interpreted_buffer_size, "r%d, *(u64*)%s = %s", op1_str, op2_str);
            break;
        }
        case Opcode::AssertPointer: {
            frame[dest].set_first_type(ast::ptr_type{});
            std::snprintf(varying, disassembly_text_size, "r%d", dest);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d != nullptr", dest);
            break;
        }
        default: {
            std::snprintf(varying, disassembly_text_size, "???");
            std::snprintf(interpreted, interpreted_buffer_size, "UNKNOWN INSTRUCTION");
            break;
        }
    }

    if (!is_unknown(table_entry) && op1 == frame.m_symbolTable.second.size()) {
        frame.m_symbolTable.second.push_back(std::move(table_entry));
    }
    line.m_text = std::string(disassembly_text);
    line.m_comment = std::string(interpreted);
}

void Disassembler::insert_label(const std::vector<u32> &labels, const function_disassembly_line &line, const u32 func_size, const u32 indent) {
    auto label_location = std::find(labels.begin(), labels.end(), line.m_location);
    if (label_location != labels.end()) {
        const u32 label_index = std::distance(labels.begin(), label_location);
        if (line.m_location == func_size) {
            insert_span("L_RETURN:\n", indent - m_options.m_indentPerLevel);
        } else if (line.m_location == func_size - 1 && line.m_instruction.opcode == Opcode::LoadU16Imm) {
            insert_span_indent("%*sL_RETURN_%d:\n", indent - m_options.m_indentPerLevel, line.m_instruction.operand1);  
        } else {
            insert_span_indent("%*sL_%d:\n", indent - m_options.m_indentPerLevel, label_index);  
        }
    }
}

void Disassembler::insert_goto_label(const std::vector<u32> &labels, const function_disassembly_line &line, const u32 func_size, const std::vector<function_disassembly_line> &lines) {
    if (line.m_target != -1) {
        u32 target = std::distance(labels.begin(), std::find(labels.begin(), labels.end(), line.m_target));
        if (line.m_target == func_size) {
            insert_span("=> L_RETURN");
        } else if (line.m_target == func_size - 1 && lines[line.m_target].m_instruction.opcode == Opcode::LoadU16Imm) {
            insert_span_fmt("=> L_RETURN_%d", lines[line.m_target].m_instruction.operand1);
        } else {
            insert_span_fmt("=> L_%d", target);
        }
    }
}

void Disassembler::insert_function_disassembly_text(const function_disassembly &functionDisassembly, const u32 indent) {
    auto labels = functionDisassembly.m_stackFrame.m_labels;
    char buffer[512] = {0};
    
    if (!functionDisassembly.m_stackFrame.m_registerArgs.empty()) {
        insert_span_indent("%*s[%d args]\n", indent, functionDisassembly.m_stackFrame.m_registerArgs.size());
    }
    
    for (const auto &line : functionDisassembly.m_lines) {
        u32 line_offset = std::max(67ull - line.m_text.length(), 0ull);
        insert_label(labels, line, functionDisassembly.m_lines.size() - 1, indent);
        insert_span(line.m_text.c_str(), indent);
        const std::string comment = std::string(line_offset, ' ') + line.m_comment;
        insert_span(comment.c_str());
        insert_goto_label(labels, line, functionDisassembly.m_lines.size() - 1, functionDisassembly.m_lines);
        insert_span("\n");
    }
    insert_span_indent("\n%*sSYMBOL TABLE: \n", indent);

    char line_start[64] = {0};
    char type_text[512] = {0};

    const auto& [table_location, types] = functionDisassembly.m_stackFrame.m_symbolTable;

    for (u32 i = 0; i < types.size(); ++i) {
        const auto& type = types[i];
        location value_location = table_location + i * 8;
        std::snprintf(line_start, sizeof(line_start), "%04X   0x%06X   ", i, get_offset(value_location));
        std::visit([&](auto&& entry) -> void {
            using T = std::decay_t<decltype(entry)>;
            if constexpr (std::is_same_v<T, ast::primitive_type>) {
                switch (entry.m_type) {
                    case ast::primitive_kind::I32: {
                        std::snprintf(type_text, sizeof(type_text), "int: <%i>\n", value_location.get<i32>());
                        break;
                    }
                    case ast::primitive_kind::F32: {
                        std::snprintf(type_text, sizeof(type_text), "float: <%f>\n", value_location.get<f32>());
                        break;
                    }
                    case ast::primitive_kind::STRING: {
                        std::snprintf(type_text, sizeof(type_text), "string: \"%s\"\n", value_location.get<const char*>());
                        break;
                    }
                    case ast::primitive_kind::SID: {
                        std::snprintf(type_text, sizeof(type_text), "sid: %s\n", lookup(value_location.get<sid64>()));
                        break;
                    }
                }
            } else if constexpr (std::is_same_v<T, ast::function_type>) {
                if (entry.m_return != nullptr && !is_unknown(*entry.m_return)) {
                    std::snprintf(type_text, sizeof(type_text), "function: %s%s\n", lookup(value_location.get<sid64>()), ast::type_to_declaration_string(entry).c_str());
                }
                else {
                    std::snprintf(type_text, sizeof(type_text), "function: %s\n", lookup(value_location.get<sid64>()));
                }
            } else if constexpr (std::is_same_v<T, ast::ptr_type>) {
                std::snprintf(type_text, sizeof(type_text), "pointer: \"%s\" (%s)\n", lookup(value_location.get<sid64>()), ast::type_to_declaration_string(entry).c_str());
            } else if constexpr (std::is_same_v<T, std::monostate>) {
                std::snprintf(type_text, sizeof(type_text), "unknown: %llu", value_location.get<u64>());
            }
        }, type);
        std::snprintf(buffer, sizeof(buffer), "%s %s", line_start, type_text);
        insert_span(buffer, indent);
    }
}

void Disassembler::insert_header_line() {
    constexpr int BOX_WIDTH = 100;
    insert_span_fmt("%.*s\n", BOX_WIDTH, "####################################################################################################");
    insert_span_fmt("#%-*s#\n", BOX_WIDTH - 2, " ");
    insert_span_fmt("#   DeepQuantum's DC Disassembler ver. %-*s#\n", BOX_WIDTH - 40, "beta_3");
    insert_span_fmt("#   Listing for file: %-*s#\n", BOX_WIDTH - 23, m_currentFile->m_path.filename().string().c_str());
    int num_digits = std::to_string(m_currentFile->m_size).size();
    int padding = BOX_WIDTH - (16 + num_digits + 5);
    insert_span_fmt("#   Filesize: %d bytes%-*s#\n", m_currentFile->m_size, padding, " ");
    insert_span_fmt("#%-*s#\n", BOX_WIDTH - 2, " ");
    insert_span_fmt("%.*s\n\n", BOX_WIDTH, "####################################################################################################");
}
}