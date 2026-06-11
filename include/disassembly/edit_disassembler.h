#pragma once
#include "binaryfile.h"
#include "disassembler.h"
#include "DCHeader.h"
#include <memory>

namespace dconstruct {
    enum class EditType {
        INT32,
        INT64,
        F32,
        SID_STR,
        SID_HASH,
        PTR
    };

    struct BinaryFileEdit {
        const EditType m_editType;
        union {
            i32 I32;
            u32 U32;
            u64 U64;
            f32 F32;
            void* ptr;
            const std::string* string = nullptr;
        };
    };

    class EditDisassembler : public Disassembler {
    public:
        EditDisassembler(BinaryFile* file, const SIDBase* sidbase, const std::vector<std::string>& edits, const game_type game = game_type::T2R) noexcept : Disassembler(file, sidbase, game) {
            m_edits = edits;
        }
        ~EditDisassembler() override = default;

        error_msg apply_file_edits() noexcept;
        error_msg apply_edit(const u64 offset, const u32 member_index, const BinaryFileEdit& value) noexcept;
        [[nodiscard]] BinaryFileEdit get_edit_value_from_string(const std::string& str_value) const noexcept;
        error_msg write_edited_file();
        void expand_binary_file_string_table(BinaryFile& file, const std::string& new_string);

    private:
        std::vector<std::string> m_edits;
    };
}
