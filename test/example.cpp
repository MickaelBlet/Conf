#include <gtest/gtest.h>

#include "blet/conf.h"

GTEST_TEST(configator, test1) {
    // clang-format off
    // create example string
    const char strConf[] = ""
        "[test]\n"
        "42 = -42.42";
    // clang-format on

    blet::Dict conf = blet::conf::loadString(strConf);
    EXPECT_EQ(conf["test"]["42"].getNumber(), -42.42);
}

GTEST_TEST(configator, test2) {
    // clang-format off
    // create example string
    const char strConf[] = ""
        "[test]\n"
        "0=true   ;bool\n"
        "1=-42.42 ;dec\n"
        "2=0x42   ;hex\n"
        "3=0b0101 ;binary\n"
        "4=042    ;octal";
    // clang-format on

    blet::Dict conf = blet::conf::loadString(strConf);

    EXPECT_EQ(conf["test"]["0"].getBoolean(), true);

    EXPECT_EQ(conf["test"]["1"].getNumber(), -42.42);

    EXPECT_EQ(conf["test"]["2"].getNumber(), 66);

    EXPECT_EQ(conf["test"]["3"].getNumber(), 5);

    EXPECT_EQ(conf["test"]["4"].getNumber(), 34);

    if (!conf.contains(blet::Dict::Path()["test"]["5"], blet::Dict::NUMBER_TYPE)) {
        conf["test"]["5"] = 42;
    }

    EXPECT_EQ(conf["test"]["5"].getNumber(), 42);
}

GTEST_TEST(configator, test3) {
    // clang-format off
    // create example string
    const char strConf[] = ""
        "[    test   ]   ;   comment line\n"
        "table[]=1\n"
        "table[]=2\n"
        "map[key]=value\n"
        "map[key1][key2]=value2";
    // clang-format on

    blet::Dict conf = blet::conf::loadString(strConf);

    EXPECT_EQ(conf["test"]["table"][0].getNumber(), 1);
    EXPECT_EQ(conf["test"]["table"][1].getNumber(), 2);
    EXPECT_EQ(conf["test"]["map"]["key"].getString(), "value");
    EXPECT_EQ(conf["test"]["map"]["key1"]["key2"].getString(), "value2");
}

GTEST_TEST(configator, test4) {
    // clang-format off
    // create example string
    const char strConf[] = ""
        "[ grandfather ]  ; commment line\n"
        "[ [ father ] ]\n"
        "[ [ [ child ] ] ]\n"
        "0=false   ;bool\n"
        "1=-42.42 ;dec\n"
        "2=0x42   ;hex\n"
        "3=0b0101 ;binary\n"
        "4=042    ;octal\n"
        "[grandmother][mother][child]\n"
        "5=42";
    // clang-format on

    blet::Dict conf = blet::conf::loadString(strConf);

    EXPECT_EQ(conf["grandfather"]["father"]["child"]["0"].getBoolean(), false);
    EXPECT_EQ(conf["grandfather"]["father"]["child"]["1"].getNumber(), -42.42);
    EXPECT_EQ(conf["grandfather"]["father"]["child"]["2"].getNumber(), 66);
    EXPECT_EQ(conf["grandfather"]["father"]["child"]["3"].getNumber(), 5);
    EXPECT_EQ(conf["grandfather"]["father"]["child"]["4"].getNumber(), 34);
    EXPECT_EQ(conf["grandmother"]["mother"]["child"]["5"].get<double>(), 42);
}

GTEST_TEST(configator, test5) {
    // clang-format off
    // create example string
    const char strConf[] = ""
        "[test]\n"
        "\"32[\\]\"      =     '\"2 ;\"#\\'## i\\\\' # test with comment in line\n"
        "42     [\"key[]\"]      =     \"value\"       ;       comment line\n"
        "'42 space  ' [\"key[]\"]      =     \" value space \" ; comment line";
    // clang-format on

    blet::Dict conf = blet::conf::loadString(strConf);

    std::cout << blet::conf::dump(conf, 2, ' ', blet::conf::JSON_STYLE) << std::endl;

    EXPECT_EQ(conf["test"]["32[\\]"].getString(), "\"2 ;\"#'## i\\");
    EXPECT_EQ(conf["test"]["42"]["key[]"].getString(), "value");
    EXPECT_EQ(conf["test"]["42 space  "]["key[]"].getString(), " value space ");
}
