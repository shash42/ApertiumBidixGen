#include "Graph.cpp"
#include "Biconnected.cpp"
#include<set>
#include<iostream>

//Compare predictions to actual data. Takes 2-3 mins to run.
int main()
{
    string lang = "oc-fr"; //language-pair to compare on
    string pred = "../Main/Results/RemLang/pred_" + lang + ".txt"; //file with predictions
    string orig = "../Main/LangData/Data-" + lang + ".txt"; //file with data for comparison
    Graph GP, GO, GC, GE; //G-prediction, G-original, G-correct, G-extra
    ofstream fout;
    fout.open("../Main/Analysis/Tempfile.txt"); //temporary output file for load data analytics
    GP.loadData(pred, fout);
    GO.loadData(orig, fout);

    int corr=0, extra=0;
    set<string> VC, VE; //words which it got correct, words which were extra
    for(auto u: GP.vertices){ //iterate over predicted words

        if(GO.idx_of_word.find(u.rep)==GO.idx_of_word.end())  //if the word is not in original graph
        {
            VE.insert(u.rep.surface); //add in set of extra predictions
            for(auto vidx: u.adj){ //iterate over translations
                wordData &v = GP.vertices[vidx].rep; //get the word representation
                GE.addEdge(u.rep, v); //add edge in extras graph
            }
            continue;
        }

        //otherwise word is in original graph (apertium data)
        VC.insert(u.rep.surface); //add to 'correct' words set
        int uidxO = GO.idx_of_word[u.rep]; //get its index in original graph
        wordNode &uO = GO.vertices[uidxO]; //get its vertex in original graph
        for(auto vidx: u.adj){ //iterate over adjacent nodes in predictions (Translations)
            wordData &v = GP.vertices[vidx].rep;
            if(GO.idx_of_word.find(v) == GO.idx_of_word.end()){ //if this translation word itself is not in original
                GE.addEdge(u.rep, v); //add edge in extras graph
                continue;
            }
            int vidxO = GO.idx_of_word[v]; //get index of translation word in original graph
            if(uO.adj.find(vidxO)==uO.adj.end()){ //if this translation edge is not in original graph
                GE.addEdge(u.rep, v); //add to extras graph
            }
            else{
                GC.addEdge(u.rep, v); //otherwise add to 'correct' graph
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
    gout.open(prefix + lang + "_correct.txt"); //print correct graph to passed file name
    GC.printGraph(gout); gout.close();
    gout.open(prefix + lang + "_extra.txt"); //print correct graph to passed file name
    GE.printGraph(gout);
}