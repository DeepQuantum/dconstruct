#pragma once

#include "llvm/Support/Compiler.h"

namespace llvm {

    class AsmPrinter;
    class MCContext;
    class MCInst;
    class MCOperand;
    class MachineInstr;
    class MachineOperand;

    class LLVM_LIBRARY_VISIBILITY DCVMMCInstLower {
        MCContext& Ctx;
        AsmPrinter& Printer;

    public:
        DCVMMCInstLower(MCContext& Ctx, AsmPrinter& Printer) : Ctx(Ctx), Printer(Printer) {}

        void Lower(const MachineInstr* MI, MCInst& OutMI) const;

        MCOperand LowerOperand(const MachineOperand& MO) const;
    };

} // namespace llvm
