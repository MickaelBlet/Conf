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
    using namespace Config;

    Configator conf("./test.ini");
    if (conf.isRead() == false)
    {
        std::cerr << "Open configuration Fail" << std::endl;
        return 1;
    }

    TestConfig testConfig;

    testConfig.valueString = conf.getValue<decltype(testConfig.valueString)>("", "42");
    testConfig.valueBool = conf.getValue<decltype(testConfig.valueBool)>("", "42");
    testConfig.valueInt = conf.getValue<decltype(testConfig.valueInt)>("", "42");
    testConfig.valueLong = conf.getValue<decltype(testConfig.valueLong)>("", "42");
    testConfig.valueULong = conf.getValue<decltype(testConfig.valueULong)>("", "42");
    testConfig.valueDouble = conf.getValue<decltype(testConfig.valueDouble)>("", "42");

    std::cout << "string: " << testConfig.valueString << std::endl;
    std::cout << "bool  : " << testConfig.valueBool << std::endl;
    std::cout << "int   : " << testConfig.valueInt << std::endl;
    std::cout << "long  : " << testConfig.valueLong << std::endl;
    std::cout << "ulong : " << testConfig.valueULong << std::endl;
    std::cout << "double: " << testConfig.valueDouble << std::endl;

    conf.print();

    return 0;
}