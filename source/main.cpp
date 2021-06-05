#include "DupSearch.h"
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Expected single command line argument: path" << std::endl;
    }

    DupSearch dups(argv[1]);

    dups.begin();

    dups.get([](const Duplicate* const dup)
    {
        std::cout << "Duplicates - Hash " << dup->hash << std::endl;
        for (auto file : dup->files)
        {
            std::cout << "    " << file << std::endl;
        }
    });
}
