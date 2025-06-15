#include "custom_structs.h"
#include <cmath>
#include <sstream>
#include "disassembler.h"
#include <string.h>

[[nodiscard]] const char *Disassembler::lookup(const sid64 sid) noexcept {
    auto res = this->m_currentFile->sid_cache.find(sid);
    if (res != this->m_currentFile->sid_cache.end()) {
        return res->second.c_str();
    }

    const char *hash_string = this->m_sidbase->search(sid);
    if (hash_string == nullptr) {
        const std::string new_hash_string = int_to_string_id(sid);
        auto [iter, inserted] = this->m_currentFile->sid_cache.emplace(sid, new_hash_string);
        hash_string = iter->second.c_str();
    }
    return hash_string;
}

template<typename... Args>
void Disassembler::insert_span_fmt(const char *format, const TextFormat &text_format, Args ...args) {
    char buffer[256] = {0};
    snprintf(buffer, sizeof(buffer), format, args...);
    this->insert_span(buffer, text_format);
}

[[nodiscard]] b8 Disassembler::is_possible_float(const f32 *val) const noexcept {
    f32 rounded = roundf(*val * 1e5f) / 1e5f;
    return fabsf(*val - rounded) < 1e-5f && *val > -1e5f && *val < 1e5f && rounded != 0.f;
}

void Disassembler::insert_unmapped_struct(const DC_Struct *struct_ptr, const u64 indent) {
    u64 offset = 0;
    b8 offset_gets_pointed_at = false;
    u64 move = 0;
    const char *str_ptr = nullptr;
    while (!offset_gets_pointed_at) {
        offset += move;
        const p64 data_ptr = std::bit_cast<p64>(&struct_ptr->m_data) + offset;
        if (this->m_currentFile->is_file_ptr(data_ptr)) {
            const p64 ptr_loc = *std::bit_cast<const p64*>(data_ptr);
            if (ptr_loc >= this->m_currentFile->m_stringsPtr) {
                this->insert_span("string: ", {.m_color = TYPE_COLOR}, indent);
                this->insert_span_fmt("%s\n", {.m_color = STRING_COLOR}, std::bit_cast<const char*>(ptr_loc));
            } else {
                const DC_Struct *_struct = std::bit_cast<const DC_Struct*>(ptr_loc - 8);
                this->insert_struct(_struct, indent + m_indentPerLevel);
            }
            move = 8;
        }
        else if ((str_ptr = this->m_sidbase->search(*std::bit_cast<const sid64*>(data_ptr))) != nullptr) {
            this->insert_span_fmt("%*ssid: %s\n", {.m_color = HASH_COLOR}, indent, "", str_ptr);
            move = 8;
        }
        else if (this->is_possible_float(std::bit_cast<const f32*>(data_ptr))) {
            this->insert_span_fmt("%*sfloat: %.4f\n", {.m_color = NUM_COLOR}, indent, "", *std::bit_cast<const f32*>(data_ptr));
            move = 4;
        }
        else {
            this->insert_span_fmt("%*sint: %d\n", {.m_color = NUM_COLOR}, indent, "", *std::bit_cast<const i32*>(data_ptr));
            move = 4;
        }
        offset_gets_pointed_at = this->m_currentFile->location_gets_pointed_at((void*)(data_ptr + move + 8));
    }
}

void Disassembler::disassemble() {
    this->insert_header_line();
    const std::string sep = std::string(100, '#') + "\n";
    for (i32 i = 0; i < this->m_currentFile->m_dcheader->m_numEntries; ++i) {
        this->insert_entry(this->m_currentFile->m_dcheader->m_pStartOfData + i);
        this->insert_span(sep.c_str(), {.m_color = COMMENT_COLOR, .m_fontSize = 14});
    }
    this->complete();
}

