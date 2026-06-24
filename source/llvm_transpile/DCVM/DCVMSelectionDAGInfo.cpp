#include "DCVMSelectionDAGInfo.h"

#define GET_SDNODE_DESC
#include "DCVMGenSDNodeInfo.inc"

using namespace llvm;

DCVMSelectionDAGInfo::DCVMSelectionDAGInfo()
    : SelectionDAGGenTargetInfo(DCVMGenSDNodeInfo) {}

DCVMSelectionDAGInfo::~DCVMSelectionDAGInfo() = default;
