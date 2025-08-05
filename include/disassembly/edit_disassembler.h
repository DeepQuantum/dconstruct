#pragma once
#include "disassembler.h"

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
        EditDisassembler(BinaryFile* file, const SIDBase* sidbase, const DisassemblerOptions& options, const std::vector<std::string> &edits) {
            m_currentFile = file;
            m_sidbase = sidbase;
            m_options = options;
            m_edits = edits;
        }
        ~EditDisassembler() override = default;

        void apply_file_edits() noexcept;
        void apply_edit(const u64 offset, const u32 member_index, const BinaryFileEdit& value) noexcept;
        [[nodiscard]] BinaryFileEdit get_edit_value_from_string(const std::string& str_value) const noexcept;

    private:
        std::vector<std::string> m_edits;

        void insert_span(const char* text, const u32 indent = 0, const TextFormat& text_format = TextFormat{}) override {};
        void output_edit_file();
    };
}
