#pragma once

#include "llvm/ADT/ArrayRef.h"
#include "llvm/CodeGen/MachineFunction.h"
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace llvm {

    class DCVMMachineFunctionInfo : public MachineFunctionInfo {
        virtual void anchor();

        std::vector<uint64_t> Symbols;
        std::vector<std::byte> PointerMap;
        std::unordered_map<uint64_t, unsigned> SymbolIndex;

    public:
        DCVMMachineFunctionInfo() = default;
        DCVMMachineFunctionInfo(const Function& F, const TargetSubtargetInfo* STI) {}

        MachineFunctionInfo* clone(BumpPtrAllocator& Allocator, MachineFunction& DestMF, const DenseMap<MachineBasicBlock*, MachineBasicBlock*>& Src2DstMBB) const override;

        unsigned internSymbol(uint64_t Value, bool IsPointer = false);

        ArrayRef<uint64_t> getSymbols() const { return Symbols; }
        ArrayRef<std::byte> getPointerMap() const { return PointerMap; }
    };

} // namespace llvm
