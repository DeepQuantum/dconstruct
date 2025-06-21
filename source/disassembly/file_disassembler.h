#include "disassembler.h"
#include <fstream>



class FileDisassembler : public Disassembler {

public:
    FileDisassembler(BinaryFile* file, const SIDBase* sidbase, const std::string &out_file, const DisassemblerOptions &options) {
        m_currentFile = file;
        m_sidbase = sidbase;
        m_outbuf.reserve(0x2FFFFFULL);
        m_outfptr = fopen(out_file.c_str(), "wb");
        m_options = options;
    }

private:
    std::string m_outbuf;
    FILE *m_outfptr;

    void insert_span(const char *text, const TextFormat &text_format = TextFormat{}, const u64 indent = 0) override {
        if (indent > 0) {
            m_outbuf += std::string(indent, ' ');
        }
        m_outbuf += text;
        return;
    }

    void complete() override {
        fwrite(m_outbuf.c_str(), sizeof(char), m_outbuf.length(), m_outfptr);
        fclose(m_outfptr);
    }
};