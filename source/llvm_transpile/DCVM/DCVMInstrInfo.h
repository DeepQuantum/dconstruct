#pragma once


#include "DCVMRegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "DCVMGenInstrInfo.inc"

namespace llvm {

class DCVMSubtarget;

class DCVMInstrInfo : public DCVMGenInstrInfo {
    DCVMRegisterInfo RI;

public:
   explicit DCVMInstrInfo(const DCVMSubtarget &STI);

   const DCVMRegisterInfo& getRegisterInfo() const { return RI; }

   void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
                    const DebugLoc &DL, Register DestReg, Register SrcReg,
                    bool KillSrc, bool RenamableDest = false,
                    bool RenamableSrc = false) const override;
};

} // namespace llvm
