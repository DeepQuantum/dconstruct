#include "DCVMRegisterInfo.h"
#include "DCVMFrameLowering.h"
#include "MCTargetDesc/DCVMMCTargetDesc.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"

#define GET_REGINFO_TARGET_DESC
#include "DCVMGenRegisterInfo.inc"

using namespace llvm;

DCVMRegisterInfo::DCVMRegisterInfo() : DCVMGenRegisterInfo(0) {}

const MCPhysReg* DCVMRegisterInfo::getCalleeSavedRegs(const MachineFunction* MF) const {
    static const MCPhysReg CalleeSavedRegs[] = {0};
    return CalleeSavedRegs;
}

BitVector DCVMRegisterInfo::getReservedRegs(const MachineFunction& MF) const {
    BitVector Reserved(getNumRegs());
    return Reserved;
}

bool DCVMRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj, unsigned FIOperandNum, RegScavenger* RS) const {
    return false;
}

Register DCVMRegisterInfo::getFrameRegister(const MachineFunction& MF) const {
    return Register();
}
