#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstring>

#include "configator.hpp"

using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

GTEST_TEST(Configator, get)
{
    using namespace mblet;

    struct TestConfig
    {
        std::string     valueString;
        bool            valueBool;
        int             valueInt;
        long            valueLong;
        unsigned long   valueULong;
        double          valueDouble;
    } testConfig;

    Configator::Map map;
    map[""]["42"]="-42.42";
    Configator conf;
    conf.setConfig(map);

    // pointers
    EXPECT_EQ(conf[""]["42"].get<std::string>(), "-42.42");
    EXPECT_EQ(conf[""]["42"].get<bool>(), true);
    EXPECT_EQ(conf[""]["42"].get<int>(), -42);
    EXPECT_EQ(conf[""]["42"].get<long>(), -42);
    EXPECT_EQ(conf[""]["42"].get<unsigned long>(), static_cast<unsigned long>(-42));
    EXPECT_EQ(conf[""]["42"].get<double>(), -42.42);
}