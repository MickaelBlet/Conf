#include <gtest/gtest.h>
#include <stdint.h>

#include "configator.hpp"

GTEST_TEST(configurator_getFilename, success) {
    const char* testFile = "test.ini";
    // create example file
    std::ofstream fileStream(testFile, std::ofstream::out | std::ofstream::trunc);
    fileStream.close();
    mblet::Configator conf;
    EXPECT_EQ(conf.readFile(testFile), true);
    // remove example file
    remove(testFile);
    EXPECT_EQ(conf.getFilename(), "test.ini");
}

GTEST_TEST(configurator_getFilename, failure) {
    const char* testFile = "test.ini";
    // remove example file
    remove(testFile);
    mblet::Configator conf;
    EXPECT_EQ(conf.readFile(testFile), false);
    EXPECT_EQ(conf.getFilename(), "");
}

GTEST_TEST(configurator_getFilename, test) {
    const char* testFile = "test.ini";
    // create example file
    std::ofstream fileStream(testFile, std::ofstream::out | std::ofstream::trunc);
    fileStream  << "[test]"                     << "\n"
                << "[[test]]     ;titi"              << "\n"
                << "[[[test]]] ; teststst"                   << "\n"
                << "test = 42   ;    yooo   "   << "\n"
                << "[[test2]]     ;titi"              << "\n"
                << "[[[test]]] ; teststst"                   << "\n"
                << "test = 42   ;    yooo   "   << "\n"
                << "[[[test2]]] ; teststst"                   << "\n"
                << "test = 42   ;    yooo   "   << "\n"
                << "[[[[test2]]]] ; teststst"                   << "\n"
                << "test2 = 42   ;    yooo   "   << "\n"
                << "[]" << "\n"
                << "[[]]" << "\n"
                << "test = 42   ;    yooo   "   << "\n"
                << "[1][2][3]" << "\n"
                << "test = 42   ;    yooo   "   << "\n";
    fileStream.close();
    mblet::Configator conf;
    EXPECT_EQ(conf.readFile(testFile), true);
    std::cout << conf["test"]["test"].comment << std::endl;
    conf.dump();
    // remove example file
    remove(testFile);
    EXPECT_EQ(conf.getFilename(), "test.ini");
}

// GTEST_TEST(configurator_readFile, isRead) {
//     // create example file
//     std::ofstream fileStream("test.ini", std::ofstream::out | std::ofstream::trunc);
//     fileStream << "[test] # test           \n"
//                << "42=-42.42 ;         testest";
//     fileStream.close();

//     mblet::Configator conf("test.ini");
//     remove("test.ini");
//     EXPECT_EQ(conf.isRead(), true);
//     EXPECT_EQ(conf["test"]["42"].get<std::string>(), "-42.42");
//     EXPECT_EQ(conf["test"]["42"].get<float>(), -42.42f);
//     EXPECT_EQ(conf["test"]["42"].get<double>(), -42.42);
//     EXPECT_EQ(conf["test"]["42"].get<int>(), (int)-42);
//     EXPECT_EQ(conf["test"]["42"].get<uint32_t>(), (uint32_t)-42);
//     EXPECT_EQ(conf["test"]["42"].get<uint64_t>(), (uint64_t)-42);
// }

// int main(void)
// {
//     std::ostringstream oss("");

//     mblet::Configator conf1("./test1.ini");
//     if (conf1.isRead() == false)
//     {
//         std::cerr << "Open configuration Fail: " << "./test1.ini" << std::endl;
//         return 1;
//     }
//     std::cout << conf1["test"]["42"] << std::endl;
//     oss << std::endl << conf1.getFilename() << std::endl;
//     conf1.dump(oss, 2);

//     mblet::Configator conf2("./test2.ini");
//     if (conf2.isRead() == false)
//     {
//         std::cerr << "Open configuration Fail: " << "./test2.ini" << std::endl;
//         return 1;
//     }
//     std::cout << conf2["test"]["0"].get<int>() << std::endl;
//     std::cout << conf2["test"]["1"].get<int>() << std::endl;
//     std::cout << conf2["test"]["2"].get<int>() << std::endl;
//     std::cout << conf2["test"]["3"].get<int>() << std::endl;
//     std::cout << conf2["test"]["4"].get<int>() << std::endl;
//     oss << std::endl << conf2.getFilename() << std::endl;
//     conf2.dump(oss, 2);

//     mblet::Configator conf3("./test3.ini");
//     if (conf3.isRead() == false)
//     {
//         std::cerr << "Open configuration Fail: " << "./test3.ini" << std::endl;
//         return 1;
//     }
//     std::cout << conf3["test"]["table"][0].get<int>() << std::endl;
//     std::cout << conf3["test"]["table"][1].get<int>() << std::endl;
//     std::cout << conf3["test"]["map"]["key"] << std::endl;
//     std::cout << conf3["test"]["map"]["key1"]["key2"] << std::endl;
//     oss << std::endl << conf3.getFilename() << std::endl;
//     conf3.dump(oss, 2);

//     mblet::Configator conf4("./test4.ini");
//     if (conf4.isRead() == false)
//     {
//         std::cerr << "Open configuration Fail: " << "./test4.ini" << std::endl;
//         return 1;
//     }
//     std::cout << conf4["test"]["32[\\]"] << std::endl;
//     std::cout << conf4["test"]["42"]["key[]"] << std::endl;
//     std::cout << conf4["test"]["42 space  "]["key[]"] << std::endl;
//     oss << std::endl << conf4.getFilename() << std::endl;
//     conf4.dump(oss, 2);

//     std::cout << oss.str();

//     return 0;
// }