#include "DCVMMCTargetDesc.h"
#include "DCVMInstPrinter.h"
#include "DCVMMCAsmInfo.h"
#include "TargetInfo/DCVMTargetInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Compiler.h"

using namespace llvm;

#define GET_INSTRINFO_MC_DESC
#define ENABLE_INSTR_PREDICATE_VERIFIER
#include "DCVMGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "DCVMGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "DCVMGenRegisterInfo.inc"

static MCAsmInfo *createDCVMMCAsmInfo(const MCRegisterInfo &MRI,
                                      const Triple &TT,
                                      const MCTargetOptions &Options) {
    return new DCVMELFMCAsmInfo(TT);
}

static MCInstrInfo *createDCVMMCInstrInfo() {
    MCInstrInfo *X = new MCInstrInfo();
    InitDCVMMCInstrInfo(X);
    return X;
}

static MCRegisterInfo *createDCVMMCRegisterInfo(const Triple &TT) {
    MCRegisterInfo *X = new MCRegisterInfo();
    InitDCVMMCRegisterInfo(X, 0);
    return X;
}

static MCSubtargetInfo *
createDCVMMCSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS) {
    return createDCVMMCSubtargetInfoImpl(TT, CPU, /*TuneCPU=*/CPU, FS);
}

static MCInstPrinter *createDCVMMCInstPrinter(const Triple &T,
                                              unsigned SyntaxVariant,
                                              const MCAsmInfo &MAI,
                                              const MCInstrInfo &MII,
                                              const MCRegisterInfo &MRI) {
    return new DCVMInstPrinter(MAI, MII, MRI);
}

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void LLVMInitializeDCVMTargetMC() {
    Target &T = getTheDCVMTarget();

    RegisterMCAsmInfoFn X(T, createDCVMMCAsmInfo);
    TargetRegistry::RegisterMCInstrInfo(T, createDCVMMCInstrInfo);
    TargetRegistry::RegisterMCRegInfo(T, createDCVMMCRegisterInfo);
    TargetRegistry::RegisterMCSubtargetInfo(T, createDCVMMCSubtargetInfo);
    TargetRegistry::RegisterMCInstPrinter(T, createDCVMMCInstPrinter);
    TargetRegistry::RegisterMCCodeEmitter(T, createDCVMMCCodeEmitter);
}
