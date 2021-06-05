#pragma once

#include <mutex>
#include <atomic>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <thread>

class DiskReader {
    private:
        const int MAX_READERS;         // max number of readers allowed
        std::atomic<int> currReaders;  // current number of readers
        std::mutex readerLock;                  // to serialize access to readers
        std::condition_variable notify;         // to notify if a reader slot is available

        friend class ReaderCounter;
        // provides RAII based increment/decrement to ensure that
        // current_readers is balanced even if exceptions are thrown, etc.
        class ReaderCounter {
            DiskReader *disk;
            public:
                ReaderCounter(DiskReader *d) : disk(d) {
                    disk->currReaders++;
                    //std::cout<< " Thread : " << std::this_thread::get_id() << " Increasing reader .. " <<  disk->currReaders<<std::endl;
                }

                ~ReaderCounter() {
                    disk->currReaders--;
                    //std::cout<< " Thread : " << std::this_thread::get_id() << " Decreasing reader .. " <<  disk->currReaders<<std::endl;
                    disk->notify.notify_all();
                }
        };
    
    public:
        DiskReader(int max);
        int read(std::ifstream &infile, char *buffer, int bufferSize);
};