#include <gtest/gtest.h>
#include <stdint.h>

#include "configator.hpp"

GTEST_TEST(configurator_setConfig, success) {
    const char* testFile = "test.ini";
    // create example file
    std::ofstream fileStream(testFile, std::ofstream::out | std::ofstream::trunc);
    fileStream << "[test]\n"
               << "42[42]=42";
    fileStream.close();
    mblet::Configator conf;
    EXPECT_EQ(conf.readFile(testFile), true);
    remove(testFile);
    EXPECT_EQ(conf.isRead(), true);
}

GTEST_TEST(configurator_setConfig, failure) {
    const char* testFile = "test.ini";
    mblet::Configator conf;
    EXPECT_EQ(conf.readFile(testFile), false);
    EXPECT_EQ(conf.isRead(), false);
}