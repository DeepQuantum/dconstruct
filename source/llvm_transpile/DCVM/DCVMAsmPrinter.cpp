#include "base.h"
#include "DCVM.h"
#include "DCVMMCInstLower.h"
#include "DCVMMachineFunctionInfo.h"
#include "MCTargetDesc/DCVMInstPrinter.h"
#include "MCTargetDesc/DCVMMCTargetDesc.h"
#include "TargetInfo/DCVMTargetInfo.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Metadata.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Compiler.h"
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using namespace llvm;

#define DEBUG_TYPE "asm-printer"

static std::vector<char> g_byteCode;

static void appendU64LE(std::vector<char>& Out, uint64_t Value) {
    for (unsigned i = 0; i < 8; ++i)
        Out.push_back(static_cast<char>((Value >> (i * 8)) & 0xff));
}

namespace {
    class DCVMAsmPrinter : public AsmPrinter {
        std::unique_ptr<MCInstrInfo> m_instrInfo;
        std::unique_ptr<MCCodeEmitter> m_codeEmitter;
        std::vector<char> m_funcBytes;
        std::map<const MCSymbol*, uint64_t> m_blockIndex;
        std::vector<std::pair<size_t, const MCSymbol*>> m_branchFixups;

    public:
        explicit DCVMAsmPrinter(TargetMachine& TM, std::unique_ptr<MCStreamer> Streamer) : AsmPrinter(TM, std::move(Streamer), ID) {}

        StringRef getPassName() const override { return "DCVM Assembly Printer"; }

        void emitBasicBlockStart(const MachineBasicBlock& MBB) override;
        void emitInstruction(const MachineInstr* MI) override;
        void emitFunctionBodyEnd() override;

        static const char* getRegisterName(MCRegister Reg) { return DCVMInstPrinter::getRegisterName(Reg); }

        static char ID;
    };
} // namespace

char DCVMAsmPrinter::ID = 0;

void DCVMAsmPrinter::emitBasicBlockStart(const MachineBasicBlock& MBB) {
    m_blockIndex[MBB.getSymbol()] = m_funcBytes.size() / 8;
    AsmPrinter::emitBasicBlockStart(MBB);
}

void DCVMAsmPrinter::emitInstruction(const MachineInstr* MI) {
    DCVMMCInstLower MCInstLowering(OutContext, *this);
    MCInst TmpInst;
    MCInstLowering.Lower(MI, TmpInst);
    EmitToStreamer(*OutStreamer, TmpInst);

    if (!m_codeEmitter) {
        m_instrInfo.reset(TM.getTarget().createMCInstrInfo());
        m_codeEmitter.reset(createDCVMMCCodeEmitter(*m_instrInfo, OutContext));
    }

    const size_t WordStart = m_funcBytes.size();

    SmallVector<MCFixup, 2> Fixups;
    SmallVector<char, 16> Encoded;
    m_codeEmitter->encodeInstruction(TmpInst, Encoded, Fixups, MF->getSubtarget());
    m_funcBytes.insert(m_funcBytes.end(), Encoded.begin(), Encoded.end());

    for (const MachineOperand& MO : MI->operands())
        if (MO.isMBB()) {
            m_branchFixups.emplace_back(WordStart, MO.getMBB()->getSymbol());
            break;
        }
}

void DCVMAsmPrinter::emitFunctionBodyEnd() {
    const ArrayRef<uint64_t> Symbols = MF->getInfo<DCVMMachineFunctionInfo>()->getSymbols();

    const ArrayRef<std::byte> PointerMap = MF->getInfo<DCVMMachineFunctionInfo>()->getPointerMap();

    if (!Symbols.empty()) {
        MCSymbol* TableSym = OutContext.getOrCreateSymbol(MF->getName() + "_symbol_table");
        OutStreamer->emitLabel(TableSym);
        for (const uint64_t Value : Symbols)
            OutStreamer->emitInt64(Value);
        MCSymbol* PointerMapSym = OutContext.getOrCreateSymbol(MF->getName() + "_pointer_map");
        OutStreamer->emitLabel(PointerMapSym);
        for (const std::byte Value : PointerMap)
            OutStreamer->emitInt8(static_cast<std::uint8_t>(Value));
    }

    for (const auto& [WordStart, Target] : m_branchFixups) {
        const uint64_t Index = m_blockIndex[Target];
        m_funcBytes[WordStart + 1] = static_cast<char>(Index & 0xff);
        m_funcBytes[WordStart + 3] = static_cast<char>((Index >> 8) & 0xff);
    }

    const uint32_t InstrBytes = static_cast<uint32_t>(m_funcBytes.size());
    const uint32_t SymbolBytes = static_cast<uint32_t>(Symbols.size() * 8);

    for (const uint64_t Value : Symbols)
        appendU64LE(m_funcBytes, Value);
    for (const std::byte Value : PointerMap)
        m_funcBytes.push_back(static_cast<uint8_t>(Value));

    const Function& F = MF->getFunction();
    const std::string Name = F.getName().str();
    const uint64_t Sid = SID(Name.c_str());

    appendU64LE(g_byteCode, Sid);
    appendU64LE(g_byteCode, static_cast<uint64_t>(InstrBytes) | (static_cast<uint64_t>(SymbolBytes) << 32));
    g_byteCode.insert(g_byteCode.end(), m_funcBytes.begin(), m_funcBytes.end());
    m_funcBytes.clear();
    m_blockIndex.clear();
    m_branchFixups.clear();
}

extern "C" const char* DCVMBytecodeData() {
    return g_byteCode.data();
}

extern "C" size_t DCVMBytecodeSize() {
    return g_byteCode.size();
}

extern "C" void DCVMBytecodeReset() {
    g_byteCode.clear();
}

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void LLVMInitializeDCVMAsmPrinter() {
    RegisterAsmPrinter<DCVMAsmPrinter> X(getTheDCVMTarget());
}
