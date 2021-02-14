//main CLI class

#ifndef GSOCAPERTIUM2020_CLI_H
#define GSOCAPERTIUM2020_CLI_H

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
        cerr<<"Time taken "<<s<<" : "<<report()/1e6<<" seconds"<<endl;
    }
};

class HpCheck{
    int num_H, permIntL = -1, permIntR = -1, recIntL = -1, recIntR = -1;
    float permFloatL = -1, permFloatR = -1, recFloatL = -1, recFloatR = -1, Hprecision = 1e-6;
public:
    HpCheck(int _num_H, int _permIntL, int _permIntR, int _recIntL, int _recIntR){
        num_H = _num_H;
        permIntL = _permIntL; permIntR = _permIntR;
        recIntL = _recIntL; recIntR = _recIntR;
    }
    HpCheck(int _num_H, float _permFloatL, float _permFloatR, float _recFloatL, float _recFloatR){
        num_H = _num_H;
        permFloatL = _permFloatL; permFloatR = _permFloatR;
        recFloatL = _recFloatL; recFloatR = _recFloatR;
    }

    bool FailCheck(ifstream &file, string &parameter);
    bool RangeCheckInt(ifstream &file, int x, string &parameter);
    bool RangeCheckFloat(ifstream &file, float x, string &parameter);
};
class GenPoss{
    const int num_H_Cap = 20, num_PperH_Cap = 15, num_W_Cap = 10000, num_B_Cap = 100, num_LPGen_Cap = 50;
    int num_pred;
    vector<string> l1, l2;
    vector<vector<pair<string, string>>> lI;
    vector<Config> H;
    map<string, int> POS_to_config;
    InfoSets reqd;
public:
    void SetDefaults();
    bool GetHyperparameters(ifstream &file_H);
    bool RunWords(string &exptno, ifstream &file_W, string &input_folder, bool diffpos);
    bool RunLangs(string &exptno, ifstream &file_L, string &input_folder, bool diffpos);
};

class GenPred {
    int num_folders;
    vector<string> foldernames;
public:
    void Run(ifstream &fin, string &exptname, float &confidence, bool bidixoutput);
};


#endif //GSOCAPERTIUM2020_CLI_H
