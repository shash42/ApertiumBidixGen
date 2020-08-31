#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<sstream>
#include "filesystem.hpp"
#include "Convert2to3.cpp"

using namespace std;
namespace fs = std::filesystem;

string getlast(string &link, int trim = 0) //gets pos tag
{ //trim = 1 ignores the last character, call when it is \n or "
    string pos;
    bool started = false;
    for(int i = 0; i < link.length() - trim; i++)
    {
        if(started)     pos+=link[i];
        else if(link[i]=='#')    started = true;
    }
    return pos;
}

void getparts(string line, vector<string> &parts)
{
    string word = "";
    bool start=true;
    for(int i = 1; i < line.length(); i++)
    {
        if(!start && line.substr(i-2, 3) == " , ")
        {
            start=true;
            word = "";
        }
        else if(start && line.substr(i, 2) == " ,")
        {
            parts.push_back(word);
            start=false;
        } else{
            word+=line[i];
        }
    }
    if(parts[0][0]!='"')
    {
        parts[0] = "\"" + parts[0] + "\"";
    }
    if(parts[2][0]!='"')
    {
        parts[2] = "\"" + parts[2] + "\"";
    }
    parts[1] = getlast(parts[1], 1);
    parts[3] = getlast(parts[3], 1);
}
void Parse(string inp_path, string l1, string l2, string folderpath){
    ifstream fin;     fin.open(inp_path);
    if(folderpath.empty()) folderpath = "LangData/RDFParsed";
    fs::create_directory(folderpath); //create the directory
    if(folderpath[folderpath.length()-1]!='/') folderpath+="/";
    ofstream fout; fout.open(folderpath + l1 + "-" + l2 + ".txt");
    string line, word;
    getline(fin, line);
    int count = 0;
    while(getline(fin, line))
    {
        count++;
        vector<string> parts;
        getparts(line, parts);
        fout << parts[0] << "\t" << parts[1] << "\t" << l1 << parts[2] << "\t" << parts[3] << "\t" << l2 << endl;
    }
    fin.close();    fout.close();
}

int main(int argc, char *argv[]){
    if(argc>3 || argc<=1){
        cerr << "Usage: ./rdfparse path-to-languagepair-list [path-to-destination-folder]" << endl;
        return 0;
    }
    if(!fs::exists(argv[1])){
        cerr << "Language-pair list file path not found!!" << endl;
        return 0;
    }
    LangCodes LC; //load 2 digit to 3 digit language code table
    ifstream fin; fin.open(argv[1]); //list of languages to parse
    //freopen("LangData/Analysis.txt", "w", stdout);
    while(!fin.eof()){
        string path, l1, l2;
        getline(fin, path);
        getline(fin, l1, ' '); getline(fin, l2);
        if(l1.length()==0 || l2.length()==0) continue; //added check (ignore) for empty lines/langnames
        //cerr << l1 << " " << l2 << endl;
        cerr << l1 << " " << l2 << endl;
        if(LC.langcode2to3.find(l1)!=LC.langcode2to3.end()) l1 = LC.langcode2to3[l1]; //convert to 3 letter code
        if(LC.langcode2to3.find(l2)!=LC.langcode2to3.end()) l2 = LC.langcode2to3[l2]; //convert to 3 letter code
        Parse(path, l1, l2, argv[2]); //run parsing for the given language pair
    }
}