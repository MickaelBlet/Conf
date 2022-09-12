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

#include "mblet/configator.hpp"
#include <cmath>

namespace mblet {

Configator::Map::Map():
    std::map<std::string, Map>(),
    _comment(std::string()),
    _string(std::string()),
    _number(NAN),
    _boolean(false),
    _isExist(false),
    _isNumber(false),
    _isBoolean(false)
{}

static bool s_hex(const std::string& value, std::ostream& stringStream) {
    std::size_t index = 0;

    if (value[index] == '-' || value[index] == '+') {
        ++index;
    }
    // is hex
    if (value[index] == '0' && (value[index + 1] == 'x' || value[index + 1] == 'X')) {
        ++index;
        ++index;
        while (value[index] != '\0') {
            if (value[index] >= '0' && value[index] <= '9') {
                ++index;
            }
            else if (value[index] >= 'a' && value[index] <= 'f') {
                ++index;
            }
            else if (value[index] >= 'A' && value[index] <= 'F') {
                ++index;
            }
            else {
                return false;
            }
        }
        stringStream << strtoll(value.c_str(), NULL, 16);
        return true;
    }
    else {
        return false;
    }
}

static bool s_binary(const std::string& value, std::ostream& stringStream) {
    std::size_t index = 0;

    if (value[index] == '-' || value[index] == '+') {
        ++index;
    }
    // is binary
    if (value[index] == '0' && (value[index + 1] == 'b' || value[index + 1] == 'B')) {
        ++index;
        ++index;
        std::size_t start = index;
        while (value[index] != '\0') {
            if (value[index] == '0' || value[index] == '1') {
                ++index;
            }
            else {
                return false;
            }
        }
        stringStream << strtoull(value.c_str() + start, NULL, 2);
        return true;
    }
    else {
        return false;
    }
}

static bool s_octal(const std::string& value, std::ostream& stringStream) {
    std::size_t index = 0;

    if (value[index] == '-' || value[index] == '+') {
        ++index;
    }
    // is binary
    if (value[index] == '0' && value.find('.') == std::string::npos && value.find('e') == std::string::npos
             && value.find('E') == std::string::npos) {
        while (value[index] != '\0') {
            if (value[index] >= '0' && value[index] <= '8') {
                ++index;
            }
            else {
                return false;
            }
        }
        stringStream << strtoll(value.c_str(), NULL, 8);
        return true;
    }
    else {
        return false;
    }
}

static bool s_double(const std::string& value, std::ostream& stringStream) {
    char *endPtr = NULL;
    stringStream << strtod(value.c_str(), &endPtr);
    if (endPtr != NULL && endPtr[0] != '\0') {
        return false;
    }
    return true;
}

static bool s_bool(const std::string& value, bool& retBoolean) {
    static const std::pair<std::string, bool> pairStrToBool[] = {
        std::pair<std::string, bool>("TRUE", true),
        std::pair<std::string, bool>("True", true),
        std::pair<std::string, bool>("true", true),
        std::pair<std::string, bool>("ON", true),
        std::pair<std::string, bool>("On", true),
        std::pair<std::string, bool>("on", true),
        std::pair<std::string, bool>("YES", true),
        std::pair<std::string, bool>("Yes", true),
        std::pair<std::string, bool>("yes", true),
        std::pair<std::string, bool>("Y", true),
        std::pair<std::string, bool>("y", true),
        std::pair<std::string, bool>("1", true),
        std::pair<std::string, bool>("FALSE", false),
        std::pair<std::string, bool>("False", false),
        std::pair<std::string, bool>("false", false),
        std::pair<std::string, bool>("OFF", false),
        std::pair<std::string, bool>("Off", false),
        std::pair<std::string, bool>("off", false),
        std::pair<std::string, bool>("NO", false),
        std::pair<std::string, bool>("No", false),
        std::pair<std::string, bool>("no", false),
        std::pair<std::string, bool>("N", false),
        std::pair<std::string, bool>("n", false),
        std::pair<std::string, bool>("0", false)
    };
    static const std::map<std::string, bool> strToBool(pairStrToBool,
                                                       pairStrToBool + sizeof(pairStrToBool) / sizeof(*pairStrToBool));

    std::map<std::string, bool>::const_iterator cit = strToBool.find(value);
    if (cit != strToBool.end()) {
        retBoolean = cit->second;
        return true;
    }
    else {
        return false;
    }
}

void Configator::Map::stringToType() {
    std::stringstream stringStream("");
    bool retBoolean;
    if (s_bool(_string, retBoolean)) {
        _isBoolean = true;
        _isNumber = true;
        _boolean = retBoolean;
        if (retBoolean) {
            _number = 1;
        }
        else {
            _number = 0;
        }
    }
    else if (s_hex(_string, stringStream) || s_binary(_string, stringStream) ||
        s_octal(_string, stringStream) || s_double(_string, stringStream)) {
        _isNumber = true;
        stringStream >> _number;
    }
}

}