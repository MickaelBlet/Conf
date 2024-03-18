/**
 * conf.cpp
 *
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * Copyright (c) 2022-2023 BLET Mickaël.
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

#include "blet/conf.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fstream> // std::ifstream
#include <iomanip> // std::setprecision
#include <limits>  // std::numeric_limits

namespace blet {

namespace conf {

const char replaceBackslash = -42;

LoadException::LoadException(const std::string& filename, const std::string& message) :
    std::exception(),
    filename_(filename),
    message_(message),
    line_(0),
    column_(0) {
    std::ostringstream oss("");
    oss << "Parse ";
    if (!filename_.empty()) {
        oss << filename_ << ": ";
    }
    oss << '(' << message_ << ")";
    what_ = oss.str();
}

LoadException::LoadException(const std::string& filename, std::size_t line, std::size_t column,
                             const std::string& message) :
    std::exception(),
    filename_(filename),
    message_(message),
    line_(line),
    column_(column) {
    std::ostringstream oss("");
    oss << "Parse at ";
    if (!filename_.empty()) {
        oss << filename_ << ':';
    }
    oss << line_ << ':' << column_ << " (" << message_ << ")";
    what_ = oss.str();
}

LoadException::~LoadException() throw() {}

const char* LoadException::what() const throw() {
    return what_.c_str();
}

const std::string& LoadException::filename() const throw() {
    return filename_;
}

const std::string& LoadException::message() const throw() {
    return message_;
}

const std::size_t& LoadException::line() const throw() {
    return line_;
}

const std::size_t& LoadException::column() const throw() {
    return column_;
}

/**
 * @brief structure of info dumper
 */
struct ConfDumpInfo {
    ConfDumpInfo(std::ostream& os_, std::size_t& indent_, char& indentCharacter_, enum EDumpStyle& style_) :
        os(os_),
        indent(indent_),
        indentCharacter(indentCharacter_),
        style(style_),
        indexSection(0),
        index(0) {}
    ~ConfDumpInfo() {}

    std::ostream& os;
    std::size_t& indent;
    char& indentCharacter;
    enum EDumpStyle& style;
    std::size_t indexSection;
    std::size_t index;
};

// -----------------------------------------------------------------------------
// .................................
// .#####...##..##..##...##..#####..
// .##..##..##..##..###.###..##..##.
// .##..##..##..##..##.#.##..#####..
// .##..##..##..##..##...##..##.....
// .#####....####...##...##..##.....
// .................................
// -----------------------------------------------------------------------------

static inline bool s_forceKeyString(const std::string& str) {
    bool ret = false;
    if (str.empty()) {
        ret = true;
    }
    else {
        for (std::size_t i = 0; i < str.size(); ++i) {
            if (str[i] == ' ' || str[i] == '=' || str[i] == ',' || str[i] == ';' || str[i] == '#' || str[i] == '[' ||
                str[i] == ']' || str[i] == '"' || str[i] == '\'' || str[i] == '\a' || str[i] == '\b' ||
                str[i] == '\f' || str[i] == '\r' || str[i] == '\t' || str[i] == '\v' || str[i] == '\\') {
                ret = true;
                break;
            }
        }
    }
    return ret;
}

static inline void s_stringEscape(std::ostream& oss, const std::string& str) {
    for (std::size_t i = 0; i < str.size(); ++i) {
        switch (str[i]) {
            case '\a':
                oss << '\\' << 'a';
                break;
            case '\b':
                oss << '\\' << 'b';
                break;
            case '\f':
                oss << '\\' << 'f';
                break;
            case '\n':
                oss << '\\' << 'n';
                break;
            case '\r':
                oss << '\\' << 'r';
                break;
            case '\t':
                oss << '\\' << 't';
                break;
            case '\v':
                oss << '\\' << 'v';
                break;
            case '\'':
                oss << '\\' << '\'';
                break;
            case '"':
                oss << '\\' << '"';
                break;
            case '\\':
                oss << '\\' << '\\';
                break;
            default:
                oss << str[i];
                break;
        }
    }
}

static inline void s_newlineDump(ConfDumpInfo& info, const blet::Dict& dict) {
    if (info.indent != 0) {
        if (dict.getType() == blet::Dict::OBJECT_TYPE && !dict.getValue().getObject().empty()) {
            info.os << '\n';
        }
        if (dict.getType() == blet::Dict::ARRAY_TYPE && !dict.getValue().getArray().empty()) {
            info.os << '\n';
        }
    }
}

static inline void s_indentDump(ConfDumpInfo& info) {
    if (info.indent != 0) {
        info.os << std::string(info.indent * info.index, info.indentCharacter);
    }
}

static inline void s_nullDump(ConfDumpInfo& info, const blet::Dict& /*dict*/) {
    info.os << "null";
}

static inline void s_numberDump(ConfDumpInfo& info, const blet::Dict& dict) {
    info.os << dict.getValue().getNumber();
}

static inline void s_booleanDump(ConfDumpInfo& info, const blet::Dict& dict) {
    if (dict.getBoolean()) {
        info.os << "true";
    }
    else {
        info.os << "false";
    }
}

