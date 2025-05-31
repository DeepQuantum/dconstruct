#include "listing_view_controller.h"
#include "../mainwindow.h"
#include <stdexcept>
#include <cmath>
#include <sstream>


ListingViewController::ListingViewController(BinaryFile &file, MainWindow *mainWindow) {
    this->m_mainWindow = mainWindow;
    this->m_currentFile = file;
    this->createListingView();
}

void ListingViewController::insertSpan(const std::string &line, QColor color, int fontSize, u8 indent) {
    QTextCursor cursor = this->m_mainWindow->getListingView()->textCursor();
    cursor.movePosition(QTextCursor::End);
    this->m_mainWindow->getListingView()->setTextCursor(cursor);


    QTextCharFormat format;
    format.setForeground(color);
    format.setFontPointSize(fontSize);
    cursor.insertText(QString::fromStdString(std::string(indent, ' ') + line), format);
}

void ListingViewController::insertComment(const std::string &comment, u8 indent) {
    this->insertSpan(comment, MainWindow::COMMENT_COLOR, 14, indent);
}

void ListingViewController::insertHash(const std::string &hash, b8 surround) {
    std::string out;
    if (surround) {
        out = "<" + hash + ">";
    } else {
        out = hash;
    }
    this->insertSpan(out, MainWindow::HASH_COLOR, 14, 0);
}

void ListingViewController::createListingView() {
    this->insertHeaderLine();
    for (const auto &symbol : this->m_currentFile.m_symbols) {
        std::string line;
        switch (symbol.type) {
            case SymbolType::B8: {
                line = "BOOL: " + MainWindow::offsetToString(this->getOffset(symbol.b8_ptr)) + " <" + (*symbol.b8_ptr ? "true" : "false") + ">";
                this->insertSpan(line + "\n", MainWindow::OPCODE_COLOR, 10, 2);
                break;
            }
            case SymbolType::I32: {
                line = "Integer: " + MainWindow::offsetToString(this->getOffset(symbol.i32_ptr)) + " <" + std::to_string(*symbol.i32_ptr) + ">";
                this->insertSpan(line + "\n", MainWindow::OPCODE_COLOR, 10, 2);
                break;
            }
            case SymbolType::F32: {
                line = "Float: " + MainWindow::offsetToString(this->getOffset(symbol.f32_ptr)) + " <" + std::to_string(*symbol.f32_ptr) + ">";
                this->insertSpan(line + "\n", MainWindow::OPCODE_COLOR, 10, 2);
                break;
            }
            case SymbolType::HASH: {
                line = "Hash: " + MainWindow::intToSIDString(*reinterpret_cast<uint64_t*>(symbol.hash_ptr));
                this->insertSpan(line + "\n", MainWindow::OPCODE_COLOR, 10, 2);
                break;
            }
            case SymbolType::SS: {
                line = "State Script ID: " + MainWindow::intToSIDString(reinterpret_cast<StateScript*>(symbol.ss_ptr)->m_stateScriptId);
                this->insertSpan(line + "\n", MainWindow::OPCODE_COLOR, 10, 2);
                break;
            }
            case SymbolType::LAMBDA: {
                this->insertComment("BEGIN LAMBDA ", 20);
                this->insertHash(this->m_mainWindow->resolveHash(symbol.id), true);
                this->insertSpan(" AT ", MainWindow::COMMENT_COLOR, 14);
                this->insertSpan("[" + MainWindow::offsetToString(this->getOffset(symbol.lambda_ptr)) + "]\n", MainWindow::OPCODE_COLOR, 14);
                auto function = this->createFunctionDisassembly(symbol.lambda_ptr);
                this->insertFunctionDisassemblyText(function);
                break;
            }
            default: {
                this->insertSpan("UNKNOWN SYMBOL TYPE: ", MainWindow::COMMENT_COLOR, 14, 20);
                this->insertSpan(MainWindow::offsetToString(this->getOffset(reinterpret_cast<void*>(symbol.i32_ptr_raw))));
                break;
            }
        }
        
    }
}


u32 ListingViewController::getOffset(const void *symbol) {
    return reinterpret_cast<uintptr_t>(symbol) - reinterpret_cast<uintptr_t>(this->m_currentFile.m_dcheader);
}

