#pragma once
#include "disassembler.h"
#include "DCHeader.h"

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

        // string editing: 
        // when editing a string, drop-down menu of existing string table appears. if user selects a pre-existing string, we simply change the pointer through existing pointer edit modes.
        // in any other case, even if just shortening the string, we make append a new string at the end of the string table and move down the reloc table.
        // requires: adjusting header reloc & data size, as well as expanding reloc table with size & bytes

        [[nodiscard]] std::optional<std::string> make_stringtable_expanded_binary_file(BinaryFile& old, const std::string& new_string) {

            const u64 old_reloc_table_size = old.m_dcheader->m_stringsOffset - old.m_dcheader->m_textSize;
            std::unique_ptr<std::byte[]> new_reloc_table = std::make_unique<std::byte[]>(old_reloc_table_size + new_string.size());
            std::memcpy(new_reloc_table.get(), old.m_bytes.get() + old.m_dcheader->m_textSize, old_reloc_table_size);

            std::memcpy(old.m_bytes.get() + old.m_dcheader->m_textSize, new_string.data(), new_string.size());
            old.m_dcheader->m_textSize += new_string.size();
            
        }

    private:
        std::vector<std::string> m_edits;
    };
}
