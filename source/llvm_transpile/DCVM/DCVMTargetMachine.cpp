#include "DCVMTargetMachine.h"
#include "DCVM.h"
#include "DCVMMachineFunctionInfo.h"
#include "DCVMTargetObjectFile.h"
#include "TargetInfo/DCVMTargetInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Compiler.h"
#include <optional>

using namespace llvm;

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void LLVMInitializeDCVMTarget() {
    RegisterTargetMachine<DCVMTargetMachine> X(getTheDCVMTarget());

    PassRegistry &PR = *PassRegistry::getPassRegistry();
    initializeDCVMDAGToDAGISelLegacyPass(PR);
}

static Reloc::Model getEffectiveRelocModel(std::optional<Reloc::Model> RM) {
    return RM.value_or(Reloc::Static);
}

static CodeModel::Model
getEffectiveCodeModel(std::optional<CodeModel::Model> CM) {
    return CM.value_or(CodeModel::Small);
}

DCVMTargetMachine::DCVMTargetMachine(const Target &T, const Triple &TT,
                                     StringRef CPU, StringRef FS,
                                     const TargetOptions &Options,
                                     std::optional<Reloc::Model> RM,
                                     std::optional<CodeModel::Model> CM,
                                     CodeGenOptLevel OL, bool JIT)
    : CodeGenTargetMachineImpl(T, TT.computeDataLayout(), TT, CPU, FS, Options,
                               getEffectiveRelocModel(RM),
                               getEffectiveCodeModel(CM), OL),
      TLOF(std::make_unique<DCVMELFTargetObjectFile>()) {
    initAsmInfo();
}

DCVMTargetMachine::~DCVMTargetMachine() = default;

const DCVMSubtarget *
DCVMTargetMachine::getSubtargetImpl(const Function &F) const {
    auto &I = SubtargetMap[TargetCPU + TargetFS];
    if (!I)
        I = std::make_unique<DCVMSubtarget>(getTargetTriple(), TargetCPU,
                                            TargetCPU, TargetFS, *this);
    return I.get();
}

MachineFunctionInfo *DCVMTargetMachine::createMachineFunctionInfo(
    BumpPtrAllocator &Allocator, const Function &F,
    const TargetSubtargetInfo *STI) const {
    return DCVMMachineFunctionInfo::create<DCVMMachineFunctionInfo>(Allocator, F,
                                                                    STI);
}

namespace {
class DCVMPassConfig : public TargetPassConfig {
public:
    DCVMPassConfig(DCVMTargetMachine &TM, PassManagerBase &PM)
        : TargetPassConfig(TM, PM) {}

    DCVMTargetMachine &getDCVMTargetMachine() const {
        return getTM<DCVMTargetMachine>();
    }

    bool addInstSelector() override;
};
} // namespace

TargetPassConfig *DCVMTargetMachine::createPassConfig(PassManagerBase &PM) {
    return new DCVMPassConfig(*this, PM);
}

bool DCVMPassConfig::addInstSelector() {
    addPass(createDCVMISelDag(getDCVMTargetMachine()));
    return false;
}
