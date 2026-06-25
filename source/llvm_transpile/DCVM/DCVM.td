include "llvm/Target/Target.td"
include "DCVMConsts.td"
include "DCVMRegisterInfo.td"
include "DCVMCallingConv.td"
include "DCVMInstrInfo.td"

def DCVMInstrInfo : InstrInfo;

defm : RemapAllTargetPseudoPointerOperands<GPR>;

def : ProcessorModel<"generic", NoSchedModel, []>;

def DCVM : Target {
  let InstructionSet = DCVMInstrInfo;
}
