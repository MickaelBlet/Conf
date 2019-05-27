#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstring>

#include "configator.hpp"

using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

// GTEST_TEST(Configator_getValue, DISABLED_success_withDefaultValue)
GTEST_TEST(Configator_getValue, success_withDefaultValue)
{
    using namespace Config;

    struct TestConfig
    {
        std::string     valueString;
        bool            valueBool;
        int             valueInt;
        long            valueLong;
        unsigned long   valueULong;
        double          valueDouble;
    } testConfig;

    Configator conf({
        {"",
            {
                {"42", "-42.42"}
            }
        }
    });

    // pointers
    EXPECT_EQ(conf.getValue("", "42", &testConfig.valueString, "0"), true);
    EXPECT_EQ(testConfig.valueString, "-42.42");

    EXPECT_EQ(conf.getValue("", "42", &testConfig.valueBool, false), true);
    EXPECT_EQ(testConfig.valueBool, true);

    EXPECT_EQ(conf.getValue("", "42", &testConfig.valueInt, 0), true);
    EXPECT_EQ(testConfig.valueInt, -42);

    EXPECT_EQ(conf.getValue("", "42", &testConfig.valueLong, 0), true);
    EXPECT_EQ(testConfig.valueInt, -42);

    EXPECT_EQ(conf.getValue("", "42", &testConfig.valueULong, 0), true);
    EXPECT_EQ(testConfig.valueULong, static_cast<unsigned long>(-42));

    EXPECT_EQ(conf.getValue("", "42", &testConfig.valueDouble, 0), true);
    EXPECT_EQ(testConfig.valueDouble, -42.42);

    // references
    EXPECT_EQ(conf.getValue("", "42", testConfig.valueString, "0"), true);
    EXPECT_EQ(testConfig.valueString, "-42.42");

    EXPECT_EQ(conf.getValue("", "42", testConfig.valueBool, false), true);
    EXPECT_EQ(testConfig.valueBool, true);

    EXPECT_EQ(conf.getValue("", "42", testConfig.valueInt, 0), true);
    EXPECT_EQ(testConfig.valueInt, -42);

    EXPECT_EQ(conf.getValue("", "42", testConfig.valueLong, 0), true);
    EXPECT_EQ(testConfig.valueInt, -42);

    EXPECT_EQ(conf.getValue("", "42", testConfig.valueULong, 0), true);
    EXPECT_EQ(testConfig.valueULong, static_cast<unsigned long>(-42));

    EXPECT_EQ(conf.getValue("", "42", testConfig.valueDouble, 0), true);
    EXPECT_EQ(testConfig.valueDouble, -42.42);
}

// GTEST_TEST(Configator_getValue, DISABLED_success_withoutDefaultValue)
GTEST_TEST(Configator_getValue, success_withoutDefaultValue)
{
    using namespace Config;

    struct TestConfig
    {
        std::string     valueString;
        bool            valueBool;
        int             valueInt;
        long            valueLong;
        unsigned long   valueULong;
        double          valueDouble;
    } testConfig;

    Configator conf({
        {"",
            {
                {"42", "-42.42"}
            }
        }
    });

    // pointers
    EXPECT_EQ(conf.getValue("", "42", &testConfig.valueString), true);
    EXPECT_EQ(testConfig.valueString, "-42.42");

    EXPECT_EQ(conf.getValue("", "42", &testConfig.valueBool), true);
    EXPECT_EQ(testConfig.valueBool, true);

    EXPECT_EQ(conf.getValue("", "42", &testConfig.valueInt), true);
    EXPECT_EQ(testConfig.valueInt, -42);

    EXPECT_EQ(conf.getValue("", "42", &testConfig.valueLong), true);
    EXPECT_EQ(testConfig.valueInt, -42);

    EXPECT_EQ(conf.getValue("", "42", &testConfig.valueULong), true);
    EXPECT_EQ(testConfig.valueULong, static_cast<unsigned long>(-42));

    EXPECT_EQ(conf.getValue("", "42", &testConfig.valueDouble), true);
    EXPECT_EQ(testConfig.valueDouble, -42.42);

    // references
    EXPECT_EQ(conf.getValue("", "42", testConfig.valueString), true);
    EXPECT_EQ(testConfig.valueString, "-42.42");

    EXPECT_EQ(conf.getValue("", "42", testConfig.valueBool), true);
    EXPECT_EQ(testConfig.valueBool, true);

    EXPECT_EQ(conf.getValue("", "42", testConfig.valueInt), true);
    EXPECT_EQ(testConfig.valueInt, -42);

    EXPECT_EQ(conf.getValue("", "42", testConfig.valueLong), true);
    EXPECT_EQ(testConfig.valueInt, -42);

    EXPECT_EQ(conf.getValue("", "42", testConfig.valueULong), true);
    EXPECT_EQ(testConfig.valueULong, static_cast<unsigned long>(-42));

    EXPECT_EQ(conf.getValue("", "42", testConfig.valueDouble), true);
    EXPECT_EQ(testConfig.valueDouble, -42.42);
}

