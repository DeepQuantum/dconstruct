#include "DCVM.h"
#include "DCVMMachineFunctionInfo.h"
#include "DCVMSubtarget.h"
#include "DCVMTargetMachine.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/CodeGen/TargetOpcodes.h"
#include "llvm/IR/IntrinsicsDCVM.h"
#include "llvm/Support/MathExtras.h"

using namespace llvm;

#define DEBUG_TYPE "dcvm-isel"
#define PASS_NAME "DCVM DAG->DAG Pattern Instruction Selection"

namespace {
    class DCVMDAGToDAGISel : public SelectionDAGISel {
        const DCVMSubtarget* Subtarget = nullptr;

    public:
        DCVMDAGToDAGISel() = delete;

        explicit DCVMDAGToDAGISel(DCVMTargetMachine& TM) : SelectionDAGISel(TM) {}

        bool runOnMachineFunction(MachineFunction& MF) override {
            Subtarget = &MF.getSubtarget<DCVMSubtarget>();
            return SelectionDAGISel::runOnMachineFunction(MF);
        }

        void Select(SDNode* N) override;

#include "DCVMGenDAGISel.inc"
    };

    class DCVMDAGToDAGISelLegacy : public SelectionDAGISelLegacy {
    public:
        static char ID;
        explicit DCVMDAGToDAGISelLegacy(DCVMTargetMachine& TM) : SelectionDAGISelLegacy(ID, std::make_unique<DCVMDAGToDAGISel>(TM)) {}
    };
} // namespace

char DCVMDAGToDAGISelLegacy::ID = 0;

INITIALIZE_PASS(DCVMDAGToDAGISelLegacy, DEBUG_TYPE, PASS_NAME, false, false)

