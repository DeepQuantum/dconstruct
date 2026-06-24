#pragma once

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/DataTypes.h"

#include <memory>

namespace llvm {
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class Target;

MCCodeEmitter *createDCVMMCCodeEmitter(const MCInstrInfo &MCII, MCContext &Ctx);
} // namespace llvm

#define GET_REGINFO_ENUM
#include "DCVMGenRegisterInfo.inc"

#define GET_INSTRINFO_ENUM
#define GET_INSTRINFO_MC_HELPER_DECLS
#include "DCVMGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "DCVMGenSubtargetInfo.inc"
