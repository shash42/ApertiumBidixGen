#ifndef GSOCAPERTIUM2020_COMPARE_CPP
#define GSOCAPERTIUM2020_COMPARE_CPP

#include "Graph.h"
#include "Biconnected.h"
#include "Graph.cpp"

#include<set>
#include<iostream>

//Compare predictions to actual data. Takes 2-3 mins to run.
class Compare{
    void putInExtra(Graph &divG, Graph GE[], wordData u, wordData v, string &l1, string &l2);
    void getUsedData(Graph &GD, int idxign);
    void getStats(Graph &inG, Graph &notInG, Graph &divG, Graph GE[], Graph &GC,
                  set<string> &VC, set<string> &VE, string &l1, string &l2);
public:
    Compare(string l1, string l2, int idxign, string &exptno);
};
void Compare::getUsedData(Graph &GD, int idxign)
{
    int num_pairs = 11; //number of pairs of languages.
    string input_file;
    ifstream file_list;
    file_list.open("../LangData-List.txt"); //file with names of lang pairs
   // ofstream fout;
 //   fout.open("../Analysis/Tempfile.txt");
    for(int i = 0; i < num_pairs; i++){
        file_list >> input_file;
        if(i==idxign) continue; //ignore this language pair (incase of removal and generation tests)
        //cout << input_file << endl; //output current input file for tracking progress
     //   fout << input_file << endl;
        GD.loadData(input_file);
    }
    //fout.close();
}

//which extra graph to put in based on common vertices: 0-none, 1-lang1 vertex, 2-lang2 vertex, 3-both vertices
void Compare::putInExtra(Graph &divG, Graph GE[], wordData u, wordData v, string &l1, string &l2){
    int missedClass = 0;
    if(divG.idx_of_word.find(u)!=divG.idx_of_word.end()){
        //missedClass |= (u.info["lang"]==l1)*1 + (u.info["lang"]==l2)*2; //uncomment if wordData is dynamic
        missedClass |= (u.lang==l1)*1 + (u.lang==l2)*2;
    }
    if(divG.idx_of_word.find(v)!=divG.idx_of_word.end()){
        //missedClass |= (v.info["lang"]==l1)*1 + (v.info["lang"]==l2)*2; //uncomment if wordData is dynamic
        missedClass |= (v.lang==l1)*1 + (v.lang==l2)*2;
    }
    GE[missedClass].addEdge(u, v);
}
//If translation in inG is not in notInG, classify extra class based on common vertices with divG and put in GE.
void Compare::getStats(Graph &inG, Graph &notInG, Graph &divG, Graph GE[], Graph &GC,
        set<string> &VC, set<string> &VE, string &l1, string &l2)
{
    for(auto u: inG.vertices){ //iterate over inG words
        for(auto vidx: u.adj){ //iterate over translations
            //if translation is there, add to GC, else add to appropriate GE.
            wordData &v = inG.vertices[vidx].rep; //get the word representation
            auto notThere = notInG.idx_of_word.end();
            if(notInG.idx_of_word.find(u.rep)==notThere){ //if u is not there in notInG
                //VE.insert(u.rep.info["surface"]); //uncomment if wordData is dynamic
                VE.insert(u.rep.surface);
                putInExtra(divG, GE, u.rep, v, l1, l2); //obviously it is a translation not in notInG
            }
            else if(notInG.idx_of_word.find(v)==notThere){ //if v is not there in notInG
                //VE.insert(v.info["surface"]); //Uncomment if wordData is dynamic
                VE.insert(v.surface);

                putInExtra(divG, GE, u.rep, v, l1, l2); //obviously it is a translation not in notInG
            }

            else{ //both u, v are there in notInG
                //VC.insert(u.rep.info["surface"]); //Uncomment if WordData is dynamic add to 'correct' words set
                VC.insert(u.rep.surface);
                int uidx2 = notInG.idx_of_word[u.rep]; //get its index in notInG graph
                wordNode &u2 = notInG.vertices[uidx2]; //get its vertex in notInG graph
                int vidx2 = notInG.idx_of_word[v];

                if(u2.adj.find(vidx2) == u2.adj.end()){ //if translation is not there
                    putInExtra(divG, GE, u.rep, v, l1, l2);
                }
                else{
                    GC.addEdge(u.rep, v); //otherwise add to 'correct' graph
                }
            }
        }
    }
}

Compare::Compare(string l1, string l2, int idxign, string &exptno){
    string lang = l1+"-"+l2; //language-pair to compare on
    string pred = "../Results/Expts/" + exptno + "/Analysis/" + lang + "/predictions.txt";
    string orig = "../LangData/Data-" + lang + ".txt"; //file with data for comparison
    Graph GP, GD, GO, GC, GE[4], GM[4]; //G-prediction, G-original, G-correct, G-extra, G-missed
    //ofstream fout;
  //  fout.open("../Analysis/Tempfile.txt"); //temporary output file for load data analytics
    GP.loadData(pred);
    GO.loadData(orig);
    getUsedData(GD, idxign);
    set<string> VC, VE, VM; //words which it got correct, words which were extra and missed

    getStats(GP, GO, GO, GE, GC, VC, VE, l1, l2); //Get Precision Stats
    getStats(GO, GP, GD, GM, GC, VC, VM, l1, l2); //Get Recall Stats

    //fout.close();
    ofstream summary, gout;
    string prefix = "../Results/Expts/" + exptno + "/Analysis/" + lang + "/";
    summary.open(prefix + "compare-summary.txt");
    summary << "Number of correct vertices (in P and O):" << VC.size() << endl;
    summary << "Number of extra vertices (in P, not in O): " << VE.size() << endl;
    summary << "Number of missed vertices (in O, not in P):" << VM.size() << endl;
    summary << "Correct Graph (in P and O)" << endl;
    summary << "Number of Vertices: " << GC.vertices.size() << endl;
    summary << "Number of Edges: " << GC.num_edges << endl;

    summary << endl << "Classification Legend: ";
    summary << "0 if no vertex in common, 1 if " << l1 << " vertex in common, 2 if";
    summary << l2 << " vertex in common, 3 if both vertices common\n" << endl;

    summary << "Extra Graph (in P, not in O, classified by O)" << endl;
    for(int i = 0; i < 4; i++){
        summary << "Number of Vertices in class" << i << ": " << GE[i].vertices.size() << endl;
        summary << "Number of Edges in class" << i << ": " << GE[i].num_edges << endl;
    }
    summary << "Missed Graph (in O, not in P, classified by D)" << endl;
    for(int i = 0; i < 4; i++){
        summary << "Number of Vertices in class" << i << ": " << GM[i].vertices.size() << endl;
        summary << "Number of Edges in class" << i << ": " << GM[i].num_edges << endl;
    }
    summary.close();
    gout.open(prefix + "correct.txt"); //print correct graph to passed file name
    GC.printGraph(gout); gout.close();
    for(int i = 0; i < 4; i++){
        gout.open(prefix + "extra" + to_string(i) + ".txt"); //print extra graph to passed file name
        GE[i].printGraph(gout);
        gout.close();
    }
    for(int i = 0; i < 4; i++){
        gout.open(prefix + "missed" + to_string(i) + ".txt"); //print missed graph to passed file name
        GM[i].printGraph(gout);
        gout.close();
    }
}

#endif //GSOCAPERTIUM2020_COMPARE_CPP