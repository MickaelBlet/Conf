/**
 * configator.hpp
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

#ifndef _MBLET_CONFIGATOR_HPP_
# define _MBLET_CONFIGATOR_HPP_

# include <iostream>
# include <sstream>
# include <fstream>
# include <vector>

# include <stdlib.h>

namespace mblet {

/**
 * @brief load .ini file with table index
 */
class Configator {

  public:
    /**
     * @brief Container map by index
     */
    class Map : public std::vector<std::pair<std::string, Map> > {

      public:
        /**
         * @brief Map exception
         */
        class Exception : public std::exception {

          public:
            /**
             * @brief Construct a new Exception object
             *
             * @param str : exception message
             */
            Exception(const char* str) throw();

            /**
             * @brief Destroy the Exception object
             */
            virtual ~Exception() throw();

            /**
             * @brief get the exception message
             *
             * @return const char* : exception message
             */
            virtual const char* what() const throw();

          private:
            std::string _str;

        };

        /**
         * @brief overide operator stream for print value
         *
         * @param os
         * @param map
         * @return std::ostream&
         */
        friend std::ostream& operator<<(std::ostream& os, const Map& map);

        /**
         * @brief Construct a new Map object
         */
        Map();

        /**
         * @brief overide operator for set value from std::string
         *
         * @param str : new value
         */
        void operator=(const std::string& str);

        /**
         * @brief overide operator for get map from index
         *
         * @param index : at index
         * @return Map& : map from index
         */
        Map& operator[](std::size_t index);

        /**
         * @brief overide operator for get const map from index
         *
         * @param index : at index
         * @return const Map& : map from index
         */
        const Map& operator[](std::size_t index) const;

        /**
         * @brief overide operator for get map from string
         *
         * @param str : at string
         * @return Map& : map from string
         */
        Map& operator[](const std::string& str);

        /**
         * @brief overide operator for get const map from string
         *
         * @param str : search string
         * @return const Map& : map from string
         */
        const Map& operator[](const std::string& str) const;

        /**
         * @brief find from string
         *
         * @param str : search string
         * @return Map::iterator : read/write iterator at find element
         */
        Map::iterator find(const std::string& str);

        /**
         * @brief find from string
         *
         * @param str : search string
         * @return Map::const_iterator : read-only iterator at find element
         */
        Map::const_iterator find(const std::string& str) const;

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

        void valueToStream(std::ostream& stringStream) const;

    };

    /**
    * @brief Map exception
    */
    class Exception : public std::exception {

      public:
        /**
         * @brief Construct a new Exception object
         *
         * @param str : exception message
         */
        Exception(const char* str) throw();

        /**
         * @brief Destroy the Exception object
         */
        virtual ~Exception() throw();

        /**
         * @brief get the exception message
         *
         * @return const char* : exception message
         */
        virtual const char* what() const throw();

      private:
        std::string _str;

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
    Configator(const std::string& filename);

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
    bool readFile(const std::string& filename);

    /**
     * @brief Get the Filename object
     *
     * @return std::string : copy of filename
     */
    const std::string& getFilename() const;

    /**
     * @brief Check if file is read
     *
     * @return true : file is read
     * @return false : file is not read
     */
    bool isRead() const;

    /**
     * @brief Set the Config object
     *
     * @param mapConfig
     */
    void setConfig(const Map& mapConfig);

    /**
     * @brief Get the Config object
     *
     * @return const Map& : all mac config
     */
    const Map& getConfig() const;

    /**
     * @brief overide operator for get map from index
     *
     * @param index : at index
     * @return Map& : map from index
     */
    Map& operator[](std::size_t index);

    /**
     * @brief overide operator for get map from string
     *
     * @param str : at str
     * @return Map& : map from string
     */
    Map& operator[](const std::string& str);

    /**
     * @brief overide operator for get const map from index
     *
     * @param index : at index
     * @return Map& : map from index
     */
    const Map& operator[](std::size_t index) const;

    /**
     * @brief overide operator for get const map from string
     *
     * @param str : at str
     * @return Map& : map from string
     */
    const Map& operator[](const std::string& str) const;

    /**
     * @brief dump in string the map
     *
     * @param indent
     * @return std::string
     */
    std::ostream& dump(std::ostream& oss = std::cout, std::size_t indent = 4) const;

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
    void readStream(std::istream& stream);

}; // class Configator

} // namespace Config

#endif // _MBLET_CONFIGATOR_HPP_