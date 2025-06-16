#include "disassembler.h"
#include <fstream>

class FileDisassembler : public Disassembler {

public:
    FileDisassembler(BinaryFile* file, const SIDBase* sidbase, const std::string &out_file) {
        this->m_currentFile = file;
        this->m_sidbase = sidbase;
        this->m_outbuf.reserve(0x2FFFFFULL);
        this->m_outfptr = fopen(out_file.c_str(), "wb");
        this->m_perfFile = fopen("./perffile.txt", "w");
    }

private:
    std::string m_outbuf;
    FILE *m_outfptr;

    void insert_span(const char *text, const TextFormat &text_format = TextFormat{}, const u64 indent = 0) override {
        if (indent != 0) {
            this->m_outbuf += std::string(indent, ' ');
        }
        this->m_outbuf += text;
        return;
    }

    void complete() override {
        fwrite(this->m_outbuf.c_str(), sizeof(char), m_outbuf.length(), this->m_outfptr);
        fclose(this->m_outfptr);
    }
};