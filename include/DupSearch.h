#pragma once 

#include <mutex>
#include <atomic>
#include <list>
#include <future>
#include <functional>
#include <map>

#include "Duplicate.h"
#include "FileEnumerator.h"

class DupSearch{
    private:
        FileEnumerator enumerator;
        std::atomic<bool> started;
        std::mutex startLock;

        std::list<std::future<void>> fileHashers;
        std::future<void> scheduler;

        std::map<std::string, std::unique_ptr<Duplicate>> entries;
        std::mutex entriesLock;

        // hashes a file and adds to the entries map
        void process_file(const std::string& file);

        // deques files from files_to_process queue and processes them
        void hash_files();

        // schedules the background threads
        void thread_scheduler();

        // trims the non-duplicates from the map
        void trim();
    public:
        DupSearch(std::string root);

        // begins the hashing process
        void begin();

        // waits until the process is complete and 
        // executes the provided callback for each duplicate
        void get(std::function<void(const Duplicate* const dup)> pred);
};