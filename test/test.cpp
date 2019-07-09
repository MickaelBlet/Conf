#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstring>

#include "configator.hpp"

using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

GTEST_TEST(Configator, test1)
{
    mblet::Configator conf("./test1.ini");
    std::cout << conf["test"]["42"] << std::endl;
}

GTEST_TEST(Configator, test2)
{
    mblet::Configator conf("./test2.ini");
    std::cout << conf["test"]["0"].get<int>() << std::endl;
    std::cout << conf["test"]["1"].get<int>() << std::endl;
    std::cout << conf["test"]["2"].get<int>() << std::endl;
    std::cout << conf["test"]["3"].get<int>() << std::endl;
}