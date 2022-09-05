#include <gtest/gtest.h>

#include "mblet/configator.hpp"
#include "fileGuard.hpp"

GTEST_TEST(readFile, success) {
    const char* testFile = "test.ini";
    // create example file
    FileGuard fileGuard(testFile, std::ofstream::out | std::ofstream::trunc);
    fileGuard.close();
    mblet::Configator conf;
    EXPECT_EQ(conf.readFile(testFile), true);
}

GTEST_TEST(readFile, failure) {
    const char* testFile = "test.ini";
    mblet::Configator conf;
    EXPECT_EQ(conf.readFile(testFile), false);
}