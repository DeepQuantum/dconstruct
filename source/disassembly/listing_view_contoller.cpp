#include "custom_structs.h"
#include "../mainwindow.h"
#include <stdexcept>
#include <cmath>
#include <sstream>
#include "listing_view_controller.h"

ListingViewController::ListingViewController(BinaryFile &file, MainWindow *mainWindow) {
    this->m_mainWindow = mainWindow;
    this->m_currentFile = file;
    this->create_listing_view();
}

[[nodiscard]] const char *ListingViewController::lookup(const stringid_64 sid) noexcept {
    auto res = this->m_currentFile.sid_cache.find(sid);
    if (res != this->m_currentFile.sid_cache.end()) {
        return res->second;
    }

    const char *hash_string = this->m_mainWindow->m_sidbase.search(sid);
    if (hash_string == nullptr) {
        hash_string = int_to_string_id(sid);
        this->m_currentFile.sid_cache.emplace(sid, hash_string);
    }
    return hash_string;
}

void ListingViewController::insert_span(const char *line, QColor color, int fontSize, u8 indent) {
    auto* view = this->m_mainWindow->getListingView();
    QTextCursor cursor = view->textCursor();
    cursor.movePosition(QTextCursor::End);
    view->setTextCursor(cursor);

    QTextCharFormat format;
    format.setForeground(color);
    format.setFontPointSize(fontSize);
    cursor.insertText(QString(indent, ' ') + QString::fromStdString(line), format);
}

void ListingViewController::insert_comment(const std::string &comment, u8 indent) {
    this->insert_span(comment.c_str(), MainWindow::COMMENT_COLOR, 14, indent);
}

void ListingViewController::insert_hash(const std::string &hash, b8 surround) {
    std::string out;
    if (surround) {
        out = "<" + hash + ">";
    } else {
        out = hash;
    }
    this->insert_span(out.c_str(), MainWindow::HASH_COLOR, 14, 0);
}

void ListingViewController::parse_custom_strucht_from_pointer(const u64 *p) {
    const std::string type_id = this->lookup(*reinterpret_cast<const stringid_64*>(p - 1));
}

void ListingViewController::parse_custom_struct(Entry *entry) {
    const u8 *base = this->m_currentFile.m_bytes.data();
    char buffer[64] = {0};
    switch (entry->m_typeId) {
        case SID("array"): {
            //DCArray 
            break;
        }
        case SID("map"): {
            DCMap *map = reinterpret_cast<DCMap*>(entry->m_entryPtr);
            for (u64 i = 0; i < map->size; ++i) {
                const std::string key_hash = this->lookup(map->keys[i]);
                u64 *value_ptr = reinterpret_cast<u64*>(map->values[i]);
                Entry dummy = Entry{
                    entry->m_scriptId,
                    *reinterpret_cast<const stringid_64*>(value_ptr - 1),
                    value_ptr
                };
                this->insert_entry(&dummy);
                sprintf(buffer, "%s:\n", key_hash);
                this->insert_span(buffer, MainWindow::STRING_COLOR, 14, 12);
            }
            break;
        }
        default: {
            this->insert_span("Unhandled");
        }
    }
    this->insert_span("\n");
}

void ListingViewController::create_listing_view() {
    this->insert_header_line();
    for (std::size_t i = 0; i < this->m_currentFile.m_dcheader->m_numEntries; ++i) {
        this->insert_entry(this->m_currentFile.m_dcheader->m_pStartOfData + i);
    }
}

