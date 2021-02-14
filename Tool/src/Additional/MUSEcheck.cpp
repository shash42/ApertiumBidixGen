//Code for comparison with MUSE data

#include<bits/stdc++.h>

using namespace std;

//modify - list of language pairs to compare on
vector<pair<string, string>> langlist = {{"eng", "cat"}, {"eng", "spa"}, {"fra", "spa"}, {"spa", "ita"}, {"spa", "por"}};
int num_langs = langlist.size();

struct info{
    int tot=0, cor=0, bwtot=0, bwcor=0;
    int cat_tot[4]={0, 0, 0, 0}, cat_cor[4]={0, 0, 0, 0};
    int cat_bwtot[4] = {0, 0, 0, 0}, cat_bwcor[4]={0, 0, 0, 0};
} extraresults[5];

struct recinfo{
    int tot=0, cor=0, bwtot=0, bwcor=0;
} recresults[5];

void loadMUSE(set<pair<string, string>> &trans, set<string> &words, string path){
    ifstream fin; fin.open(path);
    string line;
    while(getline(fin, line)){
        istringstream iss(line);
        string s1, s2;
        iss >> s1 >> s2;
        trans.insert({s1, s2});
        words.insert(s1); words.insert(s2);
    }
}

void loadPred(vector<pair<string, string>> &trans, string path, string l1, string l2){
    set<string> POS = {"noun", "verb", "adjective", "adverb", "numeral"}; //modify - POS of predicted translations to use
    ifstream fin; fin.open(path);
    string line;
    while(getline(fin, line)){
        istringstream iss(line);
        string uw, up, ul, vw, vp, vl;
        getline(iss, uw, '\t'); getline(iss, up, '\t'); getline(iss, ul, '\t');
        getline(iss, vw, '\t'); getline(iss, vp, '\t'); getline(iss, vl, '\t');
        if(POS.find(up)==POS.end()) continue;
        if(ul!=l1 || vl!=l2) continue;
        trans.push_back({uw, vw});
    }
}

void printFull(string outpath){
    ofstream fout; fout.open(outpath);
    float avgP[] = {0, 0, 0, 0, 0};

    for(int i = 0; i < num_langs; i++){
        fout << langlist[i].first << " " << langlist[i].second << endl;
        int Oet = extraresults[i].bwtot, Oec = extraresults[i].bwcor;
        float Oprecision = (Oec*100.00)/Oet;
        avgP[0] += Oprecision;
        fout << "Overall:" << " Total " << Oet << " Correct " << Oec << " Percentage " << Oprecision << endl;

        for(int j = 0; j < 4; j++){
            int et = extraresults[i].cat_bwtot[j], ec = extraresults[i].cat_bwcor[j];
            float precision = (ec*100.00)/et;
            avgP[j+1] += precision;
            fout << "Extra " << j << ": Total " << et << " Correct " << ec << " Percentage " << precision << endl;
        }
        int et1 = extraresults[i].cat_bwtot[1] + extraresults[i].cat_bwtot[2];
        int ec1 = extraresults[i].cat_bwcor[1] + extraresults[i].cat_bwcor[2];
        float precision = (ec1*100.00)/et1;
        fout << "Extra 1 common: Total " << et1 << " Correct " << ec1 << " Percentage " << precision << endl;
    }

    fout << "Aggregate\n";
    for(int i = 0; i < 5; i++){
        fout << avgP[i]/num_langs << " | ";
    }
}

void getUsed(set<pair<string, string>> &MUSE, set<pair<string, string>> &Test, string used, int num_pairs, string l1, string l2)
{
    set<string> POS = {"noun", "verb", "adjective", "adverb", "numeral"}; //modify - POS of input data to consider (for BWR)
    string input_file;
    ifstream file_list;
    file_list.open(used);
    set<string> WordsI1, WordsI2;
    for(int i = 0; i < num_pairs; i++){
        file_list >> input_file;
        ifstream fin; fin.open(input_file);
        string line;
        while(getline(fin, line)){
            istringstream iss(line);
            string uw, up, ul, vw, vp, vl;
            getline(iss, uw, '\t'); getline(iss, up, '\t'); getline(iss, ul, '\t');
            getline(iss, vw, '\t'); getline(iss, vp, '\t'); getline(iss, vl, '\t');
            if(POS.find(up)==POS.end()) continue;
            
            if(ul==l1){
                WordsI1.insert(uw);
            }
            else if(ul==l2){
                WordsI2.insert(uw);
            }
            if(vl==l1){
                WordsI1.insert(vw);
            }
            else if(vl==l2){
                WordsI2.insert(vw);
            }
        }
    }

    for(auto p: MUSE){
        if(WordsI1.find(p.first) != WordsI1.end() && WordsI2.find(p.second) != WordsI2.end()){
            Test.insert(p);
        }
    }
}

