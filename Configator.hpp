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

// # include <string>
// #  include <cxxabi.h>

// template <typename T>
// std::string TypeToString__()
// {
//     using TR = typename std::remove_reference<T>::type;
//     #ifdef __GNUC__
//         char *demangle = abi::__cxa_demangle(typeid(TR), nullptr, nullptr, nullptr);
//         std::string retStr = demangle;
//         if (demangle != nullptr)
//             std::free(demangle);
//         else
//             retStr = typeid(TR);
//     #else
//         std::string retStr = typeid(TR);
//     #endif
//     if (std::is_const<TR>::value)
//         retStr += " const";
//     if (std::is_volatile<TR>::value)
//         retStr += " volatile";
//     if (std::is_lvalue_reference<T>::value)
//         retStr += "&";
//     else if (std::is_rvalue_reference<T>::value)
//         retStr += "&&";
//     return retStr;
// }

// # define TypeToString(_x) TypeToString__<decltype(_x)>()

namespace Conf
{

class Configator
{

private:

    struct Value
    {
        Value():
        typeString(std::string()),
        isInt(false),
        isLong(false),
        isUnsignedLong(false),
        isLongDouble(false),
        typeInt(-1),
        typeLong(-1),
        typeUnsignedLong(-1),
        typeDouble(-1),
        typeLongDouble(-1)
        {}

        Value &operator=(const std::string &value)
        {
            this->typeString = value;
            if (value.empty())
                return *this;
            bool             neg = false;
            const char *strValue = value.c_str();
            if (*strValue == '-')
            {
                neg = true;
                ++strValue;
            }
            else if (*strValue == '+')
            {
                ++strValue;
            }
            if (value.size() > 2 && *strValue == '0' && *(strValue + 1) == 'x')
            {
                try { this->typeInt = std::stoi(strValue, nullptr, 16); isInt = true; } catch (std::exception &e) { isInt = false; }
                try { this->typeLong = std::stol(strValue, nullptr, 16); isLong = true; } catch (std::exception &e) { isLong = false; }
                try { this->typeUnsignedLong = std::stoul(strValue, nullptr, 16); isUnsignedLong = true; } catch (std::exception &e) { isUnsignedLong = false; }
            }
            else if (value.size() > 2 && *strValue == '0' && *(strValue + 1) == 'b')
            {
                try { this->typeInt = std::stoi(strValue + 2, nullptr, 2); isInt = true; } catch (std::exception &e) { isInt = false; }
                try { this->typeLong = std::stol(strValue + 2, nullptr, 2); isLong = true; } catch (std::exception &e) { isLong = false; }
                try { this->typeUnsignedLong = std::stoul(strValue + 2, nullptr, 2); isUnsignedLong = true; } catch (std::exception &e) { isUnsignedLong = false; }
            }
            else if (value.size() > 2 && *strValue == '0')
            {
                try { this->typeInt = std::stoi(strValue, nullptr, 8); isInt = true; } catch (std::exception &e) { isInt = false; }
                try { this->typeLong = std::stol(strValue, nullptr, 8); isLong = true; } catch (std::exception &e) { isLong = false; }
                try { this->typeUnsignedLong = std::stoul(strValue, nullptr, 8); isUnsignedLong = true; } catch (std::exception &e) { isUnsignedLong = false; }
            }
            else
            {
                try { this->typeInt = std::stoi(strValue); isInt = true; } catch (std::exception &e) { isInt = false; }
                try { this->typeLong = std::stol(strValue); isLong = true; } catch (std::exception &e) { isLong = false; }
                try { this->typeUnsignedLong = std::stoul(strValue); isUnsignedLong = true; } catch (std::exception &e) { isUnsignedLong = false; }
            }

            try { this->typeDouble = std::stod(strValue); isDouble = true; } catch (std::exception &e) { isDouble = false; }
            try { this->typeLongDouble = std::stold(strValue); isLongDouble = true; } catch (std::exception &e) { isLongDouble = false; }

            if (neg)
            {
                this->typeLong *= -1;
                this->typeUnsignedLong *= -1;
                this->typeDouble *= -1;
            }
        }

        std::string     typeString;

        bool            isInt;
        bool            isLong;
        bool            isUnsignedLong;
        bool            isDouble;
        bool            isLongDouble;

        int             typeInt;
        long            typeLong;
        unsigned long   typeUnsignedLong;
        double          typeDouble;
        long double     typeLongDouble;
    };

    class _exception : public std::exception
    {

        public:
            _exception(std::string const &str) throw() : _str(str) {}
            virtual ~_exception() {}
            virtual const char *what() const throw() {return _str.c_str();}
        private:
            std::string _str;

    };

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

