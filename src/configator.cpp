/**
 * configator.cpp
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

#include <list>

#include "mblet/configator.hpp"

namespace mblet {

Configator::Configator():
    _mapConfig(Configator::Map()),
    _filename(std::string()),
    _isRead(false) {
    return ;
}

Configator::Configator(const char* filename):
    _mapConfig(Configator::Map()),
    _filename(std::string()),
    _isRead(false) {
    readFile(filename);
    return ;
}

Configator::Configator(const Configator& src):
    _mapConfig(src._mapConfig),
    _filename(src._filename),
    _isRead(src._isRead) {
    return ;
}

Configator::~Configator() {
    return ;
}

Configator& Configator::operator=(const Configator& rhs) {
    _mapConfig = rhs._mapConfig;
    _filename  = rhs._filename;
    _isRead    = rhs._isRead;
    return *this;
}

bool Configator::readFile(const char* filename) {
    _mapConfig.clear();
    _filename = "";
    _isRead   = false;

    std::ifstream fileStream(filename); // open file
    if (fileStream.is_open()) {
        _filename = filename;
        _isRead   = true;
        parseStream(fileStream); // parse file
        fileStream.close();
    }
    return _isRead;
}

// =============================================================================
// dump

static inline void s_printCommentDump(std::ostream& oss, const std::string& str) {
    if (!str.empty()) {
        oss << " ; " << str;
    }
}

static inline void s_printDump(std::ostream& oss, const std::string& str) {
    unsigned int i;
    for (i = 0; i < str.size(); ++i) {
        if (str[i] == ' ' || str[i] == '"' || str[i] == '#' || str[i] == ';' || str[i] == '\\' || str[i] == '['
            || str[i] == ']') {
            break;
        }
    }
    if (i < str.size()) {
        oss << '"';
        for (i = 0; i < str.size(); ++i) {
            if (str[i] == '"' || str[i] == '\\') {
                oss << '\\' << str[i];
            }
            else {
                oss << str[i];
            }
        }
        oss << '"';
    }
    else {
        oss << str;
    }
}

static inline void s_sectionCommentDump(std::ostream& oss, const std::string& str) {
    if (!str.empty()) {
        oss << "; ";
        unsigned int i;
        for (i = 0; i < str.size(); ++i) {
            oss << str[i];
            if (str[i] == '\n') {
                oss << "; ";
            }
        }
        oss << '\n';
    }
}

static inline void s_sectionDump(std::ostream& oss, const std::string& str, std::size_t sectionIndex) {
    oss << std::string(sectionIndex, '[');
    s_printDump(oss, str);
    oss << std::string(sectionIndex, ']');
}

static void s_recurseDump(std::ostream& oss, const Configator::Map& map, std::size_t sectionIndex = 1) {
    Configator::Map::const_iterator itSection;
    for (itSection = map.begin(); itSection != map.end(); ++itSection) {
        if (itSection->second.empty()) {
            s_printDump(oss, itSection->first);
            oss << " = ";
            s_printDump(oss, itSection->second.getString());
            s_printCommentDump(oss, itSection->second.getComment());
            oss << '\n';
        }
    }
    for (itSection = map.begin(); itSection != map.end(); ++itSection) {
        if (itSection->second.size() > 0) {
            if (!itSection->second.getString().empty()) {
                s_printDump(oss, itSection->first);
                oss << " = ";
                s_printDump(oss, itSection->second.getString());
                oss << '\n';
            }
            s_sectionDump(oss, itSection->first, sectionIndex);
            oss << '\n';
            s_sectionCommentDump(oss, itSection->second.getComment());
            s_recurseDump(oss, itSection->second, sectionIndex + 1);
        }
    }
}

std::ostream& Configator::dump(std::ostream& oss) const {
    s_recurseDump(oss, _mapConfig);
    return oss;
}

// =============================================================================
// parse

/**
 * @brief check if character is comment
 *
 * @param c
 * @return true : c is comment character
 * @return false : c is not comment character
 */
static inline bool s_isComment(const char& c) {
    if (c == ';' || c == '#') {
        return true;
    }
    else {
        return false;
    }
}

