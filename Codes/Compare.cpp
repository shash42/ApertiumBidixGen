#include "Graph.cpp"
#include "Biconnected.cpp"
#include<set>
#include<iostream>

int main()
{
    string lang = "oc-fr";
    string pred = "../Main/Results/RemLang/pred_" + lang + ".txt";
    string orig = "../Main/LangData/Data-" + lang + ".txt";
    Graph GP, GO, GC, GE; //G-prediction, G-original, G-correct, G-extra
    ofstream fout;
    fout.open("../Main/Analysis/Tempfile.txt");
    GP.loadData(pred, fout);
    GO.loadData(orig, fout);
    int corr=0, extra=0;
    set<string> VC, VE;
    for(auto u: GP.vertices){
        if(GO.idx_of_word.find(u.rep)==GO.idx_of_word.end())
        {
            VE.insert(u.rep.surface);
            for(auto vidx: u.adj){
                wordData &v = GP.vertices[vidx].rep;
                GE.addEdge(u.rep, v);
            }
            continue;
        }
        VC.insert(u.rep.surface);
        int uidxO = GO.idx_of_word[u.rep];
        wordNode &uO = GO.vertices[uidxO];
        for(auto vidx: u.adj){
            wordData &v = GP.vertices[vidx].rep;
            if(GO.idx_of_word.find(v) == GO.idx_of_word.end()){
                GE.addEdge(u.rep, v);
                continue;
            }
            int vidxO = GO.idx_of_word[v];
            if(uO.adj.find(vidxO)==uO.adj.end()){
                GE.addEdge(u.rep, v);
            }
            else{
                GC.addEdge(u.rep, v);
            }
        }
    }
    fout.close();
    ofstream summary, gout;
    string prefix = "../Main/Results/RemLang/Analysis/";
    summary.open(prefix + lang + "_summary.txt");
    summary << "Number of correct vertices: " << VC.size() << endl;
    summary << "Number of extra vertices: " << VE.size() << endl;
    summary << "Correct Graph" << endl;
    summary << "Number of Vertices: " << GC.vertices.size() << endl;
    summary << "Number of Edges: " << GC.num_edges << endl;
    summary << "Extra Graph" << endl;
    summary << "Number of Vertices: " << GE.vertices.size() << endl;
    summary << "Number of Edges: " << GE.num_edges << endl;
    summary.close();
    gout.open(prefix + lang + "_correct.txt");
    GC.printGraph(gout); gout.close();
    gout.open(prefix + lang + "_extra.txt");
    GE.printGraph(gout);
}