#include "DCVMMachineFunctionInfo.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

void DCVMMachineFunctionInfo::anchor() {}

unsigned DCVMMachineFunctionInfo::internSymbol(uint64_t Value) {
    auto [It, Inserted] = SymbolIndex.try_emplace(Value, Symbols.size());
    if (Inserted) {
        if (Symbols.size() >= 256)
            report_fatal_error("DCVM symbol table exceeds 256 entries");
        Symbols.push_back(Value);
    }
    return It->second;
}

MachineFunctionInfo *DCVMMachineFunctionInfo::clone(
    BumpPtrAllocator &Allocator, MachineFunction &DestMF,
    const DenseMap<MachineBasicBlock *, MachineBasicBlock *> &Src2DstMBB)
    const {
    return DestMF.cloneInfo<DCVMMachineFunctionInfo>(*this);
}
