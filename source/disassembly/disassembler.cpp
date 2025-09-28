#include "disassembly/custom_structs.h"
#include <cmath>
#include <chrono>
#include "disassembly/disassembler.h"
#include "decompilation/decompiler.h"
#include <cstring>
#include <fstream>

static constexpr char ENTRY_SEP[] = "##############################";


namespace dconstruct {
[[nodiscard]] const char *Disassembler::lookup(const sid64 sid) {
    auto res = m_currentFile->m_sidCache.find(sid);
    if (res != m_currentFile->m_sidCache.end()) {
        return res->second.c_str();
    }

    const char *hash_string = m_sidbase->search(sid);
    if (hash_string == nullptr) {
        const std::string new_hash_string = int_to_string_id(sid);
        auto [iter, inserted] = m_currentFile->m_sidCache.emplace(sid, new_hash_string);
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

[[nodiscard]] b8 Disassembler::is_unmapped_sid(const location loc) const noexcept {
    const b8 in_range = loc.get<sid64>() >= m_sidbase->m_lowestSid && loc.get<sid64>() <= m_sidbase->m_highestSid;
    const b8 is_fileptr = m_currentFile->is_file_ptr(loc);
    return loc.is_aligned() && in_range && !is_fileptr;
}

[[nodiscard]] b8 Disassembler::is_possible_float(const f32 *val) const noexcept {
    f32 rounded = std::roundf(*val * 1e4f) / 1e4f;
    return std::fabsf(*val - rounded) < 1e-4f && *val > -1e4f && *val < 1e4f && rounded != 0.f;
}

[[nodiscard]] b8 Disassembler::is_possible_i32(const i32 *val) const noexcept {
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
    b8 offset_gets_pointed_at = false;
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
            function_disassembly function = create_function_disassembly(reinterpret_cast<const ScriptLambda*>(&struct_ptr->m_data), name_id);
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
    b8 is_nullptr = var->m_pDeclValue == nullptr;


    insert_span_indent("%*s[0x%06X] ",  indent, get_offset(var));
    insert_span_fmt("%-8s ", lookup(var->m_declTypeId));
    insert_span_fmt("%-20s = ", lookup(var->m_declId));

    switch (var->m_declTypeId) {
        case SID("boolean"): {
            if (!is_nullptr) 
                insert_span(*reinterpret_cast<b8*>(var->m_pDeclValue) ? "true" : "false");
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

void Disassembler::insert_on_block(const SsOnBlock *block, const u32 indent) {
    switch (block->m_blockType) {
        case 0: {
            insert_span("ON start {\n", indent);
            break;
        }
        case 1: {
            insert_span("ON end {\n", indent);
            break;
        }
        case 2: {
            insert_span_indent("%*sON event %s {\n", indent, lookup(block->m_blockEventId));
            break;
        }
        case 3: {
            insert_span("ON update {\n",indent);
            break;
        }
        case 4: {
            insert_span("ON virtual {\n",indent);
            break;
        }
        default: {
            insert_span_indent("%*sON UNKNOWN [%d] {\n", indent, block->m_blockType);
            break;
        }
    }

    for (i16 i = 0; i < block->m_trackGroup.m_numTracks; ++i) {
        SsTrack *track_ptr = block->m_trackGroup.m_aTracks + i;
        const char* track_name = lookup(track_ptr->m_trackId);
        insert_span_indent("%*sTRACK %s {\n", indent + m_options.m_indentPerLevel, track_name);
        for (i16 j = 0; j < track_ptr->m_totalLambdaCount; ++j) {
            insert_span("{\n", indent + m_options.m_indentPerLevel * 2);
            function_disassembly function = create_function_disassembly(track_ptr->m_pSsLambda[j].m_pScriptLambda);
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
    for (i16 i = 0; i < stateScript->m_stateCount; ++i) {
        SsState *state_ptr = stateScript->m_pSsStateTable + i;
        const char *state_name = lookup(state_ptr->m_stateId);
        insert_span_indent("%*sSTATE %s {\n", indent + m_options.m_indentPerLevel, state_name);
        for (i64 j = 0; j < state_ptr->m_numSsOnBlocks; ++j) {
            insert_on_block(state_ptr->m_pSsOnBlocks + j, indent + m_options.m_indentPerLevel * 2);
        }
        insert_span_indent("%*s} END STATE %s\n\n", indent + m_options.m_indentPerLevel, state_name);
    }
}

[[nodiscard]] b8 Disassembler::pointer_gets_called(const u32 dst, const u32 start_idx, const function_disassembly& fn) const {
    for (auto it = fn.m_lines.begin() + start_idx; it != fn.m_lines.end(); ++it) {
        if (it->m_instruction.destination == dst) {
            return it->m_instruction.opcode == Opcode::Call || it->m_instruction.opcode == Opcode::CallFf;
        }
    }
    return false;
}

[[nodiscard]] function_disassembly Disassembler::create_function_disassembly(const ScriptLambda *lambda, const sid64 name_id) {
    Instruction *instructionPtr = reinterpret_cast<Instruction*>(lambda->m_pOpcode);
    const u64 instructionCount = reinterpret_cast<Instruction*>(lambda->m_pSymbols) - instructionPtr;

    std::vector<function_disassembly_line> lines;
    lines.reserve(instructionCount);

    function_disassembly functionDisassembly {
        std::move(lines),
        StackFrame(location(lambda->m_pSymbols)),
        name_id ? lookup(name_id) : "anonymous@" + std::to_string(get_offset(lambda->m_pOpcode))
    };


    for (u64 i = 0; i < instructionCount; ++i) {
        functionDisassembly.m_lines.emplace_back(i, instructionPtr);
    }

    b8 counting_args = true;

    for (u64 i = 0; i < instructionCount; ++i) {
        process_instruction(i, functionDisassembly);
        if (counting_args) {
            if (functionDisassembly.m_lines[i].m_instruction.operand1 >= 49) {
                functionDisassembly.m_stackFrame.m_registerArgs.push_back(RegisterValueType::UNKNOWN);
            } else {
                counting_args = false;
            }
        }
    }
    return functionDisassembly;
}

[[nodiscard]] function_disassembly Disassembler::create_function_disassembly(std::vector<Instruction>&& instructions, const std::string &name, const location& symbol_table) {
    std::vector<function_disassembly_line> lines;
    lines.reserve(instructions.size());

    for (u64 i = 0; i < instructions.size(); ++i) {
        lines.emplace_back(i, instructions.data());
    }

    function_disassembly functionDisassembly {
        std::move(lines),
        StackFrame(symbol_table),
        name
    };

    b8 counting_args = true;

    for (u64 i = 0; i < instructions.size(); ++i) {
        process_instruction(i, functionDisassembly);
        if (counting_args) {
            if (functionDisassembly.m_lines[i].m_instruction.operand1 >= 49) {
                functionDisassembly.m_stackFrame.m_registerArgs.push_back(RegisterValueType::UNKNOWN);
            } else {
                counting_args = false;
            }
        }
    }
    return functionDisassembly;
}

void Disassembler::set_register_types(Register &op1, Register &op2, const RegisterValueType type) {
    if (op1.m_type == RegisterValueType::UNKNOWN && op2.m_type == RegisterValueType::UNKNOWN) {
        op1.set_first_type(type);
        op2.set_first_type(type);
    } else {
        auto temp = op1.m_type;
        op1.set_first_type(op2.m_type);
        op2.set_first_type(temp);
    }
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
    SymbolTableEntry table_entry;
    table_entry.m_type = SymbolTableEntryType::NONE;

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
        frame.to_string(dst_str, interpreted_buffer_size, dest, lookup(frame[dest].m_type == RegisterValueType::POINTER ? frame[dest].m_PTR.m_sid : frame[dest].m_SID));
        if (frame[dest].m_type != RegisterValueType::UNKNOWN && frame[dest].isArg) {
            frame.m_registerArgs[frame[dest].argNum] = frame[dest].m_type;
        }
        if (dest != op1 && dest != op2 && istr.operand1_is_used()) {
            frame[dest].isArg = false;
            frame[dest].isReturn = false;
        }
    }
    if (!istr.operand1_is_immediate()) {
        frame.to_string(op1_str, interpreted_buffer_size, op1, lookup(frame[op1].m_type == RegisterValueType::POINTER ? frame[op1].m_PTR.m_sid : frame[op1].m_SID));
    }
    if (!istr.operand2_is_immediate()) {
        frame.to_string(op2_str, interpreted_buffer_size, op2, lookup(frame[op2].m_type == RegisterValueType::POINTER ? frame[op2].m_PTR.m_sid : frame[op2].m_SID));
    }
    const Opcode opcode = istr.opcode;
    switch (opcode) {
        case Opcode::Return: {
            std::snprintf(varying, disassembly_text_size,"r%d", dest);
            std::snprintf(interpreted, interpreted_buffer_size, "Return %s", dst_str);
            for (const auto& reg : fn.m_stackFrame.m_registers) {
                if (reg.isArg && reg.m_type != RegisterValueType::UNKNOWN) {
                    fn.m_stackFrame.m_registerArgs[reg.argNum] = reg.m_type;
                }
            }
            fn.m_stackFrame.m_returnType = frame[dest].m_type;
            break;
        }
        case Opcode::IAdd: {
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[op1].set_first_type(RegisterValueType::I64);
            frame[op2].set_first_type(RegisterValueType::I64);
            if (frame[op1].m_type == RegisterValueType::POINTER) {
                frame[dest].m_type = RegisterValueType::POINTER;
                frame[dest].m_PTR = frame[op1].m_PTR;
                frame[dest].m_PTR.m_offset += frame[op2].m_U64;
            } else {
                frame[dest].m_type = RegisterValueType::I64;
            }
            frame.to_string(dst_str, interpreted_buffer_size, dest, lookup(frame[dest].m_PTR.m_sid));
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = %s + %s", dest, op1_str, op2_str);
            break;
        }
        case Opcode::ISub: {
            frame[op1].set_first_type(RegisterValueType::I64);
            frame[op2].set_first_type(RegisterValueType::I64);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = RegisterValueType::I64;
            frame.to_string(dst_str, interpreted_buffer_size, dest);
            std::snprintf(interpreted, interpreted_buffer_size, "%s = %s - %s", dst_str, op1_str, op2_str);
            break;
        }
        case Opcode::IMul: {
            frame[op1].set_first_type(RegisterValueType::I64);
            frame[op2].set_first_type(RegisterValueType::I64);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = RegisterValueType::I64;
            frame.to_string(dst_str, interpreted_buffer_size, dest);
            std::snprintf(interpreted, interpreted_buffer_size, "%s = %s * %s", dst_str, op1_str, op2_str);
            break;
        }
        case Opcode::IDiv: {
            frame[op1].set_first_type(RegisterValueType::I64);
            frame[op2].set_first_type(RegisterValueType::I64);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = RegisterValueType::I64;
            frame.to_string(dst_str, interpreted_buffer_size, dest);
            const char op = opcode == Opcode::ISub ? '-' : opcode == Opcode::IMul ? '*' : opcode == Opcode::IDiv ? '/' : ' ';
            std::snprintf(interpreted, interpreted_buffer_size, "%s = %s %c %s", dst_str, op1_str, op, op2_str);
            break;
        }
        case Opcode::FAdd: {
            frame[op1].set_first_type(RegisterValueType::F32);
            frame[op2].set_first_type(RegisterValueType::F32);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = RegisterValueType::F32;
            frame[dest].m_F32 = frame[op1].m_F32 + frame[op2].m_F32;
            frame.to_string(dst_str, interpreted_buffer_size, dest);
            std::snprintf(interpreted, interpreted_buffer_size, "%s = %s + %s", dst_str, op1_str, op2_str);
            break;
        }
        case Opcode::FSub: {
            frame[op1].set_first_type(RegisterValueType::F32);
            frame[op2].set_first_type(RegisterValueType::F32);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = RegisterValueType::F32;
            frame.to_string(dst_str, interpreted_buffer_size, dest);
            std::snprintf(interpreted, interpreted_buffer_size, "%s = %s - %s", dst_str, op1_str, op2_str);
            break;
        }
        case Opcode::FMul: {
            frame[op1].set_first_type(RegisterValueType::F32);
            frame[op2].set_first_type(RegisterValueType::F32);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = RegisterValueType::F32;
            frame.to_string(dst_str, interpreted_buffer_size, dest);
            std::snprintf(interpreted, interpreted_buffer_size, "%s = %s * %s", dst_str, op1_str, op2_str);
            break;
        }   
        case Opcode::FDiv: {
            frame[op1].set_first_type(RegisterValueType::F32);
            frame[op2].set_first_type(RegisterValueType::F32);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = RegisterValueType::F32;
            frame.to_string(dst_str, interpreted_buffer_size, dest);
            std::snprintf(interpreted, interpreted_buffer_size, "%s = %s / %s", dst_str, op1_str, op2_str);
            break;
        }
        case Opcode::LoadStaticInt: {
            const i64 table_value = frame.m_symbolTable.get<i64>(op1 * 8);
            table_entry.m_type = SymbolTableEntryType::INT64;
            table_entry.m_i64 = table_value;
            std::snprintf(varying, disassembly_text_size,"r%d, %d", dest, op1);
            frame[dest].m_type = RegisterValueType::I64;
            frame[dest].m_I64 = table_value;
            frame.to_string(dst_str, interpreted_buffer_size, dest);
            std::snprintf(interpreted, interpreted_buffer_size, "%s = ST[%d] -> <%s>", dst_str, op1, op1_str); 
            break;
        }
        case Opcode::LoadStaticFloat: {
            const f32 table_value = frame.m_symbolTable.get<f32>(op1 * 8);
            table_entry.m_type = SymbolTableEntryType::FLOAT;
            table_entry.m_f32 = table_value;
            std::snprintf(varying, disassembly_text_size,"r%d, %d", dest, op1);
            frame[dest].m_type = RegisterValueType::F32;
            frame[dest].m_F32 = table_value;
            frame.to_string(dst_str, interpreted_buffer_size, dest);
            std::snprintf(interpreted, interpreted_buffer_size, "%s = ST[%d] -> <%s>", dst_str, op1, op1_str); 
            break;
        }
        case Opcode::LoadStaticPointer: {
            const p64 table_value = frame.m_symbolTable.get<p64>(op1 * 8);
            table_entry.m_type = SymbolTableEntryType::POINTER;
            table_entry.m_pointer = table_value;
            std::snprintf(varying, disassembly_text_size,"r%d, %d", dest, op1);
            frame[dest].m_type = RegisterValueType::POINTER;
            frame.to_string(dst_str, interpreted_buffer_size, dest);
            std::snprintf(interpreted, interpreted_buffer_size, "%s = ST[%d] -> <%s>", dst_str, op1, op1_str); 
            break;
        }
        case Opcode::LoadU16Imm: {
            const u16 value = op1 | (op2 << 8);
            std::snprintf(varying, disassembly_text_size,"r%d, %d", dest, value);
            frame[dest].m_type = RegisterValueType::U16;
            frame[dest].m_U64 = value;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = %d", dest, value);
            break;
        }
        case Opcode::LoadU32: {
            frame[op1].set_first_type(RegisterValueType::U32_POINTER);
            std::snprintf(varying, disassembly_text_size,"r%d, [r%d]", dest, op1);
            frame[dest].m_type = RegisterValueType::U32;
            frame[dest].m_I32 = 0;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = *(u32*)%s", dest, op1_str);
            break;
        }
        case Opcode::LoadFloat: {
            frame[op1].set_first_type(RegisterValueType::F32_POINTER);
            std::snprintf(varying, disassembly_text_size,"r%d, [r%d]", dest, op1);
            frame[dest].m_type = RegisterValueType::F32;
            frame[dest].m_F32 = 0.f;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = *(f32*)%s", dest, op1_str);
            break;
        }
        case Opcode::LoadPointer: {
            frame[op1].set_first_type(RegisterValueType::POINTER);
            std::snprintf(varying, disassembly_text_size, "r%d, [r%d]", dest, op1);
            frame[dest].m_type = RegisterValueType::POINTER;
            frame[dest].m_PTR = RegisterPointer{frame[op1].m_PTR.m_base, frame[op1].m_PTR.m_offset, frame[op1].m_PTR.m_sid};
            frame.to_string(dst_str, interpreted_buffer_size, op1, lookup(frame[op1].m_PTR.m_sid));
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = *(p64*)%s", dest, dst_str);
            break;
        }
        case Opcode::LoadI64: {
            frame[op1].set_first_type(RegisterValueType::I64_POINTER);
            std::snprintf(varying, disassembly_text_size,"r%d, [r%d]", dest, op1);
            frame[dest].m_type = RegisterValueType::I64;
            frame[dest].m_I64 = 0;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = *(i64*)%s", dest, op1_str);
            break;
        }
        case Opcode::LoadU64: {
            frame[op1].set_first_type(RegisterValueType::U64_POINTER);
            std::snprintf(varying, disassembly_text_size,"r%d, [r%d]", dest, op1);
            frame[dest].m_type = RegisterValueType::U64;
            frame[dest].m_U64 = 0;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = *(u64*)%s", dest, op1_str);
            break;
        }
        case Opcode::StoreInt: {
            frame[op1].set_first_type(RegisterValueType::I32_POINTER);
            std::snprintf(varying, disassembly_text_size,"[r%d], r%d", dest, op1);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d, *(i32*)%s = %s", dest, op1_str, op2_str);
            frame[dest].m_type = RegisterValueType::I32;
            frame[dest].m_I8 = 0;
            break;
        }
        case Opcode::StoreFloat: {
            frame[op1].set_first_type(RegisterValueType::F32_POINTER);
            std::snprintf(varying, disassembly_text_size,"[r%d], r%d", dest, op1);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d, *(f32*)%s = %s", dest, op1_str, op2_str);
            frame[dest].m_type = RegisterValueType::F32;
            frame[dest].m_I8 = 0;
            break;
        }
        case Opcode::StorePointer: {
            frame[op1].set_first_type(RegisterValueType::POINTER);
            std::snprintf(varying, disassembly_text_size,"[r%d], r%d", dest, op1);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d, *(p64*)%s = %s", dest, op1_str, op2_str);
            frame[dest].m_type = RegisterValueType::POINTER;
            frame[dest].m_PTR = {0, 0, 0};
            break;
        }
        case Opcode::LookupInt: {
            std::snprintf(varying, disassembly_text_size,"r%d, %d", dest, op1);
            const i64 value = frame.m_symbolTable.get<i64>(op1 * 8);
            frame[dest].m_type = RegisterValueType::I64;
            frame[dest].m_I64 = value;
            table_entry.m_type = SymbolTableEntryType::POINTER;
            table_entry.m_i64 = value;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%s>", dest, op1, lookup(value));
            break;
        }
        case Opcode::LookupFloat: {
            std::snprintf(varying, disassembly_text_size,"r%d, %d", dest, op1);
            f32 value = frame.m_symbolTable.get<f32>(op1 * 8);
            frame[dest].m_type = RegisterValueType::F32;
            frame[dest].m_F32 = value;
            table_entry.m_type = SymbolTableEntryType::FLOAT;
            table_entry.m_f32 = value;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%.2f>", dest, op1, value);
            break;
        }
        case Opcode::LookupPointer: {
            std::snprintf(varying, disassembly_text_size,"r%d, %d", dest, op1);
            const p64 value = frame.m_symbolTable.get<p64>(op1 * 8);
            frame[dest].m_type = RegisterValueType::POINTER;
            frame[dest].m_PTR.m_base = 0;
            frame[dest].m_PTR.m_offset = 0;
            frame[dest].m_PTR.m_sid = value;
            const b8 is_function = pointer_gets_called(dest, istr_idx + 1, fn);
            table_entry.m_pointer = value;
            if (m_currentFile->is_file_ptr(frame.m_symbolTable + (op1 * 8))) {
                table_entry.m_type = SymbolTableEntryType::POINTER;
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%s>", dest, op1, reinterpret_cast<const char*>(value));
            } else {
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%s>", dest, op1, lookup(value));
                if (is_function) {
                    table_entry.m_type = SymbolTableEntryType::FUNCTION;
                } else {
                    table_entry.m_type = SymbolTableEntryType::POINTER;
                }
            }
            break;
        }
        case Opcode::MoveInt: {
            frame[op1].set_first_type(RegisterValueType::I64);
            std::snprintf(varying, disassembly_text_size,"r%d, %d", dest, op1);
            frame[dest].m_type = RegisterValueType::I64;
            frame[dest].m_I64 = frame[op1].m_I64;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d <%lli>", dest, op1, frame[op1].m_I64);
            break;
        }
        case Opcode::MoveFloat: {
            frame[op1].set_first_type(RegisterValueType::F32);
            std::snprintf(varying, disassembly_text_size,"r%d, %d", dest, op1);
            frame[dest].m_type = RegisterValueType::F32;
            frame[dest].m_F32 = frame[op1].m_F32;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d <%f>", dest, op1, frame[op1].m_F32);
            break;
        }
        case Opcode::MovePointer: {
            frame[op1].set_first_type(RegisterValueType::POINTER);
            std::snprintf(varying, disassembly_text_size,"r%d, %d", dest, op1);
            frame[dest].m_type = RegisterValueType::POINTER;
            frame[dest].m_PTR = frame[op1].m_PTR;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d <%s>", dest, op1, lookup(frame[op1].m_PTR.get()));
            break;
        }
        case Opcode::CastInteger: {
            std::snprintf(varying, disassembly_text_size,"r%d, r%d", dest, op1);
            frame[dest].m_type = RegisterValueType::I32;
            frame[dest].m_I32 = (i32)frame[op1].m_F32;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = int(r%d) -> <%f> => <%d>", dest, op1, frame[op1].m_F32, frame[dest].m_I32);
            break;
        }
        case Opcode::CastFloat: {
            std::snprintf(varying, disassembly_text_size,"r%d, r%d", dest, op1);
            frame[dest].m_type = RegisterValueType::F32;
            frame[dest].m_F32 = (f32)frame[op1].m_I32;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = float(r%d) -> <%d> => <%f>", dest, op1, frame[op1].m_I32, frame[dest].m_F32);
            break;
        }
        case Opcode::Call: 
        case Opcode::CallFf: {
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, %d", dest, op1, op2);
            char comment_str[300];
            const char* function_name = lookup(frame[op1].m_PTR.m_sid);
            u8 offset = std::snprintf(comment_str, sizeof(comment_str), "r%d = %s(", dest, function_name);
            for (u64 i = 0; i < op2; ++i) {
                if (i != 0) {
                    offset += std::snprintf(comment_str + offset, sizeof(comment_str) - offset, ", ");
                }
                frame.to_string(dst_str, interpreted_buffer_size, 49 + i, lookup(frame[i + 49].m_type == RegisterValueType::POINTER ? frame[i + 49].m_PTR.m_sid : frame[i + 49].m_SID));
                offset += std::snprintf(comment_str + offset, sizeof(comment_str) - offset, "%s", dst_str);
            }
            if (m_builtinFuntions.contains(function_name)) {
                const auto& builtin = m_builtinFuntions.at(function_name);
                const RegisterValueType return_type = builtin[0];
                frame[dest].m_type = return_type;
                for (u32 i = 0; i < builtin.size() - 1; ++i) {
                    frame[49 + i].m_type = builtin[i + 1];
                }
            } else {
                frame[dest].m_type = RegisterValueType::POINTER;
            }
            frame[dest].isReturn = true;
            frame[dest].m_PTR = { 0, 0, frame[op1].m_PTR.m_sid };
            std::snprintf(interpreted, interpreted_buffer_size, "%s)", comment_str);
            break;
        }
        case Opcode::IEqual: {
            set_register_types(frame[op1], frame[op2], RegisterValueType::I64);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            if (dest == op1) {
                if (frame[op1].m_type != RegisterValueType::UNKNOWN && frame[op1].isArg) {
                    frame.m_registerArgs[frame[op1].argNum] = frame[op1].m_type;
                }
                frame[op1].isArg = false;
            }
            //frame[dest].m_type = RegisterValueType::BOOL;
            //frame[dest].m_BOOL = frame[op1].m_I64 == frame[op2].m_I64;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = %s == %s", 
                dest, 
                op1_str,
                op2_str
            );
            break;
        }
        case Opcode::IGreaterThan: {
            frame[op1].m_type = RegisterValueType::I64;
            frame[op2].m_type = RegisterValueType::I64;
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = RegisterValueType::BOOL;
            frame[dest].m_BOOL = frame[op1].m_I64 > frame[op2].m_I64;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%lld] > r%d [%lld]", 
                dest, 
                op1, 
                frame[op1].m_I64, 
                op2, 
                frame[op2].m_I64
            );
            break;
        }
        case Opcode::IGreaterThanEqual: {
            frame[op1].m_type = RegisterValueType::I64;
            frame[op2].m_type = RegisterValueType::I64;
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = RegisterValueType::BOOL;
            frame[dest].m_BOOL = frame[op1].m_I64 >= frame[op2].m_I64;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%lli] >= r%d [%lli]", 
                dest, 
                op1, 
                frame[op1].m_I64, 
                op2, 
                frame[op2].m_I64
            );
            break;
        }
        case Opcode::ILessThan: {
            frame[op1].m_type = RegisterValueType::I64;
            frame[op2].m_type = RegisterValueType::I64;
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = RegisterValueType::BOOL;
            frame[dest].m_BOOL = frame[op1].m_I64 < frame[op2].m_I64;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%s] < r%d [%s]", 
                dest, 
                op1, 
                op1_str, 
                op2, 
                op2_str
            );
            break;
        }
        case Opcode::ILessThanEqual: {
            frame[op1].m_type = RegisterValueType::I64;
            frame[op2].m_type = RegisterValueType::I64;
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = RegisterValueType::BOOL;
            frame[dest].m_BOOL = frame[op1].m_I64 <= frame[op2].m_I64;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%lli] <= r%d [%lli]", 
                dest, 
                op1, 
                frame[op1].m_I64, 
                op2, 
                frame[op2].m_I64
            );
            break;
        }
        case Opcode::FEqual: {
            frame[op1].m_type = RegisterValueType::F32;
            frame[op2].m_type = RegisterValueType::F32;
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = RegisterValueType::BOOL;
            frame[dest].m_BOOL = frame[op1].m_F32 == frame[op2].m_F32;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%f] == r%d [%f]", 
                dest, 
                op1, 
                frame[op1].m_F32, 
                op2, 
                frame[op2].m_F32
            );
            break;
        }
        case Opcode::FGreaterThan: {
            frame[op1].m_type = RegisterValueType::F32;
            frame[op2].m_type = RegisterValueType::F32;
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = RegisterValueType::BOOL;
            frame[dest].m_BOOL = frame[op1].m_F32 > frame[op2].m_F32;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%f] > r%d [%f]", 
                dest, 
                op1, 
                frame[op1].m_F32, 
                op2, 
                frame[op2].m_F32
            );
            break;
        }
        case Opcode::FGreaterThanEqual: {
            frame[op1].m_type = RegisterValueType::F32;
            frame[op2].m_type = RegisterValueType::F32;
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = RegisterValueType::BOOL;
            frame[dest].m_BOOL = frame[op1].m_F32 >= frame[op2].m_F32;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%f] >= r%d [%f]", 
                dest, 
                op1, 
                frame[op1].m_F32, 
                op2, 
                frame[op2].m_F32
            );
            break;
        }
        case Opcode::FLessThan: {
            frame[op1].m_type = RegisterValueType::F32;
            frame[op2].m_type = RegisterValueType::F32;
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = RegisterValueType::BOOL;
            frame[dest].m_BOOL = frame[op1].m_F32 < frame[op2].m_F32;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%f] < r%d [%f]", 
                dest, 
                op1, 
                frame[op1].m_F32, 
                op2, 
                frame[op2].m_F32
            );
            break;
        }
        case Opcode::FLessThanEqual: {
            frame[op1].m_type = RegisterValueType::F32;
            frame[op2].m_type = RegisterValueType::F32;
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = RegisterValueType::BOOL;
            frame[dest].m_BOOL = frame[op1].m_F32 <= frame[op2].m_F32;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%f] <= r%d [%f]", 
                dest, 
                op1, 
                frame[op1].m_F32, 
                op2, 
                frame[op2].m_F32
            );
            break;
        }
        case Opcode::IMod: {
            set_register_types(frame[op1], frame[op2], RegisterValueType::I64);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = RegisterValueType::I32;
            if (frame[op2].m_I64 == 0) {
                frame[dest].m_I32 = frame[op1].m_I64 % 1;
            } else {
                frame[dest].m_I32 = frame[op1].m_I64 % frame[op2].m_I64;
            }
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%lli] %% r%d [%lli] -> <%d>", 
                dest, 
                op1, 
                frame[op1].m_I64, 
                op2, 
                frame[op2].m_I64,
                frame[dest].m_I32
            );
            break;
        }
        case Opcode::FMod: {
            set_register_types(frame[op1], frame[op2], RegisterValueType::F32);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = RegisterValueType::F32;
            if (frame[op2].m_F32 == 0.f) {
                frame[dest].m_F32 = fmodf(frame[op1].m_F32, 1.f);
            } else {
                frame[dest].m_F32 = fmodf(frame[op1].m_F32, frame[op2].m_F32);
            }
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%f] %% r%d [%f] -> <%f>", 
                dest, 
                op1, 
                frame[op1].m_F32, 
                op2, 
                frame[op2].m_F32,
                frame[dest].m_F32
            );
            break;
        }
        case Opcode::IAbs: {
            frame[op1].set_first_type(RegisterValueType::I64);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d", dest, op1);
            frame[dest].m_type = RegisterValueType::I32;
            frame[dest].m_I64 = abs(frame[op1].m_I64);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = ABS(r%d) [%lli] -> <%lli>", 
                dest, 
                op1, 
                frame[op1].m_I64,
                frame[dest].m_I64
            );
            break;
        }
        case Opcode::FAbs: {
            frame[op1].set_first_type(RegisterValueType::F32);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d", dest, op1);
            frame[dest].m_type = RegisterValueType::F32;
            frame[dest].m_F32 = abs(frame[op1].m_F32);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = ABS(r%d) [%f] -> <%f>", 
                dest, 
                op1,
                frame[op1].m_F32,
                frame[dest].m_F32
            );
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
            //frame[op1].set_first_type(RegisterValueType::BOOL);
            u32 target = dest | (op2 << 8);
            if (fn.m_lines[target].m_instruction.opcode == istr.opcode && fn.m_lines[target].m_instruction.operand1 == op1) {
                target = fn.m_lines[target].m_instruction.destination | (fn.m_lines[target].m_instruction.operand2 << 8);
            }
            std::snprintf(varying, disassembly_text_size,"r%d, 0x%X", op1, target);
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
            frame[op1].set_first_type(RegisterValueType::BOOL);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d", dest, op1);
            frame[dest].m_type = RegisterValueType::BOOL;
            //frame[dest].m_BOOL = !frame[op1].m_BOOL;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = !%s", dest, op1_str);
            break;
        }
        case Opcode::OpBitAnd: {
            frame[op1].set_first_type(RegisterValueType::I64);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = frame[op1].m_type;
            frame[dest].m_U64 = frame[op1].m_U64 & frame[op2].m_U64;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%llu] & r%d [%llu] -> <%llu>", dest, op1, frame[op1].m_U64, op2, frame[op2].m_U64, frame[dest].m_U64);
            break;
        }
        case Opcode::OpBitNot: {
            frame[op1].set_first_type(RegisterValueType::I64);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d", dest, op1);
            frame[dest].m_type = frame[op1].m_type;
            frame[dest].m_U64 = ~frame[op1].m_U64;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = ~r%d [%llu] -> <%llu>", dest, op1, frame[op1].m_U64, frame[dest].m_U64);
            break;
        }
        case Opcode::OpBitOr: {
            frame[op1].set_first_type(RegisterValueType::I64);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = frame[op1].m_type;
            frame[dest].m_U64 = frame[op1].m_U64 | frame[op2].m_U64;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%llu] | r%d [%llu] -> <%llu>", dest, op1, frame[op1].m_U64, op2, frame[op2].m_U64, frame[dest].m_U64);
            break;
        }
        case Opcode::OpBitXor: {
            frame[op1].set_first_type(RegisterValueType::I64);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = frame[op1].m_type;
            frame[dest].m_U64 = frame[op1].m_U64 ^ frame[op2].m_U64;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%llu] ^ r%d [%llu] -> <%llu>", dest, op1, frame[op1].m_U64, op2, frame[op2].m_U64, frame[dest].m_U64);
            break;
        }
        case Opcode::OpBitNor: {
            frame[op1].set_first_type(RegisterValueType::I64);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = frame[op1].m_type;
            frame[dest].m_U64 = ~(frame[op1].m_U64 | frame[op2].m_U64);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = ~(r%d [%llu] | r%d [%llu]) -> <%llu>", dest, op1, frame[op1].m_U64, op2, frame[op2].m_U64, frame[dest].m_U64);
            break;
        }
        case Opcode::OpLogAnd: {
            frame[op1].set_first_type(RegisterValueType::BOOL);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = RegisterValueType::BOOL;
            frame[dest].m_BOOL = frame[op1].m_BOOL && frame[op2].m_BOOL;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%d] && r%d [%d]", dest, op1, frame[op1].m_BOOL, op2, frame[op2].m_BOOL);
            break;
        } 
        case Opcode::OpLogOr: {
            frame[op1].set_first_type(RegisterValueType::BOOL);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = RegisterValueType::BOOL;
            frame[dest].m_BOOL = frame[op1].m_BOOL || frame[op2].m_BOOL;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%d] || r%d [%d]", dest, op1, frame[op1].m_BOOL, op2, frame[op2].m_BOOL);
            break;
        }
        case Opcode::INeg: {
            frame[op1].set_first_type(RegisterValueType::I64);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d", dest, op1);
            frame[dest].m_type = RegisterValueType::I64;
            frame[dest].m_I64 = -frame[op1].m_I64;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = -r%d [%lli] -> <%lli>", dest, op1, frame[op1].m_I64, frame[dest].m_I64);
            break;
        }
        case Opcode::FNeg: {
            frame[op1].set_first_type(RegisterValueType::F32);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d", dest, op1);
            frame[dest].m_type = RegisterValueType::F32;
            frame[dest].m_F32 = -frame[op1].m_F32;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = -r%d [%f] -> <%f>", dest, op1, frame[op1].m_F32, frame[dest].m_F32);
            break;
        }
        case Opcode::LoadParamCnt: {
            // wat
            break;
        }
        case Opcode::IAddImm: {
            frame[op1].set_first_type(RegisterValueType::I64);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, %d", dest, op1, op2);
            const char* resolved = nullptr;
            if (frame[op1].m_type == RegisterValueType::POINTER) {
                frame[dest].m_type = RegisterValueType::POINTER;
                frame[dest].m_PTR = {frame[op1].m_PTR.m_base, frame[op1].m_PTR.m_offset + op2, frame[op1].m_PTR.m_sid};
                resolved = lookup(frame[dest].m_PTR.m_sid);
            } else {
                frame[dest].m_type = RegisterValueType::I64;
                frame[dest].m_I64 = frame[op1].m_I64 + op2;
            }
            frame.to_string(dst_str, interpreted_buffer_size, op1, resolved);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = %s + %d -> <%s>", dest, op1_str, op2, dst_str);
            break;
        }
        case Opcode::ISubImm: {
            frame[op1].set_first_type(RegisterValueType::I64);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, %d", dest, op1, op2);
            i64 value = frame[op1].m_I64 - op2;
            frame[dest].m_type = RegisterValueType::I64;
            frame[dest].m_I64 = value;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%lli] - %d -> <%lli>", dest, op1, frame[op1].m_I64, op2, frame[dest].m_I64);
            break;
        }
        case Opcode::IMulImm: {
            frame[op1].set_first_type(RegisterValueType::I64);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, %d", dest, op1, op2);
            i64 value = frame[op1].m_I64 * op2;
            frame[dest].m_type = RegisterValueType::I64;
            frame[dest].m_I64 = value;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%lli] * %d -> <%lli>", dest, op1, frame[op1].m_I64, op2, frame[dest].m_I64);
            break;
        }
        case Opcode::IDivImm: {
            frame[op1].set_first_type(RegisterValueType::I64);
            std::snprintf(varying, disassembly_text_size,"r%d, r%d, %d", dest, op1, op2);
            i64 value;
            if (op2 == 0) {
                value = frame[op1].m_I64 / 1;
            } else {
                value = frame[op1].m_I64 / op2;
            }
            frame[dest].m_type = RegisterValueType::I64;
            frame[dest].m_I64 = value;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%lli] / %d -> <%lli>", dest, op1, frame[op1].m_I64, op2, frame[dest].m_I64);
            break;
        }
        case Opcode::LoadStaticI32Imm: {
            std::snprintf(varying, disassembly_text_size,"r%d, %d", dest, op1);
            const i32 value = frame.m_symbolTable.get<i32>(op1 * 8);
            frame[dest].m_type = RegisterValueType::I32;
            frame[dest].m_I32 = value;
            table_entry.m_type = SymbolTableEntryType::INT32;
            table_entry.m_i32 = value;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%d>", dest, op1, value);
            break;
        }
        case Opcode::LoadStaticFloatImm: {
            std::snprintf(varying, disassembly_text_size,"r%d, %d", dest, op1);
            f32 value = frame.m_symbolTable.get<f32>(op1 * 8);
            frame[dest].m_type = RegisterValueType::F32;
            frame[dest].m_F32 = value;
            table_entry.m_type = SymbolTableEntryType::FLOAT;
            table_entry.m_f32 = value;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%.2f>", dest, op1, value);
            break;
        }
        case Opcode::LoadStaticPointerImm: {
            std::snprintf(varying, disassembly_text_size,"r%d, %d", dest, op1);
            p64 value = frame.m_symbolTable.get<p64>(op1 * 8);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> \"%s\"", dest, op1, reinterpret_cast<const char*>(value));
            frame[dest].m_type = RegisterValueType::STRING;
            frame[dest].m_PTR = { value, 0, 0 };
            table_entry.m_type = SymbolTableEntryType::STRING;
            table_entry.m_pointer = value;
            break;
        }
        case Opcode::LoadStaticI64Imm: {
            std::snprintf(varying, disassembly_text_size,"r%d, %d", dest, op1);
            i64 value = frame.m_symbolTable.get<i64>(op1 * 8);
            frame[dest].m_type = RegisterValueType::I64;
            frame[dest].m_I64 = value;
            table_entry.m_type = SymbolTableEntryType::INT64;
            table_entry.m_i64 = value;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%llu>", dest, op1, value);
            break;
        }
        case Opcode::LoadStaticU64Imm: {
            std::snprintf(varying, disassembly_text_size,"r%d, %d", dest, op1);
            const u64 value = frame.m_symbolTable.get<u64>(op1 * 8);
            if (value >= 0x000FFFFFFFFFFFFF) {
                const char *hash_str = lookup(value);
                frame[dest].m_type = RegisterValueType::HASH;
                frame[dest].m_SID = value;
                table_entry.m_type = SymbolTableEntryType::STRINGID_64;
                table_entry.m_hash = value;
                frame.to_string(dst_str, interpreted_buffer_size, dest, hash_str);
                std::snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%s>", dest, op1, dst_str);
            } else if (m_currentFile->is_file_ptr(frame.m_symbolTable + (op1 * 8))) {
                printf("here");
            }
            break;
        }
        case Opcode::IntAsh: {
            frame[op1].set_first_type(RegisterValueType::I64);
            frame[op2].set_first_type(RegisterValueType::I8);
            std::snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", dest, op1, op2);

            u64 hash = frame[op1].m_I64 >> -(char)frame[op2].m_I8;
            if (frame[op2].m_I64 == 0) {
                hash = frame[op1].m_I64 << frame[op2].m_I8;
            }
            frame[dest].m_type = RegisterValueType::I64;
            frame[dest].m_I64 = hash;
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
            std::snprintf(varying, disassembly_text_size, "r%d, r%d", dest, op1);
            const u32 value = frame.m_symbolTable.get<u32>(op1 * 8);
            frame[dest].m_type = RegisterValueType::U32;
            frame[dest].m_U32 = value;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%d>", dest, op1, value);
            break;
        }
        case Opcode::LoadStaticI8Imm: {
            std::snprintf(varying, disassembly_text_size, "r%d, r%d", dest, op1);
            const i8 value = frame.m_symbolTable.get<i8>(op1 * 8);
            frame[dest].m_type = RegisterValueType::I8;
            frame[dest].m_I8 = value;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%d>", dest, op1, value);
            break;
        }
        case Opcode::LoadStaticI16Imm: {
            std::snprintf(varying, disassembly_text_size, "r%d, r%d", dest, op1);
            const i16 value = frame.m_symbolTable.get<i16>(op1 * 8);
            frame[dest].m_type = RegisterValueType::I16;
            frame[dest].m_I16 = value;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%d>", dest, op1, value);
            break;
        }
        case Opcode::LoadStaticU16Imm: {
            std::snprintf(varying, disassembly_text_size, "r%d, r%d", dest, op1);
            const u16 value = frame.m_symbolTable.get<u16>(op1 * 8);
            frame[dest].m_type = RegisterValueType::U16;
            frame[dest].m_U16 = value;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%d>", dest, op1, value);
            break;
        }
        case Opcode::LoadI8: {
            frame[op1].m_type = RegisterValueType::I8_POINTER;
            std::snprintf(varying, disassembly_text_size, "r%d, [r%d]", dest, op1);
            frame[dest].m_type = RegisterValueType::I8;
            frame[dest].m_I8 = 0;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = *(i8*)%s", dest, op1_str);
            break;
        }
        case Opcode::LoadU8: {
            frame[op1].m_type = RegisterValueType::U8_POINTER;
            std::snprintf(varying, disassembly_text_size, "r%d, [r%d]", dest, op1);
            frame[dest].m_type = RegisterValueType::U8;
            frame[dest].m_U8 = 0;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = *(u8*)%s", dest, op1_str);
            break;
        }
        case Opcode::LoadI16: {
            frame[op1].m_type = RegisterValueType::I16_POINTER;
            std::snprintf(varying, disassembly_text_size, "r%d, [r%d]", dest, op1);
            frame[dest].m_type = RegisterValueType::I16;
            frame[dest].m_I16 = 0;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = *(i16*)%s", dest, op1_str);
            break;
        }
        case Opcode::LoadU16: {
            frame[op1].m_type = RegisterValueType::U16_POINTER;
            std::snprintf(varying, disassembly_text_size, "r%d, [r%d]", dest, op1);
            frame[dest].m_type = RegisterValueType::U16;
            frame[dest].m_U16 = 0;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = *(u16*)%s", dest, op1_str);
            break;
        }
        case Opcode::LoadI32: {
            frame[op1].m_type = RegisterValueType::I32_POINTER;
            std::snprintf(varying, disassembly_text_size, "r%d, [r%d]", dest, op1);
            frame[dest].m_type = RegisterValueType::I32;
            frame[dest].m_I32 = 0;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = *(i32*)%s", dest, op1_str);
            break;
        }
        case Opcode::StoreI8: {
            frame[op1].m_type = RegisterValueType::I8_POINTER;
            std::snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", dest, op1, op2);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d, *(i8*)%s = %s", dest, op1_str, op2_str);
            frame[dest].m_type = RegisterValueType::I8;
            frame[dest].m_I8 = 0;
            break;
        }
        case Opcode::StoreU8: {
            frame[op1].m_type = RegisterValueType::U8_POINTER;
            std::snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", dest, op1, op2);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d, *(u8*)%s = %s", dest, op1_str, op2_str);
            frame[dest].m_type = RegisterValueType::U8;
            frame[dest].m_U8 = 0;
            break;
        }
        case Opcode::StoreI16: {
            frame[op1].m_type = RegisterValueType::I16_POINTER;
            std::snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", dest, op1, op2);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d, *(i16*)%s = %s", dest, op1_str, op2_str);
            frame[dest].m_type = RegisterValueType::I16;
            frame[dest].m_I16 = 0;
            break;
        }
        case Opcode::StoreU16: {
            frame[op1].m_type = RegisterValueType::U16_POINTER;
            std::snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", dest, op1, op2);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d, *(u16*)%s = %s", dest, op1_str, op2_str);
            frame[dest].m_type = RegisterValueType::U16;
            frame[dest].m_U16 = 0;
            break;
        }
        case Opcode::StoreI32: {
            frame[op1].m_type = RegisterValueType::I32_POINTER;
            std::snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", dest, op1, op2);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d, *(i32*)%s = %s", dest, op1_str, op2_str);
            frame[dest].m_type = RegisterValueType::I32;
            frame[dest].m_I32 = 0;
            break;
        }
        case Opcode::StoreU32: {
            frame[op1].m_type = RegisterValueType::U32_POINTER;
            std::snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", dest, op1, op2);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d, *(u32*)%s = %s", dest, op1_str, op2_str);
            frame[dest].m_type = RegisterValueType::U32;
            frame[dest].m_U32 = 0;
            break;
        }
        case Opcode::StoreI64: {
            frame[op1].m_type = RegisterValueType::I64_POINTER;
            std::snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", dest, op1, op2);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d, *(i64*)%s = %s", dest, op1_str, op2_str);
            frame[dest].m_type = RegisterValueType::I64;
            frame[dest].m_I64 = 0;
            break;
        }
        case Opcode::StoreU64: {
            frame[op1].m_type = RegisterValueType::U64_POINTER;
            std::snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", dest, op1, op2);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d, *(u64*)%s = %s", dest, op1_str, op2_str);
            frame[dest].m_type = RegisterValueType::U64;
            frame[dest].m_U64 = 0;
            break;
        }
        case Opcode::INotEqual: {
            set_register_types(frame[op1], frame[op2], RegisterValueType::I64);
            std::snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = RegisterValueType::BOOL;
            frame[dest].m_BOOL = frame[op1].m_U64 != frame[op2].m_U64;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = %s != %s", dest, op1_str, op2_str);
            break;
        }
        case Opcode::FNotEqual: {
            set_register_types(frame[op1], frame[op2], RegisterValueType::F32);
            std::snprintf(varying, disassembly_text_size, "r%d, r%d, r%d", dest, op1, op2);
            frame[dest].m_type = RegisterValueType::BOOL;
            frame[dest].m_BOOL = frame[op1].m_F32 != frame[op2].m_F32;
            std::snprintf(interpreted, interpreted_buffer_size, "r%d = %s != %s", dest, op1_str, op2_str);
            break;
        }
        case Opcode::AssertPointer: {
            frame[dest].m_type = RegisterValueType::POINTER;
            std::snprintf(varying, disassembly_text_size,"r%d", dest);
            std::snprintf(interpreted, interpreted_buffer_size, "r%d?", dest);
            break;
        }
        default: {
            std::snprintf(varying, disassembly_text_size, "???");
            std::snprintf(interpreted, interpreted_buffer_size, "UNKNOWN INSTRUCTION");
            break;
        }
    }

    if (table_entry.m_type != SymbolTableEntryType::NONE && op1 == frame.m_symbolTableEntries.size()) {
        frame.m_symbolTableEntries.push_back(table_entry);
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
    char type[512] = {0};

    location table = functionDisassembly.m_stackFrame.m_symbolTable;


    for (u32 i = 0; i < functionDisassembly.m_stackFrame.m_symbolTableEntries.size(); ++i) {
        const SymbolTableEntry& entry = functionDisassembly.m_stackFrame.m_symbolTableEntries[i];
        std::snprintf(line_start, sizeof(line_start), "%04X   0x%06X   ", i, get_offset(table + i * 8));
        switch (entry.m_type) {
            case SymbolTableEntryType::FLOAT: {
                std::snprintf(type, sizeof(type), "float: <%f>\n", entry.m_f32);
                break;
            }
            case SymbolTableEntryType::INT32: {
                std::snprintf(type, sizeof(type), "int: <%i>\n", entry.m_i32);
                break;
            }
            case SymbolTableEntryType::INT64: {
                std::snprintf(type, sizeof(type), "int: <%lli>\n", entry.m_i64);
                break;
            }
            case SymbolTableEntryType::STRING: {
                std::snprintf(type, sizeof(type), "string: \"%s\"\n", reinterpret_cast<const char*>(entry.m_pointer));
                break;
            }
            case SymbolTableEntryType::POINTER: 
            case SymbolTableEntryType::FUNCTION: {
                std::snprintf(type, sizeof(type), "pointer: <%s>\n", lookup(entry.m_pointer));
                break;
            }
            case SymbolTableEntryType::STRINGID_64: {
                std::snprintf(type, sizeof(type), "sid: <%s>\n", lookup(entry.m_hash));
                break;
            }
            case SymbolTableEntryType::NONE:
            case SymbolTableEntryType::UNKNOWN_TYPE: {
                std::snprintf(type, sizeof(type), "UNKNOWN: <%llX>", entry.m_i64);
                break;
            } 
        }
        std::snprintf(buffer, sizeof(buffer), "%s %s", line_start, type);
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