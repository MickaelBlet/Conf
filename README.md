# Configator

Config reader for c++

## Build
```bash
# static build
mkdir build ; pushd build && cmake .. && make -j ; popd
# shared build
mkdir build ; pushd build && cmake -DBUILD_SHARED_LIBS=1 .. && make -j ; popd
# static build and launch test
mkdir build ; pushd build && cmake -DBUILD_TESTING=1 .. && make -j && make test ; popd
# shared build and launch test
mkdir build ; pushd build && cmake -DBUILD_SHARED_LIBS=1 -DBUILD_TESTING=1 .. && make -j && make test ; popd
```

## Examples
### Access by map
```conf
$ cat ./test1.conf
[test]
42=-42.42
```
```cpp
mblet::Configator conf("./test1.conf");
double d = conf["test"]["42"];
std::cout << d << std::endl;
// output:
// -42.42
```

### Convert value
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
mblet::Configator conf("./test2.conf");
std::cout << conf["test"]["0"].get<int>() << std::endl;
std::cout << conf["test"]["1"].get<int>() << std::endl;
std::cout << conf["test"]["2"].get<int>() << std::endl;
std::cout << conf["test"]["3"].get<int>() << std::endl;
std::cout << conf["test"]["4"].get<int>() << std::endl;
std::cout << conf["test"]["5"].get<int>(42) << std::endl; // take default key
// output:
// 1
// -42
// 66
// 5
// 34
// 42
```

### Array/Map value
```conf
$ cat ./test3.conf
[    test     ]    ; commment line
array[]=1
array[]=2
map[key]=value
map[key1][key2]=value2
```
```cpp
mblet::Configator conf("./test3.conf");
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

### Multi sections
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
mblet::Configator conf("./test4.conf");
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

### Quotes
```conf
$ cat ./test5.conf
[test]
"32[\\]"      =     '"2 ;"#\'## i\\' # test with comment in line
42     ["key[]"]      =     "value" ; comment line
'42 space  ' ["key[]"]      =     " value space " ; comment line
```
```cpp
mblet::Configator conf("./test5.conf");
std::cout << conf["test"]["32[\\]"] << std::endl;
std::cout << conf["test"]["42"]["key[]"] << std::endl;
std::cout << conf["test"]["42 space  "]["key[]"] << std::endl;
// output:
// "2 ;"#'## i\
// value
//  value space
```