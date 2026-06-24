#pragma once

#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

class DCVMSubtarget;

class DCVMTargetLowering : public TargetLowering {
    const DCVMSubtarget *Subtarget;

public:
    DCVMTargetLowering(const TargetMachine &TM, const DCVMSubtarget &STI);

    SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;

    SDValue
    LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
                         const SmallVectorImpl<ISD::InputArg> &Ins,
                         const SDLoc &DL, SelectionDAG &DAG,
                         SmallVectorImpl<SDValue> &InVals) const override;

    SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
                        const SmallVectorImpl<ISD::OutputArg> &Outs,
                        const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL,
                        SelectionDAG &DAG) const override;
};

} // namespace llvm
