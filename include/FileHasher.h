#pragma once 

#include "DiskReader.h"
#include <string>

class FileHasher{
    private:
        static DiskReader disk;
        
        // private constructer, so that no object can be created
        FileHasher() {

        }
    public:
        static std::string getMD5Hash(const std::string& fPath);
};