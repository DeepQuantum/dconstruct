#include "disassembler.h"

namespace dconstruct {
    enum EditType {
        INT4,
        INT8,
        F4,
        SID_STR,
        SID_HASH,
        PTR
    };

    struct BinaryFileEdit {
        const EditType m_editType;
        union {
            i32 i32;
            u32 u32;
            u64 u64;
            f32 f32;
            void* ptr;
            const std::string* string = nullptr;
        };
    };

    class EditDisassembler : public Disassembler {

    public:
        EditDisassembler(BinaryFile* file, const SIDBase* sidbase, const DisassemblerOptions& options, const std::vector<std::string>& edits) {
            m_currentFile = file;
            m_sidbase = sidbase;
            m_options = options;
            m_edits = edits;
        }

        void apply_file_edits() noexcept;
        void apply_edit(const u64 offset, const u32 member_index, const BinaryFileEdit& value) noexcept;
        [[nodiscard]] BinaryFileEdit get_edit_value_from_string(const std::string& str_value) const noexcept;

    private:
        std::vector<std::string> m_edits;

        void insert_span(const char*, const TextFormat& = TextFormat{}, const u64 = 0) override {};
        void complete() override {};
    };
}
