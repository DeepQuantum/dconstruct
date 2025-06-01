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


    Instruction *instructionPtr = reinterpret_cast<Instruction*>(lambda->m_pOpcode);
    u64 instructionCount = reinterpret_cast<Instruction*>(lambda->m_pSymbols) - instructionPtr;

    std::vector<FunctionDisassemblyLine> lines;
    lines.reserve(instructionCount);

    FunctionDisassembly functionDisassembly {
        std::move(lines),
        std::vector<SymbolTableEntry>(),
        StackFrame(),
    };

    functionDisassembly.m_stackFrame.m_symbolTablePtr = lambda->m_pSymbols;

    for (u64 i = 0; i < instructionCount; ++i) {
        functionDisassembly.m_lines.emplace_back(i, instructionPtr);
    }

    for (u64 i = 0; i < instructionCount; ++i) {
        processInstruction(functionDisassembly.m_stackFrame, functionDisassembly.m_lines[i]);
    }
    return functionDisassembly;
}

void ListingViewController::processInstruction(StackFrame &stackFrame, FunctionDisassemblyLine &line) {
    char disassembly_text[256] = {0};
    char comment[128] = {0};
    const Instruction istr = line.m_instruction;
    SymbolTableEntry table_entry;
    table_entry.m_type = NONE;
    sprintf(disassembly_text, "%04X   0x%06X   %02X %02X %02X %02X   %-20s",
            line.m_location,
            this->getOffset((void*)(line.m_globalPointer + line.m_location)),
            istr.opcode,
            istr.destination,
            istr.operand1,
            istr.operand2,
            istr.opcodeToString().c_str()
    );
    char *varying = disassembly_text + strlen(disassembly_text);

    Register &dest = stackFrame[istr.destination < 128 ? istr.destination : 0];
    Register &op1 = stackFrame[istr.operand1 < 128 ? istr.operand1 : 0];
    Register &op2 = stackFrame[istr.operand2 < 128 ? istr.operand2 : 0];

    const std::string dst_stdstr = stackFrame.toString(istr.destination < 128 ? istr.destination : 0, this->m_mainWindow->resolveHash(dest.m_SID));
    const std::string op1_stdstr = stackFrame.toString(istr.operand1 < 128 ? istr.operand1 : 0, this->m_mainWindow->resolveHash(op1.m_SID));
    const std::string op2_stdstr = stackFrame.toString(istr.operand2 < 128 ? istr.operand2 : 0, this->m_mainWindow->resolveHash(op2.m_SID));

    const char *dst_str = dst_stdstr.c_str();
    const char *op1_str = op1_stdstr.c_str();
    const char *op2_str = op2_stdstr.c_str();

    switch (istr.opcode) {
        case Return: {
            sprintf(varying, "r%d", istr.destination);
            sprintf(comment, "Return %s", dst_str);
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
            sprintf(comment, "%s = %s + %s", stackFrame.toString(istr.destination).c_str(), op1_str, op2_str);
            break;
        }
        case ISub: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = RegisterValueType::R_I64;
            dest.m_I64 = op1.m_I64 - op2.m_I64;
            sprintf(comment, "%s = %s - %s", stackFrame.toString(istr.destination).c_str(), op1_str, op2_str);
            break;
        }
        case IMul: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = RegisterValueType::R_I64;
            dest.m_I64 = op1.m_I64 * op2.m_I64;
            sprintf(comment, "%s = %s * %s", stackFrame.toString(istr.destination).c_str(), op1_str, op2_str);
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
            sprintf(comment, "%s = %s / %s", stackFrame.toString(istr.destination).c_str(), op1_str, op2_str);
            break;
        }
        case FAdd: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = RegisterValueType::R_F32;
            dest.m_I64 = op1.m_F32 + op2.m_F32;
            sprintf(comment, "%s = %s + %s", stackFrame.toString(istr.destination).c_str(), op1_str, op2_str);
            break;
        }
        case FSub: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = RegisterValueType::R_F32;
            dest.m_I64 = op1.m_F32 - op2.m_F32;
            sprintf(comment, "%s = %s - %s", stackFrame.toString(istr.destination).c_str(), op1_str, op2_str);
            break;
        }
        case FMul: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = RegisterValueType::R_F32;
            dest.m_I64 = op1.m_F32 * op2.m_F32;
            sprintf(comment, "%s = %s * %s", stackFrame.toString(istr.destination).c_str(), op1_str, op2_str);
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
            sprintf(comment, "%s = %s / %s", stackFrame.toString(istr.destination).c_str(), op1_str, op2_str);
            break;
        }
        case LoadStaticInt: {
            const i64 table_value = reinterpret_cast<i64*>(stackFrame.m_symbolTablePtr)[istr.operand1];
            table_entry.m_type = SymbolTableEntryType::INT;
            table_entry.m_i64 = table_value;
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_I64;
            dest.m_I64 = table_value;
            sprintf(comment, "%s = ST[%d] -> <%s>", stackFrame.toString(istr.destination).c_str(), istr.operand1, op1_str); 
            break;
        }
        case LoadStaticFloat: {
            const f32 table_value = reinterpret_cast<f32*>(stackFrame.m_symbolTablePtr)[istr.operand1];
            table_entry.m_type = SymbolTableEntryType::FLOAT;
            table_entry.m_f32 = table_value;
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_F32;
            dest.m_F32 = table_value;
            sprintf(comment, "%s = ST[%d] -> <%s>", stackFrame.toString(istr.destination).c_str(), istr.operand1, op1_str); 
            break;
        }
        case LoadStaticPointer: {
            const intptr_t table_value = reinterpret_cast<intptr_t*>(stackFrame.m_symbolTablePtr)[istr.operand1];
            table_entry.m_type = SymbolTableEntryType::POINTER;
            table_entry.m_pointer = table_value;
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            dest.m_type = RegisterValueType::R_POINTER;
            dest.m_PTR.m_base = table_value;
            sprintf(comment, "%s = ST[%d] -> <%s>", stackFrame.toString(istr.destination).c_str(), istr.operand1, op1_str); 
            break;
        }
        case LoadU16Imm: {
            const u16 value = istr.operand1 | (istr.operand2 << 8);
            sprintf(varying, "r%d, %d", istr.destination, value);
            dest.m_type = RegisterValueType::R_U16;
            dest.m_U64 = value;
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
            f32 value = *reinterpret_cast<f32*>(stackFrame.m_symbolTablePtr + istr.operand1); 
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
            break;
        }
        case MoveInt: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            dest.m_type = R_I64;
            dest.m_I64 = op1.m_I64;
            sprintf(comment, "r%d = r%d <%d>", istr.destination, istr.operand1, op1.m_I64);
            break;
        }
        case MoveFloat: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            dest.m_type = R_F32;
            dest.m_F32 = op1.m_F32;
            sprintf(comment, "r%d = r%d <%f>", istr.destination, istr.operand1, op1.m_I64);
            break;
        }
        case MovePointer: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            dest.m_type = R_POINTER;
            dest.m_PTR = op1.m_PTR;
            sprintf(comment, "r%d = r%d <%s>", istr.destination, istr.operand1, this->m_mainWindow->resolveHash(op1.m_PTR.get()).c_str());
            break;
        }
        case CastInteger: {
            sprintf(varying, "r%d, r%d", istr.destination, istr.operand1);
            dest.m_type = R_I32;
            dest.m_I32 = (i32)op1.m_F32;
            sprintf(comment, "r%d = int(r%d) -> <%f> => <%d>", istr.destination, istr.operand1, op1.m_F32, dest.m_I32);
            break;
        }
        case CastFloat: {
            sprintf(varying, "r%d, r%d", istr.destination, istr.operand1);
            dest.m_type = R_F32;
            dest.m_F32 = (f32)op1.m_I32;
            sprintf(comment, "r%d = float(r%d) -> <%d> => <%f>", istr.destination, istr.operand1, op1.m_I32, dest.m_F32);
            break;
        }
        case Call: {
            sprintf(varying, "r%d, r%d, %d", istr.destination, istr.operand1, istr.operand2);
            std::string comment_str = "r" + std::to_string(istr.destination) + " = " + this->m_mainWindow->resolveHash(op1.m_PTR.get()) + "(";
            for (u64 i = 0; i < istr.operand2; ++i) {
                if (i != 0) {
                    comment_str += ", ";
                }
                comment_str += stackFrame.toString(49 + i, this->m_mainWindow->resolveHash(stackFrame[i + 49].m_SID));
            }
            dest.m_type = R_HASH;
            dest.isReturn = true;
            comment_str += ")";
            sprintf(comment, "%s", comment_str.c_str());
            break;
        }
        case CallFf: {
            sprintf(varying, "r%d, r%d, %d", istr.destination, istr.operand1, istr.operand2);
            std::string comment_str = "r" + std::to_string(istr.destination) + " = " + this->m_mainWindow->resolveHash(op1.m_PTR.get()) + "(";
            for (u64 i = 0; i < istr.operand2; ++i) {
                if (i != 0) {
                    comment_str += ", ";
                }
                comment_str += stackFrame.toString(49 + i, this->m_mainWindow->resolveHash(stackFrame[i + 49].m_SID));
            }
            dest.m_type = R_HASH;
            dest.isReturn = true;
            comment_str += ")";
            sprintf(comment, "%s", comment_str.c_str());
            break;
        }
        case IEqual: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_I64 == op2.m_I64;
            sprintf(comment, "r%d = r%d [%d] == r%d [%d] -> <%s>", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_I64, 
                istr.operand2, 
                stackFrame[istr.operand2].m_I64,
                dest.m_BOOL ? "TRUE" : "FALSE"
            );
            break;
        }
        case IGreaterThan: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_I64 > op2.m_I64;
            sprintf(comment, "r%d = r%d [%d] > r%d [%d] -> <%s>", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_I64, 
                istr.operand2, 
                stackFrame[istr.operand2].m_I64,
                dest.m_BOOL ? "TRUE" : "FALSE"
            );
            break;
        }
        case IGreaterThanEqual: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_I64 >= op2.m_I64;
            sprintf(comment, "r%d = r%d [%d] >= r%d [%d] -> <%s>", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_I64, 
                istr.operand2, 
                stackFrame[istr.operand2].m_I64,
                dest.m_BOOL ? "TRUE" : "FALSE"
            );
            break;
        }
        case ILessThan: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_I64 < op2.m_I64;
            sprintf(comment, "r%d = r%d [%d] < r%d [%d] -> <%s>", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_I64, 
                istr.operand2, 
                stackFrame[istr.operand2].m_I64,
                dest.m_BOOL ? "TRUE" : "FALSE"
            );
            break;
        }
        case ILessThanEqual: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_I64 <= op2.m_I64;
            sprintf(comment, "r%d = r%d [%d] <= r%d [%d] -> <%s>", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_I64, 
                istr.operand2, 
                stackFrame[istr.operand2].m_I64,
                dest.m_BOOL ? "TRUE" : "FALSE"
            );
            break;
        }
        case FEqual: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_F32 == op2.m_F32;
            sprintf(comment, "r%d = r%d [%f] == r%d [%f] -> <%s>", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_F32, 
                istr.operand2, 
                stackFrame[istr.operand2].m_F32,
                dest.m_BOOL ? "TRUE" : "FALSE"
            );
            break;
        }
        case FGreaterThanEqual: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_F32 >= op2.m_F32;
            sprintf(comment, "r%d = r%d [%f] >= r%d [%f] -> <%s>", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_F32, 
                istr.operand2, 
                stackFrame[istr.operand2].m_F32,
                dest.m_BOOL ? "TRUE" : "FALSE"
            );
            break;
        }
        case FLessThan: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_F32 < op2.m_F32;
            sprintf(comment, "r%d = r%d [%f] < r%d [%f] -> <%s>", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_F32, 
                istr.operand2, 
                stackFrame[istr.operand2].m_F32,
                dest.m_BOOL ? "TRUE" : "FALSE"
            );
            break;
        }
        case FLessThanEqual: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_F32 <= op2.m_F32;
            sprintf(comment, "r%d = r%d [%f] <= r%d [%f] -> <%s>", 
                istr.destination, 
                istr.operand1, 
                stackFrame[istr.operand1].m_F32, 
                istr.operand2, 
                stackFrame[istr.operand2].m_F32,
                dest.m_BOOL ? "TRUE" : "FALSE"
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
            sprintf(comment, "r%d = r%d [%d] % r%d [%d] -> <%d>", 
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
            sprintf(comment, "r%d = r%d [%f] % r%d [%f] -> <%f>", 
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
            sprintf(comment, "r%d = ABS(r%d) [%d] -> <%d>", 
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
            sprintf(comment, "r%d = ABS(r%d) [%f] -> <%f>", 
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
            u32 label_name = stackFrame.getLabelIndex(target);
            line.m_label = target;
            break;
        }
        case BranchIf: {
            u32 target = istr.destination | (istr.operand2 << 8);
            sprintf(varying, "r%d, %04X", istr.operand1, target);
            u32 label_name = stackFrame.getLabelIndex(target);
            sprintf(comment, "IF r%d [%s] ", istr.operand1, op1_str);
            line.m_label = target;
            break;
        }
        case BranchIfNot: {
            u32 target = istr.destination | (istr.operand2 << 8);
            sprintf(varying, "r%d, %04X", istr.operand1, target);
            u32 label_name = stackFrame.getLabelIndex(target);
            sprintf(comment, "IF NOT r%d [%s] ", istr.operand1, op1_str);
            line.m_label = target;
            break;
        }
        case OpLogNot: {
            sprintf(varying, "r%d, r%d", istr.destination, istr.operand1);
            dest.m_type = R_BOOL;
            dest.m_BOOL = !op1.m_BOOL;
            sprintf(comment, "r%d = !r%d -> <%s>", istr.destination, istr.operand1, dest.m_BOOL ? "TRUE" : "FALSE");
            break;
        }
        case OpBitAnd: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = op1.m_type;
            dest.m_U64 = op1.m_U64 & op2.m_U64;
            sprintf(comment, "r%d = r%d [%llu] & r%d [%llu] -> <%llu>", istr.destination, istr.operand1, op1.m_U64, istr.operand2, op2.m_U64, dest.m_U64);
            break;
        }
        case OpBitNot: {
            sprintf(varying, "r%d, r%d", istr.destination, istr.operand1);
            dest.m_type = op1.m_type;
            dest.m_U64 = ~op1.m_U64;
            sprintf(comment, "r%d = ~r%d [%llu] -> <%llu>", istr.destination, istr.operand1, op1.m_U64, dest.m_U64);
            break;
        }
        case OpBitOr: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = op1.m_type;
            dest.m_U64 = op1.m_U64 | op2.m_U64;
            sprintf(comment, "r%d = r%d [%llu] | r%d [%llu] -> <%llu>", istr.destination, istr.operand1, op1.m_U64, istr.operand2, op2.m_U64, dest.m_U64);
            break;
        }
        case OpBitXor: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = op1.m_type;
            dest.m_U64 = op1.m_U64 ^ op2.m_U64;
            sprintf(comment, "r%d = r%d [%llu] ^ r%d [%llu] -> <%llu>", istr.destination, istr.operand1, op1.m_U64, istr.operand2, op2.m_U64, dest.m_U64);
            break;
        }
        case OpBitNor: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = op1.m_type;
            dest.m_U64 = ~(op1.m_U64 | op2.m_U64);
            sprintf(comment, "r%d = ~(r%d [%llu] | r%d [%llu]) -> <%llu>", istr.destination, istr.operand1, op1.m_U64, istr.operand2, op2.m_U64, dest.m_U64);
            break;
        }
        case OpLogAnd: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_BOOL && op2.m_BOOL;
            sprintf(comment, "r%d = r%d [%d] && r%d [%d]", istr.destination, istr.operand1, op1.m_BOOL, istr.operand2, op2.m_BOOL);
            break;
        } 
        case OpLogOr: {
            sprintf(varying, "r%d, r%d, r%d", istr.destination, istr.operand1, istr.operand2);
            dest.m_type = R_BOOL;
            dest.m_BOOL = op1.m_BOOL || op2.m_BOOL;
            sprintf(comment, "r%d = r%d [%d] || r%d [%d]", istr.destination, istr.operand1, op1.m_BOOL, istr.operand2, op2.m_BOOL);
            break;
        }
        case INeg: {
            sprintf(varying, "r%d, r%d", istr.destination, istr.operand1);
            dest.m_type = R_I64;
            dest.m_I64 = -op1.m_I64;
            sprintf(comment, "r%d = -r%d [%d] -> <%d>", istr.destination, istr.operand1, op1.m_I64, dest.m_I64);
            break;
        }
        case FNeg: {
            sprintf(varying, "r%d, r%d", istr.destination, istr.operand1);
            dest.m_type = R_F32;
            dest.m_F32 = -op1.m_F32;
            sprintf(comment, "r%d = -r%d [%f] -> <%f>", istr.destination, istr.operand1, op1.m_F32, dest.m_F32);
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
            sprintf(comment, "r%d = r%d [%d] + %d -> <%d>", istr.destination, istr.operand1, op1.m_I64, istr.operand2, dest.m_I64);
        }
        case ISubImm: {
            sprintf(varying, "r%d, r%d, %d", istr.destination, istr.operand1, istr.operand2);
            i64 value = op1.m_I64 - istr.operand2;
            dest.m_type = R_I64;
            dest.m_I64 = value;
            sprintf(comment, "r%d = r%d [%d] - %d -> <%d>", istr.destination, istr.operand1, op1.m_I64, istr.operand2, dest.m_I64);
        }
        case IMulImm: {
            sprintf(varying, "r%d, r%d, %d", istr.destination, istr.operand1, istr.operand2);
            i64 value = op1.m_I64 * istr.operand2;
            dest.m_type = R_I64;
            dest.m_I64 = value;
            sprintf(comment, "r%d = r%d [%d] * %d -> <%d>", istr.destination, istr.operand1, op1.m_I64, istr.operand2, dest.m_I64);
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
            sprintf(comment, "r%d = r%d [%d] / %d -> <%d>", istr.destination, istr.operand1, op1.m_I64, istr.operand2, dest.m_I64);
            break;
        }
        case LoadStaticI32Imm: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            i32 value = *reinterpret_cast<i32*>(stackFrame.m_symbolTablePtr + istr.operand1); 
            dest.m_type = RegisterValueType::R_I32;
            dest.m_I32 = value;
            table_entry.m_type = SymbolTableEntryType::INT;
            table_entry.m_i64 = value;
            sprintf(comment, "r%d = ST[%d] -> <%d>", istr.destination, istr.operand1, value);
            break;
        }
        case LoadStaticFloatImm: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            f32 value = *reinterpret_cast<f32*>(stackFrame.m_symbolTablePtr + istr.operand1); 
            dest.m_type = RegisterValueType::R_F32;
            dest.m_F32 = value;
            table_entry.m_type = SymbolTableEntryType::FLOAT;
            table_entry.m_f32 = value;
            sprintf(comment, "r%d = ST[%d] -> <%.2f>", istr.destination, istr.operand1, value);
            break;
        }
        case LoadStaticPointerImm: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            uintptr_t value = reinterpret_cast<uintptr_t*>(stackFrame.m_symbolTablePtr)[istr.operand1]; 
            dest.m_type = RegisterValueType::R_POINTER;
            dest.m_PTR = {value, 0};
            table_entry.m_type = SymbolTableEntryType::POINTER;
            table_entry.m_pointer = value;
            sprintf(comment, "r%d = ST[%d] -> <0x%X>", istr.destination, istr.operand1, value);
            break;
        }
        case LoadStaticI64Imm: {
            sprintf(varying, "r%d, %d", istr.destination, istr.operand1);
            i64 value = reinterpret_cast<i64*>(stackFrame.m_symbolTablePtr)[istr.operand1]; 
            dest.m_type = RegisterValueType::R_I64;
            dest.m_I64 = value;
            table_entry.m_type = SymbolTableEntryType::INT;
            table_entry.m_i64 = value;
            sprintf(comment, "r%d = ST[%d] -> <%d>", istr.destination, istr.operand1, value);
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
                sprintf(comment, "r%d = ST[%d] -> <%s>", istr.destination, istr.operand1, stackFrame.toString(istr.destination, this->m_mainWindow->resolveHash(value)).c_str());
            } else if (value % 8 == 0 && value < this->m_currentFile.m_bytes.size()) {
                dest.m_type = RegisterValueType::R_POINTER;
                dest.m_PTR = {value, 0};
                table_entry.m_type = SymbolTableEntryType::POINTER;
                table_entry.m_pointer = value;
                sprintf(comment, "r%d = ST[%d] -> <%s>", istr.destination, istr.operand1, this->m_currentFile.m_bytes[value]);
            }
            break;
        }
        case Move: {
            sprintf(varying, "r%d, r%d", istr.destination, istr.operand1);
            dest.m_type = op1.m_type;
            dest.m_PTR = op1.m_PTR;
            dest.isReturn = op1.isReturn;
            sprintf(comment, "r%d = r%d [%s]", istr.destination, istr.operand1, op1_str);
            break;
        }
    }
    if (table_entry.m_type != NONE) {
        stackFrame.m_symbolTable.push_back(table_entry);
    }
    line.m_text = std::string(disassembly_text);
    line.m_comment = std::string(comment);
}

void ListingViewController::insertFunctionDisassemblyText(const FunctionDisassembly &functionDisassembly) {
    auto labels = functionDisassembly.m_stackFrame.m_labels;
    for (const auto &line : functionDisassembly.m_lines) {
        auto label_loc = std::find(labels.begin(), labels.end(), line.m_location);
        if (label_loc != labels.end()) {
            this->insertSpan("L_" + std::to_string(std::distance(labels.begin(), label_loc)) + ":\n", MainWindow::COMMENT_COLOR, 14, 10);
        }
        this->insertSpan(line.m_text, MainWindow::OPCODE_COLOR, 12, 14);
        std::string comment = std::string(70 - line.m_text.length(), ' ') + line.m_comment;
        this->insertSpan(comment, MainWindow::STRING_COLOR, 12);
        if (line.m_label != -1) {
            u32 target = std::distance(labels.begin(), std::find(labels.begin(), labels.end(), line.m_label));
            this->insertSpan("=> L_" + std::to_string(target), MainWindow::COMMENT_COLOR, 12);
        }
        this->insertSpan("\n\n");
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