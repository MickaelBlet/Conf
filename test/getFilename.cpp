#include <gtest/gtest.h>
#include <stdint.h>

#include "configator.hpp"

GTEST_TEST(configurator_getFilename, success) {
    const char* testFile = "test.ini";
    // create example file
    std::ofstream fileStream(testFile, std::ofstream::out | std::ofstream::trunc);
    fileStream.close();
    mblet::Configator conf;
    EXPECT_EQ(conf.readFile(testFile), true);
    // remove example file
    remove(testFile);
    EXPECT_EQ(conf.getFilename(), "test.ini");
}

GTEST_TEST(configurator_getFilename, failure) {
    const char* testFile = "test.ini";
    mblet::Configator conf;
    EXPECT_EQ(conf.readFile(testFile), false);
    EXPECT_EQ(conf.getFilename(), "");
}