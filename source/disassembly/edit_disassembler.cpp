#include "edit_disassembler.h"
#include <cctype>
#include <iostream>

//-e 0xE98[0]=test
//-e 0xE43[0]=#653463242342
//-e 0xE97[53]=1.54
//-e 0xE97[2]=2
//-e 0xE97[1]=0xE234

namespace dconstruct {
    [[nodiscard]] BinaryFileEdit EditDisassembler::get_edit_value_from_string(const std::string& str_value) const noexcept {
        if (str_value.find(".") != std::string::npos) {
            return {
                .m_editType = EditType::F4,
                .f32 = std::stof(str_value.c_str())
            };
        }
        else if (std::isalpha(str_value[0])) {
            return {
                .m_editType = EditType::SID_STR,
                .string = &str_value
            };
        }
        else if (str_value[0] == '#') {
            return {
                .m_editType = EditType::SID_HASH,
                .u64 = std::stoull(str_value.substr(1), nullptr, 16)
            };
        }
        else if (str_value[0] == '0' && str_value[1] == 'x') {
            return {
                .m_editType = EditType::PTR,
                .u64 = std::stoull(str_value, nullptr, 0) + reinterpret_cast<p64>(m_currentFile->m_bytes.get())
            };
        }
        else {
            return {
                .m_editType = EditType::INT4,
                .i32 = std::stol(str_value)
            };
        }
    }

    void EditDisassembler::apply_edit(const u64 struct_offset, const u32 member_index, const BinaryFileEdit& value) noexcept {
        const location struct_member_start = location(m_currentFile->m_bytes.get()) + struct_offset;
        u32 member_location = 0;
        u32 last_member_size = 0;
        for (u32 i = 0; i < member_index; ++i) {
            last_member_size = insert_next_struct_member(struct_member_start + member_location, 0);
            member_location += last_member_size;
        }
        const u32 edit_member_size = insert_next_struct_member(struct_member_start + member_location, 0);
        if (edit_member_size == 8 && (value.m_editType != EditType::SID_STR && value.m_editType != EditType::SID_HASH && value.m_editType != EditType::PTR)) {
            std::cout << "warning: member " << member_index << " of struct at location 0x" << std::hex << struct_offset
                << " is size 8, but value passed is of size 4. edit will not be applied.\n";
            return;
        }
        if (edit_member_size == 4 && (value.m_editType == EditType::SID_STR || value.m_editType == EditType::SID_HASH || value.m_editType == EditType::PTR)) {
            std::cout << "warning: member " << member_index << " of struct at location 0x" << std::hex << struct_offset
                << " is size 4, but value passed is of size 8. edit will not be applied.\n";
            return;
        }
        const location edit_location = struct_member_start + member_location;
        std::cout << "applying change at location 0x" << std::hex << struct_offset << "[0x" << member_index << "]: " << std::dec;
        switch (value.m_editType) {
            case EditType::INT4: {
                std::cout << edit_location.get<i32>() << "->" << value.i32 << '\n';
                *reinterpret_cast<i32*>(const_cast<std::byte*>(edit_location.m_ptr)) = value.i32;
                break;
            }
            case EditType::INT8: {
                std::cout << edit_location.get<u64>() << "->" << value.u64 << '\n';
                *reinterpret_cast<u64*>(const_cast<std::byte*>(edit_location.m_ptr)) = value.u64;
                break;
            }
            case EditType::F4: {
                std::cout << edit_location.get<f32>() << "->" << value.f32 << '\n';
                *reinterpret_cast<f32*>(const_cast<std::byte*>(edit_location.m_ptr)) = value.f32;
                break;
            }
            case EditType::SID_STR: {
                const char* old_sid = lookup(edit_location.get<sid64>());
                const u64 new_sid = SID(value.string->c_str());
                if (m_sidbase->search(new_sid) != nullptr) {
                    std::cout << old_sid << "->" << *value.string << '\n';
                    *reinterpret_cast<sid64*>(const_cast<std::byte*>(edit_location.m_ptr)) = new_sid;
                }
                else {
                    std::cout << "warning: the sid '" << value.string << "' does not exist within the sidbase, so the edit will not be applied. " \
                        "if this is intentional, please use the numerical hash instead (#" << std::hex << std::setfill('0') << std::setw(16) << ").";
                }
                break;
            }
            case EditType::SID_HASH: {
                const char* old_sid = lookup(edit_location.get<sid64>());
                const std::string new_sid = int_to_string_id(value.u64);
                std::cout << old_sid << '=' << new_sid << '\n';
                *reinterpret_cast<sid64*>(const_cast<std::byte*>(edit_location.m_ptr)) = value.u64;
                break;
            }
            case EditType::PTR: {
                std::cout << edit_location.get<u64>() - reinterpret_cast<p64>(m_currentFile->m_bytes.get()) << '=' << value.u64 - reinterpret_cast<p64>(m_currentFile->m_bytes.get()) << '\n';
                *reinterpret_cast<u64*>(const_cast<std::byte*>(edit_location.m_ptr)) = value.u64;
                break;
            }
        }
    }

    void EditDisassembler::apply_file_edits() noexcept {
        b8 applied_at_least_one = false;
        u16 edit_index = 0;
        for (const auto& edit_str : m_edits) {
            ++edit_index;
            const u64 equal_offset = edit_str.rfind('=');
            const u64 left_sqbr_offset = edit_str.rfind('[');
            if (equal_offset == std::string::npos || left_sqbr_offset == std::string::npos) {
                std::cout << "error: malformed input in edit " << edit_index << ", not applying\n";
                continue;
            }

            const u64 struct_location = std::stoi(edit_str.substr(0, left_sqbr_offset), nullptr, 0);
            const u32 right_sqbr_offset = edit_str.find(']', left_sqbr_offset);
            if (right_sqbr_offset == std::string::npos) {
                std::cout << "error: malformed input in edit " << edit_index << ", not applying\n";
                continue;
            }

            const std::string member_string = edit_str.substr(left_sqbr_offset + 1, right_sqbr_offset - left_sqbr_offset - 1);
            const u64 member_index = std::stoi(member_string, nullptr, 0);

            const std::string str_value = edit_str.substr(equal_offset + 1, edit_str.length() - equal_offset);

            const BinaryFileEdit edit = get_edit_value_from_string(str_value);

            apply_edit(struct_location, member_index, edit);
            applied_at_least_one = true;
        }
        if (applied_at_least_one) {
            complete();
        }
    }

    void EditDisassembler::complete() {
        const std::filesystem::path edited_file_path = m_currentFile->m_path.parent_path() / (m_currentFile->m_path.stem().string() + "_edited.bin");
        std::cout << "creating edited file: " << edited_file_path << '\n';
        std::unique_ptr<std::byte[]> unmapped_bytes = m_currentFile->get_unmapped();
        FILE *out = fopen(edited_file_path.string().c_str(), "wb");
        fwrite(unmapped_bytes.get(), sizeof(unmapped_bytes[0]), m_currentFile->m_size, out);
    }
}

