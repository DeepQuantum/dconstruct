add_llvm_component_library(LLVMDCVMInfo
  DCVMTargetInfo.cpp

  LINK_COMPONENTS
  MC
  Support

  ADD_TO_COMPONENT
  DCVM
  )
