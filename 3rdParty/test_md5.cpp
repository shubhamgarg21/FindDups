#include <md5.h>
#include <string>
#include <fstream>
#include <iostream>

std::string hash_file(const std::string& file)
{
    MD5 md5;
    char buffer[4096];

    std::ifstream infile(file, std::ios::binary);

    while (infile.good())
    {
        infile.read(buffer, sizeof(buffer));
        int count = infile.gcount();
        if (count > 0)
        {
            md5.update(buffer, count);
        }
    }

    md5.finalize();

    return md5.hexdigest();
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "Expected single command line argument: path" << std::endl;
        return 1;
    }

    std::cout << hash_file(argv[1]) << std::endl;

    return 0;
}