#include <gtest/gtest.h>

#include "blet/conf.h"

GTEST_TEST(loadString, comment) {
    // clang-format off
    const char confStr[] = ""
        "; comment\n"
        "# comment";
    // clang-format on

    blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_TRUE(conf.isNull());
}

GTEST_TEST(loadString, backSlash) {
    // clang-format off
    const char confStr[] = ""
        "[test]\n"
        "\"test\\\\\\\"test\" = 42";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_EQ(conf["test"]["test\\\"test"], 42);
}

GTEST_TEST(loadString, keyOfMap) {
    // clang-format off
    const char confStr[] = ""
        "[test]\n"
        "test [ test ] = 42";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_EQ(conf["test"]["test"]["test"], 42);
}

GTEST_TEST(loadString, keyOfMultiMap) {
    // clang-format off
    const char confStr[] = ""
        "[test]\n"
        "test [ test ][ test ] = 42";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_EQ(conf["test"]["test"]["test"]["test"], 42);
}

GTEST_TEST(loadString, quoteInKeyOfMap) {
    // clang-format off
    const char confStr[] = ""
        "[test]\n"
        "test [\"test\\\"test\"] = 42";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_EQ(conf["test"]["test"]["test\"test"], 42);
}

GTEST_TEST(loadString, empty) {
    {
        blet::Dict conf = blet::conf::loadString("");
        EXPECT_TRUE(conf.isNull());
    }
    {
        blet::Dict conf = blet::conf::loadString("  ");
        EXPECT_TRUE(conf.isNull());
    }
    {
        blet::Dict conf = blet::conf::loadString("\t");
        EXPECT_TRUE(conf.isNull());
    }
    {
        blet::Dict conf = blet::conf::loadString("\n");
        EXPECT_TRUE(conf.isNull());
    }
}

GTEST_TEST(loadString, nullValue) {
    // clang-format off
    const char confStr[] = ""
        "[test]\n"
        "test1 = \n"
        "test2 = Null\n"
        "test3 = null\n"
        "test4 = None\n"
        "test5 = none";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_TRUE(conf["test"]["test1"].isNull());
    EXPECT_TRUE(conf["test"]["test2"].isNull());
    EXPECT_TRUE(conf["test"]["test3"].isNull());
    EXPECT_TRUE(conf["test"]["test4"].isNull());
    EXPECT_TRUE(conf["test"]["test5"].isNull());
}

GTEST_TEST(loadString, nullObject) {
    // clang-format off
    const char confStr[] = ""
        "[test]\n"
        "test = {}";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_TRUE(conf["test"]["test"].isObject());
    EXPECT_TRUE(conf["test"]["test"].empty());
}

GTEST_TEST(loadString, nullArray) {
    // clang-format off
    const char confStr[] = ""
        "[test]\n"
        "test = []";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_TRUE(conf["test"]["test"].isArray());
    EXPECT_TRUE(conf["test"]["test"].empty());
}

GTEST_TEST(loadString, multiSectionWithQuote) {
    // clang-format off
    const char confStr[] = ""
        "[test]\n"
        "[[\"te\\\"st\"]]\n"
        "test = 42";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_EQ(conf["test"]["te\"st"]["test"], 42);
}

GTEST_TEST(loadString, multiSectionSubLevel) {
    // clang-format off
    const char confStr[] = ""
        "[test]\n"
        "[[test]]\n"
        "test = 42\n"
        "[[[test2]]]\n"
        "[[[[test3]]]]\n"
        "test = 42\n"
        "[[test4]]\n"
        "test = 42";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_EQ(conf["test"]["test"]["test"], 42);
    EXPECT_EQ(conf["test"]["test"]["test2"]["test3"]["test"], 42);
    EXPECT_EQ(conf["test"]["test4"]["test"], 42);
}

GTEST_TEST(loadString, sectionWithQuote) {
    // clang-format off
    const char confStr[] = ""
        "[\"te\\\"st\"]\n"
        "test = 42";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_EQ(conf["te\"st"]["test"], 42);
}

GTEST_TEST(loadString, sectionDefault) {
    // clang-format off
    const char confStr[] = ""
        "[]\n"
        "test = 42";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_EQ(conf["test"], 42);
}

GTEST_TEST(loadString, multiInlineSectionWithQuote) {
    // clang-format off
    const char confStr[] = ""
        "[test][\"te\\\"st\"]\n"
        "test = 42";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_EQ(conf["test"]["te\"st"]["test"], 42);
}

GTEST_TEST(loadString, jsonArrayQuoteValue) {
    // clang-format off
    const char confStr[] = ""
        "[test]\n"
        "test = [ \"va\\\"lue\" ]";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_EQ(conf["test"]["test"][0], "va\"lue");
}

GTEST_TEST(loadString, jsonArrayValue) {
    // clang-format off
    const char confStr[] = ""
        "[test]\n"
        "test = [ 42 ]";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_EQ(conf["test"]["test"][0], 42);
}

GTEST_TEST(loadString, jsonArrayValueObject) {
    // clang-format off
    const char confStr[] = ""
        "[test]\n"
        "test = [ {} ]";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_TRUE(conf["test"]["test"][0].isObject());
    EXPECT_TRUE(conf["test"]["test"][0].empty());
}

GTEST_TEST(loadString, jsonArrayValueArray) {
    // clang-format off
    const char confStr[] = ""
        "[test]\n"
        "test = [ [] ]";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_TRUE(conf["test"]["test"][0].isArray());
    EXPECT_TRUE(conf["test"]["test"][0].empty());
}

