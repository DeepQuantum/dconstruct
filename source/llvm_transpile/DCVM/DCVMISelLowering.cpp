#include "DCVMISelLowering.h"
#include "DCVMSubtarget.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
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

    setOperationAction(ISD::SINT_TO_FP, MVT::i64, Legal);
    setOperationAction(ISD::SINT_TO_FP, MVT::f32, Legal);
    setOperationAction(ISD::FP_TO_SINT, MVT::i64, Legal);
    setOperationAction(ISD::FP_TO_SINT, MVT::f32, Legal);

    setOperationAction(ISD::BRCOND, MVT::Other, Legal);
    setOperationAction(ISD::BR_CC, MVT::i64, Expand);
    setOperationAction(ISD::BR_CC, MVT::f32, Expand);

    setOperationAction(ISD::SELECT, MVT::i64, Expand);
    setOperationAction(ISD::SELECT, MVT::f32, Expand);
    setOperationAction(ISD::SELECT_CC, MVT::i64, Custom);
    setOperationAction(ISD::SELECT_CC, MVT::f32, Custom);

    for (const MVT MemVT : {MVT::i8, MVT::i16, MVT::i32}) {
        setLoadExtAction(ISD::ZEXTLOAD, MVT::i64, MemVT, Legal);
        setLoadExtAction(ISD::SEXTLOAD, MVT::i64, MemVT, Legal);
        setLoadExtAction(ISD::EXTLOAD, MVT::i64, MemVT, Legal);
        setTruncStoreAction(MVT::i64, MemVT, Legal);
    }

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
    case DCVMISD::SELECT:
        return "DCVMISD::SELECT";
    default:
        return nullptr;
    }
}

SDValue DCVMTargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const {
    switch (Op.getOpcode()) {
    case ISD::SELECT_CC:
        return LowerSELECT_CC(Op, DAG);
    default:
        llvm_unreachable("DCVMTargetLowering::LowerOperation unimplemented");
    }
}

SDValue DCVMTargetLowering::LowerSELECT_CC(SDValue Op, SelectionDAG &DAG) const {
    const SDLoc DL(Op);
    SDValue LHS = Op.getOperand(0);
    SDValue RHS = Op.getOperand(1);
    SDValue TrueV = Op.getOperand(2);
    SDValue FalseV = Op.getOperand(3);
    SDValue CC = Op.getOperand(4);

    SDValue Cond = DAG.getNode(ISD::SETCC, DL, MVT::i64, LHS, RHS, CC);
    return DAG.getNode(DCVMISD::SELECT, DL, Op.getValueType(), Cond, TrueV, FalseV);
}

MachineBasicBlock *
DCVMTargetLowering::EmitInstrWithCustomInserter(MachineInstr &MI,
                                                MachineBasicBlock *BB) const {
    const TargetInstrInfo &TII = *Subtarget->getInstrInfo();
    const DebugLoc DL = MI.getDebugLoc();
    MachineFunction *MF = BB->getParent();
    const BasicBlock *LLVMBB = BB->getBasicBlock();

    MachineBasicBlock *FalseMBB = MF->CreateMachineBasicBlock(LLVMBB);
    MachineBasicBlock *SinkMBB = MF->CreateMachineBasicBlock(LLVMBB);
    MF->insert(++BB->getIterator(), FalseMBB);
    MF->insert(++FalseMBB->getIterator(), SinkMBB);

    SinkMBB->splice(SinkMBB->begin(), BB, std::next(MachineBasicBlock::iterator(MI)),
                    BB->end());
    SinkMBB->transferSuccessorsAndUpdatePHIs(BB);

    BB->addSuccessor(FalseMBB);
    BB->addSuccessor(SinkMBB);
    FalseMBB->addSuccessor(SinkMBB);

    BuildMI(BB, DL, TII.get(DCVM::BRANCHIF))
        .addMBB(SinkMBB)
        .addReg(MI.getOperand(1).getReg());

    BuildMI(*SinkMBB, SinkMBB->begin(), DL, TII.get(TargetOpcode::PHI),
            MI.getOperand(0).getReg())
        .addReg(MI.getOperand(2).getReg())
        .addMBB(BB)
        .addReg(MI.getOperand(3).getReg())
        .addMBB(FalseMBB);

    MI.eraseFromParent();
    return SinkMBB;
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
    const MDNode *Distance = CB->getMetadata("dcvm.distance");
    if (Distance == nullptr || Distance->getNumOperands() == 0)
        return false;
    const auto *DistanceStr = dyn_cast<MDString>(Distance->getOperand(0));
    return DistanceStr != nullptr && DistanceStr->getString() == "far";
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
