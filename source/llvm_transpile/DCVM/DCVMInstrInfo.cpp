#include "DCVMInstrInfo.h"
#include "DCVMSubtarget.h"
#include "MCTargetDesc/DCVMMCTargetDesc.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"

#define GET_INSTRINFO_CTOR_DTOR
#include "DCVMGenInstrInfo.inc"

namespace llvm {
    DCVMInstrInfo::DCVMInstrInfo(const DCVMSubtarget &STI)
        : DCVMGenInstrInfo(STI, RI), RI() {}

    void DCVMInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                    MachineBasicBlock::iterator MI,
                                    const DebugLoc &DL, Register DestReg,
                                    Register SrcReg, bool KillSrc,
                                    bool RenamableDest, bool RenamableSrc) const {
        BuildMI(MBB, MI, DL, get(DCVM::MOVEr), DestReg)
            .addReg(SrcReg, getKillRegState(KillSrc));
    }
} // namespace llvm
