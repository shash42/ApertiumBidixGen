#include "Graph.cpp"
#include "Biconnected.cpp"
#include "DensityAlgo.cpp"
#include "callers.cpp"
#include "PosstoPred.cpp"

#include<chrono>
#include "filesystem.hpp" 
#include<algorithm>

using namespace std::chrono;
namespace fs = std::filesystem;

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

void CreateDir(string &exptno){
    fs::create_directory("../Results");
    fs::create_directory("../Results/Expts");
    if(fs::exists("../Results/Expts/" + exptno)){
        char choice = 'I';
        while(choice == 'I'){
            cout << "Experiment " << exptno << " already exists. Are you sure you want to overwrite it? [Y/N]: ";
            cin >> choice;
            choice = toupper(choice);
            if(choice != 'Y' && choice != 'N'){
                cout << "Invalid choice! Try again." << endl;
                choice = 'I';
            }
        }
        if(choice=='N') {
            cout << "Please run the program again!" << endl;
            exit(7);
        }
    }
    fs::create_directory("../Results/Expts/" + exptno);
    fs::create_directory("../Results/Expts/" + exptno + "/Analysis");
}

class Generate{
    int num_pred;
    vector<string> l1, l2;
    vector<vector<pair<string, string>>> lI;
    vector<Config> H;
    map<string, int> POS_to_config;
    InfoSets reqd;
public:
    void SetDefaults();
    void GetHyperparameters();
    void RunWords(string &exptno);
    void RunLangs(string &exptno);
};

void Generate::SetDefaults() {
    Config defaultconfig, transitiveconfig;
    H.push_back(defaultconfig);
    transitiveconfig.transitive = 2;
    transitiveconfig.context_depth = 3;
    H.push_back(transitiveconfig);
    POS_to_config["properNoun"] = 1; POS_to_config["numeral"] = 1;
    reqd.infolist.push_back("lang"); reqd.infolist.push_back("pos"); reqd.infolist.push_back("word_rep");
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

    H.clear(); //remove defaults
    POS_to_config.clear();
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

    int num_H = 0;
    Config defaultconfig;   H.push_back(defaultconfig);
    string takein;
    file_H >> takein;
    while(takein != "POS_To_Hyperparameter_Map"){
        if(takein=="end"){
            file_H >> takein;
            if(takein != "POS_To_Hyperparameter_Map"){
                H.push_back(defaultconfig);
                num_H++;
            }
            continue;
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
            H[num_H].max_cycle_length = x;
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
            cout << "Invalid Hyperparameter Name " << takein << ". Try Again!";
            exit(7);
        }
        file_H >> takein;
    }

    cout << H.size() << " Hyperparameter Configurations Retrieved" << endl;

    file_H >> takein;
    while(takein != "ENDOFFILE"){
        string equalsym; file_H >> equalsym;
        int x; file_H >> x;
        POS_to_config[takein] = x;
        file_H >> takein;
    }
    file_H.close();
}

void Generate::RunWords(string &exptno) {
    num_pred = 1;
    lI.resize(num_pred);
    string W_filename;
    do {
        cout << "Enter word, language list file path relative to run location" << endl;
        cin >> W_filename;
        if(!fs::exists(W_filename)){
            cout << "File not found" << endl;
        }
    }
    while(!fs::exists(W_filename));
    ifstream file_W; file_W.open(W_filename);

    string outfilename;
    getline(file_W, outfilename);
    int numWords;
    string numwordline;
    getline(file_W, numwordline);
    stringstream(numwordline) >> numWords;
    for(int i = 0; i < numWords; i++){
        string word;
        getline(file_W, word);
        reqd.condOR["word_rep"].insert(word);
    }
    int num_lang; file_W >> num_lang;
    lI[0].resize(num_lang);
    for(int j = 0; j < num_lang; j++){
        file_W >> lI[0][j].first;
    }
    for(int j = 0; j < num_lang; j++){
        file_W >> lI[0][j].second;
    }
    file_W.close();
    cout << "Word file read, " << numWords << " words received! " << endl;
    cout << outfilename << endl;
    string lp2 = "Wont Be Used";
    Graph G;
    runPairs(G, lI[0]); //load pairs into graph(object, langpairindex to ignore)
    cout << "Loaded" << endl;
    Stopwatch timer;
    timer.start(); // start timer
    map<string, Graph> predicted; //string stores language pair and maps it to a graph
    reqd.condOR["lang"].clear();
    string dirpath = "../Results/Expts/" + exptno + "/Analysis/" + outfilename;
    fs::create_directory(dirpath);
    int new_trans = runDirectWords(G, H, POS_to_config, predicted, exptno, outfilename, lp2, reqd);
    //cout << new_trans << endl;
    timer.end();
    timer.log();
}

void Generate::RunLangs(string &exptno) {
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
    file_L.close();

    for (int i = 0; i < num_pred; i++) {
        cout << "Language No.: " << i + 1 << endl;
        string lp1 = l1[i] + "-" + l2[i], lp2 = l2[i] + "-" + l1[i]; //language pair to get predictions for
        string dirpath = "../Results/Expts/" + exptno + "/Analysis/" + lp1;
        fs::create_directory(dirpath);

        Graph G;
        runPairs(G, lI[i]); //load pairs into graph(object, langpairindex to ignore)
        cout << "Loaded" << endl;
        Stopwatch timer;
        timer.start(); // start timer
        map<string, Graph> predicted; //string stores language pair and maps it to a graph

        //precompute biconnected components and then run
        reqd.condOR["lang"].clear();
        bool fixedlp = false;
        if(l1[i] != "NoFix"){
            reqd.condOR["lang"].insert(l1[i]);
            reqd.condOR["lang"].insert(l2[i]);
            fixedlp = true;
        }
        int new_trans = runBicompLangs(G, H, POS_to_config, fixedlp, predicted, exptno, lp1, lp2, reqd);
       // cout << new_trans << endl;
        timer.end();
        timer.log();
    }
}

int main(){
    string exptno;
    cout << "Enter the Experiment Number: ";
    cin >> exptno;
    CreateDir(exptno);

    cout << "Do you want to generate new possibilities (1) or convert an existing possibilities file to predictions (2) ?: ";
    int choice;
    cin >> choice;
    if(choice==1){
        Generate Predictor;
        Predictor.SetDefaults();
        Predictor.GetHyperparameters();

        cout << "Do you want to generate translations for some words (1) or get all translations for some languages (2) ?: ";
        int WLchoice; cin >> WLchoice;
        if(WLchoice==1){
            Predictor.RunWords(exptno);
        }
        else if(WLchoice==2){
            Predictor.RunLangs(exptno);
        }
    }
    else if(choice==2){
        cout << "Enter your required confidence threshold: " << endl;
        float threshold; cin >> threshold;
        cout << "How many folders (inside the experiment's folder) do you want to generate predictions for?: ";
        int numfolders; cin >> numfolders;
        cout << "Enter 1 foldername per line" << endl;
        vector<string> foldernames;
        for(int i = 0; i < numfolders; i++){
            string temp;
            while(temp.empty()){
                getline(cin, temp);
                string dirpath; dirpath = "../Results/Expts/" + exptno + "/Analysis/" + temp;
                if(!fs::exists(dirpath)){
                    cout << "Invalid foldername, " << to_string(numfolders - i) << " left. Try again!";
                    temp.clear();
                }
            }
            foldernames.push_back(temp);
        }
        convert(exptno, threshold, numfolders, foldernames);
    }
    return 0;
}
