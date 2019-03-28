/**
 * @file Configator.hpp
 * @author Mickaël BLET
 */

#ifndef _CONFIG_CONFIGATOR_HPP_
# define _CONFIG_CONFIGATOR_HPP_

# include <sstream>
# include <fstream>
# include <iostream>
# include <iomanip> // lexicographical_compare
# include <map>
# include <mutex>   // mutex, lock_guard

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
     * @brief Construct a new Configator object
     * 
     * @param src 
     */
    Configator(Configator &&src):
    _mapConfig(std::move(src._mapConfig)),
    _filename(std::move(src._filename)),
    _isRead(std::move(src._isRead))
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
        std::lock_guard<std::mutex> lockGuard(this->_mutex);
        std::lock_guard<std::mutex> lockGuardParent(rhs._mutex);
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
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _isRead = false;

        std::ifstream fileStream(filename.c_str());
        if (fileStream.is_open() == false)
            return false;

        _mapConfig.clear();
        _filename = filename;
        _isRead = true;
        readStream(fileStream);
        fileStream.close();
        return true;
    }

    /**
     * @brief Get the Filename object
     * 
     * @return const std::string& 
     */
    const std::string& getFilename(void) const
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
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
        std::lock_guard<std::mutex> lockGuard(_mutex);
        return _isRead;
    }

    /**
     * @brief Set the Config object
     * 
     * @param mapConfig 
     */
    void setConfig(const std::map<const std::string, std::map<const std::string, std::string> > &mapConfig)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _mapConfig.clear();
        for (const std::pair<const std::string, std::map<const std::string, std::string> > &sectionMap : mapConfig)
        {
            for (const std::pair<const std::string, std::string> &keyMap : sectionMap.second)
            {
                _mapConfig[sectionMap.first][keyMap.first] = keyMap.second;
            }
        }
    }

    /**
     * @brief Add the Config object
     * 
     * @param mapConfig 
     */
    void addConfig(const std::map<const std::string, std::map<const std::string, std::string> > &mapConfig)
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
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
        std::lock_guard<std::mutex> lockGuard(_mutex);
        _mapConfig[section].clear();
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
        std::lock_guard<std::mutex> lockGuard(_mutex);
        std::ostringstream ss;
        ss << value;
        _mapConfig[section][key] = ss.str();
    }

    /**
     * @brief Get a copy of Config object
     * 
     * @return std::map<std::string, std::map<std::string, std::string> > 
     */
    std::map<std::string, std::map<std::string, std::string> > getConfig(void) const
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
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
     * @brief Get a copy of Section object from section name
     * 
     * @return std::map<std::string, std::string>
     */
    std::map<std::string, std::string> getSection(const std::string &section) const
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
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
        std::lock_guard<std::mutex> lockGuard(_mutex);
        *retValue = defaultValue;
        MapConfig::const_iterator itSectionTmp = _mapConfig.find(sectionName);
        if (itSectionTmp == _mapConfig.end())
            return false;
        MapSection::const_iterator itKeyTmp = itSectionTmp->second.find(key);
        if (itKeyTmp == itSectionTmp->second.end())
            return false;
        *retValue = convertValue<T>(itKeyTmp->second);
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
        std::lock_guard<std::mutex> lockGuard(_mutex);
        retValue = defaultValue;
        MapConfig::const_iterator itSectionTmp = _mapConfig.find(sectionName);
        if (itSectionTmp == _mapConfig.end())
            return false;
        MapSection::const_iterator itKeyTmp = itSectionTmp->second.find(key);
        if (itKeyTmp == itSectionTmp->second.end())
            return false;
        retValue = convertValue<T>(itKeyTmp->second);
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
    template<typename T>
    bool getValue(const std::string &sectionName, const std::string &key, T *retValue) const
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        MapConfig::const_iterator itSectionTmp = _mapConfig.find(sectionName);
        if (itSectionTmp == _mapConfig.end())
            return false;
        MapSection::const_iterator itKeyTmp = itSectionTmp->second.find(key);
        if (itKeyTmp == itSectionTmp->second.end())
            return false;
        *retValue = convertValue<T>(itKeyTmp->second);
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
    template<typename T>
    bool getValue(const std::string &sectionName, const std::string &key, T &retValue) const
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        MapConfig::const_iterator itSectionTmp = _mapConfig.find(sectionName);
        if (itSectionTmp == _mapConfig.end())
            return false;
        MapSection::const_iterator itKeyTmp = itSectionTmp->second.find(key);
        if (itKeyTmp == itSectionTmp->second.end())
            return false;
        retValue = convertValue<T>(itKeyTmp->second);
        return true;
    }

    /**
     * @brief Get the Value object from Section and Key
     * 
     * @tparam T : type of return value
     * @tparam U : type of default value
     * @param sectionName : name of section
     * @param key : name of key
     * @param defaultValue
     * @return T : if section and key exist value else defaultValue
     */
    template<typename T, typename U>
    T getValue(const std::string &sectionName, const std::string &key, const U &defaultValue) const
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        T retValue = defaultValue;
        MapConfig::const_iterator itSectionTmp = _mapConfig.find(sectionName);
        if (itSectionTmp == _mapConfig.end())
            return retValue;
        MapSection::const_iterator itKeyTmp = itSectionTmp->second.find(key);
        if (itKeyTmp == itSectionTmp->second.end())
            return retValue;
        retValue = convertValue<T>(itKeyTmp->second);
        return retValue;
    }

        /**
     * @brief Get the Value object from Section and Key
     * 
     * @tparam T : type of return value
     * @tparam U : type of default value
     * @param sectionName : name of section
     * @param key : name of key
     * @return T : if section and key exist value else defaultValue
     */
    template<typename T>
    T getValue(const std::string &sectionName, const std::string &key) const
    {
        std::lock_guard<std::mutex> lockGuard(_mutex);
        MapConfig::const_iterator itSectionTmp = _mapConfig.find(sectionName);
        if (itSectionTmp == _mapConfig.end())
            throw std::out_of_range("Configator:" + _filename + " section: " + sectionName + " not found");
        MapSection::const_iterator itKeyTmp = itSectionTmp->second.find(key);
        if (itKeyTmp == itSectionTmp->second.end())
            throw std::out_of_range("Configator:" + _filename + " key " + key + " in " + sectionName + " not found");
        return convertValue<T>(itKeyTmp->second);
    }

