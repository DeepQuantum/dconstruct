#include <gtest/gtest.h>
#include "binaryfile.h"
#include "disassembly/file_disassembler.h"
#include "decompilation/decomp_function.h"
#include <fstream>

TEST(SANITY, Basic) {
    EXPECT_STRNE("0", "1");
}

namespace dconstruct::testing {

    static SIDBase base = *SIDBase::from_binary(R"(C:\Users\damix\Documents\GitHub\TLOU2Modding\dconstruct\test\uc4\sidbase_sorted.bin)");


    TEST(DISASSEMBLER, NonExistingFile) {
        const std::string filepath = "dc_test_files/not_found.bin";
        const std::string crash_msg = "coudln't open \"" + filepath + "\"\n";
        auto not_found = BinaryFile<>::from_path(filepath);
        ASSERT_FALSE(not_found.has_value());
        ASSERT_EQ(not_found.error(), crash_msg);
    }

    TEST(DISASSEMBLER, EmptyFile) {
        const std::string filepath = "../test/dc_test_files/empty.bin";
        auto not_found = BinaryFile<>::from_path(filepath);
        ASSERT_FALSE(not_found.has_value());
        const std::string crash_msg = filepath + " is empty.\n";
        ASSERT_EQ(not_found.error(), crash_msg);
    }

    TEST(DISASSEMBLER, NonDCFileMagic) {
        const std::string filepath = "../test/dc_test_files/random.bin";
        auto not_found = BinaryFile<>::from_path(filepath);
        ASSERT_FALSE(not_found.has_value());
        const std::string crash_msg = "not a DC-file. magic number doesn't equal 0x44433030:";
        ASSERT_EQ(not_found.error(), crash_msg);
    }

    TEST(DISASSEMBLER, NonDCFileVersion) {
        const std::string filepath = "../test/dc_test_files/random2.bin";
        auto not_found = BinaryFile<>::from_path(filepath);
        ASSERT_FALSE(not_found.has_value());
        const std::string crash_msg = "error: not a DC-file. version number doesn't equal 0x00000001: ";
        ASSERT_EQ(not_found.error(), crash_msg);
    }

    TEST(DISASSEMBLER, UC4Funcs) {
        const std::string filepath = "C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/test/uc4/ss-isl-cave-get-piton.bin";
        const std::string graph = "C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/test/uc4/graph.svg";
        auto file_res = BinaryFile<true>::from_path(filepath);
        ASSERT_TRUE(file_res.has_value());
        auto& file = *file_res;
        dconstruct::FileDisassembler<true> dis{&file, &base, "C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/test/uc4/ss-isl-cave-get-piton.asm", {}};

        dis.disassemble_functions_from_bin_file();

        dis.dump();
        std::ofstream out("C:/Users/damix/Documents/GitHub/TLOU2Modding/dconstruct/test/uc4/ss-isl-cave-get-piton.dcpl");

        for (const auto& func : dis.get_functions()) {
            auto decomp = dconstruct::dcompiler::decomp_function{func, file, ControlFlowGraph::build(func)};
            out << decomp.m_functionDefinition << "\n\n";
        }

        ASSERT_GT(dis.get_functions().size(), 0);
    }
}

