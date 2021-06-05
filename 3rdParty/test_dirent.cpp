#include <dirent.h>
#include <iostream>

using namespace std;

void process_entry(dirent *entry) {
    switch(entry->d_type){
        case DT_DIR:    
            cout << "DIR : "<< entry->d_name << endl;
            break;
        case DT_REG: 
            cout << "FILE :" << entry->d_name << endl;
            break;
        default:
            cout << "Unknown : " << entry->d_name<<endl;
    }
}

void enumerate_dir (const char * root) {
    dirent *entry;
    DIR *dir = opendir(root);
    if (dir != nullptr){
        while((entry = readdir(dir)) != nullptr){
            process_entry(entry);
        }
        closedir(dir);
    }
    return;
}


int main(int argc, char** argv){

    if (argc != 2) {
        std::cout << "Expected single command line argument: path" << std::endl;
        return 1;
    }

    enumerate_dir(argv[1]);
    //const char* root = "C:\\Personal\\pg-alteration";
    //enumerate_dir(root);
    return 0;
}
