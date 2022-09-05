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
            os << map._string;
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
            _isExist = true;
            _string = str;
            stringToType();
        }

        /**
         * @brief overide operator for get map from index
         *
         * @param index : at index
         * @return Map& : map from index
         */
        template<typename T>
        inline Map& operator[](const T& index) {
            char str[32];
            snprintf(str, sizeof(str), "%lu", static_cast<unsigned long>(index));
            return operator[](str);
        }

        /**
         * @brief overide operator for get const map from index
         *
         * @param index : at index
         * @return const Map& : map from index
         */
        template<typename T>
        inline const Map& operator[](const T& index) const {
            char str[32];
            snprintf(str, sizeof(str), "%lu", static_cast<unsigned long>(index));
            return operator[](str);
        }

        /**
         * @brief overide operator for get map from string
         *
         * @param str : at string
         * @return Map& : map from string
         */
        inline Map& operator[](const std::string& str) {
            Configator::Map::iterator it = find(str);
            if (it != end()) {
                return it->second;
            }
            insert(std::pair<std::string, Map>(str, Map()));
            return at(str);
        }

        /**
         * @brief overide operator for get const map from string
         *
         * @param str : search string
         * @return const Map& : map from string
         */
        inline const Map& operator[](const std::string& str) const {
            static Map emptyMap;

            Map::const_iterator it = find(str);
            if (it != end()) {
                return it->second;
            }
            return emptyMap;
        }

        Map& operator[](const char* str) {
            return operator[](std::string(str));
        }

        const Map& operator[](const char* str) const {
            return operator[](std::string(str));
        }

        template<size_t Size>
        Map& operator[](const char (&str)[Size]) {
            return operator[](std::string(str));
        }

        template<size_t Size>
        const Map& operator[](const char (&str)[Size]) const {
            return operator[](std::string(str));
        }

        const std::string& getComment() const {
            return _comment;
        }

        const std::string& getComment(const std::string& defaultValue) const {
            if (_isExist) {
                return _comment;
            }
            else {
                return defaultValue;
            }
        }

        const std::string& getString() const {
            return _string;
        }

        const std::string& getString(const std::string& defaultValue) const {
            if (_isExist) {
                return _string;
            }
            else {
                return defaultValue;
            }
        }

        const double& getNumber() const {
            return _number;
        }

        const double& getNumber(const double& defaultValue) const {
            if (_isExist) {
                return _number;
            }
            else {
                return defaultValue;
            }
        }

        const bool& getBoolean() const {
            return _boolean;
        }

        const bool& getBoolean(const bool& defaultValue) const {
            if (_isExist) {
                return _boolean;
            }
            else {
                return defaultValue;
            }
        }

        const bool& isExist() const {
            return _isExist;
        }

        template<typename T>
        T get() const {
            T ret;
            std::stringstream ss("");
            if (_isBoolean) {
                ss << _boolean;
            }
            else if (_isNumber) {
                ss << _number;
            }
            else {
                ss << _string;
            }
            ss >> ret;
            return ret;
        }

        template<typename T, typename U>
        T get(const U& defaultValue) const {
            T ret;
            std::stringstream ss("");

            if (!_isExist) {
                ss << defaultValue;
            }
            else {
                if (_isBoolean) {
                    ss << _boolean;
                }
                else if (_isNumber) {
                    ss << _number;
                }
                else {
                    ss << _string;
                }
            }
            ss >> ret;
            return ret;
        }

        template<typename T>
        void get(T& ret) const {
            ret = get<T>();
        }

        operator const std::string&() const {
            return getString();
        }

        operator const char*() const {
            return getString().c_str();
        }

        operator const bool&() const {
            return getBoolean();
        }

        template<typename T>
        operator T() const {
            return getNumber();
        }

      private:

        friend class Configator;

        void stringToType();

        std::string _comment;
        std::string _string;
        double _number;
        bool _boolean;
        bool _isExist;
        bool _isNumber;
        bool _isBoolean;

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

    /**
     * @brief read stream and load its config
     *
     * @param stream
     */
    inline void readStream(std::istream& stream) {
        parseStream(stream);
    }

    /**
     * @brief read string and load its config
     *
     * @param str
     */
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
     * @return const Map& : config object
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
     * @brief dump in stream
     *
     * @param oss
     * @return std::ostream&
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

} // namespace mblet

#endif // _MBLET_CONFIGATOR_HPP_