#include "custom_structs.h"
#include "../mainwindow.h"
#include <stdexcept>
#include <cmath>
#include <sstream>
#include <QScrollBar>
#include "listing_view_controller.h"
#include <bit>

ListingViewController::ListingViewController(BinaryFile *file, MainWindow *mainWindow, const SIDBase *sidbase, FILE *outfptr) {
    this->m_mainWindow = mainWindow;
    this->m_currentFile = file;
    this->m_sidbase = sidbase;
    this->m_outfptr = outfptr;
}

[[nodiscard]] const char *ListingViewController::lookup(const sid64 sid) noexcept {
    auto res = this->m_currentFile->sid_cache.find(sid);
    if (res != this->m_currentFile->sid_cache.end()) {
        return res->second.c_str();
    }

    const char *hash_string = this->m_sidbase->search(sid);
    if (hash_string == nullptr) {
        const std::string new_hash_string = int_to_string_id(sid);
        auto [iter, inserted] = this->m_currentFile->sid_cache.emplace(sid, std::move(new_hash_string));
        hash_string = iter->second.c_str();
    }
    return hash_string;
}

template<typename... Args>
void ListingViewController::insert_span_fmt(const char *format, const TextFormat &text_format, Args ...args) {
    char buffer[256] = {0};
    snprintf(buffer, sizeof(buffer), format, args...);
    this->insert_span(buffer, text_format);
}

void ListingViewController::insert_span(const char *line, const TextFormat &text_format, const u64 indent) {
    if (this->m_outfptr != nullptr) {
        if (indent != 0) {
            this->m_outbuf += std::string(indent, ' ');
        }
        this->m_outbuf += line;
        return;
    }
    auto* view = this->m_mainWindow->get_listing_view();
    QTextCursor cursor = view->textCursor();
    cursor.movePosition(QTextCursor::End);
    view->setTextCursor(cursor);
    QTextCharFormat format;
    format.setForeground(text_format.m_color);
    format.setFontPointSize(text_format.m_fontSize);
    if (indent != 0) {
        cursor.insertText(QString(indent, ' ') + QString::fromStdString(line), format);
    } else {
        cursor.insertText(QString::fromStdString(line), format);
    }
}

b8 ListingViewController::is_possible_float(const f32 *val) {
    f32 rounded = roundf(*val * 1e5f) / 1e5f;
    return fabsf(*val - rounded) < 1e-5f && *val > -1e5f && *val < 1e5f && rounded != 0.f;
}

void ListingViewController::insert_unmapped_struct(const DC_Struct *struct_ptr, const u64 indent) {
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
                this->insert_span("string: ", {.m_color = MainWindow::TYPE_COLOR}, indent);
                this->insert_span_fmt("%s\n", {.m_color = MainWindow::STRING_COLOR}, std::bit_cast<const char*>(ptr_loc));
            } else {
                const DC_Struct *_struct = std::bit_cast<const DC_Struct*>(ptr_loc - 8);
                //this->insert_span_fmt("%*s%s* -> [0x%05X] {\n", {.m_color = MainWindow::TYPE_COLOR}, indent, "", this->lookup(_struct->m_typeID), this->get_offset(_struct) + 8);
                this->insert_struct(_struct, indent + m_indentPerLevel);
                //this->insert_span("}\n", {.m_color = MainWindow::TYPE_COLOR}, indent);
            }
            move = 8;
        }
        else if ((str_ptr = this->m_sidbase->search(*std::bit_cast<const sid64*>(data_ptr))) != nullptr) {
            this->insert_span_fmt("%*ssid: %s\n", {.m_color = MainWindow::HASH_COLOR}, indent, "", str_ptr);
            move = 8;
        }
        else if (this->is_possible_float(std::bit_cast<const f32*>(data_ptr))) {
            this->insert_span_fmt("%*sfloat: %.4f\n", {.m_color = MainWindow::NUM_COLOR}, indent, "", *std::bit_cast<const f32*>(data_ptr));
            move = 4;
        }
        else {
            this->insert_span_fmt("%*sint: %d\n", {.m_color = MainWindow::NUM_COLOR}, indent, "", *std::bit_cast<const i32*>(data_ptr));
            move = 4;
        }
        offset_gets_pointed_at = this->m_currentFile->location_gets_pointed_at((void*)(data_ptr + move + 8));
    }
}

void ListingViewController::create_listing_view() {
    this->insert_header_line();
    const std::string sep = std::string(100, '#') + "\n";
    for (i32 i = 0; i < this->m_currentFile->m_dcheader->m_numEntries; ++i) {
        this->insert_entry(this->m_currentFile->m_dcheader->m_pStartOfData + i);
        this->insert_span(sep.c_str(), {.m_color = MainWindow::COMMENT_COLOR, .m_fontSize = 14});
    }

    if (this->m_mainWindow != nullptr) {
        this->m_mainWindow->get_listing_view()->moveCursor(QTextCursor::Start);
        this->m_mainWindow->get_listing_view()->ensureCursorVisible();
    }
    else {
        fwrite(this->m_outbuf.c_str(), sizeof(char), this->m_outbuf.size(), this->m_outfptr);
    }
}

void ListingViewController::insert_entry(const Entry *entry) {
    const DC_Struct *_struct = std::bit_cast<const DC_Struct*>(std::bit_cast<const u64*>(entry->m_entryPtr) - 1);
    this->insert_span_fmt("%s = ", this->ENTRY_HEADER_FMT, this->lookup(entry->m_scriptId));
    this->insert_struct(_struct);
}

