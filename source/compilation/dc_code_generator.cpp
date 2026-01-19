#include "compilation/dc_code_generator.h"


namespace dconstruct::compiler { 

void dc_code_generator::emit_instruction(const Instruction& istr) noexcept {
    m_bytestream.push_back(static_cast<std::byte>(istr.opcode));
    m_bytestream.push_back(static_cast<std::byte>(istr.destination));
    m_bytestream.push_back(static_cast<std::byte>(istr.operand1));
    m_bytestream.push_back(static_cast<std::byte>(istr.operand2));
}

void dc_code_generator::emit_instruction(const Opcode opcode, const u8 destination, const u8 operand1, const u8 operand2) noexcept {
    m_bytestream.push_back(static_cast<std::byte>(opcode));
    m_bytestream.push_back(static_cast<std::byte>(destination));
    m_bytestream.push_back(static_cast<std::byte>(operand1));
    m_bytestream.push_back(static_cast<std::byte>(operand2));
}

}