void ListingViewController::insert_entry(Entry *entry) {
    const char *line;
    char buffer[300] = {0};
    char offset_str[10] = {0};
    const u64 offset = this->get_offset((void*)entry->m_entryPtr);

    sprintf(offset_str, "[0x%06X] ", offset);

    this->insert_span(offset_str, MainWindow::COMMENT_COLOR, 12, 6);

    switch (entry->m_typeId)
    {
        case SID("boolean"):
        {
            sprintf(buffer + 10, "BOOL: <%s>\n", *reinterpret_cast<i32*>(entry->m_entryPtr) ? "true" : "false");
            this->insert_span(line, MainWindow::OPCODE_COLOR, 10);
            break;
        }
        case SID("int"):
        {
            sprintf(buffer + 10, "INT: <%s>\n", *reinterpret_cast<i32*>(entry->m_entryPtr));
            this->insert_span(line, MainWindow::OPCODE_COLOR, 10);
            break;
        }
        case SID("float"):
        {
            sprintf(buffer + 10, "FLOAT: <%s>\n", *reinterpret_cast<f32*>(entry->m_entryPtr));
            this->insert_span(line, MainWindow::OPCODE_COLOR, 10);
            break;
        }
        case SID("sid"):
        {
            sprintf(buffer + 10, "SID: <%s>\n", this->lookup(*reinterpret_cast<stringid_64*>(entry->m_entryPtr)));
            this->insert_span(line, MainWindow::OPCODE_COLOR, 10);
            break;
        }
        case SID("state-script"):
        {
            this->disassemble_state_script(reinterpret_cast<StateScript*>(entry->m_entryPtr));
            break;
        }
        case SID("script-lambda"):
        {
            this->insert_comment("BEGIN LAMBDA ", 20);
            this->insert_hash(this->lookup(entry->m_scriptId), true);
            this->insert_span(" AT [", MainWindow::COMMENT_COLOR, 14);
            this->insert_span(offset_str, MainWindow::OPCODE_COLOR, 14);
            this->insert_span("]\n", MainWindow::COMMENT_COLOR, 14);
            auto function = this->create_function_disassembly(reinterpret_cast<ScriptLambda*>(entry->m_entryPtr));
            this->insert_function_disassembly_text(function);
            break;
        }
        default:
        {
            sprintf(buffer, "%s: %s\n", this->lookup(entry->m_typeId), this->lookup(entry->m_scriptId));
            this->insert_span(buffer);
            this->parse_custom_struct(entry);
            break;
        }
    }
}

u32 ListingViewController::get_offset(const void *symbol) {
    return reinterpret_cast<uintptr_t>(symbol) - reinterpret_cast<uintptr_t>(this->m_currentFile.m_dcheader);
}

void ListingViewController::insert_variable(SsDeclaration *var) {
    char buffer[128] = {0};
    std::string type_name;
    char var_format[36] = {0};

    b8 is_nullptr = var->m_pDeclValue == nullptr;

    switch (var->m_declTypeId) {
        case SID("boolean"): {
            type_name = "BOOLEAN";
            if (!is_nullptr) 
                sprintf(var_format, "%s", *reinterpret_cast<b8*>(var->m_pDeclValue) == 1 ? "TRUE" : "FALSE");
            break;
        }
        case SID("vector"): {
            type_name = "VECTOR";
            if (!is_nullptr) {
                f32 *val = reinterpret_cast<f32*>(var->m_pDeclValue);
                sprintf(var_format, "(%.4f, %.4f, %.4f, %.4f)", val[0], val[1], val[2], val[3]);
            }
            break;
        }
        case SID("quat"): {
            type_name = "QUAT";
            if (!is_nullptr) {
                f32 *val = reinterpret_cast<f32*>(var->m_pDeclValue);
                sprintf(var_format, "(%.4f, %.4f, %.4f, %.4f)", val[0], val[1], val[2], val[3]);
            }
            break;
        }
        case SID("float"): {
            type_name = "FLOAT";
            if (!is_nullptr) {
                sprintf(var_format, "%.4f", *reinterpret_cast<f32*>(var->m_pDeclValue));
            }
            break;
        }
        case SID("string"): {
            type_name = "STRING";
            if (!is_nullptr) {
                sprintf(var_format, "%s", *reinterpret_cast<const char**>(var->m_pDeclValue));
            }
            break;
        }
        case SID("symbol"): {
            type_name = "SYMBOL";
            if (!is_nullptr) {
                const std::string val = this->lookup(*reinterpret_cast<stringid_64*>(var->m_pDeclValue));
                sprintf(var_format, "%s", val.c_str());
            }
            break;
        }
        case SID("int32"): {
            type_name = "INT32";
            if (!is_nullptr) {
                sprintf(var_format, "%d", *reinterpret_cast<i32*>(var->m_pDeclValue));
            }
            break;
        }
        case SID("uint64"): {
            type_name = "UINT64";
            if (!is_nullptr) {
                sprintf(var_format, "%llX", *reinterpret_cast<u64*>(var->m_pDeclValue));
            }
            break;
        }
        case SID("timer"): {
            type_name = "TIMER";
            if (!is_nullptr) {
                sprintf(var_format, "%f", *reinterpret_cast<f32*>(var->m_pDeclValue));
            }
            break;
        }
        case SID("point"): {
            type_name = "POINT";
            if (!is_nullptr) {
                f32 *val = reinterpret_cast<f32*>(var->m_pDeclValue);
                sprintf(var_format, "(%.2f, %.2f, %.2f)", val[0], val[1], val[2]);
            }
            break;
        }
        case SID("bound-frame"): {
            type_name = "BOUND-FRAME";
            if (!is_nullptr) {
                sprintf(var_format, "%f", *reinterpret_cast<f32*>(var->m_pDeclValue));
            }
            break;
        }
        default: {
            type_name = this->lookup(var->m_declTypeId);
            strcpy(var_format, "???");
            break;
        }
    }
    if (is_nullptr) {
        strcpy(var_format, "UNINITIALIZED");
    }
    const std::string name_id = this->lookup(var->m_declId);
    sprintf(buffer, "[%05X] %s %s = <%s>\n", this->get_offset(var), type_name.c_str(), name_id.c_str(), var_format);
    this->insert_span(buffer, MainWindow::OPCODE_COLOR, 12, 8);
}

