#include <gtest/gtest.h>

#include "configator.hpp"

GTEST_TEST(configator, test1) {
    // create example string
    std::string strConf("[test]\n" \
                        "42=-42.42");

    mblet::Configator conf;
    conf.readString(strConf);

    EXPECT_EQ(conf["test"]["42"].value, "-42.42");
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

    EXPECT_EQ(conf["test"]["0"].value, "true");
    EXPECT_EQ(conf["test"]["0"].get<int>(), 1);
    EXPECT_EQ(conf["test"]["0"].comment, "bool");

    EXPECT_EQ(conf["test"]["1"].value, "-42.42");
    EXPECT_EQ(conf["test"]["1"].get<int>(), -42);
    EXPECT_EQ(conf["test"]["1"].get<double>(), -42.42);
    EXPECT_EQ(conf["test"]["1"].comment, "dec");

    EXPECT_EQ(conf["test"]["2"].value, "0x42");
    EXPECT_EQ(conf["test"]["2"].get<int>(), 66);
    EXPECT_EQ(conf["test"]["2"].comment, "hex");

    EXPECT_EQ(conf["test"]["3"].value, "0b0101");
    EXPECT_EQ(conf["test"]["3"].get<int>(), 5);
    EXPECT_EQ(conf["test"]["3"].comment, "binary");

    EXPECT_EQ(conf["test"]["4"].value, "042");
    EXPECT_EQ(conf["test"]["4"].get<int>(42), 34);
    EXPECT_EQ(conf["test"]["4"].comment, "octal");

    EXPECT_EQ(conf["test"]["5"].get<int>(42), 42);
    EXPECT_EQ(conf["test"]["5"].get<double>(42.42), 42.42);
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

    EXPECT_EQ(conf["test"]["table"][1].get<int>(), 2);
    EXPECT_EQ(conf["test"]["table"][0].get<int>(), 1);
    EXPECT_EQ(conf["test"]["map"]["key"].value, "value");
    EXPECT_EQ(conf["test"]["map"]["key1"]["key2"].value, "value2");

    EXPECT_EQ(conf["test"].comment, "commment line");
}

GTEST_TEST(configator, test4) {
    // create example string
    std::string strConf("[ grandfather ]  ; commment line\n" \
                        "[ [ father ] ]\n" \
                        "[ [ [ child ] ] ]\n" \
                        "0=true   ;bool\n" \
                        "1=-42.42 ;dec\n" \
                        "2=0x42   ;hex\n" \
                        "3=0b0101 ;binary\n" \
                        "4=042    ;octal\n" \
                        "[grandmother][mother][child]\n" \
                        "5=42");

    mblet::Configator conf;
    conf.readString(strConf);

    EXPECT_EQ(conf["grandfather"]["father"]["child"]["0"].get<int>(), 1);
    EXPECT_EQ(conf["grandfather"]["father"]["child"]["1"].get<int>(), -42);
    EXPECT_EQ(conf["grandfather"]["father"]["child"]["2"].get<int>(), 66);
    EXPECT_EQ(conf["grandfather"]["father"]["child"]["3"].get<int>(), 5);
    EXPECT_EQ(conf["grandfather"]["father"]["child"]["4"].get<int>(), 34);
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

    EXPECT_EQ(conf["test"]["32[\\]"].value, "\"2 ;\"#'## i\\");
    EXPECT_EQ(conf["test"]["32[\\]"].comment, "test with comment in line");
    EXPECT_EQ(conf["test"]["42"]["key[]"].value, "value");
    EXPECT_EQ(conf["test"]["42"]["key[]"].comment, "comment line");
    EXPECT_EQ(conf["test"]["42 space  "]["key[]"].value, " value space ");
    EXPECT_EQ(conf["test"]["42 space  "]["key[]"].comment, "comment line");
}