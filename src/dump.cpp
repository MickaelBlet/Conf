/**
 * dump.cpp
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

#include <iomanip> // std::setprecision
#include <limits>  // std::numeric_limits

#include "blet/conf.h"

namespace blet {

namespace conf {

// -----------------------------------------------------------------------------
// .................................
// .#####...##..##..##...##..#####..
// .##..##..##..##..###.###..##..##.
// .##..##..##..##..##.#.##..#####..
// .##..##..##..##..##...##..##.....
// .#####....####...##...##..##.....
// .................................
// -----------------------------------------------------------------------------

class Dumper {
  public:
    Dumper(std::ostream& os, std::size_t indent, char indentCharacter) :
        os_(os),
        indent_(indent),
        indentCharacter_(indentCharacter),
        indentIndex_(0) {}

    ~Dumper() {}

    void stringEscape(const std::string& str) {
        for (std::size_t i = 0; i < str.size(); ++i) {
            switch (str[i]) {
                case '\a':
                    os_ << '\\' << 'a';
                    break;
                case '\b':
                    os_ << '\\' << 'b';
                    break;
                case '\f':
                    os_ << '\\' << 'f';
                    break;
                case '\n':
                    os_ << '\\' << 'n';
                    break;
                case '\r':
                    os_ << '\\' << 'r';
                    break;
                case '\t':
                    os_ << '\\' << 't';
                    break;
                case '\v':
                    os_ << '\\' << 'v';
                    break;
                case '\'':
                    os_ << '\\' << '\'';
                    break;
                case '"':
                    os_ << '\\' << '"';
                    break;
                case '\\':
                    os_ << '\\' << '\\';
                    break;
                default:
                    os_ << str[i];
                    break;
            }
        }
    }

    void newlineDump(const blet::Dict& dict) {
        if (indent_ != 0 && ((dict.isArray() && !dict.getValue().getArray().empty()) ||
                             (dict.isObject() && !dict.getValue().getObject().empty()))) {
            os_ << '\n';
        }
    }

    void indentDump() {
        if (indent_ != 0) {
            os_ << std::string(indent_ * indentIndex_, indentCharacter_);
        }
    }

    void nullDump() {
        os_ << "null";
    }

    void numberDump(const blet::Dict& dict) {
        os_ << dict.getValue().getNumber();
    }

    void booleanDump(const blet::Dict& dict) {
        if (dict.getValue().getBoolean()) {
            os_ << "true";
        }
        else {
            os_ << "false";
        }
    }

    void stringDump(const blet::Dict& dict) {
        os_ << '"';
        stringEscape(dict.getValue().getString());
        os_ << '"';
    }

  protected:
    static bool forceKeyString(const std::string& str) {
        bool ret = false;
        if (str.empty()) {
            ret = true;
        }
        else {
            for (std::size_t i = 0; ret == false && i < str.size(); ++i) {
                switch (str[i]) {
                    case ' ':
                    case '=':
                    case ',':
                    case ';':
                    case '#':
                    case '[':
                    case ']':
                    case '{':
                    case '}':
                    case '"':
                    case '\'':
                    case '\a':
                    case '\b':
                    case '\f':
                    case '\r':
                    case '\t':
                    case '\v':
                    case '\\':
                        ret = true;
                        break;
                    default:
                        break;
                }
            }
        }
        return ret;
    }

    std::ostream& os_;
    std::size_t indent_;
    char indentCharacter_;
    std::size_t indentIndex_;
};

// -----------------------------------------------------------------------------
// .........................................................................
// .######...####....####...##..##..........#####...##..##..##...##..#####..
// .....##..##......##..##..###.##..........##..##..##..##..###.###..##..##.
// .....##...####...##..##..##.###..........##..##..##..##..##.#.##..#####..
// .##..##......##..##..##..##..##..........##..##..##..##..##...##..##.....
// ..####....####....####...##..##..........#####....####...##...##..##.....
// .........................................................................
// -----------------------------------------------------------------------------

class JsonDumper : public Dumper {
  public:
    JsonDumper(std::ostream& os, std::size_t indent, char indentCharacter) :
        Dumper(os, indent, indentCharacter) {}

    ~JsonDumper() {}

    void jsonDumpTypeFirst(const blet::Dict& dict) {
        switch (dict.getType()) {
            case blet::Dict::NULL_TYPE:
            case blet::Dict::BOOLEAN_TYPE:
            case blet::Dict::NUMBER_TYPE:
            case blet::Dict::STRING_TYPE:
                jsonDumpType(dict);
                break;
            case blet::Dict::ARRAY_TYPE:
                os_ << "\"\"";
                if (indent_ > 0) {
                    os_ << ' ';
                }
                os_ << '=';
                if (indent_ > 0) {
                    os_ << ' ';
                }
                jsonDumpArray(dict);
                break;
            case blet::Dict::OBJECT_TYPE:
                jsonDumpObjectFirst(dict);
                break;
        }
    }

    void jsonDumpObjectFirst(const blet::Dict& dict) {
        for (blet::Dict::object_t::const_iterator cit = dict.getValue().getObject().begin();
             cit != dict.getValue().getObject().end(); ++cit) {
            if (cit != dict.getValue().getObject().begin()) {
                os_ << '\n';
            }
            // key
            indentDump();
            if (forceKeyString(cit->first)) {
                os_ << '"';
                stringEscape(cit->first);
                os_ << '"';
            }
            else {
                stringEscape(cit->first);
            }
            if (indent_ > 0) {
                os_ << ' ';
            }
            os_ << '=';
            if (indent_ > 0) {
                os_ << ' ';
            }
            jsonDumpType(cit->second);
        }
    }

    void jsonDumpType(const blet::Dict& dict) {
        switch (dict.getType()) {
            case blet::Dict::NULL_TYPE:
                nullDump();
                break;
            case blet::Dict::BOOLEAN_TYPE:
                booleanDump(dict);
                break;
            case blet::Dict::NUMBER_TYPE:
                numberDump(dict);
                break;
            case blet::Dict::STRING_TYPE:
                stringDump(dict);
                break;
            case blet::Dict::ARRAY_TYPE:
                jsonDumpArray(dict);
                break;
            case blet::Dict::OBJECT_TYPE:
                jsonDumpObject(dict);
                break;
        }
    }

    void jsonDumpArray(const blet::Dict& dict) {
        if (dict.getValue().getArray().empty()) {
            os_ << "[]";
        }
        else {
            os_ << '[';
            newlineDump(dict);
            ++indentIndex_;
            for (std::size_t i = 0; i < dict.getValue().getArray().size(); ++i) {
                if (i > 0) {
                    os_ << ',';
                    newlineDump(dict);
                }
                // value
                indentDump();
                jsonDumpType(dict.getValue().getArray()[i]);
            }
            --indentIndex_;
            newlineDump(dict);
            indentDump();
            os_ << ']';
        }
    }

    void jsonDumpObject(const blet::Dict& dict) {
        if (dict.getValue().getObject().empty()) {
            os_ << "{}";
        }
        else {
            os_ << '{';
            newlineDump(dict);
            ++indentIndex_;
            for (blet::Dict::object_t::const_iterator cit = dict.getValue().getObject().begin();
                 cit != dict.getValue().getObject().end(); ++cit) {
                if (cit != dict.getValue().getObject().begin()) {
                    os_ << ',';
                    newlineDump(dict);
                }
                // key
                indentDump();
                if (forceKeyString(cit->first)) {
                    os_ << '"';
                    stringEscape(cit->first);
                    os_ << '"';
                }
                else {
                    stringEscape(cit->first);
                }
                if (indent_ > 0) {
                    os_ << ' ';
                }
                os_ << '=';
                if (indent_ > 0) {
                    os_ << ' ';
                }
                jsonDumpType(cit->second);
            }
            --indentIndex_;
            newlineDump(dict);
            indentDump();
            os_ << '}';
        }
    }
};

// -----------------------------------------------------------------------------
// .........................................................................
// ..####....####...##..##..######..........#####...##..##..##...##..#####..
// .##..##..##..##..###.##..##..............##..##..##..##..###.###..##..##.
// .##......##..##..##.###..####............##..##..##..##..##.#.##..#####..
// .##..##..##..##..##..##..##..............##..##..##..##..##...##..##.....
// ..####....####...##..##..##..............#####....####...##...##..##.....
// .........................................................................
// -----------------------------------------------------------------------------

class ConfDumper : public JsonDumper {
  public:
    ConfDumper(std::ostream& os, std::size_t indent, char indentCharacter) :
        JsonDumper(os, indent, indentCharacter),
        sectionIndex_(0) {}

    ~ConfDumper() {}

    void confDumpType(const blet::Dict& dict) {
        switch (dict.getType()) {
            case blet::Dict::NULL_TYPE:
                nullDump();
                break;
            case blet::Dict::BOOLEAN_TYPE:
                booleanDump(dict);
                break;
            case blet::Dict::NUMBER_TYPE:
                numberDump(dict);
                break;
            case blet::Dict::STRING_TYPE:
                stringDump(dict);
                break;
            case blet::Dict::ARRAY_TYPE:
                confDumpArray("", dict);
                break;
            case blet::Dict::OBJECT_TYPE:
                confDumpObject(dict);
                break;
        }
    }

    void confDumpArray(const std::string& key, const blet::Dict& dict) {
        if (dict.getValue().getArray().empty()) {
            // key
            indentDump();
            if (forceKeyString(key)) {
                os_ << '"';
                stringEscape(key);
                os_ << '"';
            }
            else {
                stringEscape(key);
            }
            // value operator
            if (indent_ > 0) {
                os_ << ' ';
            }
            os_ << '=';
            if (indent_ > 0) {
                os_ << ' ';
            }
            os_ << "[]";
        }
        else {
            for (std::size_t i = 0; i < dict.getValue().getArray().size(); ++i) {
                if (i > 0) {
                    os_ << '\n';
                }
                // key
                indentDump();
                if (forceKeyString(key)) {
                    os_ << '"';
                    stringEscape(key);
                    os_ << '"';
                }
                else {
                    stringEscape(key);
                }
                // array operator
                os_ << "[]";
                // value operator
                if (indent_ > 0) {
                    os_ << ' ';
                }
                os_ << '=';
                if (indent_ > 0) {
                    os_ << ' ';
                }
                // value
                switch (dict.getValue().getArray()[i].getType()) {
                    case blet::Dict::NULL_TYPE:
                    case blet::Dict::BOOLEAN_TYPE:
                    case blet::Dict::NUMBER_TYPE:
                    case blet::Dict::STRING_TYPE:
                        confDumpType(dict.getValue().getArray()[i]);
                        break;
                    case blet::Dict::ARRAY_TYPE:
                        jsonDumpArray(dict.getValue().getArray()[i]);
                        break;
                    case blet::Dict::OBJECT_TYPE:
                        jsonDumpObject(dict.getValue().getArray()[i]);
                        break;
                }
            }
        }
    }

    void confDumpObject(const blet::Dict& dict) {
        int index = 0;
        ++sectionIndex_;
        for (blet::Dict::object_t::const_iterator cit = dict.getValue().getObject().begin();
             cit != dict.getValue().getObject().end(); ++cit) {
            switch (cit->second.getType()) {
                case blet::Dict::NULL_TYPE:
                case blet::Dict::BOOLEAN_TYPE:
                case blet::Dict::NUMBER_TYPE:
                case blet::Dict::STRING_TYPE:
                    if (index > 0) {
                        os_ << '\n';
                    }
                    ++index;
                    indentDump();
                    if (forceKeyString(cit->first)) {
                        os_ << '"';
                        stringEscape(cit->first);
                        os_ << '"';
                    }
                    else {
                        stringEscape(cit->first);
                    }
                    if (indent_ > 0) {
                        os_ << ' ';
                    }
                    os_ << '=';
                    if (indent_ > 0) {
                        os_ << ' ';
                    }
                    confDumpType(cit->second);
                    break;
                case blet::Dict::ARRAY_TYPE:
                    if (index > 0) {
                        os_ << '\n';
                    }
                    ++index;
                    confDumpArray(cit->first, cit->second);
                    break;
                case blet::Dict::OBJECT_TYPE:
                    break;
            }
        }
        for (blet::Dict::object_t::const_iterator cit = dict.getValue().getObject().begin();
             cit != dict.getValue().getObject().end(); ++cit) {
            if (cit->second.getType() == blet::Dict::OBJECT_TYPE) {
                if (index > 0) {
                    os_ << '\n';
                }
                ++index;
                if (cit->second.getValue().getObject().empty()) {
                    if (forceKeyString(cit->first)) {
                        os_ << '"';
                        stringEscape(cit->first);
                        os_ << '"';
                    }
                    else {
                        stringEscape(cit->first);
                    }
                    if (indent_ > 0) {
                        os_ << ' ';
                    }
                    os_ << '=';
                    if (indent_ > 0) {
                        os_ << ' ';
                    }
                    os_ << "{}";
                }
                else {
                    // new section
                    os_ << std::string(sectionIndex_, '[');
                    if (forceKeyString(cit->first)) {
                        os_ << '"';
                        stringEscape(cit->first);
                        os_ << '"';
                    }
                    else {
                        stringEscape(cit->first);
                    }
                    os_ << std::string(sectionIndex_, ']');
                    os_ << '\n';
                    confDumpType(cit->second);
                }
            }
        }
        --sectionIndex_;
    }

  private:
    std::size_t sectionIndex_;
};

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

void dump(const blet::Dict& dict, std::ostream& os, std::size_t indent, char indentCharacter, enum EDumpStyle style) {
    os << std::setprecision(std::numeric_limits<double>::digits10 + 1);
    switch (style) {
        case CONF_STYLE: {
            ConfDumper conf(os, indent, indentCharacter);
            conf.confDumpType(dict);
            break;
        }
        case JSON_STYLE: {
            JsonDumper conf(os, indent, indentCharacter);
            conf.jsonDumpTypeFirst(dict);
            break;
        }
    }
}

std::string dump(const blet::Dict& dict, std::size_t indent, char indentCharacter, enum EDumpStyle style) {
    std::ostringstream oss("");
    dump(dict, oss, indent, indentCharacter, style);
    return oss.str();
}

} // namespace conf

} // namespace blet
