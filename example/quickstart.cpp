#include <iostream>

#include "blet/conf.h"

int main(int /*argc*/, char* /*argv*/[]) {
    const blet::Dict conf = blet::conf::loadFile("./example/quickstart.conf");

    std::cout << conf["foo"]["0"] << std::endl;                // null
    std::cout << conf["foo"]["1"] << std::endl;                // true
    std::cout << conf["foo"]["2"] << std::endl;                // 42.42
    std::cout << conf["foo"]["3"] << std::endl;                // str
    std::cout << conf["foo"]["nextLevel"]["foo"] << std::endl; // bar
    std::cout << conf["foo"]["nextLevel"]["bar"] << std::endl; // escape character
    std::cout << conf["bar"]["foo"][0] << std::endl;           // 1
    std::cout << conf["bar"]["foo"][1] << std::endl;           // 2
    std::cout << conf["bar"]["foo"][2] << std::endl;           // 3

    std::cout << conf["foo"]["4"]["foo"]["bar"] << std::endl; // 42

    const blet::Dict& parents = conf["parents"];
    for (std::size_t i = 0; i < parents.size(); ++i) {
        const std::string parentName = parents[i]["name"];
        const int parentAge = parents[i]["age"];
        const char parentGender = parents[i]["gender"].getString()[0];
        std::cout << parentName << std::endl;
        std::cout << parentAge << std::endl;
        std::cout << parentGender << std::endl;
        const blet::Dict& childs = parents[i]["childs"];
        for (std::size_t j = 0; j < childs.size(); ++j) {
            const std::string childName = childs[j]["name"];
            const int childAge = childs[j]["age"];
            const char childGender = childs[j]["gender"].getString()[0];
            std::cout << childName << std::endl;   // toto, titi, tutu
            std::cout << childAge << std::endl;    // 2, 4, 0.5
            std::cout << childGender << std::endl; // M, F, F
        }
    }

    std::cout << blet::conf::dump(conf, 2) << std::endl;
    // output:
    // parents[] = {
    //   age = 42,
    //   childs = [
    //     {
    //       age = 2,
    //       gender = "M",
    //       name = "toto"
    //     },
    //     {
    //       age = 4,
    //       gender = "F",
    //       name = "titi"
    //     },
    //     {
    //       age = 0.5,
    //       gender = "F",
    //       name = "tutu"
    //     }
    //   ],
    //   gender = "M",
    //   name = "brian"
    // }
    // parents[] = {
    //   age = 42,
    //   childs = [],
    //   gender = "M",
    //   name = "toto"
    // }
    // parents[] = {
    //   age = 42,
    //   childs = [],
    //   emptyObj = {},
    //   gender = "F",
    //   name = "clara"
    // }
    // [bar]
    // foo[] = 1
    // foo[] = 2
    // foo[] = 3
    // [foo]
    // 0 = null
    // 1 = true
    // 2 = 42.42
    // 3 = "str"
    // [[4]]
    // [[[foo]]]
    // bar = 42
    // [[nextLevel]]
    // bar = "escape character\n\tstart newline with tab"
    // foo = "bar"
    std::cout << blet::conf::dump(conf, 2, ' ', blet::conf::JSON_STYLE) << std::endl;
    // output:
    // bar = {
    //   foo = [
    //     1,
    //     2,
    //     3
    //   ]
    // }
    // foo = {
    //   0 = null,
    //   1 = true,
    //   2 = 42.42,
    //   3 = "str",
    //   4 = {
    //     foo = {
    //       bar = 42
    //     }
    //   },
    //   nextLevel = {
    //     bar = "escape character\n\tstart newline with tab",
    //     foo = "bar"
    //   }
    // }
    // parents = [
    //   {
    //     age = 42,
    //     childs = [
    //       {
    //         age = 2,
    //         gender = "M",
    //         name = "toto"
    //       },
    //       {
    //         age = 4,
    //         gender = "F",
    //         name = "titi"
    //       },
    //       {
    //         age = 0.5,
    //         gender = "F",
    //         name = "tutu"
    //       }
    //     ],
    //     gender = "M",
    //     name = "brian"
    //   },
    //   {
    //     age = 42,
    //     childs = [],
    //     gender = "M",
    //     name = "toto"
    //   },
    //   {
    //     age = 42,
    //     childs = [],
    //     emptyObj = {},
    //     gender = "F",
    //     name = "clara"
    //   }
    // ]
    return 0;
}