void ListingViewController::insert_struct(const DC_Struct *entry, const u64 indent) {
    const u64 offset = this->get_offset((void*)entry);
    TextFormat opcode_format = TextFormat{MainWindow::OPCODE_COLOR, 14};
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
                this->insert_span_fmt("%*s%d. %s\n", {MainWindow::OPCODE_COLOR, 14}, indent + m_indentPerLevel, "", i + 1, this->lookup(array->contents.keys[i]));
            }
            break;
        }
        case SID("map"): {
            const dc_structs::map *map = std::bit_cast<const dc_structs::map*>(&entry->m_data);
            this->insert_span_fmt("%*skeys: [0x%05X], values: [0x%05X]\n\n", {MainWindow::HASH_COLOR, 16}, indent + m_indentPerLevel, "", this->get_offset(map->keys.data), this->get_offset(map->values.data));
            for (u64 i = 0; i < map->size; ++i) {
                const char *key_hash = this->lookup(map->keys[i]);
                this->insert_span_fmt("%*s%s {\n", opcode_format, indent + m_indentPerLevel, "", key_hash);
                const DC_Struct *struct_ptr = std::bit_cast<const DC_Struct*>(map->values[i] - 8);
                this->insert_struct(struct_ptr, indent + m_indentPerLevel * 2);
                this->insert_span("}\n", {MainWindow::CONTROL_COLOR, 16}, indent + m_indentPerLevel);
            }
            break;
        }
        default:
        {
            // if (this->m_currentFile->m_emittedStructs.find(std::bit_cast<p64>(entry)) != this->m_currentFile->m_emittedStructs.end()) {
            //     this->insert_span("ALREADY_EMITTED\n", {.m_color = MainWindow::COMMENT_COLOR}, indent);
            //     return;
            // }
            this->insert_unmapped_struct(entry, indent + m_indentPerLevel);
            break;
        }
    }
    this->insert_span("}\n", ENTRY_TYPE_FMT, indent);
    this->m_currentFile->m_emittedStructs.insert(std::bit_cast<p64>(entry));
}

u32 ListingViewController::get_offset(const void *symbol) {
    return std::bit_cast<p64>(symbol) - std::bit_cast<p64>(this->m_currentFile->m_dcheader);
}

void ListingViewController::insert_variable(const SsDeclaration *var, const u32 indent) {
    char var_contents[36] = {0};

    b8 is_nullptr = var->m_pDeclValue == nullptr;

    const char *type_name = this->lookup(var->m_declTypeId);
    const char *var_name = this->lookup(var->m_declId);

    this->insert_span_fmt("[0x%06X]", TextFormat{.m_color = MainWindow::NUM_COLOR}, this->get_offset(var));
    this->insert_span(type_name, TextFormat{.m_color = MainWindow::TYPE_COLOR, .m_fontSize = 14});
    this->insert_span(var_name, TextFormat{.m_color = MainWindow::VAR_COLOR, .m_fontSize = 14});

    switch (var->m_declTypeId) {
        case SID("boolean"): {
            if (!is_nullptr) 
                this->insert_span(*std::bit_cast<b8*>(var->m_pDeclValue) == 1 ? "TRUE" : "FALSE", TextFormat{.m_color = MainWindow::HASH_COLOR}, indent);
            break;
        }
        case SID("vector"): {
            if (!is_nullptr) {
                f32 *val = std::bit_cast<f32*>(var->m_pDeclValue);
                sprintf(var_contents, "(%.4f, %.4f, %.4f, %.4f)", val[0], val[1], val[2], val[3]);
            }
            break;
        }
        case SID("quat"): {
            if (!is_nullptr) {
                f32 *val = std::bit_cast<f32*>(var->m_pDeclValue);
                sprintf(var_contents, "(%.4f, %.4f, %.4f, %.4f)", val[0], val[1], val[2], val[3]);
            }
            break;
        }
        case SID("float"): {
            if (!is_nullptr) {
                sprintf(var_contents, "%.4f", *std::bit_cast<f32*>(var->m_pDeclValue));
            }
            break;
        }
        case SID("string"): {
            if (!is_nullptr) {
                sprintf(var_contents, "%s", *std::bit_cast<const char**>(var->m_pDeclValue));
            }
            break;
        }
        case SID("symbol"): {
            if (!is_nullptr) {
                sprintf(var_contents, "%s", this->lookup(*std::bit_cast<sid64*>(var->m_pDeclValue)));
            }
            break;
        }
        case SID("int32"): {
            if (!is_nullptr) {
                sprintf(var_contents, "%d", *std::bit_cast<i32*>(var->m_pDeclValue));
            }
            break;
        }
        case SID("uint64"): {
            if (!is_nullptr) {
                sprintf(var_contents, "%llX", *std::bit_cast<u64*>(var->m_pDeclValue));
            }
            break;
        }
        case SID("timer"): {
            if (!is_nullptr) {
                sprintf(var_contents, "%f", *std::bit_cast<f32*>(var->m_pDeclValue));
            }
            break;
        }
        case SID("point"): {
            if (!is_nullptr) {
                f32 *val = std::bit_cast<f32*>(var->m_pDeclValue);
                sprintf(var_contents, "(%.2f, %.2f, %.2f)", val[0], val[1], val[2]);
            }
            break;
        }
        case SID("bound-frame"): {
            if (!is_nullptr) {
                sprintf(var_contents, "%f", *std::bit_cast<f32*>(var->m_pDeclValue));
            }
            break;
        }
        default: {
            strcpy(var_contents, "???");
            break;
        }
    }
    if (is_nullptr) {
        strcpy(var_contents, "UNINITIALIZED");
    }
    this->insert_span_fmt("%*s<%s>\n", TextFormat{MainWindow::OPCODE_COLOR, 12}, 8, "", var_contents);
}

