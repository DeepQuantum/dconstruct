#include "DCVM.h"
#include "DCVMMachineFunctionInfo.h"
#include "DCVMSubtarget.h"
#include "DCVMTargetMachine.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/IR/IntrinsicsDCVM.h"
#include "llvm/Support/MathExtras.h"

using namespace llvm;

#define DEBUG_TYPE "dcvm-isel"
#define PASS_NAME "DCVM DAG->DAG Pattern Instruction Selection"

namespace {
class DCVMDAGToDAGISel : public SelectionDAGISel {
    const DCVMSubtarget *Subtarget = nullptr;

public:
    DCVMDAGToDAGISel() = delete;

    explicit DCVMDAGToDAGISel(DCVMTargetMachine &TM) : SelectionDAGISel(TM) {}

    bool runOnMachineFunction(MachineFunction &MF) override {
        Subtarget = &MF.getSubtarget<DCVMSubtarget>();
        return SelectionDAGISel::runOnMachineFunction(MF);
    }

    void Select(SDNode *N) override;

#include "DCVMGenDAGISel.inc"
};

class DCVMDAGToDAGISelLegacy : public SelectionDAGISelLegacy {
public:
    static char ID;
    explicit DCVMDAGToDAGISelLegacy(DCVMTargetMachine &TM)
        : SelectionDAGISelLegacy(ID, std::make_unique<DCVMDAGToDAGISel>(TM)) {}
};
} // namespace

char DCVMDAGToDAGISelLegacy::ID = 0;

INITIALIZE_PASS(DCVMDAGToDAGISelLegacy, DEBUG_TYPE, PASS_NAME, false, false)

void DCVMDAGToDAGISel::Select(SDNode *N) {
    if (N->isMachineOpcode()) {
        N->setNodeId(-1);
        return;
    }

    const SDLoc DL(N);
    DCVMMachineFunctionInfo *MFI = MF->getInfo<DCVMMachineFunctionInfo>();

    switch (N->getOpcode()) {
    case ISD::Constant: {
        const uint64_t Value = cast<ConstantSDNode>(N)->getZExtValue();
        if (isUInt<16>(Value))
            break;
        const SDValue Idx =
            CurDAG->getTargetConstant(MFI->internSymbol(Value), DL, MVT::i64);
        ReplaceNode(N, CurDAG->getMachineNode(DCVM::LOADSTATICU64IMMri, DL,
                                              MVT::i64, Idx));
        return;
    }
    case ISD::ConstantFP: {
        const uint64_t Bits = cast<ConstantFPSDNode>(N)
                                  ->getValueAPF()
                                  .bitcastToAPInt()
                                  .getZExtValue();
        if (isUInt<16>(Bits)) {
            const SDValue Imm = CurDAG->getTargetConstant(Bits, DL, MVT::i64);
            ReplaceNode(N, CurDAG->getMachineNode(DCVM::LOADU16IMMri, DL,
                                                  MVT::f32, Imm));
            return;
        }
        const SDValue Idx =
            CurDAG->getTargetConstant(MFI->internSymbol(Bits), DL, MVT::i64);
        ReplaceNode(N, CurDAG->getMachineNode(DCVM::LOADSTATICFLOATIMMri, DL,
                                              MVT::f32, Idx));
        return;
    }
    case ISD::INTRINSIC_WO_CHAIN: {
        const unsigned IID = N->getConstantOperandVal(0);
        if (IID == Intrinsic::dcvm_lookup) {
            const uint64_t Sid =
                cast<ConstantSDNode>(N->getOperand(1))->getZExtValue();
            const SDValue Idx =
                CurDAG->getTargetConstant(MFI->internSymbol(Sid), DL, MVT::i64);
            ReplaceNode(N, CurDAG->getMachineNode(DCVM::LOOKUPPOINTERri, DL,
                                                  N->getValueType(0), Idx));
            return;
        }
        if (IID == Intrinsic::dcvm_intash) {
            ReplaceNode(N, CurDAG->getMachineNode(DCVM::INTASHrr, DL, MVT::i64,
                                                  N->getOperand(1),
                                                  N->getOperand(2)));
            return;
        }
        if (IID == Intrinsic::dcvm_static_pointer) {
            const uint64_t Index =
                cast<ConstantSDNode>(N->getOperand(1))->getZExtValue();
            const SDValue Idx =
                CurDAG->getTargetConstant(MFI->internSymbol(Index), DL, MVT::i64);
            ReplaceNode(N, CurDAG->getMachineNode(DCVM::LOADSTATICPOINTERIMMri, DL,
                                                  N->getValueType(0), Idx));
            return;
        }
        break;
    }
    }

    SelectCode(N);
}

FunctionPass *llvm::createDCVMISelDag(DCVMTargetMachine &TM) {
    return new DCVMDAGToDAGISelLegacy(TM);
}
