#include "DCVMInstrInfo.h"
#include "DCVMSubtarget.h"
#include "MCTargetDesc/DCVMMCTargetDesc.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Support/ErrorHandling.h"

#define GET_INSTRINFO_CTOR_DTOR
#include "DCVMGenInstrInfo.inc"

namespace llvm {
    DCVMInstrInfo::DCVMInstrInfo(const DCVMSubtarget& STI) : DCVMGenInstrInfo(STI, RI), RI() {}

    void DCVMInstrInfo::copyPhysReg(
        MachineBasicBlock& MBB, MachineBasicBlock::iterator MI, const DebugLoc& DL, Register DestReg, Register SrcReg, bool KillSrc, bool RenamableDest, bool RenamableSrc
    ) const {
        BuildMI(MBB, MI, DL, get(DCVM::MOVEr), DestReg).addReg(SrcReg, getKillRegState(KillSrc));
    }

    static bool isCondBranch(unsigned Opcode) {
        return Opcode == DCVM::BRANCHIF || Opcode == DCVM::BRANCHIFNOT;
    }

    bool DCVMInstrInfo::analyzeBranch(MachineBasicBlock& MBB, MachineBasicBlock*& TBB, MachineBasicBlock*& FBB, SmallVectorImpl<MachineOperand>& Cond, bool AllowModify) const {
        MachineBasicBlock::iterator I = MBB.getLastNonDebugInstr();
        if (I == MBB.end() || !isUnpredicatedTerminator(*I))
            return false;

        MachineInstr* LastInst = &*I;
        const unsigned LastOpc = LastInst->getOpcode();

        if (I == MBB.begin() || !isUnpredicatedTerminator(*--I)) {
            if (LastOpc == DCVM::BR) {
                TBB = LastInst->getOperand(0).getMBB();
                return false;
            }
            if (isCondBranch(LastOpc)) {
                TBB = LastInst->getOperand(0).getMBB();
                Cond.push_back(MachineOperand::CreateImm(LastOpc));
                Cond.push_back(LastInst->getOperand(1));
                return false;
            }
            return true;
        }

        MachineInstr* SecondLastInst = &*I;
        if (isCondBranch(SecondLastInst->getOpcode()) && LastOpc == DCVM::BR) {
            TBB = SecondLastInst->getOperand(0).getMBB();
            Cond.push_back(MachineOperand::CreateImm(SecondLastInst->getOpcode()));
            Cond.push_back(SecondLastInst->getOperand(1));
            FBB = LastInst->getOperand(0).getMBB();
            return false;
        }

        return true;
    }

    unsigned DCVMInstrInfo::insertBranch(MachineBasicBlock& MBB, MachineBasicBlock* TBB, MachineBasicBlock* FBB, ArrayRef<MachineOperand> Cond, const DebugLoc& DL, int* BytesAdded) const {
        assert(BytesAdded == nullptr && "code size not tracked");

        if (Cond.empty()) {
            BuildMI(&MBB, DL, get(DCVM::BR)).addMBB(TBB);
            return 1;
        }

        BuildMI(&MBB, DL, get(Cond[0].getImm())).addMBB(TBB).add(Cond[1]);
        if (FBB == nullptr)
            return 1;

        BuildMI(&MBB, DL, get(DCVM::BR)).addMBB(FBB);
        return 2;
    }

    unsigned DCVMInstrInfo::removeBranch(MachineBasicBlock& MBB, int* BytesRemoved) const {
        assert(BytesRemoved == nullptr && "code size not tracked");

        unsigned Removed = 0;
        MachineBasicBlock::iterator I = MBB.end();
        while (I != MBB.begin()) {
            --I;
            if (I->isDebugInstr())
                continue;
            if (I->getOpcode() != DCVM::BR && !isCondBranch(I->getOpcode()))
                break;
            I = MBB.erase(I);
            ++Removed;
        }
        return Removed;
    }

    bool DCVMInstrInfo::reverseBranchCondition(SmallVectorImpl<MachineOperand>& Cond) const {
        assert(Cond.size() == 2 && "invalid branch condition");
        Cond[0].setImm(Cond[0].getImm() == DCVM::BRANCHIF ? DCVM::BRANCHIFNOT : DCVM::BRANCHIF);
        return false;
    }

    void DCVMInstrInfo::storeRegToStackSlot(
        MachineBasicBlock& MBB,
        MachineBasicBlock::iterator MI,
        Register SrcReg,
        bool isKill,
        int FrameIndex,
        const TargetRegisterClass* RC,
        Register VReg,
        MachineInstr::MIFlag Flags
    ) const {
        report_fatal_error(
            "DCVM register spill requested in " + MBB.getParent()->getName() +
            ", frame index " + Twine(FrameIndex) +
            ", source register " + Twine(SrcReg.id()) +
            ", virtual register " + Twine(VReg.id())
        );
    }

    void DCVMInstrInfo::loadRegFromStackSlot(
        MachineBasicBlock& MBB,
        MachineBasicBlock::iterator MI,
        Register DestReg,
        int FrameIndex,
        const TargetRegisterClass* RC,
        Register VReg,
        unsigned SubReg,
        MachineInstr::MIFlag Flags
    ) const {
        report_fatal_error(
            "DCVM register reload requested in " + MBB.getParent()->getName() +
            ", frame index " + Twine(FrameIndex) +
            ", destination register " + Twine(DestReg.id()) +
            ", virtual register " + Twine(VReg.id())
        );
    }
} // namespace llvm
