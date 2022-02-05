/**
 * configator_map.cpp
 *
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * Copyright (c) 2022 BLET Mickaël.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "configator.hpp"

namespace mblet {

Configator::Map Configator::Map::_emptyMap = Configator::Map();

Configator::Map::Map():
    std::map<std::string, Map>(),
    comment(std::string("")),
    value(std::string("")) {
    return ;
}

void Configator::Map::valueToStream(std::ostream& stringStream) const {
    std::size_t index = 0;

    if (value[index] == '-' || value[index] == '+') {
        ++index;
    }
    // is hex
    if (value[index] == '0' && value[index + 1] == 'x') {
        stringStream << strtoll(value.c_str(), NULL, 16);
    }
    // is binary
    else if (value[index] == '0' && value[index + 1] == 'b') {
        stringStream << strtoull(value.c_str() + index + 2, NULL, 2);
    }
    // is octal
    else if (value[index] == '0' && value.find('.') == std::string::npos) {
        stringStream << strtoll(value.c_str(), NULL, 8);
    }
    // is bool
    else if (value == "TRUE" || value == "True" || value == "true" ||
             value == "ON"   || value == "On"   || value == "on"   ||
             value == "YES"  || value == "Yes"  || value == "yes") {
        stringStream << true;
    }
    else if (value == "FALSE" || value == "False" || value == "false" ||
             value == "OFF"   || value == "Off"   || value == "off"   ||
             value == "NO"    || value == "No"    || value == "no") {
        stringStream << false;
    }
    else {
        stringStream << value;
    }
}

}