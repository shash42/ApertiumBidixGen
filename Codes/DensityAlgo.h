//
// Created by shashwat on 20/06/20.
//

#ifndef GSOCAPERTIUM2020_DENSITYALGO_H
#define GSOCAPERTIUM2020_DENSITYALGO_H

#include "Graph.h"
#include "Biconnected.h"
#include<iostream>

class InfoSets
{
public:
    vector<string> infolist;
    map<string, set<string>> condOR;
};

//stores hyperparameters for ease of optimization
class Config
{
public:
    int transitive = 0; //0 -> not transitive 1 -> bicomp then transitive 2 -> completely transitive
    int context_depth = 4; //distance to which a word's context is extracted
    int max_cycle_length = 7; //maximum cycle length to limit compute
    bool st_lang_repeat = false; //REMOVE, NOT OF USE (keep false) //repeating source and targ lang is allowed (true) or not (false)
    bool any_lang_repeat = true; //REMOVE, NOT OF USE (keep true) //repeating of language is allowed (true) or not (false)
    int large_cutoff = 5; //cutoff on degree of source word to decide small or large context
    int large_min_cyc_len = 5; //min cycle length in large context
    int small_min_cyc_len = 4; //min cycle length in small context
    float deg_gt2_multiplier = 1.3; //multiplier if target word has sufficient degree
    float conf_threshold = 0.65; //confidence threshold to predict it as a new translation
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
    int source_idx_inG; //the index of the source word in the current run in the main graph G
    Graph G, dfsG; //G is the main graph and dfsG is the context graph of the current word
    vector<Config> configlist; //vector of configs to be used for this run. 0 is default
    map<string, int> POS_to_config; //map containing POS whose config isn't configlist[0]
    Config config; //configuration file containing hyperparameters for current word
    vector<int> cycle_stack; //dfs stack to extract cycles
    multiset<string> lang_in_cyc; //stores the languages in the current cycle
    vector<bool> visited; //listed of vertices that have been visited
    set<int> source_connected; //indices of words connected to source as in context graph
    ofstream fout; //output file for results

    void findContext(int source); //finds context given index of source word in context graph
    void findCycles(Graph &C, int source); //find cycles for source in the context graph - caller function for dfs
    void getMetrics(int source); //get metrics for each target word from current cycle in the cycle_stack
    void dfs(int uidx, int source, int depth); //depth first search for cycles (uidx is current node)

public:
    //constructor
    DensityAlgo(Graph &passed, vector<Config> &_configlist, map<string, int> _POS_to_config){
        G = passed;
        configlist = _configlist;
        POS_to_config = _POS_to_config;
    }
    bool wordIsReq(wordNode &u, InfoSets &reqd); //are translations to be found for u
    int run(string &passedfile, map<string, Graph> &pred, InfoSets &reqdPred, map<pair<wordData, wordData>, float> &entries);
    //main function of the class to call other functions and run the algo
    int findTrans(int source, map<string, Graph> &pred, map<pair<wordData, wordData>, float> &entries); //finalize translations and output them using metrics for each potential target
    int findTransitive(int source, map<string, Graph>&pred, map<pair<wordData, wordData>, float> &entries);
};

#endif //GSOCAPERTIUM2020_DENSITYALGO_H