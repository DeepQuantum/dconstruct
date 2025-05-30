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

void ListingViewController::checkSymbolTableLoad(const Instruction &instruction, u64 * const tablePtr, std::map<u8, SymbolTableEntry> &symbols) {
    SymbolTableEntry ste{};
    ste.m_index = instruction.operand1;
    switch (instruction.opcode) {
        case Opcode::LoadStaticInt:
            ste.m_type = SymbolTableEntryType::INT;
            ste.m_i32 = *reinterpret_cast<i32*>(&tablePtr[instruction.operand1]);
            break;
        case Opcode::LookupPointer:
        case Opcode::LoadStaticPointerImm:
            ste.m_type = SymbolTableEntryType::POINTER;
            ste.m_pointer = *reinterpret_cast<intptr_t*>(&tablePtr[instruction.operand1]);
            break;
        case Opcode::LoadStaticFloatImm:
            ste.m_type = SymbolTableEntryType::FLOAT;
            ste.m_f32 = *reinterpret_cast<f32*>(&tablePtr[instruction.operand1]);
            break;
        default:
            ste.m_type = SymbolTableEntryType::UNKNOWN_TYPE;
            ste.m_hash = tablePtr[instruction.operand1];
    }
    symbols.emplace(instruction.operand1, ste);
}

