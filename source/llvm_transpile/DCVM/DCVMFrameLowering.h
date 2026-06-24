#pragma once

#include "llvm/CodeGen/TargetFrameLowering.h"

namespace llvm {

class DCVMSubtarget;

class DCVMFrameLowering : public TargetFrameLowering {
public:
    explicit DCVMFrameLowering(const DCVMSubtarget &STI);

    void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
    void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

protected:
    bool hasFPImpl(const MachineFunction &MF) const override;
};

} // namespace llvm
