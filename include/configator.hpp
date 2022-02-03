/**
 * configator.hpp
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

#ifndef _MBLET_CONFIGATOR_HPP_
# define _MBLET_CONFIGATOR_HPP_

# include <iostream>
# include <sstream>
# include <fstream>
# include <map>

# include <cstdio>
# include <cstdlib>

namespace mblet {

/**
 * @brief load .ini file with table index
 */
class Configator {

  public:
    /**
     * @brief Container map by index
     */
    class Map : public std::map<std::string, Map> {

      public:
        /**
         * @brief overide operator stream for print value
         *
         * @param os
         * @param map
         * @return std::ostream&
         */
        inline friend std::ostream& operator<<(std::ostream& os, const Map& map) {
            os << map.value;
            return os;
        }

        /**
         * @brief Construct a new Map object
         */
        Map();

        /**
         * @brief overide operator for set value from std::string
         *
         * @param str : new value
         */
        inline void operator=(const std::string& str) {
            value = str;
        }

        /**
         * @brief overide operator for get map from index
         *
         * @param index : at index
         * @return Map& : map from index
         */
        inline Map& operator[](unsigned long index) {
            char str[64];
            snprintf(str, sizeof(str), "%lu", index);
            return this->operator[](str);
        }

        /**
         * @brief overide operator for get const map from index
         *
         * @param index : at index
         * @return const Map& : map from index
         */
        inline const Map& operator[](unsigned long index) const {
            char str[64];
            snprintf(str, sizeof(str), "%lu", index);
            return this->operator[](str);
        }

        /**
         * @brief overide operator for get map from string
         *
         * @param str : at string
         * @return Map& : map from string
         */
        inline Map& operator[](const std::string& str) {
            Configator::Map::iterator it = this->find(str);
            if (it != this->end()) {
                return it->second;
            }
            this->insert(std::pair<std::string, Map>(str, Map()));
            return this->at(str);
        }

        /**
         * @brief overide operator for get const map from string
         *
         * @param str : search string
         * @return const Map& : map from string
         */
        inline const Map& operator[](const std::string& str) const {
            Map::const_iterator it = find(str);
            if (it != this->end()) {
                return it->second;
            }
            return _emptyMap;
        }

        /**
         * @brief get value
         * convert octal or hex value
         * bool == "true"||"false", "on"||"off"
         *
         * @tparam T : type of return (from string)
         * @return T : convert value
         */
        template<typename T>
        T get() const {
            T retValue;
            std::stringstream stringStream("");
            valueToStream(stringStream);
            stringStream >> retValue;
            return retValue;
        }

        /**
         * @brief get value with default value
         * convert octal or hex value
         * bool == "true"||"false", "on"||"off"
         *
         * @tparam T : type of return (from string)
         * @return T : convert value
         */
        template<typename T, typename U>
        T get(const U& defaultValue) const {
            T retValue;
            std::stringstream stringStream("");

            if (value.empty()) {
                stringStream << defaultValue;
            }
            else {
                valueToStream(stringStream);
            }
            stringStream >> retValue;
            return retValue;
        }

        std::string comment;
        std::string value;

      private:
        static Map _emptyMap;

        void valueToStream(std::ostream& stringStream) const;

    };

    /**
     * @brief Construct a new Configator object
     */
    Configator();

    /**
     * @brief Construct a new Configator object
     *
     * @param filename
     */
    Configator(const char* filename);

    /**
     * @brief Construct a new Configator object
     *
     * @param src
     */
    Configator(const Configator& src);

    /**
     * @brief Destroy the Configator object
     */
    ~Configator();

    /**
     * @brief Operator = overide
     *
     * @param rhs
     * @return Configator&
     */
    Configator& operator=(const Configator& rhs);

    /**
     * @brief read file and load its config
     *
     * @param filename
     * @return true
     * @return false
     */
    bool readFile(const char* filename);

    inline void readStream(std::istream& stream) {
        parseStream(stream);
    }

    inline void readString(const std::string& str) {
        std::istringstream istr(str);
        parseStream(istr);
    }

    /**
     * @brief Get the Filename object
     *
     * @return std::string : copy of filename
     */
    inline const std::string& getFilename() const {
        return _filename;
    }

    /**
     * @brief Check if file is read
     *
     * @return true : file is read
     * @return false : file is not read
     */
    inline bool isRead() const {
        return _isRead;
    }

    /**
     * @brief Set the Config object
     *
     * @param mapConfig
     */
    inline void setConfig(const Map& mapConfig) {
        _mapConfig.clear();
        _mapConfig = mapConfig;
    }

    /**
     * @brief Get the Config object
     *
     * @return const Map& : all mac config
     */
    inline const Map& getConfig() const {
        return _mapConfig;
    }

    /**
     * @brief overide operator for get const map from index
     *
     * @param index : at index
     * @return Map& : map from index
     */
    inline const Map& operator[](std::size_t index) const {
        return _mapConfig[index];
    }

    /**
     * @brief overide operator for get const map from string
     *
     * @param str : at str
     * @return Map& : map from string
     */
    inline const Map& operator[](const std::string& str) const {
        return _mapConfig[str];
    }

    /**
     * @brief dump in string the map
     *
     * @param indent
     * @return std::string
     */
    std::ostream& dump(std::ostream& oss = std::cout) const;

  protected:
    Map         _mapConfig;
    std::string _filename;
    bool        _isRead;

  private:
    /**
     * @brief parse and fill the map from stream
     *
     * @param stream
     */
    void parseStream(std::istream& stream);

}; // class Configator

} // namespace Config

#endif // _MBLET_CONFIGATOR_HPP_