/*
 * @file Configator.hpp
 * @author Mickaël BLET
 */

#ifndef _CONFIG_CONFIGATOR_HPP_
# define _CONFIG_CONFIGATOR_HPP_

# include <sstream>
# include <fstream>
# include <iostream>
# include <iomanip> // setbase
# include <vector>

namespace mblet
{

/**
 * @brief load .ini file with table index
 *
 */
class Configator
{

public:

    /**
     * @brief Container map by index
     *
     */
    struct Map : public std::vector<std::pair<std::string, Map> >
    {
        /**
         * @brief overide operator for set value from std::string
         *
         * @param str : new value
         */
        void operator=(const std::string &str)
        {
            this->value = str;
        }

        /**
         * @brief overide operator for get map from index
         *
         * @param index : at index
         * @return Map& : map from index
         */
        Map &operator[](std::size_t index)
        {
            return this->at(index).second;
        }

        /**
         * @brief overide operator for get const map from index
         *
         * @param index : at index
         * @return const Map& : map from index
         */
        const Map &operator[](std::size_t index) const
        {
            return this->at(index).second;
        }

        /**
         * @brief overide operator for get map from string
         *
         * @param str : at string
         * @return Map& : map from string
         */
        Map &operator[](const std::string &str)
        {
            Map::iterator it = find(str);
            if (it != this->end())
                return it->second;
            this->push_back(std::pair<std::string, Map>(str, Map()));
            return this->back().second;
        }

        /**
         * @brief overide operator for get const map from string
         *
         * @param str : search string
         * @return const Map& : map from string
         */
        const Map &operator[](const std::string &str) const
        {
            Map::const_iterator it = find(str);
            if (it != this->end())
                return it->second;
            throw std::out_of_range("");
        }

        /**
         * @brief overide operator stream for print value
         *
         * @param os
         * @param map
         * @return std::ostream&
         */
        friend std::ostream &operator<<(std::ostream &os, const Map &map)
        {
            os << map.value;
            return os;
        }

        /**
         * @brief find from string
         *
         * @param str : search string
         * @return Map::iterator : read/write iterator at find element
         */
        Map::iterator find(const std::string &str)
        {
            for (Map::iterator it = this->begin() ; it != this->end() ; ++it)
            {
                if (str.size() != it->first.size())
                    continue;
                std::size_t i = 0;
                for (i = 0 ; i < str.size() && std::tolower(str[i]) == std::tolower(it->first[i]); ++i);
                if (i == str.size())
                    return it;
            }
            return this->end();
        }

        /**
         * @brief find from string
         *
         * @param str : search string
         * @return Map::const_iterator : read-only iterator at find element
         */
        Map::const_iterator find(const std::string &str) const
        {
            for (Map::const_iterator it = this->begin() ; it != this->end() ; ++it)
            {
                if (str == it->first)
                    return it;
            }
            return this->end();
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
        T get(void) const
        {
            T retValue;
            std::stringstream stringStream;
            if (value == "true" || value == "on")
            {
                stringStream << true;
                stringStream >> retValue;
                return retValue;
            }
            if (value == "false" || value == "off")
            {
                stringStream << false;
                stringStream >> retValue;
                return retValue;
            }
            int index = 0;
            if (value[index] == '-' || value[index] == '+')
            {
                index++;
            }
            if (value[index] == '0' && value[index + 1] == 'x')
            {
                long long int tmp;
                stringStream << value;
                stringStream >> std::setbase(16) >> tmp;
                stringStream.str("");
                stringStream.clear();
                stringStream << std::setbase(10) << tmp;
                stringStream >> retValue;
            }
            else if (value[index] == '0' && value.find('.') == std::string::npos)
            {
                long long int tmp;
                stringStream << value;
                stringStream >> std::setbase(8) >> tmp;
                stringStream.str("");
                stringStream.clear();
                stringStream << std::setbase(10) << tmp;
                stringStream >> retValue;
            }
            else
            {
                stringStream << value;
                stringStream >> retValue;
            }
            return retValue;
        }

        std::string value;
    };


    /**
     * @brief Construct a new Configator object
     *
     */
    Configator(void):
    _isRead(false)
    {
        return ;
    }

    /**
     * @brief Construct a new Configator object
     *
     * @param filename
     */
    Configator(const std::string &filename):
    _isRead(false)
    {
        readFile(filename);
        return ;
    }

    /**
     * @brief Construct a new Configator object
     *
     * @param src
     */
    Configator(const Configator &src):
    _mapConfig(src._mapConfig),
    _filename(src._filename),
    _isRead(src._isRead)
    {
        return ;
    }

