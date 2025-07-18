#include <gtest/gtest.h>
#include "binaryfile.h"

TEST(SANITY, Basic) {
    EXPECT_STRNE("0", "1");
}


TEST(DISASSEMBLER, NonExistingFile) {
    const std::string filepath = "dc_test_files/not_found.bin";
    const std::string crash_msg = "error: coudln't open \"" + filepath + "\"\n"; 
    EXPECT_DEATH(dconstruct::BinaryFile not_found{filepath}, crash_msg);
}

TEST(DISASSEMBLER, EmptyFile) {
    const std::string filepath = "../test/dc_test_files/empty.bin";
    const std::string crash_msg = "error: " + filepath + " is empty.\n";
    dconstruct::BinaryFile empty{filepath};

    testing::internal::CaptureStderr();

    b8 is_dc_file = empty.dc_setup();

    const std::string output = testing::internal::GetCapturedStderr();
    
    EXPECT_FALSE(is_dc_file);
    EXPECT_EQ(crash_msg, output);
}

TEST(DISASSEMBLER, NonDCFileMagic) {
    const std::string filepath = "../test/dc_test_files/random.bin";
    const std::string crash_msg = "error: not a DC-file. magic number doesn't equal 0x44433030:";
    dconstruct::BinaryFile random{filepath};

    testing::internal::CaptureStderr();

    b8 is_dc_file = random.dc_setup();

    const std::string output = testing::internal::GetCapturedStderr().substr(0, crash_msg.size());
    
    EXPECT_FALSE(is_dc_file);
    EXPECT_EQ(crash_msg, output);
}

TEST(DISASSEMBLER, NonDCFileVersion) {
    const std::string filepath = "../test/dc_test_files/random2.bin";
    const std::string crash_msg = "error: not a DC-file. version number doesn't equal 0x00000001: ";
    dconstruct::BinaryFile random{filepath};

    testing::internal::CaptureStderr();

    const b8 is_dc_file = random.dc_setup();

    const std::string output = testing::internal::GetCapturedStderr().substr(0, crash_msg.size());
    
    EXPECT_FALSE(is_dc_file);
    EXPECT_EQ(crash_msg, output);
}