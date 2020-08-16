#include "Graph.cpp"
#include "callers.cpp"
#include "PosstoPred.cpp"
#include "Convert2to3.cpp"
#include "CLIInterface.h"

#include "filesystem.hpp"
#include<algorithm>
#include<getopt.h>

namespace fs = std::filesystem;

void CreateDir(string &exptname){
    fs::create_directory("../Results");
    fs::create_directory("../Results/Expts");
    fs::create_directory("../Results/Expts/" + exptname);
    fs::create_directory("../Results/Expts/" + exptname + "/Analysis");
}

void GenPoss::SetDefaults() {
    Config defaultconfig, transitiveconfig;
    H.push_back(defaultconfig);
    transitiveconfig.transitive = 2;
    transitiveconfig.context_depth = 3;
    H.push_back(transitiveconfig);
    POS_to_config["properNoun"] = 1; POS_to_config["numeral"] = 1;
    reqd.infolist.push_back("lang"); reqd.infolist.push_back("pos"); reqd.infolist.push_back("word_rep");
}

bool GenPoss::GetHyperparameters(ifstream &file_H) {

    H.clear(); //remove defaults
    POS_to_config.clear();
    string H_filename;
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
            cerr << "Invalid Hyperparameter Name " << takein << endl;
            return false;
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
    return true;
}

bool GenPoss::RunWords(string &exptno, ifstream &file_W) {
    num_pred = 1;
    lI.resize(num_pred);

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
    //cout << outfilename << endl;
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
    return true;
}

bool GenPoss::RunLangs(string &exptno, ifstream &file_L) {
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
    return true;
}

void GenPred::Run(ifstream &fin, string &exptname, float &confidence) {
    cout << "Converting..." << endl;
    foldernames.clear();
    string tempnum;
    getline(fin, tempnum);
    num_folders = stoi(tempnum);

    for(int i = 0; i < num_folders; i++){
        string temp;
        getline(fin, temp);
        string dirpath; dirpath = "../Results/Expts/" + exptname + "/Analysis/" + temp;
        if(!fs::exists(dirpath)){
            cerr << "Invalid foldername, " << to_string(num_folders - i) << " folders left. Try again!";
            return;
        }
        foldernames.push_back(temp);
    }

    convert(exptname, confidence, num_folders, foldernames);
}

int main(int argc, char *argv[]){

    string exptname, folder_filename, hp_filename, word_filename, lang_filename;
    float confidence = -1;
    bool pred, poss;
    int c;
    while(1){
        int option_index = 0;
        static struct option long_options[] =
                {
                        {"expt_name",     required_argument, NULL,  'e'},
                        {"possible_translations",  no_argument,       NULL,  'p'},
                        {"get_predictions",  no_argument, NULL,  'g'},
                        {"confidence", optional_argument,       NULL,  'c'},
                        {"folder_file",  required_argument, NULL,  'f'},
                        {"hyperparameter_file",    optional_argument, NULL,  'h'},
                        {"word_file",    required_argument, NULL,  'w'},
                        {"language_file",    required_argument, NULL,  'l'},
                        {NULL,      0,                 NULL,    0}
                };

        c = getopt_long(argc, argv, ":e:pgc:f:h:w:l:", long_options, &option_index);
        if (c == -1)
            break;

        switch (c)
        {
            case 'e':
                exptname = optarg;
                break;
            case 'p':
                poss=true;
                break;
            case 'g':
                pred=true;
                break;
            case 'c':
                if(optarg) confidence=stof(optarg);
                break;
            case 'f':
                folder_filename=optarg;
                break;
            case 'h':
                if(optarg) hp_filename = optarg;
                break;
            case 'w':
                word_filename = optarg;
                break;
            case 'l':
                lang_filename = optarg;
                break;
            default:
                cerr << optarg << " Unknown!" << endl;
        }
    }

    if(exptname.empty()){
        cerr << "Enter valid experiment name" << endl;
        return 0;
    }
    CreateDir(exptname);

    if(poss){
        GenPoss Predictor;
        Predictor.SetDefaults();
        if(!hp_filename.empty()){
            ifstream hp_file; hp_file.open(hp_filename);
            bool valid_hp = Predictor.GetHyperparameters(hp_file);
            if(!valid_hp){
                cerr << "Please Try Again" << endl;
                return 0;
            }
        }
        if(word_filename.length() && lang_filename.length()){
            cerr << "Both word and language pair modes are not possible together. Please try again!";
            return 0;
        }
        if(word_filename.empty() && lang_filename.empty()){
            cerr << "Neither word file nor language pair file provided. Please choose a mode and try again!";
            return 0;
        }
        if(!word_filename.empty()){
            cout << word_filename << endl;
            if(!fs::exists(word_filename)){
                cerr << "The word generation information file provided does not exist, please try again";
                return 0;
            }
            ifstream word_file; word_file.open(word_filename);
            bool valid_word = Predictor.RunWords(exptname, word_file);
            if(!valid_word){
                cerr << "Please Try Again" << endl;
                return 0;
            }
        }
        else if(!lang_filename.empty()){
            if(!fs::exists(lang_filename)){
                cerr << "The language-pair generation information file provided does not exist, please try again";
                return 0;
            }
            ifstream lang_file; lang_file.open(lang_filename);
            bool valid_lang = Predictor.RunLangs(exptname, lang_file);
            if(!valid_lang){
                cerr << "Please Try Again" << endl;
                return 0;
            }
        }
    }
    else if(pred){
        if(confidence < 0 || confidence > 1){
            cerr << "Invalid confidence score, please try again";
            return 0;
        }
        if(!fs::exists(folder_filename)){
            cerr << "The folder information file provided does not exist, please try again";
            return 0;
        }
        ifstream folder_data; folder_data.open(folder_filename);
        GenPred Predictor;
        Predictor.Run(folder_data, exptname, confidence);
    }
    else
    {
        cerr << "You must provide one of --find_possibilities (-p) or --get_predictions (-g)";
        return 0;
    }
    return 0;
}