        typedef std::map<const std::string, Value, InsensitiveCompare>      MapSection;
        typedef std::map<const std::string, MapSection, InsensitiveCompare> MapConfig;
    #else
        typedef std::map<const std::string, Value>      MapSection;
        typedef std::map<const std::string, MapSection> MapConfig;
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

    Configator(const std::map<const std::string, std::map<const std::string, std::string> > &mapConfig):
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
     * @brief 
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
        readStream(filename, fileStream);
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

    void setConfig(const MapConfig &mapConfig)
    {
        _mapConfig = mapConfig;
    }

    void setSection(const std::string &section, const std::map<const std::string, std::string> &mapSection)
    {
        for (const auto &subItem : mapSection)
        {
            _mapConfig[section][subItem.first] = subItem.second;
        }
    }

    template<typename T>
    void setKey(const std::string &section, const std::string &key, const T &value)
    {
        std::ostringstream ss;
        ss << value;
        _mapConfig[section][key] = ss.str();
    }

    const MapConfig &getConfig(void) const
    {
        return _mapConfig;
    }

    const MapSection &getSection(const std::string &section) const
    {
        MapConfig::const_iterator itSectionTmp = _mapConfig.find(section);
        if (itSectionTmp == _mapConfig.end())
            return _emptyMapSection;
        return itSectionTmp->second;
    }

    const std::string& getValueString(const std::string& sectionName, const std::string& key, const std::string &defaultValue = "") const
    {
        MapConfig::const_iterator itSectionTmp = _mapConfig.find(sectionName);
        if (itSectionTmp == _mapConfig.end())
            return defaultValue;
        MapSection::const_iterator itKeyTmp = itSectionTmp->second.find(key);
        if (itKeyTmp == itSectionTmp->second.end())
            return defaultValue;
        return itKeyTmp->second.typeString;
    }

    const long& getValueLong(const std::string& sectionName, const std::string& key, const long &defaultValue = -1) const
    {
        MapConfig::const_iterator itSectionTmp = _mapConfig.find(sectionName);
        if (itSectionTmp == _mapConfig.end())
            return defaultValue;
        MapSection::const_iterator itKeyTmp = itSectionTmp->second.find(key);
        if (itKeyTmp == itSectionTmp->second.end())
            return defaultValue;
        return itKeyTmp->second.typeLong;
    }

    const unsigned long& getValueUnsignedLong(const std::string& sectionName, const std::string& key, const unsigned long &defaultValue = -1) const
    {
        MapConfig::const_iterator itSectionTmp = _mapConfig.find(sectionName);
        if (itSectionTmp == _mapConfig.end())
            return defaultValue;
        MapSection::const_iterator itKeyTmp = itSectionTmp->second.find(key);
        if (itKeyTmp == itSectionTmp->second.end())
            return defaultValue;
        return itKeyTmp->second.typeUnsignedLong;
    }

    const double& getValueDouble(const std::string& sectionName, const std::string& key, const double &defaultValue = -1) const
    {
        MapConfig::const_iterator itSectionTmp = _mapConfig.find(sectionName);
        if (itSectionTmp == _mapConfig.end())
            return defaultValue;
        MapSection::const_iterator itKeyTmp = itSectionTmp->second.find(key);
        if (itKeyTmp == itSectionTmp->second.end())
            return defaultValue;
        return itKeyTmp->second.typeDouble;
    }

    template<typename T>
    T                   getValue(const char *sectionName, const char *key, const T &defaultValue = 0) const
    {
        MapConfig::const_iterator itSectionTmp = _mapConfig.find(sectionName);
        if (itSectionTmp == _mapConfig.end())
            return T(defaultValue);
        MapSection::const_iterator itKeyTmp = itSectionTmp->second.find(key);
        // std::cout << TypeToString__<T>() << std::endl;
        if (itKeyTmp == itSectionTmp->second.end())
            return T(defaultValue);
        else if (typeid(T) == typeid(int) && itKeyTmp->second.isInt)
            return itKeyTmp->second.typeInt;
        else if (typeid(T) == typeid(long) && itKeyTmp->second.isLong)
            return itKeyTmp->second.typeLong;
        else if (typeid(T) == typeid(unsigned long) && itKeyTmp->second.isUnsignedLong)
            return itKeyTmp->second.typeUnsignedLong;
        else if (typeid(T) == typeid(double) && itKeyTmp->second.isDouble)
            return itKeyTmp->second.typeDouble;
        else if (typeid(T) == typeid(long double) && itKeyTmp->second.isLongDouble)
            return itKeyTmp->second.typeLongDouble;
        return T(defaultValue);
    }