FunctionDisassembly ListingViewController::createFunctionDisassembly(ScriptLambda *lambda) {
    this->insertSpan("INSTRUCTION POINTER: " + MainWindow::offsetToString(this->getOffset(lambda->m_pOpcode)) + "\n", MainWindow::COMMENT_COLOR, 14, 20);
    this->insertSpan("SYMBOL TABLE POINTER: " + MainWindow::offsetToString(this->getOffset(lambda->m_pSymbols)) + "\n", MainWindow::COMMENT_COLOR, 14, 20);

    StackFrame stackFrame;

    Instruction *instructionPtr = reinterpret_cast<Instruction*>(lambda->m_pOpcode);
    u64 instructionCount = reinterpret_cast<Instruction*>(lambda->m_pSymbols) - instructionPtr;
    u8 symbolTableSize = 0;

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
    std::string comment = "";
    const Instruction istr = line.m_instruction;
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
    switch (istr.opcode) {
        case Return: {
            sprintf(varying, "r%d", istr.operand1);
            comment = "Return " + stackFrame[istr.operand1];
            break;
        }
        case IAdd: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            comment = "%s = %s + %s" + stackFrame[istr.destination], stackFrame[istr.operand1], stackFrame[istr.operand2];
            stackFrame.registers[istr.destination].m_type = RegisterValueType::R_I64;
            stackFrame.registers[istr.destination].m_value.m_I64 = stackFrame.registers[istr.operand1].m_value.m_I64 + stackFrame.registers[istr.operand2].m_value.m_I64;
        }
        case ISub: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            comment = "%s = %s - %s" + stackFrame[istr.destination], stackFrame[istr.operand1], stackFrame[istr.operand2];
            stackFrame.registers[istr.destination].m_type = RegisterValueType::R_I64;
            stackFrame.registers[istr.destination].m_value.m_I64 = stackFrame.registers[istr.operand1].m_value.m_I64 - stackFrame.registers[istr.operand2].m_value.m_I64;
        }
        case IMul: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            comment = "%s = %s * %s" + stackFrame[istr.destination], stackFrame[istr.operand1], stackFrame[istr.operand2];
            stackFrame.registers[istr.destination].m_type = RegisterValueType::R_I64;
            stackFrame.registers[istr.destination].m_value.m_I64 = stackFrame.registers[istr.operand1].m_value.m_I64 * stackFrame.registers[istr.operand2].m_value.m_I64;
        }
        case IDiv: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            comment = "%s = %s / %s" + stackFrame[istr.destination], stackFrame[istr.operand1], stackFrame[istr.operand2];
            stackFrame.registers[istr.destination].m_type = RegisterValueType::R_I64;
            if (stackFrame.registers[istr.operand2].m_value.m_I64 == 0) {
                stackFrame.registers[istr.destination].m_value.m_I64 = stackFrame.registers[istr.operand1].m_value.m_I64 / 1;
                comment += " divide by 0";
            } else {
                stackFrame.registers[istr.destination].m_value.m_I64 = stackFrame.registers[istr.operand1].m_value.m_I64 / stackFrame.registers[istr.operand2].m_value.m_I64;
            }
        }
        case FAdd: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            comment = "%s = %s + %s" + stackFrame[istr.destination], stackFrame[istr.operand1], stackFrame[istr.operand2];
            stackFrame.registers[istr.destination].m_type = RegisterValueType::R_F32;
            stackFrame.registers[istr.destination].m_value.m_I64 = stackFrame.registers[istr.operand1].m_value.m_F32 + stackFrame.registers[istr.operand2].m_value.m_F32;
        }
        case FSub: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            comment = "%s = %s - %s" + stackFrame[istr.destination], stackFrame[istr.operand1], stackFrame[istr.operand2];
            stackFrame.registers[istr.destination].m_type = RegisterValueType::R_F32;
            stackFrame.registers[istr.destination].m_value.m_I64 = stackFrame.registers[istr.operand1].m_value.m_F32 - stackFrame.registers[istr.operand2].m_value.m_F32;
        }
        case FMul: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            comment = "%s = %s * %s" + stackFrame[istr.destination], stackFrame[istr.operand1], stackFrame[istr.operand2];
            stackFrame.registers[istr.destination].m_type = RegisterValueType::R_F32;
            stackFrame.registers[istr.destination].m_value.m_I64 = stackFrame.registers[istr.operand1].m_value.m_F32 * stackFrame.registers[istr.operand2].m_value.m_F32;
        }
        case FDiv: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            comment = "%s = %s / %s" + stackFrame[istr.destination], stackFrame[istr.operand1], stackFrame[istr.operand2];
            stackFrame.registers[istr.destination].m_type = RegisterValueType::R_F32;
            if (stackFrame.registers[istr.operand2].m_value.m_F32 == 0) {
                stackFrame.registers[istr.destination].m_value.m_F32 = stackFrame.registers[istr.operand1].m_value.m_F32 / 1;
                comment += " divide by 0";
            } else {
                stackFrame.registers[istr.destination].m_value.m_F32 = stackFrame.registers[istr.operand1].m_value.m_F32 / stackFrame.registers[istr.operand2].m_value.m_F32;
            }
        }
        case LoadStaticInt: {
            
        }
        case Branch: {
            
        }
    }
    line.m_text = std::string(disassembly_text);
    line.m_comment = comment + "\n";
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
    std::string current_script_name = this->m_mainWindow->resolveHash(this->m_currentFile.m_dcscript->m_stateScriptId);
    this->m_mainWindow->getListingView()->clear();
    this->m_mainWindow->getListingView()->setReadOnly(true);
    this->insertSpan("DeepQuantum's DC Disassembler ver. " + std::to_string(MainWindow::VersionNumber) + "\n", MainWindow::STRING_COLOR, 14, 20);
    this->insertSpan("Listing for script: " + current_script_name + "\n", MainWindow::STRING_COLOR , 14, 20);
    this->insertSpan("Script ID: " + MainWindow::intToSIDString(this->m_currentFile.m_dcscript->m_stateScriptId), MainWindow::STRING_COLOR, 14, 20);
    this->insertSpan("Filesize: " + std::to_string(this->m_currentFile.m_size) + " bytes\n\n", MainWindow::STRING_COLOR, 14, 20);
    this->insertSpan("START OF DISASSEMBLY\n", MainWindow::COMMENT_COLOR, 14, 20);
    this->insertSpan("--------------------------------------------------\n", MainWindow::COMMENT_COLOR, 14, 20);
}