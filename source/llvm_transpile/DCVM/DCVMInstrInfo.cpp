#include "DCVMInstrInfo.h"

#define GET_INSTRINFO_CTOR_DTOR
#include "DCVMGenInstrInfo.inc"

namespace llvm {
    DCVMInstrInfo::DCVMInstrInfo() : DCVMGenInstrInfo(), RI() {}
} // namespace llvm
