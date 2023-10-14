# Examples

## Access by map

```conf
$ cat ./test1.conf
[test]
42=-42.42
```
```cpp
blet::Dict conf = blet::conf::loadFile("./test1.conf");
double d = conf["test"]["42"];
std::cout << d << std::endl;
// output:
// -42.42
```

## Convert value

```conf
$ cat ./test2.conf
[test]
0=true   ;bool
1=-42.42 ;dec
2=0x42   ;hex
3=0b0101 ;binary
4=042    ;octal
```
```cpp
blet::Dict conf = blet::conf::loadFile("./test2.conf");
std::cout << conf["test"]["0"].get<int>() << std::endl;
std::cout << conf["test"]["1"].get<int>() << std::endl;
std::cout << conf["test"]["2"].get<int>() << std::endl;
std::cout << conf["test"]["3"].get<int>() << std::endl;
std::cout << conf["test"]["4"].get<int>() << std::endl;
// check type and path of key
if (!conf.contains(blet::Dict::Path()["test"]["5"], blet::Dict::NUMBER_TYPE)) {
    conf["test"]["5"] = 42;
}
std::cout << conf["test"]["5"].get<int>() << std::endl;
// output:
// 1
// -42
// 66
// 5
// 34
// 42
```

## Array/Map value

```conf
$ cat ./test3.conf
[    test     ]    ; commment line
array[]=1
array[]=2
map[key]=value
map[key1][key2]=value2
```
```cpp
blet::Dict conf = blet::conf::loadFile("./test3.conf");
std::cout << conf["test"]["array"][0].get<int>() << std::endl;
std::cout << conf["test"]["array"][1].get<int>() << std::endl;
std::cout << conf["test"]["map"]["key"] << std::endl;
std::cout << conf["test"]["map"]["key1"]["key2"] << std::endl;
// output:
// 1
// 2
// value
// value2
```

## Multi sections

```conf
$ cat ./test4.conf
[ grandfather ]  ; commment line
[ [ father ] ]
[ [ [ child ] ] ]
0=false  ;bool
1=-42.42 ;dec
2=0x42   ;hex
3=0b0101 ;binary
4=042    ;octal
[grandmother][mother][child]
5=42
```
```cpp
blet::Dict conf = blet::conf::loadFile("./test4.conf");
std::cout << conf["grandfather"]["father"]["child"]["0"].get<int>() << std::endl;
std::cout << conf["grandfather"]["father"]["child"]["1"].get<int>() << std::endl;
std::cout << conf["grandfather"]["father"]["child"]["2"].get<int>() << std::endl;
std::cout << conf["grandfather"]["father"]["child"]["3"].get<int>() << std::endl;
std::cout << conf["grandfather"]["father"]["child"]["4"].get<int>() << std::endl;
std::cout << conf["grandmother"]["mother"]["child"]["5"].get<int>() << std::endl;
// output:
// 0
// -42
// 66
// 5
// 34
// 42
```

## Quotes

```conf
$ cat ./test5.conf
[test]
"32[\\]"      =     '"2 ;"#\'## i\\' # test with comment in line
42     ["key[]"]      =     "value" ; comment line
'42 space  ' ["key[]"]      =     " value space " ; comment line
```
```cpp
blet::Dict conf = blet::conf::loadFile("./test5.conf");
std::cout << conf["test"]["32[\\]"] << std::endl;
std::cout << conf["test"]["42"]["key[]"] << std::endl;
std::cout << conf["test"]["42 space  "]["key[]"] << std::endl;
// output:
// "2 ;"#'## i\
// value
//  value space
```

## loadFile

```conf
$ cat ./example/example.conf
[world]
hello = 42
```

```cpp
#include <iostream>

#include "blet/conf.h"

int main(int /*argc*/, char* /*argv*/[]) {
    const blet::Dict conf = blet::conf::loadFile("./example/example.conf");
    std::cout << conf["world"]["hello"] << std::endl; // 42
    return 0;
}
```

```
$ ./build/example/loadFile.blet_conf.example
42
```

## loadStream

```cpp
#include <iostream>

#include "blet/conf.h"

int main(int /*argc*/, char* /*argv*/[]) {
    std::istringstream iss(
        "[world]\n"
        "hello = 42"
    );
    const blet::Dict conf = blet::conf::loadStream(iss);
    int value = conf["world"]["hello"];
    std::cout << value << std::endl; // 42
    return 0;
}
```

```
$ ./build/example/loadStream.blet_conf.example
42
```

## loadString

```cpp
#include <iostream>

#include "blet/conf.h"

int main(int /*argc*/, char* /*argv*/[]) {
    const std::string str(
        "[world]\n"
        "hello = 42"
    );
    const blet::Dict conf = blet::conf::loadString(str);
    int value = conf["world"]["hello"];
    std::cout << value << std::endl; // 42
    return 0;
}
```

```
$ ./build/example/loadString.blet_conf.example
42
```

## loadData

```cpp
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
```

```
$ ./build/example/loadData.blet_conf.example
42
```

## dump

```cpp
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
```

```
$ ./build/example/dump.blet_conf.example
# Conf style
array[] = 4
array[] = 2
boolean = true
foo = "bar"
null = null
number = 42

# Json style
array = [
  4,
  2
]
boolean = true
foo = "bar"
null = null
number = 42

----------------------------------------
# Conf style
[nameOfSection]
array[] = 4
array[] = 2
boolean = true
foo = "bar"
null = null
number = 42

# Json style
nameOfSection = {
  array = [
    4,
    2
  ],
  boolean = true,
  foo = "bar",
  null = null,
  number = 42
}

----------------------------------------
# Conf style
[nameOfSection1]
boolean = true
foo = "bar"
number = 42
[nameOfSection2]
array[] = 4
array[] = 2
null = null

# Json style
nameOfSection1 = {
  boolean = true,
  foo = "bar",
  number = 42
}
nameOfSection2 = {
  array = [
    4,
    2
  ],
  null = null
}

----------------------------------------
# Conf style
[nameOfSection1]
[[sectionChild1]]
foo = "bar"
[[sectionChild2]]
number = 42
[[sectionChild3]]
boolean = true
[nameOfSection2]
[[sectionChild1]]
null = null
[[sectionChild2]]
array[] = 4
array[] = 2

# Json style
nameOfSection1 = {
  sectionChild1 = {
    foo = "bar"
  },
  sectionChild2 = {
    number = 42
  },
  sectionChild3 = {
    boolean = true
  }
}
nameOfSection2 = {
  sectionChild1 = {
    null = null
  },
  sectionChild2 = {
    array = [
      4,
      2
    ]
  }
}
```