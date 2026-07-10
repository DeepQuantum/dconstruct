#include "disassembly/instructions.h"
#include "string.h"
#include <format>
#include <sstream>

namespace dconstruct {

    [[nodiscard]] std::string state_script_function_id::to_string() const noexcept {
        const std::string idx_str = std::to_string(m_idx);
        const size_t total_size =
            m_state.m_name.size() + m_track.m_name.size() + m_event.m_name.size() + idx_str.size() + 3 * SEP.size();
        std::string result;
        result.reserve(total_size);
        result += m_state.m_name;
        result += SEP;
        result += m_event.m_name;
        result += SEP;
        result += m_track.m_name;
        result += SEP;
        result += idx_str;
        return result;
    }

    [[nodiscard]] std::string state_script_function_id::to_unique_string(const u64 original_offset) const noexcept {
        std::string result = to_string();
        result += SEP;
        result += std::format("{:#x}", original_offset);
        return result;
    }

    [[nodiscard]] std::string embedded_function_id::to_string() const noexcept {
        std::ostringstream os;
        os << m_entry;
        bool first = true;
        for (const auto& strct : m_outerStructs) {
            os << "__";
            os << std::hex << strct.first << "@" << strct.second;
            if (!first) {
                os << "__";
            }
            first = false;
        }
        return os.str();
    }
    void StackFrame::to_string(
        char* buffer,
        const u64 buffer_size,
        const u64 idx,
        const char* resolved
    ) const noexcept {
        const Register& reg = m_registers[idx];
        if (reg.m_value == Register::UNKNOWN_VAL) {
            std::snprintf(buffer, buffer_size, "?");
        } else if (reg.m_containsArg) {
            std::snprintf(buffer, buffer_size, "arg_%i", reg.m_argNum);
        } else if (std::holds_alternative<ast::primitive_type>(reg.m_type)) {
            switch (std::get<ast::primitive_type>(reg.m_type).m_type) {
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

    [[nodiscard]] const std::string& function_disassembly::get_id() const noexcept {
        if (!m_isScriptFunction) {
            return std::get<std::string>(m_id);
        } else {
            if (m_stateScriptId.empty()) {
                m_stateScriptId = std::get<state_script_function_id>(m_id).to_string();
            }
            return m_stateScriptId;
        }
    }

    [[nodiscard]] const std::string& function_disassembly::get_unique_id() const noexcept {
        if (!m_isScriptFunction) {
            return std::get<std::string>(m_id);
        } else {
            if (m_uniqueStateScriptId.empty()) {
                m_uniqueStateScriptId = std::get<state_script_function_id>(m_id).to_unique_string(m_originalOffset);
            }
            return m_uniqueStateScriptId;
        }
    }
}
