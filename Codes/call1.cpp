#include "Graph.cpp"
#include "Biconnected.cpp"
#include "DensityAlgo.cpp"
#include "Compare.cpp"
#include "CountByPOS.cpp"

#include<chrono>
#include<experimental/filesystem>

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

//get statistics on the biconnected components
void getStatsComps(Graph &G)
{
    Biconnected B;
    B.findBicomps(G);
    ofstream compfile;
    compfile.open("../Main/Analysis/TDbigcompfile-eng.txt");
    int subgraphnum = 0;
    int mxv = 0, mxe = 0, cntg100=0, cntg1000=0; //max size and count of comps with >100 or >1000 vertices

    for(auto SG: G.subGraphs)
    {
        if(SG.vertices.size()>=100) cntg100++; //count components >100 vertices
        if(SG.vertices.size()>=1000) cntg1000++; //count components >1000 vertices
        if(SG.vertices.size()<=2) continue; //if less than 2 (singleton vertex) ignore
        compfile << "Num vertices: " << SG.vertices.size() << endl;
        compfile << "Num edges: " << SG.num_edges << endl;
        mxv = max(mxv, (int) SG.vertices.size()); //to get max component size in vertices
        mxe = max(mxe, (int) SG.num_edges); //to get max component size in edges
        subgraphnum++;
        compfile << subgraphnum << endl;

        set<string> engwords;
        for(auto u: SG.vertices) //print only english words of the component
        {
            //if(u.rep.info["lang"]=="en") engwords.insert(u.rep.info["surface"]); //uncomment if dynamic wordData
            if(u.rep.lang=="en") engwords.insert(u.rep.surface);
        }
        for(auto u: engwords)
        {
            compfile << u << endl;
        }
        compfile << endl;
    }
    cout << mxv << " " << mxe << endl;
    cout << cntg100 << " " << cntg1000 << endl;
    compfile.close();
}

//load the pairs listed in LangData-List into the graph
void runPairs(Graph &G, int idxign)
{
    int num_pairs = 11; //number of pairs of languages.
    string input_file;
    ifstream file_list;
    file_list.open("../Main/LangData-List.txt"); //file with names of lang pairs
    ofstream fout;
    fout.open("../Main/Analysis/Tempfile.txt");
    int prev_nodes = 0, prev_edges = 0; //cumalative vertices/edges till now

    for(int i = 0; i < num_pairs; i++)
    {
        file_list >> input_file;
        if(i==idxign) continue; //ignore this language pair (incase of removal and generation tests)
        //cout << input_file << endl; //output current input file for tracking progress
        fout << input_file << endl;
        G.loadData(input_file, fout);
        fout << "Number of vertices: " << G.vertices.size() - prev_nodes << endl; //vertices in this file
        fout << "Number of edges: " << G.num_edges - prev_edges << endl; //edges in this file
        prev_nodes = G.vertices.size(); prev_edges = G.num_edges; //total number of nodes
        //cout << "done" << endl;
    }
}

//get predictions for a single language-pair
void predByLang(string &file_pref, map<string, Graph> &pred, string &lp1, string &lp2){
    ofstream summary; summary.open(file_pref + "pred-summary.txt");
    for(auto &langpair: pred){
        if(langpair.first!=lp1 && langpair.first!=lp2) continue; //if not required pair continue
        Graph &langG = langpair.second; //graph of the language pair
        summary << langpair.first << endl;
        summary << "Number of vertices: " << langG.vertices.size() << endl;
        summary << "Number of edges: " << langG.num_edges << endl;
        string file_name = file_pref + "predictions.txt";
        ofstream outfile; outfile.open(file_name);
        langG.printGraph(outfile);
        summary << endl; //blank line
    }
}

//Load a small word based context graph - FIX FUNCTION OR REMOVE
/*void runWords(Graph &G, string &word)
{
    string fin_name = "../Main/SampleWord/" + word + ".txt";
    ofstream fout;
    fout.open("../Main/Results/" + word + "_analysis.txt");
    G.loadData(fin_name, fout); //unidirectional data load
    cout << "loaded" << endl;
}*/

