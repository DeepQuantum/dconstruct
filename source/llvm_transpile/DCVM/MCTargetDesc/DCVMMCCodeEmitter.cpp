#include "DCVMMCTargetDesc.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"

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
    if (MO.isReg())
        return Ctx.getRegisterInfo()->getEncodingValue(MO.getReg());
    if (MO.isImm())
        return static_cast<unsigned>(MO.getImm());
    if (MO.isExpr())
        return 0;
    llvm_unreachable("unsupported operand in DCVMMCCodeEmitter");
}

void DCVMMCCodeEmitter::encodeInstruction(const MCInst &MI,
                                          SmallVectorImpl<char> &CB,
                                          SmallVectorImpl<MCFixup> &Fixups,
                                          const MCSubtargetInfo &STI) const {
    const uint64_t Bits = getBinaryCodeForInstr(MI, Fixups, STI);
    for (unsigned i = 0; i < 8; ++i)
        CB.push_back(static_cast<char>((Bits >> (i * 8)) & 0xff));
}

#include "DCVMGenMCCodeEmitter.inc"

MCCodeEmitter *llvm::createDCVMMCCodeEmitter(const MCInstrInfo &MCII,
                                             MCContext &Ctx) {
    return new DCVMMCCodeEmitter(MCII, Ctx);
}
