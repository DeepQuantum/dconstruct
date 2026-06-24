#pragma once

#include "DCVMFrameLowering.h"
#include "DCVMISelLowering.h"
#include "DCVMInstrInfo.h"
#include "MCTargetDesc/DCVMMCTargetDesc.h"
#include "llvm/CodeGen/SelectionDAGTargetInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/TargetParser/Triple.h"
#include <memory>

#define GET_SUBTARGETINFO_HEADER
#include "DCVMGenSubtargetInfo.inc"

namespace llvm {

class StringRef;
class TargetMachine;

class DCVMSubtarget : public DCVMGenSubtargetInfo {
    virtual void anchor();

    DCVMInstrInfo InstrInfo;
    DCVMTargetLowering TLInfo;
    std::unique_ptr<const SelectionDAGTargetInfo> TSInfo;
    DCVMFrameLowering FrameLowering;

public:
    DCVMSubtarget(const Triple &TT, StringRef CPU, StringRef TuneCPU,
                  StringRef FS, const TargetMachine &TM);

    const DCVMInstrInfo *getInstrInfo() const override { return &InstrInfo; }

    const TargetFrameLowering *getFrameLowering() const override {
        return &FrameLowering;
    }

    const DCVMRegisterInfo *getRegisterInfo() const override {
        return &InstrInfo.getRegisterInfo();
    }

    const DCVMTargetLowering *getTargetLowering() const override {
        return &TLInfo;
    }

    const SelectionDAGTargetInfo *getSelectionDAGInfo() const override {
        return TSInfo.get();
    }

    void ParseSubtargetFeatures(StringRef CPU, StringRef TuneCPU, StringRef FS);
    DCVMSubtarget &initializeSubtargetDependencies(StringRef CPU,
                                                   StringRef TuneCPU,
                                                   StringRef FS);
};

} // namespace llvm
