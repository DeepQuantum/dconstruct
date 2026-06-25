#include "DCVM.h"
#include "DCVMMCInstLower.h"
#include "DCVMMachineFunctionInfo.h"
#include "MCTargetDesc/DCVMInstPrinter.h"
#include "MCTargetDesc/DCVMMCTargetDesc.h"
#include "TargetInfo/DCVMTargetInfo.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Compiler.h"

using namespace llvm;

#define DEBUG_TYPE "asm-printer"

namespace {
class DCVMAsmPrinter : public AsmPrinter {
    SmallVector<char, 256> ByteCode;

public:
    explicit DCVMAsmPrinter(TargetMachine &TM,
                            std::unique_ptr<MCStreamer> Streamer)
        : AsmPrinter(TM, std::move(Streamer), ID) {}

    StringRef getPassName() const override { return "DCVM Assembly Printer"; }

    void emitInstruction(const MachineInstr *MI) override;
    void emitFunctionBodyEnd() override;
    bool doFinalization(Module &M) override;

    static const char *getRegisterName(MCRegister Reg) {
        return DCVMInstPrinter::getRegisterName(Reg);
    }

    static char ID;
};
} // namespace

char DCVMAsmPrinter::ID = 0;

void DCVMAsmPrinter::emitInstruction(const MachineInstr *MI) {
    DCVMMCInstLower MCInstLowering(OutContext, *this);
    MCInst TmpInst;
    MCInstLowering.Lower(MI, TmpInst);
    EmitToStreamer(*OutStreamer, TmpInst);
}

void DCVMAsmPrinter::emitFunctionBodyEnd() {
    const ArrayRef<uint64_t> Symbols =
        MF->getInfo<DCVMMachineFunctionInfo>()->getSymbols();
    if (Symbols.empty())
        return;

    MCSymbol *TableSym =
        OutContext.getOrCreateSymbol(MF->getName() + "_symbol_table");
    OutStreamer->emitLabel(TableSym);
    for (const uint64_t Value : Symbols)
        OutStreamer->emitInt64(Value);
}

bool DCVMAsmPrinter::doFinalization(Module &M) {
    return AsmPrinter::doFinalization(M);
}

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void
LLVMInitializeDCVMAsmPrinter() {
    RegisterAsmPrinter<DCVMAsmPrinter> X(getTheDCVMTarget());
}
