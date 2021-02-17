/**
 * configator.cpp
 *
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * Copyright (c) 2020 BLET Mickaël.
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

#include "configator.hpp"

namespace mblet {

Configator::Configator():
    _mapConfig(Configator::Map()),
    _filename(std::string()),
    _isRead(false) {
    return ;
}

Configator::Configator(const std::string& filename):
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

Configator::Map& Configator::operator[](std::size_t index) {
    return _mapConfig[index];
}

const Configator::Map& Configator::operator[](std::size_t index) const {
    return _mapConfig[index];
}

Configator::Map& Configator::operator[](const std::string& str) {
    return _mapConfig[str];
}

const Configator::Map& Configator::operator[](const std::string& str) const {
    return _mapConfig[str];
}

bool Configator::readFile(const std::string& filename) {
    _mapConfig.clear();
    _filename = "";
    _isRead   = false;

    std::ifstream fileStream(filename.c_str()); // open file
    if (fileStream.is_open()) {
        _filename = filename;
        _isRead   = true;
        readStream(fileStream); // parse file
        fileStream.close();
    }
    return _isRead;
}

void Configator::setConfig(const Configator::Map& mapConfig) {
    _mapConfig.clear();
    _mapConfig = mapConfig;
}

const Configator::Map& Configator::getConfig() const {
    return _mapConfig;
}

const std::string& Configator::getFilename() const {
    return _filename;
}

bool Configator::isRead() const {
    return _isRead;
}

// =============================================================================
// dump

static void s_recurseDump(std::ostream& oss, const Configator::Map& map, std::size_t indent, std::size_t tab = 0) {
    Configator::Map::const_iterator itSection;

    for (itSection = map.begin(); itSection != map.end(); ++itSection) {
        oss << std::string(tab * indent, ' ') << itSection->first << ':';
        if (itSection->second.size() > 0) {
            oss << std::endl;
            s_recurseDump(oss, itSection->second, indent, tab + 1);
        }
        else {
            oss << ' ' << itSection->second << std::endl;
        }
    }
}

std::ostream& Configator::dump(std::ostream& oss, std::size_t indent) const {
    s_recurseDump(oss, _mapConfig, indent);
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
static bool s_isComment(const char& c) {
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
static void s_stringJumpSpace(const std::string& str, std::size_t& index) {
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
static bool s_emptyOrComment(const std::string& line, std::string* retComment) {
    std::size_t start;
    std::size_t end;
    std::size_t i = 0;

    s_stringJumpSpace(line, i);
    if (line[i] != '\0' && !s_isComment(line[i])) {
        return false;
    }
    ++i; // jump character ';' or '#'
    s_stringJumpSpace(line, i);
    start = i;
    while (line[i] != '\0') {
        ++i;
    }
    --i; // revert jump '\0'
    while (i > 0 && isspace(line[i])) {
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
static bool s_parseSection(std::string line, std::list<std::string>* retSection, std::string* retComment) {
    char        quote;
    std::size_t start;
    std::size_t end;
    std::size_t last;
    std::size_t level = 0;
    std::size_t saveLevel = 0;
    std::size_t i = 0;

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
            if (line[i] == '\\') {
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
        while (i > 0 && isspace(line[i])) {
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
    if (saveLevel == 1) {
        retSection->clear();
        retSection->push_back(line.substr(start, end - start));
    }
    else if (saveLevel == retSection->size() + 1) {
        retSection->push_back(line.substr(start, end - start));
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
        while (line[i] != '\0' && !s_isComment(line[i])) {
            ++i;
        }
        --i; // revert jump '\0' or ';' or '#'
        while (i > 0 && isspace(line[i])) {
            --i;
        }
        ++i; // last character
        end = i;
        *retComment = line.substr(start, end - start);
    }
    return true;
}

static bool s_parseKey(std::string line, std::list<std::string>* retKey, std::string* retValue,
                       std::string* retComment) {
    char        quote;
    std::size_t start;
    std::size_t end;
    std::size_t last;
    std::size_t i = 0;

    s_stringJumpSpace(line, i);
    if (line[i] == '=') {
        return false;    // not key element
    }
    // start key name
    if (line[i] == '\"' || line[i] == '\'') {
        // get quote character
        quote = line[i];
        ++i; // jump quote
        start = i;
        // search end quote
        while (line[i] != quote) {
            if (line[i] == '\\') {
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
            return false;    // not valid key
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
        while (i > 0 && isspace(line[i])) {
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
                if (line[i] == '\\') {
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
                return false;    // not valid key
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
            while (i > 0 && isspace(line[i])) {
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
            if (line[i] == '\\') {
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
            return false;    // not valid value
        }
    }
    else {
        start = i;
        while (line[i] != '\0' && !s_isComment(line[i])) {
            ++i;
        }
        last = i;
        --i; // revert jump '\0' or ';' or '#'
        while (i > 0 && isspace(line[i])) {
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
        while (line[i] != '\0' && !s_isComment(line[i])) {
            ++i;
        }
        --i; // revert jump '\0' or ';' or '#'
        while (i > 0 && isspace(line[i])) {
            --i;
        }
        ++i; // last character
        end = i;
        *retComment = line.substr(start, end - start);
    }
    return true;
}

static Configator::Map& s_section(Configator::Map& map, const std::list<std::string>& sections) {
    std::list<std::string>::const_iterator itSection;

    Configator::Map* pMap = &map;
    for (itSection = sections.begin(); itSection != sections.end(); ++itSection) {
        pMap = &((*pMap)[*itSection]);
    }
    return *pMap;
}

void Configator::readStream(std::istream& stream) {
    std::list<std::string> sections;
    std::string line    = "";

    while (std::getline(stream, line)) {
        std::list<std::string> keys;
        std::string comment = "";
        std::string value   = "";

        if (s_emptyOrComment(line, &comment)) {
            Configator::Map& map = s_section(_mapConfig, sections);
            if (!map.comment.empty()) {
                map.comment.append("\n");
            }
            map.comment.append(comment);
        }
        else if (s_parseSection(line, &sections, &comment)) {
            Configator::Map& map = s_section(_mapConfig, sections);
            map.comment = comment;
        }
        else if (s_parseKey(line, &keys, &value, &comment)) {
            Configator::Map& map = s_section(_mapConfig, sections);
            Configator::Map* tmpMap = &(map);
            std::list<std::string>::iterator it;

            for (it = keys.begin(); it != keys.end() ; ++it) {
                if (it->empty()) {
                    std::ostringstream oss("");
                    oss << tmpMap->size();
                    tmpMap = &((*tmpMap)[oss.str()]);
                }
                else {
                    tmpMap = &((*tmpMap)[*it]);
                }
            }
            *tmpMap = value;
            tmpMap->comment = comment;
        }
    }
}

}
