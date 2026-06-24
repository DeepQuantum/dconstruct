add_llvm_component_group(DCVM)

set(LLVM_TARGET_DEFINITIONS DCVM.td)

#tablegen(LLVM DCVMGenAsmWriter.inc -gen-asm-writer)
#tablegen(LLVM DCVMGenCallingConv.inc -gen-callingconv)
#tablegen(LLVM DCVMGenDAGISel.inc -gen-dag-isel)
#tablegen(LLVM DCVMGenInstrInfo.inc -gen-instr-info)
#tablegen(LLVM DCVMGenMCCodeEmitter.inc -gen-emitter)
#tablegen(LLVM DCVMGenRegisterInfo.inc -gen-register-info)
#tablegen(LLVM DCVMGenSDNodeInfo.inc -gen-sd-node-info)
#tablegen(LLVM DCVMGenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(DCVMTableGen)

add_llvm_target(DCVMCodeGen
  DCVMAsmPrinter.cpp
  DCVMFrameLowering.cpp
  DCVMInstrInfo.cpp
  DCVMISelDAGToDAG.cpp
  DCVMISelLowering.cpp
  DCVMMachineFunctionInfo.cpp
  DCVMMCInstLower.cpp
  DCVMRegisterInfo.cpp
  DCVMSelectionDAGInfo.cpp
  DCVMSubtarget.cpp
  DCVMTargetMachine.cpp

  LINK_COMPONENTS
  AsmPrinter
  CodeGen
  CodeGenTypes
  Core
  MC
  SelectionDAG
  DCVMDesc
  DCVMInfo
  Support
  Target
  TargetParser

  ADD_TO_COMPONENT
  DCVM
  )

add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
