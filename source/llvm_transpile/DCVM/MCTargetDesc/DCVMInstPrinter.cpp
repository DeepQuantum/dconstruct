#include "DCVMInstPrinter.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRegister.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#include "DCVMGenAsmWriter.inc"

void DCVMInstPrinter::printRegName(raw_ostream &OS, MCRegister Reg) {
    OS << getRegisterName(Reg);
}

void DCVMInstPrinter::printInst(const MCInst *MI, uint64_t Address,
                                StringRef Annot, const MCSubtargetInfo &STI,
                                raw_ostream &O) {
    printInstruction(MI, Address, O);
    printAnnotation(O, Annot);
}

void DCVMInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                   raw_ostream &O) {
    const MCOperand &Op = MI->getOperand(OpNo);
    if (Op.isReg()) {
        printRegName(O, Op.getReg());
    } else if (Op.isImm()) {
        O << Op.getImm();
    } else {
        MAI.printExpr(O, *Op.getExpr());
    }
}
