/**
 * @file Configurator.hpp
 * @author Mickaël BLET
 * @date 2018-11-26
 * @version v1.0
 */

#ifndef _CONFIG_CONFIGATOR_HPP_
# define _CONFIG_CONFIGATOR_HPP_

# include <sstream>
# include <fstream>
# include <iostream>
# include <iomanip>
# include <list>
# include <map>

// options
// #define CONFIGATOR_INSENSITIVE_COMPARE

namespace Config
{

class Configator
{

public:

    #ifdef CONFIGATOR_INSENSITIVE_COMPARE
        struct InsensitiveCompare
        {
            bool operator() (const std::string & s1, const std::string & s2) const
            {
                return std::lexicographical_compare(
                    s1.begin (), s1.end (), // source range
                    s2.begin (), s2.end (), // dest range
                    [](const unsigned char&c1, const unsigned char&c2) // lambda compare
                    {
                        return std::tolower(c1) < std::tolower(c2);
                    }); // comparison
            }
        };

        typedef std::map<std::string, std::string, InsensitiveCompare> MapSection;
        typedef std::map<std::string, MapSection, InsensitiveCompare>  MapConfig;
    #else
        typedef std::map<std::string, std::string> MapSection;
        typedef std::map<std::string, MapSection>  MapConfig;
    #endif

    /**
     * @brief Construct a new Configator object
     * 
     */
    Configator(void):
    _isRead(false)
    {
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
    }

    /**
     * @brief Construct a new Configator object
     * 
     * @param mapConfig 
     */
    Configator(const std::map<std::string, std::map<std::string, std::string> > &mapConfig):
    _isRead(false)
    {
        for (const auto &item : mapConfig)
        {
            for (const auto &subItem : item.second)
            {
                _mapConfig[item.first][subItem.first] = subItem.second;
            }
        }
    }

    /**
     * @brief Construct a new Configator object
     * 
     * @param src 
     */
    Configator(Configator &src) = default;

    /**
     * @brief Operator = overide
     * 
     * @param rhs 
     * @return Configator& 
     */
    Configator &operator=(const Configator &rhs) = default;

    /**
     * @brief Destroy the Configator object
     * 
     */
    ~Configator(void) = default;

    /**
     * @brief 
     * 
     * @param filename 
     * @return true 
     * @return false 
     */
    bool readFile(const std::string& filename)
    {
        _isRead = false;

        std::ifstream fileStream(filename.c_str());

        if (fileStream.is_open() == false)
            return false;

        _mapConfig.clear();
        readStream(fileStream);
        _filename = filename;
        fileStream.close();
        _isRead = true;
        return true;
    }

    /**
     * @brief Get the Filename object
     * 
     * @return const std::string& 
     */
    const std::string& getFilename(void) const
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
    void setConfig(const std::map<const std::string, std::map<const std::string, std::string> > &mapConfig)
    {
        for (const std::pair<const std::string, std::map<const std::string, std::string> > &sectionMap : mapConfig)
        {
            for (const std::pair<const std::string, std::string> &keyMap : sectionMap.second)
            {
                _mapConfig[sectionMap.first][keyMap.first] = keyMap.second;
            }
        }
    }

    /**
     * @brief Set the Section object by Name
     * 
     * @param section 
     * @param mapSection 
     */
    void setSection(const std::string &section, const std::map<const std::string, std::string> &mapSection)
    {
        for (const std::pair<const std::string, std::string> &keyMap : mapSection)
        {
            _mapConfig[section][keyMap.first] = keyMap.second;
        }
    }

    /**
     * @brief Set the Key object by sction name and key name
     * 
     * @tparam T 
     * @param section 
     * @param key 
     * @param value 
     */
    template<typename T>
    void setKey(const std::string &section, const std::string &key, const T &value)
    {
        std::ostringstream ss;
        ss << value;
        _mapConfig[section][key] = ss.str();
    }

    /**
     * @brief Get the copy Config object
     * 
     * @return std::map<std::string, std::map<std::string, std::string> > 
     */
    std::map<std::string, std::map<std::string, std::string> > getConfig(void) const
    {
        std::map<std::string, std::map<std::string, std::string> > ret;
        for (const auto &sectionMap : _mapConfig)
        {
            for (const auto &keyMap : sectionMap.second)
            {
                ret[sectionMap.first][keyMap.first] = keyMap.second;
            }
        }
        return ret;
    }

