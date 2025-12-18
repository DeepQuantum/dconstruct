#include "disassembler.h"
#include <fstream>


namespace dconstruct {
    template<bool is_64_bit>
    class FileDisassembler : public Disassembler<is_64_bit> {

    public:
        FileDisassembler(BinaryFile<is_64_bit>* file, const SIDBase* sidbase, const std::string& out_file, const DisassemblerOptions& options) noexcept : Disassembler<is_64_bit>(file, sidbase) {
            m_outbuf.reserve(0x2FFFFFULL);
            m_outfptr = fopen(out_file.c_str(), "wb");
            this->m_options = options;
        }

        void dump() const noexcept {
            fwrite(m_outbuf.c_str(), sizeof(char), m_outbuf.length(), m_outfptr);
        }

        ~FileDisassembler() noexcept override {
            fclose(m_outfptr);
        }

        template<typename T>
        [[nodiscard]] const T* get_value_ptr_at(const u64 offset) const noexcept {
            return reinterpret_cast<const T*>(&this->m_currentFile->m_bytes[offset]);
        }

    private:
        std::string m_outbuf;
        FILE* m_outfptr;

        void insert_span(const char* text, const u32 indent = 0, const TextFormat& text_format = TextFormat{}) override {
            if (indent > 0) {
                m_outbuf += std::string(indent, ' ');
                #ifdef _DEBUG
                //printf("%*s", (i32)indent, "");
                #endif
            }
            m_outbuf += text;
            #ifdef _DEBUG
            //printf("%s", text);
            #endif
        }
    };
}