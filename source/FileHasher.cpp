#include "FileHasher.h"
#include <md5.h>
#include <fstream>

// all disk reads go through this class
// it limits the number of concurrent readers
// to mitigate disk thrashing
DiskReader FileHasher::disk(1);

std::string FileHasher::getMD5Hash(const std::string& fPath){
    const int BUFF_SIZE = 4096;
    MD5 hash;
    char buffer[BUFF_SIZE];

    std::ifstream infile(fPath, std::ios::binary);
    while(infile.good()){
        int rCount = disk.read(infile, buffer, BUFF_SIZE);
        //infile.read(buffer, BUFF_SIZE);
        //int rCount = static_cast<int>(infile.gcount());
        if (rCount > 0) hash.update(buffer, rCount);
    }
    hash.finalize();
    return hash.hexdigest();
}