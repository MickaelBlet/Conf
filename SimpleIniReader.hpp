#ifndef _CONFIGATOR_HPP_
# define _CONFIGATOR_HPP_

#include <fstream>
#include <iostream>
#include <iomanip>
#include <list>
#include <map>

#define CONFIGATOR_INSENSITIVE_COMPARE

class Configator
{

private:

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

        typedef std::map<const std::string, std::string, InsensitiveCompare>    MapSection;
        typedef std::map<const std::string, MapSection, InsensitiveCompare>     MapConfig;
    #else
        typedef std::map<const std::string, std::string>    MapSection;
        typedef std::map<const std::string, MapSection>     MapConfig;
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

    Configator(const MapConfig &mapConfig):
    _isRead(false)
    {
        _mapConfig = mapConfig;
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

    void setSection(const std::string &section, const MapSection &mapSection)
    {
        _mapConfig[section] = mapSection;
    }

    void setKey(const std::string &section, const std::string &key, const std::string &value = "")
    {
        _mapConfig[section][key] = value;
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

    const std::string& getValue(const std::string& sectionName, const std::string& key, const std::string &defaultValue = "") const
    {
        MapConfig::const_iterator itSectionTmp = _mapConfig.find(sectionName);
        if (itSectionTmp == _mapConfig.end())
            return defaultValue;
        MapSection::const_iterator itKeyTmp = itSectionTmp->second.find(key);
        if (itKeyTmp == itSectionTmp->second.end())
            return defaultValue;
        return itKeyTmp->second;
    }

    /**
     * @brief 
     * 
     * @param congifMap 
     */
    void FillMap(std::map<const std::string, std::map<const std::string, std::string> >& congifMap) const
    {
        if (_isRead == false)
        {
            throw _exception(_filename + ": was not read.");
        }

        // auto == std::pair<const std::string, std::map<const std::string, std::string> >
        for (auto &section : congifMap)
        {
            // auto == std::map<const std::string, std::map<const std::string, std::string> >::const_iterator
            auto itSectionTmp = _mapConfig.find(section.first);
            if (itSectionTmp == _mapConfig.end())
            {
                throw _exception(_filename + ": section [" + section.first + "] not found.");
            }
            // auto == std::pair<const std::string, std::string>
            for (auto &key : section.second)
            {
                // auto == std::map<const std::string, std::string>::const_iterator
                auto itKeyTmp = itSectionTmp->second.find(key.first);
                if (itKeyTmp == itSectionTmp->second.end())
                {
                    throw _exception(_filename + ": section [" + section.first + "]: key \"" + key.first + "\" not found.");
                }
                key.second = itKeyTmp->second;
            }
        }
    }

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
                if (key.second.find_first_of(' ') != key.second.npos || key.second.find_first_of('#') != key.second.npos || key.second.find_first_of(';') != key.second.npos || key.second.empty())
                    std::cout << "\"" << key.second << "\"";
                else
                    std::cout << key.second;
                std::cout << std::endl;                
            }
        }
    }

    /*
    @func:  ExportConfig
    @param: sExportFileName
    @brief: export at sExportFileName into .ini format the map
    */
    void ExportConfig(const std::string &sExportFileName) const
    {
        std::ofstream   fs(sExportFileName.c_str());
        
        if (fs.is_open() == false) // at error open
            return ;

        bool firstSection = true;
        // auto == std::pair<const std::string, std::map<const std::string, std::string> >
        for (const auto &section : _mapConfig)
        {
            if (firstSection == false)
                fs << std::endl;
            else
                firstSection = false;
            if (section.first.empty())
                fs << "[] ; global section" << std::endl;
            else
                fs << "[" << section.first << "]" << std::endl;
            // auto == std::pair<const std::string, std::string>
            for (const auto &key : section.second)
            {
                if (key.first.find_first_of(' ') != key.first.npos || key.first.find_first_of('#') != key.first.npos || key.first.find_first_of(';') != key.first.npos)
                    fs << "\"" << key.first << "\"";
                else
                    fs << key.first;
                fs << "=";
                if (key.second.find_first_of(' ') != key.second.npos || key.second.find_first_of('#') != key.second.npos || key.second.find_first_of(';') != key.second.npos || key.second.empty())
                    fs << "\"" << key.second << "\"";
                else
                    fs << key.second;
                fs << std::endl;                
            }
        }
        fs.close();
    }

private:

    static const MapSection _emptyMapSection;

    std::string _filename;
    bool        _isRead;
    MapConfig   _mapConfig;

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

#endif // _CONFIGATOR_HPP_