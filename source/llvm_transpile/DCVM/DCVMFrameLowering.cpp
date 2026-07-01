#include "DCVMFrameLowering.h"
#include "DCVMSubtarget.h"

using namespace llvm;

DCVMFrameLowering::DCVMFrameLowering(const DCVMSubtarget& STI) : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, Align(8), 0) {}

void DCVMFrameLowering::emitPrologue(MachineFunction& MF, MachineBasicBlock& MBB) const {}

void DCVMFrameLowering::emitEpilogue(MachineFunction& MF, MachineBasicBlock& MBB) const {}

bool DCVMFrameLowering::hasFPImpl(const MachineFunction& MF) const {
    return false;
}
