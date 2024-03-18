#include <iostream>

#include "blet/conf.h"

static void printDumps(const blet::Dict& dict) {
    static int counter = 0;
    if (counter > 0) {
        std::cout << '\n' << std::string(40, '-') << '\n';
    }
    std::cout << "# Conf style\n";
    std::cout << blet::conf::dump(dict, 2, ' ', blet::conf::CONF_STYLE) << std::endl << '\n';
    std::cout << "# Json style\n";
    std::cout << blet::conf::dump(dict, 2, ' ', blet::conf::JSON_STYLE) << std::endl;
    ++counter;
}

int main(int /*argc*/, char* /*argv*/[]) {
    // simple values
    {
        blet::Dict dict;
        dict["foo"] = "bar";
        dict["number"] = 42;
        dict["boolean"] = true;
        dict["null"].newNull();
        dict["array"][0] = 4;
        dict["array"][1] = 2;
        printDumps(dict);
    }
    // simple values in section
    {
        blet::Dict dict;
        dict["nameOfSection"]["foo"] = "bar";
        dict["nameOfSection"]["number"] = 42;
        dict["nameOfSection"]["boolean"] = true;
        dict["nameOfSection"]["null"].newNull();
        dict["nameOfSection"]["array"][0] = 4;
        dict["nameOfSection"]["array"][1] = 2;
        printDumps(dict);
    }
    // simple values in sections
    {
        blet::Dict dict;
        dict["nameOfSection1"]["foo"] = "bar";
        dict["nameOfSection1"]["number"] = 42;
        dict["nameOfSection1"]["boolean"] = true;
        dict["nameOfSection2"]["null"].newNull();
        dict["nameOfSection2"]["array"][0] = 4;
        dict["nameOfSection2"]["array"][1] = 2;
        printDumps(dict);
    }
    // simple values in multi sections
    {
        blet::Dict dict;
        dict["nameOfSection1"]["sectionChild1"]["foo"] = "bar";
        dict["nameOfSection1"]["sectionChild2"]["number"] = 42;
        dict["nameOfSection1"]["sectionChild3"]["boolean"] = true;
        dict["nameOfSection2"]["sectionChild1"]["null"].newNull();
        dict["nameOfSection2"]["sectionChild2"]["array"][0] = 4;
        dict["nameOfSection2"]["sectionChild2"]["array"][1] = 2;
        printDumps(dict);
    }
    return 0;
}
