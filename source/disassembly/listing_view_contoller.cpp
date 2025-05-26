#include "listing_view_controller.h"
#include "../mainwindow.h"
#include <stdexcept>
#include "instructions.h"
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
            case SymbolType::B8:
                line = "BOOL: " + MainWindow::offsetToString(this->getOffset(symbol.b8_ptr)) + " <" + (*symbol.b8_ptr ? "true" : "false") + ">";
                this->insertSpan(line + "\n", MainWindow::OPCODE_COLOR, 10, 2);
                break;
            case SymbolType::I32:
                line = "Integer: " + MainWindow::offsetToString(this->getOffset(symbol.i32_ptr)) + " <" + std::to_string(*symbol.i32_ptr) + ">";
                this->insertSpan(line + "\n", MainWindow::OPCODE_COLOR, 10, 2);
                break;
            case SymbolType::F32:
                line = "Float: " + MainWindow::offsetToString(this->getOffset(symbol.f32_ptr)) + " <" + std::to_string(*symbol.f32_ptr) + ">";
                this->insertSpan(line + "\n", MainWindow::OPCODE_COLOR, 10, 2);
                break;
            case SymbolType::HASH:
                line = "Hash: " + MainWindow::intToSIDString(*reinterpret_cast<uint64_t*>(symbol.hash_ptr));
                this->insertSpan(line + "\n", MainWindow::OPCODE_COLOR, 10, 2);
                break;
            case SymbolType::SS:
                line = "State Script ID: " + MainWindow::intToSIDString(reinterpret_cast<StateScript*>(symbol.ss_ptr)->m_stateScriptId);
                this->insertSpan(line + "\n", MainWindow::OPCODE_COLOR, 10, 2);
                break;
            case SymbolType::LAMBDA:
                this->insertComment("BEGIN LAMBDA ", 20);
                this->insertHash(this->m_mainWindow->resolveHash(symbol.id), true);
                this->insertSpan(" AT ", MainWindow::COMMENT_COLOR, 14);
                this->insertSpan("[" + MainWindow::offsetToString(this->getOffset(symbol.lambda_ptr)) + "]\n", MainWindow::OPCODE_COLOR, 14);
                this->insertFunctionDisassembly(symbol.lambda_ptr);
                break;
            default:
                this->insertSpan("UNKNOWN SYMBOL TYPE: ", MainWindow::COMMENT_COLOR, 14, 20);
                this->insertSpan(MainWindow::offsetToString(this->getOffset(reinterpret_cast<void*>(symbol.i32_ptr_raw))));
        }
        
    }
}


u32 ListingViewController::getOffset(const void *symbol) {
    return reinterpret_cast<uintptr_t>(symbol) - reinterpret_cast<uintptr_t>(this->m_currentFile.m_dcheader);
}

void ListingViewController::insertSymbolTable(u64 *symbolPtr) const noexcept {

}

void ListingViewController::insertFunctionDisassembly(ScriptLambda *lambda) {
    this->insertSpan("INSTRUCTION POINTER: " + MainWindow::offsetToString(this->getOffset(lambda->m_pOpcode)) + "\n", MainWindow::COMMENT_COLOR, 14, 20);
    this->insertSpan("SYMBOL TABLE: " + MainWindow::offsetToString(this->getOffset(lambda->m_pSymbols)) + "\n", MainWindow::COMMENT_COLOR, 14, 20);

    this->insertSymbolTable(lambda->m_pSymbols);
    

    std::unordered_map<u64, std::string> labels;

    u64 instructionCount = reinterpret_cast<Instruction*>(lambda->m_pSymbols) - reinterpret_cast<Instruction*>(lambda->m_pOpcode);
    std::vector<std::pair<u8, std::string>> instructionLines(instructionCount);
    Instruction *instructionPtr = reinterpret_cast<Instruction*>(lambda->m_pOpcode);
    char ss[256];
    for (u64 i = 0; i < instructionCount; ++i) {
        sprintf(ss, "%04X   0x%06X   %02X %02X %02X %02X       %-20s%s", 
            i, 
            this->getOffset(instructionPtr + i), 
            instructionPtr[i].opcode, 
            instructionPtr[i].destination, 
            instructionPtr[i].operand1, 
            instructionPtr[i].operand2,
            instructionPtr[i].opcodeToString().c_str(),
            instructionPtr[i].getRegisterString().c_str()
        );
        if (instructionPtr[i].opcode == Opcode::Branch || instructionPtr[i].opcode == Opcode::BranchIf || instructionPtr[i].opcode == Opcode::BranchIfNot) {
            u64 targetOffset = this->getOffset(instructionPtr + instructionPtr[i].operand1);
            labels.emplace(i, "LABEL_" + std::to_string(i) + ": ");
        }
        instructionLines[i] = {0, ss};
    }
    for (const auto &[idx, name] : labels) {
        instructionLines.insert(instructionLines.begin() + idx, {idx, name});
    }
    for (const auto &line : instructionLines) {
        if (line.first != 0) {
            this->insertSpan(line.second + "\n", MainWindow::COMMENT_COLOR, 12, 12);
        } else {
            this->insertSpan(line.second + "\n", MainWindow::OPCODE_COLOR, 12, 12);
        }
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