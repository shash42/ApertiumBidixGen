//Generate stats about the biconnected components

#include "../Graph.cpp"
#include "../Biconnected.cpp"

#include<chrono>
#include<bits/stdc++.h>
#include<experimental/filesystem>
#include<algorithm>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;
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
    compfile.open("Results/Stats/TD26bigcompfile-eng.txt");
    int subgraphnum = 0;
    int mxv = 0, mxe = 0, cntg10=0, cntg100=0, cntg1000=0, cntg=10; //max size and count of comps with >100 or >1000 vertices
    float avgEbyV=0;

    for(auto SG: G.subGraphs)
    {
        if(SG.vertices.size()>=10) cntg10++; //count components >=10 vertices
        if(SG.vertices.size()>=100){
            cout << "gt100: " << subgraphnum << endl; //print index of this component
            cntg100++; //count components >=100 vertices
        }
        if(SG.vertices.size()>=1000) cntg1000++; //count components >1000 vertices
        if(SG.vertices.size()<=2) continue; //if less than 2 (singleton vertex) ignore
        compfile << "Num vertices: " << SG.vertices.size() << endl;
        compfile << "Num edges: " << SG.num_edges << endl;
        mxv = max(mxv, (int) SG.vertices.size()); //to get max component size in vertices
        mxe = max(mxe, (int) SG.num_edges); //to get max component size in edges
        subgraphnum++;
        compfile << "index: " << subgraphnum << endl;
        int EbyV = (float)SG.num_edges/SG.vertices.size();
        if(EbyV > avgEbyV){
            avgEbyV = subgraphnum;
        }
        set<string> engwords;
        for(auto u: SG.vertices) //print only english words of the component
        {
            //if(u.rep.info["lang"]=="en") engwords.insert(u.rep.info["surface"]); //uncomment if dynamic wordData
            if(u.rep.lang=="eng") engwords.insert(u.rep.surface);
        }
        for(auto u: engwords)
        {
            compfile << u << endl;
        }
        compfile << endl;
    }
    cout << "average E by V: " << avgEbyV << endl;
    cout << "Maximum V, E: " << mxv << " " << mxe << endl;
    cout << "#(|V|>=10): " << cntg10 << " #(|V|>=100): " << cntg100 << " #(|V|>=1000): " << cntg1000 << endl;
    compfile.close();
}

int main(int argc, char*argv[])
{
    if(argc!=2){
        cerr << "Usage: " << argv[0] <<  " <path-to-input-data-folder> | eg: ./compare LangData/RDFParsed/" << endl;
        return 0;
    }
    int N = 27; //modify - number of langpairs for input
    vector<pair<string, string>> langpairs = {{"eng", "spa"}, {"eng", "cat"}, {"epo", "cat"}, {"epo", "eng"}, 
    {"epo", "fra"}, {"epo", "spa"}, {"fra", "cat"}, {"fra", "spa"}, {"oci", "cat"},
     {"oci", "fra"}, {"oci", "spa"}}; //initialized with 11
    
    langpairs.clear(); //clear and take some others if needed
    ifstream inp; inp.open("LangData/useful3let.txt"); //file to get langpair list
    for(int i = 0; i < N; i++){
        string a, b; inp >> a >> b;
        langpairs.push_back({a, b});
    }
    
    Graph G;
    string prefix=argv[1]; //folder 
    for(int i = 0; i < N; i++){
        if(i==0) continue;
        string lang = langpairs[i].first + "-" + langpairs[i].second;
        string inppath = prefix + lang + ".txt";
        G.loadData(inppath, true);
    }
    cout << "|V|: " << G.vertices.size() << " |E|: " << G.num_edges << endl;
    getStatsComps(G);
}
