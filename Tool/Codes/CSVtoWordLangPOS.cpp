#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<sstream>

using namespace std;

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
int main()
{
    string inp_path = "../Main/oc-fr.csv";
    string outp_path = "../Main/Data-oc-fr.txt";
    ifstream fin;
    ofstream fout;
    fin.open(inp_path);
    fout.open(outp_path);
    string line, word;
    getline(fin, line);
    int count = 0;
    while(getline(fin, line))
    {
        count++;
        vector<string> parts;
        getparts(line, parts);
        fout << parts[0] << "-" << parts[1] << "-oc " << parts[2] << "-" << parts[3] << "-fr" << endl;
    }
    fin.close();
    fout.close();
    return 0;
}