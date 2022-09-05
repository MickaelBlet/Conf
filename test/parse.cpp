#include <gtest/gtest.h>
#include <cmath>

#include "mblet/configator.hpp"

GTEST_TEST(parser, s_emptyOrComment) {
    std::string strConf("   \n"
                        ";    comment \n"
                        "; new line\n"
                        "\n"
                       );

    mblet::Configator conf;
    conf.readString(strConf);
    EXPECT_EQ(conf[""].getComment(), "comment\nnew line");
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
    EXPECT_EQ(conf["\" "].getComment(), "in section");
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
    EXPECT_EQ(conf["section"]["\" "].getComment(), "in section");
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
    EXPECT_EQ(conf[""]["key"].getString(), "value");
}

GTEST_TEST(parser, s_hex) {
    std::string strConf("hex=0x0Aa ; comment \n"
                        "nohex=0x0AaU ; comment \n"
                       );
    mblet::Configator conf;
    conf.readString(strConf);
    EXPECT_EQ(conf[""]["hex"].getNumber(), 170);
    EXPECT_EQ(std::isnan(conf[""]["nohex"].getNumber()), true);
}

GTEST_TEST(parser, s_binary) {
    std::string strConf("binary=0b0101 ; comment \n"
                        "nobinary=0b2 ; comment \n"
                       );
    mblet::Configator conf;
    conf.readString(strConf);
    EXPECT_EQ(conf[""]["binary"].getNumber(), 5);
    EXPECT_EQ(std::isnan(conf[""]["nobinary"].getNumber()), true);
}

GTEST_TEST(parser, s_octal) {
    std::string strConf("octal=0101 ; comment \n"
                        "nooctal=02a ; comment \n"
                       );
    mblet::Configator conf;
    conf.readString(strConf);
    EXPECT_EQ(conf[""]["octal"].getNumber(), 65);
    EXPECT_EQ(std::isnan(conf[""]["nooctal"].getNumber()), true);
}