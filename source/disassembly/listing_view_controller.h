#include "base.h"

#include "disassembler.h"
#include <QtWidgets/QTextEdit>

class ListingViewController {
public:
    ListingViewController(Disassembler *disassebler, QTextEdit *textEdit);
    void setScriptById(stringid_64 id);
    void createListingView();

private:
    Disassembler *m_disassembler;
    StateScript *m_currentScript;
    QTextEdit *m_textEdit;
};