#include <gtest/gtest.h>

#include "blet/conf.h"

GTEST_TEST(dump, null) {
    blet::Dict dict;
    EXPECT_EQ(blet::conf::dump(dict), "null");
    EXPECT_EQ(blet::conf::dump(dict, 0, ' ', blet::conf::JSON_STYLE), "null");
}

GTEST_TEST(dump, boolean) {
    blet::Dict dict = true;
    EXPECT_EQ(blet::conf::dump(dict), "true");
    EXPECT_EQ(blet::conf::dump(dict, 0, ' ', blet::conf::JSON_STYLE), "true");
}

GTEST_TEST(dump, number) {
    blet::Dict dict = 42;
    EXPECT_EQ(blet::conf::dump(dict), "42");
    EXPECT_EQ(blet::conf::dump(dict, 0, ' ', blet::conf::JSON_STYLE), "42");
}

GTEST_TEST(dump, string) {
    blet::Dict dict = "foo";
    EXPECT_EQ(blet::conf::dump(dict), "\"foo\"");
    EXPECT_EQ(blet::conf::dump(dict, 0, ' ', blet::conf::JSON_STYLE), "\"foo\"");
}

GTEST_TEST(dump, array) {
    std::vector<int> v;
    v.push_back(0);
    v.push_back(1);
    v.push_back(2);
    blet::Dict dict(v);
    EXPECT_EQ(blet::conf::dump(dict), "\"\"[]=0\n\"\"[]=1\n\"\"[]=2");
    EXPECT_EQ(blet::conf::dump(dict, 1, ' ', blet::conf::JSON_STYLE), "\"\" = [\n 0,\n 1,\n 2\n]");
}

GTEST_TEST(dump, object) {
    std::map<std::string, int> m;
    m["foo"] = 42;
    blet::Dict dict(m);
    EXPECT_EQ(blet::conf::dump(dict), "foo=42");
    EXPECT_EQ(blet::conf::dump(dict, 0, ' ', blet::conf::JSON_STYLE), "foo=42");
}

GTEST_TEST(dump, conf_dump_simple) {
    {
        blet::Dict dict;
        dict["section1"]["foo"] = "bar";
        // clang-format off
        const char resultStr[] = ""
            "[section1]\n"
            "foo=\"bar\"";
        // clang-format on
        EXPECT_EQ(blet::conf::dump(dict), resultStr);
    }
    {
        blet::Dict dict;
        dict["section1"]["foo"] = "bar";
        // clang-format off
        const char resultStr[] = ""
            "[section1]\n"
            "foo = \"bar\"";
        // clang-format on
        EXPECT_EQ(blet::conf::dump(dict, 2), resultStr);
    }
}

GTEST_TEST(dump, conf_dump_sections) {
    {
        blet::Dict dict;
        dict["section1"]["foo"] = "bar";
        dict["section2"]["bar"] = "foo";
        // clang-format off
        const char resultStr[] = ""
            "[section1]\n"
            "foo=\"bar\"\n"
            "[section2]\n"
            "bar=\"foo\"";
        // clang-format on
        EXPECT_EQ(blet::conf::dump(dict), resultStr);
    }
    {
        blet::Dict dict;
        dict["section1"]["foo"] = "bar";
        dict["section2"]["bar"] = "foo";
        dict["section2"]["bri\an"] = "toto";
        // clang-format off
        const char resultStr[] = ""
            "[section1]\n"
            "foo = \"bar\"\n"
            "[section2]\n"
            "bar = \"foo\"\n"
            "\"bri\\an\" = \"toto\"";
        // clang-format on
        EXPECT_EQ(blet::conf::dump(dict, 2), resultStr);
    }
}

GTEST_TEST(dump, conf_dump_array) {
    {
        blet::Dict dict;
        dict["section"]["bar"] = 42;
        dict["section"]["foo"][0] = 42;
        dict["section"]["foo"][1] = "bar";
        dict["section"]["foo"][2].newNull();
        dict["section"]["foo"][3] = true;
        // clang-format off
        const char resultStr[] = ""
            "[section]\n"
            "bar = 42\n"
            "foo[] = 42\n"
            "foo[] = \"bar\"\n"
            "foo[] = null\n"
            "foo[] = true";
        // clang-format on
        EXPECT_EQ(blet::conf::dump(dict, 2), resultStr);
    }
}

GTEST_TEST(dump, conf_dump_empty_array) {
    {
        blet::Dict dict;
        dict["section"]["foo"] = std::vector<int>();
        dict["section"]["b\ar"] = std::vector<int>();
        // clang-format off
        const char resultStr[] = ""
            "[section]\n"
            "\"b\\ar\" = []\n"
            "foo = []";
        // clang-format on
        EXPECT_EQ(blet::conf::dump(dict, 2), resultStr);
    }
}

GTEST_TEST(dump, conf_dump_array_in_array) {
    {
        std::vector<int> v;
        v.push_back(42);
        blet::Dict dict;
        dict["section"]["foo"][0] = v;
        // clang-format off
        const char resultStr[] = ""
            "[section]\n"
            "foo[] = [\n"
            "  42\n"
            "]";
        // clang-format on
        EXPECT_EQ(blet::conf::dump(dict, 2), resultStr);
    }
}

GTEST_TEST(dump, conf_dump_object_in_array) {
    {
        std::map<std::string, int> m;
        m["bar"] = 42;
        blet::Dict dict;
        dict["sec\tion"]["foo"][0] = m;
        // clang-format off
        const char resultStr[] = ""
            "[\"sec\\tion\"]\n"
            "foo[] = {\n"
            "  bar = 42\n"
            "}";
        // clang-format on
        EXPECT_EQ(blet::conf::dump(dict, 2), resultStr);
    }
}

GTEST_TEST(dump, conf_dump_empty_object) {
    {
        blet::Dict dict;
        dict["section"]["foo"] = std::map<std::string, int>();
        dict["section"]["b\ar"] = std::map<std::string, int>();
        // clang-format off
        const char resultStr[] = ""
            "[section]\n"
            "\"b\\ar\" = {}\n"
            "foo = {}";
        // clang-format on
        EXPECT_EQ(blet::conf::dump(dict, 2), resultStr);
    }
}

GTEST_TEST(dump, json_dump_object) {
    {
        blet::Dict dict;
        dict["section"]["foo"] = std::map<std::string, int>();
        dict["section"]["null"].newNull();
        dict["section"]["boolean"] = false;
        dict["section"]["str"] = "foo";
        dict["section"]["b\ar"] = std::map<std::string, int>();
        dict["sec\tion"]["bar"] = std::vector<int>();
        // clang-format off
        const char resultStr[] = ""
            "\"sec\\tion\" = {\n"
            "  bar = []\n"
            "}\n"
            "section = {\n"
            "  \"b\\ar\" = {},\n"
            "  boolean = false,\n"
            "  foo = {},\n"
            "  null = null,\n"
            "  str = \"foo\"\n"
            "}";
        // clang-format on
        EXPECT_EQ(blet::conf::dump(dict, 2, ' ', blet::conf::JSON_STYLE), resultStr);
    }
}

GTEST_TEST(dump, escape_character) {
    {
        blet::Dict dict;
        dict["section"]["\a\b\f\n\r\t\v\'\"\\"] = std::map<std::string, int>();
        // clang-format off
        const char resultStr[] = ""
            "section = {\n"
            "  \"\\a\\b\\f\\n\\r\\t\\v\\'\\\"\\\\\" = {}\n"
            "}";
        // clang-format on
        EXPECT_EQ(blet::conf::dump(dict, 2, ' ', blet::conf::JSON_STYLE), resultStr);
    }
}