void Disassembler::insert_entry(const Entry *entry) {
    const DC_Struct *_struct = std::bit_cast<const DC_Struct*>(std::bit_cast<const u64*>(entry->m_entryPtr) - 1);
    this->insert_span_fmt("%s = ", this->ENTRY_HEADER_FMT, this->lookup(entry->m_scriptId));
    this->insert_struct(_struct);
}

void Disassembler::insert_struct(const DC_Struct *entry, const u64 indent) {
    const u64 offset = this->get_offset((void*)entry);
    TextFormat opcode_format = TextFormat{OPCODE_COLOR, 14};
    this->insert_span_fmt("%*s%s [0x%05X] {\n", ENTRY_TYPE_FMT, indent, "", this->lookup(entry->m_typeID), offset);
    

    switch (entry->m_typeID)
    {
        case SID("boolean"):
        {
            this->insert_span_fmt("%*sbool %s: <%s>\n", opcode_format, indent, "", *std::bit_cast<const i32*>(entry->m_data) ? "true" : "false");
            break;
        }
        case SID("int"):
        {
            this->insert_span_fmt("%*sint %s: <%d>\n", opcode_format, indent, "", *std::bit_cast<const i32*>(entry->m_data));
            break;
        }
        case SID("float"):
        {
            this->insert_span_fmt("%*sfloat: <%.2f>\n", opcode_format, indent, "", *std::bit_cast<const i32*>(entry->m_data));
            break;
        }
        case SID("sid"):
        {
            this->insert_span_fmt("%*ssid: <%s>\n", opcode_format, indent, "", this->lookup(*std::bit_cast<const sid64*>(entry->m_data)));
            break;
        }
        case SID("state-script"):
        {
            this->insert_state_script(std::bit_cast<const StateScript*>(&entry->m_data), indent + m_indentPerLevel);
            break;
        }
        case SID("script-lambda"):
        {
            std::unique_ptr<FunctionDisassembly> function = std::make_unique<FunctionDisassembly>(std::move(this->create_function_disassembly(std::bit_cast<const ScriptLambda*>(&entry->m_data))));
            this->insert_function_disassembly_text(*function, indent + m_indentPerLevel);
            this->m_currentFile->m_functions.push_back(std::move(function));
            break;
        }
        case SID("symbol-array"): {
            const dc_structs::symbol_array *array = std::bit_cast<const dc_structs::symbol_array*>(&entry->m_data);
            for (u64 i = 0; i < array->contents.size; ++i) {
                this->insert_span_fmt("%*s%d. %s\n", {OPCODE_COLOR, 14}, indent + m_indentPerLevel, "", i + 1, this->lookup(array->contents.keys[i]));
            }
            break;
        }
        case SID("map"): {
            const dc_structs::map *map = std::bit_cast<const dc_structs::map*>(&entry->m_data);
            this->insert_span_fmt("%*skeys: [0x%05X], values: [0x%05X]\n\n", {HASH_COLOR, 16}, indent + m_indentPerLevel, "", this->get_offset(map->keys.data), this->get_offset(map->values.data));
            for (u64 i = 0; i < map->size; ++i) {
                const char *key_hash = this->lookup(map->keys[i]);
                this->insert_span_fmt("%*s%s {\n", opcode_format, indent + m_indentPerLevel, "", key_hash);
                const DC_Struct *struct_ptr = std::bit_cast<const DC_Struct*>(map->values[i] - 8);
                this->insert_struct(struct_ptr, indent + m_indentPerLevel * 2);
                this->insert_span("}\n", {CONTROL_COLOR, 16}, indent + m_indentPerLevel);
            }
            break;
        }
        default:
        {
            // if (this->m_currentFile->m_emittedStructs.find(std::bit_cast<p64>(entry)) != this->m_currentFile->m_emittedStructs.end()) {
            //     this->insert_span("ALREADY_EMITTED\n", {.m_color = COMMENT_COLOR}, indent);
            //     return;
            // }
            this->insert_unmapped_struct(entry, indent + m_indentPerLevel);
            break;
        }
    }
    this->insert_span("}\n", ENTRY_TYPE_FMT, indent);
    this->m_currentFile->m_emittedStructs.insert(std::bit_cast<p64>(entry));
}

