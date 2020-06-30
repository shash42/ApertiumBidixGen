#include "Graph.cpp"
#include "Biconnected.cpp"
#include<set>
#include<iostream>

Graph GO, GC, GE;
ofstream fout;
map<string, int> OV, OE, CV, CE, EV, EE;

//Count by part of speech
void countpos(Graph &G, map<string, int> &POSV, map<string, int> &POSE, ofstream &fout, string title)
{
    for(auto &u: G.vertices){ //Iterate over vertices (words) in the graph
        POSV[u.rep.pos]++; //Increase number of vertices
        POSE[u.rep.pos]+=u.adj.size(); //Increase number of edges
    };
}
//Output the analysis in readable format
void OutReadable(string lang)
{
    string out_name = "../Main/Results/RemLang/Analysis/POS_" + lang + ".txt";
    fout.open(out_name);
    for(auto pos: OE){ //iterate over POS, value pairs in graph of Original edges
        fout << endl << pos.first << endl;
        fout << "Vertices in Original/Correct/Extra: " << OV[pos.first] <<  " / " << CV[pos.first] << " / " << EV[pos.first] << endl;
        fout << "Edges in Original/Correct/Extra: " << OE[pos.first] <<  " / " << CE[pos.first] << " / " << EE[pos.first] << endl;
        fout << "Edge Correct/Predicted: " << CE[pos.first] * 100.00 / (CE[pos.first] + EE[pos.first]) << "%\n";
        fout << "Edge Predicted/Original: " << (CE[pos.first] + EE[pos.first]) * 100.00 / OE[pos.first] << "%\n";
    }
    fout.close();
}
//Output the analysis in raw format for visualizations
void OutRaw(string lang) {
    string out_name = "../Main/Results/RemLang/Analysis/RAW_" + lang + ".txt";
    fout.open(out_name);
    fout << GO.num_edges << " " << GC.num_edges << " " << GE.num_edges << endl;
    for(auto pos: OE){ //Iterate over POS, value pairs in graph of Original edges
        fout << pos.first << endl;
        fout << CE[pos.first] * 100.00 / (CE[pos.first] + EE[pos.first]) << endl;
        fout << (CE[pos.first] + EE[pos.first]) * 100.00 / OE[pos.first] << endl;
    }
    fout.close();
}

//count correct/extra for the output by POS category
int main()
{
    string lang = "oc-fr"; //language pair to analyze
    string file_name = "../Main/LangData/Data-" + lang + ".txt"; //get file name for original language data
    fout.open("../Main/Analysis/Tempfile.txt"); //required output file for load-data function
    GO.loadData(file_name, fout); //load data into original graph
    file_name = "../Main/Results/RemLang/Analysis/" + lang + "_correct.txt"; //file name for correct predictions
    GC.loadData(file_name, fout); //load data into correct graph
    file_name = "../Main/Results/RemLang/Analysis/" + lang + "_extra.txt"; //file name for extra predictions
    GE.loadData(file_name, fout); //load data into extra graph
    fout.close();

    countpos(GO, OV, OE, fout, "Original");
    countpos(GC, CV, CE, fout, "Correct Predictions");
    countpos(GE, EV, EE, fout, "Extra predictions");

    //OutReadable(lang);
    OutRaw(lang);
}