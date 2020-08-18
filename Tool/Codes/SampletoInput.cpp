#include<iostream>
#include<fstream>

using namespace std;

signed main()
{
    ifstream fin;
    ofstream fout;
    fin.open("../Main/SampleWord/veterinario.txt");
    fout.open("../Main/SampleWord/veterinario-word.txt");
    string edge;
    string out;
    while(getline(fin, edge))
    {
        int i = 0;
        out = "\"";
        while(edge[i]!='-')
        {
            out+=edge[i++];
        }
        out += "\"";
        while(edge[i]!=' ') out += edge[i++];
        out += edge[i++];
        out += "\"";
        while(edge[i]!='-')
        {
            out+=edge[i++];
        }
        out += "\"";
        while(i<edge.length())
        {
            out+=edge[i++];
        }
        fout << out << endl;
    }
}