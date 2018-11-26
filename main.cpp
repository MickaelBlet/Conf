#include "Configator.hpp"

struct TestConfig
{
    std::string     valueString;
    bool            valueBool;
    int             valueInt;
    long            valueLong;
    unsigned long   valueULong;
    double          valueDouble;
};

int main()
{
    using namespace Config;

    Configator conf("./test.ini");
    if (conf.isRead() == false)
    {
        std::cerr << "Open configuration Fail" << std::endl;
        return 1;
    }

    TestConfig testConfig;

    if (conf.getValue("", "42", testConfig.valueString, "0") == false)
        std::cerr << "not find key: " << "42" << " in section: [" << "" << "]" << std::endl;
    if (conf.getValue("", "42", testConfig.valueBool, false) == false)
        std::cerr << "not find key: " << "42" << " in section: [" << "" << "]" << std::endl;
    if (conf.getValue("", "42", testConfig.valueInt, 0) == false)
        std::cerr << "not find key: " << "42" << " in section: [" << "" << "]" << std::endl;
    if (conf.getValue("", "42", testConfig.valueLong, 0) == false)
        std::cerr << "not find key: " << "42" << " in section: [" << "" << "]" << std::endl;
    if (conf.getValue("", "42", testConfig.valueULong, 0) == false)
        std::cerr << "not find key: " << "42" << " in section: [" << "" << "]" << std::endl;
    if (conf.getValue("", "42", testConfig.valueDouble, 0) == false)
        std::cerr << "not find key: " << "42" << " in section: [" << "" << "]" << std::endl;

    std::cout << "string: " << testConfig.valueString << std::endl;
    std::cout << "bool  : " << testConfig.valueBool << std::endl;
    std::cout << "int   : " << testConfig.valueInt << std::endl;
    std::cout << "long  : " << testConfig.valueLong << std::endl;
    std::cout << "ulong : " << testConfig.valueULong << std::endl;
    std::cout << "double: " << testConfig.valueDouble << std::endl;

    return 0;
}