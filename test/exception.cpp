#include <gtest/gtest.h>

#include "blet/conf.h"

GTEST_TEST(loadString, end_of_section) {
    EXPECT_THROW(
        {
            try {
                blet::Dict conf = blet::conf::loadString("['test]");
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:8 (End of quote)");
                EXPECT_EQ(e.message(), "End of quote");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 8);
                throw;
            }
        },
        blet::conf::LoadException);
    EXPECT_THROW(
        {
            try {
                blet::Dict conf = blet::conf::loadString("[test");
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:6 (End of section)");
                EXPECT_EQ(e.message(), "End of section");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 6);
                throw;
            }
        },
        blet::conf::LoadException);
    EXPECT_THROW(
        {
            try {
                blet::Dict conf = blet::conf::loadString("['test'");
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:8 (End of section)");
                EXPECT_EQ(e.message(), "End of section");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 8);
                throw;
            }
        },
        blet::conf::LoadException);
}

GTEST_TEST(loadString, end_of_multi_section) {
    EXPECT_THROW(
        {
            try {
                blet::Dict conf = blet::conf::loadString("[['test]");
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:9 (End of quote)");
                EXPECT_EQ(e.message(), "End of quote");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 9);
                throw;
            }
        },
        blet::conf::LoadException);
    EXPECT_THROW(
        {
            try {
                blet::Dict conf = blet::conf::loadString("[[test");
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:7 (End of section)");
                EXPECT_EQ(e.message(), "End of section");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 7);
                throw;
            }
        },
        blet::conf::LoadException);
    EXPECT_THROW(
        {
            try {
                blet::Dict conf = blet::conf::loadString("[[test]]]");
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:10 (End of section)");
                EXPECT_EQ(e.message(), "End of section");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 10);
                throw;
            }
        },
        blet::conf::LoadException);
}

GTEST_TEST(loadString, multi_section_without_parent) {
    EXPECT_THROW(
        {
            try {
                blet::Dict conf = blet::conf::loadString("[[test]]");
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:9 (Section without parent)");
                EXPECT_EQ(e.message(), "Section without parent");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 9);
                throw;
            }
        },
        blet::conf::LoadException);
}

GTEST_TEST(loadString, end_of_linear_section) {
    EXPECT_THROW(
        {
            try {
                blet::Dict conf = blet::conf::loadString("[test]['test");
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:13 (End of quote)");
                EXPECT_EQ(e.message(), "End of quote");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 13);
                throw;
            }
        },
        blet::conf::LoadException);
    EXPECT_THROW(
        {
            try {
                blet::Dict conf = blet::conf::loadString("[test]['test'");
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:14 (End of section)");
                EXPECT_EQ(e.message(), "End of section");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 14);
                throw;
            }
        },
        blet::conf::LoadException);
    EXPECT_THROW(
        {
            try {
                blet::Dict conf = blet::conf::loadString("[test][test");
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:12 (End of section)");
                EXPECT_EQ(e.message(), "End of section");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 12);
                throw;
            }
        },
        blet::conf::LoadException);
    EXPECT_THROW(
        {
            try {
                blet::Dict conf = blet::conf::loadString("[[test]]]");
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:10 (End of section)");
                EXPECT_EQ(e.message(), "End of section");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 10);
                throw;
            }
        },
        blet::conf::LoadException);
}

GTEST_TEST(loadString, key_not_found) {
    EXPECT_THROW(
        {
            try {
                blet::Dict conf = blet::conf::loadString("=");
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:1 (Key not found)");
                EXPECT_EQ(e.message(), "Key not found");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 1);
                throw;
            }
        },
        blet::conf::LoadException);
}

GTEST_TEST(loadString, end_of_quote) {
    EXPECT_THROW(
        {
            try {
                // clang-format off
                std::string jsonStr = "'test";
                // clang-format on
                blet::Dict conf = blet::conf::loadString(jsonStr);
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:6 (End of quote)");
                EXPECT_EQ(e.message(), "End of quote");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 6);
                throw;
            }
        },
        blet::conf::LoadException);
    EXPECT_THROW(
        {
            try {
                // clang-format off
                std::string jsonStr = "test['test";
                // clang-format on
                blet::Dict conf = blet::conf::loadString(jsonStr);
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:11 (End of quote)");
                EXPECT_EQ(e.message(), "End of quote");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 11);
                throw;
            }
        },
        blet::conf::LoadException);
    EXPECT_THROW(
        {
            try {
                // clang-format off
                std::string jsonStr = "test='test";
                // clang-format on
                blet::Dict conf = blet::conf::loadString(jsonStr);
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:11 (End of quote)");
                EXPECT_EQ(e.message(), "End of quote");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 11);
                throw;
            }
        },
        blet::conf::LoadException);
}

GTEST_TEST(loadString, end_of_key) {
    EXPECT_THROW(
        {
            try {
                // clang-format off
                std::string jsonStr = "'test'";
                // clang-format on
                blet::Dict conf = blet::conf::loadString(jsonStr);
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:7 (Assign operator not found)");
                EXPECT_EQ(e.message(), "Assign operator not found");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 7);
                throw;
            }
        },
        blet::conf::LoadException);
    EXPECT_THROW(
        {
            try {
                // clang-format off
                std::string jsonStr = "test";
                // clang-format on
                blet::Dict conf = blet::conf::loadString(jsonStr);
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:5 (Assign operator not found)");
                EXPECT_EQ(e.message(), "Assign operator not found");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 5);
                throw;
            }
        },
        blet::conf::LoadException);
}

