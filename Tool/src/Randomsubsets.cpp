#include "Graph.h"
#include "Graph.cpp"
#include<chrono>
#include<random>
#include<iostream>

using namespace std;

typedef unsigned long long ull;
int rng(const int before, const int after) { // Generate random base in (before, after]
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::mt19937 mt_rand(seed ^ ull(new ull)); //remove ^ ull thing for speed
    int base = std::uniform_int_distribution<int>(before+1, after)(mt_rand);
    return base; //use steady_clock() for speed, for 64 bit mt19937_64 and lint
}

struct info
{
    wordData SLw, TLw;
    int sharedv = 0;
    float confidence = 0;
};

void getConf(vector<info> &translations, string &dirpath){

    ifstream file_poss; file_poss.open(dirpath + "/possibilities.txt");
    string edge;
    map<string, float> predmap;
    while(getline(file_poss, edge)){
        for(int j = edge.length()-1; j >= 0; j--){
            if(edge[j]=='\t'){
                string conf = edge.substr(j+1, edge.length());
                float confidence = stof(conf);
                predmap[edge.substr(0, j)] = confidence;
                break;
            }
        }
    }
    for(auto &t: translations){
        string pattern = t.SLw.surface + "\t" + t.TLw.surface;
        t.confidence = predmap[pattern];
    }
}

vector<info> getRand(vector<info> &pred, int num_req, string &l1){
    set<string> POS = {"noun", "verb", "properNoun", "adjective", "adverb", "numeral"};
    set<int> taken;
    vector<info> translations;
    taken.insert(-1); //extraneous value for the following while loop
    for(int i = 0; i < num_req; i++){
        int randidx = -1;
        while(taken.find(randidx)!=taken.end()){
            randidx = rng(0, pred.size());
            if(POS.find(pred[randidx].SLw.pos)==POS.end()) randidx=-1;
            else if(pred[randidx].SLw.lang != l1) randidx=-1;
            else if(pred[randidx].SLw.word_rep == pred[randidx].TLw.word_rep) randidx=-1;
            else if(pred[randidx].SLw.pos != pred[randidx].TLw.pos) randidx=-1;
        }
        taken.insert(randidx);
        translations.push_back(pred[randidx]);
    }
    return translations;
}

void Output(vector<info> &translations, string &dirpath){
    ofstream fout; fout.open(dirpath + "/randomsubset2.csv");
    for(auto &t: translations){
        fout << t.SLw.surface << " , " << t.TLw.surface << " , " << t.sharedv << " , " << t.confidence << '\n';
    }
}

void gen(string &exptno, int numpairs, string l1[], string l2[]){
    for(int i = 0; i < numpairs; i++){
        string lp1 = l1[i] + "-" + l2[i], lp2= l2[i] + "-" + l1[i]; //language pair to get predictions for
        string dirpath = "Results/Expts/" + exptno + "/Analysis/" + lp1;
        vector<info> predictions;

        for(int sharev = 0; sharev < 4; sharev++){
            cerr << i << " " << sharev << endl;
            string file_name = "/extra" + to_string(sharev) + ".txt";
            ifstream file_pred; file_pred.open(dirpath + file_name);
            string edge;
            while(getline(file_pred, edge)) {
                int j = 0;
                wordData SLw(j, edge, 3);
                wordData TLw(j, edge, 3);
                info temp; temp.SLw = SLw; temp.TLw = TLw; temp.sharedv = sharev;
                predictions.push_back(temp);
            }
        }
        vector<info> translations = getRand(predictions, 150, l1[i]);
        getConf(translations, dirpath);
        Output(translations, dirpath);
    }
}

signed main(){
    string exptno = "9";
    int numpairs = 1;
    string l1[] = {"oc"};
    string l2[] = {"fr"};
    gen(exptno, numpairs, l1, l2);
    return 0;
}