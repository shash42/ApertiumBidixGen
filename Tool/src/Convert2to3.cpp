#ifndef GSOCAPERTIUM2020_3LETTERCODES_CPP
#define GSOCAPERTIUM2020_3LETTERCODES_CPP

#include<string>
#include<fstream>
#include<sstream>
#include<map>

using namespace std;

class LangCodes {
public:
    map<string, string> langcode2to3;
    LangCodes() {
        ifstream flang;
        flang.open("../LangData/ISO639 - Sheet1.tsv");
        string line, cell;
        int linenum = 0;
        while (getline(flang, line, '\n')) {
            linenum++;
            if (linenum == 1) continue; //ignore first line of the tsv
            line = line.substr(0, line.length() - 1); //clean last erroneous character
            istringstream entry(line);
            int colnum = 0;
            string let2, let3;
            while (getline(entry, cell, '\t')) {
                colnum++;
                if (colnum % 3 == 1) continue; //ignore the name
                else if (colnum % 3 == 2) let2 = cell;
                else if (colnum % 3 == 0) let3 = cell;
            }
            langcode2to3[let2] = let3;
        }
        flang.close();
    }
};
#endif //GSOCAPERTIUM2020_3LETTERCODES_CPP