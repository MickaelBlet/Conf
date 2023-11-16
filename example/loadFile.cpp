#include <iostream>

#include "blet/conf.h"

int main(int /*argc*/, char* /*argv*/[]) {
    const blet::Dict conf = blet::conf::loadFile("./example/example.conf");
    std::cout << conf["world"]["hello"] << std::endl; // 42
    return 0;
}