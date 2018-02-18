#ifndef SIMPLE_INI_READER_HPP
# define SIMPLE_INI_READER_HPP

#include <fstream>
#include <iostream>
#include <iomanip>
#include <regex>
#include <list>
#include <map>

class   SimpleIniReader
{

private:

    class _exception : public std::exception
    {

        public:
            _exception(std::string const & str) throw() : _str(str) {}
            virtual ~_exception() {}
            virtual const char *what() const throw() {return _str.c_str();}
        private:
            std::string _str;

    };

public:
    /*
    @func: Constructor
    */
    SimpleIniReader(void)
    : _isRead(false)
    {
    }

    /*
    @func: Constructor
    @param: sFileName
    */
    SimpleIniReader(const std::string& sFileName)
    : _isRead(false)
    {
        Read(sFileName);
    }

    /*
    @func: Copie Constructor
    @param: sFileName
    */
    SimpleIniReader( SimpleIniReader& src)
    {
        *this = src;
    }

    SimpleIniReader& operator=(const SimpleIniReader& rhs)
    {
        if (this == &rhs)
            return *this;
        _sFileName = rhs._sFileName;
        _isRead = rhs._isRead;
        _iniMap = rhs._iniMap;
        return *this;
    }

    /*
    @func: Destructor
    */
    ~SimpleIniReader(void)
    {
    }

    /*
    @func:  Read
    @param: sFileName
    @brief: open sFileName and parse
            insert data into map
            close file
    @return:read file complete
    */
    bool Read(const std::string& sFileName)
    {
        _isRead = false;
        _iniMap.clear();

        _sFileName = sFileName;

        _fileStream.open(sFileName.c_str());

        if (_fileStream.is_open())
        {
            ReadFile();
            _fileStream.close();
            _isRead = true;            
        }
        return _isRead;
    }

    bool IsRead(void) const
    {
        return _isRead;
    }

    /*
    @func:  GetFileName
    @return:current fileName
    */
    const std::string& GetFileName(void) const
    {
        return _sFileName;
    }

    /*
    @func:  FillMap
    @brief: set the congifMap from the current map
    @except:file was not read
            section not found
            key not found
    */
    void FillMap(std::map<const std::string, std::map<const std::string, std::string> >& congifMap) const
    {
        if (_isRead == false)
        {
            throw _exception(_sFileName + ": was not read.");
        }

        // auto == std::pair<const std::string, std::map<const std::string, std::string> >
        for (auto &section : congifMap)
        {
            // auto == std::map<const std::string, std::map<const std::string, std::string> >::const_iterator
            auto itSectionTmp = _iniMap.find(section.first);
            if (itSectionTmp == _iniMap.end())
            {
                throw _exception(_sFileName + ": section [" + section.first + "] not found.");
            }
            // auto == std::pair<const std::string, std::string>
            for (auto &key : section.second)
            {
                // auto == std::map<const std::string, std::string>::const_iterator
                auto itKeyTmp = itSectionTmp->second.find(key.first);
                if (itKeyTmp == itSectionTmp->second.end())
                {
                    throw _exception(_sFileName + ": section [" + section.first + "]: key \"" + key.first + "\" not found.");
                }
                key.second = itKeyTmp->second;
            }
        }
    }

    /*
    @func:  GetMap
    @return:map of class
    */
    const std::map<const std::string, std::map<const std::string, std::string> >& GetMap(void) const
    {
        return _iniMap;
    }

    /*
    @func:  GetSection
    @param: sSectionName
    @brief: get const map of sSectionName
    @return:const map of sSectionName
            if not found sSectionName : emptySection
    */
    const std::map<const std::string, std::string>& GetSection(const std::string& sSectionName) const
    {
        // auto == std::map<const std::string, std::map<const std::string, std::string> >::const_iterator
        auto itSectionTmp = _iniMap.find(sSectionName);
        if (itSectionTmp == _iniMap.end())
            return _emptySectionMap;
        return itSectionTmp->second;
    }

    /*
    @func:  GetValue
    @param: sSectionName, sValueName, sDefaultValue = ""
    @brief: get sValueName in sSectionName
    @return:const string of sValueName
            if not found sDefaultValue
    */
    const std::string& GetValue(const std::string& sSectionName, const std::string& sValueName, const std::string& sDefaultValue = "") const
    {
        // auto == std::map<const std::string, std::map<const std::string, std::string> >::const_iterator
        auto itSectionTmp = _iniMap.find(sSectionName);
        if (itSectionTmp == _iniMap.end())
            return sDefaultValue;
        // auto == std::map<const std::string, std::string>::const_iterator
        auto itKeyTmp = itSectionTmp->second.find(sValueName);
        if (itKeyTmp == itSectionTmp->second.end())
            return sDefaultValue;
        return itKeyTmp->second;
    }

    /*
    @func:  SetValue
    @param: sSectionName, sValueName, sNewValue
    @brief: set sValueName in sSectionName
            create section if not exist
    @return:ref string of sNewValue
    */
    std::string& SetValue(const std::string& sSectionName, const std::string& sValueName, const std::string& sNewValue)
    {
        _iniMap[sSectionName][sValueName] = sNewValue;
        return _iniMap[sSectionName][sValueName];
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
        for (const auto &section : _iniMap)
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
    std::string                                                             _sFileName;
    std::ifstream                                                           _fileStream;
    bool                                                                    _isRead;
    std::map<const std::string, std::map<const std::string, std::string> >  _iniMap;
    std::map<const std::string, std::string>                                _emptySectionMap;

    /*
    @func:  ReadFile
    @brief: parse and fill the map
    */
    void ReadFile(void)
    {
        std::string sLine;
        std::string sCurrentSectionName = "";
        const std::regex patternEmpty("^\\s*$");
        const std::regex patternComment("^\\s*[#;]");
        const std::regex patternGlobalSection("^\\s*\\[\\]\\s*(?:[#;].*)?$");
        const std::regex patternSection("^\\s*\\[\\s*(.*\\S)\\s*\\]\\s*(?:[#;].*)?$");
        const std::regex patternKey("^\\s*(?:\"(.+)\"|(.*[^\\s\"]))\\s*=\\s*(?:\"(.*)\"|([^#;]*\\S))\\s*(?:[#;].*)?$");

        while(std::getline(_fileStream, sLine))
        {
            std::smatch match;
            // empty line
            if (std::regex_search (sLine, match, patternEmpty))
                continue;
            // comment line
            if (std::regex_search (sLine, match, patternComment))
                continue;
            // global section
            if (std::regex_search (sLine, match, patternGlobalSection))
            {
                sCurrentSectionName = match[1].str();
                continue;
            }
            // section
            if (std::regex_search (sLine, match, patternSection))
            {
                sCurrentSectionName = match[1].str();
                continue;
            }
            // key = value
            if (std::regex_search (sLine, match, patternKey))
            {
                const std::string &sKey     = (match[1].str().empty()) ? match[2].str() : match[1].str();
                const std::string &sValue   = (match[3].str().empty()) ? match[4].str() : match[3].str();
                _iniMap[sCurrentSectionName][sKey] = sValue;
                continue;
            }
        }
    }

};

#endif