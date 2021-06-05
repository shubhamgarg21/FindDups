#pragma once 

#include <list>
#include <string>

class Duplicate {
    public:
        std::string hash;
        std::list<std::string> files;
        Duplicate(std::string h, std::string f) : hash(h){
            files.push_back(f);
        }
};
