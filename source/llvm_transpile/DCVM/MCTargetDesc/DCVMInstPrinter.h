#pragma once

#include "DCVMMCTargetDesc.h"
#include "llvm/MC/MCInstPrinter.h"

namespace llvm {

    class DCVMInstPrinter : public MCInstPrinter {
    public:
        DCVMInstPrinter(const MCAsmInfo& MAI, const MCInstrInfo& MII, const MCRegisterInfo& MRI) : MCInstPrinter(MAI, MII, MRI) {}

        void printInst(const MCInst* MI, uint64_t Address, StringRef Annot, const MCSubtargetInfo& STI, raw_ostream& O) override;

        std::pair<const char*, uint64_t> getMnemonic(const MCInst& MI) const override;
        void printInstruction(const MCInst* MI, uint64_t Address, raw_ostream& O);
        static const char* getRegisterName(MCRegister Reg);

        void printOperand(const MCInst* MI, unsigned OpNo, raw_ostream& O);
    };

} // namespace llvm
