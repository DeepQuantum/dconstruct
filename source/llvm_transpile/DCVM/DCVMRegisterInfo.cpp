#include "DCVMRegisterInfo.h"
#include "DCVMFrameLowering.h"
#include "MCTargetDesc/DCVMMCTargetDesc.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"

#define GET_REGINFO_TARGET_DESC
#include "DCVMGenRegisterInfo.inc"

using namespace llvm;

DCVMRegisterInfo::DCVMRegisterInfo() : DCVMGenRegisterInfo(0) {}

const MCPhysReg* DCVMRegisterInfo::getCalleeSavedRegs(const MachineFunction* MF) const {
    return CSR_DCVM_SaveList;
}

BitVector DCVMRegisterInfo::getReservedRegs(const MachineFunction& MF) const {
    // General evaluation lives exclusively in the GPR class (r0-r48); the
    // argument window r49-r65 only ever holds call arguments. Calls preserve
    // every register except r0, so an argument value already in place may be
    // reused by a later call.
    return BitVector(getNumRegs());
}

bool DCVMRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj, unsigned FIOperandNum, RegScavenger* RS) const {
    report_fatal_error(
        "DCVM frame index elimination requested in " + II->getParent()->getParent()->getName() +
        ": the target has no stack, so no pass may create stack objects"
    );
}

Register DCVMRegisterInfo::getFrameRegister(const MachineFunction& MF) const {
    return Register();
}
