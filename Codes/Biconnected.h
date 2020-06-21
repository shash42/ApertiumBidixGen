//
// Created by shashwat on 15/06/20.
//
#include "Graph.h"
#include<vector>
#include<stack>
#ifndef GSOCAPERTIUM2020_BICONNECTED_H
#define GSOCAPERTIUM2020_BICONNECTED_H

using namespace std;

class Biconnected
{
    int currtime=0;
    vector<bool> visited;
    vector<int> disc, low, parent;
    stack<pair<int, int>> temp_comp;
public:
    void findBicomps(Graph &G);
    void addComp(pair<int, int> &curr_edge, Graph &G);
    void findArtPt(int u, Graph &G);
};

#endif //GSOCAPERTIUM2020_BICONNECTED_H