GTEST_TEST(loadString, end_of_map_key) {
    EXPECT_THROW(
        {
            try {
                // clang-format off
                std::string jsonStr = "test['test'";
                // clang-format on
                blet::Dict conf = blet::conf::loadString(jsonStr);
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:12 (End of map)");
                EXPECT_EQ(e.message(), "End of map");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 12);
                throw;
            }
        },
        blet::conf::LoadException);
}

GTEST_TEST(loadString, end_of_map) {
    EXPECT_THROW(
        {
            try {
                // clang-format off
                std::string jsonStr = "test[test";
                // clang-format on
                blet::Dict conf = blet::conf::loadString(jsonStr);
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:10 (End of map)");
                EXPECT_EQ(e.message(), "End of map");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 10);
                throw;
            }
        },
        blet::conf::LoadException);
}

GTEST_TEST(loadString, operator_EQ_not_found) {
    EXPECT_THROW(
        {
            try {
                // clang-format off
                std::string jsonStr = "test[test]";
                // clang-format on
                blet::Dict conf = blet::conf::loadString(jsonStr);
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:11 (Assign operator not found)");
                EXPECT_EQ(e.message(), "Assign operator not found");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 11);
                throw;
            }
        },
        blet::conf::LoadException);
}

GTEST_TEST(loadString, end_of_value) {
    EXPECT_THROW(
        {
            try {
                // clang-format off
                std::string jsonStr = "test='test'toto";
                // clang-format on
                blet::Dict conf = blet::conf::loadString(jsonStr);
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:12 (End of value)");
                EXPECT_EQ(e.message(), "End of value");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 12);
                throw;
            }
        },
        blet::conf::LoadException);
}

GTEST_TEST(loadString, end_of_json_object_value) {
    EXPECT_THROW(
        {
            try {
                // clang-format off
                std::string jsonStr = "test={";
                // clang-format on
                blet::Dict conf = blet::conf::loadString(jsonStr);
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:7 (End of object)");
                EXPECT_EQ(e.message(), "End of object");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 7);
                throw;
            }
        },
        blet::conf::LoadException);
}

GTEST_TEST(loadString, key_of_json_object_value) {
    EXPECT_THROW(
        {
            try {
                // clang-format off
                std::string jsonStr = "test={=";
                // clang-format on
                blet::Dict conf = blet::conf::loadString(jsonStr);
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:7 (Key not found)");
                EXPECT_EQ(e.message(), "Key not found");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 7);
                throw;
            }
        },
        blet::conf::LoadException);
}

GTEST_TEST(loadString, end_of_quote_at_key_in_json_object_value) {
    EXPECT_THROW(
        {
            try {
                // clang-format off
                std::string jsonStr = "test={'test";
                // clang-format on
                blet::Dict conf = blet::conf::loadString(jsonStr);
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:12 (End of quote)");
                EXPECT_EQ(e.message(), "End of quote");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 12);
                throw;
            }
        },
        blet::conf::LoadException);
}

GTEST_TEST(loadString, end_of_key_in_json_object_value) {
    EXPECT_THROW(
        {
            try {
                // clang-format off
                std::string jsonStr = "test={test";
                // clang-format on
                blet::Dict conf = blet::conf::loadString(jsonStr);
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:11 (End of key)");
                EXPECT_EQ(e.message(), "End of key");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 11);
                throw;
            }
        },
        blet::conf::LoadException);
}

GTEST_TEST(loadString, operator_EQ_not_found_in_json_object_value) {
    EXPECT_THROW(
        {
            try {
                // clang-format off
                std::string jsonStr = "test={'test'";
                // clang-format on
                blet::Dict conf = blet::conf::loadString(jsonStr);
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:13 (Assign operator not found)");
                EXPECT_EQ(e.message(), "Assign operator not found");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 13);
                throw;
            }
        },
        blet::conf::LoadException);
}

GTEST_TEST(loadString, end_of_quote_at_value_in_json_object_value) {
    EXPECT_THROW(
        {
            try {
                // clang-format off
                std::string jsonStr = "test={'test'='test";
                // clang-format on
                blet::Dict conf = blet::conf::loadString(jsonStr);
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:19 (End of quote)");
                EXPECT_EQ(e.message(), "End of quote");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 19);
                throw;
            }
        },
        blet::conf::LoadException);
}

GTEST_TEST(loadString, end_of_json_array_value) {
    EXPECT_THROW(
        {
            try {
                // clang-format off
                std::string jsonStr = "test=[";
                // clang-format on
                blet::Dict conf = blet::conf::loadString(jsonStr);
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:7 (End of array)");
                EXPECT_EQ(e.message(), "End of array");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 7);
                throw;
            }
        },
        blet::conf::LoadException);
}

GTEST_TEST(loadString, end_of_quote_in_json_array_value) {
    EXPECT_THROW(
        {
            try {
                // clang-format off
                std::string jsonStr = "test=['test";
                // clang-format on
                blet::Dict conf = blet::conf::loadString(jsonStr);
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:12 (End of quote)");
                EXPECT_EQ(e.message(), "End of quote");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 12);
                throw;
            }
        },
        blet::conf::LoadException);
}

GTEST_TEST(loadString, end_of_value_in_json_array_value) {
    EXPECT_THROW(
        {
            try {
                // clang-format off
                std::string jsonStr = "test=['test'=";
                // clang-format on
                blet::Dict conf = blet::conf::loadString(jsonStr);
            }
            catch (const blet::conf::LoadException& e) {
                EXPECT_STREQ(e.what(), "Load at 1:13 (End of value)");
                EXPECT_EQ(e.message(), "End of value");
                EXPECT_EQ(e.filename(), "");
                EXPECT_EQ(e.line(), 1);
                EXPECT_EQ(e.column(), 13);
                throw;
            }
        },
        blet::conf::LoadException);
}
