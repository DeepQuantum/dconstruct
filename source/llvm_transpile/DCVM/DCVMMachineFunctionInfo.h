#pragma once

#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {

class DCVMMachineFunctionInfo : public MachineFunctionInfo {
    virtual void anchor();

public:
    DCVMMachineFunctionInfo() = default;
    DCVMMachineFunctionInfo(const Function &F, const TargetSubtargetInfo *STI) {}

    MachineFunctionInfo *
    clone(BumpPtrAllocator &Allocator, MachineFunction &DestMF,
          const DenseMap<MachineBasicBlock *, MachineBasicBlock *> &Src2DstMBB)
        const override;
};

} // namespace llvm