static inline void s_stringDump(ConfDumpInfo& info, const blet::Dict& dict) {
    info.os << '"';
    s_stringEscape(info.os, dict.getValue().getString());
    info.os << '"';
}

// -----------------------------------------------------------------------------
// .........................................................................
// .######...####....####...##..##..........#####...##..##..##...##..#####..
// .....##..##......##..##..###.##..........##..##..##..##..###.###..##..##.
// .....##...####...##..##..##.###..........##..##..##..##..##.#.##..#####..
// .##..##......##..##..##..##..##..........##..##..##..##..##...##..##.....
// ..####....####....####...##..##..........#####....####...##...##..##.....
// .........................................................................
// -----------------------------------------------------------------------------

static void s_jsonDumpType(ConfDumpInfo& info, const blet::Dict& dict);

static inline void s_jsonDumpObject(ConfDumpInfo& info, const blet::Dict& dict) {
    if (dict.getValue().getObject().empty()) {
        info.os << "{}";
    }
    else {
        info.os << '{';
        s_newlineDump(info, dict);
        ++info.index;
        for (blet::Dict::object_t::const_iterator cit = dict.getValue().getObject().begin();
             cit != dict.getValue().getObject().end(); ++cit) {
            if (cit != dict.getValue().getObject().begin()) {
                info.os << ',';
                s_newlineDump(info, dict);
            }
            // key
            s_indentDump(info);
            if (s_forceKeyString(cit->first)) {
                info.os << '"';
                s_stringEscape(info.os, cit->first);
                info.os << '"';
            }
            else {
                s_stringEscape(info.os, cit->first);
            }
            if (info.indent > 0) {
                info.os << ' ';
            }
            info.os << '=';
            if (info.indent > 0) {
                info.os << ' ';
            }
            s_jsonDumpType(info, cit->second);
        }
        --info.index;
        s_newlineDump(info, dict);
        s_indentDump(info);
        info.os << '}';
    }
}

static inline void s_jsonDumpArray(ConfDumpInfo& info, const blet::Dict& dict) {
    if (dict.getValue().getArray().empty()) {
        info.os << "[]";
    }
    else {
        info.os << '[';
        s_newlineDump(info, dict);
        ++info.index;
        for (std::size_t i = 0; i < dict.getValue().getArray().size(); ++i) {
            if (i > 0) {
                info.os << ',';
                s_newlineDump(info, dict);
            }
            // value
            s_indentDump(info);
            s_jsonDumpType(info, dict.getValue().getArray()[i]);
        }
        --info.index;
        s_newlineDump(info, dict);
        s_indentDump(info);
        info.os << ']';
    }
}

static inline void s_jsonDumpType(ConfDumpInfo& info, const blet::Dict& dict) {
    switch (dict.getType()) {
        case blet::Dict::NULL_TYPE:
            s_nullDump(info, dict);
            break;
        case blet::Dict::BOOLEAN_TYPE:
            s_booleanDump(info, dict);
            break;
        case blet::Dict::NUMBER_TYPE:
            s_numberDump(info, dict);
            break;
        case blet::Dict::STRING_TYPE:
            s_stringDump(info, dict);
            break;
        case blet::Dict::ARRAY_TYPE:
            s_jsonDumpArray(info, dict);
            break;
        case blet::Dict::OBJECT_TYPE:
            s_jsonDumpObject(info, dict);
            break;
    }
}

static inline void s_jsonDumpObjectFirst(ConfDumpInfo& info, const blet::Dict& dict) {
    for (blet::Dict::object_t::const_iterator cit = dict.getValue().getObject().begin();
         cit != dict.getValue().getObject().end(); ++cit) {
        if (cit != dict.getValue().getObject().begin()) {
            info.os << '\n';
        }
        // key
        s_indentDump(info);
        if (s_forceKeyString(cit->first)) {
            info.os << '"';
            s_stringEscape(info.os, cit->first);
            info.os << '"';
        }
        else {
            s_stringEscape(info.os, cit->first);
        }
        if (info.indent > 0) {
            info.os << ' ';
        }
        info.os << '=';
        if (info.indent > 0) {
            info.os << ' ';
        }
        s_jsonDumpType(info, cit->second);
    }
}

static inline void s_jsonDumpTypeFirst(ConfDumpInfo& info, const blet::Dict& dict) {
    switch (dict.getType()) {
        case blet::Dict::NULL_TYPE:
            s_nullDump(info, dict);
            break;
        case blet::Dict::BOOLEAN_TYPE:
            s_booleanDump(info, dict);
            break;
        case blet::Dict::NUMBER_TYPE:
            s_numberDump(info, dict);
            break;
        case blet::Dict::STRING_TYPE:
            s_stringDump(info, dict);
            break;
        case blet::Dict::ARRAY_TYPE:
            info.os << "\"\"";
            if (info.indent > 0) {
                info.os << ' ';
            }
            info.os << '=';
            if (info.indent > 0) {
                info.os << ' ';
            }
            s_jsonDumpArray(info, dict);
            break;
        case blet::Dict::OBJECT_TYPE:
            s_jsonDumpObjectFirst(info, dict);
            break;
    }
}

