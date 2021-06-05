#pragma once 

#include <queue>
#include <mutex>
#include <string>

#include <dirent.h>

class FileEnumerator {
    private:
        std::queue<std::string> files;    // files that will be returned when next_file is called
        std::queue<std::string> dirs;     // directories we have yet to search
        std::recursive_mutex queue_mutex; // serialize access to the queues. recursive because next_file calls good
        
        // add the entry to the appropriate queue
        void process_entry(const std::string& root, const dirent* const entry);

        // should we process this entry or not? E.g., we skip directories named "." or ".."
        bool should_skip(const dirent* const entry);

        // if the files queue is empty, populate it with more
        void load_more();

        // loads entries from another directory
        void load_dir(const std::string& root);

    public:
        // creates a recursive enumerator for the path specified
        FileEnumerator(std::string root);

        // true if there are potentially more files available
        // false if we know that there will never be more files
        bool good();

        // the next file - or an empty string if there are no
        // more files (good() should returns false in this case)
        std::string next_file();
};