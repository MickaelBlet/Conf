/**
 * conf.h
 *
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * Copyright (c) 2023 BLET Mickaël.
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

#ifndef _BLET_CONF_H_
#define _BLET_CONF_H_

#include <exception> // std::exception
#include <sstream>   // std::istream, std::ostream
#include <string>    // std::string

#include "blet/dict.h"

namespace blet {

namespace conf {

/**
 * @brief Parse exception from std::exception
 */
class LoadException : public std::exception {
  public:
    LoadException(const std::string& filename, const std::string& message);
    LoadException(const std::string& filename, std::size_t line, std::size_t column, const std::string& message);
    virtual ~LoadException() throw();
    const char* what() const throw();
    const std::string& filename() const throw();
    const std::string& message() const throw();
    const std::size_t& line() const throw();
    const std::size_t& column() const throw();

  protected:
    std::string _what;
    std::string _filename;
    std::string _message;
    std::size_t _line;
    std::size_t _column;
};

enum EDumpStyle {
    CONF_STYLE = 0,
    JSON_STYLE
};

/**
 * @brief Dump dict in config format.
 *
 * @param dict A dict.
 * @param os A ostream.
 * @param indent Indentation of dump.
 * @param indentCharacter Indentation character.
 */
void dump(const blet::Dict& dict, std::ostream& os, std::size_t indent = 0, char indentCharacter = ' ',
          enum EDumpStyle style = CONF_STYLE);

/**
 * @brief Dump dict in config format.
 *
 * @param dict A dict.
 * @param indent Indentation of dump.
 * @param indentCharacter Indentation character.
 * @return std::string config string.
 */
std::string dump(const blet::Dict& dict, std::size_t indent = 0, char indentCharacter = ' ',
                 enum EDumpStyle style = CONF_STYLE);

/**
 * @brief Parse and load a config from filename.
 *
 * @param filename A filename.
 * @param comment Option for accept the comment (style C/C++) in config.
 * @param additionalNext Option for accept the bad commat at end of config
 * object.
 * @return blet::Dict Dictionnary of config.
 */
blet::Dict loadFile(const char* filename);

/**
 * @brief Parse and load a config from stream.
 *
 * @param stream A stream.
 * @param comment Option for accept the comment (style C/C++) in config.
 * @param additionalNext Option for accept the bad commat at end of config
 * object.
 * @return blet::Dict Dictionnary of config.
 */
blet::Dict loadStream(std::istream& stream);

/**
 * @brief Parse and load a config from string.
 *
 * @param str A string.
 * @param comment Option for accept the comment (style C/C++) in config.
 * @param additionalNext Option for accept the bad commat at end of config
 * object.
 * @return blet::Dict Dictionnary of config.
 */
blet::Dict loadString(const std::string& str);

/**
 * @brief Parse and load a config from data.
 *
 * @param data A data.
 * @param size Size of data.
 * @param comment Option for accept the comment (style C/C++) in config.
 * @param additionalNext Option for accept the bad commat at end of config
 * object.
 * @return blet::Dict Dictionnary of config.
 */
blet::Dict loadData(const void* data, std::size_t size);

} // namespace conf

} // namespace blet

#endif // #ifndef _BLET_config_H_