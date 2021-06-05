
#include "DupSearch.h"
#include "FileHasher.h"

#include <iostream>

DupSearch::DupSearch(std::string root) : enumerator(root){

}

void DupSearch::begin(){
    if (!started) {
        std::unique_lock<std::mutex> gaurd(startLock);
        if (started) return;
        started = true;
    }
    scheduler = std::async(std::launch::async, &DupSearch::thread_scheduler, this);
}

void DupSearch::thread_scheduler(){
    unsigned int threadsCount = std::max<unsigned int>(std::thread::hardware_concurrency(),1);

    unsigned int i;
    for (i=0; i<threadsCount; i++){
        fileHashers.push_back(
            std::async(std::launch::async, &DupSearch::hash_files, this)
        );
    }

    for (std::future<void>& hasher : fileHashers) hasher.wait();

    trim();
}

void DupSearch::hash_files(){
    while(enumerator.good()){
        std::string f = enumerator.next_file();
        //std::cout << "Thread : " << std::this_thread::get_id() << " File : " << f<<std::endl;

        if (!f.empty()) process_file(f);
    }
}

void DupSearch::process_file(const std::string &file){
    std::string hash = FileHasher::getMD5Hash(file);
    std::lock_guard<std::mutex> map_guard(entriesLock);

    std::cout << "Thread : " << std::this_thread::get_id() << " Hash : " << hash << " File : " << file<<std::endl;
    // if we had map<string,duplicate> we could do this:
    // entries[digest].push_back(file);

    // Item 24 - Effective STL (Scott Meyers)
    // If a duplicate with our hash already exists in the map then this
    // will find it, otherwise it will find the appropriate location for an 
    // emplace_hint - this avoids traversing the map twice
    auto lb_existing = entries.lower_bound(hash);
    if (lb_existing != entries.end() && !(entries.key_comp()(hash, lb_existing->first)))
    {
        lb_existing->second->files.push_back(file);
    }
    else
    {
        entries.emplace_hint(lb_existing, hash,
            std::unique_ptr<Duplicate>(new Duplicate(hash, file)));
    }
}

// removes any non-duplicate entries from the map
// should only be called once hashing is complete
void DupSearch::trim()
{
    auto iter = entries.begin();
    auto end = entries.end();

    for (; iter != end;)
    {
        if (iter->second->files.size() <= 1)
        {
            entries.erase(iter++);
        }
        else
        {
            ++iter;
        }
    }
}

// waits for the process to complete and invokes the predicate
// for each duplicate 
void DupSearch::get(std::function<void(const Duplicate* const dup)> pred)
{
    if (scheduler.valid())
    {
        // the scheduler will terminate when the results are ready
        scheduler.wait();

        for (auto& entry : entries)
        {
            pred(entry.second.get());
        }
    }
}