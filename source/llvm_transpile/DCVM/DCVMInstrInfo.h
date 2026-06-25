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

   bool analyzeBranch(MachineBasicBlock &MBB, MachineBasicBlock *&TBB,
                      MachineBasicBlock *&FBB,
                      SmallVectorImpl<MachineOperand> &Cond,
                      bool AllowModify) const override;

   unsigned insertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB,
                         MachineBasicBlock *FBB, ArrayRef<MachineOperand> Cond,
                         const DebugLoc &DL,
                         int *BytesAdded = nullptr) const override;

   unsigned removeBranch(MachineBasicBlock &MBB,
                        int *BytesRemoved = nullptr) const override;

   bool reverseBranchCondition(
       SmallVectorImpl<MachineOperand> &Cond) const override;
};

} // namespace llvm
