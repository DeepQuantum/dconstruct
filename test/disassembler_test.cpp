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
    EXPECT_DEATH(dconstruct::BinaryFile{filepath}, crash_msg);
}

TEST(DISASSEMBLER, NonDCFileMagic) {
    const std::string filepath = "../test/dc_test_files/random.bin";
    const std::string crash_msg = "error: not a DC-file. magic number doesn't equal 0x44433030:";
    EXPECT_DEATH(dconstruct::BinaryFile {filepath}, crash_msg);
}

TEST(DISASSEMBLER, NonDCFileVersion) {
    const std::string filepath = "../test/dc_test_files/random2.bin";
    const std::string crash_msg = "error: not a DC-file. version number doesn't equal 0x00000001: ";
    EXPECT_DEATH(dconstruct::BinaryFile{ filepath }, crash_msg);
}