#include "DCVMSubtarget.h"
#include "DCVMSelectionDAGInfo.h"
#include "llvm/ADT/StringRef.h"

using namespace llvm;

#define DEBUG_TYPE "dcvm-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "DCVMGenSubtargetInfo.inc"

void DCVMSubtarget::anchor() {}

DCVMSubtarget& DCVMSubtarget::initializeSubtargetDependencies(StringRef CPU, StringRef TuneCPU, StringRef FS) {
    if (TuneCPU.empty())
        TuneCPU = CPU;
    ParseSubtargetFeatures(CPU, TuneCPU, FS);
    return *this;
}

DCVMSubtarget::DCVMSubtarget(const Triple& TT, StringRef CPU, StringRef TuneCPU, StringRef FS, const TargetMachine& TM)
    : DCVMGenSubtargetInfo(TT, CPU, TuneCPU, FS), InstrInfo(*this), TLInfo(TM, *this), FrameLowering(*this) {
    initializeSubtargetDependencies(CPU, TuneCPU, FS);
    TSInfo = std::make_unique<DCVMSelectionDAGInfo>();
}
