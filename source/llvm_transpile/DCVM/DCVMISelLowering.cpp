#include "DCVMISelLowering.h"
#include "DCVMSubtarget.h"
#include "llvm/CodeGen/SelectionDAG.h"

using namespace llvm;

DCVMTargetLowering::DCVMTargetLowering(const TargetMachine &TM,
                                       const DCVMSubtarget &STI)
    : TargetLowering(TM), Subtarget(&STI) {}

SDValue DCVMTargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const {
    llvm_unreachable("DCVMTargetLowering::LowerOperation unimplemented");
}

SDValue DCVMTargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {
    return Chain;
}

SDValue DCVMTargetLowering::LowerReturn(
    SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::OutputArg> &Outs,
    const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL,
    SelectionDAG &DAG) const {
    return Chain;
}
