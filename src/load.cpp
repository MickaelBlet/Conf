/**
 * load.cpp
 *
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * Copyright (c) 2024 BLET Mickaël.
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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fstream> // std::ifstream

#include "blet/conf.h"

namespace blet {

namespace conf {

class StringReader {
  public:
    StringReader(std::istream& stream) :
        stream_(stream),
        pos_(0),
        line_(1),
        posColumn_(0) {
        stream_.seekg(0, stream_.end);
        std::size_t streamLength = stream_.tellg();
        stream_.seekg(0, stream_.beg);
        std::vector<char> vBuffer(streamLength);
        stream_.read(vBuffer.data(), vBuffer.size());
        str_.append(vBuffer.begin(), vBuffer.end());
    }

    ~StringReader() {}

    template<std::size_t Size>
    bool operator==(const char (&str)[Size]) {
        return !str_.compare(pos_, Size - 1, str, Size - 1);
    }

    void operator++() {
        if (str_[pos_] == '\n') {
            posColumn_ = pos_ + 1;
            ++line_;
        }
        ++pos_;
    }

    void operator+=(std::size_t index) {
        for (std::size_t i = 0; i < index; ++i) {
            operator++();
        }
    }

    const char& operator[](std::size_t index) {
        return str_[pos_ + index];
    }

    std::string substr(std::size_t start, std::size_t end) {
        return str_.substr(start, end - start);
    }

    std::size_t line() {
        return line_;
    }

    std::size_t column() {
        return pos_ - posColumn_ + 1;
    }

    std::size_t index() {
        return pos_;
    }

    std::istream& streamOffset(std::size_t offset) {
        return stream_.seekg(offset, stream_.beg);
    }

  private:
    std::istream& stream_;
    std::string str_;
    std::size_t pos_;
    std::size_t line_;
    std::size_t posColumn_;
};

class Loader {
  public:
    Loader(const std::string& filename, std::istream& stream) :
        filename_(filename),
        reader_(stream) {}

    ~Loader() {}

    enum EValueFromType {
        DEFAULT_VALUE_FROM_TYPE,
        OBJECT_VALUE_FROM_TYPE,
        ARRAY_VALUE_FROM_TYPE
    };

    void load(blet::Dict& dict) {
        spaceJump();
        while (reader_[0] != '\0') {
            loadType(dict);
        }
    }

    void loadType(blet::Dict& dict) {
        if (reader_[0] == '[') {
            loadSection(dict);
        }
        else if (reader_[0] == '{') {
            if (currentSections_.empty()) {
                loadObject(dict);
            }
            else {
                loadObject(*(currentSections_.top()));
            }
        }
        else {
            // create the default section if needed
            if (currentSections_.empty()) {
                currentSections_.push(&dict);
            }
            loadKey(*(currentSections_.top()));
        }
    }

    void loadSection(blet::Dict& dict, bool linear = false) {
        ++reader_; // jump '['
        spaceJumpLine();
        // multi section
        if (reader_[0] == '[') {
            std::size_t level = 1;
            while (reader_[0] == '[') {
                ++reader_; // jump '['
                ++level;
                spaceJumpLine();
            }
            std::string sectionName = loadSectionName();
            std::size_t maxlevel = level - 1;
            while (reader_[0] == ']') {
                ++reader_; // jump ']'
                --level;
                spaceJumpLine();
            }
            if (level != 0) {
                throw LoadException(filename_, reader_.line(), reader_.column(), "End of section");
            }
            while (currentSections_.size() > maxlevel) {
                currentSections_.pop();
            }
            if (maxlevel == currentSections_.size()) {
                currentSections_.push(&(currentSections_.top()->operator[](sectionName)));
            }
            else {
                throw LoadException(filename_, reader_.line(), reader_.column(), "Section without parent");
            }
        }
        // basic or linear section
        else {
            if (!linear) {
                // clear currentSections_
                currentSections_ = std::stack<blet::Dict*>();
            }
            std::string sectionName = loadSectionName();
            if (sectionName.empty()) {
                // set the default section
                currentSections_.push(&dict);
            }
            else {
                // add new section
                currentSections_.push(&(dict[sectionName]));
            }
            ++reader_; // jump ']'
        }
        spaceJumpLine();
        if (reader_[0] == '[') {
            // recursive linear
            loadSection(*(currentSections_.top()), true);
        }
        spaceJump();
    }

    std::string loadSectionName() {
        std::size_t start = reader_.index();
        std::size_t end = reader_.index();
        // section name
        if (reader_[0] == '"' || reader_[0] == '\'') {
            loadQuoteIndexes(&start, &end);
            spaceJumpLine();
            if (reader_[0] != ']') {
                throw LoadException(filename_, reader_.line(), reader_.column(), "End of section");
            }
            return stringEscape(reader_.substr(start, end));
        }
        else {
            while (reader_[0] != ']') {
                if (reader_[0] == '\0' || reader_[0] == '\n') {
                    throw LoadException(filename_, reader_.line(), reader_.column(), "End of section");
                }
                if ((reader_[0] >= '\t' && reader_[0] <= '\r') || reader_[0] == ' ') {
                    spaceJumpLine();
                }
                else {
                    ++reader_;
                    end = reader_.index();
                }
            }
            return reader_.substr(start, end);
        }
    }

    void loadKey(blet::Dict& dict) {
        if (reader_[0] == '=' || reader_[0] == ':') {
            throw LoadException(filename_, reader_.line(), reader_.column(), "Key not found");
        }
        blet::Dict* pKeyDict = loadKeyDict(dict);
        spaceJump();
        if (reader_[0] != '=' && reader_[0] != ':') {
            throw LoadException(filename_, reader_.line(), reader_.column(), "Assign operator not found");
        }
        ++reader_; // jump '=' or ':'
        spaceJumpLine();
        loadValue(*pKeyDict);
        spaceJump();
    }

    blet::Dict* loadKeyDict(blet::Dict& dict) {
        blet::Dict* pCurrentDict = &(dict[loadKeyName()]);
        // map key
        while (reader_[0] == '[') {
            std::string keyMapName = loadKeyMapName();
            if (keyMapName.empty()) {
                pCurrentDict->push_back(blet::Dict());
                pCurrentDict = &(pCurrentDict->back());
            }
            else {
                pCurrentDict = &(pCurrentDict->operator[](keyMapName));
            }
        }
        return pCurrentDict;
    }

    std::string loadKeyName() {
        std::size_t start = reader_.index();
        std::size_t end = reader_.index();
        if (reader_[0] == '"' || reader_[0] == '\'') {
            loadQuoteIndexes(&start, &end);
            spaceJumpLine();
            if (reader_[0] != '=' && reader_[0] != ':' && reader_[0] != '[') {
                throw LoadException(filename_, reader_.line(), reader_.column(), "Assign operator not found");
            }
        }
        else {
            while (reader_[0] != '=' && reader_[0] != ':' && reader_[0] != '[') {
                if (reader_[0] == '\0' || reader_[0] == '\n') {
                    throw LoadException(filename_, reader_.line(), reader_.column(), "Assign operator not found");
                }
                if ((reader_[0] >= '\t' && reader_[0] <= '\r') || reader_[0] == ' ') {
                    spaceJumpLine();
                }
                else {
                    ++reader_;
                    end = reader_.index();
                }
            }
        }
        return stringEscape(reader_.substr(start, end));
    }

    std::string loadKeyMapName() {
        ++reader_; // jump '['
        spaceJumpLine();
        std::size_t start = reader_.index();
        std::size_t end = reader_.index();
        if (reader_[0] == '"' || reader_[0] == '\'') {
            loadQuoteIndexes(&start, &end);
            spaceJumpLine();
            if (reader_[0] != ']') {
                throw LoadException(filename_, reader_.line(), reader_.column(), "End of map");
            }
        }
        else {
            while (reader_[0] != ']') {
                if (reader_[0] == '\0' || reader_[0] == '\n') {
                    throw LoadException(filename_, reader_.line(), reader_.column(), "End of map");
                }
                if ((reader_[0] >= '\t' && reader_[0] <= '\r') || reader_[0] == ' ') {
                    spaceJumpLine();
                }
                else {
                    ++reader_;
                    end = reader_.index();
                }
            }
        }
        ++reader_; // jump ']'
        spaceJumpLine();
        return stringEscape(reader_.substr(start, end));
    }

    void loadValue(blet::Dict& dict, EValueFromType fromType = DEFAULT_VALUE_FROM_TYPE) {
        std::size_t start = reader_.index();
        std::size_t end = reader_.index();
        switch (reader_[0]) {
            case '"':
            case '\'': {
                loadQuoteIndexes(&start, &end);
                spaceJumpLine();
                commentJump();
                if (reader_[0] != '\n' && reader_[0] != '\0') {
                    if (fromType == DEFAULT_VALUE_FROM_TYPE ||
                        (fromType == OBJECT_VALUE_FROM_TYPE && reader_[0] != '}' && reader_[0] != ',') ||
                        (fromType == ARRAY_VALUE_FROM_TYPE && reader_[0] != ']' && reader_[0] != ',')) {
                        throw LoadException(filename_, reader_.line(), reader_.column(), "End of value");
                    }
                }
                dict.clear();
                dict = stringEscape(reader_.substr(start, end));
                break;
            }
            case '{': {
                loadObject(dict);
                break;
            }
            case '[': {
                loadArray(dict);
                break;
            }
            default: {
                while (reader_[0] != '\n' && reader_[0] != ';' && reader_[0] != '#' && reader_[0] != '\0') {
                    if (fromType == OBJECT_VALUE_FROM_TYPE && (reader_[0] == '}' || reader_[0] == ',')) {
                        break;
                    }
                    if (fromType == ARRAY_VALUE_FROM_TYPE && (reader_[0] == ']' || reader_[0] == ',')) {
                        break;
                    }
                    if ((reader_[0] >= '\t' && reader_[0] <= '\r') || reader_[0] == ' ') {
                        spaceJumpLine();
                    }
                    else {
                        ++reader_;
                        end = reader_.index();
                    }
                }
                dict.clear();
                getValue(dict, reader_.substr(start, end));
                break;
            }
        }
    }

    void loadObject(blet::Dict& dict) {
        std::size_t start = reader_.index();
        std::size_t end = reader_.index();
        bool next = false;

        if (!dict.isObject()) {
            dict.newObject();
        }
        ++reader_; // jump '{'
        spaceJump();
        while (reader_[0] != '}' || next) {
            if (reader_[0] == '\0') {
                throw LoadException(filename_, reader_.line(), reader_.column(), "End of object");
            }
            if (reader_[0] == '=' || reader_[0] == ':') {
                throw LoadException(filename_, reader_.line(), reader_.column(), "Key not found");
            }
            // start key name
            if (reader_[0] == '\"' || reader_[0] == '\'') {
                loadQuoteIndexes(&start, &end);
            }
            else {
                start = reader_.index();
                while (reader_[0] != '=' && reader_[0] != ':') {
                    if (reader_[0] == '\0' || reader_[0] == '\n') {
                        throw LoadException(filename_, reader_.line(), reader_.column(), "End of key");
                    }
                    if ((reader_[0] >= '\t' && reader_[0] <= '\r') || reader_[0] == ' ') {
                        spaceJumpLine();
                    }
                    else {
                        ++reader_;
                        end = reader_.index();
                    }
                }
            }
            blet::Dict* objDict = &(dict.operator[](stringEscape(reader_.substr(start, end))));
            spaceJump();
            if (reader_[0] != '=' && reader_[0] != ':') {
                throw LoadException(filename_, reader_.line(), reader_.column(), "Assign operator not found");
            }
            ++reader_; // jump '=' or ':'
            spaceJump();
            // recursive
            loadValue(*objDict, OBJECT_VALUE_FROM_TYPE);
            spaceJump();
            if (reader_[0] == ',') {
                ++reader_; // jump ','
                spaceJump();
                next = true;
            }
            else {
                next = false;
            }
        }
        ++reader_; // jump '}'
        spaceJump();
    }

    void loadArray(blet::Dict& dict) {
        bool next = false;

        dict.newArray();
        ++reader_; // jump '[]'
        spaceJump();
        while (reader_[0] != ']' || next) {
            if (reader_[0] == '\0') {
                throw LoadException(filename_, reader_.line(), reader_.column(), "End of array");
            }
            dict.push_back(blet::Dict());
            blet::Dict& arrDict = dict.back();
            // recursive
            loadValue(arrDict, ARRAY_VALUE_FROM_TYPE);
            spaceJump();
            if (reader_[0] == ',') {
                ++reader_; // jump ','
                spaceJump();
                next = true;
            }
            else {
                next = false;
            }
        }
        ++reader_; // jump '}'
        spaceJump();
    }

    void getValue(blet::Dict& dict, const std::string& value) {
        std::string lowerStr(value);
        for (std::size_t i = 0; i < lowerStr.size(); ++i) {
            if (lowerStr[i] >= 'A' && lowerStr[i] <= 'Z') {
                lowerStr[i] = lowerStr[i] + ' ';
            }
        }
        switch (lowerStr[0]) {
            case '-':
            case '+':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                s_parseNumber(value, dict);
                break;
            case 'f':
                if (lowerStr == "false") {
                    dict = false;
                }
                else {
                    dict = value;
                }
                break;
            case 'n':
                if (lowerStr == "no") {
                    dict = false;
                }
                else if (lowerStr == "none" || lowerStr == "null") {
                    dict.newNull();
                }
                else {
                    dict = value;
                }
                break;
            case 'o':
                if (lowerStr == "on") {
                    dict = true;
                }
                else if (lowerStr == "off") {
                    dict = false;
                }
                else {
                    dict = value;
                }
                break;
            case 't':
                if (lowerStr == "true") {
                    dict = true;
                }
                else {
                    dict = value;
                }
                break;
            case 'y':
                if (lowerStr == "yes") {
                    dict = true;
                }
                else {
                    dict = value;
                }
                break;
            case '\0':
                dict.newNull();
                break;
            default:
                dict = value;
                break;
        }
    }

    void loadQuoteIndexes(std::size_t* pStart, std::size_t* pEnd) {
        // get quote character
        const char quote = reader_[0];
        ++reader_; // jump quote
        *pStart = reader_.index();
        // search end quote
        while (reader_[0] != quote) {
            if (reader_[0] == '\\' && (reader_[1] == quote || reader_[1] == '\\')) {
                ++reader_;
            }
            if (reader_[0] == '\0' || reader_[0] == '\n') {
                throw LoadException(filename_, reader_.line(), reader_.column(), "End of quote");
            }
            ++reader_;
        }
        *pEnd = reader_.index();
        ++reader_; // jump quote
    }

    void spaceJumpLine() {
        // isspace
        while (reader_[0] != '\n' && ((reader_[0] >= '\t' && reader_[0] <= '\r') || reader_[0] == ' ')) {
            ++reader_;
        }
    }

    void spaceJump() {
        commentJump();
        // isspace
        while ((reader_[0] >= '\t' && reader_[0] <= '\r') || reader_[0] == ' ') {
            ++reader_;
            commentJump();
        }
    }

    void commentJump() {
        if (reader_[0] == '#' || reader_[0] == ';') {
            while (reader_[0] != '\0' && reader_[0] != '\n') {
                ++reader_; // jump character
            }
        }
    }

    static std::string stringEscape(const std::string& str) {
        std::ostringstream oss("");
        for (std::size_t i = 0; i < str.size(); ++i) {
            if (str[i] == '\\') {
                switch (str[i + 1]) {
                    case 'a':
                        oss << '\a';
                        break;
                    case 'b':
                        oss << '\b';
                        break;
                    case 'f':
                        oss << '\f';
                        break;
                    case 'n':
                        oss << '\n';
                        break;
                    case 'r':
                        oss << '\r';
                        break;
                    case 't':
                        oss << '\t';
                        break;
                    case 'v':
                        oss << '\v';
                        break;
                    case '\'':
                        oss << '\'';
                        break;
                    case '"':
                        oss << '\"';
                        break;
                    case '\\':
                        oss << '\\';
                        break;
                    default:
                        oss << '\\';
                        if (str[i + 1] != '\0') {
                            oss << str[i + 1];
                        }
                        break;
                }
                ++i;
            }
            else {
                oss << str[i];
            }
        }
        return oss.str();
    }

  private:
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
                else {
                    return false;
                }
            }
            stringStream << ::strtol(value.c_str(), NULL, 16);
            return true;
        }
        else {
            return false;
        }
    }

    static bool s_binary(const std::string& value, std::ostream& stringStream) {
        std::size_t index = 0;
        bool sub = false;
        if (value[index] == '-' || value[index] == '+') {
            if (value[index] == '-') {
                sub = true;
            }
            ++index;
        }
        // is binary
        if (value[index] == '0' && value[index + 1] == 'b') {
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
            if (sub) {
                long lvalue;
                std::stringstream ss("");
                ss << '-' << ::strtoul(value.c_str() + start, NULL, 2);
                ss >> lvalue;
                stringStream << lvalue;
            }
            else {
                stringStream << ::strtoul(value.c_str() + start, NULL, 2);
            }
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
        if (value[index] == '0' && value.find('.') == std::string::npos && value.find('e') == std::string::npos) {
            while (value[index] != '\0') {
                if (value[index] >= '0' && value[index] <= '8') {
                    ++index;
                }
                else {
                    return false;
                }
            }
            stringStream << ::strtol(value.c_str(), NULL, 8);
            return true;
        }
        else {
            return false;
        }
    }

    static bool s_double(const std::string& value, std::ostream& stringStream) {
        char* endPtr = NULL;
        stringStream << ::strtod(value.c_str(), &endPtr);
        if (endPtr != NULL && endPtr[0] != '\0') {
            return false;
        }
        return true;
    }

    static void s_parseNumber(const std::string& str, blet::Dict& dict) {
        std::stringstream stringStream("");
        if (s_hex(str, stringStream) || s_binary(str, stringStream) || s_octal(str, stringStream) ||
            s_double(str, stringStream)) {
            double num;
            stringStream >> num;
            dict.newNumber(num);
        }
        else {
            dict = str;
        }
    }

    const std::string filename_;
    StringReader reader_;
    std::stack<blet::Dict*> currentSections_;
};

blet::Dict loadFile(const char* filename) {
    std::ifstream fileStream(filename); // open file
    if (!fileStream.is_open()) {
        throw LoadException(filename, "Open file failed");
    }
    blet::Dict dict;
    Loader loader(filename, fileStream);
    loader.load(dict);
    return dict;
}

blet::Dict loadStream(std::istream& stream) {
    blet::Dict dict;
    Loader loader("", stream);
    loader.load(dict);
    return dict;
}

blet::Dict loadString(const std::string& str) {
    std::istringstream iss(str);
    return loadStream(iss);
}

blet::Dict loadData(const void* data, std::size_t size) {
    return loadString(std::string(static_cast<const char*>(data), size));
}

} // namespace conf

} // namespace blet