void DCVMDAGToDAGISel::Select(SDNode* N) {
    if (N->isMachineOpcode()) {
        N->setNodeId(-1);
        return;
    }

    const SDLoc DL(N);
    DCVMMachineFunctionInfo* MFI = MF->getInfo<DCVMMachineFunctionInfo>();

    switch (N->getOpcode()) {
        case ISD::Constant: {
            const uint64_t Value = cast<ConstantSDNode>(N)->getZExtValue();
            if (N->getValueType(0) == MVT::i32) {
                SDNode* Wide = nullptr;
                if (isUInt<16>(Value)) {
                    const SDValue Imm = CurDAG->getTargetConstant(Value, DL, MVT::i64);
                    Wide = CurDAG->getMachineNode(DCVM::LOADU16IMMri, DL, MVT::i64, Imm);
                } else {
                    const SDValue Idx = CurDAG->getTargetConstant(MFI->internSymbol(Value), DL, MVT::i64);
                    Wide = CurDAG->getMachineNode(DCVM::LOADSTATICU64IMMri, DL, MVT::i64, Idx);
                }
                const SDValue RC = CurDAG->getTargetConstant(DCVM::GPR32RegClassID, DL, MVT::i64);
                ReplaceNode(N, CurDAG->getMachineNode(TargetOpcode::COPY_TO_REGCLASS, DL, MVT::i32, SDValue(Wide, 0), RC));
                return;
            }
            if (isUInt<16>(Value))
                break;
            const SDValue Idx = CurDAG->getTargetConstant(MFI->internSymbol(Value), DL, MVT::i64);
            ReplaceNode(N, CurDAG->getMachineNode(DCVM::LOADSTATICU64IMMri, DL, MVT::i64, Idx));
            return;
        }
        case ISD::ConstantFP: {
            const uint64_t Bits = cast<ConstantFPSDNode>(N)->getValueAPF().bitcastToAPInt().getZExtValue();
            if (isUInt<16>(Bits)) {
                const SDValue Imm = CurDAG->getTargetConstant(Bits, DL, MVT::i64);
                ReplaceNode(N, CurDAG->getMachineNode(DCVM::LOADU16IMMri, DL, MVT::f32, Imm));
                return;
            }
            const SDValue Idx = CurDAG->getTargetConstant(MFI->internSymbol(Bits), DL, MVT::i64);
            ReplaceNode(N, CurDAG->getMachineNode(DCVM::LOADSTATICFLOATIMMri, DL, MVT::f32, Idx));
            return;
        }
        case ISD::INTRINSIC_WO_CHAIN: {
            const unsigned IID = N->getConstantOperandVal(0);
            if (IID == Intrinsic::dcvm_lookup) {
                const uint64_t Sid = cast<ConstantSDNode>(N->getOperand(1))->getZExtValue();
                const SDValue Idx = CurDAG->getTargetConstant(MFI->internSymbol(Sid), DL, MVT::i64);
                ReplaceNode(N, CurDAG->getMachineNode(DCVM::LOOKUPPOINTERri, DL, N->getValueType(0), Idx));
                return;
            }
            if (IID == Intrinsic::dcvm_intash) {
                ReplaceNode(N, CurDAG->getMachineNode(DCVM::INTASHrr, DL, MVT::i64, N->getOperand(1), N->getOperand(2)));
                return;
            }
            if (IID == Intrinsic::dcvm_static_pointer) {
                const uint64_t Index = cast<ConstantSDNode>(N->getOperand(1))->getZExtValue();
                const SDValue Idx = CurDAG->getTargetConstant(MFI->internSymbol(Index, true), DL, MVT::i64);
                ReplaceNode(N, CurDAG->getMachineNode(DCVM::LOADSTATICPOINTERIMMri, DL, N->getValueType(0), Idx));
                return;
            }
            if (IID == Intrinsic::dcvm_load_static_int) {
                ReplaceNode(N, CurDAG->getMachineNode(DCVM::LOADSTATICINTrr, DL, MVT::i64, N->getOperand(1)));
                return;
            }
            if (IID == Intrinsic::dcvm_load_static_float) {
                ReplaceNode(N, CurDAG->getMachineNode(DCVM::LOADSTATICFLOATrr, DL, MVT::f32, N->getOperand(1)));
                return;
            }
            if (IID == Intrinsic::dcvm_load_static_pointer) {
                ReplaceNode(N, CurDAG->getMachineNode(DCVM::LOADSTATICPOINTERrr, DL, N->getValueType(0), N->getOperand(1)));
                return;
            }
            break;
        }
        case ISD::SHL: {
            const ConstantSDNode* ShAmt = dyn_cast<ConstantSDNode>(N->getOperand(1));
            if (!ShAmt)
                break;
            const uint64_t Factor = 1ULL << ShAmt->getZExtValue();
            if (isUInt<8>(Factor)) {
                const SDValue Imm = CurDAG->getTargetConstant(Factor, DL, MVT::i64);
                ReplaceNode(N, CurDAG->getMachineNode(DCVM::IMULIMMri, DL, MVT::i64, N->getOperand(0), Imm));
                return;
            }
            const SDValue Idx = CurDAG->getTargetConstant(MFI->internSymbol(Factor), DL, MVT::i64);
            SDNode* FactorReg = CurDAG->getMachineNode(DCVM::LOADSTATICU64IMMri, DL, MVT::i64, Idx);
            ReplaceNode(N, CurDAG->getMachineNode(DCVM::IMULrr, DL, MVT::i64, N->getOperand(0), SDValue(FactorReg, 0)));
            return;
        }
        case ISD::SUB: {
            if (N->getValueType(0) != MVT::i32)
                break;

            const SDValue LHS = N->getOperand(0);
            const SDValue RHS = N->getOperand(1);
            if (RHS.getOpcode() != ISD::MUL)
                break;

            const ConstantSDNode* Divisor = dyn_cast<ConstantSDNode>(RHS.getOperand(1));
            SDValue Quotient = RHS.getOperand(0);
            if (!Divisor) {
                Divisor = dyn_cast<ConstantSDNode>(RHS.getOperand(0));
                Quotient = RHS.getOperand(1);
            }
            if (!Divisor || Divisor->getZExtValue() == 0 || Quotient.getOpcode() != ISD::SRL)
                break;

            const ConstantSDNode* Shift = dyn_cast<ConstantSDNode>(Quotient.getOperand(1));
            const SDValue MulHi = Quotient.getOperand(0);
            if (!Shift || Shift->getZExtValue() != 6 || MulHi.getOpcode() != ISD::MULHU)
                break;

            const ConstantSDNode* Magic = dyn_cast<ConstantSDNode>(MulHi.getOperand(1));
            SDValue Dividend = MulHi.getOperand(0);
            if (!Magic) {
                Magic = dyn_cast<ConstantSDNode>(MulHi.getOperand(0));
                Dividend = MulHi.getOperand(1);
            }
            if (!Magic || Dividend != LHS)
                break;

            // llvm expands urem by constants into mulhi/shift/sub during legalization.
            // fold that expansion back to the vm modulo opcode.
            const uint64_t DivisorValue = Divisor->getZExtValue();
            const SDValue GPR = CurDAG->getTargetConstant(DCVM::GPRRegClassID, DL, MVT::i64);
            const SDValue GPR32 = CurDAG->getTargetConstant(DCVM::GPR32RegClassID, DL, MVT::i64);
            SDNode* Dividend64 = CurDAG->getMachineNode(TargetOpcode::COPY_TO_REGCLASS, DL, MVT::i64, LHS, GPR);
            SDNode* Divisor64 = nullptr;
            if (isUInt<16>(DivisorValue)) {
                const SDValue Imm = CurDAG->getTargetConstant(DivisorValue, DL, MVT::i64);
                Divisor64 = CurDAG->getMachineNode(DCVM::LOADU16IMMri, DL, MVT::i64, Imm);
            } else {
                const SDValue Idx = CurDAG->getTargetConstant(MFI->internSymbol(DivisorValue), DL, MVT::i64);
                Divisor64 = CurDAG->getMachineNode(DCVM::LOADSTATICU64IMMri, DL, MVT::i64, Idx);
            }
            SDNode* Mod = CurDAG->getMachineNode(DCVM::IMODrr, DL, MVT::i64, SDValue(Dividend64, 0), SDValue(Divisor64, 0));
            ReplaceNode(N, CurDAG->getMachineNode(TargetOpcode::COPY_TO_REGCLASS, DL, MVT::i32, SDValue(Mod, 0), GPR32));
            return;
        }
    }

    SelectCode(N);
}

FunctionPass* llvm::createDCVMISelDag(DCVMTargetMachine& TM) {
    return new DCVMDAGToDAGISelLegacy(TM);
}
