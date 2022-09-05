#include <gtest/gtest.h>

#include "mblet/configator.hpp"

GTEST_TEST(dump, success) {
    std::string strConf("[1] ; 1\n"
                        "; 1\n"
                        "1=1 ; 2\n"
                        " \"2 \" = \"\\\" \" ; 3 3 \n"
                        "2 = woot ?\n"
                        "[[2]]\n"
                        " \"2  \" = \"\\\" \" ; 3 3 \n"
                        "\" 2 \" = \"\\\" \" ; 3 3 \n"
                        "2[1] = 3 ; 4\n"
                        "\n"
                       );

    mblet::Configator conf;
    conf.readString(strConf);
    std::ostringstream oss;
    conf.dump(oss);
    EXPECT_EQ(conf["1"]["2 "].getString(), "\" ");
    EXPECT_EQ(conf["1"]["2"]["2  "].getString(), "\" ");
    EXPECT_EQ(conf["1"]["2"][" 2 "].getString(), "\" ");
    EXPECT_EQ(conf["1"]["2"]["2"]["1"].getString(), "3");
    EXPECT_EQ(oss.str(),
              "[1]\n; 1\n; 1\n1 = 1 ; 2\n\"2 \" = \"\\\" \" ; 3 3\n2 = \"woot ?\"\n[[2]]\n\" 2 \" = \"\\\" \" ; 3 3\n\"2  \" = \"\\\" \" ; 3 3\n[[[2]]]\n1 = 3 ; 4\n");
    mblet::Configator conf2;
    conf2.readString(oss.str());
    std::ostringstream oss2;
    conf2.dump(oss2);
    EXPECT_EQ(conf2["1"]["2 "].getString(), "\" ");
    EXPECT_EQ(conf2["1"]["2"]["2  "].getString(), "\" ");
    EXPECT_EQ(conf2["1"]["2"][" 2 "].getString(), "\" ");
    EXPECT_EQ(conf2["1"]["2"]["2"]["1"].getString(), "3");
    EXPECT_EQ(oss2.str(),
              "[1]\n; 1\n; 1\n1 = 1 ; 2\n\"2 \" = \"\\\" \" ; 3 3\n2 = \"woot ?\"\n[[2]]\n\" 2 \" = \"\\\" \" ; 3 3\n\"2  \" = \"\\\" \" ; 3 3\n[[[2]]]\n1 = 3 ; 4\n");
}