#include "FileEnumerator.h"
#include <memory>
#include <functional>

FileEnumerator::FileEnumerator(std::string root){
    dirs.push(root);
}

std::string FileEnumerator::next_file(){
    std::unique_lock<std::recursive_mutex> gaurd(queue_mutex);

    std::string to_return;

    if (good()){
        to_return = files.front();
        files.pop();
    }
    return to_return;
}

bool FileEnumerator::good(){
    std::unique_lock<std::recursive_mutex> gaurd(queue_mutex);
    load_more();
    return files.size() > 0;
}

void FileEnumerator::load_more(){
    while(files.size() == 0 && dirs.size()>0){
        std::string next_dir = dirs.front();
        dirs.pop();

        load_dir(next_dir);
    }
}

void FileEnumerator::load_dir(const std::string& root){
    struct dirent* entry;

    // use RAII to close the directory
    std::unique_ptr<DIR, std::function<void(DIR*)>> dir(opendir(root.c_str()), [](DIR* d){
        if (d != nullptr) closedir(d);
    });

    if (dir != nullptr){
        while((entry = readdir(dir.get())) != nullptr){
            process_entry(root, entry);
        }
    }
}

void FileEnumerator::process_entry(const std::string& root, const dirent* const entry) {
    if (should_skip(entry)) return;

    std::string full_path = root + "/" + entry->d_name;

    switch (entry->d_type)
    {
    case DT_REG:
        files.push(full_path);
        break;
    case DT_DIR:
        dirs.push(full_path);
        break;
    default:
        // write error? 
        break;
    }
}

bool FileEnumerator::should_skip(const dirent* const entry){
    if (entry->d_type == DT_DIR && 
        (strcmp(entry->d_name,".") == 0 || strcmp(entry->d_name,"..") == 0)) return true;
    return false;
}