void ListingViewController::insert_on_block(SsOnBlock *block) {
    char buffer[32] = {0};
    switch (block->m_blockType) {
        case 0: {
            strcpy(buffer,"ON start\n");
            break;
        }
        case 1: {
            strcpy(buffer,"ON end\n");
            break;
        }
        case 2: {
            sprintf(buffer, "ON event %s\n", this->lookup(block->m_blockEventId));
            break;
        }
        case 3: {
            strcpy(buffer,"ON update\n");
            break;
        }
        case 4: {
            strcpy(buffer,"ON virtual\n");
            break;
        }
        default: {
            strcpy(buffer,"ON UNKNOWN\n");
            break;
        }
    }
    this->insert_span(buffer, MainWindow::COMMENT_COLOR, 12, 9);
    for (u64 i = 0; i < block->m_trackGroup.m_numTracks; ++i) {
        SsTrack *track_ptr = block->m_trackGroup.m_aTracks + i;
        const char *track_name = this->lookup(track_ptr->m_trackId);
        sprintf(buffer, "TRACK %s:\n", track_name);
        this->insert_span(buffer, MainWindow::COMMENT_COLOR, 12, 10);
        for (u64 j = 0; j < track_ptr->m_totalLambdaCount; ++j) {
            FunctionDisassembly function = this->create_function_disassembly(track_ptr->m_pSsLambda[j].m_pScriptLambda);
            this->insert_function_disassembly_text(function);
        }
    }
}

