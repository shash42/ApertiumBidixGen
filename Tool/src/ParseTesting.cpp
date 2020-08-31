#include "Graph.h"
#include "Biconnected.h"
#include "Graph.cpp"

#include<set>
#include<iostream>

map<string, string> let2to3 = { {"en", "eng"}, {"es", "spa"}, {"ca", "cat"}, {"fr", "fra"}, {"oc", "oci"} };
//which extra graph to put in based on common vertices: 0-none, 1-lang1 vertex, 2-lang2 vertex, 3-both vertices
void putInExtra(Graph &divG, Graph GE[], wordData u, wordData v, string &l1, string &l2){
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
void getStats(Graph &inG, Graph &notInG, Graph &divG, Graph GE[], Graph &GC,
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

void Compare(string l1, string l2, ofstream &summary){
    string lang = l1+"-"+l2; //language-pair to compare on
    string pred = "Bidixes/Parsed/Useful/" + lang + ".txt";
    string orig = "Bidixes/RDFParsed/Data-" + lang + ".txt"; //file with data for comparison
    string missed = "Bidixes/RDFParsed/Missed/" + lang + ".txt";
    Graph GP, GO, GC, GE[4], GM[4]; //G-prediction, G-original, G-correct, G-extra, G-missed

    GP.loadData(pred);
    GO.loadData(orig);
    cerr << GP.vertices.size() << " " << GO.vertices.size() << " " << GP.num_edges << " " << GO.num_edges << endl;
    set<string> VC, VE, VM; //words which it got correct, words which were extra and missed

    getStats(GP, GO, GO, GE, GC, VC, VE, l1, l2); //Get Precision Stats
    getStats(GO, GP, GP, GM, GC, VC, VM, l1, l2); //Get Recall Stats

    summary << "Number of correct vertices (in P and O):" << VC.size() << endl;
    summary << "Number of extra vertices (in P, not in O): " << VE.size() << endl;
    summary << "Number of missed vertices (in O, not in P):" << VM.size() << endl;
    summary << "Number of predicted edges: " << GP.num_edges << " " << "Number of Original edges: " << GO.num_edges << endl;
    summary << "Number of Correct Edges: " << GC.num_edges << endl;
    summary << endl << "Classification Legend: ";
    summary << "0 if no vertex in common, 1 if " << l1 << " vertex in common, 2 if";
    summary << l2 << " vertex in common, 3 if both vertices common\n" << endl;

    summary << "Extra Graph (in P, not in O, classified by O)" << endl;
    for(int i = 0; i < 4; i++){
        summary << "Number of Vertices in class" << i << ": " << GE[i].vertices.size() << endl;
        summary << "Number of Edges in class" << i << ": " << GE[i].num_edges << endl;
    }

    ofstream fmiss; fmiss.open(missed);
    summary << "Missed Graph (in O, not in P, classified by P)" << endl;
    for(int i = 0; i < 4; i++){
        GM[i].printGraph(fmiss);
        summary << "Number of Vertices in class" << i << ": " << GM[i].vertices.size() << endl;
        summary << "Number of Edges in class" << i << ": " << GM[i].num_edges << endl;
    }
}
void To3letter(){
    ifstream fin;
    fin.open("Bidixes/RDFParsed/Data-List.txt");
    for(int i = 0; i < 11; i++){
        string l1, l2;
        fin >> l1 >> l2;
        cout << l1 << " " << l2 << endl;
        if(l1.length()<3) continue;
        string langfile = "Bidixes/RDFParsed/Data-" + l1 + "-" + l2 + ".txt";
        Graph G;
        G.loadData(langfile);
        for(auto &v: G.vertices){
         //   int oldidx = G.idx_of_word[v.rep];
           // G.idx_of_word.erase(v.rep);
            string tlang = let2to3[v.rep.lang];
            v.rep.lang = tlang;  v.rep.makesurface();
            //G.idx_of_word[v.rep] = oldidx;
        }
        ofstream langdata; langdata.open(langfile);
        G.printGraph(langdata);
    }
}
int main(){
    //To3letter();
    ofstream fout;
    fout.open("Bidixes/RDFParsed/ResAnalysis.txt");
    ifstream fin;
    fin.open("Bidixes/RDFParsed/Data-List.txt");
    for(int i = 0; i < 11; i++){
        string l1, l2;
        fin >> l1 >> l2;
        cerr << l1 << " " << l2 << endl;
        fout << l1 << "-" << l2 << endl;
        Compare(l1, l2, fout);
        fout << endl << endl;
    }
}