/**
 * @brief move index to character after spaces
 *
 * @param str
 * @param index
 */
static inline void s_stringJumpSpace(const std::string& str, std::size_t& index) {
    while (::isspace(str[index])) {
        ++index;
    }
}

/**
 * @brief detect if line is empty or comment
 *
 * @param line
 * @return true : line is empty or comment
 * @return false : line is not empty or comment
 */
static inline bool s_emptyOrComment(const std::string& line, std::string* retComment) {
    std::size_t start;
    std::size_t end;
    std::size_t i = 0;

    s_stringJumpSpace(line, i);
    if (line[i] == '\0') {
        return true; // empty line
    }
    if (!s_isComment(line[i])) {
        return false;
    }
    ++i; // jump character ';' or '#'
    s_stringJumpSpace(line, i);
    start = i;
    while (line[i] != '\0') {
        ++i;
    }
    --i; // revert jump '\0'
    while (i > 0 && ::isspace(line[i])) {
        --i;
    }
    ++i; // last character
    end = i;
    *retComment = line.substr(start, end - start);
    return true;
}

/**
 * @brief parse section name
 *
 * @param line
 * @param retSection
 * @param retComment
 * @return true
 * @return false
 */
static inline bool s_parseSections(std::string line, std::list<std::string>* retSection, std::string* retComment) {
    char                   quote;
    std::size_t            start;
    std::size_t            end;
    std::size_t            last;
    std::size_t            level = 0;
    std::size_t            i = 0;
    std::list<std::string> tmpSection = *retSection;

    s_stringJumpSpace(line, i);
    // if not begin section
    if (line[i] != '[') {
        return false;
    }
    while (line[i] == '[') {
        ++i; // jump character '['
        ++level;
        s_stringJumpSpace(line, i);
        if (line[i] == '[') {
            return false;
        }
        // start section name
        if (line[i] == '\"' || line[i] == '\'') {
            // get quote character
            quote = line[i];
            ++i; // jump quote
            start = i;
            // search end quote
            while (line[i] != quote) {
                if (line[i] == '\\' && (line[i + 1] == quote || line[i + 1] == '\\')) {
                    line.erase(i, 1);
                }
                if (line[i] == '\0') {
                    return false;
                }
                ++i;
            }
            end = i;
            ++i; // jump quote
            s_stringJumpSpace(line, i);
        }
        else {
            start = i;
            while (line[i] != ']') {
                if (line[i] == '\0') {
                    return false;
                }
                ++i;
            }
            last = i;
            --i; // revert jump ']'
            while (i > 0 && ::isspace(line[i])) {
                --i;
            }
            ++i; // last character
            end = i;
            i = last;
        }
        if (line[i] != ']') {
            return false;
        }
        ++i; // jump ]
        if (level == 1) {
            tmpSection.clear();
        }
        tmpSection.push_back(line.substr(start, end - start));
        s_stringJumpSpace(line, i);
    }
    if (line[i] != '\0' && !s_isComment(line[i])) {
        tmpSection.pop_back();
        return false;
    }
    if (s_isComment(line[i])) {
        ++i; // jump character ';' or '#'
        s_stringJumpSpace(line, i);
        start = i;
        while (line[i] != '\0') {
            ++i;
        }
        --i; // revert jump '\0'
        while (i > 0 && ::isspace(line[i])) {
            --i;
        }
        ++i; // last character
        end = i;
        *retComment = line.substr(start, end - start);
    }
    *retSection = tmpSection;
    return true;
}

/**
 * @brief parse section name
 *
 * @param line
 * @param retSection
 * @param retComment
 * @return true
 * @return false
 */
