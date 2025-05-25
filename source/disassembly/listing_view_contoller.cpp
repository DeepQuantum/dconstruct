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

void ListingViewController::insertLine(const std::string &line, QColor color, int fontSize, u8 indent) {
    QTextCursor cursor = this->m_mainWindow->getListingView()->textCursor();
    cursor.movePosition(QTextCursor::End);
    this->m_mainWindow->getListingView()->setTextCursor(cursor);


    QTextCharFormat format;
    format.setForeground(color);
    format.setFontPointSize(fontSize);
    cursor.insertText(QString::fromStdString(std::string(indent, ' ') + line + "\n"), format);
}

void ListingViewController::createListingView() {
    this->insertHeaderLine();
    for (const auto &symbol : this->m_currentFile.m_symbols) {
        std::string line;
        switch (symbol.type) {
            case SymbolType::B8:
                line = "BOOL: " + MainWindow::offsetToString(this->getOffset(symbol.b8_ptr)) + " <" + (*symbol.b8_ptr ? "true" : "false") + ">";
                break;
            case SymbolType::I32:
                line = "Integer: " + MainWindow::offsetToString(this->getOffset(symbol.i32_ptr)) + " <" + std::to_string(*symbol.i32_ptr) + ">";
                break;
            case SymbolType::F32:
                line = "Float: " + MainWindow::offsetToString(this->getOffset(symbol.f32_ptr)) + " <" + std::to_string(*symbol.f32_ptr) + ">";
                break;
            case SymbolType::HASH:
                line = "Hash: " + MainWindow::intToSIDString(*reinterpret_cast<uint64_t*>(symbol.hash_ptr));
                break;
            case SymbolType::SS:
                line = "State Script ID: " + MainWindow::intToSIDString(reinterpret_cast<StateScript*>(symbol.ss_ptr)->m_stateScriptId);
                break;
            case SymbolType::LAMBDA:
                this->insertLine("BEGIN LAMBDA <" + this->m_mainWindow->resolveHash(symbol.id) + "> AT [" + MainWindow::offsetToString(this->getOffset(symbol.lambda_ptr)) + "]", MainWindow::COMMENT_COLOR, 14, 20);
                this->insertFunctionDisassembly(symbol.lambda_ptr);
                break;
            default:
                this->insertLine("UNKNOWN SYMBOL TYPE: " + MainWindow::offsetToString(this->getOffset(reinterpret_cast<void*>(symbol.i32_ptr_raw))), MainWindow::COMMENT_COLOR, 14, 20);
        }
        this->insertLine(line, MainWindow::OPCODE_COLOR, 10, 2);
    }
}


u32 ListingViewController::getOffset(const void *symbol) {
    return reinterpret_cast<uintptr_t>(symbol) - reinterpret_cast<uintptr_t>(this->m_currentFile.m_dcheader);
}

void ListingViewController::insertFunctionDisassembly(ScriptLambda *lambda) {
    this->insertLine("INSTRUCTION POINTER: " + MainWindow::offsetToString(this->getOffset(lambda->m_pOpcode)), MainWindow::COMMENT_COLOR, 14, 20);
    this->insertLine("SYMBOL TABLE: " + MainWindow::offsetToString(this->getOffset(lambda->m_pSymbols)), MainWindow::COMMENT_COLOR, 14, 20);

    std::unordered_map<u64, std::string> labels;

    u64 instructionCount = reinterpret_cast<Instruction*>(lambda->m_pSymbols) - reinterpret_cast<Instruction*>(lambda->m_pOpcode);
    std::vector<std::pair<u8, std::string>> instructionLines(instructionCount);
    Instruction *instructionPtr = reinterpret_cast<Instruction*>(lambda->m_pOpcode);
    char ss[128];
    for (u64 i = 0; i < instructionCount; ++i) {
        sprintf(ss, "%04X   0x%06X   %02X %02X %02X %02X       %s", 
            i, 
            this->getOffset(instructionPtr + i), 
            instructionPtr[i].opcode, 
            instructionPtr[i].destination, 
            instructionPtr[i].operand1, 
            instructionPtr[i].operand2,
            instructionPtr[i].opcodeToString().c_str()
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
            this->insertLine(line.second, MainWindow::COMMENT_COLOR, 12, 12);
        } else {
            this->insertLine(line.second, MainWindow::OPCODE_COLOR, 12, 12);
        }
    }
} 

void ListingViewController::insertHeaderLine() {
    std::string current_script_name = this->m_mainWindow->resolveHash(this->m_currentFile.m_dcscript->m_stateScriptId);
    this->m_mainWindow->getListingView()->clear();
    this->m_mainWindow->getListingView()->setReadOnly(true);
    this->insertLine("DeepQuantum's DC Disassembler ver. " + std::to_string(MainWindow::VersionNumber), MainWindow::STRING_COLOR, 14, 20);
    this->insertLine("Listing for script: " + current_script_name, MainWindow::STRING_COLOR, 14, 20);
    this->insertLine("Script ID: " + MainWindow::intToSIDString(this->m_currentFile.m_dcscript->m_stateScriptId), MainWindow::STRING_COLOR, 14, 20);
    this->insertLine("Filesize: " + std::to_string(this->m_currentFile.m_size) + " bytes", MainWindow::STRING_COLOR, 14, 20);
    this->insertLine("");
    this->insertLine("START OF DISASSEMBLY", MainWindow::COMMENT_COLOR, 14, 20);
    this->insertLine("--------------------------------------------------", MainWindow::COMMENT_COLOR, 14, 20);
}