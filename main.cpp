#include "SimpleIniReader.hpp"

int main(void)
{
    // SimpleIniReader   configIni("./test.1.ini");
    Configator   configIni("./test.ini");

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
        configIni.FillMap(readMap);
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
    std::cout << "show >>>> " << configIni.getValue("test", "3") << std::endl;
    return 0;
}