#pragma once

#include "llvm/Target/TargetMachine.h"

namespace llvm {

class DCVMTargetMachine;
class FunctionPass;
class PassRegistry;
class Target;

FunctionPass* createDCVMISelDag(DCVMTargetMachine& TM);

void initializeDCVMDAGToDAGISelLegacyPass(PassRegistry&);

Target& getTheDCVMTarget();

} // namespace llvm
