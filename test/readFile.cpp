#include <gtest/gtest.h>
#include <stdint.h>

#include "configator.hpp"

GTEST_TEST(readFile, success) {
    const char* testFile = "test.ini";
    // create example file
    std::ofstream fileStream(testFile, std::ofstream::out | std::ofstream::trunc);
    fileStream.close();
    mblet::Configator conf;
    EXPECT_EQ(conf.readFile(testFile), true);
    // remove example file
    remove(testFile);
}

GTEST_TEST(readFile, failure) {
    const char* testFile = "test.ini";
    mblet::Configator conf;
    EXPECT_EQ(conf.readFile(testFile), false);
}