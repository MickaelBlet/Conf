#include <iostream>

#include "blet/conf.h"

int main(int /*argc*/, char* /*argv*/[]) {
    const std::string str(
        "[world]\n"
        "hello = 42");
    const blet::Dict conf = blet::conf::loadString(str);
    int value = conf["world"]["hello"];
    std::cout << value << std::endl; // 42
    return 0;
}