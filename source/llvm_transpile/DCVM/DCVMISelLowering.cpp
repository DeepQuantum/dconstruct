#include "DCVMISelLowering.h"
#include "DCVMSubtarget.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Metadata.h"

using namespace llvm;

#include "DCVMGenCallingConv.inc"

DCVMTargetLowering::DCVMTargetLowering(const TargetMachine &TM,
                                       const DCVMSubtarget &STI)
    : TargetLowering(TM, STI), Subtarget(&STI) {
    addRegisterClass(MVT::i64, &DCVM::GPRRegClass);
    addRegisterClass(MVT::f32, &DCVM::GPRRegClass);

    setBooleanContents(ZeroOrOneBooleanContent);
    setOperationAction(ISD::SETCC, MVT::i64, Legal);
    setOperationAction(ISD::SETCC, MVT::f32, Legal);

    setOperationAction(ISD::ConstantFP, MVT::f32, Legal);

    setOperationAction(ISD::FREM, MVT::f32, Legal);
    setOperationAction(ISD::ABS, MVT::i64, Legal);
    setOperationAction(ISD::FABS, MVT::f32, Legal);

    computeRegisterProperties(Subtarget->getRegisterInfo());
}

EVT DCVMTargetLowering::getSetCCResultType(const DataLayout &DL,
                                           LLVMContext &Context,
                                           EVT VT) const {
    return MVT::i64;
}

const char *DCVMTargetLowering::getTargetNodeName(unsigned Opcode) const {
    switch (Opcode) {
    case DCVMISD::RET:
        return "DCVMISD::RET";
    case DCVMISD::CALL:
        return "DCVMISD::CALL";
    case DCVMISD::CALLFF:
        return "DCVMISD::CALLFF";
    default:
        return nullptr;
    }
}

SDValue DCVMTargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const {
    llvm_unreachable("DCVMTargetLowering::LowerOperation unimplemented");
}

SDValue DCVMTargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {
    MachineFunction &MF = DAG.getMachineFunction();

    SmallVector<CCValAssign, 16> ArgLocs;
    CCState CCInfo(CallConv, IsVarArg, MF, ArgLocs, *DAG.getContext());
    CCInfo.AnalyzeFormalArguments(Ins, CC_DCVM);

    for (const CCValAssign &VA : ArgLocs) {
        assert(VA.isRegLoc() && "DCVM only supports register arguments");
        const Register VReg = MF.addLiveIn(VA.getLocReg(), &DCVM::GPRRegClass);
        InVals.push_back(DAG.getCopyFromReg(Chain, DL, VReg, VA.getLocVT()));
    }

    return Chain;
}

SDValue DCVMTargetLowering::LowerReturn(
    SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::OutputArg> &Outs,
    const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL,
    SelectionDAG &DAG) const {
    MachineFunction &MF = DAG.getMachineFunction();

    SmallVector<CCValAssign, 16> RVLocs;
    CCState CCInfo(CallConv, IsVarArg, MF, RVLocs, *DAG.getContext());
    CCInfo.AnalyzeReturn(Outs, RetCC_DCVM);

    SDValue Glue;
    SmallVector<SDValue, 4> RetOps(1, Chain);

    for (unsigned i = 0; i < RVLocs.size(); ++i) {
        const CCValAssign &VA = RVLocs[i];
        assert(VA.isRegLoc() && "DCVM only supports register returns");
        Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), OutVals[i], Glue);
        Glue = Chain.getValue(1);
        RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
    }

    RetOps[0] = Chain;
    if (Glue.getNode())
        RetOps.push_back(Glue);

    return DAG.getNode(DCVMISD::RET, DL, MVT::Other, RetOps);
}

static bool callIsFar(const CallBase *CB) {
    if (CB == nullptr)
        return false;
    const MDNode *Callees = CB->getMetadata(LLVMContext::MD_callees);
    if (Callees == nullptr || Callees->getNumOperands() == 0)
        return false;
    const auto *Callee = mdconst::dyn_extract_or_null<Function>(Callees->getOperand(0));
    if (Callee == nullptr)
        return false;
    const MDNode *SidDistance = Callee->getMetadata("dcvm.sid_distance");
    if (SidDistance == nullptr || SidDistance->getNumOperands() < 4)
        return false;
    const auto *Distance = dyn_cast<MDString>(SidDistance->getOperand(3));
    return Distance != nullptr && Distance->getString() == "far";
}

SDValue DCVMTargetLowering::LowerCall(CallLoweringInfo &CLI,
                                      SmallVectorImpl<SDValue> &InVals) const {
    SelectionDAG &DAG = CLI.DAG;
    const SDLoc &DL = CLI.DL;
    MachineFunction &MF = DAG.getMachineFunction();
    SDValue Chain = CLI.Chain;
    CLI.IsTailCall = false;

    SmallVector<CCValAssign, 16> ArgLocs;
    CCState CCInfo(CLI.CallConv, CLI.IsVarArg, MF, ArgLocs, *DAG.getContext());
    CCInfo.AnalyzeCallOperands(CLI.Outs, CC_DCVM);

    SmallVector<std::pair<Register, SDValue>, 8> RegsToPass;
    for (unsigned i = 0; i < ArgLocs.size(); ++i) {
        const CCValAssign &VA = ArgLocs[i];
        assert(VA.isRegLoc() && "DCVM only supports register arguments");
        RegsToPass.emplace_back(VA.getLocReg(), CLI.OutVals[i]);
    }

    SDValue Glue;
    for (const auto &Reg : RegsToPass) {
        Chain = DAG.getCopyToReg(Chain, DL, Reg.first, Reg.second, Glue);
        Glue = Chain.getValue(1);
    }

    SmallVector<SDValue, 8> Ops;
    Ops.push_back(Chain);
    Ops.push_back(CLI.Callee);
    Ops.push_back(DAG.getTargetConstant(RegsToPass.size(), DL, MVT::i64));
    for (const auto &Reg : RegsToPass)
        Ops.push_back(DAG.getRegister(Reg.first, Reg.second.getValueType()));
    if (Glue.getNode())
        Ops.push_back(Glue);

    const unsigned CallOpc = callIsFar(CLI.CB) ? DCVMISD::CALLFF : DCVMISD::CALL;
    const SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);
    Chain = DAG.getNode(CallOpc, DL, NodeTys, Ops);
    Glue = Chain.getValue(1);

    SmallVector<CCValAssign, 16> RVLocs;
    CCState RetCCInfo(CLI.CallConv, CLI.IsVarArg, MF, RVLocs, *DAG.getContext());
    RetCCInfo.AnalyzeCallResult(CLI.Ins, RetCC_DCVM);

    for (const CCValAssign &VA : RVLocs) {
        const SDValue Value =
            DAG.getCopyFromReg(Chain, DL, VA.getLocReg(), VA.getLocVT(), Glue);
        Chain = Value.getValue(1);
        Glue = Value.getValue(2);
        InVals.push_back(Value);
    }

    return Chain;
}
