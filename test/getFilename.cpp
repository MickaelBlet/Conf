#include <gtest/gtest.h>
#include <stdint.h>

#include "configator.hpp"
#include "fileGuard.hpp"

GTEST_TEST(getFilename, success) {
    const char* testFile = "test.ini";
    // create example file
    FileGuard fileGuard(testFile, std::ofstream::out | std::ofstream::trunc);
    fileGuard.close();
    mblet::Configator conf;
    EXPECT_EQ(conf.readFile(testFile), true);
    EXPECT_EQ(conf.getFilename(), "test.ini");
}

GTEST_TEST(getFilename, failure) {
    const char* testFile = "test.ini";
    mblet::Configator conf;
    EXPECT_EQ(conf.readFile(testFile), false);
    EXPECT_EQ(conf.getFilename(), "");
}