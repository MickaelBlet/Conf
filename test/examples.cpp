#include <gtest/gtest.h>

#include "mblet/configator.hpp"

GTEST_TEST(configator, test1) {
    // create example string
    std::string strConf("[test]\n" \
                        "42=-42.42");

    mblet::Configator conf;
    conf.readString(strConf);

    EXPECT_EQ(conf["test"]["42"].getString(), "-42.42");
}

GTEST_TEST(configator, test2) {
    // create example string
    std::string strConf("[test]\n" \
                        "0=true   ;bool\n" \
                        "1=-42.42 ;dec\n" \
                        "2=0x42   ;hex\n" \
                        "3=0b0101 ;binary\n" \
                        "4=042    ;octal");

    mblet::Configator conf;
    conf.readString(strConf);

    EXPECT_EQ(conf["test"]["0"].getString(), "true");
    EXPECT_EQ(conf["test"]["0"].getNumber(), 1);
    EXPECT_EQ(conf["test"]["0"].getComment(), "bool");

    EXPECT_EQ(conf["test"]["1"].getString(), "-42.42");
    EXPECT_EQ(conf["test"]["1"].getNumber(), -42.42);
    EXPECT_EQ(conf["test"]["1"].get<int>(), -42);
    EXPECT_EQ(conf["test"]["1"].getComment(), "dec");

    EXPECT_EQ(conf["test"]["2"].getString(), "0x42");
    EXPECT_EQ(conf["test"]["2"].getNumber(), 66);
    EXPECT_EQ(conf["test"]["2"].getComment(), "hex");

    EXPECT_EQ(conf["test"]["3"].getString(), "0b0101");
    EXPECT_EQ(conf["test"]["3"].getNumber(), 5);
    EXPECT_EQ(conf["test"]["3"].getComment(), "binary");

    EXPECT_EQ(conf["test"]["4"].getString(), "042");
    EXPECT_EQ(conf["test"]["4"].getNumber(), 34);
    EXPECT_EQ(conf["test"]["4"].getComment(), "octal");

    EXPECT_EQ(conf["test"]["5"].get<int>(42), 42);
}

GTEST_TEST(configator, test3) {
    // create example string
    std::string strConf("[    test     ]    ;        commment line\n" \
                        "table[]=1\n" \
                        "table[]=2\n" \
                        "map[key]=value\n" \
                        "map[key1][key2]=value2");

    mblet::Configator conf;
    conf.readString(strConf);

    EXPECT_EQ(conf["test"]["table"][1].getNumber(), 2);
    EXPECT_EQ(conf["test"]["table"][0].getNumber(), 1);
    EXPECT_EQ(conf["test"]["map"]["key"].getString(), "value");
    EXPECT_EQ(conf["test"]["map"]["key1"]["key2"].getString(), "value2");

    EXPECT_EQ(conf["test"].getComment(), "commment line");
}

GTEST_TEST(configator, test4) {
    // create example string
    std::string strConf("[ grandfather ]  ; commment line\n" \
                        "[ [ father ] ]\n" \
                        "[ [ [ child ] ] ]\n" \
                        "0=false   ;bool\n" \
                        "1=-42.42 ;dec\n" \
                        "2=0x42   ;hex\n" \
                        "3=0b0101 ;binary\n" \
                        "4=042    ;octal\n" \
                        "[grandmother][mother][child]\n" \
                        "5=42");

    mblet::Configator conf;
    conf.readString(strConf);

    EXPECT_EQ(conf["grandfather"]["father"]["child"]["0"].getNumber(), 0);
    EXPECT_EQ(conf["grandfather"]["father"]["child"]["1"].getNumber(), -42.42);
    EXPECT_EQ(conf["grandfather"]["father"]["child"]["2"].getNumber(), 66);
    EXPECT_EQ(conf["grandfather"]["father"]["child"]["3"].getNumber(), 5);
    EXPECT_EQ(conf["grandfather"]["father"]["child"]["4"].getNumber(), 34);
    EXPECT_EQ(conf["grandmother"]["mother"]["child"]["5"].get<std::string>(), "42");
}

GTEST_TEST(configator, test5) {
    // create example string
    std::string strConf("[test]\n" \
                        "\"32[\\]\"      =     '\"2 ;\"#\\'## i\\\\' # test with comment in line\n" \
                        "42     [\"key[]\"]      =     \"value\"       ;       comment line\n" \
                        "'42 space  ' [\"key[]\"]      =     \" value space \" ; comment line");

    mblet::Configator conf;
    conf.readString(strConf);

    EXPECT_EQ(conf["test"]["32[\\]"].getString(), "\"2 ;\"#'## i\\");
    EXPECT_EQ(conf["test"]["32[\\]"].getComment(), "test with comment in line");
    EXPECT_EQ(conf["test"]["42"]["key[]"].getString(), "value");
    EXPECT_EQ(conf["test"]["42"]["key[]"].getComment(), "comment line");
    EXPECT_EQ(conf["test"]["42 space  "]["key[]"].getString(), " value space ");
    EXPECT_EQ(conf["test"]["42 space  "]["key[]"].getComment(), "comment line");
}