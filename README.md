# Conf

Config load and dump library  
Header only library at [single_include/blet/conf.h](single_include/blet/conf.h).  
Examples at [docs/examples.md](docs/examples.md)

## Quickstart

```ini
# ------------------------------------
# Content of ./example/quickstart.conf
# ------------------------------------

; basic ini format (with support spaces)
[foo]
0 = Null  ; icase null
1 = true  ; icase boolean (format: true,false | on,off | yes,no)
2 = 42.42 ; number double (format: binary{0b10101}, octal{042}, hexadecimal{0xFF})
3 = "str" ; string
; multi ini section
[[ nextLevel ]]
foo = bar
; array ini value
[bar]
foo[] = 0b0001 ; 1
foo[] = 0b0010 ; 2
foo[] = 0b0011 ; 3
; map ini value with re-use key
[foo]
4[foo][bar] = 052 ; 42
; inline multi section with re-use key
[foo][nextLevel]
"bar" = "escape character\n\tstart newline with tab"
; use default section
[]
; pseudo json
parents[] = {
    name = brian
    age = 42
    gender = M
    childs = [
        {
            name = toto
            age = 2
            gender = M
        }
        {
            name = titi
            age = 4
            gender = F
        }
        {
            name = tutu
            age = 0.5
            gender = F
        }
    ]
}
parents[] = {
    name = toto
    age = 0x2A
    gender = M
    childs = []
}
; pseudo json one line
parents[] = { name = clara, age = 42, gender = F, childs = [], emptyObj = {} }
```
```cpp
const blet::Dict conf = blet::conf::loadFile("./example/quickstart.conf");

std::cout << conf["foo"]["0"] << std::endl; // null
std::cout << conf["foo"]["1"] << std::endl; // true
std::cout << conf["foo"]["2"] << std::endl; // 42.42
std::cout << conf["foo"]["3"] << std::endl; // str
std::cout << conf["foo"]["nextLevel"]["foo"] << std::endl; // bar
std::cout << conf["foo"]["nextLevel"]["bar"] << std::endl; // escape character
std::cout << conf["bar"]["foo"][0] << std::endl; // 1
std::cout << conf["bar"]["foo"][1] << std::endl; // 2
std::cout << conf["bar"]["foo"][2] << std::endl; // 3

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
```

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

## Load Functions

### loadFile

```cpp
blet::Dict loadFile(const char* filename);
```
Take a path of configuration file and load them for create a Dict object.  
Example at [docs/examples.md#loadFile](docs/examples.md#loadfile).

### loadStream

```cpp
blet::Dict loadStream(std::istream& stream);
```

Take a std::istream and load them for create a Dict object.  
Example at [docs/examples.md#loadStream](docs/examples.md#loadstream).

### loadString

```cpp
blet::Dict loadString(const std::string& str);
```

Take a std::string and load them for create a Dict object.  
Example at [docs/examples.md#loadString](docs/examples.md#loadstring).

### loadData

```cpp
blet::Dict loadData(const void* data, std::size_t size);
```

Take a data and size and load them for create a Dict object.  
Example at [docs/examples.md#loadData](docs/examples.md#loaddata).

## Dump Functions

### Dump

``` cpp
void dump(const blet::Dict& dict, std::ostream& os, std::size_t indent = 0, char indentCharacter = ' ', enum blet::conf::EDumpStyle style = blet::conf::CONF_STYLE);
```

Dump from a Dict to configuration format in stream.  
You can set the `indent` and `indentCharacter` and `style` with `CONF_STYLE` or `JSON_STYLE` for better formatting.  
Example at [docs/examples.md#DumpStream](docs/examples.md#dumpstream).

``` cpp
std::string dump(const blet::Dict& dict, std::size_t indent = 0, char indentCharacter = ' ', enum blet::conf::EDumpStyle style = blet::conf::CONF_STYLE);
```

Dump from a Dict to configuration format in string.  
You can set the `indent` and `indentCharacter` and `style` with `CONF_STYLE` or `JSON_STYLE` for better formatting.  
Example at [docs/examples.md#DumpStream](docs/examples.md#dumpstring).