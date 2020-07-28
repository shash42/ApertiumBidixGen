#include "Graph.cpp"
#include "Biconnected.cpp"
#include "DensityAlgo.cpp"
#include "Compare.cpp"
#include "CountByPOS.cpp"
#include "callers.cpp"

#include<chrono>
#include<experimental/filesystem>
#include<algorithm>

using namespace std::chrono;
namespace fs = std::experimental::filesystem;

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

class Generate{
    int num_pred;
    vector<string> l1, l2;
    vector<vector<pair<string, string>>> lI;
    vector<Config> H;
    map<string, int> POS_to_config;
public:
    void SetDefaults();
    void GetHyperparameters();
    void GetLangs();
    void Run(string &exptno);
};
void Generate::SetDefaults() {
    Config defaultconfig, transitiveconfig;
    H.push_back(defaultconfig);
    transitiveconfig.transitive = 2;
    transitiveconfig.context_depth = 3;
    H.push_back(transitiveconfig);
    POS_to_config["properNoun"] = 1; POS_to_config["numeral"] = 1;
}
void Generate::GetHyperparameters() {
    char choice = 'I';
    while(choice == 'I'){
        cout << "Do you want to enter your own hyperparameter configurations? [Y/N]: ";
        cin >> choice;
        choice = toupper(choice);
        if(choice != 'Y' && choice != 'N'){
            choice = 'I';
            cout << "Invalid Choice." << endl;
        }
    }
    if(choice == 'N'){
        return;
    }

    string H_filename;
    do {
        cout << "Enter Hyperparameter file path relative to Interface.cpp run location" << endl;
        cin >> H_filename;
        if(!fs::exists(H_filename)){
            cout << "File not found" << endl;
        }
    }
    while(!fs::exists(H_filename));
    ifstream file_H;   file_H.open(H_filename);

    string takein;
    file_H >> takein;
    int num_H = 0;
    Config defaultconfig;   H.push_back(defaultconfig);
    while(takein != "POS_To_Hyperparameter_Map"){
        if(takein=="end"){
            file_H >> takein;
            if(takein != "POS_To_Hyperparameter_Map"){
                H.push_back(defaultconfig);
                num_H++;
            }
        }
        else if(takein=="transitive"){
            string equalsym; file_H >> equalsym;
            int x; file_H >> x;
            H[num_H].transitive = x;
        }
        else if(takein=="context_depth"){
            string equalsym; file_H >> equalsym;
            int x; file_H >> x;
            H[num_H].context_depth = x;
        }
        else if(takein=="max_cycle_length"){
            string equalsym; file_H >> equalsym;
            int x; file_H >> x;
            H[num_H].context_depth = x;
        }
        else if(takein=="large_cutoff"){
            string equalsym; file_H >> equalsym;
            int x; file_H >> x;
            H[num_H].large_cutoff = x;
        }
        else if(takein=="large_min_cyc_len"){
            string equalsym; file_H >> equalsym;
            int x; file_H >> x;
            H[num_H].large_min_cyc_len = x;
        }
        else if(takein=="small_min_cyc_len"){
            string equalsym; file_H >> equalsym;
            int x; file_H >> x;
            H[num_H].small_min_cyc_len = x;
        }
        else if(takein=="deg_gt2_multiplier"){
            string equalsym; file_H >> equalsym;
            float x; file_H >> x;
            H[num_H].deg_gt2_multiplier = x;
        }
        else if(takein=="conf_threshold"){
            string equalsym; file_H >> equalsym;
            float x; file_H >> x;
            H[num_H].conf_threshold = x;
        }
        else{
            cout << "Invalid Hyperparameter Name. Try Again!";
            exit(7);
        }
    }

    cout << H.size() << " Hyperparameter Configurations Retrieved" << endl;

    file_H >> takein;
    while(takein != "ENDOFFILE"){
        string equalsym; file_H >> equalsym;
        int x; file_H >> x;
        POS_to_config[takein] = x;
    }
}
void Generate::GetLangs() {
    string L_filename;
    do {
        cout << "Enter language list file path relative to run location" << endl;
        cin >> L_filename;
        if(!fs::exists(L_filename)){
            cout << "File not found" << endl;
        }
    }
    while(!fs::exists(L_filename));

    ifstream file_L; file_L.open(L_filename);
    file_L >> num_pred;
    l1.resize(num_pred); l2.resize(num_pred); lI.resize(num_pred);
    for(int i = 0; i < num_pred; i++){
        file_L >> l1[i] >> l2[i];
        int num_lang; file_L >> num_lang;
        lI[i].resize(num_lang);
        for(int j = 0; j < num_lang; j++){
            file_L >> lI[i][j].first;
        }
        for(int j = 0; j < num_lang; j++){
            file_L >> lI[i][j].second;
        }
    }
}
void Generate::Run(string &exptno) {
    InfoSets reqd;
    reqd.infolist.push_back("lang"); reqd.infolist.push_back("pos"); reqd.infolist.push_back("word_rep");

    for (int i = 0; i < num_pred; i++) {
        cout << "Language No.: " << i + 1 << endl;
        Stopwatch timer;
        string lp1 = l1[i] + "-" + l2[i], lp2 = l2[i] + "-" + l1[i]; //language pair to get predictions for
        string dirpath = "../Results/Expts/" + exptno + "/Analysis/" + lp1;
        fs::create_directory(dirpath);
        string prefix = "rem_" + lp1; //output file

        //cin >> word;
        Graph G;
        runPairs(G, lI[i]); //load pairs into graph(object, langpairindex to ignore)
        cout << "Loaded" << endl;
        timer.start(); // start timer
        map<string, Graph> predicted; //string stores language pair and maps it to a graph

        //precompute biconnected components and then run
        reqd.condOR["lang"].clear();
        reqd.condOR["lang"].insert(l1[i]);
        reqd.condOR["lang"].insert(l2[i]);
        int new_trans = runBicomp(G, H, POS_to_config, prefix, predicted, exptno, lp1, lp2, reqd);
        cout << new_trans << endl;
        timer.end();
        timer.log();
    }
}

void CreateDir(string &exptno){
    fs::create_directory("../Results");
    fs::create_directory("../Results/Expts");
    fs::create_directory("../Results/Expts/" + exptno);
    fs::create_directory("../Results/Expts/" + exptno + "/Analysis");
}

int main(){
    string exptno;
    cout << "Enter the Experiment Number: ";
    cin >> exptno;
    CreateDir(exptno);
    Generate Predictor;
    Predictor.SetDefaults();
    Predictor.GetHyperparameters();
    Predictor.GetLangs();
    Predictor.Run(exptno);
    return 0;
}