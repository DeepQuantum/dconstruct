#pragma once

#include "llvm/CodeGen/SelectionDAGTargetInfo.h"

#define GET_SDNODE_ENUM
#include "DCVMGenSDNodeInfo.inc"

namespace llvm {

class DCVMSelectionDAGInfo : public SelectionDAGGenTargetInfo {
public:
    DCVMSelectionDAGInfo();

    ~DCVMSelectionDAGInfo() override;
};

} // namespace llvm
