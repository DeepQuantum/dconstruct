#pragma once


#include "DCVMRegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "DCVMGenInstrInfo.inc"

namespace llvm {

class DCVMSubtarget;

class DCVMInstrInfo : public DCVMGenInstrInfo {
    DCVMRegisterInfo RI;

public:
   explicit DCVMInstrInfo();

   const DCVMRegisterInfo& getRegisterInfo() const { return RI; }
};

} // namespace llvm
