#ifndef GSOCAPERTIUM2020_GRAPH_H
#define GSOCAPERTIUM2020_GRAPH_H

#include<string>
#include<fstream>
#include<vector>
#include<map>
#include<set>

using namespace std;

struct wordData
{
    string word_rep, lang, pos, surface;
    string extract_rep(int &i, string edge);
    string extract_info(int &i, string edge);
    wordData() = default;
    void makesurface(){
        surface = "\"" + word_rep + "\"-" + pos + "-" + lang;
    }
    wordData(int &i, string &edge)
    {
        word_rep = extract_rep(i, edge);
        pos = extract_info(i, edge);
        lang = extract_info(i, edge);
        makesurface();
    }
    bool operator<(const wordData &t1) const
    {
        return surface < t1.surface;
    }
};

class wordNode
{
public:
    wordData rep;
    //int lang_num; //the mapped integer for the language
    set<int> adj; //stores nodes which have edges to this node in the graph
    explicit wordNode(wordData s){
        rep = s;
        rep.makesurface();
    }
};

class Graph
{
public:
    map<wordData, int> idx_of_word; //map that stores index of each word
    int num_edges = 0;
    vector<wordNode> vertices;
    vector<Graph> subGraphs;
    void initNode(wordData &s);
    int getIdx(wordData &s);
    void addEdge(wordData &u, wordData &v);
    void loadData(string &input_file, ofstream &fout);
    void reset();
    void printGraph(ofstream &fout);
};

#endif GSOCAPERTIUM2020_GRAPH_H