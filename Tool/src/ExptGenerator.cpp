#include<bits/stdc++.h>

using namespace std;

const int N = 27;
string inp[N][2];

void genLangFile(){
    freopen("sampleconfigs/lang-full-expt.txt", "w", stdout);
    cout << N << endl;
    for(int i = 0; i < N; i++){
        cout << inp[i][0] << " " << inp[i][1] << endl;
        cout << N-1 << endl;
        for(int k = 0; k < 2; k++){
            for(int j = 0; j < N; j++){
                if(j==i) continue;
                cout << inp[j][k] << " ";
            }
            cout << endl;
        }
    }
}

void genLangPathList(){
    string inpprefix = "Results/Expts/Use26Gen1/Analysis/";
    string langprefix = "LangData/Parsed/";
    ofstream outp;
    for(int i = 0; i < N; i++){
        string path = inpprefix + inp[i][0] + "-" + inp[i][1] + "/langpathlist.txt";
        outp.open(path);
        for(int j = 0; j < N; j++){
            if(j==i) continue;
            string path2 = langprefix + inp[j][0] + "-" + inp[j][1] + ".txt";
            outp << path2 << endl;
        }
        outp.close();
    }
}

void genFolder(){
    string outpath = "sampleconfigs/folder-full-expt.txt";
    ofstream outp; outp.open(outpath);
    outp << N << endl;
    for(int i = 0; i < N; i++){
        outp << inp[i][0] << "-" << inp[i][1] << endl;
    }
    outp.close();
}
signed main(){
    freopen("LangData/useful3let.txt", "r", stdin);
    for(int i = 0; i < N; i++){
        cin >> inp[i][0] >> inp[i][1];
    }
    genLangPathList();
}