#include<bits/stdc++.h>

using namespace std;

signed main(){
    string path = "../LangData/langlist.txt";
    ifstream fin; fin.open(path);
    ofstream fout; fout.open("../LangData/ApertiumRaw-langfile-list.txt");
    while(!fin.eof()) {
        string l1, l2;
        fin >> l1 >> l2;
        string lp = l1 + "-" + l2;
        fout << "../LangData/Raw/apertium-" + lp + "." + lp + ".dix" << endl;
        fout << l1 << " " << l2 << endl;
    }
    fin.close(); fout.close();
}