//Run after precomputing biconnected components
int runBicomp(Graph &G, vector<Config> configlist, map<string, int> &POS_to_config, string &prefix,
        map<string, Graph> &pred, string &exptno, string &lp1, string &lp2, InfoSets reqd)
{
    string fileout_name = "../Main/Results/" + prefix + "bicomp_out.txt";
    ofstream fout;
    fout.open(fileout_name); fout.close();
    int new_trans=0;

    Biconnected B; //object of biconnected computation class
    B.findBicomps(G);


    for(auto SG: G.subGraphs) //iterate over components and run density algo for each
    {
        DensityAlgo D = DensityAlgo(SG, configlist, POS_to_config);
        new_trans += D.run(fileout_name, pred, reqd); //append output to fileout_name
    }

    bool req_bicompless_run = false; //is a cycle-less (transitive) run required?
    InfoSets reqd_transitive = reqd; //additional restrictions for cycle-less run if any
    vector<Config> configlist_transitive = configlist;
    Graph tempG;
    DensityAlgo temp = DensityAlgo(tempG, configlist, POS_to_config);
    for(auto &w: G.vertices){ //iterate over vertices
        if(!temp.wordIsReq(w, reqd)) continue; //if vertex is not required by original reqd ignore
        if(configlist[POS_to_config[w.rep.pos]].transitive==2){ //otherwise if it's a non-bicomp transitive run POS
            req_bicompless_run = true; //then we need a bicompless run
            configlist_transitive[POS_to_config[w.rep.pos]].transitive=1; //set its trans_configlist to 1
            reqd_transitive.condOR["pos"].insert(w.rep.pos); //this POS is to be processed in the transitive run
        }
    }
    if(req_bicompless_run){ //if the bicconnected component-less run is required
        //initialize a transitive Density algo object
        DensityAlgo DTrans = DensityAlgo(G, configlist_transitive, POS_to_config);
        new_trans += DTrans.run(fileout_name, pred, reqd_transitive);
    }

    string pred_file_name = "../Main/Results/Expts/" + exptno + "/Analysis/" + lp1 + "/";
    predByLang(pred_file_name, pred, lp1, lp2);
    return new_trans;
}

//Run directly without precomputing biconnected components
int runDirect(Graph &G, vector<Config> &configlist, map<string, int> &POS_to_config, string &prefix,
              map<string, Graph> &pred, string &exptno, string &lp1, string &lp2, InfoSets reqd)
{
    string fileout_name = "../Main/Results/" + prefix + "_out.txt";
    ofstream fout;
    fout.open(fileout_name); fout.close();
    DensityAlgo D(G, configlist, POS_to_config);
    int new_trans = D.run(fileout_name, pred, reqd);
    string pred_file_name = "../Main/Results/Expts/" + exptno + "/Analysis/" + lp1 + "/";
    predByLang(pred_file_name, pred, lp1, lp2);
    return new_trans;
}

//Generates predictions for all language pairs by leaving out that pair and using others as input.
void genAll(string exptno, vector<Config> &configlist, map<string, int> &POS_to_config, InfoSets reqd){
    int numpairs = 11;
    string l1[] = {"en", "en", "fr", "fr", "eo", "eo", "eo", "eo", "oc", "oc", "oc"};
    string l2[] = {"es", "ca", "es", "ca", "fr", "ca", "en", "es", "ca", "es", "fr"};

    for(int i = 1; i < 11; i++){
        cout << "Language No.: " << i+1 << endl;
        Stopwatch timer;
        string lp1 = l1[i] + "-" + l2[i], lp2= l2[i] + "-" + l1[i]; //language pair to get predictions for
        string dirpath = "../Main/Results/Expts/" + exptno + "/Analysis/" + lp1;
        fs::create_directory(dirpath);
        string prefix= "rem_" + lp1; //output file

        //cin >> word;
        Graph G;
        runPairs(G, i); //load pairs into graph(object, langpairindex to ignore)
        timer.start(); // start timer
        map<string, Graph> predicted; //string stores language pair and maps it to a graph

        //precompute biconnected components and then run
        reqd.condOR["lang"].clear(); reqd.condOR["lang"].insert(l1[i]); reqd.condOR["lang"].insert(l2[i]);
        int new_trans = runBicomp(G, configlist, POS_to_config, prefix, predicted, exptno, lp1, lp2, reqd);
        cout << new_trans << endl;
        timer.end();
        timer.log();

        //Do analysis of predictions
        Compare C(l1[i], l2[i], i, exptno);
        CountbyPOS cntPOS(exptno, lp1);
    }
}
int main()
{
    int num_configs = 2;
    vector<Config> config(num_configs); //initialize with number of different configs required
    map<string, int> POS_to_config;
    //config.large_cutoff = 0;
    config[0].context_depth = 4;
    config[0].conf_threshold = 0.65;
    //config[0].max_cycle_length = 7;
    //config[0].large_min_cyc_len = 5; config[0].small_min_cyc_len = 4;
    //config[0].deg_gt2_multiplier = 1;
    config[1].transitive = true;
    config[1].context_depth = 4;
    config[1].conf_threshold = 0.1; //every pruned cycle gets selected
    config[1].large_min_cyc_len = 4;
    POS_to_config["properNoun"] = 1; POS_to_config["numeral"] = 1;
    InfoSets reqd;
    reqd.infolist.push_back("lang"); reqd.infolist.push_back("pos"); reqd.infolist.push_back("word_rep");
    genAll("7", config, POS_to_config, reqd);
    //cin >> word;
    //Graph G;
    //runPairs(G, idxign); //load pairs into graph(object, langpairindex to ignore)
    //getStatsComps(G);
    //runWords(G, word);
    //ofstream debugfile;
    //debugfile.open("../Main/Results/debugG.txt");
    //G.printGraph(debugfile);
}