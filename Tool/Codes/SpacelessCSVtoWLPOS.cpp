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

string getword(int &i, string &line)
{
    string word = "\"";
    while(i < line.length())
    {
        if(line.substr(i, 2) == ",h")
        {
            word+="\"";
            i++;
            return word;
        }
        else word+=line[i];
        i++;
    }
}
string getlink(int &i, string &line)
{
    string word;
    while(i < line.length())
    {
        if(line[i]==',' || i==line.length()-1)
        {
            i++;
            return word;
        }
        else word+=line[i];
        i++;
    }
}
void getparts(string line, vector<string> &parts)
{
    int i = 0;
    parts.push_back(getword(i, line));
    parts.push_back(getlink(i, line));
    parts.push_back(getword(i, line));
    parts.push_back(getlink(i, line));
    parts[1] = getlast(parts[1]);
    parts[3] = getlast(parts[3]);
}
int main()
{
    string inp_path = "../Main/en-es.csv";
    string outp_path = "../Main/Data-en-es.txt";
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
        fout << parts[0] << "-" << parts[1] << "-en " << parts[2] << "-" << parts[3] << "-es" << endl;
    }
    fin.close();
    fout.close();
    return 0;
}