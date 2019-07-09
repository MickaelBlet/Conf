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
3=042    ;octal
```
```cpp
mblet::Configator conf("./test2.ini");
std::cout << conf["test"]["0"].get<int>() << std::endl;
std::cout << conf["test"]["1"].get<int>() << std::endl;
std::cout << conf["test"]["2"].get<int>() << std::endl;
std::cout << conf["test"]["3"].get<int>() << std::endl;
// output:
// 1
// -42
// 66
// 34
```