    /**
     * @brief Get the copy Section object from section name
     * 
     * @return std::map<std::string, std::map<std::string, std::string> > 
     */
    std::map<std::string, std::string> getSection(const std::string &section) const
    {
        std::map<std::string, std::string> ret;
        MapConfig::const_iterator itSectionTmp = _mapConfig.find(section);
        if (itSectionTmp == _mapConfig.end())
            return ret;
        for (const auto &keyMap : itSectionTmp->second)
        {
            ret[keyMap.first] = keyMap.second;
        }
        return ret;
    }

    /**
     * @brief Get the Value object from Section and Key
     * 
     * @tparam T : type of return value
     * @tparam U : type of default value
     * @param sectionName : name of section
     * @param key : name of key
     * @param retValue
     * @param defaultValue
     * @return true : if section and key exist
     * @return false : if section or key not exist (set retValue with defaultValue)
     */
    template<typename T, typename U>
    bool getValue(const std::string &sectionName, const std::string &key, T *retValue, const U &defaultValue) const
    {
        *retValue = defaultValue;
        MapConfig::const_iterator itSectionTmp = _mapConfig.find(sectionName);
        if (itSectionTmp == _mapConfig.end())
            return false;
        MapSection::const_iterator itKeyTmp = itSectionTmp->second.find(key);
        if (itKeyTmp == itSectionTmp->second.end())
            return false;
        std::stringstream stringStream;
        if (itKeyTmp->second == "true" || itKeyTmp->second == "on")
        {
            stringStream << true;
            stringStream >> *retValue;
            return true;
        }
        else if (itKeyTmp->second == "false" || itKeyTmp->second == "off")
        {
            stringStream << false;
            stringStream >> *retValue;
            return true;
        }
        int index = 0;
        if (itKeyTmp->second[index] == '-' || itKeyTmp->second[index] == '+')
        {
            index++;
        }
        if (itKeyTmp->second[index] == '0' && itKeyTmp->second[index + 1] == 'x')
        {
            long long int tmp;
            stringStream << itKeyTmp->second;
            stringStream >> std::setbase(16) >> tmp;
            stringStream.str("");
            stringStream.clear();
            stringStream << std::setbase(10) << tmp;
            stringStream >> *retValue;
        }
        else if (itKeyTmp->second[index] == '0' && itKeyTmp->second.find('.') == std::string::npos)
        {
            long long int tmp;
            stringStream << itKeyTmp->second;
            stringStream >> std::setbase(8) >> tmp;
            stringStream.str("");
            stringStream.clear();
            stringStream << std::setbase(10) << tmp;
            stringStream >> retValue;
        }
        else
        {
            stringStream << itKeyTmp->second;
            stringStream >> *retValue;
        }
        return true;
    }

    /**
     * @brief Get the Value object from Section and Key
     * 
     * @tparam T : type of return value
     * @tparam U : type of default value
     * @param sectionName : name of section
     * @param key : name of key
     * @param retValue
     * @param defaultValue
     * @return true : if section and key exist
     * @return false : if section or key not exist (set retValue with defaultValue)
     */
    template<typename T, typename U>
    bool getValue(const std::string &sectionName, const std::string &key, T &retValue, const U &defaultValue) const
    {
        retValue = defaultValue;
        MapConfig::const_iterator itSectionTmp = _mapConfig.find(sectionName);
        if (itSectionTmp == _mapConfig.end())
            return false;
        MapSection::const_iterator itKeyTmp = itSectionTmp->second.find(key);
        if (itKeyTmp == itSectionTmp->second.end())
            return false;
        std::stringstream stringStream;
        if (itKeyTmp->second == "true" || itKeyTmp->second == "on")
        {
            stringStream << true;
            stringStream >> retValue;
            return true;
        }
        else if (itKeyTmp->second == "false" || itKeyTmp->second == "off")
        {
            stringStream << false;
            stringStream >> retValue;
            return true;
        }
        int index = 0;
        if (itKeyTmp->second[index] == '-' || itKeyTmp->second[index] == '+')
        {
            index++;
        }
        if (itKeyTmp->second[index] == '0' && itKeyTmp->second[index + 1] == 'x')
        {
            long long int tmp;
            stringStream << itKeyTmp->second;
            stringStream >> std::setbase(16) >> tmp;
            stringStream.str("");
            stringStream.clear();
            stringStream << std::setbase(10) << tmp;
            stringStream >> retValue;
        }
        else if (itKeyTmp->second[index] == '0' && itKeyTmp->second.find('.') == std::string::npos)
        {
            long long int tmp;
            stringStream << itKeyTmp->second;
            stringStream >> std::setbase(8) >> tmp;
            stringStream.str("");
            stringStream.clear();
            stringStream << std::setbase(10) << tmp;
            stringStream >> retValue;
        }
        else
        {
            stringStream << itKeyTmp->second;
            stringStream >> retValue;
        }
        return true;
    }

private:

    /**
     * @brief check if character is comment
     * 
     * @param c 
     * @return true : @c is comment character
     * @return false : @c is not comment character
     */
    bool                isComment(const unsigned char &c)
    {
        if (c == ';' || c == '#')
            return true;
        return false;
    }

    /**
     * @brief parse and add key in _mapConfig at currentSectionName
     * 
     * @param line 
     * @param currentSectionName 
     * @return true : if line is a key value
     * @return false : if line is not a key value
     */
    bool                parseKey(const std::string &line, const std::string &currentSectionName)
    {
        std::size_t startKey;
        std::size_t endKey;
        std::size_t startValue;
        std::size_t endValue;
        std::size_t i = 0;

        while (isspace(line[i]))
            i++;
        if (line[i] == '=')
            return false;
        if (line[i] == '\"')
        {
            i++;
            startKey = i;
            while (line[i] != '\"')
            {
                if (line[i] == '\0')
                    return false;
                i++;
            }
            endKey = i;
        }
        else
        {
            startKey = i;
            while (line[i] != '=')
            {
                if (line[i] == '\0')
                    return false;
                i++;
            }
            i--;
            while (isspace(line[i]))
                i--;
            i++;
            endKey = i;
        }
        while (isspace(line[i]))
            i++;
        while (line[i] != '=')
        {
            if (line[i] == '\0')
                return false;
            i++;
        }
        i++;
        while (isspace(line[i]))
            i++;
        if (line[i] == '\"')
        {
            i++;
            startValue = i;
            while (line[i] != '\"')
            {
                if (line[i] == '\0')
                    return false;
                i++;
            }
            endValue = i;
            i++;
        }
        else
        {
            startValue = i;
            while (isComment(line[i]) != true && line[i] != '\0')
                i++;
            i--;
            while (isspace(line[i]))
                i--;
            i++;
            endValue = i;
        }
        while (isspace(line[i]))
            i++;
        if (isComment(line[i]) || line[i] == '\0')
        {
            _mapConfig[currentSectionName][line.substr(startKey, endKey - startKey)] = line.substr(startValue, endValue - startValue);
            return true;
        }
        return false;
    }

    /**
     * @brief parse and get section name
     * 
     * @param line 
     * @param retSection : return name of section if found
     * @return true : if section found
     * @return false : if section not found
     */
    bool                parseSection(const std::string &line, std::string *retSection)
    {
        std::size_t i = 0;
        while (isspace(line[i]))
            i++;
        if (line[i] != '[')
            return false;
        i++;
        while (isspace(line[i]))
            i++;
        std::size_t start = i;
        while (line[i] != ']')
        {
            if (line[i] == '\0')
                return false;
            i++;
        }
        i--;
        while (isspace(line[i]))
            i--;
        i++;
        std::size_t end = i;
        while (line[i] != ']')
            i++;
        i++;
        while (isspace(line[i]))
            i++;
        if (isComment(line[i]) || line[i] == '\0')
        {
            *retSection = line.substr(start, end - start);
            return true;
        }
        return false;
    }

    /**
     * @brief detect if line is empty or comment
     * 
     * @param line 
     * @return true : line is empty or comment
     * @return false : line is not empty or comment
     */
    bool                emptyOrComment(const std::string &line)
    {
        std::size_t i = 0;
        while (isspace(line[i]))
            i++;
        if (isComment(line[i]) || line[i] == '\0')
            return true;
        return false;
    }

    /**
     * @brief parse and fill the map from fileStream
     * 
     * @param fileStream 
     */
    void                readStream(std::istream &fileStream)
    {
        std::string currentSectionName = "";

        std::string line;
        while(std::getline(fileStream, line))
        {
            if (!emptyOrComment(line)
            &&  !parseSection(line, &currentSectionName))
                parseKey(line, currentSectionName);
        }
    }

    MapConfig   _mapConfig;
    std::string _filename;
    bool        _isRead;

}; // end class Configator

} // end namespace Config

#endif // _CONFIG_CONFIGATOR_HPP_