    /**
     * @brief Operator = overide
     *
     * @param rhs
     * @return Configator&
     */
    Configator &operator=(const Configator &rhs)
    {
        if (this == &rhs)
            return *this;
        this->_mapConfig = rhs._mapConfig;
        this->_filename = rhs._filename;
        this->_isRead = rhs._isRead;
        return *this;
    }

    /**
     * @brief Destroy the Configator object
     *
     */
    ~Configator(void)
    {
        return ;
    }

    /**
     * @brief read file and load its config
     *
     * @param filename
     * @return true
     * @return false
     */
    bool readFile(const std::string& filename)
    {
        _mapConfig.clear();
        _filename = "";
        _isRead = false;

        std::ifstream fileStream(filename.c_str()); // open file
        if (fileStream.is_open() == false)
            return false;

        _filename = filename;
        _isRead = true;
        readStream(fileStream); // parse file
        fileStream.close();
        return true;
    }

    /**
     * @brief Get the Filename object
     *
     * @return std::string : copy of filename
     */
    std::string getFilename(void) const
    {
        return _filename;
    }

    /**
     * @brief Check if file is read
     *
     * @return true : file is read
     * @return false : file is not read
     */
    bool isRead(void) const
    {
        return _isRead;
    }

    /**
     * @brief Set the Config object
     *
     * @param mapConfig
     */
    void setConfig(const Map &mapConfig)
    {
        _mapConfig.clear();
        _mapConfig = mapConfig;
    }

    /**
     * @brief overide operator for get map from index
     *
     * @param index : at index
     * @return Map& : map from index
     */
    Map &operator[](std::size_t index)
    {
        return _mapConfig[index];
    }

    /**
     * @brief overide operator for get map from string
     *
     * @param str : at str
     * @return Map& : map from string
     */
    Map &operator[](const std::string &str)
    {
        return _mapConfig[str];
    }

    /**
     * @brief dump in string the map
     *
     * @param indent
     * @return std::string
     */
    std::string dump(int indent = 4)
    {
        std::ostringstream oss;
        recurseDump(oss, _mapConfig, indent);
        return oss.str();
    }

private:

    /**
     * @brief call by dump method
     *
     * @param oss
     * @param map
     * @param indent
     * @param tab
     */
    static void recurseDump(std::ostringstream &oss, const Map &map, int indent, int tab = 0)
    {
        for (Map::const_iterator itSection = map.begin();
             itSection != map.end();
             ++itSection)
        {
            oss << std::string(tab * indent, ' ') << itSection->first << ':';
            if (itSection->second.size() > 0)
            {
                oss << std::endl;
                recurseDump(oss, itSection->second, indent, tab + 1);
            }
            else
            {
                oss << ' ' << itSection->second.value << std::endl;
            }
        }
    }

    /**
     * @brief check if character is comment
     *
     * @param c
     * @return true : c is comment character
     * @return false : c is not comment character
     */
    static bool isComment(const char &c)
    {
        if (c != ';' && c != '#')
            return false;
        return true;
    }

    /**
     * @brief parse key of key
     *
     * @param line
     * @param retKeyOfKey
     * @return true
     * @return false
     */
    static bool parseKeys(std::string &key, std::vector<std::string> &retKeys)
    {
        std::size_t endKey;
        std::size_t startKey;
        std::size_t i = 0;
        char        quote;

        // check table end
        if (key[key.size() - 1] != ']')
        {
            retKeys.push_back(key);
            return true;
        }
        // check table start
        for (i = key.size() - 1 ; key[i] != '[' ; --i)
        {
            if (i <= 0)
                return false;
        }
        for (i = 0 ; key[i] != '[' ; ++i);
        retKeys.push_back(key.substr(0, i));
        for ( ; i < key.size() ; ++i)
        {
            for ( ; isspace(key[i]) ; ++i);
            if (key[i] != '[')
                return false;
            ++i;
            for ( ; isspace(key[i]) ; ++i);
            if (key[i] == '\"' || key[i] == '\'')
            {
                quote = key[i];
                ++i; // jump quote
                startKey = i;
                for ( ; key[i] != quote; ++i)
                {
                    if (key[i] == '\\')
                        key.erase(i, 1);
                    if (key[i] == '\0')
                        return false; // end quote not found
                }
                endKey = i;
                ++i;
                for ( ; isspace(key[i]) ; ++i);
                if (key[i] != ']')
                    return false;
                retKeys.push_back(key.substr(startKey, endKey - startKey));
            }
            else
            {
                startKey = i;
                for ( ; key[i] != ']' ; ++i)
                {
                    if (key[i] == '\0')
                        return false; // ']' not found
                }
                for (--i ; isspace(key[i]) ; --i);
                ++i;
                endKey = i;
                retKeys.push_back(key.substr(startKey, endKey - startKey));
            }
            for ( ; key[i] != ']' ; ++i);
        }
        return true;
    }

