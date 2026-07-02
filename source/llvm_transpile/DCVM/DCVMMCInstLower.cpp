#include "DCVMMCInstLower.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

MCOperand DCVMMCInstLower::LowerOperand(const MachineOperand& MO) const {
    switch (MO.getType()) {
        case MachineOperand::MO_Register:
            return MCOperand::createReg(MO.getReg());
        case MachineOperand::MO_Immediate:
            return MCOperand::createImm(MO.getImm());
        case MachineOperand::MO_MachineBasicBlock:
            return MCOperand::createExpr(MCSymbolRefExpr::create(MO.getMBB()->getSymbol(), Ctx));
        default:
            llvm_unreachable("unsupported operand type in DCVMMCInstLower");
    }
}

void DCVMMCInstLower::Lower(const MachineInstr* MI, MCInst& OutMI) const {
    OutMI.setOpcode(MI->getOpcode());
    for (const MachineOperand& MO : MI->operands()) {
        if (MO.isRegMask() || (MO.isReg() && MO.isImplicit()))
            continue;
        OutMI.addOperand(LowerOperand(MO));
    }
}
