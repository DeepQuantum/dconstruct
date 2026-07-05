#pragma once

#include <llvm/Analysis/TargetTransformInfoImpl.h>

namespace llvm {

class DCVMTTIImpl final : public TargetTransformInfoImplCRTPBase<DCVMTTIImpl> {
public:
    explicit DCVMTTIImpl(const DataLayout& DL) : TargetTransformInfoImplCRTPBase<DCVMTTIImpl>(DL) {}

    bool shouldBuildLookupTables() const override { return false; }

    bool shouldBuildLookupTablesForConstant(Constant* C) const override { return false; }
};
} // namespace llvm
