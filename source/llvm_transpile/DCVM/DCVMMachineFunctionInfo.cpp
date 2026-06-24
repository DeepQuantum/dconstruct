#include "DCVMMachineFunctionInfo.h"

using namespace llvm;

void DCVMMachineFunctionInfo::anchor() {}

MachineFunctionInfo *DCVMMachineFunctionInfo::clone(
    BumpPtrAllocator &Allocator, MachineFunction &DestMF,
    const DenseMap<MachineBasicBlock *, MachineBasicBlock *> &Src2DstMBB)
    const {
    return DestMF.cloneInfo<DCVMMachineFunctionInfo>(*this);
}
