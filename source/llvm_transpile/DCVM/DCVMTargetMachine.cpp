#include "DCVMTargetMachine.h"
#include "DCVM.h"
#include "DCVMMachineFunctionInfo.h"
#include "DCVMTargetObjectFile.h"
#include "DCVMTargetTransformInfo.h"
#include "TargetInfo/DCVMTargetInfo.h"
#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Pass.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Transforms/Utils.h"
#include <optional>

using namespace llvm;

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void LLVMInitializeDCVMTarget() {
    RegisterTargetMachine<DCVMTargetMachine> X(getTheDCVMTarget());

    PassRegistry& PR = *PassRegistry::getPassRegistry();
    initializeDCVMDAGToDAGISelLegacyPass(PR);
    initializeDCVMLowerSwitchPass(PR);
}

static Reloc::Model getEffectiveRelocModel(std::optional<Reloc::Model> RM) {
    return RM.value_or(Reloc::Static);
}

static CodeModel::Model getDCVMCodeModel(std::optional<CodeModel::Model> CM) {
    return CM.value_or(CodeModel::Small);
}

DCVMTargetMachine::DCVMTargetMachine(
    const Target& T, const Triple& TT, StringRef CPU, StringRef FS, const TargetOptions& Options, std::optional<Reloc::Model> RM, std::optional<CodeModel::Model> CM, CodeGenOptLevel OL, bool JIT
)
    : CodeGenTargetMachineImpl(T, "e-p:64:64-i64:64-n64", TT, CPU, FS, Options, getEffectiveRelocModel(RM), getDCVMCodeModel(CM), OL), TLOF(std::make_unique<DCVMELFTargetObjectFile>()) {
    initAsmInfo();
}

DCVMTargetMachine::~DCVMTargetMachine() = default;

const DCVMSubtarget* DCVMTargetMachine::getSubtargetImpl(const Function& F) const {
    auto& I = SubtargetMap[TargetCPU + TargetFS];
    if (!I)
        I = std::make_unique<DCVMSubtarget>(getTargetTriple(), TargetCPU, TargetCPU, TargetFS, *this);
    return I.get();
}

MachineFunctionInfo* DCVMTargetMachine::createMachineFunctionInfo(BumpPtrAllocator& Allocator, const Function& F, const TargetSubtargetInfo* STI) const {
    return DCVMMachineFunctionInfo::create<DCVMMachineFunctionInfo>(Allocator, F, STI);
}

TargetTransformInfo DCVMTargetMachine::getTargetTransformInfo(const Function& F) const {
    return TargetTransformInfo(std::make_unique<DCVMTTIImpl>(F.getDataLayout()));
}

extern "C" void DCVMSetBytecodeSink(TargetMachine* Machine, std::vector<char>* Sink) {
    static_cast<DCVMTargetMachine*>(Machine)->BytecodeSink = Sink;
}

static void lowerSwitchToEqualityChain(SwitchInst* SI) {
    BasicBlock* OrigBB = SI->getParent();
    Function* F = OrigBB->getParent();
    LLVMContext& Ctx = F->getContext();
    Value* Cond = SI->getCondition();
    BasicBlock* DefaultBB = SI->getDefaultDest();

    SmallVector<std::pair<ConstantInt*, BasicBlock*>, 8> Cases;
    for (const auto& Case : SI->cases())
        Cases.emplace_back(Case.getCaseValue(), Case.getCaseSuccessor());

    const size_t NumCases = Cases.size();

    SmallVector<BasicBlock*, 8> CmpBBs;
    for (size_t i = 0; i < NumCases; ++i)
        CmpBBs.push_back(BasicBlock::Create(Ctx, "sw.eq", F));

    MapVector<BasicBlock*, SmallVector<BasicBlock*, 4>> NewPreds;

    for (size_t i = 0; i < NumCases; ++i) {
        IRBuilder<> Builder(CmpBBs[i]);
        Value* Eq = Builder.CreateICmpEQ(Cond, Cases[i].first, "sw.eq.cmp");
        BasicBlock* ElseBB = (i + 1 < NumCases) ? CmpBBs[i + 1] : DefaultBB;
        Builder.CreateCondBr(Eq, Cases[i].second, ElseBB);
        NewPreds[Cases[i].second].push_back(CmpBBs[i]);
        if (i + 1 == NumCases)
            NewPreds[DefaultBB].push_back(CmpBBs[i]);
    }

    BasicBlock* EntryBB = NumCases ? CmpBBs[0] : DefaultBB;
    SI->eraseFromParent();
    IRBuilder<>(OrigBB).CreateBr(EntryBB);

    for (auto& Entry : NewPreds) {
        BasicBlock* Succ = Entry.first;
        for (PHINode& PN : Succ->phis()) {
            const int Idx = PN.getBasicBlockIndex(OrigBB);
            if (Idx < 0)
                continue;
            Value* IncomingV = PN.getIncomingValue(Idx);
            PN.removeIncomingValue(Idx, false);
            for (BasicBlock* Pred : Entry.second)
                PN.addIncoming(IncomingV, Pred);
        }
    }
}

namespace {
    class DCVMLowerSwitch : public FunctionPass {
    public:
        static char ID;
        DCVMLowerSwitch() : FunctionPass(ID) {}

        bool runOnFunction(Function& F) override {
            SmallVector<SwitchInst*, 8> Switches;
            for (BasicBlock& BB : F)
                if (auto* SI = dyn_cast<SwitchInst>(BB.getTerminator()))
                    Switches.push_back(SI);
            for (SwitchInst* SI : Switches)
                lowerSwitchToEqualityChain(SI);
            return !Switches.empty();
        }

        StringRef getPassName() const override { return "DCVM Lower Switch"; }
    };
} // namespace

char DCVMLowerSwitch::ID = 0;

INITIALIZE_PASS(DCVMLowerSwitch, "dcvm-lower-switch", "DCVM Lower Switch", false, false)

namespace {
    class DCVMPassConfig : public TargetPassConfig {
    public:
        DCVMPassConfig(DCVMTargetMachine& TM, PassManagerBase& PM) : TargetPassConfig(TM, PM) {}

        DCVMTargetMachine& getDCVMTargetMachine() const { return getTM<DCVMTargetMachine>(); }

        void addIRPasses() override;
        bool addInstSelector() override;
    };
} // namespace

TargetPassConfig* DCVMTargetMachine::createPassConfig(PassManagerBase& PM) {
    return new DCVMPassConfig(*this, PM);
}

void DCVMPassConfig::addIRPasses() {
    addPass(new DCVMLowerSwitch());
    TargetPassConfig::addIRPasses();
}

bool DCVMPassConfig::addInstSelector() {
    addPass(createDCVMISelDag(getDCVMTargetMachine()));
    return false;
}
