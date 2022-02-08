#ifndef _FILE_GUARD_HPP_
#define _FILE_GUARD_HPP_

#include <fstream>
#include <string>
#include <cstdio>

class FileGuard : public std::ofstream {
  public:

    FileGuard(const char* filename, std::ios_base::openmode openMode)
        : std::ofstream(filename, openMode),
          _filename(filename) {

    }
    ~FileGuard() {
        close();
        remove(_filename.c_str());
    }

  private:

    std::string _filename;
};

#endif //_FILE_GUARD_HPP_