//
// Created by shashwat on 20/06/20.
//

#ifndef GSOCAPERTIUM2020_DENSITYALGO_H
#define GSOCAPERTIUM2020_DENSITYALGO_H

#include "Graph.h"
#include "Biconnected.h"
#include<iostream>

//stores hyperparameters for ease of optimization
class Config
{
public:
    int context_depth = 3; //distance to which a word's context is extracted
    int max_cycle_length = 7; //maximum cycle length to limit compute
    bool source_lang_repeat = false; //repeating source lang is allowed (true) or not
    int large_cutoff = 5; //cutoff on degree of source word to decide small or large context
    int large_min_cyc_len = 5; //min cycle length in large context
    int small_min_cyc_len = 4; //min cycle length in small context
    float deg_gt2_multiplier = 1.4; //multiplier if target word has sufficient degree
    float conf_threshold = 0.7; //confidence threshold to predict it as a new translation
};

//Metrics stored for each cycle corresponding to a translation pair
class Metrics
{
public:
    int sldeg=0, tldeg=0, num_vertices=0, num_edges=0;
    float density=0;
    bool is_edge =  false;
};

//Class containing functions and objects for the cycle density algorithm
class DensityAlgo
{   //TAKE CARE: indices in main graph (G) and context graph (dfsG) differ
    vector<vector<vector<Metrics>>> M; //2D matrix. M[i][j] has vector of metrics corresponding to each cycle.
    vector<vector<Metrics>> Best;
    int source_idx_inG; //the index of the source word in the current run in the main graph G
    Graph G, dfsG; //G is the main graph and dfsG is the context graph of the current word
    Config config; //configuration file containing hyperparameters
    vector<int> cycle_stack; //dfs stack to extract cycles
    vector<bool> visited; //listed of vertices that have been visited
    set<int> source_connected; //indices of words connected to source as in context graph
    ofstream fout; //output file for results
    void findContext(int source); //finds context given index of source word in context graph
    void findCycles(Graph &C, int source); //find cycles for source in the context graph - caller function for dfs
    void getMetrics(int source); //get metrics for each target word from current cycle in the cycle_stack
    void dfs(int uidx, int source, int depth); //depth first search for cycles (uidx is current node)
public:
    //constructor
    DensityAlgo(Graph &passed, Config &reqconfig){
        G = passed;
        config = reqconfig;
        Best.resize(G.vertices.size());
        for(auto u: Best) u.resize(G.vertices.size());
    }
    int run(string &passedfile, map<string, Graph> &pred); //main function of the class to call other functions and run the algo
    int findBest(int source, map<string, Graph> &pred); //finalize translations and output them using metrics for each potential target
};

#endif //GSOCAPERTIUM2020_DENSITYALGO_H