[[nodiscard]] u32 Disassembler::get_offset(const void *symbol) const noexcept {
    return std::bit_cast<p64>(symbol) - std::bit_cast<p64>(this->m_currentFile->m_dcheader);
}

void Disassembler::insert_variable(const SsDeclaration *var, const u32 indent) {
    b8 is_nullptr = var->m_pDeclValue == nullptr;

    constexpr TextFormat var_format = TextFormat{.m_color = VAR_COLOR, .m_fontSize = 14};

    this->insert_span_fmt("%*s[0x%06X] ", TextFormat{.m_color = NUM_COLOR}, indent, "", this->get_offset(var));
    this->insert_span_fmt("%-8s ", TextFormat{.m_color = TYPE_COLOR, .m_fontSize = 14}, this->lookup(var->m_declTypeId));
    this->insert_span_fmt("%-20s = ", TextFormat{.m_color = VAR_COLOR, .m_fontSize = 14},  this->lookup(var->m_declId));

    switch (var->m_declTypeId) {
        case SID("boolean"): {
            if (!is_nullptr) 
                this->insert_span(*std::bit_cast<b8*>(var->m_pDeclValue) ? "true" : "false", TextFormat{.m_color = HASH_COLOR});
            break;
        }
        case SID("vector"): {
            if (!is_nullptr) {
                f32 *val = std::bit_cast<f32*>(var->m_pDeclValue);
                this->insert_span_fmt("(%.4f, %.4f, %.4f, %.4f)", var_format, val[0], val[1], val[2], val[3]);
            }
            break;
        }
        case SID("quat"): {
            if (!is_nullptr) {
                f32 *val = std::bit_cast<f32*>(var->m_pDeclValue);
                this->insert_span_fmt("(%.4f, %.4f, %.4f, %.4f)", var_format, val[0], val[1], val[2], val[3]);
            }
            break;
        }
        case SID("float"): {
            if (!is_nullptr) {
                this->insert_span_fmt("%.4f", var_format, *std::bit_cast<f32*>(var->m_pDeclValue));
            }
            break;
        }
        case SID("string"): {
            if (!is_nullptr) {
                this->insert_span_fmt("%s", var_format, *std::bit_cast<const char**>(var->m_pDeclValue));
            }
            break;
        }
        case SID("symbol"): {
            if (!is_nullptr) {
                this->insert_span(this->lookup(*std::bit_cast<sid64*>(var->m_pDeclValue)), var_format);
            }
            break;
        }
        case SID("int32"): {
            if (!is_nullptr) {
                this->insert_span_fmt("%i", var_format, *std::bit_cast<i32*>(var->m_pDeclValue));
            }
            break;
        }
        case SID("uint64"): {
            if (!is_nullptr) {
                this->insert_span_fmt("%llx", var_format, *std::bit_cast<u64*>(var->m_pDeclValue));
            }
            break;
        }
        case SID("timer"): {
            if (!is_nullptr) {
                this->insert_span_fmt("%f", var_format, *std::bit_cast<f32*>(var->m_pDeclValue));
            }
            break;
        }
        case SID("point"): {
            if (!is_nullptr) {
                f32 *val = std::bit_cast<f32*>(var->m_pDeclValue);
                this->insert_span_fmt("(%.2f, %.2f, %.2f)", var_format, val[0], val[1], val[2]);
            }
            break;
        }
        case SID("bound-frame"): {
            if (!is_nullptr) {
                this->insert_span_fmt("%f", var_format, *std::bit_cast<f32*>(var->m_pDeclValue));
            }
            break;
        }
        default: {
            this->insert_span("???", var_format);
            break;
        }
    }
    if (is_nullptr) {
        this->insert_span("uninitialized");
    }
    this->insert_span("\n");
   // this->insert_span_fmt("%*s<%s>\n", TextFormat{OPCODE_COLOR, 12}, 8, "", var_contents);
}

