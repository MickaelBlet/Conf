#include <gtest/gtest.h>
#include <stdint.h>

#include "configator.hpp"
#include "fileGuard.hpp"

GTEST_TEST(isRead, success) {
    const char* testFile = "test.ini";
    // create example file
    FileGuard fileGuard(testFile, std::ofstream::out | std::ofstream::trunc);
    fileGuard.close();
    mblet::Configator conf;
    EXPECT_EQ(conf.readFile(testFile), true);
    EXPECT_EQ(conf.isRead(), true);
}

GTEST_TEST(isRead, failure) {
    const char* testFile = "test.ini";
    mblet::Configator conf;
    EXPECT_EQ(conf.readFile(testFile), false);
    EXPECT_EQ(conf.isRead(), false);
}