void ListingViewController::insert_on_block(const SsOnBlock *block, const u32 indent) {
    TextFormat block_format{MainWindow::COMMENT_COLOR, 12};
    switch (block->m_blockType) {
        case 0: {
            this->insert_span("ON start\n", block_format, indent);
            break;
        }
        case 1: {
            this->insert_span("ON end\n", block_format, indent);
            break;
        }
        case 2: {
            this->insert_span_fmt("%*sON event %s\n", block_format, indent, "", this->lookup(block->m_blockEventId));
            break;
        }
        case 3: {
            this->insert_span("ON update\n", block_format, indent);
            break;
        }
        case 4: {
            this->insert_span("ON virtual\n", block_format, indent);
            break;
        }
        default: {
            this->insert_span_fmt("%*sON UNKNOWN [%d]\n", block_format, indent, "", block->m_blockType);
            break;
        }
    }

    for (i16 i = 0; i < block->m_trackGroup.m_numTracks; ++i) {
        SsTrack *track_ptr = block->m_trackGroup.m_aTracks + i;
        this->insert_span_fmt("%*sTRACK %s:\n", {MainWindow::COMMENT_COLOR, 12}, indent + m_indentPerLevel, "", this->lookup(track_ptr->m_trackId));
        for (i16 j = 0; j < track_ptr->m_totalLambdaCount; ++j) {
            std::unique_ptr<FunctionDisassembly> function = std::make_unique<FunctionDisassembly>(std::move(this->create_function_disassembly(track_ptr->m_pSsLambda[j].m_pScriptLambda)));
            this->insert_function_disassembly_text(*function, indent + m_indentPerLevel * 2);
            this->m_currentFile->m_functions.push_back(std::move(function));
        }
    }
}

void ListingViewController::insert_state_script(const StateScript *stateScript, const u32 indent) {
    TextFormat header_format{MainWindow::COMMENT_COLOR, 14};

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
        this->insert_span_fmt("%*sSTATE %s:\n", header_format, indent + m_indentPerLevel, "", this->lookup(state_ptr->m_stateId));
        for (i64 j = 0; j < state_ptr->m_numSsOnBlocks; ++j) {
            this->insert_on_block(state_ptr->m_pSsOnBlocks + j, indent + m_indentPerLevel * 2);
        }
    }
}

FunctionDisassembly ListingViewController::create_function_disassembly(const ScriptLambda *lambda) {
    // this->insert_span("INSTRUCTION POINTER: " + MainWindow::offset_to_string(this->get_offset(lambda->m_pOpcode)) + "\n", MainWindow::COMMENT_COLOR, 14, 20);
    // this->insert_span("SYMBOL TABLE POINTER: " + MainWindow::offset_to_string(this->get_offset(lambda->m_pSymbols)) + "\n", MainWindow::COMMENT_COLOR, 14, 20);

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

    for (u64 i = 0; i < instructionCount; ++i) {
        this->process_instruction(functionDisassembly.m_stackFrame, functionDisassembly.m_lines[i]);
    }
    return functionDisassembly;
}

