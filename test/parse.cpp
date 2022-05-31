#include <gtest/gtest.h>

#include "configator.hpp"

GTEST_TEST(parser, s_emptyOrComment) {
    std::string strConf("   \n"
                        ";    comment \n"
                        "; new line\n"
                        "\n"
                       );

    mblet::Configator conf;
    conf.readString(strConf);
    EXPECT_EQ(conf[""].comment, "comment\nnew line");
}

GTEST_TEST(parser, s_parseSections) {
    std::string strConf("[section]\n"
                        "[ \"\\\" \" ] ; in section \n"
                        "[ ; bad ending section\n"
                        "[ \" ; bad section quote\n"
                        "[ \"\" ; bad ending section quote\n"
                        "[] NOOOO ; bad characters ending section\n"
                       );

    mblet::Configator conf;
    conf.readString(strConf);
    EXPECT_EQ(conf["\" "].comment, "in section");
}

GTEST_TEST(parser, s_parseSectionLevel) {
    std::string strConf("[section]\n"
                        "[[ \"\\\" \" ]] ; in section \n"
                        "[[] ; bad ending section\n"
                        "[[ \" ; bad section quote\n"
                        "[[ \"\" ; bad ending section quote\n"
                        "[[]] NOOOO ; bad characters ending section\n"
                        "[[[[]]]] ; bad section number\n"
                       );

    mblet::Configator conf;
    conf.readString(strConf);
    EXPECT_EQ(conf["section"]["\" "].comment, "in section");
}

GTEST_TEST(parser, s_parseKey) {
    std::string strConf("key=value ; comment \n"
                        "=\n"
                        "k\n"
                        "\"k\\\"\n"
                        "\"k\" NOOO \n"
                        "\"k\" = \"\\\"\n"
                        "k[\"\\\"] = \"\\\"\n"
                        "k[ a ] = \"\\\"\n"
                        "\"k\" = \"\" NOOO \n"
                        "k[\"\"] = \"\\\"\n"
                       );
    mblet::Configator conf;
    conf.readString(strConf);
    EXPECT_EQ(conf[""]["key"].value, "value");
}