add_llvm_component_library(LLVMDCVMDesc
  DCVMInstPrinter.cpp
  DCVMMCAsmInfo.cpp
  DCVMMCCodeEmitter.cpp
  DCVMMCTargetDesc.cpp

  LINK_COMPONENTS
  MC
  DCVMInfo
  Support
  TargetParser

  ADD_TO_COMPONENT
  DCVM
  )
