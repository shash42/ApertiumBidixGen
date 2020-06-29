#include "Graph.cpp"
#include "Biconnected.cpp"
#include<set>
#include<iostream>

Graph GO, GC, GE;
ofstream fout;
map<string, int> OV, OE, CV, CE, EV, EE;
void countpos(Graph &G, map<string, int> &POSV, map<string, int> &POSE, ofstream &fout, string title)
{
    for(auto &u: G.vertices){
        POSV[u.rep.pos]++;
        POSE[u.rep.pos]+=u.adj.size();
    };
}
void OutReadable(string lang)
{
    string out_name = "../Main/Results/RemLang/Analysis/POS_" + lang + ".txt";
    fout.open(out_name);
    for(auto pos: OE){
        fout << endl << pos.first << endl;
        fout << "Vertices in Original/Correct/Extra: " << OV[pos.first] <<  " / " << CV[pos.first] << " / " << EV[pos.first] << endl;
        fout << "Edges in Original/Correct/Extra: " << OE[pos.first] <<  " / " << CE[pos.first] << " / " << EE[pos.first] << endl;
        fout << "Edge Correct/Predicted: " << CE[pos.first] * 100.00 / (CE[pos.first] + EE[pos.first]) << "%\n";
        fout << "Edge Predicted/Original: " << (CE[pos.first] + EE[pos.first]) * 100.00 / OE[pos.first] << "%\n";
    }
    fout.close();
}
void OutRaw(string lang) {
    string out_name = "../Main/Results/RemLang/Analysis/RAW_" + lang + ".txt";
    fout.open(out_name);
    fout << GO.num_edges << " " << GC.num_edges << " " << GE.num_edges << endl;
    for(auto pos: OE){
        fout << pos.first << endl;
        fout << CE[pos.first] * 100.00 / (CE[pos.first] + EE[pos.first]) << endl;
        fout << (CE[pos.first] + EE[pos.first]) * 100.00 / OE[pos.first] << endl;
    }
    fout.close();
}
int main()
{
    string lang = "oc-fr";
    string file_name = "../Main/LangData/Data-" + lang + ".txt";
    fout.open("../Main/Analysis/Tempfile.txt");
    GO.loadData(file_name, fout);
    file_name = "../Main/Results/RemLang/Analysis/" + lang + "_correct.txt";
    GC.loadData(file_name, fout);
    file_name = "../Main/Results/RemLang/Analysis/" + lang + "_extra.txt";
    GE.loadData(file_name, fout);
    fout.close();

    countpos(GO, OV, OE, fout, "Original");
    countpos(GC, CV, CE, fout, "Correct Predictions");
    countpos(GE, EV, EE, fout, "Extra predictions");

    //OutReadable(lang);
    OutRaw(lang);
}