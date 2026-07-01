#pragma once

#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

    class DCVMSubtarget;

    namespace DCVMISD {
        enum NodeType : unsigned {
            FIRST_NUMBER = ISD::BUILTIN_OP_END,
            RET,
            CALL,
            CALLFF,
            SELECT,
        };
    } // namespace DCVMISD

    class DCVMTargetLowering : public TargetLowering {
        const DCVMSubtarget* Subtarget;

    public:
        DCVMTargetLowering(const TargetMachine& TM, const DCVMSubtarget& STI);

        const char* getTargetNodeName(unsigned Opcode) const override;

        EVT getSetCCResultType(const DataLayout& DL, LLVMContext& Context, EVT VT) const override;

        SDValue LowerOperation(SDValue Op, SelectionDAG& DAG) const override;

        SDValue LowerSELECT(SDValue Op, SelectionDAG& DAG) const;

        SDValue LowerSELECT_CC(SDValue Op, SelectionDAG& DAG) const;

        SDValue LowerSETCC(SDValue Op, SelectionDAG& DAG) const;

        MachineBasicBlock* EmitInstrWithCustomInserter(MachineInstr& MI, MachineBasicBlock* MBB) const override;

        SDValue LowerFormalArguments(
            SDValue Chain, CallingConv::ID CallConv, bool IsVarArg, const SmallVectorImpl<ISD::InputArg>& Ins, const SDLoc& DL, SelectionDAG& DAG, SmallVectorImpl<SDValue>& InVals
        ) const override;

        SDValue LowerReturn(
            SDValue Chain, CallingConv::ID CallConv, bool IsVarArg, const SmallVectorImpl<ISD::OutputArg>& Outs, const SmallVectorImpl<SDValue>& OutVals, const SDLoc& DL, SelectionDAG& DAG
        ) const override;

        SDValue LowerCall(CallLoweringInfo& CLI, SmallVectorImpl<SDValue>& InVals) const override;
    };

} // namespace llvm
