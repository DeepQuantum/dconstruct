    #pragma once

#include "llvm/IR/DataLayout.h"
#include <llvm/Analysis/TargetTransformInfo.h>
#include <llvm/CodeGen/CodeGenTargetMachineImpl.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>

#include "DCVMSubtarget.h"

namespace llvm {

class Function;
class MachineFunctionInfo;
class TargetSubtargetInfo;

class DCVMTargetMachine : public CodeGenTargetMachineImpl {

    std::unique_ptr<TargetLoweringObjectFile> TLOF;
    mutable StringMap<std::unique_ptr<DCVMSubtarget>> SubtargetMap;

public:
    DCVMTargetMachine(
        const Target &T,
        const Triple &TT,
        StringRef CPU,
        StringRef FS,
        const TargetOptions &Options,
        std::optional<Reloc::Model> RM,
        std::optional<CodeModel::Model> CM,
        CodeGenOptLevel OL,
        bool JIT
    );

    ~DCVMTargetMachine() override;

    const DCVMSubtarget *getSubtargetImpl(const Function &F) const override;
    TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

    MachineFunctionInfo *
    createMachineFunctionInfo(BumpPtrAllocator &Allocator, const Function &F,
                              const TargetSubtargetInfo *STI) const override;

    TargetLoweringObjectFile *getObjFileLowering() const override {
        return TLOF.get();
    }
};
} // namespace llvm