// -----------------------------------------------------------------------------
// .........................................................................
// ..####....####...##..##..######..........#####...##..##..##...##..#####..
// .##..##..##..##..###.##..##..............##..##..##..##..###.###..##..##.
// .##......##..##..##.###..####............##..##..##..##..##.#.##..#####..
// .##..##..##..##..##..##..##..............##..##..##..##..##...##..##.....
// ..####....####...##..##..##..............#####....####...##...##..##.....
// .........................................................................
// -----------------------------------------------------------------------------

static void s_confDumpType(ConfDumpInfo& info, const blet::Dict& dict);

static inline void s_confDumpArray(ConfDumpInfo& info, const std::string& key, const blet::Dict& dict) {
    if (dict.getValue().getArray().empty()) {
        // key
        s_indentDump(info);
        if (s_forceKeyString(key)) {
            info.os << '"';
            s_stringEscape(info.os, key);
            info.os << '"';
        }
        else {
            s_stringEscape(info.os, key);
        }
        // value operator
        if (info.indent > 0) {
            info.os << ' ';
        }
        info.os << '=';
        if (info.indent > 0) {
            info.os << ' ';
        }
        info.os << "[]";
    }
    else {
        for (std::size_t i = 0; i < dict.getValue().getArray().size(); ++i) {
            if (i > 0) {
                info.os << '\n';
            }
            // key
            s_indentDump(info);
            if (s_forceKeyString(key)) {
                info.os << '"';
                s_stringEscape(info.os, key);
                info.os << '"';
            }
            else {
                s_stringEscape(info.os, key);
            }
            // array operator
            info.os << "[]";
            // value operator
            if (info.indent > 0) {
                info.os << ' ';
            }
            info.os << '=';
            if (info.indent > 0) {
                info.os << ' ';
            }
            // value
            switch (dict.getValue().getArray()[i].getType()) {
                case blet::Dict::NULL_TYPE:
                case blet::Dict::BOOLEAN_TYPE:
                case blet::Dict::NUMBER_TYPE:
                case blet::Dict::STRING_TYPE:
                    s_confDumpType(info, dict.getValue().getArray()[i]);
                    break;
                case blet::Dict::ARRAY_TYPE:
                    s_jsonDumpArray(info, dict.getValue().getArray()[i]);
                    break;
                case blet::Dict::OBJECT_TYPE:
                    s_jsonDumpObject(info, dict.getValue().getArray()[i]);
                    break;
            }
        }
    }
}

static inline void s_confDumpObject(ConfDumpInfo& info, const blet::Dict& dict) {
    int index = 0;
    ++info.indexSection;
    for (blet::Dict::object_t::const_iterator cit = dict.getValue().getObject().begin();
         cit != dict.getValue().getObject().end(); ++cit) {
        switch (cit->second.getType()) {
            case blet::Dict::NULL_TYPE:
            case blet::Dict::BOOLEAN_TYPE:
            case blet::Dict::NUMBER_TYPE:
            case blet::Dict::STRING_TYPE:
                if (index > 0) {
                    info.os << '\n';
                }
                ++index;
                s_indentDump(info);
                if (s_forceKeyString(cit->first)) {
                    info.os << '"';
                    s_stringEscape(info.os, cit->first);
                    info.os << '"';
                }
                else {
                    s_stringEscape(info.os, cit->first);
                }
                if (info.indent > 0) {
                    info.os << ' ';
                }
                info.os << '=';
                if (info.indent > 0) {
                    info.os << ' ';
                }
                s_confDumpType(info, cit->second);
                break;
            case blet::Dict::ARRAY_TYPE:
                if (index > 0) {
                    info.os << '\n';
                }
                ++index;
                s_confDumpArray(info, cit->first, cit->second);
                break;
            case blet::Dict::OBJECT_TYPE:
                break;
        }
    }
    for (blet::Dict::object_t::const_iterator cit = dict.getValue().getObject().begin();
         cit != dict.getValue().getObject().end(); ++cit) {
        if (cit->second.getType() == blet::Dict::OBJECT_TYPE) {
            if (index > 0) {
                info.os << '\n';
            }
            ++index;
            if (cit->second.getValue().getObject().empty()) {
                if (s_forceKeyString(cit->first)) {
                    info.os << '"';
                    s_stringEscape(info.os, cit->first);
                    info.os << '"';
                }
                else {
                    s_stringEscape(info.os, cit->first);
                }
                if (info.indent > 0) {
                    info.os << ' ';
                }
                info.os << '=';
                if (info.indent > 0) {
                    info.os << ' ';
                }
                info.os << "{}";
            }
            else {
                // new section
                info.os << std::string(info.indexSection, '[');
                if (s_forceKeyString(cit->first)) {
                    info.os << '"';
                    s_stringEscape(info.os, cit->first);
                    info.os << '"';
                }
                else {
                    s_stringEscape(info.os, cit->first);
                }
                info.os << std::string(info.indexSection, ']');
                info.os << '\n';
                s_confDumpType(info, cit->second);
            }
        }
    }
    --info.indexSection;
}