void ListingViewController::process_instruction(StackFrame &stackFrame, FunctionDisassemblyLine &line) {
    char disassembly_text[256] = {0};
    char interpreted[128] = {0};
    const Instruction istr = line.m_instruction;
    SymbolTableEntry table_entry;
    table_entry.m_type = NONE;
    sprintf(disassembly_text, "%04llX   0x%06X   %02X %02X %02X %02X   %-21s",
            line.m_location,
            this->get_offset((void*)(line.m_globalPointer + line.m_location)),
            istr.opcode,
            istr.destination,
            istr.operand1,
            istr.operand2,
            istr.opcode_to_string()
    );
    char *varying = disassembly_text + strlen(disassembly_text);

    Register &dest = stackFrame[istr.destination < 128 ? istr.destination : 0];
    Register &op1 = stackFrame[istr.operand1 < 128 ? istr.operand1 : 0];
    Register &op2 = stackFrame[istr.operand2 < 128 ? istr.operand2 : 0];

    char dst_str[128] = {0}; 
    char op1_str[128] = {0}; 
    char op2_str[128] = {0}; 

    stackFrame.to_string(dst_str, istr.destination < 128 ? istr.destination : 0, this->lookup(dest.m_SID));
    stackFrame.to_string(op1_str, istr.operand1 < 128 ? istr.operand1 : 0, this->lookup(op1.m_SID));
    stackFrame.to_string(op2_str, istr.operand2 < 128 ? istr.operand2 : 0, this->lookup(op2.m_SID));

    dest.isReturn = false;

    switch (istr.opcode) {
        case Return: {
            sprintf(varying, "r%d", istr.destination);
            sprintf(interpreted, "Return %s", dst_str);
            break;
        }
        case IAdd: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            if (op1.m_type == RegisterValueType::R_POINTER) {
                dest.m_type = RegisterValueType::R_POINTER;
                dest.m_PTR = op1.m_PTR;
            } else {
                dest.m_type = RegisterValueType::R_I64;
                dest.m_I64 = op1.m_I64 + op2.m_I64;
            }
            stackFrame.to_string(dst_str, istr.destination);
            sprintf(interpreted, "%s = %s + %s", dst_str, op1_str, op2_str);
            break;
        }
        case ISub: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = RegisterValueType::R_I64;
            dest.m_I64 = op1.m_I64 - op2.m_I64;
            stackFrame.to_string(dst_str, istr.destination);
            sprintf(interpreted, "%s = %s - %s", dst_str, op1_str, op2_str);
            break;
        }
        case IMul: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = RegisterValueType::R_I64;
            dest.m_I64 = op1.m_I64 * op2.m_I64;
            stackFrame.to_string(dst_str, istr.destination);
            sprintf(interpreted, "%s = %s * %s", dst_str, op1_str, op2_str);
            break;
        }
        case IDiv: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = RegisterValueType::R_I64;
            if (op2.m_I64 == 0) {
                dest.m_I64 = op1.m_I64 / 1;
            } else {
                dest.m_I64 = op1.m_I64 / op2.m_I64;
            }
            stackFrame.to_string(dst_str, istr.destination);
            sprintf(interpreted, "%s = %s / %s", dst_str, op1_str, op2_str);
            break;
        }
        case FAdd: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = RegisterValueType::R_F32;
            dest.m_I64 = op1.m_F32 + op2.m_F32;
            stackFrame.to_string(dst_str, istr.destination);
            sprintf(interpreted, "%s = %s + %s", dst_str, op1_str, op2_str);
            break;
        }
        case FSub: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = RegisterValueType::R_F32;
            dest.m_I64 = op1.m_F32 - op2.m_F32;
            stackFrame.to_string(dst_str, istr.destination);
            sprintf(interpreted, "%s = %s - %s", dst_str, op1_str, op2_str);
            break;
        }
        case FMul: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = RegisterValueType::R_F32;
            dest.m_I64 = op1.m_F32 * op2.m_F32;
            stackFrame.to_string(dst_str, istr.destination);
            sprintf(interpreted, "%s = %s * %s", dst_str, op1_str, op2_str);
            break;
        }
        case FDiv: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = RegisterValueType::R_F32;
            if (op2.m_F32 == 0) {
                dest.m_F32 = op1.m_F32 / 1;
                sprintf(interpreted + 13, "%s", " divide by 0");
            } else {
                dest.m_F32 = op1.m_F32 / op2.m_F32;
            }
            stackFrame.to_string(dst_str, istr.destination);
            sprintf(interpreted, "%s = %s / %s", dst_str, op1_str, op2_str);
            break;
        }
        case LoadStaticInt: {
            const i64 table_value = std::bit_cast<i64*>(stackFrame.m_symbolTablePtr)[istr.operand1];
            table_entry.m_type = SymbolTableEntryType::INT;
            table_entry.m_i64 = table_value;
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_I64;
            dest.m_I64 = table_value;
            stackFrame.to_string(dst_str, istr.destination);
            sprintf(interpreted, "%s = ST[%d] -> <%s>", dst_str, istr.operand1, op1_str); 
            break;
        }
        case LoadStaticFloat: {
            const f32 table_value = std::bit_cast<f32*>(stackFrame.m_symbolTablePtr)[istr.operand1];
            table_entry.m_type = SymbolTableEntryType::FLOAT;
            table_entry.m_f32 = table_value;
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_F32;
            dest.m_F32 = table_value;
            stackFrame.to_string(dst_str, istr.destination);
            sprintf(interpreted, "%s = ST[%d] -> <%s>", dst_str, istr.operand1, op1_str); 
            break;
        }
        case LoadStaticPointer: {
            const p64 table_value = std::bit_cast<p64*>(stackFrame.m_symbolTablePtr)[istr.operand1];
            table_entry.m_type = SymbolTableEntryType::POINTER;
            table_entry.m_pointer = table_value;
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_POINTER;
            stackFrame.to_string(dst_str, istr.destination);
            sprintf(interpreted, "%s = ST[%d] -> <%s>", dst_str, istr.operand1, op1_str); 
            break;
        }
        case LoadU16Imm: {
            const u16 value = istr.operand1 | (istr.operand2 << 8);
            sprintf(varying, "r%d, %d", istr.destination, value);
            dest.m_type = RegisterValueType::R_U16;
            dest.m_U64 = value;
            sprintf(interpreted, "r%d = %d", istr.destination, value);
            break;
        }
        case LoadU32: {
            sprintf(varying, "r%d, [r%d]", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_I32;
            dest.m_I32 = 0;
            sprintf(interpreted, "r%d = [0x%llx + 0x%llx]", istr.destination, op1.m_PTR.m_base, op1.m_PTR.m_offset);
            break;
        }
        case LoadFloat: {
            sprintf(varying, "r%d, [r%d]", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_F32;
            dest.m_F32 = 0.f;
            sprintf(interpreted, "r%d = [0x%llx + 0x%llx]", istr.destination, op1.m_PTR.m_base, op1.m_PTR.m_offset);
            break;
        }
        case LoadPointer: {
            sprintf(varying, "r%d, [r%d]", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_POINTER;
            dest.m_PTR = {0, 0};
            sprintf(interpreted, "r%d = [0x%llx + 0x%llx]", istr.destination, op1.m_PTR.m_base, op1.m_PTR.m_offset);
            break;
        }
        case LoadI64: {
            sprintf(varying, "r%d, [r%d]", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_I64;
            dest.m_I64 = 0;
            sprintf(interpreted, "r%d = [0x%llx + 0x%llx]", istr.destination, op1.m_PTR.m_base, op1.m_PTR.m_offset);
            break;
        }
        case LoadU64: {
            sprintf(varying, "r%d, [r%d]", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_U64;
            dest.m_U64 = 0;
            sprintf(interpreted, "r%d = [0x%llx + 0x%llx]", istr.destination, op1.m_PTR.m_base, op1.m_PTR.m_offset);
            break;
        }
        case StoreInt: {
            sprintf(varying, "[r%d], r%d", istr.destination, istr.operand1);
            break;
        }
        case LookupInt: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            i64 value = std::bit_cast<i64*>(stackFrame.m_symbolTablePtr)[istr.operand1]; 
            dest.m_type = RegisterValueType::R_I64;
            dest.m_I64 = value;
            table_entry.m_type = SymbolTableEntryType::POINTER;
            table_entry.m_i64 = value;
            sprintf(interpreted, "r%d = ST[%d] -> <%s>", istr.destination, istr.operand1, this->lookup(value));
            break;
        }
        case LookupFloat: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            f32 value = *std::bit_cast<f32*>(stackFrame.m_symbolTablePtr + istr.operand1); 
            dest.m_type = RegisterValueType::R_F32;
            dest.m_F32 = value;
            table_entry.m_type = SymbolTableEntryType::FLOAT;
            table_entry.m_f32 = value;
            sprintf(interpreted, "r%d = ST[%d] -> <%.2f>", istr.destination, istr.operand1, value);
            break;
        }
        case LookupPointer: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            const p64 value = std::bit_cast<p64*>(stackFrame.m_symbolTablePtr)[istr.operand1];
            dest.m_type = RegisterValueType::R_POINTER;
            dest.m_PTR.m_base = value;
            dest.m_PTR.m_offset = 0;
            table_entry.m_type = SymbolTableEntryType::POINTER;
            table_entry.m_pointer = value;
            if (this->m_currentFile->is_file_ptr(reinterpret_cast<p64>(stackFrame.m_symbolTablePtr + istr.operand1))) {
               sprintf(interpreted, "r%d = ST[%d] -> <%s>", istr.destination, istr.operand1, reinterpret_cast<const char*>(value));
            } else {
                sprintf(interpreted, "r%d = ST[%d] -> <%s>", istr.destination, istr.operand1, this->lookup(value));
            }
            break;
        }
        case MoveInt: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            dest.m_type = R_I64;
            dest.m_I64 = op1.m_I64;
            sprintf(interpreted, "r%d = r%d <%lli>", istr.destination, istr.operand1, op1.m_I64);
            break;
        }
        case MoveFloat: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            dest.m_type = R_F32;
            dest.m_F32 = op1.m_F32;
            sprintf(interpreted, "r%d = r%d <%f>", istr.destination, istr.operand1, op1.m_F32);
            break;
        }
        case MovePointer: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            dest.m_type = R_POINTER;
            dest.m_PTR = op1.m_PTR;
            sprintf(interpreted, "r%d = r%d <%s>", istr.destination, istr.operand1, this->lookup(op1.m_PTR.get()));
            break;
        }
        case CastInteger: {
            sprintf(varying, "r%d, r%d", istr.destination, istr.operand1);
            dest.m_type = R_I32;
            dest.m_I32 = (i32)op1.m_F32;
            sprintf(interpreted, "r%d = int(r%d) -> <%f> => <%d>", istr.destination, istr.operand1, op1.m_F32, dest.m_I32);
            break;
        }
        case CastFloat: {
            sprintf(varying, "r%d, r%d", istr.destination, istr.operand1);
            dest.m_type = R_F32;
            dest.m_F32 = (f32)op1.m_I32;
            sprintf(interpreted, "r%d = float(r%d) -> <%d> => <%f>", istr.destination, istr.operand1, op1.m_I32, dest.m_F32);
            break;
        }
        case Call: {
            sprintf(varying, "r%d, r%d, %d", istr.destination, istr.operand1, istr.operand2);
            char comment_str[200] = {0};
            sprintf(comment_str, "r%d = %s(", istr.destination, this->lookup(op1.m_PTR.get()));
            for (u64 i = 0; i < istr.operand2; ++i) {
                if (i != 0) {
                    strcat(comment_str, ", ");
                }
                stackFrame.to_string(dst_str, 49 + i, this->lookup(stackFrame[i + 49].m_SID));
                strcat(comment_str, dst_str);
            }
            dest.m_type = R_HASH;
            dest.isReturn = true;
            sprintf(interpreted, "%s)", comment_str);
            break;
        }
        case CallFf: {
            sprintf(varying, "r%d, r%d, %d", istr.destination, istr.operand1, istr.operand2);
            char comment_str[200] = {0};
            sprintf(comment_str, "r%d = %s(", istr.destination, this->lookup(op1.m_PTR.get()));
            for (u64 i = 0; i < istr.operand2; ++i) {
                if (i != 0) {
                    strcat(comment_str, ", ");
                }
                stackFrame.to_string(dst_str, 49 + i, this->lookup(stackFrame[i + 49].m_SID));
                strcat(comment_str, dst_str);
            }
            dest.m_type = R_HASH;
            dest.isReturn = true;
            sprintf(interpreted, "%s)", comment_str);
            break;
        }
        case IEqual: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_I64 == op2.m_I64;
            sprintf(interpreted, "r%d = r%d [%lli] == r%d [%lli]", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_I64, 
                istr.operand2, 
                stackFrame[istr.operand2].m_I64
            );
            break;
        }
        case IGreaterThan: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_I64 > op2.m_I64;
            sprintf(interpreted, "r%d = r%d [%lld] > r%d [%lld]", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_I64, 
                istr.operand2, 
                stackFrame[istr.operand2].m_I64
            );
            break;
        }
        case IGreaterThanEqual: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_I64 >= op2.m_I64;
            sprintf(interpreted, "r%d = r%d [%lli] >= r%d [%lli]", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_I64, 
                istr.operand2, 
                stackFrame[istr.operand2].m_I64
            );
            break;
        }
        case ILessThan: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_I64 < op2.m_I64;
            sprintf(interpreted, "r%d = r%d [%lli] < r%d [%lli]", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_I64, 
                istr.operand2, 
                stackFrame[istr.operand2].m_I64
            );
            break;
        }
        case ILessThanEqual: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_I64 <= op2.m_I64;
            sprintf(interpreted, "r%d = r%d [%lli] <= r%d [%lli]", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_I64, 
                istr.operand2, 
                stackFrame[istr.operand2].m_I64
            );
            break;
        }
        case FEqual: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_F32 == op2.m_F32;
            sprintf(interpreted, "r%d = r%d [%f] == r%d [%f]", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_F32, 
                istr.operand2, 
                stackFrame[istr.operand2].m_F32
            );
            break;
        }
        case FGreaterThanEqual: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_F32 >= op2.m_F32;
            sprintf(interpreted, "r%d = r%d [%f] >= r%d [%f]", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_F32, 
                istr.operand2, 
                stackFrame[istr.operand2].m_F32
            );
            break;
        }
        case FLessThan: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_F32 < op2.m_F32;
            sprintf(interpreted, "r%d = r%d [%f] < r%d [%f]", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_F32, 
                istr.operand2, 
                stackFrame[istr.operand2].m_F32
            );
            break;
        }
        case FLessThanEqual: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_F32 <= op2.m_F32;
            sprintf(interpreted, "r%d = r%d [%f] <= r%d [%f]", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_F32, 
                istr.operand2, 
                stackFrame[istr.operand2].m_F32
            );
            break;
        }
        case IMod: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_I32;
            if (op2.m_I64 == 0) {
                dest.m_I32 = op1.m_I64 % 1;
            } else {
                dest.m_I32 = op1.m_I64 % op2.m_I64;
            }
            sprintf(interpreted, "r%d = r%d [%lli] %% r%d [%lli] -> <%d>", 
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
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_F32;
            if (op2.m_F32 == 0.f) {
                dest.m_F32 = fmodf(op1.m_F32, 1.f);
            } else {
                dest.m_F32 = fmodf(op1.m_F32, op2.m_F32);
            }
            sprintf(interpreted, "r%d = r%d [%f] %% r%d [%f] -> <%f>", 
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
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_I32;
            dest.m_I64 = abs(op1.m_I64);
            sprintf(interpreted, "r%d = ABS(r%d) [%lli] -> <%lli>", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_I64,
                dest.m_I64
            );
            break;
        }
        case FAbs: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_I32;
            dest.m_F32 = abs(op1.m_F32);
            sprintf(interpreted, "r%d = ABS(r%d) [%f] -> <%f>", 
                istr.destination, 
                istr.operand1,
                stackFrame[istr.operand1].m_F32,
                dest.m_F32
            );
            break;
        }
        case Branch: {
            u32 target = istr.destination | (istr.operand2 << 8);
            sprintf(varying, "%04X", target);
            //u32 label_name = stackFrame.get_label_index(target);
            line.m_label = target;
            break;
        }
        case BranchIf: {
            u32 target = istr.destination | (istr.operand2 << 8);
            sprintf(varying, "r%d, %04X", istr.operand1, target);
            //u32 label_name = stackFrame.get_label_index(target);
            sprintf(interpreted, "IF r%d [%s] ", istr.operand1, op1_str);
            line.m_label = target;
            break;
        }
        case BranchIfNot: {
            u32 target = istr.destination | (istr.operand2 << 8);
            sprintf(varying, "r%d, %04X", istr.operand1, target);
            //u32 label_name = stackFrame.get_label_index(target);
            sprintf(interpreted, "IF NOT r%d [%s] ", istr.operand1, op1_str);
            line.m_label = target;
            break;
        }
        case OpLogNot: {
            sprintf(varying, "r%d, r%d", istr.destination, istr.operand1);
            dest.m_type = R_BOOL;
            dest.m_BOOL = !op1.m_BOOL;
            sprintf(interpreted, "r%d = !r%d -> <%s>", istr.destination, istr.operand1, dest.m_BOOL ? "TRUE" : "FALSE");
            break;
        }
        case OpBitAnd: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = op1.m_type;
            dest.m_U64 = op1.m_U64 & op2.m_U64;
            sprintf(interpreted, "r%d = r%d [%llu] & r%d [%llu] -> <%llu>", istr.destination, istr.operand1, op1.m_U64, istr.operand2, op2.m_U64, dest.m_U64);
            break;
        }
        case OpBitNot: {
            sprintf(varying, "r%d, r%d", istr.destination, istr.operand1);
            dest.m_type = op1.m_type;
            dest.m_U64 = ~op1.m_U64;
            sprintf(interpreted, "r%d = ~r%d [%llu] -> <%llu>", istr.destination, istr.operand1, op1.m_U64, dest.m_U64);
            break;
        }
        case OpBitOr: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = op1.m_type;
            dest.m_U64 = op1.m_U64 | op2.m_U64;
            sprintf(interpreted, "r%d = r%d [%llu] | r%d [%llu] -> <%llu>", istr.destination, istr.operand1, op1.m_U64, istr.operand2, op2.m_U64, dest.m_U64);
            break;
        }
        case OpBitXor: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = op1.m_type;
            dest.m_U64 = op1.m_U64 ^ op2.m_U64;
            sprintf(interpreted, "r%d = r%d [%llu] ^ r%d [%llu] -> <%llu>", istr.destination, istr.operand1, op1.m_U64, istr.operand2, op2.m_U64, dest.m_U64);
            break;
        }
        case OpBitNor: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = op1.m_type;
            dest.m_U64 = ~(op1.m_U64 | op2.m_U64);
            sprintf(interpreted, "r%d = ~(r%d [%llu] | r%d [%llu]) -> <%llu>", istr.destination, istr.operand1, op1.m_U64, istr.operand2, op2.m_U64, dest.m_U64);
            break;
        }
        case OpLogAnd: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_BOOL && op2.m_BOOL;
            sprintf(interpreted, "r%d = r%d [%d] && r%d [%d]", istr.destination, istr.operand1, op1.m_BOOL, istr.operand2, op2.m_BOOL);
            break;
        } 
        case OpLogOr: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_BOOL || op2.m_BOOL;
            sprintf(interpreted, "r%d = r%d [%d] || r%d [%d]", istr.destination, istr.operand1, op1.m_BOOL, istr.operand2, op2.m_BOOL);
            break;
        }
        case INeg: {
            sprintf(varying, "r%d, r%d", istr.destination, istr.operand1);
            dest.m_type = R_I64;
            dest.m_I64 = -op1.m_I64;
            sprintf(interpreted, "r%d = -r%d [%lli] -> <%lli>", istr.destination, istr.operand1, op1.m_I64, dest.m_I64);
            break;
        }
        case FNeg: {
            sprintf(varying, "r%d, r%d", istr.destination, istr.operand1);
            dest.m_type = R_F32;
            dest.m_F32 = -op1.m_F32;
            sprintf(interpreted, "r%d = -r%d [%f] -> <%f>", istr.destination, istr.operand1, op1.m_F32, dest.m_F32);
            break;
        }
        case LoadParamCnt: {
            // wat
            break;
        }
        case IAddImm: {
            sprintf(varying, "r%d, r%d, %d", istr.destination, istr.operand1, istr.operand2);
            i64 value = op1.m_I64 + istr.operand2;
            dest.m_type = R_I64;
            dest.m_I64 = value;
            sprintf(interpreted, "r%d = r%d [%lli] + %d -> <%lli>", istr.destination, istr.operand1, op1.m_I64, istr.operand2, dest.m_I64);
            break;
        }
        case ISubImm: {
            sprintf(varying, "r%d, r%d, %d", istr.destination, istr.operand1, istr.operand2);
            i64 value = op1.m_I64 - istr.operand2;
            dest.m_type = R_I64;
            dest.m_I64 = value;
            sprintf(interpreted, "r%d = r%d [%lli] - %d -> <%lli>", istr.destination, istr.operand1, op1.m_I64, istr.operand2, dest.m_I64);
            break;
        }
        case IMulImm: {
            sprintf(varying, "r%d, r%d, %d", istr.destination, istr.operand1, istr.operand2);
            i64 value = op1.m_I64 * istr.operand2;
            dest.m_type = R_I64;
            dest.m_I64 = value;
            sprintf(interpreted, "r%d = r%d [%lli] * %d -> <%lli>", istr.destination, istr.operand1, op1.m_I64, istr.operand2, dest.m_I64);
            break;
        }
        case IDivImm: {
            sprintf(varying, "r%d, r%d, %d", istr.destination, istr.operand1, istr.operand2);
            i64 value;
            if (istr.operand2 == 0) {
                value = op1.m_I64 / 1;
            } else {
                value = op1.m_I64 / istr.operand2;
            }
            dest.m_type = R_I64;
            dest.m_I64 = value;
            sprintf(interpreted, "r%d = r%d [%lli] / %d -> <%lli>", istr.destination, istr.operand1, op1.m_I64, istr.operand2, dest.m_I64);
            break;
        }
        case LoadStaticI32Imm: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            i32 value = *std::bit_cast<i32*>(stackFrame.m_symbolTablePtr + istr.operand1); 
            dest.m_type = RegisterValueType::R_I32;
            dest.m_I32 = value;
            table_entry.m_type = SymbolTableEntryType::INT;
            table_entry.m_i64 = value;
            sprintf(interpreted, "r%d = ST[%d] -> <%d>", istr.destination, istr.operand1, value);
            break;
        }
        case LoadStaticFloatImm: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            f32 value = *std::bit_cast<f32*>(stackFrame.m_symbolTablePtr + istr.operand1); 
            dest.m_type = RegisterValueType::R_F32;
            dest.m_F32 = value;
            table_entry.m_type = SymbolTableEntryType::FLOAT;
            table_entry.m_f32 = value;
            sprintf(interpreted, "r%d = ST[%d] -> <%.2f>", istr.destination, istr.operand1, value);
            break;
        }
        case LoadStaticPointerImm: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            p64 value = std::bit_cast<p64*>(stackFrame.m_symbolTablePtr)[istr.operand1]; 
            dest.m_type = RegisterValueType::R_POINTER;
            dest.m_PTR = {value, 0};
            table_entry.m_type = SymbolTableEntryType::POINTER;
            table_entry.m_pointer = value;
            if (value >= this->m_currentFile->m_stringsPtr) {
                sprintf(interpreted, "r%d = ST[%d] -> \"%s\"", istr.destination, istr.operand1, reinterpret_cast<const char*>(value));
            } else {
                sprintf(interpreted, "r%d = ST[%d] -> <0x%d>", istr.destination, istr.operand1, this->get_offset((void*)value));
            }
            break;
        }
        case LoadStaticI64Imm: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            i64 value = std::bit_cast<i64*>(stackFrame.m_symbolTablePtr)[istr.operand1]; 
            dest.m_type = RegisterValueType::R_I64;
            dest.m_I64 = value;
            table_entry.m_type = SymbolTableEntryType::INT;
            table_entry.m_i64 = value;
            sprintf(interpreted, "r%d = ST[%d] -> <%llu>", istr.destination, istr.operand1, value);
            break;
        }
        case LoadStaticU64Imm: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            u64 value = reinterpret_cast<u64*>(stackFrame.m_symbolTablePtr)[istr.operand1];
            if (value >= 0x000FFFFFFFFFFFFF) {
                const char *hash_str = this->lookup(value);
                dest.m_type = RegisterValueType::R_HASH;
                dest.m_SID = value;
                table_entry.m_type = SymbolTableEntryType::STRINGID_64;
                table_entry.m_hash = value;
                stackFrame.to_string(dst_str, istr.destination, hash_str);
                sprintf(interpreted, "r%d = ST[%d] -> <%s>", istr.destination, istr.operand1, dst_str);
            } else if (this->m_currentFile->is_file_ptr(reinterpret_cast<p64>(stackFrame.m_symbolTablePtr + istr.operand1))) {
                printf("here");
            }
            break;
        }
        case Move: {
            sprintf(varying, "r%d, r%d", istr.destination, istr.operand1);
            dest = op1;
            sprintf(interpreted, "r%d = r%d [%s]", istr.destination, istr.operand1, op1_str);
            break;
        }
        case AssertPointer: {
            sprintf(varying, "r%d", istr.destination);
            sprintf(interpreted, "r%d?", istr.destination);
            break;
        }
        default: {
            sprintf(varying, "???");
            sprintf(interpreted, "UNKNOWN INSTRUCTION");
            break;
        }
    }
    if (table_entry.m_type != NONE) {
        stackFrame.m_symbolTable.emplace(istr.operand1, table_entry);
    }
    line.m_text = std::string(disassembly_text);
    line.m_comment = std::string(interpreted);
}