void Disassembler::insert_on_block(const SsOnBlock *block, const u32 indent) {
    TextFormat block_format{COMMENT_COLOR, 12};
    switch (block->m_blockType) {
        case 0: {
            this->insert_span("ON start {\n", block_format, indent);
            break;
        }
        case 1: {
            this->insert_span("ON end {\n", block_format, indent);
            break;
        }
        case 2: {
            this->insert_span_fmt("%*sON event %s {\n", block_format, indent, "", this->lookup(block->m_blockEventId));
            break;
        }
        case 3: {
            this->insert_span("ON update {\n", block_format, indent);
            break;
        }
        case 4: {
            this->insert_span("ON virtual {\n", block_format, indent);
            break;
        }
        default: {
            this->insert_span_fmt("%*sON UNKNOWN [%d] {\n", block_format, indent, "", block->m_blockType);
            break;
        }
    }

    for (i16 i = 0; i < block->m_trackGroup.m_numTracks; ++i) {
        SsTrack *track_ptr = block->m_trackGroup.m_aTracks + i;
        this->insert_span_fmt("%*sTRACK %s {\n", {COMMENT_COLOR, 12}, indent + m_indentPerLevel, "", this->lookup(track_ptr->m_trackId));
        for (i16 j = 0; j < track_ptr->m_totalLambdaCount; ++j) {
            this->insert_span("{\n", COMMENT_FMT, indent + m_indentPerLevel * 2);
            std::unique_ptr<FunctionDisassembly> function = std::make_unique<FunctionDisassembly>(std::move(this->create_function_disassembly(track_ptr->m_pSsLambda[j].m_pScriptLambda)));
            this->insert_function_disassembly_text(*function, indent + m_indentPerLevel * 3);
            this->m_currentFile->m_functions.push_back(std::move(function));
            this->insert_span("}\n", COMMENT_FMT, indent + m_indentPerLevel * 2);
        }
        this->insert_span("}\n\n", COMMENT_FMT, indent + m_indentPerLevel);
    }

    this->insert_span("}\n", COMMENT_FMT, indent);
}

void Disassembler::insert_state_script(const StateScript *stateScript, const u32 indent) {
    TextFormat header_format{COMMENT_COLOR, 14};

    if (stateScript->m_pSsOptions != nullptr && stateScript->m_pSsOptions->m_pSymbolArray != nullptr) {
        SymbolArray *s_array = stateScript->m_pSsOptions->m_pSymbolArray;
        this->insert_span("OPTIONS: ", header_format, indent);
        for (i32 i = 0; i < s_array->m_numEntries; ++i) {
            this->insert_span(this->lookup(s_array->m_pSymbols[i]), header_format, indent + m_indentPerLevel);
            this->insert_span("\n");
        }
    }

    SsDeclarationList *decl_table = stateScript->m_pSsDeclList;
    if (decl_table != nullptr) {
        this->insert_span("DECLARATIONS: \n", header_format, indent);
        for (i32 i = 0; i < decl_table->m_numDeclarations; ++i) {
            SsDeclaration *decl = decl_table->m_pDeclarations + i;
            if (decl->m_isVar) {
                this->insert_variable(decl, indent + m_indentPerLevel);
            }
        }
    }
    for (i16 i = 0; i < stateScript->m_stateCount; ++i) {
        SsState *state_ptr = stateScript->m_pSsStateTable + i;
        const char *state_name = this->lookup(state_ptr->m_stateId);
        this->insert_span_fmt("%*sSTATE %s {\n", header_format, indent + m_indentPerLevel, "", state_name);
        for (i64 j = 0; j < state_ptr->m_numSsOnBlocks; ++j) {
            this->insert_on_block(state_ptr->m_pSsOnBlocks + j, indent + m_indentPerLevel * 2);
        }
        this->insert_span_fmt("%*s} END STATE %s\n\n", COMMENT_FMT, indent + m_indentPerLevel, "", state_name);
    }
}