static inline bool s_parseSectionLevel(std::string line, std::list<std::string>* retSection, std::string* retComment) {
    char                   quote;
    std::size_t            start;
    std::size_t            end;
    std::size_t            last;
    std::size_t            level = 0;
    std::size_t            saveLevel = 0;
    std::size_t            i = 0;
    std::list<std::string> tmpSection = *retSection;

    s_stringJumpSpace(line, i);
    // if not begin section
    if (line[i] != '[') {
        return false;
    }
    while (line[i] == '[') {
        ++i; // jump character '['
        ++level;
        s_stringJumpSpace(line, i);
    }
    // start section name
    if (line[i] == '\"' || line[i] == '\'') {
        // get quote character
        quote = line[i];
        ++i; // jump quote
        start = i;
        // search end quote
        while (line[i] != quote) {
            if (line[i] == '\\' && (line[i + 1] == quote || line[i + 1] == '\\')) {
                line.erase(i, 1);
            }
            if (line[i] == '\0') {
                return false;
            }
            ++i;
        }
        end = i;
        ++i; // jump quote
        s_stringJumpSpace(line, i);
    }
    else {
        start = i;
        while (line[i] != ']') {
            if (line[i] == '\0') {
                return false;
            }
            ++i;
        }
        last = i;
        --i; // revert jump ']'
        while (i > 0 && ::isspace(line[i])) {
            --i;
        }
        ++i; // last character
        end = i;
        i = last;
    }
    saveLevel = level;
    while (line[i] == ']') {
        ++i; // jump character ']'
        --level;
        s_stringJumpSpace(line, i);
    }
    if (level != 0) {
        return false;
    }
    --saveLevel;
    while (tmpSection.size() > saveLevel) {
        tmpSection.pop_back();
    }
    if (saveLevel == tmpSection.size()) {
        tmpSection.push_back(line.substr(start, end - start));
    }
    else {
        return false;
    }
    s_stringJumpSpace(line, i);
    if (line[i] != '\0' && !s_isComment(line[i])) {
        return false;
    }
    if (s_isComment(line[i])) {
        ++i; // jump character ';' or '#'
        s_stringJumpSpace(line, i);
        start = i;
        while (line[i] != '\0') {
            ++i;
        }
        --i; // revert jump '\0'
        while (i > 0 && ::isspace(line[i])) {
            --i;
        }
        ++i; // last character
        end = i;
        *retComment = line.substr(start, end - start);
    }
    *retSection = tmpSection;
    return true;
}

/**
 * @brief parse key
 *
 * @param line
 * @param retKey
 * @param retValue
 * @param retComment
 * @return true
 * @return false
 */
