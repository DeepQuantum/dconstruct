#include "listing_view_controller.h"
#include "mainwindow.h"
#include <stdexcept>


void ListingViewController::setDisassembler(Disassembler *disassembler) {
    this->m_disassembler = disassembler;
}

void ListingViewController::setScriptById(stringid_64 id) {
    if (this->m_disassembler == nullptr) {
        throw std::runtime_error("Disassembler is not set");
    }

    for (const auto &script : this->m_disassembler->m_scripts) {
        if (script.m_dcheader->m_pStartOfData->m_scriptId == id) {
            this->m_currentScript = script.m_dcscript;
            return;
        }
    }

    throw std::runtime_error("Script with the given ID not found");
}

void ListingViewController::createListingView() {
    if (this->m_currentScript == nullptr) {
        throw std::runtime_error("Current script is not set");
    }

    m_textEdit.set
}