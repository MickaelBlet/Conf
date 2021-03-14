#include <gtest/gtest.h>
#include <iostream>

#include "configator.hpp"

GTEST_TEST(configurator, test1) {
    const char *testFile = "test.ini";
    // create example file
    std::ofstream fileStream(testFile, std::ofstream::out | std::ofstream::trunc);
    fileStream << "[test]" << '\n'
               << "42=-42.42" << '\n';
    fileStream.close();

    mblet::Configator conf;
    EXPECT_EQ(conf.readFile(testFile), true);

    // remove example file
    remove(testFile);

    EXPECT_EQ(conf["test"]["42"].value, "-42.42");
}

GTEST_TEST(configurator, test2) {
    const char *testFile = "test.ini";
    // create example file
    std::ofstream fileStream(testFile, std::ofstream::out | std::ofstream::trunc);
    fileStream << "[test]" << '\n'
               << "0=true   ;bool" << '\n'
               << "1=-42.42 ;dec" << '\n'
               << "2=0x42   ;hex" << '\n'
               << "3=0b0101 ;binary" << '\n'
               << "4=042    ;octal" << '\n';
    fileStream.close();

    mblet::Configator conf;
    EXPECT_EQ(conf.readFile(testFile), true);

    // remove example file
    remove(testFile);

    EXPECT_EQ(conf["test"]["0"].get<int>(), 1);
    EXPECT_EQ(conf["test"]["1"].get<int>(), -42);
    EXPECT_EQ(conf["test"]["2"].get<int>(), 66);
    EXPECT_EQ(conf["test"]["3"].get<int>(), 5);
    EXPECT_EQ(conf["test"]["4"].get<int>(), 34);
    EXPECT_EQ(conf["test"]["5"].get<int>(42), 42);
}

GTEST_TEST(configurator, test3) {
    const char *testFile = "test.ini";
    // create example file
    std::ofstream fileStream(testFile, std::ofstream::out | std::ofstream::trunc);
    fileStream << "[    test     ]    ; commment line" << '\n'
               << "table[]=1" << '\n'
               << "table[]=2" << '\n'
               << "map[key]=value" << '\n'
               << "map[key1][key2]=value2" << '\n';
    fileStream.close();

    mblet::Configator conf;
    EXPECT_EQ(conf.readFile(testFile), true);

    // remove example file
    remove(testFile);

    EXPECT_EQ(conf["test"]["table"][0].get<int>(), 1);
    EXPECT_EQ(conf["test"]["table"][1].get<int>(), 2);
    EXPECT_EQ(conf["test"]["map"]["key"].value, "value");
    EXPECT_EQ(conf["test"]["map"]["key1"]["key2"].value, "value2");
}

GTEST_TEST(configurator, test4) {
    const char *testFile = "test.ini";
    // create example file
    std::ofstream fileStream(testFile, std::ofstream::out | std::ofstream::trunc);
    fileStream << "[ grandfather ]  ; commment line" << '\n'
               << "[ [ father ] ]" << '\n'
               << "[ [ [ child ] ] ]" << '\n'
               << "0=true   ;bool" << '\n'
               << "1=-42.42 ;dec" << '\n'
               << "2=0x42   ;hex" << '\n'
               << "3=0b0101 ;binary" << '\n'
               << "4=042    ;octal" << '\n'
               << "[grandmother][mother][child]" << '\n'
               << "5=42" << '\n';
    fileStream.close();

    mblet::Configator conf;
    EXPECT_EQ(conf.readFile(testFile), true);

    // remove example file
    remove(testFile);

    EXPECT_EQ(conf["grandfather"]["father"]["child"]["0"].get<int>(), 1);
    EXPECT_EQ(conf["grandfather"]["father"]["child"]["1"].get<int>(), -42);
    EXPECT_EQ(conf["grandfather"]["father"]["child"]["2"].get<int>(), 66);
    EXPECT_EQ(conf["grandfather"]["father"]["child"]["3"].get<int>(), 5);
    EXPECT_EQ(conf["grandfather"]["father"]["child"]["4"].get<int>(), 34);
    EXPECT_EQ(conf["grandmother"]["mother"]["child"]["5"].get<int>(), 42);
}

GTEST_TEST(configurator, test5) {
    const char *testFile = "test.ini";
    // create example file
    std::ofstream fileStream(testFile, std::ofstream::out | std::ofstream::trunc);
    fileStream << "[test]" << '\n'
               << "\"32[\\\\]\"      =     '\"2 ;\"#\\'## i\\\\' # test with comment in line" << '\n'
               << "42     [\"key[]\"]      =     \"value\" ; comment line" << '\n'
               << "'42 space  ' [\"key[]\"]      =     \" value space \" ; comment line" << '\n';
    fileStream.close();

    mblet::Configator conf;
    EXPECT_EQ(conf.readFile(testFile), true);

    // remove example file
    remove(testFile);

    EXPECT_EQ(conf["test"]["32[\\]"].value, "\"2 ;\"#'## i\\");
    EXPECT_EQ(conf["test"]["42"]["key[]"].value, "value");
    EXPECT_EQ(conf["test"]["42 space  "]["key[]"].value, " value space ");
}