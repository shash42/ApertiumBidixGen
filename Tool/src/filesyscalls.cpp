#include "filesyscalls.h"

void createdir(string s, mode_t mode){
    const char *temppath = s.c_str();
    int ret = mkdir(temppath, mode);
    if(ret < 0 && errno!=EEXIST){
        cerr << "Error creating directory : " << strerror(errno) << endl;
    }
}

bool existspath(string s){
    struct stat sb;
    const char *temppath = s.c_str();
    return (stat(temppath, &sb) == 0);
}