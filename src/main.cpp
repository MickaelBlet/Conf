#include <iostream>

#include "configator.hpp"

struct TestConfig
{
    std::string     valueString;
    bool            valueBool;
    int             valueInt;
    long            valueLong;
    unsigned long   valueULong;
    double          valueDouble;
};

int main(void)
{
    // using namespace mblet;

    mblet::Configator conf("./test.ini");
    if (conf.isRead() == false)
    {
        std::cerr << "Open configuration Fail" << std::endl;
        return 1;
    }

    int i = conf[""]["42"].get<int>();
    double d = conf[""]["42"].get<double>();

    std::cout << i << std::endl;
    std::cout << d << std::endl;

    // for (const auto &item : conf["test"]["k"])
    // {
    //     std::cout << item.first << ":" << item.second << std::endl;
    // }

    // Configator::Map map;

    // map["TEST"]["2"] = "3";
    // map["1"]["2"]["3"] = "4";

    // std::string test = map["1"]["2"]["3"].value;

    // std::cout << test << std::endl;

    // conf.setConfig(map);

    // conf[1][0][0] = "5";

    // std::cout << conf["1"]["2"]["3"].value << std::endl;

    // TestConfig testConfig;

    // // testConfig.valueString = conf.getValue<std::string>("", "42");
    // testConfig.valueString = conf.getValue<std::string>("TEST", "2");
    // testConfig.valueBool = conf.getValue<bool>("TEST", "2");
    // // testConfig.valueInt = conf.getValue("", "42");
    // // testConfig.valueLong = conf.getValue("", "42");
    // // testConfig.valueULong = conf.getValue("", "42");
    // // testConfig.valueDouble = conf.getValue("", "42");

    // std::cout << "string: " << testConfig.valueString << std::endl;
    // std::cout << "bool  : " << std::boolalpha << testConfig.valueBool << std::endl;
    // std::cout << "int   : " << testConfig.valueInt << std::endl;
    // std::cout << "long  : " << testConfig.valueLong << std::endl;
    // std::cout << "ulong : " << testConfig.valueULong << std::endl;
    // std::cout << "double: " << testConfig.valueDouble << std::endl;

    std::cout << conf.dump(2) << std::endl;

    return 0;
}