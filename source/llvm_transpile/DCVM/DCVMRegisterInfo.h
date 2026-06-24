#pragma once

#include "llvm/CodeGen/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "DCVMGenRegisterInfo.inc"

namespace llvm {

class DCVMSubtarget;

struct DCVMRegisterInfo : public DCVMGenRegisterInfo {
    DCVMRegisterInfo();

    const MCPhysReg *getCalleeSavedRegs(const MachineFunction *MF) const override;

    BitVector getReservedRegs(const MachineFunction &MF) const override;

    bool eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj,
                             unsigned FIOperandNum,
                             RegScavenger *RS = nullptr) const override;

    Register getFrameRegister(const MachineFunction &MF) const override;
};

} // namespace llvm
