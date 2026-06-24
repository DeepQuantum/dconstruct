#include "DCVMMCInstLower.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/MC/MCInst.h"

using namespace llvm;

MCOperand DCVMMCInstLower::LowerOperand(const MachineOperand &MO) const {
    return MCOperand();
}

void DCVMMCInstLower::Lower(const MachineInstr *MI, MCInst &OutMI) const {}
