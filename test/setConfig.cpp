#include <gtest/gtest.h>
#include <stdint.h>

#include "configator.hpp"

GTEST_TEST(setConfig, success) {
    const char* testFile = "test.ini";
    // create example file
    std::ofstream fileStream(testFile, std::ofstream::out | std::ofstream::trunc);
    fileStream << "[test]\n"
               << "42[42]=42";
    fileStream.close();
    mblet::Configator conf;
    EXPECT_EQ(conf.readFile(testFile), true);
    // remove example file
    remove(testFile);
    EXPECT_EQ(conf.isRead(), true);
    EXPECT_EQ(conf.getConfig().size(), 1);
    EXPECT_EQ(conf["test"]["42"]["42"].value, "42");
    // clean configuration map
    mblet::Configator::Map newMap;
    conf.setConfig(newMap);
    EXPECT_EQ(conf.getConfig().size(), 0);
}