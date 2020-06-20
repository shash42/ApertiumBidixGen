#include "Graph.h"

string wordData::extract_rep(int &i, string edge) {
    string temp;
    i++; //ignore first quotation mark
    while(edge.substr(i, 2) != "\"-")
    {
        temp+=edge[i];
        i++;
    }
    i+=2;
    return temp;
}
string wordData::extract_info(int &i, string edge) {
    string temp;
    while(edge[i]!='-' && edge[i]!=' ' && i<edge.length())
    {
        temp+=edge[i];
        i++;
    }
    i++;
    return temp;
}

void Graph::initNode(wordData &s)
{
    wordNode v_new(s);
    vertices.push_back(v_new);
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
void Graph::addEdge(wordData &u, wordData &v) {
    int u_idx = getIdx(u);
    int v_idx = getIdx(v);
    //Add to to the vertex adjacency list
    if(vertices[u_idx].adj.find(v_idx)==vertices[u_idx].adj.end())
    { //set ensures no multi-edges. all edges are added as bidirectional
        num_edges++;
        vertices[u_idx].adj.insert(v_idx);
        vertices[v_idx].adj.insert(u_idx);
    }
}
void Graph::loadData(string &input_file, ofstream &fout)
{
    ifstream fin; //input file variable;
    string edge; //stores one edge information, i.e. one line in input file
    fin.open(input_file);
    while(getline(fin, edge)) //get edges till EOF
    {
        //cerr << edge << endl;
        int i = 0;
        wordData SLw(i, edge);
        wordData TLw(i, edge); //Source lang word and Target lang word
        addEdge(SLw, TLw); //Add the edge
    }
    fout << "Number of words: " << vertices.size() << endl;
    fout << "Number of edges: " << num_edges << endl;
}
void Graph::printGraph(ofstream &fout)
{
    for(auto u: vertices)
    {
        for(auto vidx: u.adj)
        {
            wordNode v = vertices[vidx];
            fout << u.rep.surface << " " << v.rep.surface << endl;
        }
    }
}

void Graph::reset() {
    idx_of_word.clear();
    vertices.clear();
    subGraphs.clear();
}