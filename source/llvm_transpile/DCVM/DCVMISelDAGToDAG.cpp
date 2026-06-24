#include "DCVM.h"
#include "DCVMSubtarget.h"
#include "DCVMTargetMachine.h"
#include "llvm/CodeGen/SelectionDAGISel.h"

using namespace llvm;

#define DEBUG_TYPE "dcvm-isel"
#define PASS_NAME "DCVM DAG->DAG Pattern Instruction Selection"

namespace {
class DCVMDAGToDAGISel : public SelectionDAGISel {
    const DCVMSubtarget *Subtarget = nullptr;

public:
    DCVMDAGToDAGISel() = delete;

    explicit DCVMDAGToDAGISel(DCVMTargetMachine &TM) : SelectionDAGISel(TM) {}

    bool runOnMachineFunction(MachineFunction &MF) override {
        Subtarget = &MF.getSubtarget<DCVMSubtarget>();
        return SelectionDAGISel::runOnMachineFunction(MF);
    }

    void Select(SDNode *N) override;

#include "DCVMGenDAGISel.inc"
};

class DCVMDAGToDAGISelLegacy : public SelectionDAGISelLegacy {
public:
    static char ID;
    explicit DCVMDAGToDAGISelLegacy(DCVMTargetMachine &TM)
        : SelectionDAGISelLegacy(ID, std::make_unique<DCVMDAGToDAGISel>(TM)) {}
};
} // namespace

char DCVMDAGToDAGISelLegacy::ID = 0;

INITIALIZE_PASS(DCVMDAGToDAGISelLegacy, DEBUG_TYPE, PASS_NAME, false, false)

void DCVMDAGToDAGISel::Select(SDNode *N) {
    if (N->isMachineOpcode()) {
        N->setNodeId(-1);
        return;
    }

    SelectCode(N);
}

FunctionPass *llvm::createDCVMISelDag(DCVMTargetMachine &TM) {
    return new DCVMDAGToDAGISelLegacy(TM);
}
