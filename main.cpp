#include <iostream>

#include "configator.hpp"

int main(void)
{
    mblet::Configator conf1("./test1.ini");
    if (conf1.isRead() == false)
    {
        std::cerr << "Open configuration Fail: " << "./test1.ini" << std::endl;
        return 1;
    }
    std::cout << conf1["test"]["42"] << std::endl;
    std::cout << conf1.dump() << std::endl;

    mblet::Configator conf2("./test2.ini");
    if (conf2.isRead() == false)
    {
        std::cerr << "Open configuration Fail: " << "./test2.ini" << std::endl;
        return 1;
    }
    std::cout << conf2["test"]["0"].get<int>() << std::endl;
    std::cout << conf2["test"]["1"].get<int>() << std::endl;
    std::cout << conf2["test"]["2"].get<int>() << std::endl;
    std::cout << conf2["test"]["3"].get<int>() << std::endl;
    std::cout << conf2["test"]["4"].get<int>() << std::endl;
    std::cout << conf2.dump() << std::endl;

    mblet::Configator conf3("./test3.ini");
    if (conf3.isRead() == false)
    {
        std::cerr << "Open configuration Fail: " << "./test3.ini" << std::endl;
        return 1;
    }
    std::cout << conf3["test"]["table"][0].get<int>() << std::endl;
    std::cout << conf3["test"]["table"][1].get<int>() << std::endl;
    std::cout << conf3["test"]["map"]["key"] << std::endl;
    std::cout << conf3["test"]["map"]["key1"]["key2"] << std::endl;
    std::cout << conf3.dump() << std::endl;

    mblet::Configator conf4("./test4.ini");
    if (conf4.isRead() == false)
    {
        std::cerr << "Open configuration Fail: " << "./test4.ini" << std::endl;
        return 1;
    }
    std::cout << conf4["test"]["32[\\]"] << std::endl;
    std::cout << conf4["test"]["42"]["key[]"] << std::endl;
    std::cout << conf4.dump() << std::endl;

    return 0;
}