#include <gtest/gtest.h>

#include "configator.hpp"

GTEST_TEST(setConfig, success) {
    // create example string
    std::string strConf("[unused]\n" \
                        "[test]\n" \
                        "42[42]=42");

    mblet::Configator conf;
    conf.readString(strConf);
    EXPECT_EQ(conf.getConfig().size(), 2); // two sections
    EXPECT_EQ(conf["test"]["42"]["42"].value, "42");
    // clean configuration map
    mblet::Configator::Map newMap;
    conf.setConfig(newMap);
    EXPECT_EQ(conf.getConfig().size(), 0);
}