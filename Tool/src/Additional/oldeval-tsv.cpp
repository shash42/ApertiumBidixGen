//Human eval combine to get majority

#include <bits/stdc++.h>

using namespace std;

struct Trans{
    string uw, up, ul, vw, vp, vl;
    bool corr;
};

void getold(vector<Trans> &T, string path){
    ifstream fin; fin.open(path);
    string line;
    string u, v, res;
    Trans temp;
    getline(fin, line); //ignore header
    while(getline(fin, line)){
        istringstream iss(line);
        getline(iss, u, '\t'); getline(iss, v, '\t'); getline(iss, res, '\t');
        istringstream uiss(u);
        getline(uiss, temp.uw, '-'); getline(uiss, temp.up, '-'); getline(uiss, temp.ul, '-');
        istringstream viss(v);
        getline(viss, temp.vw, '-'); getline(viss, temp.vp, '-'); getline(viss, temp.vl, '-');
        if(res=="Correct") temp.corr = 1;
        else temp.corr = 0;
        T.push_back(temp);
    }
}

void getmaj(vector<Trans> I[3], vector<Trans> &T){
    for(int i = 0; i < I[0].size(); i++){
        int corr = 0;
        for(int j = 0; j < 3; j++){
            if(I[j][i].corr) corr++;
        }
        Trans temp = I[0][i];
        if(corr>=2) temp.corr = true;
        else temp.corr = false;
        T.push_back(temp);
    }
}

void printnew(vector<Trans> &T, string path){
    ofstream fout; fout.open(path);
    int outcorr=0;
    for(int i = 0; i < T.size(); i++){
        string YN = "Y"; 
        if(!T[i].corr) YN = "N";
        else outcorr++;
        fout << T[i].uw << "\t" << T[i].up << "\t" << T[i].ul << "\t" << T[i].vw << "\t" << T[i].vp << "\t" << T[i].vl << "\t" << YN << endl;
    }
    cout << "Correct: " << outcorr << endl;
}

signed main(){
    vector<Trans> I[3], T;
    string paths[] = {"LangData/HumanEval/oci-fra/Aure Séguier.tsv", "LangData/HumanEval/oci-fra/Aure Séguier.tsv", "LangData/HumanEval/oci-fra/Aure Séguier.tsv"};
    string outpath = "LangData/HumanParsed/oci-fra.tsv";
    for(int i = 0; i < 3; i++){
        getold(I[i], paths[i]);
    }
    getmaj(I, T);
    printnew(T, outpath);
}