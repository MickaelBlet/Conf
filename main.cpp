#include "SimpleIniReader.hpp"

int main(void)
{
    SimpleIniReader   configIni("./test1.ini");

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
        std::cout << "TEST1 KO !" << std::endl;
        std::cerr << "Error: " << e.what() << std::endl;
    }

    std::cout << "IsRead ? " << std::boolalpha << configIni.IsRead() << std::endl;

    return 0;
}