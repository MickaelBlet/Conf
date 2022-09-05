#include <gtest/gtest.h>

#include "mblet/configator.hpp"

GTEST_TEST(constructor, success) {
    // create example string
    std::string strConf("[unused]\n" \
                        "[test]\n" \
                        "42[42]=42");

    mblet::Configator conf;
    conf.readString(strConf);

    mblet::Configator conf2(conf);
    mblet::Configator conf3;
    conf3 = conf2;
    EXPECT_EQ(conf.getConfig(), conf2.getConfig());
    EXPECT_EQ(conf2.getConfig(), conf3.getConfig());
}