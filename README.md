# Configator

ini reader for c++

### Access by map
```ini
$ cat ./test1.ini
[test]
42=-42.42
```
```cpp
mblet::Configator conf("./test1.ini");
std::cout << conf["test"]["42"] << std::endl;
// output:
// -42.42
```

### Convert value
```ini
$ cat ./test2.ini
[test]
0=true   ;bool
1=-42.42 ;dec
2=0x42   ;hex
3=0b0101 ;binary
4=042    ;octal
```
```cpp
mblet::Configator conf("./test2.ini");
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

### Table value
```ini
$ cat ./test3.ini
[    test     ]    ; commment line
table[]=1
table[]=2
map[key]=value
map[key1][key2]=value2
```
```cpp
mblet::Configator conf("./test3.ini");
std::cout << conf["test"]["table"][0].get<int>() << std::endl;
std::cout << conf["test"]["table"][1].get<int>() << std::endl;
std::cout << conf["test"]["map"]["key"] << std::endl;
std::cout << conf["test"]["map"]["key1"]["key2"] << std::endl;
// output:
// 1
// 2
// value
// value2
```

### Quotes
```ini
$ cat ./test4.ini
[test]
"32[\\]"      =     '"2 ;"#\'## i\\' # test with comment in line
42     ["key[]"]      =     "value" ; comment line
'42 space  ' ["key[]"]      =     " value space " ; comment line
```
```cpp
mblet::Configator conf("./test4.ini");
std::cout << conf["test"]["32[\\]"] << std::endl;
std::cout << conf["test"]["42"]["key[]"] << std::endl;
std::cout << conf["test"]["42 space  "]["key[]"] << std::endl;
// output:
// "2 ;"#'## i\
// value
//  value space
```