static inline void s_confDumpType(ConfDumpInfo& info, const blet::Dict& dict) {
    switch (dict.getType()) {
        case blet::Dict::NULL_TYPE:
            s_nullDump(info, dict);
            break;
        case blet::Dict::BOOLEAN_TYPE:
            s_booleanDump(info, dict);
            break;
        case blet::Dict::NUMBER_TYPE:
            s_numberDump(info, dict);
            break;
        case blet::Dict::STRING_TYPE:
            s_stringDump(info, dict);
            break;
        case blet::Dict::ARRAY_TYPE:
            s_confDumpArray(info, "", dict);
            break;
        case blet::Dict::OBJECT_TYPE:
            s_confDumpObject(info, dict);
            break;
    }
}

void dump(const blet::Dict& dict, std::ostream& os, std::size_t indent, char indentCharacter, enum EDumpStyle style) {
    os << std::setprecision(std::numeric_limits<double>::digits10 + 1);
    ConfDumpInfo info(os, indent, indentCharacter, style);
    switch (info.style) {
        case CONF_STYLE:
            s_confDumpType(info, dict);
            break;
        case JSON_STYLE:
            s_jsonDumpTypeFirst(info, dict);
            break;
    }
}

std::string dump(const blet::Dict& dict, std::size_t indent, char indentCharacter, enum EDumpStyle style) {
    std::ostringstream oss("");
    dump(dict, oss, indent, indentCharacter, style);
    return oss.str();
}

/**
 * @brief structure of info parser
 */
struct ConfParseInfo {
    ConfParseInfo(const std::string& filename_) :
        filename(filename_) {}
    ~ConfParseInfo() {}

    std::size_t line(std::size_t i) const {
        return indexToLine[i];
    }

    std::size_t column(std::size_t i) const {
        return i - lineToIndex[indexToLine[i] - 1] + 1;
    }

    std::size_t lastLine(std::size_t /*i*/) const {
        return indexToLine.back();
    }

    std::size_t lastColumn(std::size_t i) const {
        return i + 1 - lineToIndex[indexToLine.back() - 1];
    }

    const std::string& filename;
    std::vector<std::size_t> indexToLine;
    std::vector<std::size_t> lineToIndex;
    std::list<blet::Dict*> currentSections;
};

static inline std::string s_replaceEscapeChar(const std::string& str) {
    std::string ret(str);
    for (std::size_t i = 0; i < ret.size(); ++i) {
        if (ret[i] == '\\') {
            switch (ret[i + 1]) {
                case 'a':
                    ret.erase(i, 1);
                    ret[i] = '\a';
                    break;
                case 'b':
                    ret.erase(i, 1);
                    ret[i] = '\b';
                    break;
                case 'f':
                    ret.erase(i, 1);
                    ret[i] = '\f';
                    break;
                case 'n':
                    ret.erase(i, 1);
                    ret[i] = '\n';
                    break;
                case 'r':
                    ret.erase(i, 1);
                    ret[i] = '\r';
                    break;
                case 't':
                    ret.erase(i, 1);
                    ret[i] = '\t';
                    break;
                case 'v':
                    ret.erase(i, 1);
                    ret[i] = '\v';
                    break;
                case '\'':
                    ret.erase(i, 1);
                    ret[i] = '\'';
                    break;
                case '"':
                    ret.erase(i, 1);
                    ret[i] = '\"';
                    break;
                case '\\':
                    ret.erase(i, 1);
                    ret[i] = '\\';
                    break;
            }
        }
    }
    return ret;
}

static inline void s_stringJumpSpace(const std::string& str, std::size_t& index) {
    // isspace
    while ((str[index] >= '\t' && str[index] <= '\r') || str[index] == ' ') {
        ++index;
    }
}

static inline void s_stringJumpSpaceNotNewLine(const std::string& str, std::size_t& index) {
    // isspace
    while (str[index] != '\n' && ((str[index] >= '\t' && str[index] <= '\r') || str[index] == ' ')) {
        ++index;
    }
}

static inline void s_stringReverseJumpSpace(const std::string& str, std::size_t& index) {
    // isspace
    while (index > 0 && ((str[index] >= '\t' && str[index] <= '\r') || str[index] == ' ')) {
        --index;
    }
}

void s_parseType(const std::string& str, ConfParseInfo& info, std::size_t& i, blet::Dict& dict);
void s_parseJsonArray(const std::string& str, ConfParseInfo& info, std::size_t& i, blet::Dict& dict);

