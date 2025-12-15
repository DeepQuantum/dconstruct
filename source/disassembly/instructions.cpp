#include "disassembly/instructions.h"
#include "string.h"

namespace dconstruct {
    void StackFrame::to_string(char* buffer, const u64 buffer_size, const u64 idx, const char* resolved) const noexcept {
        const Register& reg = m_registers[idx];
        if (reg.m_value == Register::UNKNOWN_VAL) {
            std::snprintf(buffer, buffer_size, "?");
        }
        else if (reg.m_containsArg) {
            std::snprintf(buffer, buffer_size, "arg_%i", reg.m_argNum);
        } else if (std::holds_alternative<ast::primitive_type>(reg.m_type)) {
            switch(std::get<ast::primitive_type>(reg.m_type).m_type) {
                case ast::primitive_kind::SID: {
                    strncpy(buffer, resolved, buffer_size);
                    break;
                } 
                case ast::primitive_kind::STRING: {
                    std::snprintf(buffer, buffer_size, "\"%s\"", reinterpret_cast<const char*>(reg.m_value));
                    break;
                }         
                case ast::primitive_kind::F32: {
                    const f32 float_value = std::bit_cast<f32>(static_cast<u32>(reg.m_value));
                    std::snprintf(buffer, buffer_size, "%f", float_value);
                    break;
                }
                case ast::primitive_kind::I8:
                case ast::primitive_kind::I16:
                case ast::primitive_kind::I32:
                case ast::primitive_kind::I64: {
                    std::snprintf(buffer, buffer_size, "%lli", reg.m_value);
                    break;
                }
                default: {
                    std::snprintf(buffer, buffer_size, "%llu", reg.m_value);
                }
            }
        } else if (reg.is_pointer()) {
            std::snprintf(buffer, buffer_size, "[%s%s + %u]", reg.m_isReturn ? "RET_" : "", resolved, reg.m_pointerOffset);
        } else if (reg.m_isReturn) {
            std::snprintf(buffer, buffer_size, "RET_%s", resolved);
        } else {
            std::snprintf(buffer, buffer_size, "%s", resolved);
        }
    }

    Register& StackFrame::operator[](const u64 idx) noexcept {
        return m_registers[idx];
    }
}