    template<typename T>
    bool    isType(const char *sectionName, const char *key) const
    {
        MapConfig::const_iterator itSectionTmp = _mapConfig.find(sectionName);
        if (itSectionTmp == _mapConfig.end())
            return false;
        MapSection::const_iterator itKeyTmp = itSectionTmp->second.find(key);
        if (itKeyTmp == itSectionTmp->second.end())
            return false;
        else if (typeid(T) == typeid(int))
            return itKeyTmp->second.isInt;
        else if (typeid(T) == typeid(long))
            return itKeyTmp->second.isLong;
        else if (typeid(T) == typeid(unsigned long))
            return itKeyTmp->second.isUnsignedLong;
        else if (typeid(T) == typeid(double))
            return itKeyTmp->second.isDouble;
        else if (typeid(T) == typeid(long double))
            return itKeyTmp->second.isLongDouble;
    }

    template<typename T>
    const std::string    &getValue(const char *sectionName, const char *key, const std::string &defaultValue = std::string()) const
    {
        MapConfig::const_iterator itSectionTmp = _mapConfig.find(sectionName);
        if (itSectionTmp == _mapConfig.end())
            return defaultValue;
        MapSection::const_iterator itKeyTmp = itSectionTmp->second.find(key);
        if (itKeyTmp == itSectionTmp->second.end())
            return defaultValue;
        return itKeyTmp->second.typeString;
    }

    /**
     * @brief 
     * 
     * @param congifMap 
     */
    // void FillMap(std::map<const std::string, std::map<const std::string, std::string> >& congifMap) const
    // {
    //     if (_isRead == false)
    //     {
    //         throw _exception(_filename + ": was not read.");
    //     }

    //     // auto == std::pair<const std::string, std::map<const std::string, std::string> >
    //     for (auto &section : congifMap)
    //     {
    //         // auto == std::map<const std::string, std::map<const std::string, std::string> >::const_iterator
    //         auto itSectionTmp = _mapConfig.find(section.first);
    //         if (itSectionTmp == _mapConfig.end())
    //         {
    //             throw _exception(_filename + ": section [" + section.first + "] not found.");
    //         }
    //         // auto == std::pair<const std::string, std::string>
    //         for (auto &key : section.second)
    //         {
    //             // auto == std::map<const std::string, std::string>::const_iterator
    //             auto itKeyTmp = itSectionTmp->second.find(key.first);
    //             if (itKeyTmp == itSectionTmp->second.end())
    //             {
    //                 throw _exception(_filename + ": section [" + section.first + "]: key \"" + key.first + "\" not found.");
    //             }
    //             key.second = itKeyTmp->second;
    //         }
    //     }
    // }

    void PrintConfig(void)
    {
        // auto == std::pair<const std::string, std::map<const std::string, std::string> >
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
                if (key.second.typeString.find_first_of(' ') != key.second.typeString.npos || key.second.typeString.find_first_of('#') != key.second.typeString.npos || key.second.typeString.find_first_of(';') != key.second.typeString.npos || key.second.typeString.empty())
                    std::cout << "\"" << key.second.typeString << "\"";
                else
                    std::cout << key.second.typeString;
                std::cout << std::endl;                
            }
        }
    }

    /*
    @func:  ExportConfig
    @param: sExportFileName
    @brief: export at sExportFileName into .ini format the map
    */
    // void ExportConfig(const std::string &sExportFileName) const
    // {
    //     std::ofstream   fs(sExportFileName.c_str());
        
    //     if (fs.is_open() == false) // at error open
    //         return ;

    //     bool firstSection = true;
    //     // auto == std::pair<const std::string, std::map<const std::string, std::string> >
    //     for (const auto &section : _mapConfig)
    //     {
    //         if (firstSection == false)
    //             fs << std::endl;
    //         else
    //             firstSection = false;
    //         if (section.first.empty())
    //             fs << "[] ; global section" << std::endl;
    //         else
    //             fs << "[" << section.first << "]" << std::endl;
    //         // auto == std::pair<const std::string, std::string>
    //         for (const auto &key : section.second)
    //         {
    //             if (key.first.find_first_of(' ') != key.first.npos || key.first.find_first_of('#') != key.first.npos || key.first.find_first_of(';') != key.first.npos)
    //                 fs << "\"" << key.first << "\"";
    //             else
    //                 fs << key.first;
    //             fs << "=";
    //             if (key.second.find_first_of(' ') != key.second.npos || key.second.find_first_of('#') != key.second.npos || key.second.find_first_of(';') != key.second.npos || key.second.empty())
    //                 fs << "\"" << key.second << "\"";
    //             else
    //                 fs << key.second;
    //             fs << std::endl;                
    //         }
    //     }
    //     fs.close();
    // }
    MapConfig   _mapConfig;

private:

    static const MapSection _emptyMapSection;

    std::string _filename;
    bool        _isRead;

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
    void readStream(const std::string &filename, std::istream &fileStream)
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

};

};

#endif // _CONFIGATOR_HPP_