#include "base.h"

#include "disassembler.h"

class ListingViewController {
public:
    ListingViewController();
    void setDisassembler(Disassembler *disassembler);
    void setScriptById(stringid_64 id);
    void createListingView();

private:
    Disassembler *m_disassembler;
    StateScript *m_currentScript;
    QTextEdit *m_textEdit;
};