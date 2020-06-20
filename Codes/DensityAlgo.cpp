#include "Graph.h"
#include "Biconnected.h"
#include "DensityAlgo.h"
#include<iostream>
#include<queue>

void DensityAlgo::findContext(int source)
{
    queue< int > bfs; // Stores the queue (node, depth) for bfs order
    map<int, int> depth;
    bfs.push(source); //Start at the word w, with 0 distance
    depth[source] = 0;
    while(!bfs.empty()) //run till queue is empty
    {
        int uidx = bfs.front();
        bfs.pop(); //Remove front node from queue
        wordNode &u = G.vertices[uidx];

        //Push adjacent vertices into the queue
        for(auto vidx: u.adj)
        {
            wordNode &v = G.vertices[vidx];
            //Add edge into the context graph. note that the indices in context graph and main graph differ
            dfsG.addEdge(u.rep, v.rep); //add uni-directional edge as other direction will come eventually
            //The target vertex should not be visited should have distance <= context_depth defined
            if(depth.find(vidx) == depth.end() && depth[uidx] < config.context_depth)
            {
                bfs.push(vidx);
                depth[vidx] = depth[uidx]+1; //depth of target node is 1 + depth of source node.
            }
        }
    }
}
void DensityAlgo::getMetrics(int source) {
    set<int> vertex_list; //list of vertices in this cycle
    for(auto u: cycle_stack) //can take all vertices because cycle started and ends at first vertex.
    {
        vertex_list.insert(u);
    }
    vector<int> deg(cycle_stack.size()); //stores degree within the cycle for each vertex
    int num_edges = 0; //number of edges within the cycle, double-counted. O(edges in dfsG)
    for(int i = 0; i < cycle_stack.size(); i++){
        int uidx = cycle_stack[i];
        for(auto vidx: dfsG.vertices[uidx].adj){
            //if the edge going out is to a vertex within the cycle, count the edge
            if(vertex_list.find(vidx)!=vertex_list.end()){
                num_edges++; deg[i]++;
            }
        }
    }
    //denom = denominator of density formula.
    int denom = (cycle_stack.size() * (cycle_stack.size() - 1)) / 2;
    for(int i = 1; i < cycle_stack.size(); i++) //start from 1 here as first node is just the source word
    {
        int vidx = cycle_stack[i];
        if(source_connected.find(vidx)!=source_connected.end())
        {
            continue; //if this edge already exists no need for metrics
        }
        Metrics temp; //store metrics for this cycle if the ith node is target-word
        temp.num_edges = num_edges;
        temp.num_vertices = cycle_stack.size();
        temp.sldeg = deg[0]; temp.tldeg = deg[i]; //source word is indexed 0 here so.
        //calculate density (divided by 2 to offset double counting done earlier)
        temp.density = (float) num_edges / (float) (2*denom);
        //first dimension is acc to index in G. Second is according to index in context graph.
        M[source_idx_inG][vidx].push_back(temp);
    }
}
int DensityAlgo::findTrans(int source)
{
    int num_trans = 0;
    fout << "Confidence score matchings for lemma: " << dfsG.vertices[source].rep.surface << endl;
    wordNode u = G.vertices[source_idx_inG];
    for(int i = 0; i < dfsG.vertices.size(); i++)
    {
        if(i==source) continue;
        vector<Metrics> &cycles = M[source_idx_inG][i];
        float confidence=0;
        for(auto cyc: cycles){
            float curr_den = cyc.density;
            if(cyc.tldeg > 2)
            {
                curr_den *= config.deg_gt2_multiplier;
                if(curr_den > 1.0) curr_den = 1;
            }
            if(curr_den > confidence) confidence = curr_den;
        }
        if(dfsG.vertices[source].adj.find(i)!=dfsG.vertices[source].adj.end())
        {
            fout << "Existing: " << dfsG.vertices[i].rep.surface << endl;
            continue;
        }
        wordNode v = dfsG.vertices[i];
        if(confidence >= config.conf_threshold)
        {
            num_trans++;
            fout << "New Translation!: " << v.rep.surface << " = " << confidence << " - " << M[source_idx_inG][i].size();
        } else{

            fout << "In context: " << v.rep.surface << " = " << confidence << " - " << M[source_idx_inG][i].size();
        }
        fout << endl;
    }
    fout << endl;
    return num_trans;
}
void DensityAlgo::dfs(int uidx, int source, int depth)
{
    visited[uidx]=true; //Mark the vertex as visited
    cycle_stack.push_back(uidx); //Put it in the stack
    wordNode &u = dfsG.vertices[uidx];
    for(auto vidx: u.adj) //Iterate over adjacent vertices of current vertex
    {
        wordNode &v = dfsG.vertices[vidx];
        //if source lang repeat is not allowed and v is of same lang is u, don't go to v.
        if(source!=vidx && !config.source_lang_repeat && v.rep.lang == dfsG.vertices[source].rep.lang) continue;
        //Only if the adjacent vertex is not visited
        if(visited[vidx] == false)
        { //and the current cycle length is not equal to maximum
            if(cycle_stack.size() < config.max_cycle_length - 1)
            {   //Visit this node in the dfs
                dfs(vidx, source, depth+1);
            }
        }
        /*Otherwise it is a visited node so a cycle is formed
        If it is source itself and the cycle is not a single edge going back,
        it is a cycle that needs to be considered*/
        else if(vidx == source)
        {
            //check if the min length requirements are met
            //min_len_req stores the min length required based on no. of source-word translations
            int min_len_req = config.large_min_cyc_len;
            //if the number of adjacent words are smaller than the cutoff reqd
            if(dfsG.vertices[source].adj.size() < config.large_cutoff)
            {   //use min_len_req for small cycle.
                min_len_req = config.small_min_cyc_len;
            }
            if(depth >= min_len_req - 1) getMetrics(source);
        }
    }
    visited[uidx]=false;
    cycle_stack.pop_back();
}
void DensityAlgo::findCycles(Graph &C, int source){
    cycle_stack.clear(); //clear stack
    visited.resize(C.vertices.size()); //clear visited and set size = no. of nodes in dfsG
    visited.assign(C.vertices.size(), false);
    dfsG = C; //currently, dfsG will be a reference to the context graph
    dfs(source, source, 0);
}
int DensityAlgo::run(string &passedfile)
{
    fout.open(passedfile, ios::app);
    fout << G.vertices.size() << " " << G.num_edges << endl;
    M.resize(G.vertices.size()); //dim1 of M = no. of vertices in this graph - flag
    int num_trans = 0;
    for(int i = 0; i < G.vertices.size(); i++)
    {
        if(i%1000==0 && i) cout << i << endl;
        source_idx_inG = i;
        dfsG.reset();
        findContext(i); //get the context graph
        int source_idx_inC = dfsG.idx_of_word[G.vertices[i].rep];
        source_connected.clear();
        for(auto vidx: dfsG.vertices[source_idx_inC].adj){
            source_connected.insert(vidx);
        }
        M[i].resize(dfsG.vertices.size()); //dim2 of M = no. of vertices in context of word (dfsG)
        findCycles(dfsG, source_idx_inC); //this word is 0 in context-graph as it will be the first word added
        num_trans += findTrans(source_idx_inC);
    }
    fout.close();
    return num_trans;
}