static inline bool s_parseKey(std::string line, std::list<std::string>* retKey, std::string* retValue,
                              std::string* retComment) {
    char        quote;
    std::size_t start;
    std::size_t end;
    std::size_t last;
    std::size_t i = 0;

    s_stringJumpSpace(line, i);
    if (line[i] == '=') {
        return false; // not key element
    }
    // start key name
    if (line[i] == '\"' || line[i] == '\'') {
        // get quote character
        quote = line[i];
        ++i; // jump quote
        start = i;
        // search end quote
        while (line[i] != quote) {
            if (line[i] == '\\' && (line[i + 1] == quote || line[i + 1] == '\\')) {
                line.erase(i, 1);
            }
            if (line[i] == '\0') {
                return false;
            }
            ++i;
        }
        end = i;
        ++i; // jump quote
        s_stringJumpSpace(line, i);
        if (line[i] != '=' && line[i] != '[') {
            return false; // not valid key
        }
    }
    else {
        start = i;
        while (line[i] != '=' && line[i] != '[') {
            if (line[i] == '\0') {
                return false;
            }
            ++i;
        }
        last = i;
        --i; // revert jump '=' or '['
        while (i > 0 && ::isspace(line[i])) {
            --i;
        }
        ++i; // last character
        end = i;
        i = last;
    }
    retKey->push_back(line.substr(start, end - start));
    // check table key
    while (line[i] == '[') {
        ++i; // jump '['
        s_stringJumpSpace(line, i);
        // start key name
        if (line[i] == '\"' || line[i] == '\'') {
            // get quote character
            quote = line[i];
            ++i; // jump quote
            start = i;
            // search end quote
            while (line[i] != quote) {
                if (line[i] == '\\' && (line[i + 1] == quote || line[i + 1] == '\\')) {
                    line.erase(i, 1);
                }
                if (line[i] == '\0') {
                    return false;
                }
                ++i;
            }
            end = i;
            ++i; // jump quote
            s_stringJumpSpace(line, i);
            if (line[i] != ']') {
                return false; // not valid key
            }
        }
        else {
            start = i;
            while (line[i] != ']') {
                if (line[i] == '\0') {
                    return false;
                }
                ++i;
            }
            last = i;
            --i; // revert jump '=' or '['
            while (i > 0 && ::isspace(line[i])) {
                --i;
            }
            ++i; // last character
            end = i;
            i = last;
        }
        ++i; // jump ']'
        s_stringJumpSpace(line, i);
        retKey->push_back(line.substr(start, end - start));
    }
    s_stringJumpSpace(line, i);
    if (line[i] != '=') {
        return false;
    }
    ++i; // jump '='
    s_stringJumpSpace(line, i);
    // start value
    if (line[i] == '\"' || line[i] == '\'') {
        // get quote character
        quote = line[i];
        ++i; // jump quote
        start = i;
        // search end quote
        while (line[i] != quote) {
            if (line[i] == '\\' && (line[i + 1] == quote || line[i + 1] == '\\')) {
                line.erase(i, 1);
            }
            if (line[i] == '\0') {
                return false;
            }
            ++i;
        }
        end = i;
        ++i; // jump quote
        s_stringJumpSpace(line, i);
        if (line[i] != '\0' && !s_isComment(line[i])) {
            return false; // not valid value
        }
    }
    else {
        start = i;
        while (line[i] != '\0' && !s_isComment(line[i])) {
            ++i;
        }
        last = i;
        --i; // revert jump '\0' or ';' or '#'
        while (i > 0 && ::isspace(line[i])) {
            --i;
        }
        ++i; // last character
        end = i;
        i = last;
    }
    *retValue = line.substr(start, end - start);
    if (s_isComment(line[i])) {
        ++i; // jump character ';' or '#'
        s_stringJumpSpace(line, i);
        start = i;
        while (line[i] != '\0') {
            ++i;
        }
        --i; // revert jump '\0'
        while (i > 0 && ::isspace(line[i])) {
            --i;
        }
        ++i; // last character
        end = i;
        *retComment = line.substr(start, end - start);
    }
    return true;
}

/**
 * @brief get map of section
 *
 * @param map
 * @param sections
 * @return Configator::Map&
 */
static inline Configator::Map& s_section(Configator::Map& map, const std::list<std::string>& sections) {
    std::list<std::string>::const_iterator itSection;

    Configator::Map* pMap = &map;
    for (itSection = sections.begin(); itSection != sections.end(); ++itSection) {
        pMap = &((*pMap)[*itSection]);
    }
    return *pMap;
}

void Configator::parseStream(std::istream& stream) {
    std::string            line("");
    std::list<std::string> sections;

    // default section
    sections.push_back("");

    while (std::getline(stream, line)) {
        std::list<std::string> keys;
        std::string comment = "";
        std::string value   = "";

        if (s_emptyOrComment(line, &comment)) {
            if (!comment.empty()) {
                Configator::Map& map = s_section(_mapConfig, sections);
                map._isExist = true;
                if (!map._comment.empty()) {
                    map._comment.append("\n");
                }
                map._comment.append(comment);
            }
        }
        else if (s_parseSections(line, &sections, &comment)) {
            Configator::Map& map = s_section(_mapConfig, sections);
            map._isExist = true;
            map._comment = comment;
        }
        else if (s_parseSectionLevel(line, &sections, &comment)) {
            Configator::Map& map = s_section(_mapConfig, sections);
            map._isExist = true;
            map._comment = comment;
        }
        else if (s_parseKey(line, &keys, &value, &comment)) {
            Configator::Map& map = s_section(_mapConfig, sections);
            Configator::Map* tmpMap = &(map);
            std::list<std::string>::iterator it;

            for (it = keys.begin(); it != keys.end() ; ++it) {
                if (it->empty()) {
                    char str[32];
                    snprintf(str, sizeof(str), "%lu", static_cast<unsigned long>(tmpMap->size()));
                    tmpMap = &((*tmpMap)[str]);
                }
                else {
                    tmpMap = &((*tmpMap)[*it]);
                }
            }
            *tmpMap = value;
            tmpMap->_comment = comment;
        }
    }
}

}
