#pragma once

#include "llvm/CodeGen/SelectionDAGTargetInfo.h"

namespace llvm {

class DCVMSelectionDAGInfo : public SelectionDAGTargetInfo {
public:
    DCVMSelectionDAGInfo();

    ~DCVMSelectionDAGInfo() override;
};

} // namespace llvm
