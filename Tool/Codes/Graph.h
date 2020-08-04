#ifndef GSOCAPERTIUM2020_GRAPH_H
#define GSOCAPERTIUM2020_GRAPH_H

#include<string>
#include<fstream>
#include<vector>
#include<map>
#include<set>

using namespace std;

//Used to store information about a given word
//Assumption: all members of info that are accessed are defined at initialization
//After each modification, declaration without passing index, edge, call makesurface()
struct wordData
{
    vector<string> infolist; //use this to make the types of info stored dynamic
    //word_rep stores written representation
    string word_rep, pos, lang, surface;
    //map<string, string> info;
    //use this later if dynamic info is required. Change .typeofinfo to .info["typeofinfo"] everywhere
    string extract_rep(int &i, string edge); //extract written representation
    string extract_info(int &i, string edge); //extract information about word
    wordData() = default; //default constructor
    wordData(int &i, string &edge) //given edge in input format, extract word data
    {
        /*info["word_rep"] = extract_rep(i, edge); //get written representation
        info["pos"] = extract_info(i, edge); //get part of speech
        info["lang"] = extract_info(i, edge);*/ //get language
        word_rep = extract_rep(i, edge); //get written representation
        pos = extract_info(i, edge); //get part of speech
        lang = extract_info(i, edge); //get language
        makesurface(); //make the surface form for ordering/map etc.
    }
    bool operator<(const wordData &t1) const
    { //operator for the map
        //string s1 = info.at("surface"), s2 = t1.info.at("surface");
        string s1 = surface, s2 = t1.surface;
        return s1 < s2;
    }
public:
    void makesurface(){ //surface stores the word in input/output format
        //info["surface"] = "\"" + info["word_rep"] + "\"-" + info["pos"] + "-" + info["lang"];
        surface = "\"" + word_rep + "\"-" + pos + "-" + lang;
    }
};

class wordNode //vertex of a graph class
{
public:
    wordData rep; //written representation and information for the word
    //int lang_num; //the mapped integer for the language
    set<int> adj; //stores nodes which have edges to this node in the graph
    explicit wordNode(wordData s){ //constructor
        rep = s;
        rep.makesurface(); //important to make surface as used for comparison in map
    }
};

//Graph class that contains
class Graph
{
public:
    map<wordData, int> idx_of_word; //map that stores index of each word
    int num_edges = 0; //number of edges in the graph
    vector<wordNode> vertices; //vector of vertices
    vector<Graph> subGraphs; //subgraphs (like biconnected comoonents) inside the graph
    void initNode(wordData &s); //initialize a node in the graph using worddata
    int getIdx(wordData &s); //get index of (or initialize) node corresponding to the word data
    bool addEdge(wordData &u, wordData &v); //add an edge between nodes having worddata u, v
    void loadData(string &input_file); //load data into the graph from input_file
    void reset(); //reset the graph object (empty it)
    void printGraph(ofstream &fout); //print the graph edges
};

#endif GSOCAPERTIUM2020_GRAPH_H