// void ListingViewController::checkSymbolTableLoad(const Instruction &instruction, u64 * const tablePtr, std::map<u8, SymbolTableEntry> &symbols) {
//     SymbolTableEntry ste{};
//     ste.m_index = instruction.operand1;
//     switch (instruction.opcode) {
//         case Opcode::LoadStaticInt:
//             ste.m_type = SymbolTableEntryType::INT;
//             ste.m_i32 = *reinterpret_cast<i32*>(&tablePtr[instruction.operand1]);
//             break;
//         case Opcode::LookupPointer:
//         case Opcode::LoadStaticPointerImm:
//             ste.m_type = SymbolTableEntryType::POINTER;
//             ste.m_pointer = *reinterpret_cast<intptr_t*>(&tablePtr[instruction.operand1]);
//             break;
//         case Opcode::LoadStaticFloatImm:
//             ste.m_type = SymbolTableEntryType::FLOAT;
//             ste.m_f32 = *reinterpret_cast<f32*>(&tablePtr[instruction.operand1]);
//             break;
//         default:
//             ste.m_type = SymbolTableEntryType::UNKNOWN_TYPE;
//             ste.m_hash = tablePtr[instruction.operand1];
//     }
//     symbols.emplace(instruction.operand1, ste);
// }

FunctionDisassembly ListingViewController::createFunctionDisassembly(ScriptLambda *lambda) {
    this->insertSpan("INSTRUCTION POINTER: " + MainWindow::offsetToString(this->getOffset(lambda->m_pOpcode)) + "\n", MainWindow::COMMENT_COLOR, 14, 20);
    this->insertSpan("SYMBOL TABLE POINTER: " + MainWindow::offsetToString(this->getOffset(lambda->m_pSymbols)) + "\n", MainWindow::COMMENT_COLOR, 14, 20);

    StackFrame stackFrame;
    stackFrame.m_symbolTablePtr = lambda->m_pSymbols;

    Instruction *instructionPtr = reinterpret_cast<Instruction*>(lambda->m_pOpcode);
    u64 instructionCount = reinterpret_cast<Instruction*>(lambda->m_pSymbols) - instructionPtr;

    std::vector<FunctionDisassemblyLine> lines;
    lines.reserve(instructionCount);

    FunctionDisassembly functionDisassembly {
        std::move(lines),
        std::vector<SymbolTableEntry>()
    };

    for (u64 i = 0; i < instructionCount; ++i) {
        functionDisassembly.m_lines.emplace_back(i, instructionPtr);
    }

    for (u64 i = 0; i < instructionCount; ++i) {
        auto &line = functionDisassembly.m_lines[i];
        processInstruction(stackFrame, line);
        if (line.m_instruction.isBranchInstruction()) {
            functionDisassembly.m_lines[line.m_instruction.destination].m_locationsPointedFrom.push_back(i); 
        }
        functionDisassembly.m_lines[i] = line;
    }
    return functionDisassembly;
}