// GTEST_TEST(Configator_getValue, DISABLED_success_Return_withDefaultValue)
GTEST_TEST(Configator_getValue, success_Return_withDefaultValue)
{
    using namespace Config;

    struct TestConfig
    {
        std::string     valueString;
        bool            valueBool;
        int             valueInt;
        long            valueLong;
        unsigned long   valueULong;
        double          valueDouble;
    } testConfig;

    Configator conf({
        {"",
            {
                {"42", "-42.42"}
            }
        }
    });

    testConfig.valueString = conf.getValue<std::string>("", "42", "0");
    EXPECT_EQ(testConfig.valueString, "-42.42");

    testConfig.valueBool = conf.getValue<bool>("", "42", false);
    EXPECT_EQ(testConfig.valueBool, true);

    testConfig.valueInt = conf.getValue<int>("", "42", 0);
    EXPECT_EQ(testConfig.valueInt, -42);

    testConfig.valueLong = conf.getValue<long>("", "42", 0);
    EXPECT_EQ(testConfig.valueInt, -42);

    testConfig.valueULong = conf.getValue<unsigned long>("", "42", 0);
    EXPECT_EQ(testConfig.valueULong, static_cast<unsigned long>(-42));

    testConfig.valueDouble = conf.getValue<double>("", "42", 0);
    EXPECT_EQ(testConfig.valueDouble, -42.42);
}

// GTEST_TEST(Configator_getValue, DISABLED_success_Return_withoutDefaultValue)
GTEST_TEST(Configator_getValue, success_Return_withoutDefaultValue)
{
    using namespace Config;

    struct TestConfig
    {
        std::string     valueString;
        bool            valueBool;
        int             valueInt;
        long            valueLong;
        unsigned long   valueULong;
        double          valueDouble;
    } testConfig;

    Configator conf({
        {"",
            {
                {"42", "-42.42"}
            }
        }
    });

    testConfig.valueString = conf.getValue<std::string>("", "42");
    EXPECT_EQ(testConfig.valueString, "-42.42");

    testConfig.valueBool = conf.getValue<bool>("", "42");
    EXPECT_EQ(testConfig.valueBool, true);

    testConfig.valueInt = conf.getValue<int>("", "42");
    EXPECT_EQ(testConfig.valueInt, -42);

    testConfig.valueLong = conf.getValue<long>("", "42");
    EXPECT_EQ(testConfig.valueInt, -42);

    testConfig.valueULong = conf.getValue<unsigned long>("", "42");
    EXPECT_EQ(testConfig.valueULong, static_cast<unsigned long>(-42));

    testConfig.valueDouble = conf.getValue<double>("", "42");
    EXPECT_EQ(testConfig.valueDouble, -42.42);
}

// GTEST_TEST(Configator_getValue, DISABLED_failure_withDefaultValue)
GTEST_TEST(Configator_getValue, failure_withDefaultValue)
{
    using namespace Config;

    struct TestConfig
    {
        std::string     valueString;
        bool            valueBool;
        int             valueInt;
        long            valueLong;
        unsigned long   valueULong;
        double          valueDouble;
    } testConfig;

    Configator conf({
        {"",
            {
                {"42", "-42.42"}
            }
        }
    });

    // pointers
    EXPECT_EQ(conf.getValue("", "24", &testConfig.valueString, "0"), false);
    EXPECT_EQ(testConfig.valueString, "0");

    EXPECT_EQ(conf.getValue("", "24", &testConfig.valueBool, false), false);
    EXPECT_EQ(testConfig.valueBool, false);

    EXPECT_EQ(conf.getValue("", "24", &testConfig.valueInt, -1), false);
    EXPECT_EQ(testConfig.valueInt, -1);

    EXPECT_EQ(conf.getValue("", "24", &testConfig.valueLong, -1), false);
    EXPECT_EQ(testConfig.valueInt, -1);

    EXPECT_EQ(conf.getValue("", "24", &testConfig.valueULong, -1), false);
    EXPECT_EQ(testConfig.valueULong, static_cast<unsigned long>(-1));

    EXPECT_EQ(conf.getValue("", "24", &testConfig.valueDouble, -1), false);
    EXPECT_EQ(testConfig.valueDouble, -1);

    // references
    EXPECT_EQ(conf.getValue("", "24", testConfig.valueString, "0"), false);
    EXPECT_EQ(testConfig.valueString, "0");

    EXPECT_EQ(conf.getValue("", "24", testConfig.valueBool, false), false);
    EXPECT_EQ(testConfig.valueBool, false);

    EXPECT_EQ(conf.getValue("", "24", testConfig.valueInt, -1), false);
    EXPECT_EQ(testConfig.valueInt, -1);

    EXPECT_EQ(conf.getValue("", "24", testConfig.valueLong, -1), false);
    EXPECT_EQ(testConfig.valueInt, -1);

    EXPECT_EQ(conf.getValue("", "24", testConfig.valueULong, -1), false);
    EXPECT_EQ(testConfig.valueULong, static_cast<unsigned long>(-1));

    EXPECT_EQ(conf.getValue("", "24", testConfig.valueDouble, -1), false);
    EXPECT_EQ(testConfig.valueDouble, -1);
}