GTEST_TEST(loadString, jsonArrayMultiValue) {
    // clang-format off
    const char confStr[] = ""
        "[test]\n"
        "test = [ 42, 42 ]";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_EQ(conf["test"]["test"][0], 42);
    EXPECT_EQ(conf["test"]["test"][1], 42);
}

GTEST_TEST(loadString, jsonObjectQuoteKey) {
    // clang-format off
    const char confStr[] = ""
        "[test]\n"
        "test = { \"k\\\"ey\" = null }";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_TRUE(conf["test"]["test"]["k\"ey"].isNull());
}

GTEST_TEST(loadString, jsonObjectKey) {
    // clang-format off
    const char confStr[] = ""
        "[test]\n"
        "test = { key = null }";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_TRUE(conf["test"]["test"]["key"].isNull());
}

GTEST_TEST(loadString, jsonObjectQuoteValue) {
    // clang-format off
    const char confStr[] = ""
        "[test]\n"
        "test = { \"key\" = \"va\\\"lue\" }";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_EQ(conf["test"]["test"]["key"], "va\"lue");
}

GTEST_TEST(loadString, jsonObjectObjectValue) {
    // clang-format off
    const char confStr[] = ""
        "[test]\n"
        "test = { \"key\" = {} }";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_TRUE(conf["test"]["test"]["key"].isObject());
    EXPECT_TRUE(conf["test"]["test"]["key"].empty());
}

GTEST_TEST(loadString, jsonObjectArrayValue) {
    // clang-format off
    const char confStr[] = ""
        "[test]\n"
        "test = { \"key\" = [] }";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_TRUE(conf["test"]["test"]["key"].isArray());
    EXPECT_TRUE(conf["test"]["test"]["key"].empty());
}

GTEST_TEST(loadString, jsonObjectMultiValue) {
    // clang-format off
    const char confStr[] = ""
        "[test]\n"
        "test = { key1 = 1, key2 = 2 }";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_EQ(conf["test"]["test"]["key1"], 1);
    EXPECT_EQ(conf["test"]["test"]["key2"], 2);
}

GTEST_TEST(loadString, parseValue) {
    // clang-format off
    const char confStr[] = ""
        "[test]\n"
        "1 = false\n"
        "2 = falseeee\n"
        "3 = No\n"
        "4 = noooo\n"
        "5 = on\n"
        "6 = off\n"
        "7 = onnn\n"
        "8 = true\n"
        "9 = trueeee\n"
        "10 = yes\n"
        "11 = yesss\n"
        "12 = default";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_EQ(conf["test"]["1"], false);
    EXPECT_EQ(conf["test"]["2"], "falseeee");
    EXPECT_EQ(conf["test"]["3"], false);
    EXPECT_EQ(conf["test"]["4"], "noooo");
    EXPECT_EQ(conf["test"]["5"], true);
    EXPECT_EQ(conf["test"]["6"], false);
    EXPECT_EQ(conf["test"]["7"], "onnn");
    EXPECT_EQ(conf["test"]["8"], true);
    EXPECT_EQ(conf["test"]["9"], "trueeee");
    EXPECT_EQ(conf["test"]["10"], true);
    EXPECT_EQ(conf["test"]["11"], "yesss");
    EXPECT_EQ(conf["test"]["12"], "default");
}

GTEST_TEST(loadString, parseValueHex) {
    // clang-format off
    const char confStr[] = ""
        "[test]\n"
        "1 = -0x0a98\n"
        "2 = 0x0a98g\n";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_EQ(conf["test"]["1"], -2712);
    EXPECT_EQ(conf["test"]["2"], "0x0a98g");
}

GTEST_TEST(loadString, parseValueBinary) {
    // clang-format off
    const char confStr[] = ""
        "[test]\n"
        "1 = -0b101\n"
        "2 = 0b101\n"
        "3 = 0b1012\n";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_EQ(conf["test"]["1"], -5);
    EXPECT_EQ(conf["test"]["2"], 5);
    EXPECT_EQ(conf["test"]["3"], "0b1012");
}

GTEST_TEST(loadString, parseValueOctal) {
    // clang-format off
    const char confStr[] = ""
        "[test]\n"
        "1 = -042\n";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_EQ(conf["test"]["1"], -34);
}

GTEST_TEST(loadString, parseReplaceEscapeChar) {
    // clang-format off
    const char confStr[] = ""
        "[test]\n"
        "1 = \"\\a\\b\\f\\n\\r\\t\\v\\\'\\\"\\\\\"\n";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_EQ(conf["test"]["1"], "\a\b\f\n\r\t\v\'\"\\");
}

GTEST_TEST(loadString, parseJson) {
    // clang-format off
    const char confStr[] = ""
        "{\n"
        "  \"test\": 42\n"
        "}\n";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_EQ(conf["test"], 42);
}

GTEST_TEST(loadString, parseJsonOneLine) {
    // clang-format off
    const char confStr[] = "{\"test1\":42,\"test2\":true}";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_EQ(conf["test1"], 42);
    EXPECT_EQ(conf["test2"], true);
}

GTEST_TEST(loadString, parseJsonWithSection) {
    // clang-format off
    const char confStr[] = ""
        "[test]"
        "{\n"
        "  \"test\": 42\n"
        "}\n";
    // clang-format on

    const blet::Dict conf = blet::conf::loadString(confStr);
    EXPECT_EQ(conf["test"]["test"], 42);
}
