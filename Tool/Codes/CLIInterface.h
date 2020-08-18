#ifndef GSOCAPERTIUM2020_CLIINTERFACE_H
#define GSOCAPERTIUM2020_CLIINTERFACE_H

#include <vector>
#include <map>
#include <string>
#include <chrono>

using namespace std::chrono;

//Used to time code
struct Stopwatch{
    decltype(high_resolution_clock::now()) sTime;
    decltype(high_resolution_clock::now()) eTime;
    inline void start(){ //start the timer
        sTime = high_resolution_clock::now();
    }
    inline void end(){ //end the timer
        eTime = high_resolution_clock::now();
    }
    long long report(){ //calculate time taken
        return duration_cast<microseconds>(eTime - sTime).count();
    }
    void log(string s=""){ //output the time taken
        cout<<"Time taken "<<s<<" : "<<report()/1e6<<" seconds"<<endl;
    }
};

class GenPoss{
    int num_pred;
    vector<string> l1, l2;
    vector<vector<pair<string, string>>> lI;
    vector<Config> H;
    map<string, int> POS_to_config;
    InfoSets reqd;
public:
    void SetDefaults();
    bool GetHyperparameters(ifstream &file_H);
    bool RunWords(string &exptno, ifstream &file_W);
    bool RunLangs(string &exptno, ifstream &file_L);
};

class GenPred {
    int num_folders;
    vector<string> foldernames;
public:
    void Run(ifstream &fin, string &exptname, float &confidence);
};


#endif //GSOCAPERTIUM2020_CLIINTERFACE_H
