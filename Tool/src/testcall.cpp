#include "Graph.cpp"
//#include "Biconnected.cpp"
#include "DensityAlgo.cpp"
#include "Compare.cpp"
#include "CountByPOS.cpp"
#include "callers.cpp"
#include "PosstoPred.cpp"

#include<chrono>
#include<experimental/filesystem>
#include<algorithm>
#include <sys/stat.h>
#include <sys/types.h>

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

int main()
{
    int N = 27;
    vector<pair<string, string>> langpairs = {{"eng", "spa"}, {"eng", "cat"}, {"epo", "cat"}, {"epo", "eng"}, 
    {"epo", "fra"}, {"epo", "spa"}, {"fra", "cat"}, {"fra", "spa"}, {"oci", "cat"},
     {"oci", "fra"}, {"oci", "spa"}};
    /*vector<pair<string, string>> langpairs;
    ifstream inp; inp.open("LangData/useful3let.txt");
    for(int i = 0; i < N; i++){
        string a, b; inp >> a >> b;
        langpairs.push_back({a, b});
    }*/
    for(int i = 0; i < N; i++){
        string lang = langpairs[i].first + "-" + langpairs[i].second;
        string pred = "Results/Expts/RDF-Outer-Use10Gen1/Analysis/" + lang + "/predictions.txt";
        string orig = "LangData/RDFParsed/" + lang + ".txt";
        string used = "Results/Expts/Use10Gen1/Analysis/" + lang + "/langpathlist.txt";
        string prefix = "Results/Expts/RDF-Outer-Use10Gen1/Analysis/" + lang + "/";
        int num_used = N-1;
        Compare Cx(langpairs[i].first, langpairs[i].second, pred, orig, used, num_used, prefix);
        CountbyPOS CPx(lang, orig, prefix);
    }
    //Comparing 
    /*for(int i = 0; i < N; i++){
        string lang = langpairs[i].first + "-" + langpairs[i].second;
        string pred = "LangData/RDFParsed/" + lang + ".txt";
        string orig = "LangData/Parsed/" + lang + ".txt";
        string used = pred;
        string prefix = "LangData/RDFData/metrics/" + lang + "/";
        const char *lmao = prefix.c_str();
        mkdir(lmao, 0777);
        int num_used = N-1;
        Compare Cx(langpairs[i].first, langpairs[i].second, pred, orig, used, num_used, prefix);
        CountbyPOS CPx(lang, orig, prefix);
    }*/
}