void ListingViewController::disassemble_state_script(StateScript *stateScript) {
    if (stateScript->m_pSsOptions != nullptr && stateScript->m_pSsOptions->m_pSymbolArray != nullptr) {
        SymbolArray *s_array = stateScript->m_pSsOptions->m_pSymbolArray;
        this->insert_span("OPTIONS: ", MainWindow::COMMENT_COLOR, 14, 6);
        for (u64 i = 0; i < s_array->m_numEntries; ++i) {
            this->insert_span(this->lookup(s_array->m_pSymbols[i]));
            this->insert_span("\n");
        }
    }
    SsDeclarationList *decl_table = stateScript->m_pSsDeclList;
    if (decl_table != nullptr) {
        this->insert_span("DECLARATIONS: \n", MainWindow::COMMENT_COLOR, 14, 6);
        for (u64 i = 0; i < decl_table->m_numDeclarations; ++i) {
            SsDeclaration *decl = decl_table->m_pDeclarations + i;
            if (decl->m_isVar) {
                this->insert_variable(decl);
            }
        }
    }
    char buffer[64] = {0};
    for (u64 i = 0; i < stateScript->m_stateCount; ++i) {
        SsState *state_ptr = stateScript->m_pSsStateTable + i;
        const char* state_name = this->lookup(state_ptr->m_stateId);
        sprintf(buffer, "STATE %s:\n", state_name);
        this->insert_span(buffer, MainWindow::COMMENT_COLOR, 14, 6);
        for (u64 j = 0; j < state_ptr->m_numSsOnBlocks; ++j) {
            this->insert_on_block(state_ptr->m_pSsOnBlocks + j);
        }
    }
}

