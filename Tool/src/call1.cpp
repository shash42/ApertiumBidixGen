#include "Graph.cpp"
#include "Biconnected.cpp"
#include "DensityAlgo.cpp"
#include "Compare.cpp"
#include "CountByPOS.cpp"
#include "callers.cpp"
#include "PosstoPred.cpp"

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

//get statistics on the biconnected components - REQUIRES UPDATES
void getStatsComps(Graph &G)
{
    Biconnected B;
    B.findBicomps(G);
    ofstream compfile;
    compfile.open("../Analysis/TDbigcompfile-eng.txt");
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

//Generates predictions for all language pairs by leaving out that pair and using others as input.
void genAll(string exptno, vector<Config> &configlist, map<string, int> &POS_to_config, InfoSets reqd){
    int numpairs = 11;
    string l1[] = {"en", "en", "fr", "fr", "eo", "eo", "eo", "eo", "oc", "oc", "oc"};
    string l2[] = {"es", "ca", "es", "ca", "fr", "ca", "en", "es", "ca", "es", "fr"};

    for(int i = 0; i < numpairs; i++){
        cout << "Language No.: " << i+1 << endl;
        Stopwatch timer;
        string lp1 = l1[i] + "-" + l2[i], lp2= l2[i] + "-" + l1[i]; //language pair to get predictions for
        string dirpath = "../Results/Expts/" + exptno + "/Analysis/" + lp1;
        fs::create_directory(dirpath);
        string prefix= "rem_" + lp1; //output file

        //cin >> word;
        Graph G;
        runPairs(G, i); //load pairs into graph(object, langpairindex to ignore)
        cout << "Loaded" << endl;
        timer.start(); // start timer
        map<string, Graph> predicted; //string stores language pair and maps it to a graph

        //precompute biconnected components and then run
        reqd.condOR["lang"].clear(); reqd.condOR["lang"].insert(l1[i]); reqd.condOR["lang"].insert(l2[i]);
        int new_trans = runBicomp(G, configlist, POS_to_config, prefix, predicted, exptno, lp1, lp2, reqd);
        cout << new_trans << endl;
        timer.end();
        timer.log();

        //Do analysis of predictions
        //Compare C(l1[i], l2[i], i, exptno);
        //CountbyPOS cntPOS(exptno, lp1);
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
    config[0].max_cycle_length = 7;
    config[0].deg_gt2_multiplier = 1.3;
    config[0].large_min_cyc_len = 5; config[0].small_min_cyc_len = 4;
    //config[0].deg_gt2_multiplier = 1;
    config[1].transitive = 2;
    config[1].context_depth = 4;
    config[1].conf_threshold = 0.1; //every pruned cycle gets selected
    config[1].large_min_cyc_len = 4;
    POS_to_config["properNoun"] = 1; POS_to_config["numeral"] = 1;
    InfoSets reqd;
    reqd.infolist.push_back("lang"); reqd.infolist.push_back("pos"); reqd.infolist.push_back("word_rep");
    genAll("10", config, POS_to_config, reqd);
    //cin >> word;
    //Graph G;
    //runPairs(G, idxign); //load pairs into graph(object, langpairindex to ignore)
    //getStatsComps(G);
    //runWords(G, word);
    //ofstream debugfile;
    //debugfile.open("../Results/debugG.txt");
    //G.printGraph(debugfile);
}