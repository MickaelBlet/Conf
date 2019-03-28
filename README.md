# Configator

ini reader for c++

## Example

```cpp
#include "configator.hpp"

using namespace std;

struct TestConfig
{
    string          valueString;
    bool            valueBool;
    int             valueInt;
    long            valueLong;
    unsigned long   valueULong;
    double          valueDouble;
};

int main()
{
    using namespace Config;

    Configator conf("./test.ini");
    if (conf.isRead() == false)
    {
        cerr << "Open configuration Fail" << endl;
        return 1;
    }

    TestConfig testConfig;

    if (conf.getValue("", "42", testConfig.valueString, "0") == false)
        cerr << "not find key: " << "42" << " in section: [" << "" << "]" << endl;
    if (conf.getValue("", "42", testConfig.valueBool, false) == false)
        cerr << "not find key: " << "42" << " in section: [" << "" << "]" << endl;
    if (conf.getValue("", "42", testConfig.valueInt, 0) == false)
        cerr << "not find key: " << "42" << " in section: [" << "" << "]" << endl;
    if (conf.getValue("", "42", testConfig.valueLong, 0) == false)
        cerr << "not find key: " << "42" << " in section: [" << "" << "]" << endl;
    if (conf.getValue("", "42", testConfig.valueULong, 0) == false)
        cerr << "not find key: " << "42" << " in section: [" << "" << "]" << endl;
    if (conf.getValue("", "42", testConfig.valueDouble, 0) == false)
        cerr << "not find key: " << "42" << " in section: [" << "" << "]" << endl;

    cout << "string: " << testConfig.valueString << endl;
    cout << "bool  : " << testConfig.valueBool << endl;
    cout << "int   : " << testConfig.valueInt << endl;
    cout << "long  : " << testConfig.valueLong << endl;
    cout << "ulong : " << testConfig.valueULong << endl;
    cout << "double: " << testConfig.valueDouble << endl;

    return 0;
}
```