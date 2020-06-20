//
// Created by shashwat on 20/06/20.
//

#ifndef GSOCAPERTIUM2020_DENSITYALGO_H
#define GSOCAPERTIUM2020_DENSITYALGO_H

#include "Graph.h"
#include "Biconnected.h"
#include<iostream>

class Config
{
public:
    int context_depth = 3;
    int max_cycle_length = 7;
    bool source_lang_repeat = false; //repeating source lang is allowed (true) or not
    int large_min_cyc_len = 5;
    int small_min_cyc_len = 4;
    int large_cutoff = 5;
    float deg_gt2_multiplier = 1.4;
    float conf_threshold = 0.7;
};
class Metrics
{
public:
    int sldeg=0, tldeg=0, num_vertices=0, num_edges=0;
    float density=0;
    bool is_edge =  false;
};
class DensityAlgo
{
    vector<vector<vector<Metrics>>> M; //2D matrix. M[i][j] has vector of metrics corresponding to each cycle.
    int source_idx_inG;
    Graph G, dfsG;
    Config config;
    vector<int> cycle_stack;
    vector<bool> visited;
    set<int> source_connected;
    ofstream fout;
public:
    DensityAlgo(Graph &passed, Config &reqconfig){
        G = passed;
        config = reqconfig;
    }
    int run(string &passedfile);
    void findContext(int source);
    void findCycles(Graph &C, int source);
    void getMetrics(int source);
    void dfs(int uidx, int source, int depth);
    int findTrans(int source);
};

#endif //GSOCAPERTIUM2020_DENSITYALGO_H
