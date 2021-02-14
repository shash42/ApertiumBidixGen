//Code for comparing output with human evaluation

#include <bits/stdc++.h>

using namespace std;

struct Trans{
    string uw, up, ul, vw, vp, vl;
    bool corr;
};

struct compobj{
    string uw, up, vw, vp;
};

struct srt{
    bool operator()(const compobj &a, const compobj &b){
        return a.uw < b.uw;
    }
};

void gethuman(vector<Trans> &T, string path){
    ifstream fin; fin.open(path);
    string line, res;
    Trans temp;
    while(getline(fin, line)){
        istringstream iss(line);
        getline(iss, temp.uw, '\t'); getline(iss, temp.up, '\t'); getline(iss, temp.ul, '\t');
        getline(iss, temp.vw, '\t'); getline(iss, temp.vp, '\t'); getline(iss, temp.vl, '\t');
        getline(iss, res);
        if(res=="Y") temp.corr = 1;
        else temp.corr = 0;
        T.push_back(temp);
    }
}

void getpred(vector<Trans> &T, string path, string l1){
    ifstream fin; fin.open(path);
    string line;
    Trans temp;
    while(getline(fin, line)){
        istringstream iss(line);
        getline(iss, temp.uw, '\t'); getline(iss, temp.up, '\t'); getline(iss, temp.ul, '\t');
        getline(iss, temp.vw, '\t'); getline(iss, temp.vp, '\t'); getline(iss, temp.vl, '\n');
        if(temp.ul != l1) continue;
        T.push_back(temp);
    }
}

pair<int, int> compare(vector<Trans> &T, map<compobj, int, srt> &M){
    int found=0, correct=0;
    set<compobj, srt> S;
    for(auto t: T){
        S.insert({t.uw, t.up, t.vw, t.vp});
    }
    for(compobj temp: S){;
        if(M.find(temp)!=M.end()){
            found++;
            correct+=M[temp];
        }
    }
    return {found, correct};
}

int main(int argc, char*argv[])
{
    if(argc!=5){
        cerr << "Usage: " << argv[0] <<  " <path-to-human-eval-tsv> <path-to-results-folder> <lang1> <lang2>" << endl;
        cerr << "Example: " << argv[0] << " LangData/HumanParsed/eng-cat.tsv Results/Expts/Eg/Analysis/ eng cat" << endl;
        return 0;
    }
    vector<Trans> H, P;
    string human = argv[1];
    string prefix = argv[2]; prefix += argv[3]; prefix += "-";
    prefix += argv[4]; prefix += "/";
    string l1 = argv[3];

    gethuman(H, human);
    map<compobj, int, srt> M;
    for(auto t: H){
        compobj temp; temp.up = t.up; temp.uw = t.uw; temp.vp = t.vp; temp.vw = t.vw;
        M[temp] = t.corr;
    }

    for(int i = 0; i < 4; i++){
        string inppath = prefix + "extra" + to_string(i) + ".txt";
        getpred(P, inppath, l1);
    }

    pair<int, int> metric = compare(P, M);

    string outpath = prefix + "human.txt";
    ofstream fout; fout.open(outpath);
    //fout << "lmao";
    fout << "Found: " << metric.first << "\tCorrect: " << metric.second << "\tPrecision: " << (100.00*metric.second) / metric.first;
}