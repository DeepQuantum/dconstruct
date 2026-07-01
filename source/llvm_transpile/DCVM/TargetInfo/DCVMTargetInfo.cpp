#include "TargetInfo/DCVMTargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Compiler.h"

using namespace llvm;

Target& llvm::getTheDCVMTarget() {
    static Target TheDCVMTarget;
    return TheDCVMTarget;
}

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void LLVMInitializeDCVMTargetInfo() {
    RegisterTarget<Triple::UnknownArch, /*HasJIT=*/false> X(getTheDCVMTarget(), "dcvm", "DCVM", "DCVM");
}