// GTEST_TEST(Configator_getValue, DISABLED_failure_withoutDefaultValue)
GTEST_TEST(Configator_getValue, failure_withoutDefaultValue)
{
    using namespace Config;

    struct TestConfig
    {
        std::string     valueString;
        bool            valueBool;
        int             valueInt;
        long            valueLong;
        unsigned long   valueULong;
        double          valueDouble;
    } testConfig;

    Configator conf({
        {"",
            {
                {"42", "-42.42"}
            }
        }
    });

    // pointers
    EXPECT_EQ(conf.getValue("", "24", &testConfig.valueString), false);
    EXPECT_EQ(conf.getValue("", "24", &testConfig.valueBool), false);
    EXPECT_EQ(conf.getValue("", "24", &testConfig.valueInt), false);
    EXPECT_EQ(conf.getValue("", "24", &testConfig.valueLong), false);
    EXPECT_EQ(conf.getValue("", "24", &testConfig.valueULong), false);
    EXPECT_EQ(conf.getValue("", "24", &testConfig.valueDouble), false);

    // references
    EXPECT_EQ(conf.getValue("", "24", testConfig.valueString), false);
    EXPECT_EQ(conf.getValue("", "24", testConfig.valueBool), false);
    EXPECT_EQ(conf.getValue("", "24", testConfig.valueInt), false);
    EXPECT_EQ(conf.getValue("", "24", testConfig.valueLong), false);
    EXPECT_EQ(conf.getValue("", "24", testConfig.valueULong), false);
    EXPECT_EQ(conf.getValue("", "24", testConfig.valueDouble), false);
}

// GTEST_TEST(Configator_getValue, DISABLED_failure_Return_withDefaultValue)
GTEST_TEST(Configator_getValue, failure_Return_withDefaultValue)
{
    using namespace Config;

    struct TestConfig
    {
        std::string     valueString;
        bool            valueBool;
        int             valueInt;
        long            valueLong;
        unsigned long   valueULong;
        double          valueDouble;
    } testConfig;

    Configator conf({
        {"",
            {
                {"42", "-42.42"}
            }
        }
    });

    testConfig.valueString = conf.getValue<std::string>("", "24", "0");
    EXPECT_EQ(testConfig.valueString, "0");

    testConfig.valueBool = conf.getValue<bool>("", "24", false);
    EXPECT_EQ(testConfig.valueBool, false);

    testConfig.valueInt = conf.getValue<int>("", "24", 0);
    EXPECT_EQ(testConfig.valueInt, 0);

    testConfig.valueLong = conf.getValue<long>("", "24", 0);
    EXPECT_EQ(testConfig.valueInt, 0);

    testConfig.valueULong = conf.getValue<unsigned long>("", "24", 0);
    EXPECT_EQ(testConfig.valueULong, static_cast<unsigned long>(0));

    testConfig.valueDouble = conf.getValue<double>("", "24", 0);
    EXPECT_EQ(testConfig.valueDouble, 0);
}

// GTEST_TEST(Configator_getValue, DISABLED_failure_Return_withoutDefaultValue)
GTEST_TEST(Configator_getValue, failure_Return_withoutDefaultValue)
{
    using namespace Config;

    struct TestConfig
    {
        std::string     valueString;
        bool            valueBool;
        int             valueInt;
        long            valueLong;
        unsigned long   valueULong;
        double          valueDouble;
    } testConfig;

    Configator conf({
        {"",
            {
                {"42", "-42.42"}
            }
        }
    });

    EXPECT_ANY_THROW(testConfig.valueString = conf.getValue<std::string>("", "24"));
    EXPECT_ANY_THROW(testConfig.valueBool = conf.getValue<bool>("", "24"));
    EXPECT_ANY_THROW(testConfig.valueInt = conf.getValue<int>("", "24"));
    EXPECT_ANY_THROW(testConfig.valueLong = conf.getValue<long>("", "24"));
    EXPECT_ANY_THROW(testConfig.valueULong = conf.getValue<unsigned long>("", "24"));
    EXPECT_ANY_THROW(testConfig.valueDouble = conf.getValue<double>("", "24"));
}