    /**
     * @brief parse key and value
     *
     * @param line
     * @param retKey : return key
     * @param retValue : return value
     * @return true : if line is a key value
     * @return false : if line is not a key value
     */
    static bool parseKey(std::string line, const std::string &section, Map *retMap)
    {
        std::size_t startKey;
        std::size_t endKey;
        std::vector<std::string> keys;
        std::size_t startValue;
        std::size_t endValue;
        std::size_t i = 0;
        char        quote;

        for ( ; isspace(line[i]) ; ++i);
        if (line[i] == '=')
            return false; // first element not found
        if (line[i] == '\"' || line[i] == '\'')
        {
            quote = line[i];
            ++i; // jump quote
            startKey = i;
            for ( ; line[i] != quote; ++i)
            {
                if (line[i] == '\\')
                    line.erase(i, 1);
                if (line[i] == '\0')
                    return false; // end quote not found
            }
            endKey = i;
            ++i;
            keys.push_back(line.substr(startKey, endKey - startKey));
        }
        else
        {
            startKey = i;
            for ( ; line[i] != '=' ; ++i)
            {
                if (line[i] == '\0')
                    return false; // '=' not found
            }
            for (--i ; isspace(line[i]) ; --i);
            ++i;
            endKey = i;
            std::string key = line.substr(startKey, endKey - startKey);
            // search table key
            if (parseKeys(key, keys) == false)
                return false;
        }
        for ( ; isspace(line[i]) ; ++i);
        if (line[i] != '=')
            return false; // '=' not found
        for (++i ; isspace(line[i]) ; ++i);
        if (line[i] == '\"' || line[i] == '\'')
        {
            quote = line[i];
            ++i;
            startValue = i;
            for ( ; line[i] != quote ; ++i)
            {
                if (line[i] == '\\')
                    line.erase(i, 1);
                if (line[i] == '\0')
                    return false; // end quote not found
            }
            endValue = i;
            ++i;
        }
        else
        {
            startValue = i;
            for ( ; isComment(line[i]) != true && line[i] != '\0' ; ++i);
            for (--i ; isspace(line[i]) ; --i);
            ++i;
            endValue = i;
        }
        for ( ; isspace(line[i]) ; ++i);
        if (line[i] != '\0' && !isComment(line[i]))
            return false;
        Map *tmpMap = &((*retMap)[section]);
        for (std::size_t j = 0 ; j < keys.size() ; ++j)
        {
            if (keys[j].size() == 0)
            {
                std::ostringstream oss;
                oss << tmpMap->size();
                tmpMap = &((*tmpMap)[oss.str()]);
            }
            else
            {
                tmpMap = &((*tmpMap)[keys[j]]);
            }
        }
        *tmpMap = line.substr(startValue, endValue - startValue);
        return true;
    }

    /**
     * @brief parse and get section name
     *
     * @param line
     * @param retSection : return name of section if found
     * @return true : if section found
     * @return false : if section not found
     */
    static bool parseSection(const std::string &line, std::string *retSection)
    {
        std::size_t start;
        std::size_t end;
        std::size_t i = 0;

        for ( ; isspace(line[i]) ; ++i);
        if (line[i] != '[')
            return false;
        for (++i ; isspace(line[i]) ; ++i);
        start = i;
        for ( ; line[i] != ']' ; ++i)
        {
            if (line[i] == '\0')
                return false;
        }
        for (--i ; isspace(line[i]) ; --i);
            ++i;
        end = i;
        for ( ; line[i] != ']' ; ++i);
        for (++i ; isspace(line[i]) ; ++i);
        if (line[i] != '\0' && !isComment(line[i]))
            return false;
        *retSection = line.substr(start, end - start);
        return true;
    }

    /**
     * @brief detect if line is empty or comment
     *
     * @param line
     * @return true : line is empty or comment
     * @return false : line is not empty or comment
     */
    static bool emptyOrComment(const std::string &line)
    {
        std::size_t i = 0;

        for ( ; isspace(line[i]) ; ++i);
        if (line[i] != '\0' && !isComment(line[i]))
            return false;
        return true;
    }

    /**
     * @brief parse and fill the map from fileStream
     *
     * @param fileStream
     */
    void readStream(std::istream &fileStream)
    {
        std::string currentSectionName = "";

        std::string line;
        while(std::getline(fileStream, line))
        {
            if (emptyOrComment(line))
            {
                continue;
            }
            if (parseSection(line, &currentSectionName))
            {
                continue;
            }
            if (parseKey(line, currentSectionName, &_mapConfig))
            {
                continue;
            }
        }
    }

    Map                 _mapConfig;
    std::string         _filename;
    bool                _isRead;

}; // class Configator

} // namespace Config

#endif // _CONFIG_CONFIGATOR_HPP_