void ListingViewController::insert_function_disassembly_text(const FunctionDisassembly &functionDisassembly, const u32 indent) {
    auto labels = functionDisassembly.m_stackFrame.m_labels;
    char buffer[128] = {0};
    constexpr TextFormat label_format = {MainWindow::COMMENT_COLOR, 14};
    constexpr TextFormat text_format = {MainWindow::OPCODE_COLOR, 12};
    constexpr TextFormat comment_format = {MainWindow::COMMENT_COLOR, 12};
    for (const auto &line : functionDisassembly.m_lines) {
        u32 line_offset = std::max(67ull - line.m_text.length(), 0ull);
        auto label_loc = std::find(labels.begin(), labels.end(), line.m_location);
        if (label_loc != labels.end()) {
            const u32 label_idx = std::distance(labels.begin(), label_loc);
            if (line.m_location == functionDisassembly.m_lines.size() - 1) {
                this->insert_span("L_RETURN:\n", label_format, indent);
            } else {
                sprintf(buffer, "L_%d:\n", label_idx);
                this->insert_span(buffer, label_format, indent);  
            }
        }
        this->insert_span(line.m_text.c_str(), text_format, indent);
        std::string comment = std::string(line_offset, ' ') + line.m_comment;
        this->insert_span(comment.c_str(), {MainWindow::STRING_COLOR, 12});
        if (line.m_label != -1) {
            u32 target = std::distance(labels.begin(), std::find(labels.begin(), labels.end(), line.m_label));
            if (line.m_label == (functionDisassembly.m_lines.size() - 1)) {
                this->insert_span("=> L_RETURN", comment_format);
            } else {
                sprintf(buffer, "=> L_%d", target);
                this->insert_span(buffer, comment_format);
            }
        }
        this->insert_span("\n");
    }
    this->insert_span("SYMBOL TABLE: \n", {MainWindow::COMMENT_COLOR, 12}, indent);

    char line_start[64] = {0};
    char type[128] = {0};

    u64 *table_ptr = functionDisassembly.m_stackFrame.m_symbolTablePtr;

    for (u32 i = 0; i < functionDisassembly.m_stackFrame.m_symbolTable.size(); ++i) {
        const SymbolTableEntry &entry = functionDisassembly.m_stackFrame.m_symbolTable.at(i);
        sprintf(line_start, "%04X   0x%06X   ", i, this->get_offset(table_ptr + i));
        switch (entry.m_type) {
            case SymbolTableEntryType::FLOAT: {
                sprintf(type, "FLOAT <%f>\n", entry.m_f32);
                break;
            }
            case SymbolTableEntryType::INT: {
                sprintf(type, "INT <%lli>\n", entry.m_i64);
                break;
            }
            case SymbolTableEntryType::POINTER:{
                if (false) {
                    sprintf(type, "POINTER \"%s\"\n", reinterpret_cast<const char*>(entry.m_pointer));
                } else {
                    sprintf(type, "POINTER <%s>\n", this->lookup(entry.m_pointer));
                }
                break;
            }
            case SymbolTableEntryType::STRINGID_64: {
                sprintf(type, "SID <%s>\n", this->lookup(entry.m_hash));
                break;
            }
            case SymbolTableEntryType::NONE:
            case SymbolTableEntryType::UNKNOWN_TYPE: {
                sprintf(type, "UNKNOWN: <%llX>", entry.m_i64);
                break;
            } 
        }
        sprintf(buffer, "%s %s", line_start, type);
        this->insert_span(buffer, {MainWindow::COMMENT_COLOR, 12}, indent);
    }
}

