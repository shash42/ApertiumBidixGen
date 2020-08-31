#include "Graph.cpp"
#include "callers.cpp"
#include "PosstoPred.cpp"
#include "Convert2to3.cpp"
#include "CLI.h"

#include "filesystem.hpp"
#include<algorithm>
#include<getopt.h>

namespace fs = std::filesystem;

void CreateDir(string &exptname){
    fs::create_directory("Results");
    fs::create_directory("Results/Expts");
    fs::create_directory("Results/Expts/" + exptname);
    fs::create_directory("Results/Expts/" + exptname + "/Analysis");
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

bool HpCheck::FailCheck(ifstream &file, string &parameter) {
    if(file.fail()){
        file.clear();
        cerr << "Invalid input, "<< " hyperparameter set " << num_H << " - " << parameter << " not assigned!\n";
        file.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
    return true;
}

bool HpCheck::RangeCheckInt(ifstream &file, int x, string &parameter) {
    //cerr << parameter << " " << x << endl;
    if(x>=permIntL && x<=permIntR){
        return true;
    }
    cerr << "Hyperparameter set " << num_H << " - Parameter: " << parameter << " not assigned!" << endl;
    cerr << "Permissible range for " << parameter <<  "is [" << permIntL <<", " << permIntR << "]" << endl;
    if(recIntL >= 0 && recIntR >= 0) cerr << "Recommended range for " << parameter <<  "is [" << recIntL <<", " << recIntR << "]" << endl;
    file.ignore(numeric_limits<streamsize>::max(), '\n');
    return false;
}

bool HpCheck::RangeCheckFloat(ifstream &file, float x, string &parameter) {
    //cerr << parameter << " " << x << endl;
    if(x>=permFloatL - Hprecision && x<permFloatR + Hprecision){
        return true;
    }
    cerr << "Hyperparameter set " << num_H << " - Parameter: " << parameter << " not assigned!" << endl;
    cerr << "Permissible range for " << parameter <<  "is [" << permFloatL <<", " << permFloatR << "]" << endl;
    if(recFloatL > -Hprecision && recFloatR > -Hprecision) cerr << "Recommended range for " << parameter <<  "is [" << recFloatL <<", " << recFloatR << "]" << endl;
    file.ignore(numeric_limits<streamsize>::max(), '\n');
    return false;
}

bool equalcheck(string &equalsym, int &num_H, string &takein, ifstream &file_H){
    if(equalsym!="="){
        cerr << "Formatting error detected in hyperparameter set " << num_H << ", " << takein << " not assigned!" << endl;
        file_H.ignore(numeric_limits<streamsize>::max(), '\n');
        file_H >> takein; //required for next iteration as after this it will skip this iteration of the loop
        transform(takein.begin(), takein.end(), takein.begin(), ::tolower); //convert to lower case for ease of use
        return false;
    }
    else return true;
}

void length3codecheck(string &langcode)
{
    if(langcode.length()!=3){
        cerr << "Warning: You have not used 3 digit lang-codes in " << langcode << endl; //don't break as it may be intended
        cerr << "It might still work if you are sure this is intended, but can otherwise lead to unexpected behaviour" << endl;
    }
}
bool GenPoss::GetHyperparameters(ifstream &file_H) {

    H.clear(); //remove defaults
    POS_to_config.clear();
    string H_filename;
    int num_H = 0;
    Config defaultconfig;   H.push_back(defaultconfig);
    string takein;
    file_H >> takein;
    transform(takein.begin(), takein.end(), takein.begin(), ::tolower); //convert to lower case for ease of use
    int perset = 0;
    while(takein != "pos_to_hyperparameter_map"){
        //cerr << takein << endl;
        perset++;
        if(takein=="end"){
            perset = 0;
            file_H >> takein;

            transform(takein.begin(), takein.end(), takein.begin(), ::tolower); //convert to lower case for ease of use
            if(takein != "pos_to_hyperparameter_map"){
                H.push_back(defaultconfig);
                num_H++;
            }
            if(num_H > num_H_Cap){
                cerr << "Too many hyperparameter sets received, current upper-limit is" << num_H_Cap << endl;
                cerr << "Maximum 1 hyperparameter set per POS can be used. Thus, more hyperparameter sets shouldn't be needed" << endl;
                cerr << "Possible causes: error in format like not adding POS_to_Hyperparameter_Map in the correct position in the config file" << endl;
                return false;
            }
            continue;
        }
        if(perset > num_PperH_Cap){
            cerr << "Too many parameters for this hyperparameter set were recieved." << endl;
            cerr << "Are you sure you didn't miss an \"end\" at the end of a hyperparameter set?" << endl;
            return false;
        }
        else if(takein=="transitive"){
            string equalsym; file_H >> equalsym;
            if(!equalcheck(equalsym, num_H, takein, file_H)){ //file_H >> takein is done inside so can directly jump to next iteration
                continue;
            }
            int x; file_H >> x;
            HpCheck HpChecker(num_H, (int) 0, (int) 2, -1, -1);
            if(HpChecker.FailCheck(file_H, takein) && HpChecker.RangeCheckInt(file_H, x, takein)){
                H[num_H].transitive = x;
            }
        }
        else if(takein=="context_depth"){
            string equalsym; file_H >> equalsym;
            if(!equalcheck(equalsym, num_H, takein, file_H)){ //file_H >> takein is done inside so can directly jump to next iteration
                continue;
            }
            int x; file_H >> x;
            HpCheck HpChecker(num_H, (int) 1, (int) 10, 2, 5);
            if(HpChecker.FailCheck(file_H, takein) && HpChecker.RangeCheckInt(file_H, x, takein)){
                H[num_H].context_depth = x;
            }
        }
        else if(takein=="max_cycle_length"){
            string equalsym; file_H >> equalsym;
            if(!equalcheck(equalsym, num_H, takein, file_H)){ //file_H >> takein is done inside so can directly jump to next iteration
                continue;
            }
            int x; file_H >> x;
            HpCheck HpChecker(num_H, (int) 4, (int) 12, 6, 9);
            if(HpChecker.FailCheck(file_H, takein) && HpChecker.RangeCheckInt(file_H, x, takein)){
                H[num_H].max_cycle_length = x;
            }
        }
        else if(takein=="large_cutoff"){
            string equalsym; file_H >> equalsym;
            if(!equalcheck(equalsym, num_H, takein, file_H)){ //file_H >> takein is done inside so can directly jump to next iteration
                continue;
            }
            int x; file_H >> x;
            HpCheck HpChecker(num_H, (int) 0, (int) 12, 3, 7);
            if(HpChecker.FailCheck(file_H, takein) && HpChecker.RangeCheckInt(file_H, x, takein)){
                H[num_H].large_cutoff = x;
            }
        }
        else if(takein=="large_min_cyc_len"){
            string equalsym; file_H >> equalsym;
            if(!equalcheck(equalsym, num_H, takein, file_H)){ //file_H >> takein is done inside so can directly jump to next iteration
                continue;
            }
            int x; file_H >> x;
            HpCheck HpChecker(num_H, (int) 0, (int) 10, 4, 6);
            if(HpChecker.FailCheck(file_H, takein) && HpChecker.RangeCheckInt(file_H, x, takein)){
                H[num_H].large_min_cyc_len = x;
            }
        }
        else if(takein=="small_min_cyc_len"){
            string equalsym; file_H >> equalsym;
            if(!equalcheck(equalsym, num_H, takein, file_H)){ //file_H >> takein is done inside so can directly jump to next iteration
                continue;
            }
            int x; file_H >> x;
            HpCheck HpChecker(num_H, (int) 0, (int) 8, 4, 5);
            if(HpChecker.FailCheck(file_H, takein) && HpChecker.RangeCheckInt(file_H, x, takein)){
                H[num_H].small_min_cyc_len = x;
            }
        }
        else if(takein=="deg_gt2_multiplier"){
            string equalsym; file_H >> equalsym;
            if(!equalcheck(equalsym, num_H, takein, file_H)){ //file_H >> takein is done inside so can directly jump to next iteration
                continue;
            }
            float x; file_H >> x;
            HpCheck HpChecker(num_H, (float) 0.5, (float) 3.0, (float) 1, (float) 1.5);
            if(HpChecker.FailCheck(file_H, takein) && HpChecker.RangeCheckFloat(file_H, x, takein)){
                H[num_H].deg_gt2_multiplier = x;
            }
        }
        else if(takein=="conf_threshold"){
            string equalsym; file_H >> equalsym;
            if(!equalcheck(equalsym, num_H, takein, file_H)){ //file_H >> takein is done inside so can directly jump to next iteration
                continue;
            }
            float x; file_H >> x;
            HpCheck HpChecker(num_H, (float) 0.1, (float) 1.01, (float) 0.5, (float) 0.8);
            if(HpChecker.FailCheck(file_H, takein) && HpChecker.RangeCheckFloat(file_H, x, takein)){
                H[num_H].conf_threshold = x;
            }
        }
        else{
            cerr << "Invalid Hyperparameter Name " << takein << endl;
            file_H.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        file_H >> takein;
        transform(takein.begin(), takein.end(), takein.begin(), ::tolower); //convert to lower case for ease of use
    }

    cerr << H.size() << " Hyperparameter Configurations Retrieved" << endl;

    int iterations = 0;
    file_H >> takein;
    while(takein != "ENDOFFILE"){
        iterations++;

        set<string> POS = {"noun", "properNoun", "verb", "adverb", "adjective", "numeral", "pronoun", "preposition",
                           "punctuation", "article", "determiner", "conjunction", "particle"};
        if(POS.find(takein)==POS.end()){
            cerr << takein << " is not among the usable POS. This line has been ignored." << endl;
            file_H.ignore(numeric_limits<streamsize>::max(), '\n');
            file_H >> takein;
            continue;
        }
        if(iterations > num_H_Cap){
            cerr << "Too many POS-Hyperparameter mappings received, there is currently an upper-limit of" << num_H_Cap << endl;
            cerr << "Maximum 1 hyperparameter set per POS can be used. Thus, more mappings shouldn't be needed" << endl;
            cerr << "Possible causes: error in format like not ENDOFFILE in the correct position in the config file" << endl;
            return false;
        }

        string equalsym; file_H >> equalsym;
        if(!equalcheck(equalsym, num_H, takein, file_H)){ //file_H >> takein is done inside so can directly jump to next iteration
            continue;
        }
        int x; file_H >> x;
        if(file_H.fail()){
            file_H.clear();
            cerr << "Mapping number " << iterations << " provided is in an invalid format." << endl;
            return false;
        }
        else if(x<0 || x>num_H){
            cerr << "POS mapped to invalid hyperparameter " << x << " as " << num_H << " hyperparameter configs were provided" << endl;
            return false;
        }
        else{
            POS_to_config[takein] = x;
        }
        file_H >> takein;
    }
    file_H.close();
    return true;
}

bool GenPoss::RunWords(string &exptname, ifstream &file_W, string &input_folder, bool diffpos) {
    num_pred = 1;
    lI.resize(num_pred);

    string outfilename;
    getline(file_W, outfilename);
    int numWords;
    string numwordline;
    getline(file_W, numwordline);
    stringstream numwordstream = stringstream(numwordline);
    numwordstream >> numWords;
    if(numwordstream.fail()){
        numwordstream.clear();
        cerr << "Invalid input value for requirement: Number of Words" << endl;
        return false;
    }
    if(numWords > num_W_Cap){
        cerr << "Too many words. The current upper limit is " << num_W_Cap << endl;
        return false;
    }
    for(int i = 0; i < numWords; i++){
        string word;
        getline(file_W, word);
        reqd.condOR["word_rep"].insert(word);
    }
    int num_lang; file_W >> num_lang;
    if(file_W.fail()){
        file_W.clear();
        cerr << "Invalid input value for requirement: Number of Language Pairs to use" << endl;
        return false;
    }
    if(numWords > num_B_Cap){
        cerr << "Too many bidixes. The current upper limit is " << num_B_Cap << endl;
        return false;
    }
    lI[0].resize(num_lang);
    for(int j = 0; j < num_lang; j++){
        file_W >> lI[0][j].first;
        length3codecheck(lI[0][j].first);
    }
    for(int j = 0; j < num_lang; j++){
        file_W >> lI[0][j].second; //[TODO: Add checks for parsed existence of .first-.second here]
        length3codecheck(lI[0][j].second);
        if(!fs::exists(input_folder + lI[0][j].first + "-" + lI[0][j].second + ".txt")){
            cerr << "Data for pair " << lI[0][j].first << "-" << lI[0][j].second << " not found in provided input directory!" << endl;
            exit(1);
        }
    }
    file_W.close();
    cerr << "Word Config file read, " << numWords << " words received to generate output for! " << endl;
    //cout << outfilename << endl;
    string lp2 = "Wont Be Used";
    Graph G;
    runPairs(G, lI[0], input_folder); //load pairs into graph(object, langpairindex to ignore)
    cerr << "Loaded" << endl;
    Stopwatch timer;
    timer.start(); // start timer
    map<string, Graph> predicted; //string stores language pair and maps it to a graph
    reqd.condOR["lang"].clear();
    string dirpath = "Results/Expts/" + exptname + "/Analysis/" + outfilename;
    fs::create_directory(dirpath);
    int new_trans = runDirectWords(G, H, POS_to_config, predicted, exptname, outfilename, lp2, reqd, diffpos);
    //cerr << new_trans << endl;
    timer.end();
    timer.log();
    return true;
}

bool GenPoss::RunLangs(string &exptno, ifstream &file_L, string &input_folder, bool diffpos) {
    file_L >> num_pred;
    if(file_L.fail()){
        file_L.clear();
        cerr << "Invalid input value for requirement: Number of Language Pairs to use" << endl;
        return false;
    }
    if(num_pred > num_LPGen_Cap){
        cerr << "Too many required generation files. The current upper limit is " << num_LPGen_Cap << endl;
        return false;
    }
    l1.resize(num_pred); l2.resize(num_pred); lI.resize(num_pred);
    for(int i = 0; i < num_pred; i++){
        file_L >> l1[i] >> l2[i];
        if(l1[i]!="NoFix"){
            length3codecheck(l1[i]); length3codecheck(l2[i]);
        }
        int num_lang; file_L >> num_lang;
        if(file_L.fail()){
            file_L.clear();
            cerr << "Invalid input value for requirement: Number of Language Pairs to use" << endl;
            return false;
        }
        if(num_pred > num_B_Cap){
            cerr << "Too many bidixes. The current upper limit is " << num_B_Cap << endl;
            return false;
        }
        lI[i].resize(num_lang);
        for(int j = 0; j < num_lang; j++){
            file_L >> lI[i][j].first;
            length3codecheck(lI[i][j].first);
        }
        for(int j = 0; j < num_lang; j++){
            file_L >> lI[i][j].second;
            length3codecheck(lI[i][j].second);
            if(!fs::exists(input_folder + lI[i][j].first + "-" + lI[i][j].second + ".txt")){
                cerr << "Data for pair " << lI[i][j].first << "-" << lI[i][j].second << " not found in provided input directory!" << endl;
                exit(1);
            }
        }
    }
    file_L.close();

    for (int i = 0; i < num_pred; i++) {
        cerr << "Language No.: " << i + 1 << endl;
        string lp1 = l1[i] + "-" + l2[i], lp2 = l2[i] + "-" + l1[i]; //language pair to get predictions for
        string dirpath = "Results/Expts/" + exptno + "/Analysis/" + lp1;
        fs::create_directory(dirpath);

        Graph G;
        runPairs(G, lI[i], input_folder); //load pairs into graph(object, langpairindex to ignore)
        cerr << "Loaded" << endl;
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
        int new_trans = runBicompLangs(G, H, POS_to_config, fixedlp, predicted, exptno, lp1, lp2, reqd, diffpos);
        // cerr << new_trans << endl;
        timer.end();
        timer.log();
    }
    return true;
}

void GenPred::Run(ifstream &fin, string &exptname, float &confidence, bool bidixoutput) {
    foldernames.clear();
    fin >> num_folders;
    if(fin.fail()){
        cerr << "Invalid input type for number of folders, integer between [1, 100] required. Please try again\n";
        return;
    }
    if(num_folders < 1 || num_folders > 100){
        cerr << "Invalid value for number of folders, integer between [1, 100] required. Please try again\n";
        return;
    }
    fin.ignore(); //ignore the \n because after this we use getline
    for(int i = 0; i < num_folders; i++){
        string temp;
        getline(fin, temp);
        string dirpath; dirpath = "Results/Expts/" + exptname + "/Analysis/" + temp;
        if(temp.empty() || !fs::exists(dirpath)){
            cerr << "Invalid foldername on line " << i+1 << ", Please try again\n";
            return;
        }
        foldernames.push_back(temp);
    }
    cerr << "Converting..." << endl;
    convert(exptname, confidence, num_folders, foldernames, bidixoutput);
}

int main(int argc, char *argv[]){

    string about_message = "About:\n\tApertiumBidixGen helps automatically generate entries for Apertium bilingual dictionaries using data from existing"
                           " Apertium data. It allows a friendly experimental setup for the user to try different configurations and reach "
                           "upon what works best for the language-pair they are developing. More about the project at: https://github.com/shash42/ApertiumBidixGen \n\n";

    string usage_message = "Usage:\n"
                           "\t./bidixgen (-e | --expt-folder)<result_folder> (-p | --possible-translations)<input_folder>"
                           " [(-o | --own-hyperparameters)<hp_config>] (-w | --word-file)<word_file> (-n | --no-diff-pos)\n\n"
                           "\t./bidixgen (-e | --expt-folder)<result_folder> (-p | --possible-translations)<input_folder>"
                           " [(-o | --own-hyperparameters)<hp_config>] (-l | --language-file)<lang_file> (-n | --no-diff-pos)\n\n"
                           "\t./bidixgen (-e | --expt-folder)<result_folder> (-g | --get-predictions) (-f | --folder-file)<folder_config>"
                           " [(-c | --confidence)<score0-1>] [(-b | --bidix-output)]\n\n"
                           "For more information: Checkout the README.md downloaded locally or at https://github.com/shash42/ApertiumBidixGen/tree/master/Tool/README.md\n";

    if(argc<=1){
        cerr << about_message << endl << usage_message << endl;
        return 0;
    }
    string exptname, folder_filename, hp_filename, word_filename, lang_filename, lang_folder;
    float confidence = 0.65;
    bool pred=false, poss=false, diffpos=true, bidixoutput=false;
    int c;
    while(1){
        int option_index = 0;
        static struct option long_options[] =
                {
                        {"expt-folder",     required_argument, NULL,  'e'},
                        {"possible-translations",  required_argument,       NULL,  'p'},
                        {"get-predictions",  no_argument, NULL,  'g'},
                        {"confidence", optional_argument,       NULL,  'c'},
                        {"folder-file",  required_argument, NULL,  'f'},
                        {"no-diff-pos",      no_argument,     NULL,    'n'},
                        {"own-hyperparameters",    optional_argument, NULL,  'o'},
                        {"word-file",    required_argument, NULL,  'w'},
                        {"language-file",    required_argument, NULL,  'l'},
                        {"help",    no_argument, NULL,  'h'},
                        {"bidix-output",    no_argument, NULL,  'b'},
                        {NULL,      0,                 NULL,    0}
                };

        c = getopt_long_only(argc, argv, "he:p:gc:f:bo:w:l:n", long_options, &option_index);
        if (c == -1)
            break;

        switch (c)
        {
            case 'e':
                exptname = optarg;
                break;
            case 'p':
                poss=true;
                lang_folder = optarg;
                if(!fs::exists(lang_folder)){
                    cerr << "Input folder for language data provided (argument to -p) does not exist!\n";
                    exit(1);
                }
                if(lang_folder[lang_folder.length()-1]!='/') lang_folder += "/";
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
            case 'o':
                if(optarg) hp_filename = optarg;
                break;
            case 'w':
                word_filename = optarg;
                break;
            case 'l':
                lang_filename = optarg;
            case 'n':
                diffpos = false;
                break;
            case 'b':
                bidixoutput = true;
                break;
            case 'h':
                cerr << usage_message << endl;
                return 0;
            default:
                cerr << optarg << " Unknown!" << endl;
        }
    }

    if(exptname.empty()){
        cerr << "Experiment name not provided" << endl;
        exit(1);
    }
    for(int i = 0; i < exptname.length(); i++){
        
    }
    CreateDir(exptname);
    if(poss && pred){
        cerr << "Ambiguous command, both possible_translations and get_predicitons mode selected. Choose one!\n";
        exit(1);
    }
    if(poss){
        GenPoss Predictor;
        Predictor.SetDefaults();
        if(!hp_filename.empty()){
            if(!fs::exists(hp_filename)){
                cerr << "The hyperparameter information file provided does not exist, please try again\n";
                exit(1);
            }
            ifstream hp_file; hp_file.open(hp_filename);
            bool valid_hp = Predictor.GetHyperparameters(hp_file);
            if(!valid_hp){
                cerr << "Please Try Again" << endl;
                exit(1);
            }
        }
        else{
            cerr << "Using default hyperparameters!" << endl;
        }
        if(word_filename.length() && lang_filename.length()){
            cerr << "Both word and language pair modes are not possible together. Please try again!\n";
            exit(1);
        }
        if(word_filename.empty() && lang_filename.empty()){
            cerr << "Neither word file nor language pair file provided. Please choose a mode and try again!\n";
            exit(1);
        }
        if(!word_filename.empty()){
            if(!fs::exists(word_filename)){
                cerr << "The word generation information file provided does not exist, please try again\n";
                exit(1);
            }
            ifstream word_file; word_file.open(word_filename);
            bool valid_word = Predictor.RunWords(exptname, word_file, lang_folder, diffpos);
            if(!valid_word){
                cerr << "Please try again!\n";
                exit(1);
            }
        }
        else if(!lang_filename.empty()){
            if(!fs::exists(lang_filename)){
                cerr << "The language-pair generation information file provided does not exist, please try again\n";
                exit(1);
            }
            ifstream lang_file; lang_file.open(lang_filename);
            bool valid_lang = Predictor.RunLangs(exptname, lang_file, lang_folder, diffpos);
            if(!valid_lang){
                exit(1);
            }
        }
    }
    else if(pred){
        if(confidence < 0 || confidence > 1){
            cerr << "Invalid confidence score\n";
            exit(1);
        }
        if(!fs::exists(folder_filename)){
            cerr << "The folder information file provided does not exist, please try again\n";
            exit(1);
        }
        ifstream folder_data; folder_data.open(folder_filename);
        GenPred Predictor;
        Predictor.Run(folder_data, exptname, confidence, bidixoutput);
    }
    else
    {
        cerr << "You must provide one of --find_possibilities (-p) or --get_predictions (-g)\n";
        exit(1);
    }
    return 0;
}