static inline bool s_hex(const std::string& value, std::ostream& stringStream) {
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

static inline bool s_binary(const std::string& value, std::ostream& stringStream) {
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

static inline bool s_octal(const std::string& value, std::ostream& stringStream) {
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

static inline bool s_double(const std::string& value, std::ostream& stringStream) {
    char* endPtr = NULL;
    stringStream << ::strtod(value.c_str(), &endPtr);
    if (endPtr != NULL && endPtr[0] != '\0') {
        return false;
    }
    return true;
}

static inline void s_parseNumber(const std::string& str, blet::Dict& dict) {
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

static inline void s_parseValue(const std::string& str, blet::Dict& dict) {
    std::string lowerStr(str);
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
            s_parseNumber(str, dict);
            break;
        case 'f':
            if (lowerStr == "false") {
                dict = false;
            }
            else {
                dict = str;
            }
            break;
        case 'n':
            if (lowerStr == "no") {
                dict = false;
            }
            else if (lowerStr == "null") {
                dict.newNull();
            }
            else {
                dict = str;
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
                dict = str;
            }
            break;
        case 't':
            if (lowerStr == "true") {
                dict = true;
            }
            else {
                dict = str;
            }
            break;
        case 'y':
            if (lowerStr == "yes") {
                dict = true;
            }
            else {
                dict = str;
            }
            break;
        default:
            dict = str;
            break;
    }
    if (dict.isString()) {
        dict = s_replaceEscapeChar(dict.getValue().getString());
    }
}

static inline void s_parseJsonObject(const std::string& str, ConfParseInfo& info, std::size_t& i, blet::Dict& dict) {
    blet::Dict* currentDict;
    ConfParseInfo valueInfo = info;
    std::size_t start;
    std::size_t end;
    bool next = false;

    dict.newObject();
    ++i; // jump '{'
    s_stringJumpSpace(str, i);
    while (str[i] != '}' || next) {
        if (str[i] == '\0') {
            throw LoadException(info.filename, info.lastLine(i), info.lastColumn(i), "End of json object value");
        }
        // parse key
        if (str[i] == '=') {
            throw LoadException(info.filename, info.line(i), info.column(i), "Key of json object value");
        }
        // start key name
        if (str[i] == '\"' || str[i] == '\'') {
            // get quote character
            const char quote = str[i];
            ++i; // jump quote
            start = i;
            // search end quote
            while (str[i] != quote) {
                if (str[i] == '\\' && (str[i + 1] == quote || str[i + 1] == '\\')) {
                    ++i;
                }
                if (str[i] == '\0' || str[i] == '\n') {
                    throw LoadException(info.filename, info.line(i), info.column(i),
                                        "End of quote at key in json object value");
                }
                ++i;
            }
            end = i;
            ++i; // jump quote
        }
        else {
            start = i;
            while (str[i] != '=') {
                if (str[i] == '\0' || str[i] == '\n') {
                    throw LoadException(info.filename, info.line(i), info.column(i), "End of key in json object value");
                }
                ++i;
            }
            end = i - 1;
            s_stringReverseJumpSpace(str, end);
            ++end;
        }
        currentDict = &(dict.operator[](s_replaceEscapeChar(str.substr(start, end - start))));
        valueInfo.currentSections.push_back(currentDict);
        s_stringJumpSpace(str, i);
        if (str[i] != '=') {
            throw LoadException(info.filename, info.line(i), info.column(i),
                                "Operator = not found in json object value");
        }
        ++i; // jump '='
        s_stringJumpSpace(str, i);
        // start value
        if (str[i] == '\"' || str[i] == '\'') {
            // get quote character
            const char quote = str[i];
            ++i; // jump quote
            start = i;
            // search end quote
            while (str[i] != quote) {
                if (str[i] == '\\' && (str[i + 1] == quote || str[i + 1] == '\\')) {
                    ++i;
                }
                if (str[i] == '\0' || str[i] == '\n') {
                    throw LoadException(info.filename, info.line(i), info.column(i),
                                        "End of quote at value in json object value");
                }
                ++i;
            }
            end = i;
            ++i; // jump quote
            s_stringJumpSpace(str, i);
            *currentDict = s_replaceEscapeChar(str.substr(start, end - start));
        }
        else {
            switch (str[i]) {
                case '{':
                    s_parseJsonObject(str, valueInfo, i, *currentDict);
                    break;
                case '[':
                    s_parseJsonArray(str, valueInfo, i, *currentDict);
                    break;
                default:
                    start = i;
                    while (str[i] != ',' && str[i] != '}' && str[i] != '\n' && str[i] != '\0') {
                        ++i;
                    }
                    end = i - 1;
                    s_stringReverseJumpSpace(str, end);
                    ++end;
                    s_parseValue(str.substr(start, end - start), *currentDict);
                    break;
            }
        }
        s_stringJumpSpace(str, i);
        if (str[i] == ',') {
            ++i; // jump ','
            s_stringJumpSpace(str, i);
            next = true;
        }
        else {
            next = false;
        }
    }
    ++i; // jump '}'
    s_stringJumpSpace(str, i);
}

void s_parseJsonArray(const std::string& str, ConfParseInfo& info, std::size_t& i, blet::Dict& dict) {
    std::size_t start;
    std::size_t end;
    bool next = false;

    dict.newArray();
    ++i; // jump '['
    s_stringJumpSpace(str, i);
    // parse value array
    while (str[i] != ']' || next) {
        if (str[i] == '\0') {
            throw LoadException(info.filename, info.line(i), info.column(i), "End of json array value");
        }
        // start value
        if (str[i] == '\"' || str[i] == '\'') {
            // get quote character
            const char quote = str[i];
            ++i; // jump quote
            start = i;
            // search end quote
            while (str[i] != quote) {
                if (str[i] == '\\' && (str[i + 1] == quote || str[i + 1] == '\\')) {
                    ++i;
                }
                if (str[i] == '\0' || str[i] == '\n') {
                    throw LoadException(info.filename, info.line(i), info.column(i),
                                        "End of quote in json array value");
                }
                ++i;
            }
            end = i;
            ++i; // jump quote
            s_stringJumpSpaceNotNewLine(str, i);
            if (str[i] != ',' && str[i] != ']' && str[i] != '\n' && str[i] != '\0') {
                throw LoadException(info.filename, info.line(i), info.column(i), "End of value in json array value");
            }
            dict.push_back(s_replaceEscapeChar(str.substr(start, end - start)));
        }
        else {
            dict.push_back(blet::Dict());
            info.currentSections.push_back(&(dict.back()));
            switch (str[i]) {
                case '{':
                    s_parseJsonObject(str, info, i, dict.back());
                    break;
                case '[':
                    s_parseJsonArray(str, info, i, dict.back());
                    break;
                default:
                    start = i;
                    while (str[i] != ',' && str[i] != ']' && str[i] != '\n' && str[i] != '\0') {
                        ++i;
                    }
                    end = i - 1;
                    s_stringReverseJumpSpace(str, end);
                    ++end;
                    s_parseValue(str.substr(start, end - start), dict.back());
            }
        }
        s_stringJumpSpace(str, i);
        if (str[i] == ',') {
            ++i; // jump ','
            s_stringJumpSpace(str, i);
            next = true;
        }
        else {
            next = false;
        }
    }
    ++i; // jump ']'
    s_stringJumpSpace(str, i);
}

static void s_parseSection(const std::string& str, ConfParseInfo& info, std::size_t& i, blet::Dict& dict) {
    std::size_t level = 1;
    std::size_t start;
    std::size_t end;
    ++i; // jump '['
    s_stringJumpSpaceNotNewLine(str, i);
    // is multi section level
    if (str[i] == '[') {
        while (str[i] == '[') {
            ++i; // jump '['
            ++level;
            s_stringJumpSpaceNotNewLine(str, i);
        }
        // start section name
        if (str[i] == '\"' || str[i] == '\'') {
            // get quote character
            const char quote = str[i];
            ++i; // jump quote
            start = i;
            // search end quote
            while (str[i] != quote) {
                if (str[i] == '\\' && (str[i + 1] == quote || str[i + 1] == '\\')) {
                    ++i;
                }
                if (str[i] == '\0' || str[i] == '\n') {
                    throw LoadException(info.filename, info.line(i), info.column(i), "End of quote multi section");
                }
                ++i;
            }
            end = i;
            ++i; // jump quote
            s_stringJumpSpaceNotNewLine(str, i);
        }
        else {
            start = i;
            while (str[i] != ']') {
                if (str[i] == '\0' || str[i] == '\n') {
                    throw LoadException(info.filename, info.line(i), info.column(i), "End of multi section");
                }
                ++i;
            }
            end = i - 1;
            s_stringReverseJumpSpace(str, end);
            ++end;
        }
        std::size_t maxlevel = level;
        while (str[i] == ']') {
            ++i; // jump ']'
            --level;
            s_stringJumpSpaceNotNewLine(str, i);
        }
        if (level != 0) {
            throw LoadException(info.filename, info.line(i), info.column(i), "End of multi section");
        }
        --maxlevel;
        while (info.currentSections.size() > maxlevel) {
            info.currentSections.pop_back();
        }
        if (maxlevel == info.currentSections.size()) {
            info.currentSections.push_back(
                &(info.currentSections.back()->operator[](s_replaceEscapeChar(str.substr(start, end - start)))));
        }
        else {
            throw LoadException(info.filename, info.line(i), info.column(i), "Multi section without parent");
        }
        s_stringJumpSpaceNotNewLine(str, i);
    }
    else {
        // start section name
        if (str[i] == '\"' || str[i] == '\'') {
            // get quote character
            const char quote = str[i];
            ++i; // jump quote
            start = i;
            // search end quote
            while (str[i] != quote) {
                if (str[i] == '\\' && (str[i + 1] == quote || str[i + 1] == '\\')) {
                    ++i;
                }
                if (str[i] == '\0' || str[i] == '\n') {
                    throw LoadException(info.filename, info.line(i), info.column(i), "End of quote section");
                }
                ++i;
            }
            end = i;
            ++i; // jump quote
            s_stringJumpSpaceNotNewLine(str, i);
            if (str[i] != ']') {
                throw LoadException(info.filename, info.line(i), info.column(i), "End of section");
            }
            ++i; // jump ']'
        }
        else {
            start = i;
            while (str[i] != ']') {
                if (str[i] == '\0' || str[i] == '\n') {
                    throw LoadException(info.filename, info.line(i), info.column(i), "End of section");
                }
                ++i;
            }
            end = i - 1;
            ++i; // jump ']'
            s_stringReverseJumpSpace(str, end);
            ++end;
        }
        info.currentSections.clear();
        if (end - start > 0) {
            info.currentSections.push_back(&(dict[s_replaceEscapeChar(str.substr(start, end - start))]));
        }
        else {
            info.currentSections.push_back(&dict);
        }
        s_stringJumpSpaceNotNewLine(str, i);
        while (str[i] == '[') {
            ++i;
            s_stringJumpSpaceNotNewLine(str, i);
            // start section name
            if (str[i] == '\"' || str[i] == '\'') {
                // get quote character
                const char quote = str[i];
                ++i; // jump quote
                start = i;
                // search end quote
                while (str[i] != quote) {
                    if (str[i] == '\\' && (str[i + 1] == quote || str[i + 1] == '\\')) {
                        ++i;
                    }
                    if (str[i] == '\0' || str[i] == '\n') {
                        throw LoadException(info.filename, info.line(i), info.column(i), "End of quote linear section");
                    }
                    ++i;
                }
                end = i;
                ++i; // jump quote
                s_stringJumpSpaceNotNewLine(str, i);
                if (str[i] != ']') {
                    throw LoadException(info.filename, info.line(i), info.column(i), "End of linear section");
                }
                ++i; // jump ']'
            }
            else {
                start = i;
                while (str[i] != ']') {
                    if (str[i] == '\0' || str[i] == '\n') {
                        throw LoadException(info.filename, info.line(i), info.column(i), "End of linear section");
                    }
                    ++i;
                }
                end = i - 1;
                ++i; // jump ']'
                s_stringReverseJumpSpace(str, end);
                ++end;
            }
            info.currentSections.push_back(
                &(info.currentSections.back()->operator[](s_replaceEscapeChar(str.substr(start, end - start)))));
            s_stringJumpSpaceNotNewLine(str, i);
        }
    }
    s_stringJumpSpace(str, i);
}

inline void s_parseKeyValue(const std::string& str, ConfParseInfo& info, std::size_t& i, blet::Dict& dict) {
    blet::Dict* currentDict = &dict;
    ConfParseInfo valueInfo = info;
    if (str[i] == '=') {
        throw LoadException(info.filename, info.line(i), info.column(i), "Key not found");
    }
    std::size_t start;
    std::size_t end;
    // start key name
    if (str[i] == '\"' || str[i] == '\'') {
        // get quote character
        const char quote = str[i];
        ++i; // jump quote
        start = i;
        // search end quote
        while (str[i] != quote) {
            if (str[i] == '\\' && (str[i + 1] == quote || str[i + 1] == '\\')) {
                ++i;
            }
            if (str[i] == '\0' || str[i] == '\n') {
                throw LoadException(info.filename, info.line(i), info.column(i), "End of quote");
            }
            ++i;
        }
        end = i;
        ++i; // jump quote
        s_stringJumpSpaceNotNewLine(str, i);
        if (str[i] != '=' && str[i] != '[') {
            throw LoadException(info.filename, info.line(i), info.column(i), "End of key");
        }
    }
    else {
        start = i;
        while (str[i] != '=' && str[i] != '[') {
            if (str[i] == '\0' || str[i] == '\n') {
                throw LoadException(info.filename, info.line(i), info.column(i), "End of key");
            }
            ++i;
        }
        end = i - 1;
        s_stringReverseJumpSpace(str, end);
        ++end;
    }
    currentDict = &(currentDict->operator[](s_replaceEscapeChar(str.substr(start, end - start))));
    valueInfo.currentSections.push_back(currentDict);
    // check table key
    while (str[i] == '[') {
        ++i; // jump '['
        s_stringJumpSpaceNotNewLine(str, i);
        // start key name
        if (str[i] == '\"' || str[i] == '\'') {
            // get quote character
            const char quote = str[i];
            ++i; // jump quote
            start = i;
            // search end quote
            while (str[i] != quote) {
                if (str[i] == '\\' && (str[i + 1] == quote || str[i + 1] == '\\')) {
                    ++i;
                }
                if (str[i] == '\0' || str[i] == '\n') {
                    throw LoadException(info.filename, info.line(i), info.column(i), "End of quote");
                }
                ++i;
            }
            end = i;
            ++i; // jump quote
            s_stringJumpSpaceNotNewLine(str, i);
            if (str[i] != ']') {
                throw LoadException(info.filename, info.line(i), info.column(i), "End of key of map");
            }
        }
        else {
            start = i;
            while (str[i] != ']') {
                if (str[i] == '\0' || str[i] == '\n') {
                    throw LoadException(info.filename, info.line(i), info.column(i), "End of map");
                }
                ++i;
            }
            end = i - 1;
            s_stringReverseJumpSpace(str, end);
            ++end;
        }
        ++i; // jump ']'
        s_stringJumpSpaceNotNewLine(str, i);
        if (end - start > 0) {
            currentDict = &(currentDict->operator[](s_replaceEscapeChar(str.substr(start, end - start))));
        }
        else {
            currentDict->push_back(blet::Dict());
            currentDict = &(currentDict->back());
        }
        valueInfo.currentSections.push_back(currentDict);
    }
    s_stringJumpSpaceNotNewLine(str, i);
    if (str[i] != '=') {
        throw LoadException(info.filename, info.line(i), info.column(i), "Operator = not found");
    }
    ++i; // jump '='
    s_stringJumpSpaceNotNewLine(str, i);
    // start value
    if (str[i] == '\"' || str[i] == '\'') {
        // get quote character
        const char quote = str[i];
        ++i; // jump quote
        start = i;
        // search end quote
        while (str[i] != quote) {
            if (str[i] == '\\' && (str[i + 1] == quote || str[i + 1] == '\\')) {
                ++i;
            }
            if (str[i] == '\0' || str[i] == '\n') {
                throw LoadException(info.filename, info.line(i), info.column(i), "End of quote");
            }
            ++i;
        }
        end = i;
        ++i; // jump quote
        s_stringJumpSpaceNotNewLine(str, i);
        if (str[i] != '\n' && str[i] != '\0') {
            throw LoadException(info.filename, info.line(i), info.column(i), "End of value");
        }
        *currentDict = s_replaceEscapeChar(str.substr(start, end - start));
    }
    else {
        switch (str[i]) {
            case '{':
                s_parseJsonObject(str, valueInfo, i, *currentDict);
                break;
            case '[':
                s_parseJsonArray(str, valueInfo, i, *currentDict);
                break;
            default:
                start = i;
                while (str[i] != '\n' && str[i] != '\0') {
                    ++i;
                }
                end = i - 1;
                s_stringReverseJumpSpace(str, end);
                ++end;
                s_parseValue(str.substr(start, end - start), *currentDict);
                break;
        }
    }
    s_stringJumpSpace(str, i);
}

inline void s_parseType(const std::string& str, ConfParseInfo& info, std::size_t& i, blet::Dict& dict) {
    if (str[i] == '[') {
        s_parseSection(str, info, i, dict);
    }
    else {
        if (info.currentSections.empty()) {
            info.currentSections.push_back(&dict);
        }
        s_parseKeyValue(str, info, i, *(info.currentSections.back()));
    }
}

static void s_replaceCommentBySpace(std::string& str) {
    // replace double backslash
    for (std::size_t i = 0; i < str.size() && str[i] != '\0'; ++i) {
        if (str[i] == '\\' && str[i + 1] == '\\') {
            str[i] = replaceBackslash;
            str[i + 1] = replaceBackslash;
        }
    }
    for (std::size_t i = 0; i < str.size() && str[i] != '\0'; ++i) {
        // jump inside of string
        if (str[i] == '"' || str[i] == '\'') {
            const char quote = str[i];
            ++i;
            while (str[i] != quote) {
                if (str[i] == '\\' && (str[i + 1] == quote || str[i + 1] == '\\')) {
                    ++i; // escape character
                }
                if (str[i] == '\n' || str[i] == '\0') {
                    break;
                }
                ++i;
            }
        }
        // replace
        else if ((str[i] == '#' || str[i] == ';') && (i == 0 || (i > 0 && str[i - 1] != '\\'))) {
            while (str[i] != '\0' && str[i] != '\n') {
                str[i] = ' ';
                ++i;
            }
        }
    }
    // replace double backslash
    for (std::size_t i = 0; i < str.size() && str[i] != '\0'; ++i) {
        if (str[i] == replaceBackslash && str[i + 1] == replaceBackslash) {
            str[i] = '\\';
            str[i + 1] = '\\';
        }
    }
}

static std::string s_streamToStr(ConfParseInfo& info, std::istream& stream) {
    // generate line and column index
    std::ostringstream oss("");
    std::string line("");
    std::size_t nLine = 1;
    info.lineToIndex.push_back(0);
    while (std::getline(stream, line)) {
        if (nLine > 1) {
            oss << '\n';
        }
        oss << line;
        info.lineToIndex.push_back(line.size() + 1 + info.lineToIndex.back());
        info.indexToLine.insert(info.indexToLine.end(), line.size() + 1, nLine);
        ++nLine;
    }
    return oss.str();
}

static blet::Dict s_loadStream(std::istream& stream, const std::string& filename) {
    ConfParseInfo info(filename);
    std::string str = s_streamToStr(info, stream);
    s_replaceCommentBySpace(str);
    std::size_t i = 0;
    s_stringJumpSpace(str, i);
    try {
        blet::Dict dict;
        while (str[i] != '\0') {
            s_parseType(str, info, i, dict);
        }
        return dict;
    }
    catch (const LoadException& /*e*/) {
        throw;
    }
}

blet::Dict loadFile(const char* filename) {
    std::ifstream fileStream(filename); // open file
    if (!fileStream.is_open()) {
        throw LoadException(filename, "Open file failed");
    }
    return s_loadStream(fileStream, filename);
}

blet::Dict loadStream(std::istream& stream) {
    return s_loadStream(stream, std::string());
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
