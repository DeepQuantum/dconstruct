#include "decompiler.h"
#include "binaryfile.h"
#include "file_disassembler.h"
#include <gtest/gtest.h>
#include <filesystem>


namespace dconstruct::testing {

    
    static FileDisassembler get_test_disassembler(const std::filesystem::path &inpath) {
        SIDBase base{};
        base.load("test_sidbase.bin");
        BinaryFile file(inpath.string());
        if (!file.dc_setup()) {
            return;
        }

        FileDisassembler disassembler(&file, &base, "", DisassemblerOptions{});
        return disassembler;
    }


    TEST(DECOMPILER, BasicAdd) {
        FileDisassembler da = get_test_disassembler("dc_test_files/small.bin");
    }

}
