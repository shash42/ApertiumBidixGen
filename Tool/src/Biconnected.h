//Biconnected component class

#ifndef GSOCAPERTIUM2020_BICONNECTED_H
#define GSOCAPERTIUM2020_BICONNECTED_H
#include "Graph.h"
#include<vector>
#include<stack>

using namespace std;

//Given a graph, populates its subgraph with its biconnected components (BCC)
class Biconnected
{
    int currtime; //time used for calculating low time and discovery time
    vector<bool> visited; //whether the vertex has been visited
    vector<int> disc, low, parent; //required vectors for algorithm
    stack<pair<int, int>> temp_comp; //stack that contains the edges (as indices of vertices)
    void reset();
    void addComp(pair<int, int> &curr_edge, Graph &G); //add component based on stack
    void findArtPt(int u, Graph &G); //find articulation points to generate BCC (depth first search)
public:
    void findBicomps(Graph &G); //caller function
};

#endif //GSOCAPERTIUM2020_BICONNECTED_H


