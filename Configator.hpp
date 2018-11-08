/**
 * @brief 
 * 
 * @file Configurator.hpp
 * @author BLET Mickael
 * @date 2018-09-24
 */

#ifndef _CONFIGATOR_HPP_
# define _CONFIGATOR_HPP_

# include <typeinfo>
# include <fstream>
# include <sstream>
# include <iostream>
# include <iomanip>
# include <list>
# include <map>

#define CONFIGATOR_INSENSITIVE_COMPARE

namespace Conf
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
                    s1.begin (), s1.end (),   // source range
                    s2.begin (), s2.end (),   // dest range
                    [](const unsigned char&c1, const unsigned char&c2) // lambda compare
                    {
                        return std::tolower(c1) < std::tolower(c2);
                    });  // comparison
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
     * @brief 
     * 
     * @return true 
     * @return false 
     */
    bool isRead(void) const
    {
        return _isRead;
    }

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

    void setSection(const std::string &section, const std::map<const std::string, std::string> &mapSection)
    {
        for (const std::pair<const std::string, std::string> &keyMap : mapSection)
        {
            _mapConfig[section][keyMap.first] = keyMap.second;
        }
    }

    template<typename T>
    void setKey(const std::string &section, const std::string &key, const T &value)
    {
        std::ostringstream ss;
        ss << value;
        _mapConfig[section][key] = ss.str();
    }

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

    template<typename T, typename U>
    bool getValue(const char *sectionName, const char *key, T *retValue, const U &defaultValue) const
    {
        *retValue = defaultValue;
        MapConfig::const_iterator itSectionTmp = _mapConfig.find(sectionName);
        if (itSectionTmp == _mapConfig.end())
            return false;
        MapSection::const_iterator itKeyTmp = itSectionTmp->second.find(key);
        if (itKeyTmp == itSectionTmp->second.end())
            return false;
        std::stringstream stringStream;
        const char *value = itKeyTmp->second.c_str();
        int index = 0;
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
        if (value[index] == '-' || value[index] == '+')
        {
            index++;
        }
        if (value[index] == '0' && value[index + 1] == 'x')
        {
            long long int tmp;
            stringStream << itKeyTmp->second;
            stringStream >> std::setbase(16) >> tmp;
            stringStream.str("");
            stringStream.clear();
            stringStream << std::setbase(10) << tmp;
            stringStream >> *retValue;
        }
        else
        {
            stringStream << itKeyTmp->second;
            stringStream >> *retValue;
        }
        return true;
    }

    template<typename T, typename U>
    bool getValue(const char *sectionName, const char *key, T &retValue, const U &defaultValue) const
    {
        retValue = defaultValue;
        MapConfig::const_iterator itSectionTmp = _mapConfig.find(sectionName);
        if (itSectionTmp == _mapConfig.end())
            return false;
        MapSection::const_iterator itKeyTmp = itSectionTmp->second.find(key);
        if (itKeyTmp == itSectionTmp->second.end())
            return false;
        std::stringstream stringStream;
        const char *value = itKeyTmp->second.c_str();
        int index = 0;
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
        if (value[index] == '-' || value[index] == '+')
        {
            index++;
        }
        if (value[index] == '0' && value[index + 1] == 'x')
        {
            long long int tmp;
            stringStream << itKeyTmp->second;
            stringStream >> std::setbase(16) >> tmp;
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

    void PrintConfig(void)
    {
        // auto == std::pair<std::string, std::map<std::string, std::string> >
        for (const auto &section : _mapConfig)
        {
            std::cout << "[" << section.first << "]" << std::endl;
            for (const auto &key : section.second)
            {
                if (key.first.find_first_of(' ') != key.first.npos || key.first.find_first_of('#') != key.first.npos || key.first.find_first_of(';') != key.first.npos)
                    std::cout << "\"" << key.first << "\"";
                else
                    std::cout << key.first;
                std::cout << "=";
                if (key.second.find_first_of(' ') != key.second.npos || key.second.find_first_of('#') != key.second.npos || key.second.find_first_of(';') != key.second.npos || key.second.empty())
                    std::cout << "\"" << key.second << "\"";
                else
                    std::cout << key.second;
                std::cout << std::endl;                
            }
        }
    }

private:

    bool parseKey(const std::string &line, const std::string &currentSectionName)
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
            while (line[i] != ';' && line[i] != '#' && line[i] != '\0')
                i++;
            i--;
            while (isspace(line[i]))
                i--;
            i++;
            endValue = i;
        }
        while (isspace(line[i]))
            i++;
        if (line[i] == '\0' || line[i] == ';' || line[i] == '#')
        {
            _mapConfig[currentSectionName][line.substr(startKey, endKey - startKey)] = line.substr(startValue, endValue - startValue);
            return true;
        }
        return false;
    }

    bool parseSection(const std::string &line, std::string *retSection)
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
        if (line[i] == '\0' || line[i] == ';' || line[i] == '#')
        {
            *retSection = line.substr(start, end - start);
            return true;
        }
        return false;
    }

    bool globalSection(const std::string &line)
    {
        std::size_t i = 0;
        while (isspace(line[i]))
            i++;
        if (line[i] != '[')
            return false;
        i++;
        if (line[i] != ']')
            return false;
        i++;
        while (isspace(line[i]))
            i++;
        if (line[i] == '\0' || line[i] == ';' || line[i] == '#')
            return true;
        return false;
    }

    bool comment(const std::string &line)
    {
        std::size_t i = 0;
        while (isspace(line[i]))
            i++;
        if (line[i] == ';' || line[i] == '#')
            return true;
        return false;
    }

    bool empty(const std::string &line)
    {
        for (const char &c : line)
        {
            if (!isspace(c))
                return false;
        }
        return true;
    }

    /*
    @func:  ReadFile
    @brief: parse and fill the map
    */
    void readStream(std::istream &fileStream)
    {
        std::string currentSectionName = "";

        std::string line;
        while(std::getline(fileStream, line))
        {
            if (!empty(line)
            &&  !comment(line)
            &&  !parseSection(line, &currentSectionName))
                parseKey(line, currentSectionName);
        }
    }

    MapConfig   _mapConfig;
    std::string _filename;
    bool        _isRead;

}; // end class Configator

} //end namespace Conf

#endif // _CONFIGATOR_HPP_