#ifndef GSOCAPERTIUM2020_GRAPH_CPP
#define GSOCAPERTIUM2020_GRAPH_CPP
#include "Graph.h"

string wordData::extract_rep(int &i, string edge) {
    string temp; //stores return value (word representation)
    i++; //ignore first quotation mark
    while(edge.substr(i, 2) != "\"-") //till "-
    {
        temp+=edge[i];
        i++;
    }
    i+=2; //skip the "-
    return temp;
}
string wordData::extract_info(int &i, string edge) {
    string temp; //stores the information for return
    while(edge[i]!='-' && edge[i]!=' ' && i<edge.length()) //info ends at a '-' or ' ' or at end
    {
        temp+=edge[i];
        i++;
    }
    i++; //skip the end symbol ('-' or ' ')
    return temp;
}

void Graph::initNode(wordData &s)
{
    wordNode v_new(s); //make a wordNode out of the data
    vertices.push_back(v_new); //add it to the vertex list
}
/*Get the current index of the lemma represented in string s
or create a new node if it doesnt already exist*/
int Graph::getIdx(wordData &s)
{
    if(idx_of_word.find(s) == idx_of_word.end())
    {  //If it is not in the map add a new node to the map and take its index
        initNode(s);
        idx_of_word[s] = vertices.size()-1;
    }
    return idx_of_word[s];
}
bool Graph::addEdge(wordData &u, wordData &v) {
    int u_idx = getIdx(u);
    int v_idx = getIdx(v);
    //If it is not already in the vertex adjacency list
    if(vertices[u_idx].adj.find(v_idx)==vertices[u_idx].adj.end())
    { //set ensures no multi-edges. all edges are added as bidirectional
        num_edges++;
        vertices[u_idx].adj.insert(v_idx);
        vertices[v_idx].adj.insert(u_idx); //edges are assumed to be bidirectional
        return true;
    }
    return false;
}
void Graph::loadData(string &input_file)
{
    ifstream fin; //input file variable;
    string edge; //stores one edge information, i.e. one line in input file
    fin.open(input_file);

    while(getline(fin, edge)) //get edges till EOF
    {
        //cerr << edge << endl;
        int i = 0;
        wordData SLw(i, edge); //Source lang word
        wordData TLw(i, edge); //Target lang word
        addEdge(SLw, TLw); //Add the edge
    }

    //fout << "Number of words: " << vertices.size() << endl;
    //fout << "Number of edges: " << num_edges << endl;
}
void Graph::printGraph(ofstream &fout)
{ //u->v v->u will both be printed but won't necessarily be together in the output
    for(auto u: vertices)
    {
        for(auto vidx: u.adj)
        { //print edges using surface forms of the nodes they connect
            wordNode v = vertices[vidx];
            //fout << u.rep.info["surface"] << " " << v.rep.info["surface"] << endl;
            fout << u.rep.surface << " " << v.rep.surface << endl;
        }
    }
}
//clear all information of the graph
void Graph::reset() {
    idx_of_word.clear();
    vertices.clear();
    subGraphs.clear();
}
#endif //GSOCAPERTIUM2020_GRAPH_CPP