#include <gtest/gtest.h>

#include "blet/conf.h"
#include "mock/fileGuard.h"

GTEST_TEST(loadFile, except_open_file) {
    const char* testFile = "/tmp/blet_test_loadFile_except_open_file.conf";
    {
        // create example file
        test::blet::FileGuard fileGuard(testFile, std::ofstream::out | std::ofstream::trunc);
    } // remove file

    EXPECT_THROW(
        {
            try {
                blet::Dict conf = blet::conf::loadFile(testFile);
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Parse /tmp/blet_test_loadFile_except_open_file.conf: (Open file failed)");
                EXPECT_EQ(e.message(), "Open file failed");
                EXPECT_EQ(e.filename(), "/tmp/blet_test_loadFile_except_open_file.conf");
                EXPECT_EQ(e.line(), 0);
                EXPECT_EQ(e.column(), 0);
                throw;
            }
        },
        blet::conf::LoadException);
}

GTEST_TEST(loadFile, except_parsing) {
    // clang-format off
    std::string confStr = ""
        "[section]\n"
        "=";
    // clang-format on

    // create example file
    const char* testFile = "/tmp/blet_test_loadFile_except_parsing.conf";
    test::blet::FileGuard fileGuard(testFile, std::ofstream::out | std::ofstream::trunc);
    fileGuard << confStr << std::flush;
    fileGuard.close();

    EXPECT_THROW(
        {
            try {
                blet::Dict conf = blet::conf::loadFile(testFile);
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Parse at /tmp/blet_test_loadFile_except_parsing.conf:2:1 (Key not found)");
                EXPECT_EQ(e.message(), "Key not found");
                EXPECT_EQ(e.filename(), "/tmp/blet_test_loadFile_except_parsing.conf");
                EXPECT_EQ(e.line(), 2);
                EXPECT_EQ(e.column(), 1);
                throw;
            }
        },
        blet::conf::LoadException);
}
