#include <gtest/gtest.h>
#include "binaryfile.h"
#include "disassembly/file_disassembler.h"
#include "decompilation/decomp_function.h"
#include <fstream>

TEST(SANITY, Basic) {
    EXPECT_STRNE("0", "1");
}

namespace dconstruct::testing {

    static SIDBase base = SIDBase::from_binary(R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\uc4\sidbase_sorted.bin)");


    TEST(DISASSEMBLER, NonExistingFile) {
        const std::string filepath = "dc_test_files/not_found.bin";
        const std::string crash_msg = "error: coudln't open \"" + filepath + "\"\n"; 
        EXPECT_DEATH(dconstruct::BinaryFile<i64> not_found{filepath}, crash_msg);
    }

    TEST(DISASSEMBLER, EmptyFile) {
        const std::string filepath = "../test/dc_test_files/empty.bin";
        const std::string crash_msg = "error: " + filepath + " is empty.\n";
        EXPECT_DEATH(dconstruct::BinaryFile<i64>{filepath}, crash_msg);
    }

    TEST(DISASSEMBLER, NonDCFileMagic) {
        const std::string filepath = "../test/dc_test_files/random.bin";
        const std::string crash_msg = "error: not a DC-file. magic number doesn't equal 0x44433030:";
        EXPECT_DEATH(dconstruct::BinaryFile<i64> {filepath}, crash_msg);
    }

    TEST(DISASSEMBLER, NonDCFileVersion) {
        const std::string filepath = "../test/dc_test_files/random2.bin";
        const std::string crash_msg = "error: not a DC-file. version number doesn't equal 0x00000001: ";
        EXPECT_DEATH(dconstruct::BinaryFile<i64>{ filepath }, crash_msg);
    }

    TEST(DISASSEMBLER, UC4Funcs) {
        const std::string filepath = "C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/test/uc4/ss-isl-cave-get-piton.bin";
        const std::string graph = "C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/test/uc4/graph.svg";
        BinaryFile<i32> file(filepath);
        dconstruct::FileDisassembler dis{&file, &base, "C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/test/uc4/ss-isl-cave-get-piton.asm", {}};

        dis.disassemble_functions_from_bin_file();

        auto decomp = dconstruct::dcompiler::decomp_function{dis.get_functions()[0], file, graph};
        std::ofstream out("C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/test/uc4/ss-isl-cave-get-piton.dcpl");
        out << decomp.to_string();

        ASSERT_GT(dis.get_functions().size(), 0);
    }
}