void printRec(string outpath){
    ofstream fout; fout.open(outpath);
    float avgR = 0;
    float avgBWR = 0;

    for(int i = 0; i < num_langs; i++){
        fout << langlist[i].first << " " << langlist[i].second << endl;
        int Oet = recresults[i].tot, Oec = recresults[i].cor;
        float Orecall = (Oec*100.00)/Oet;
        avgR += Orecall;
        fout << "Overall:" << " Total " << Oet << " Correct " << Oec << " Percentage " << Orecall << endl;

        int Bet = recresults[i].bwtot, Bec = recresults[i].bwcor;
        float Brecall = (Bec*100.00)/Bet;
        avgBWR += Brecall;
        fout << "BW:" << " Total " << Bet << " Correct " << Bec << " Percentage " << Brecall << endl;
    }

    fout << "Aggregate\n";
    fout << "Overall: " << avgR/5 << " | BW: " << avgBWR/5 << endl;
}

int main(int argc, char*argv[])
{
    if(argc!=5){
        cerr << "Usage: " << argv[0] <<  " <path-to-muse-data-folder> <path-to-results-folder> <output-precision-file-path>";
        cerr << " <output-recall-file-path>" << endl;
        cerr << "Example: " << argv[0] << " LangData/MUSE/InRDF/ Results/Expts/Final-N20-26-MUSED/Analysis/";
        cerr << " Results/Expts/Final-N20-26-MUSED/MUSE5extra1BW.txt Results/Expts/Final-N20-26-MUSED/MUSERecall.txt" << endl;
        return 0;
    }
    string MUSEprefix = argv[1];
    string predprefix = argv[2];
    string outpath = argv[3];
    string recpath = argv[4];
    for(int i = 0; i < num_langs; i++){
        set<pair<string, string>> MUSE;
        set<string> MUSEwords;
        string lang = langlist[i].first + "-" + langlist[i].second;
        loadMUSE(MUSE, MUSEwords, MUSEprefix + lang + ".txt");

        //Precision
        string extraprefix = predprefix + lang + "/extra";
        vector<pair<string, string>> extraentries[4];
        for(int j = 0; j < 4; j++){
            string path = extraprefix + to_string(j) + ".txt";
            loadPred(extraentries[j], path, langlist[i].first, langlist[i].second);

            for(auto e: extraentries[j]){
                extraresults[i].tot++; extraresults[i].cat_tot[j]++;
                if(MUSE.find(e)!=MUSE.end()){
                    extraresults[i].cor++; extraresults[i].cat_cor[j]++;
                }
                if(MUSEwords.find(e.first) != MUSEwords.end() && MUSEwords.find(e.second) != MUSEwords.end()){
                    extraresults[i].cat_bwtot[j]++; extraresults[i].bwtot++;
                    if(MUSE.find(e)!=MUSE.end()){
                        extraresults[i].cat_bwcor[j]++; extraresults[i].bwcor++;
                    }
                }
            }
        }

        //Recall
        set<pair<string, string>> Test;
        //modify - path passed below contains paths to bidixes of used language pairs as input data, to compute BWR
        getUsed(MUSE, Test, "sampleconfigs/Use26Gen1-Paths/" + lang + "/langpathlist.txt", 26, langlist[i].first, langlist[i].second);
        string predpath = predprefix + lang + "/predictions.txt";
        vector<pair<string, string>> predictions;
        loadPred(predictions, predpath, langlist[i].first, langlist[i].second);
        
        recresults[i].tot = MUSE.size(); recresults[i].bwtot = Test.size();
        for(auto e: predictions){
            if(MUSE.find(e)!=MUSE.end()){
                recresults[i].cor++;
            }
            if(Test.find(e)!=Test.end()){
                recresults[i].bwcor++;
            }
        }

    }
    printRec(recpath);
    printFull(outpath);
}