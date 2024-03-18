#include <iostream>

#include "blet/conf.h"

int main(int /*argc*/, char* /*argv*/[]) {
    const char str[] =
        "[world]\n"
        "hello = 42";
    const blet::Dict conf = blet::conf::loadData(str, sizeof(str) - 1);
    int value = conf["world"]["hello"];
    std::cout << value << std::endl; // 42
    return 0;
}
