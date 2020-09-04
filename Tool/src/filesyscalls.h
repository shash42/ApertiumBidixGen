#ifndef GSOCAPERTIUM2020_FILESYSCALLS_H
#define GSOCAPERTIUM2020_FILESYSCALLS_H

#include <string>
#include <sys/stat.h>
#include <cstring>
#include <iostream>

using namespace std;

void createdir(string s, mode_t mode);
bool existspath(string s);

#endif
