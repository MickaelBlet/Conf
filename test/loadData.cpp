#include <gtest/gtest.h>

#include "blet/conf.h"

GTEST_TEST(loadData, comment) {
    // clang-format off
    const char confStr[] = ""
        "; comment\n"
        "# comment";
    // clang-format on

    blet::Dict conf = blet::conf::loadData(confStr, sizeof(confStr) - 1);
    EXPECT_TRUE(conf.isNull());
}