#include "DiskReader.h"

// must use initializer list to set the value of const READERS 
DiskReader::DiskReader(int max) : MAX_READERS(std::max(max,1)) {

}

int DiskReader::read(std::ifstream& infile, char* buffer, int bufferSize){
    int readCount = 0;

    std::unique_lock<std::mutex> gaurd(readerLock);
    notify.wait(gaurd, [this]{
        return currReaders < MAX_READERS;
    });

    ReaderCounter readerCounter(this);
    gaurd.unlock();

    if (infile.good()){
        infile.read(buffer,bufferSize);
        readCount = static_cast<int>(infile.gcount());
    }

    // why ???
    readerCounter.~ReaderCounter();
    
    return readCount;
}

