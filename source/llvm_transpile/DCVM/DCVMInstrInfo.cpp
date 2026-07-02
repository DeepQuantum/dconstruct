#include "DCVMInstrInfo.h"
#include "DCVMSubtarget.h"
#include "MCTargetDesc/DCVMMCTargetDesc.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/raw_ostream.h"

#define GET_INSTRINFO_CTOR_DTOR
#include "DCVMGenInstrInfo.inc"

namespace llvm {
    DCVMInstrInfo::DCVMInstrInfo(const DCVMSubtarget& STI) : DCVMGenInstrInfo(STI, RI), RI() {}

    void DCVMInstrInfo::copyPhysReg(
        MachineBasicBlock& MBB, MachineBasicBlock::iterator MI, const DebugLoc& DL, Register DestReg, Register SrcReg, bool KillSrc, bool RenamableDest, bool RenamableSrc
    ) const {
        BuildMI(MBB, MI, DL, get(DCVM::MOVEr), DestReg).addReg(SrcReg, getKillRegState(KillSrc));
    }

    std::optional<DestSourcePair> DCVMInstrInfo::isCopyInstrImpl(const MachineInstr& MI) const {
        if (MI.getOpcode() == DCVM::MOVEr)
            return DestSourcePair{MI.getOperand(0), MI.getOperand(1)};
        return std::nullopt;
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
        MachineFunction* MF = MBB.getParent();
        MachineRegisterInfo& MRI = MF->getRegInfo();

        errs() << "\n=== DCVM spill trace ===\n";
        errs() << "function: " << MF->getName() << "\n";
        errs() << "frame index: " << FrameIndex << "\n";
        errs() << "source register: " << SrcReg << " id=" << SrcReg.id() << "\n";
        errs() << "virtual register argument: " << VReg << " id=" << VReg.id() << "\n";
        errs() << "isKill: " << isKill << ", flags: " << Flags << "\n";
        if (RC != nullptr) {
            errs() << "register class: " << TRI.getRegClassName(RC) << "\n";
        }

        errs() << "\nspill insertion point:\n";
        if (MI == MBB.end()) {
            errs() << "  <end of basic block>\n";
        } else {
            MI->print(errs());
        }

        if (SrcReg.isVirtual()) {
            if (MachineInstr* DefMI = MRI.getVRegDef(SrcReg)) {
                errs() << "\nsource register def:\n";
                DefMI->print(errs());
            } else {
                errs() << "\nsource register def: <none>\n";
            }

            errs() << "\nsource register operands:\n";
            for (MachineOperand& MO : MRI.reg_nodbg_operands(SrcReg)) {
                errs() << "  " << (MO.isDef() ? "def" : "use") << ": ";
                MO.getParent()->print(errs());
            }
        }

        if (VReg.isVirtual() && VReg != SrcReg) {
            if (MachineInstr* DefMI = MRI.getVRegDef(VReg)) {
                errs() << "\nvirtual register argument def:\n";
                DefMI->print(errs());
            } else {
                errs() << "\nvirtual register argument def: <none>\n";
            }
        }

        errs() << "\ncurrent machine basic block:\n";
        MBB.print(errs());
        errs() << "\nstack trace:\n";
        sys::PrintStackTrace(errs());
        errs() << "=== end DCVM spill trace ===\n\n";

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
        MachineFunction* MF = MBB.getParent();
        MachineRegisterInfo& MRI = MF->getRegInfo();

        errs() << "\n=== DCVM reload trace ===\n";
        errs() << "function: " << MF->getName() << "\n";
        errs() << "frame index: " << FrameIndex << "\n";
        errs() << "destination register: " << DestReg << " id=" << DestReg.id() << "\n";
        errs() << "virtual register argument: " << VReg << " id=" << VReg.id() << "\n";
        errs() << "subreg: " << SubReg << ", flags: " << Flags << "\n";
        if (RC != nullptr) {
            errs() << "register class: " << TRI.getRegClassName(RC) << "\n";
        }

        errs() << "\nreload insertion point:\n";
        if (MI == MBB.end()) {
            errs() << "  <end of basic block>\n";
        } else {
            MI->print(errs());
        }

        if (DestReg.isVirtual()) {
            errs() << "\ndestination register operands:\n";
            for (MachineOperand& MO : MRI.reg_nodbg_operands(DestReg)) {
                errs() << "  " << (MO.isDef() ? "def" : "use") << ": ";
                MO.getParent()->print(errs());
            }
        }

        errs() << "\ncurrent machine basic block:\n";
        MBB.print(errs());
        errs() << "=== end DCVM reload trace ===\n\n";

        report_fatal_error(
            "DCVM register reload requested in " + MBB.getParent()->getName() +
            ", frame index " + Twine(FrameIndex) +
            ", destination register " + Twine(DestReg.id()) +
            ", virtual register " + Twine(VReg.id())
        );
    }
} // namespace llvm
