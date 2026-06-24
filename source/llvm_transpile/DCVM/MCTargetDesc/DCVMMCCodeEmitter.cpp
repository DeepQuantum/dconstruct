#include "DCVMMCTargetDesc.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"

using namespace llvm;

namespace {
class DCVMMCCodeEmitter : public MCCodeEmitter {
    const MCInstrInfo &MCII;
    MCContext &Ctx;

public:
    DCVMMCCodeEmitter(const MCInstrInfo &MCII, MCContext &Ctx)
        : MCII(MCII), Ctx(Ctx) {}
    DCVMMCCodeEmitter(const DCVMMCCodeEmitter &) = delete;
    void operator=(const DCVMMCCodeEmitter &) = delete;
    ~DCVMMCCodeEmitter() override = default;

    uint64_t getBinaryCodeForInstr(const MCInst &MI,
                                   SmallVectorImpl<MCFixup> &Fixups,
                                   const MCSubtargetInfo &STI) const;

    unsigned getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                               SmallVectorImpl<MCFixup> &Fixups,
                               const MCSubtargetInfo &STI) const;

    void encodeInstruction(const MCInst &MI, SmallVectorImpl<char> &CB,
                           SmallVectorImpl<MCFixup> &Fixups,
                           const MCSubtargetInfo &STI) const override;
};
} // namespace

unsigned DCVMMCCodeEmitter::getMachineOpValue(const MCInst &MI,
                                              const MCOperand &MO,
                                              SmallVectorImpl<MCFixup> &Fixups,
                                              const MCSubtargetInfo &STI) const {
    return 0;
}

void DCVMMCCodeEmitter::encodeInstruction(const MCInst &MI,
                                          SmallVectorImpl<char> &CB,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {}

#include "DCVMGenMCCodeEmitter.inc"

MCCodeEmitter *llvm::createDCVMMCCodeEmitter(const MCInstrInfo &MCII,
                                             MCContext &Ctx) {
    return new DCVMMCCodeEmitter(MCII, Ctx);
}
