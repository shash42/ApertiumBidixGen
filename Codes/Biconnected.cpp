#include "Biconnected.h"
#include "Graph.h"

using namespace std;

void Biconnected::findBicomps(Graph &G)
{
    int V = G.vertices.size();
    visited.resize(V); disc.resize(V); low.resize(V); parent.resize(V);
    for(int i = 0; i < V; i++)
    {
        if(!visited[i])
        {
            parent[i] = -1;
            findArtPt(i, G);
            pair<int, int> curr_edge = {-1, -1};
            //dummy-edge, never comes. happens till stack empty
            addComp(curr_edge, G);
        }
    }
}
void Biconnected::addComp(pair<int, int> &curr_edge, Graph &G)
{
    if(temp_comp.empty()) return;
    Graph new_G;
    pair<int, int> e_to_add;
    do
    {
        e_to_add = temp_comp.top();
        wordData u = G.vertices[e_to_add.first].rep;
        wordData v = G.vertices[e_to_add.second].rep;
        new_G.addEdge(u, v);
        temp_comp.pop();
    } while(!temp_comp.empty() && e_to_add!=curr_edge);
    G.subGraphs.push_back(new_G);
}
void Biconnected::findArtPt(int u, Graph &G)
{
    int num_child = 0;
    disc[u]=low[u]=++currtime;
    visited[u]=true;
    for(int v: G.vertices[u].adj)
    {
        pair<int, int> curr_edge = {u, v};
        if(!visited[v]){
            num_child++;
            temp_comp.push(curr_edge);
            parent[v] = u;
            findArtPt(v, G);
            low[u] = min(low[u], low[v]);
            if(parent[u]==-1 && num_child>1)
            {
                addComp(curr_edge, G);
            }
            else if(parent[u]!=-1 && low[v] >= disc[u])
            {
                addComp(curr_edge, G);
            }
        }
        else if(parent[u]!=v){
            low[u] = min(low[u], disc[v]);
            if(disc[v] < disc[u]) temp_comp.push(curr_edge);
        }
    }
}
