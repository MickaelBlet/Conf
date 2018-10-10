#include "Configator.hpp"
// #include "test.hpp"

void test123(void);

struct config
{
    std::string valueString;
    bool        valueBool;
    int         valueInt;
    long        valueLong;
};

int main(void)
{
    using namespace Conf;

    // config c;
    // SimpleIniReader   configIni("./test.1.ini");
    Configator   configIni("./test.ini");
    
    // configIni.fillMap({
    //     {"", {
    //         {"valueString", &c.valueString, std::string("0")},
    //         {"valueBool", &c.valueBool, true},
    //         {"valueLong", &c.valueLong, 0}
    //     }}
    // });

    // std::cout << c.valueString << std::endl;
    // std::cout << std::boolalpha << c.valueBool << std::endl;
    // std::cout << c.valueLong << std::endl;

    Configator  Config({
        {"", {
            {"1",""},
            {"2",""},
        }},
        {"1", {
            {"1","1"},
            {"2","2"},
        }},
        {"2", {
            {"1","3"},
            {"2",""},
        }},
    });

    std::map<const std::string, std::map<const std::string, std::string> >  readMap = {
        {"", {
            {"1",""},
            {"2",""},
        }},
        {"1", {
            {"1",""},
            {"2",""},
        }},
        {"2", {
            {"1",""},
            {"2",""},
        }},
    };
    
    std::cout << "TEST1:" << std::endl;

    try
    {
        // configIni.FillMap(readMap);
        std::cout << "TEST1 OK" << std::endl;
    }
    catch (std::exception &e)
    {
        // std::cout << "TEST1 KO !" << std::endl;
        // std::cerr << "Error: " << e.what() << std::endl;
    }

    std::cout << "IsRead ? " << std::boolalpha << configIni.isRead() << std::endl;

    configIni.PrintConfig();
    configIni.setKey("test", "3", "true");

    std::map<std::string, std::map<std::string, std::string>> map = configIni.getConfig();

    std::string stringValue;
    bool boolValue;
    int intValue;
    // long longValue;
    unsigned long long ullValue;
    double doubleValue;
    std::cout << "show >>>> " << configIni.getValue("", "5", stringValue, "OOO") << std::endl;
    std::cout << "show >>>> " << configIni.getValue("", "5", boolValue, false) << std::endl;
    std::cout << "show >>>> " << configIni.getValue("", "5", intValue, 0) << std::endl;
    std::cout << "show >>>> " << configIni.getValue("", "5", ullValue, 0) << std::endl;
    std::cout << "show >>>> " << configIni.getValue("", "5", doubleValue, 0) << std::endl;

    std::cout << "show >>>> " << configIni.getValue("test", "3", stringValue, "OOO") << std::endl;
    std::cout << "show >>>> " << configIni.getValue("test", "3", boolValue, false) << std::endl;
    std::cout << "show >>>> " << configIni.getValue("test", "3", intValue, 0) << std::endl;
    std::cout << "show >>>> " << configIni.getValue("test", "3", ullValue, 0) << std::endl;
    std::cout << "show >>>> " << configIni.getValue("test", "3", doubleValue, 0) << std::endl;
    // std::cout << "show >>>> " << configIni.getValue("", "2", "") << std::endl;
    // std::cout << "show >>>> " << configIni.getValue("", "2", 0) << std::endl;
    // std::cout << "show >>>> " << configIni.getValue("", "3", 0) << std::endl;
    // std::cout << "show >>>> " << configIni.getValue("", "4", 0) << std::endl;
    // std::cout << "show >>>> " << configIni.getValue("test", "6978", 0) << std::endl;
    // std::cout << "show >>>> " << configIni.getValue("", "5", 0) << std::endl;

    // std::cout << "show >>>> " << Config.getValueString("1", "1", "") << std::endl;
    // std::cout << "show >>>> " << Config.getValueLong("1", "2", 0) << std::endl;
    // std::cout << "show >>>> " << Config.getValueLong("2", "1", 0) << std::endl;
    // const std::string &test = Config.getValue<const std::string &>("2", "2", "1.23456789");
    // std::cout << "show >>>> !!! " << test << std::endl;
    // std::cout << "show int >>>> " << (int)Config.getValue<int>("2", "2", 2) << std::endl;
    // std::cout << "show long >>>> " << (long)Config.getValue<long>("2", "2", 2) << std::endl;
    // std::cout << "show unsigned long >>>> " << (unsigned long)Config.getValue<unsigned long>("2", "2", 2) << std::endl;
    // std::cout << "show double        >>>> " << (double)Config.getValue<double>("2", "2", 2) << std::endl;
    // std::cout << "show long double   >>>> " << (long double)Config.getValue<long double>("2", "2", 2) << std::endl;
    // std::cout << "show std::string   >>>> " << (std::string)Config.getValue<std::string>("2", "2", "2") << std::endl;
    // std::cout << std::endl;
    // Config.setKey("2", "2", 0 == 0);
    // std::cout << "show long double   >>>> " << (long double)Config.getValue<long double>("2", "2", 2) << std::endl;
    // std::cout << "show std::string   >>>> " << (std::string)Config.getValue<std::string>("2", "2", "2") << std::endl;
    // test123();
    // std::cout << "isType int           >>>> " << std::boolalpha << Config.isType<int>("2", "2") << std::endl;
    // std::cout << "isType long          >>>> " << std::boolalpha << Config.isType<long>("2", "2") << std::endl;
    // std::cout << "isType unsigned long >>>> " << std::boolalpha << Config.isType<unsigned long>("2", "2") << std::endl;
    // std::cout << "isType double        >>>> " << std::boolalpha << Config.isType<double>("2", "2") << std::endl;
    // std::cout << "isType long double   >>>> " << std::boolalpha << Config.isType<long double>("2", "2") << std::endl;
    return 0;
}