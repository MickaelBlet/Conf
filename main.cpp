#include "Configator.hpp"
// #include "test.hpp"

void test123(void);

struct config
{
    std::string valueString;
    bool valueBool;
    int valueInt;
    long valueLong;
};

int main(void)
{
    using namespace Conf;

    config c;
    // SimpleIniReader   configIni("./test.1.ini");
    Configator   configIni("./test.ini");
    
    configIni.fillMap({
        {"", {
            {"valueString", &c.valueString, "0"},
            {"valueBool", &c.valueBool, true}
        }}
    });

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

    // Configator::MapConfig map = configIni.getConfig();
    // std::cout << "test >>>> " << map["test"]["1"].section["1"] << std::endl;
    // map["test"]["1"].section["3"] = "yolo";
    // std::cout << "test >>>> " << map["test"]["1"].key << std::endl;
    // std::cout << "test >>>> " << map["test"]["3"] << std::endl;

    configIni.PrintConfig();
    configIni.setKey("test", "3", "yolo ");
    std::cout << "show >>>> " << configIni.getValueString("test", "3", "") << std::endl;
    std::cout << "show >>>> " << configIni.getValueLong("test", "3", 0) << std::endl;
    std::cout << "show >>>> " << configIni.getValueLong("", "1", 0) << std::endl;
    std::cout << "show >>>> " << configIni.getValueString("", "2", "") << std::endl;
    std::cout << "show >>>> " << configIni.getValueUnsignedLong("", "2", 0) << std::endl;
    std::cout << "show >>>> " << configIni.getValueLong("", "3", 0) << std::endl;
    std::cout << "show >>>> " << configIni.getValueDouble("", "4", 0) << std::endl;
    std::cout << "show >>>> " << configIni.getValueDouble("test", "6978", 0) << std::endl;
    std::cout << "show >>>> " << configIni.getValueDouble("", "5", 0) << std::endl;

    std::cout << "show >>>> " << Config.getValueString("1", "1", "") << std::endl;
    std::cout << "show >>>> " << Config.getValueLong("1", "2", 0) << std::endl;
    std::cout << "show >>>> " << Config.getValueLong("2", "1", 0) << std::endl;
    const std::string &test = Config.getValue<const std::string &>("2", "2", "1.23456789");
    std::cout << "show >>>> !!! " << test << std::endl;
    std::cout << "show int >>>> " << (int)Config.getValue<int>("2", "2", 2) << std::endl;
    std::cout << "show long >>>> " << (long)Config.getValue<long>("2", "2", 2) << std::endl;
    std::cout << "show unsigned long >>>> " << (unsigned long)Config.getValue<unsigned long>("2", "2", 2) << std::endl;
    std::cout << "show double        >>>> " << (double)Config.getValue<double>("2", "2", 2) << std::endl;
    std::cout << "show long double   >>>> " << (long double)Config.getValue<long double>("2", "2", 2) << std::endl;
    std::cout << "show std::string   >>>> " << (std::string)Config.getValue<std::string>("2", "2", "2") << std::endl;
    std::cout << std::endl;
    Config.setKey("2", "2", 0 == 0);
    std::cout << (std::string)Config._mapConfig["2"]["2"].typeString << std::endl;
    std::cout << "show long double   >>>> " << (long double)Config.getValue<long double>("2", "2", 2) << std::endl;
    std::cout << "show std::string   >>>> " << (std::string)Config.getValue<std::string>("2", "2", "2") << std::endl;
    test123();
    std::cout << "isType int           >>>> " << std::boolalpha << Config.isType<int>("2", "2") << std::endl;
    std::cout << "isType long          >>>> " << std::boolalpha << Config.isType<long>("2", "2") << std::endl;
    std::cout << "isType unsigned long >>>> " << std::boolalpha << Config.isType<unsigned long>("2", "2") << std::endl;
    std::cout << "isType double        >>>> " << std::boolalpha << Config.isType<double>("2", "2") << std::endl;
    std::cout << "isType long double   >>>> " << std::boolalpha << Config.isType<long double>("2", "2") << std::endl;
    return 0;
}