FunctionDisassembly Disassembler::create_function_disassembly(const ScriptLambda *lambda) {
    Instruction *instructionPtr = std::bit_cast<Instruction*>(lambda->m_pOpcode);
    u64 instructionCount = std::bit_cast<Instruction*>(lambda->m_pSymbols) - instructionPtr;

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
        this->process_instruction(functionDisassembly.m_stackFrame, functionDisassembly.m_lines[i]);
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
            this->get_offset((void*)(line.m_globalPointer + line.m_location)),
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

    stackFrame.to_string(dst_str, interpreted_buffer_size, istr.destination < 128 ? istr.destination : 0, this->lookup(dest.m_SID));
    stackFrame.to_string(op1_str, interpreted_buffer_size, istr.operand1 < 128 ? istr.operand1 : 0, this->lookup(op1.m_SID));
    stackFrame.to_string(op2_str, interpreted_buffer_size, istr.operand2 < 128 ? istr.operand2 : 0, this->lookup(op2.m_SID));

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
            const i64 table_value = std::bit_cast<i64*>(stackFrame.m_symbolTablePtr)[istr.operand1];
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
            const f32 table_value = std::bit_cast<f32*>(stackFrame.m_symbolTablePtr)[istr.operand1];
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
            const p64 table_value = std::bit_cast<p64*>(stackFrame.m_symbolTablePtr)[istr.operand1];
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
            dest.m_type = RegisterValueType::R_I32;
            dest.m_I32 = 0;
            snprintf(interpreted, interpreted_buffer_size, "r%d = [0x%llx + 0x%llx]", istr.destination, op1.m_PTR.m_base, op1.m_PTR.m_offset);
            break;
        }
        case LoadFloat: {
            snprintf(varying, disassembly_text_size,"r%d, [r%d]", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_F32;
            dest.m_F32 = 0.f;
            snprintf(interpreted, interpreted_buffer_size, "r%d = [0x%llx + 0x%llx]", istr.destination, op1.m_PTR.m_base, op1.m_PTR.m_offset);
            break;
        }
        case LoadPointer: {
            snprintf(varying, disassembly_text_size,"r%d, [r%d]", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_POINTER;
            dest.m_PTR = {0, 0};
            snprintf(interpreted, interpreted_buffer_size, "r%d = [0x%llx + 0x%llx]", istr.destination, op1.m_PTR.m_base, op1.m_PTR.m_offset);
            break;
        }
        case LoadI64: {
            snprintf(varying, disassembly_text_size,"r%d, [r%d]", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_I64;
            dest.m_I64 = 0;
            snprintf(interpreted, interpreted_buffer_size, "r%d = [0x%llx + 0x%llx]", istr.destination, op1.m_PTR.m_base, op1.m_PTR.m_offset);
            break;
        }
        case LoadU64: {
            snprintf(varying, disassembly_text_size,"r%d, [r%d]", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_U64;
            dest.m_U64 = 0;
            snprintf(interpreted, interpreted_buffer_size, "r%d = [0x%llx + 0x%llx]", istr.destination, op1.m_PTR.m_base, op1.m_PTR.m_offset);
            break;
        }
        case StoreInt: {
            snprintf(varying, disassembly_text_size,"[r%d], r%d", istr.destination, istr.operand1);
            break;
        }
        case LookupInt: {
            snprintf(varying, disassembly_text_size,"r%d, %d", istr.destination, istr.operand1);
            i64 value = std::bit_cast<i64*>(stackFrame.m_symbolTablePtr)[istr.operand1]; 
            dest.m_type = RegisterValueType::R_I64;
            dest.m_I64 = value;
            table_entry.m_type = SymbolTableEntryType::POINTER;
            table_entry.m_i64 = value;
            snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%s>", istr.destination, istr.operand1, this->lookup(value));
            break;
        }
        case LookupFloat: {
            snprintf(varying, disassembly_text_size,"r%d, %d", istr.destination, istr.operand1);
            f32 value = *std::bit_cast<f32*>(stackFrame.m_symbolTablePtr + istr.operand1); 
            dest.m_type = RegisterValueType::R_F32;
            dest.m_F32 = value;
            table_entry.m_type = SymbolTableEntryType::FLOAT;
            table_entry.m_f32 = value;
            snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%.2f>", istr.destination, istr.operand1, value);
            break;
        }
        case LookupPointer: {
            snprintf(varying, disassembly_text_size,"r%d, %d", istr.destination, istr.operand1);
            const p64 value = std::bit_cast<p64*>(stackFrame.m_symbolTablePtr)[istr.operand1];
            dest.m_type = RegisterValueType::R_POINTER;
            dest.m_PTR.m_base = value;
            dest.m_PTR.m_offset = 0;
            table_entry.m_type = SymbolTableEntryType::POINTER;
            table_entry.m_pointer = value;
            if (this->m_currentFile->is_file_ptr(reinterpret_cast<p64>(stackFrame.m_symbolTablePtr + istr.operand1))) {
               snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%s>", istr.destination, istr.operand1, reinterpret_cast<const char*>(value));
            } else {
                snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%s>", istr.destination, istr.operand1, this->lookup(value));
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
            snprintf(interpreted, interpreted_buffer_size, "r%d = r%d <%s>", istr.destination, istr.operand1, this->lookup(op1.m_PTR.get()));
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
            snprintf(comment_str, sizeof(comment_str), "r%d = %s(", istr.destination, this->lookup(op1.m_PTR.get()));
            for (u64 i = 0; i < istr.operand2; ++i) {
                if (i != 0) {
                    strncat(comment_str, ", ", sizeof(comment_str) - strlen(comment_str));
                }
                stackFrame.to_string(dst_str, interpreted_buffer_size, 49 + i, this->lookup(stackFrame[i + 49].m_SID));
                strncat(comment_str, dst_str, sizeof(comment_str) - strlen(comment_str));
            }
            dest.m_type = R_HASH;
            dest.isReturn = true;
            snprintf(interpreted, interpreted_buffer_size, "%s)", comment_str);
            break;
        }
        case CallFf: {
            snprintf(varying, disassembly_text_size,"r%d, r%d, %d", istr.destination, istr.operand1, istr.operand2);
            char comment_str[200] = {0};
            snprintf(comment_str, sizeof(comment_str), "r%d = %s(", istr.destination, this->lookup(op1.m_PTR.get()));
            for (u64 i = 0; i < istr.operand2; ++i) {
                if (i != 0) {
                    strncat(comment_str, ", ", sizeof(comment_str) - strlen(comment_str));
                }
                stackFrame.to_string(dst_str, interpreted_buffer_size, 49 + i, this->lookup(stackFrame[i + 49].m_SID));
                strncat(comment_str, dst_str, sizeof(comment_str) - strlen(comment_str));
            }
            dest.m_type = R_HASH;
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
            if (op1.m_type == R_POINTER) {
                dest.m_type = R_POINTER;
                dest.m_PTR = {op1.m_PTR.m_base, istr.operand2};
            } else {
                dest.m_type = R_I64;
                dest.m_I64 = op1.m_I64 + istr.operand2;
            }
            stackFrame.to_string(op1_str, interpreted_buffer_size, istr.operand1);
            snprintf(interpreted, interpreted_buffer_size, "r%d = r%d [%s] + %d -> <%s>", istr.destination, istr.operand1, op1_str, istr.operand2, op1_str);
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
            i32 value = *std::bit_cast<i32*>(stackFrame.m_symbolTablePtr + istr.operand1); 
            dest.m_type = RegisterValueType::R_I32;
            dest.m_I32 = value;
            table_entry.m_type = SymbolTableEntryType::INT;
            table_entry.m_i64 = value;
            snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%d>", istr.destination, istr.operand1, value);
            break;
        }
        case LoadStaticFloatImm: {
            snprintf(varying, disassembly_text_size,"r%d, %d", istr.destination, istr.operand1);
            f32 value = *std::bit_cast<f32*>(stackFrame.m_symbolTablePtr + istr.operand1); 
            dest.m_type = RegisterValueType::R_F32;
            dest.m_F32 = value;
            table_entry.m_type = SymbolTableEntryType::FLOAT;
            table_entry.m_f32 = value;
            snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%.2f>", istr.destination, istr.operand1, value);
            break;
        }
        case LoadStaticPointerImm: {
            snprintf(varying, disassembly_text_size,"r%d, %d", istr.destination, istr.operand1);
            p64 value = std::bit_cast<p64*>(stackFrame.m_symbolTablePtr)[istr.operand1]; 
            if (value >= this->m_currentFile->m_stringsPtr) {
                snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> \"%s\"", istr.destination, istr.operand1, reinterpret_cast<const char*>(value));
                dest.m_type = RegisterValueType::R_STRING;
                dest.m_PTR = {value, 0};
                table_entry.m_type = SymbolTableEntryType::STRING;
                table_entry.m_pointer = value;
            } else {
                snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <0x%d>", istr.destination, istr.operand1, this->get_offset((void*)value));
                dest.m_type = RegisterValueType::R_POINTER;
                dest.m_PTR = {value, 0};
                table_entry.m_type = SymbolTableEntryType::POINTER;
                table_entry.m_pointer = value;
            }
            break;
        }
        case LoadStaticI64Imm: {
            snprintf(varying, disassembly_text_size,"r%d, %d", istr.destination, istr.operand1);
            i64 value = std::bit_cast<i64*>(stackFrame.m_symbolTablePtr)[istr.operand1]; 
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
                const char *hash_str = this->lookup(value);
                dest.m_type = RegisterValueType::R_HASH;
                dest.m_SID = value;
                table_entry.m_type = SymbolTableEntryType::STRINGID_64;
                table_entry.m_hash = value;
                stackFrame.to_string(dst_str, interpreted_buffer_size, istr.destination, hash_str);
                snprintf(interpreted, interpreted_buffer_size, "r%d = ST[%d] -> <%s>", istr.destination, istr.operand1, dst_str);
            } else if (this->m_currentFile->is_file_ptr(reinterpret_cast<p64>(stackFrame.m_symbolTablePtr + istr.operand1))) {
                printf("here");
            }
            break;
        }
        case Move: {
            snprintf(varying, disassembly_text_size,"r%d, r%d", istr.destination, istr.operand1);

            dest = op1;
            
            if (op1.isArg) {
                dest.argNum = op1.argNum;
            }
            
            snprintf(interpreted, interpreted_buffer_size, "r%d = %s", istr.destination, op1_str);
            break;
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
            this->insert_span("L_RETURN:\n", label_format, indent - 2);
        } else if (line.m_location == func_size - 1 && line.m_instruction.opcode == Opcode::LoadU16Imm) {
            this->insert_span_fmt("%*sL_RETURN_%d:\n", label_format, indent - 2, "", line.m_instruction.operand1);  
        } else {
            this->insert_span_fmt("%*sL_%d:\n", label_format, indent - 2, "", label_index);  
        }
    }
}

void Disassembler::insert_goto_label(const std::vector<u32> &labels, const FunctionDisassemblyLine &line, const u32 func_size, const std::vector<FunctionDisassemblyLine> &lines) noexcept {
    if (line.m_label != -1) {
        u32 target = std::distance(labels.begin(), std::find(labels.begin(), labels.end(), line.m_label));
        if (line.m_label == func_size) {
            this->insert_span("=> L_RETURN", COMMENT_FMT);
        } else if (line.m_label == func_size - 1 && lines[line.m_label].m_instruction.opcode == Opcode::LoadU16Imm) {
            this->insert_span_fmt("=> L_RETURN_%d", COMMENT_FMT, lines[line.m_label].m_instruction.operand1);
        } else {
            this->insert_span_fmt("=> L_%d", COMMENT_FMT, target);
        }
    }
}

void Disassembler::insert_function_disassembly_text(const FunctionDisassembly &functionDisassembly, const u32 indent) {
    auto labels = functionDisassembly.m_stackFrame.m_labels;
    char buffer[512] = {0};
    constexpr TextFormat text_format = {OPCODE_COLOR, 12};
    
    if (functionDisassembly.m_stackFrame.m_argCount > 0) {
        this->insert_span_fmt("%*s[%d args]\n", COMMENT_FMT, indent, "", functionDisassembly.m_stackFrame.m_argCount);
    }
    
    for (const auto &line : functionDisassembly.m_lines) {
        u32 line_offset = std::max(67ull - line.m_text.length(), 0ull);
        this->insert_label(labels, line, functionDisassembly.m_lines.size() - 1, indent);
        this->insert_span(line.m_text.c_str(), text_format, indent);
        const std::string comment = std::string(line_offset, ' ') + line.m_comment;
        this->insert_span(comment.c_str(), {STRING_COLOR, 12});
        this->insert_goto_label(labels, line, functionDisassembly.m_lines.size() - 1, functionDisassembly.m_lines);
        this->insert_span("\n");
    }
    this->insert_span("\n");
    this->insert_span("SYMBOL TABLE: \n", {COMMENT_COLOR, 12}, indent);

    char line_start[64] = {0};
    char type[512] = {0};

    u64 *table_ptr = functionDisassembly.m_stackFrame.m_symbolTablePtr;

    for (u32 i = 0; i < functionDisassembly.m_stackFrame.m_symbolTable.size(); ++i) {
        const SymbolTableEntry &entry = functionDisassembly.m_stackFrame.m_symbolTable.at(i);
        snprintf(line_start, sizeof(line_start), "%04X   0x%06X   ", i, this->get_offset(table_ptr + i));
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
                snprintf(type, sizeof(type), "pointer: <%s>\n", this->lookup(entry.m_pointer));
                break;
            }
            case SymbolTableEntryType::STRINGID_64: {
                snprintf(type, sizeof(type), "sid: <%s>\n", this->lookup(entry.m_hash));
                break;
            }
            case SymbolTableEntryType::NONE:
            case SymbolTableEntryType::UNKNOWN_TYPE: {
                snprintf(type, sizeof(type), "UNKNOWN: <%llX>", entry.m_i64);
                break;
            } 
        }
        snprintf(buffer, sizeof(buffer), "%s %s", line_start, type);
        this->insert_span(buffer, {COMMENT_COLOR, 12}, indent);
    }
}

void Disassembler::insert_header_line() {
    const char* current_script_name;
    const char* current_script_id;
    const TextFormat header_format = {STRING_COLOR, 14};
    if (this->m_currentFile->m_dcscript != nullptr) {
        current_script_name = this->lookup(this->m_currentFile->m_dcscript->m_stateScriptId);
        current_script_id = int_to_string_id(this->m_currentFile->m_dcscript->m_stateScriptId).c_str();
    } else {
        current_script_name = "UNKNOWN SCRIPT";
        current_script_id = "UNKNOWN SCRIPT ID";
    }
    this->insert_span_fmt("%*sDeepQuantum's DC Disassembler ver. %d\n", header_format, 14, "", m_versionNumber);
    this->insert_span_fmt("%*sListing for script: %s\n", header_format, 14, "", current_script_name);
    this->insert_span_fmt("%*sScript ID: %s\n", header_format, 14, "", current_script_id);
    this->insert_span_fmt("%*sFilesize: %d bytes\n", header_format, 14, "", this->m_currentFile->m_size);
    this->insert_span("START OF DISASSEMBLY\n", header_format, 14);
    this->insert_span("--------------------------------------------------\n", header_format, 14);
}