void ListingViewController::insert_header_line() {
    const char* current_script_name;
    const char* current_script_id;
    const TextFormat header_format = {MainWindow::STRING_COLOR, 14};
    if (this->m_currentFile->m_dcscript != nullptr) {
        current_script_name = this->lookup(this->m_currentFile->m_dcscript->m_stateScriptId);
        current_script_id = int_to_string_id(this->m_currentFile->m_dcscript->m_stateScriptId).c_str();
    } else {
        current_script_name = "UNKNOWN SCRIPT";
        current_script_id = "UNKNOWN SCRIPT ID";
    }
    if (this->m_mainWindow != nullptr) {
        this->m_mainWindow->get_listing_view()->clear();
        this->m_mainWindow->get_listing_view()->setReadOnly(true);
    }
    this->insert_span_fmt("%*sDeepQuantum's DC Disassembler ver. %d\n", header_format, 14, "", MainWindow::VersionNumber);
    this->insert_span_fmt("%*sListing for script: %s\n", header_format, 14, "", current_script_name);
    this->insert_span_fmt("%*sScript ID: %s\n", header_format, 14, "", current_script_id);
    this->insert_span_fmt("%*sFilesize: %d bytes\n", header_format, 14, "", this->m_currentFile->m_size);
    this->insert_span("START OF DISASSEMBLY\n", header_format, 14);
    this->insert_span("--------------------------------------------------\n", header_format, 14);
}