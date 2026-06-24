#pragma once

#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

namespace llvm {

class DCVMELFTargetObjectFile : public TargetLoweringObjectFileELF {
public:
    DCVMELFTargetObjectFile() = default;
};

} // namespace llvm