void ListingViewController::processInstruction(StackFrame &stackFrame, FunctionDisassemblyLine &line) {
    char disassembly_text[256] = {0};
    char comment[128] = {0};
    const Instruction istr = line.m_instruction;
    SymbolTableEntry table_entry;
    table_entry.m_type = NONE;
    sprintf(disassembly_text, "%04X   0x%06X   %02X %02X %02X %02X       %-20s",
            line.m_location,
            this->getOffset((void*)line.m_globalPointer),
            istr.opcode,
            istr.destination,
            istr.operand1,
            istr.operand2,
            istr.opcodeToString().c_str()
    );
    char *varying = disassembly_text + 49;
    Register &dest = stackFrame.registers[istr.destination];
    Register &op1 = stackFrame.registers[istr.operand1];
    Register &op2 = stackFrame.registers[istr.operand2];
    switch (istr.opcode) {
        case Return: {
            sprintf(varying, "r%d", istr.destination);
            sprintf(comment, "Return %s", stackFrame[istr.destination].c_str());
            break;
        }
        case IAdd: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            if (op1.m_type == RegisterValueType::R_POINTER) {
                dest.m_type = RegisterValueType::R_POINTER;
                dest.m_PTR.m_base = op1.m_PTR.m_base;
                dest.m_PTR.m_offset = op1.m_PTR.m_base + op1.m_I64;
            } else {
                dest.m_type = RegisterValueType::R_I64;
                dest.m_I64 = op1.m_I64 + op2.m_I64;
            }
            sprintf(comment, "%s = %s + %s", stackFrame[istr.destination].c_str(), stackFrame[istr.operand1].c_str(), stackFrame[istr.operand2].c_str());
            break;
        }
        case ISub: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = RegisterValueType::R_I64;
            dest.m_I64 = op1.m_I64 - op2.m_I64;
            sprintf(comment, "%s = %s - %s", stackFrame[istr.destination].c_str(), stackFrame[istr.operand1].c_str(), stackFrame[istr.operand2].c_str());
            break;
        }
        case IMul: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = RegisterValueType::R_I64;
            dest.m_I64 = op1.m_I64 * op2.m_I64;
            sprintf(comment, "%s = %s * %s", stackFrame[istr.destination].c_str(), stackFrame[istr.operand1].c_str(), stackFrame[istr.operand2].c_str());
            break;
        }
        case IDiv: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = RegisterValueType::R_I64;
            if (op2.m_I64 == 0) {
                dest.m_I64 = op1.m_I64 / 1;
                sprintf(comment + 13, "%s", " divide by 0");
            } else {
                dest.m_I64 = op1.m_I64 / op2.m_I64;
            }
            sprintf(comment, "%s = %s / %s", stackFrame[istr.destination].c_str(), stackFrame[istr.operand1].c_str(), stackFrame[istr.operand2].c_str());
            break;
        }
        case FAdd: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = RegisterValueType::R_F32;
            dest.m_I64 = op1.m_F32 + op2.m_F32;
            sprintf(comment, "%s = %s + %s", stackFrame[istr.destination].c_str(), stackFrame[istr.operand1].c_str(), stackFrame[istr.operand2].c_str());
            break;
        }
        case FSub: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = RegisterValueType::R_F32;
            dest.m_I64 = op1.m_F32 - op2.m_F32;
            sprintf(comment, "%s = %s - %s", stackFrame[istr.destination].c_str(), stackFrame[istr.operand1].c_str(), stackFrame[istr.operand2].c_str());
            break;
        }
        case FMul: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = RegisterValueType::R_F32;
            dest.m_I64 = op1.m_F32 * op2.m_F32;
            sprintf(comment, "%s = %s * %s", stackFrame[istr.destination].c_str(), stackFrame[istr.operand1].c_str(), stackFrame[istr.operand2].c_str());
            break;
        }
        case FDiv: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = RegisterValueType::R_F32;
            if (op2.m_F32 == 0) {
                dest.m_F32 = op1.m_F32 / 1;
                sprintf(comment + 13, "%s", " divide by 0");
            } else {
                dest.m_F32 = op1.m_F32 / op2.m_F32;
            }
            sprintf(comment, "%s = %s / %s", stackFrame[istr.destination].c_str(), stackFrame[istr.operand1].c_str(), stackFrame[istr.operand2].c_str());
            break;
        }
        case LoadStaticInt: {
            const i64 table_value = reinterpret_cast<i64*>(stackFrame.m_symbolTablePtr)[istr.operand1];
            table_entry.m_type = SymbolTableEntryType::INT;
            table_entry.m_i64 = table_value;
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_I64;
            dest.m_I64 = table_value;
            sprintf(comment, "%s = ST[%d] -> <%s>", stackFrame[istr.destination], istr.operand1, stackFrame[istr.operand1]); 
            break;
        }
        case LoadStaticFloat: {
            const f32 table_value = reinterpret_cast<f32*>(stackFrame.m_symbolTablePtr)[istr.operand1];
            table_entry.m_type = SymbolTableEntryType::FLOAT;
            table_entry.m_f32 = table_value;
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_F32;
            dest.m_F32 = table_value;
            sprintf(comment, "%s = ST[%d] -> <%s>", stackFrame[istr.destination], istr.operand1, stackFrame[istr.operand1]); 
            break;
        }
        case LoadStaticPointer: {
            const intptr_t table_value = reinterpret_cast<intptr_t*>(stackFrame.m_symbolTablePtr)[istr.operand1];
            table_entry.m_type = SymbolTableEntryType::POINTER;
            table_entry.m_pointer = table_value;
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_POINTER;
            dest.m_PTR.m_base = table_value;
            sprintf(comment, "%s = ST[%d] -> <%s>", stackFrame[istr.destination], istr.operand1, stackFrame[istr.operand1]); 
            break;
        }
        case LoadU16Imm: {
            const u16 value = istr.operand1 | (istr.operand2 << 8);
            sprintf(varying, "r%d, %d", istr.destination, value);
            dest.m_type = RegisterValueType::R_U16;
            dest.m_U16 = value;
            sprintf(comment, "r%d = %d", istr.destination, value);
            break;
        }
        case LoadU32: {
            sprintf(varying, "r%d, [r%d]", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_I32;
            dest.m_I32 = 0;
            sprintf(comment, "r%d = [0x%x + 0x%x]", istr.destination, op1.m_PTR.m_base, op1.m_PTR.m_offset);
            break;
        }
        case LoadFloat: {
            sprintf(varying, "r%d, [r%d]", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_F32;
            dest.m_F32 = 0.f;
            sprintf(comment, "r%d = [0x%x + 0x%x]", istr.destination, op1.m_PTR.m_base, op1.m_PTR.m_offset);
            break;
        }
        case LoadPointer: {
            sprintf(varying, "r%d, [r%d]", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_POINTER;
            dest.m_PTR = {0, 0};
            sprintf(comment, "r%d = [0x%x + 0x%x]", istr.destination, op1.m_PTR.m_base, op1.m_PTR.m_offset);
            break;
        }
        case LoadI64: {
            sprintf(varying, "r%d, [r%d]", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_I64;
            dest.m_I64 = 0;
            sprintf(comment, "r%d = [0x%x + 0x%x]", istr.destination, op1.m_PTR.m_base, op1.m_PTR.m_offset);
            break;
        }
        case LoadU64: {
            sprintf(varying, "r%d, [r%d]", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_U64;
            dest.m_U64 = 0;
            sprintf(comment, "r%d = [0x%x + 0x%x]", istr.destination, op1.m_PTR.m_base, op1.m_PTR.m_offset);
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
            sprintf(comment, "r%d = ST[%d] -> <%s>", istr.destination, istr.operand1, this->m_mainWindow->resolveHash(value).c_str());
            break;
        }
        case LookupFloat: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            f32 value = reinterpret_cast<f32*>(stackFrame.m_symbolTablePtr)[istr.operand1]; 
            dest.m_type = RegisterValueType::R_F32;
            dest.m_F32 = value;
            table_entry.m_type = SymbolTableEntryType::FLOAT;
            table_entry.m_f32 = value;
            sprintf(comment, "r%d = ST[%d] -> <%.2f>", istr.destination, istr.operand1, value);
            break;
        }
        case LookupPointer: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            intptr_t value = reinterpret_cast<intptr_t*>(stackFrame.m_symbolTablePtr)[istr.operand1];
            dest.m_type = RegisterValueType::R_POINTER;
            dest.m_PTR.m_base = value;
            dest.m_PTR.m_offset = 0;
            table_entry.m_type = SymbolTableEntryType::POINTER;
            table_entry.m_pointer = value;
            sprintf(comment, "r%d = ST[%d] -> <%s>", istr.destination, istr.operand1, this->m_mainWindow->resolveHash(value).c_str());
        }
        case Branch: {
        }
    }
    if (table_entry.m_type != NONE) {
        stackFrame.m_symbolTable.push_back(table_entry);
    }
    line.m_text = std::string(disassembly_text);
    line.m_comment = std::string(comment) + "\n";
}

void ListingViewController::insertFunctionDisassemblyText(const FunctionDisassembly &functionDisassembly) {
    std::set<u64> labels;
    for (const auto &line : functionDisassembly.m_lines) {
        if (!line.m_locationsPointedFrom.empty()) {
            auto label_index_it = labels.find(line.m_location);
            u64 label_index;
            if (label_index_it != labels.end()) {
                label_index = *label_index_it;
            } else {
                label_index = labels.size();
                labels.insert(line.m_location);
            }
            this->insertSpan("LABEL_" + std::to_string(label_index) + ":\n", MainWindow::COMMENT_COLOR, 10, 12);
        }
        this->insertSpan(line.m_text, MainWindow::OPCODE_COLOR, 12, 14);
        this->insertSpan(line.m_comment, MainWindow::STRING_COLOR, 12, 14);
    }
}

void ListingViewController::insertHeaderLine() {
    std::string current_script_name;
    std::string current_script_id;
    if (this->m_currentFile.m_dcscript != nullptr) {
        current_script_name = this->m_mainWindow->resolveHash(this->m_currentFile.m_dcscript->m_stateScriptId);
        current_script_id = MainWindow::intToSIDString(this->m_currentFile.m_dcscript->m_stateScriptId);
    } else {
        current_script_name = "UNKOWN SCRIPT";
        current_script_id = "UNKNOWN SCRIPT ID";
    }
    this->m_mainWindow->getListingView()->clear();
    this->m_mainWindow->getListingView()->setReadOnly(true);
    this->insertSpan("DeepQuantum's DC Disassembler ver. " + std::to_string(MainWindow::VersionNumber) + "\n", MainWindow::STRING_COLOR, 14, 20);
    this->insertSpan("Listing for script: " + current_script_name + "\n", MainWindow::STRING_COLOR , 14, 20);
    this->insertSpan("Script ID: " + current_script_id + "\n", MainWindow::STRING_COLOR, 14, 20);
    this->insertSpan("Filesize: " + std::to_string(this->m_currentFile.m_size) + " bytes\n\n", MainWindow::STRING_COLOR, 14, 20);
    this->insertSpan("START OF DISASSEMBLY\n", MainWindow::COMMENT_COLOR, 14, 20);
    this->insertSpan("--------------------------------------------------\n", MainWindow::COMMENT_COLOR, 14, 20);
}