FunctionDisassembly ListingViewController::create_function_disassembly(ScriptLambda *lambda) {
    // this->insert_span("INSTRUCTION POINTER: " + MainWindow::offset_to_string(this->get_offset(lambda->m_pOpcode)) + "\n", MainWindow::COMMENT_COLOR, 14, 20);
    // this->insert_span("SYMBOL TABLE POINTER: " + MainWindow::offset_to_string(this->get_offset(lambda->m_pSymbols)) + "\n", MainWindow::COMMENT_COLOR, 14, 20);

    Instruction *instructionPtr = reinterpret_cast<Instruction*>(lambda->m_pOpcode);
    u64 instructionCount = reinterpret_cast<Instruction*>(lambda->m_pSymbols) - instructionPtr;

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
    sprintf(disassembly_text, "%04X   0x%06X   %02X %02X %02X %02X   %-20s",
            line.m_location,
            this->get_offset((void*)(line.m_globalPointer + line.m_location)),
            istr.opcode,
            istr.destination,
            istr.operand1,
            istr.operand2,
            istr.opcode_to_string().c_str()
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
            const i64 table_value = reinterpret_cast<i64*>(stackFrame.m_symbolTablePtr)[istr.operand1];
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
            const f32 table_value = reinterpret_cast<f32*>(stackFrame.m_symbolTablePtr)[istr.operand1];
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
            const uintptr_t table_value = reinterpret_cast<uintptr_t*>(stackFrame.m_symbolTablePtr)[istr.operand1];
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
            sprintf(interpreted, "r%d = [0x%x + 0x%x]", istr.destination, op1.m_PTR.m_base, op1.m_PTR.m_offset);
            break;
        }
        case LoadFloat: {
            sprintf(varying, "r%d, [r%d]", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_F32;
            dest.m_F32 = 0.f;
            sprintf(interpreted, "r%d = [0x%x + 0x%x]", istr.destination, op1.m_PTR.m_base, op1.m_PTR.m_offset);
            break;
        }
        case LoadPointer: {
            sprintf(varying, "r%d, [r%d]", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_POINTER;
            dest.m_PTR = {0, 0};
            sprintf(interpreted, "r%d = [0x%x + 0x%x]", istr.destination, op1.m_PTR.m_base, op1.m_PTR.m_offset);
            break;
        }
        case LoadI64: {
            sprintf(varying, "r%d, [r%d]", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_I64;
            dest.m_I64 = 0;
            sprintf(interpreted, "r%d = [0x%x + 0x%x]", istr.destination, op1.m_PTR.m_base, op1.m_PTR.m_offset);
            break;
        }
        case LoadU64: {
            sprintf(varying, "r%d, [r%d]", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_U64;
            dest.m_U64 = 0;
            sprintf(interpreted, "r%d = [0x%x + 0x%x]", istr.destination, op1.m_PTR.m_base, op1.m_PTR.m_offset);
            break;
        }
        case StoreInt: {
            sprintf(varying, "[r%d], r%d", istr.destination, istr.operand1);
            break;
        }
        case LookupInt: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            i64 value = reinterpret_cast<i64*>(stackFrame.m_symbolTablePtr)[istr.operand1]; 
            dest.m_type = RegisterValueType::R_I64;
            dest.m_I64 = value;
            table_entry.m_type = SymbolTableEntryType::POINTER;
            table_entry.m_i64 = value;
            sprintf(interpreted, "r%d = ST[%d] -> <%s>", istr.destination, istr.operand1, this->lookup(value));
            break;
        }
        case LookupFloat: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            f32 value = *reinterpret_cast<f32*>(stackFrame.m_symbolTablePtr + istr.operand1); 
            dest.m_type = RegisterValueType::R_F32;
            dest.m_F32 = value;
            table_entry.m_type = SymbolTableEntryType::FLOAT;
            table_entry.m_f32 = value;
            sprintf(interpreted, "r%d = ST[%d] -> <%.2f>", istr.destination, istr.operand1, value);
            break;
        }
        case LookupPointer: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            const uintptr_t value = reinterpret_cast<uintptr_t*>(stackFrame.m_symbolTablePtr)[istr.operand1];
            dest.m_type = RegisterValueType::R_POINTER;
            dest.m_PTR.m_base = value;
            dest.m_PTR.m_offset = 0;
            table_entry.m_type = SymbolTableEntryType::POINTER;
            table_entry.m_pointer = value;
            sprintf(interpreted, "r%d = ST[%d] -> <%s>", istr.destination, istr.operand1, this->lookup(value));
            break;
        }
        case MoveInt: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            dest.m_type = R_I64;
            dest.m_I64 = op1.m_I64;
            sprintf(interpreted, "r%d = r%d <%d>", istr.destination, istr.operand1, op1.m_I64);
            break;
        }
        case MoveFloat: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            dest.m_type = R_F32;
            dest.m_F32 = op1.m_F32;
            sprintf(interpreted, "r%d = r%d <%f>", istr.destination, istr.operand1, op1.m_I64);
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
            sprintf(interpreted, "r%d = r%d [%d] == r%d [%d]", 
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
            sprintf(interpreted, "r%d = r%d [%d] > r%d [%d]", 
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
            sprintf(interpreted, "r%d = r%d [%d] >= r%d [%d]", 
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
            sprintf(interpreted, "r%d = r%d [%d] < r%d [%d]", 
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
            sprintf(interpreted, "r%d = r%d [%d] <= r%d [%d]", 
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
            sprintf(interpreted, "r%d = r%d [%d] % r%d [%d] -> <%d>", 
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
            sprintf(interpreted, "r%d = r%d [%f] % r%d [%f] -> <%f>", 
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
            sprintf(interpreted, "r%d = ABS(r%d) [%d] -> <%d>", 
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
            u32 label_name = stackFrame.get_label_index(target);
            line.m_label = target;
            break;
        }
        case BranchIf: {
            u32 target = istr.destination | (istr.operand2 << 8);
            sprintf(varying, "r%d, %04X", istr.operand1, target);
            u32 label_name = stackFrame.get_label_index(target);
            sprintf(interpreted, "IF r%d [%s] ", istr.operand1, op1_str);
            line.m_label = target;
            break;
        }
        case BranchIfNot: {
            u32 target = istr.destination | (istr.operand2 << 8);
            sprintf(varying, "r%d, %04X", istr.operand1, target);
            u32 label_name = stackFrame.get_label_index(target);
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
            sprintf(interpreted, "r%d = -r%d [%d] -> <%d>", istr.destination, istr.operand1, op1.m_I64, dest.m_I64);
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
        }
        case IAddImm: {
            sprintf(varying, "r%d, r%d, %d", istr.destination, istr.operand1, istr.operand2);
            i64 value = op1.m_I64 + istr.operand2;
            dest.m_type = R_I64;
            dest.m_I64 = value;
            sprintf(interpreted, "r%d = r%d [%d] + %d -> <%d>", istr.destination, istr.operand1, op1.m_I64, istr.operand2, dest.m_I64);
        }
        case ISubImm: {
            sprintf(varying, "r%d, r%d, %d", istr.destination, istr.operand1, istr.operand2);
            i64 value = op1.m_I64 - istr.operand2;
            dest.m_type = R_I64;
            dest.m_I64 = value;
            sprintf(interpreted, "r%d = r%d [%d] - %d -> <%d>", istr.destination, istr.operand1, op1.m_I64, istr.operand2, dest.m_I64);
        }
        case IMulImm: {
            sprintf(varying, "r%d, r%d, %d", istr.destination, istr.operand1, istr.operand2);
            i64 value = op1.m_I64 * istr.operand2;
            dest.m_type = R_I64;
            dest.m_I64 = value;
            sprintf(interpreted, "r%d = r%d [%d] * %d -> <%d>", istr.destination, istr.operand1, op1.m_I64, istr.operand2, dest.m_I64);
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
            sprintf(interpreted, "r%d = r%d [%d] / %d -> <%d>", istr.destination, istr.operand1, op1.m_I64, istr.operand2, dest.m_I64);
            break;
        }
        case LoadStaticI32Imm: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            i32 value = *reinterpret_cast<i32*>(stackFrame.m_symbolTablePtr + istr.operand1); 
            dest.m_type = RegisterValueType::R_I32;
            dest.m_I32 = value;
            table_entry.m_type = SymbolTableEntryType::INT;
            table_entry.m_i64 = value;
            sprintf(interpreted, "r%d = ST[%d] -> <%d>", istr.destination, istr.operand1, value);
            break;
        }
        case LoadStaticFloatImm: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            f32 value = *reinterpret_cast<f32*>(stackFrame.m_symbolTablePtr + istr.operand1); 
            dest.m_type = RegisterValueType::R_F32;
            dest.m_F32 = value;
            table_entry.m_type = SymbolTableEntryType::FLOAT;
            table_entry.m_f32 = value;
            sprintf(interpreted, "r%d = ST[%d] -> <%.2f>", istr.destination, istr.operand1, value);
            break;
        }
        case LoadStaticPointerImm: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            uintptr_t value = reinterpret_cast<uintptr_t*>(stackFrame.m_symbolTablePtr)[istr.operand1]; 
            dest.m_type = RegisterValueType::R_POINTER;
            dest.m_PTR = {value, 0};
            table_entry.m_type = SymbolTableEntryType::POINTER;
            table_entry.m_pointer = value;
            sprintf(interpreted, "r%d = ST[%d] -> <0x%X>", istr.destination, istr.operand1, value);
            break;
        }
        case LoadStaticI64Imm: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            i64 value = reinterpret_cast<i64*>(stackFrame.m_symbolTablePtr)[istr.operand1]; 
            dest.m_type = RegisterValueType::R_I64;
            dest.m_I64 = value;
            table_entry.m_type = SymbolTableEntryType::INT;
            table_entry.m_i64 = value;
            sprintf(interpreted, "r%d = ST[%d] -> <%d>", istr.destination, istr.operand1, value);
            break;
        }
        case LoadStaticU64Imm: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            u64 value = reinterpret_cast<u64*>(stackFrame.m_symbolTablePtr)[istr.operand1];
            if (value >= 0x000FFFFFFFFFFFFF) {
                dest.m_type = RegisterValueType::R_HASH;
                dest.m_SID = value;
                table_entry.m_type = SymbolTableEntryType::STRINGID_64;
                table_entry.m_hash = value;
                stackFrame.to_string(dst_str, istr.destination, this->lookup(value));
                sprintf(interpreted, "r%d = ST[%d] -> <%s>", istr.destination, istr.operand1, dst_str);
            } else if (value % 8 == 0 && value < this->m_currentFile.m_bytes.size()) {
                dest.m_type = RegisterValueType::R_POINTER;
                dest.m_PTR = {value, 0};
                table_entry.m_type = SymbolTableEntryType::POINTER;
                table_entry.m_pointer = value;
                sprintf(interpreted, "r%d = ST[%d] -> <%s>", istr.destination, istr.operand1, this->m_currentFile.m_bytes[value]);
            }
            break;
        }
        case Move: {
            sprintf(varying, "r%d, r%d", istr.destination, istr.operand1);
            dest = op1;
            sprintf(interpreted, "r%d = r%d [%s]", istr.destination, istr.operand1, op1_str);
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

void ListingViewController::insert_function_disassembly_text(const FunctionDisassembly &functionDisassembly) {
    auto labels = functionDisassembly.m_stackFrame.m_labels;
    char buffer[128] = {0};
    for (const auto &line : functionDisassembly.m_lines) {
        u32 line_offset = std::max(67ull - line.m_text.length(), 0ull);
        auto label_loc = std::find(labels.begin(), labels.end(), line.m_location);
        if (label_loc != labels.end()) {
            const u32 label_idx = std::distance(labels.begin(), label_loc);
            if (line.m_location == functionDisassembly.m_lines.size() - 1) {
                this->insert_span("L_RETURN:\n", MainWindow::COMMENT_COLOR, 14, 10);
            } else {
                sprintf(buffer, "L_%d:\n", label_idx);
                this->insert_span(buffer, MainWindow::COMMENT_COLOR, 14, 10);  
            }
        }
        this->insert_span(line.m_text.c_str(), MainWindow::OPCODE_COLOR, 12, 14);
        std::string comment = std::string(line_offset, ' ') + line.m_comment;
        this->insert_span(comment.c_str(), MainWindow::STRING_COLOR, 12);
        if (line.m_label != -1) {
            u32 target = std::distance(labels.begin(), std::find(labels.begin(), labels.end(), line.m_label));
            if (line.m_label == functionDisassembly.m_lines.size() - 1) {
                this->insert_span("=> L_RETURN", MainWindow::COMMENT_COLOR, 12);
            } else {
                sprintf(buffer, "=> L_%d", target);
                this->insert_span(buffer, MainWindow::COMMENT_COLOR, 12);
            }
        }
        this->insert_span("\n");
    }
    this->insert_span("SYMBOL TABLE: \n", MainWindow::COMMENT_COLOR, 12, 12);

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
                sprintf(type, "INT <%d>\n", entry.m_i64);
                break;
            }
            case SymbolTableEntryType::POINTER:{
                sprintf(type, "POINTER <%s>\n", this->lookup(entry.m_hash));
                break;
            }
            case SymbolTableEntryType::STRINGID_64: {
                sprintf(type, "SID <%s>\n", this->lookup(entry.m_hash));
                break;
            }
        }
        sprintf(buffer, "%s %s", line_start, type);
        this->insert_span(buffer, MainWindow::COMMENT_COLOR, 12, 14);
    }
}

void ListingViewController::insert_header_line() {
    const char* current_script_name;
    const char* current_script_id;
    if (this->m_currentFile.m_dcscript != nullptr) {
        current_script_name = this->lookup(this->m_currentFile.m_dcscript->m_stateScriptId);
        current_script_id = int_to_string_id(this->m_currentFile.m_dcscript->m_stateScriptId);
    } else {
        current_script_name = "UNKOWN SCRIPT";
        current_script_id = "UNKNOWN SCRIPT ID";
    }
    this->m_mainWindow->getListingView()->clear();
    this->m_mainWindow->getListingView()->setReadOnly(true);
    // this->insert_span("DeepQuantum's DC Disassembler ver. " + std::to_string(MainWindow::VersionNumber) + "\n", MainWindow::STRING_COLOR, 14, 20);
    // this->insert_span("Listing for script: " + current_script_name + "\n", MainWindow::STRING_COLOR , 14, 20);
    // this->insert_span("Script ID: " + current_script_id + "\n", MainWindow::STRING_COLOR, 14, 20);
    // this->insert_span("Filesize: " + std::to_string(this->m_currentFile.m_size) + " bytes\n\n", MainWindow::STRING_COLOR, 14, 20);
    // this->insert_span("START OF DISASSEMBLY\n", MainWindow::COMMENT_COLOR, 14, 20);
    // this->insert_span("--------------------------------------------------\n", MainWindow::COMMENT_COLOR, 14, 20);
}