private:

    template<typename T>
    T convertValue(const std::string &value) const
    {
        T retValue;
        std::stringstream stringStream;
        if (value == "true" || value == "on")
        {
            stringStream << true;
            stringStream >> retValue;
            return retValue;
        }
        else if (value == "false" || value == "off")
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

    /**
     * @brief check if character is comment
     * 
     * @param c 
     * @return true : c is comment character
     * @return false : c is not comment character
     */
    static bool isComment(const unsigned char &c)
    {
        if (c != ';' && c != '#')
            return false;
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
    static bool parseKey(const std::string &line, std::string *retKey, std::string *retValue)
    {
        std::size_t startKey;
        std::size_t endKey;
        std::size_t startValue;
        std::size_t endValue;
        std::size_t i = 0;

        for (;isspace(line[i]);i++);
        if (line[i] == '=')
            return false; // first element not found
        if (line[i] == '\"')
        {
            for (startKey = ++i ; line[i] != '\"' ; ++i)
            {
                if (line[i] == '\0')
                    return false; // '"' not found
            }
            endKey = i++;
        }
        else
        {
            for (startKey = i ; line[i] != '=' ; ++i)
            {
                if (line[i] == '\0')
                    return false; // '=' not found
            }
            while (isspace(line[--i]));
            endKey = ++i;
        }
        for (;isspace(line[i]);i++);
        if (line[i++] != '=')
            return false; // '=' not found
        for (;isspace(line[i]);i++);
        if (line[i] == '\"')
        {
            for (startValue = ++i ; line[i] != '\"' ; ++i)
            {
                if (line[i] == '\0')
                    return false; // '"' not found
            }
            endValue = i++;
        }
        else
        {
            for (startValue = i ; isComment(line[i]) != true && line[i] != '\0' ; ++i);
            while (isspace(line[--i]));
            endValue = ++i;
        }
        for (;isspace(line[i]);i++);
        if (line[i] != '\0' && !isComment(line[i]))
            return false;
        *retKey = line.substr(startKey, endKey - startKey);
        *retValue = line.substr(startValue, endValue - startValue);
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

        for (;isspace(line[i]);++i);
        if (line[i++] != '[')
            return false;
        for (;isspace(line[i]);++i);
        start = i;
        while (line[i] != ']')
        {
            if (line[i++] == '\0')
                return false;
        }
        while (isspace(line[--i]));
        end = ++i;
        while (line[i++] != ']');
        for (;isspace(line[i]);++i);
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

        for (;isspace(line[i]);++i);
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
        std::string key;
        std::string value;

        std::string line;
        while(std::getline(fileStream, line))
        {
            if (emptyOrComment(line))
                continue;
            if (parseSection(line, &currentSectionName))
                continue;
            if (parseKey(line, &key, &value))
                _mapConfig[currentSectionName][key] = value;
        }
    }

    MapConfig           _mapConfig;
    std::string         _filename;
    bool                _isRead;
    mutable std::mutex  _mutex;

}; // class Configator

} // namespace Config

#endif // _CONFIG_CONFIGATOR_HPP_