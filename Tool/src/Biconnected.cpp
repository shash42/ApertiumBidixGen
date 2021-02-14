//Biconnected component computation implementation

#include "Biconnected.h"
#include "Graph.h"

using namespace std;

//assumes no multi-edges and self loops
//Reference - https://www.geeksforgeeks.org/biconnected-components/
//Intuition - https://codeforces.com/blog/entry/68138

//Resets the Biconnected object
void Biconnected::reset()
{
    currtime = 0;
    visited.clear(); disc.clear(); low.clear(); parent.clear();
    while(!temp_comp.empty()) temp_comp.pop();
}
void Biconnected::findBicomps(Graph &G)
{
    reset();
    int V = G.vertices.size(); //number of vertices in the graph
    visited.resize(V); disc.resize(V); low.resize(V); parent.resize(V); //initialize the required arrays
    for(int i = 0; i < V; i++) //loop over all vertices and dfs if not yet visited (useful if disconnected graph)
    {
        if(!visited[i]) //if not visited start from it
        {
            parent[i] = -1; //i is the starting vertex so no parent
            findArtPt(i, G); //dfs function for finding
            pair<int, int> curr_edge = {-1, -1};
            //dummy-edge, never comes. addComp while-loop runs till stack empty
            addComp(curr_edge, G); //component connecting the initial part is added
        }
    }
}
//add component based on the stack
void Biconnected::addComp(pair<int, int> &curr_edge, Graph &G)
{
    if(temp_comp.empty()) return; //if stack is already empty return
    Graph new_G; //the component to be added, initially empty
    pair<int, int> e_to_add;
    do //add edges to new_G till (including) curr_edge is reached in the stack
    {
        e_to_add = temp_comp.top(); //edge to be added
        //get wordData as it is used to add edge in a graph
        wordData u = G.vertices[e_to_add.first].rep;
        wordData v = G.vertices[e_to_add.second].rep;
        new_G.addEdge(u, v);
        temp_comp.pop(); //pop edge from the stack
    } while(!temp_comp.empty() && e_to_add!=curr_edge);
    G.subGraphs.push_back(new_G); //add the new graph into G's subgraph vector
}
//depth first search (recursive) to find biconnected components
void Biconnected::findArtPt(int u, Graph &G)
{
    int num_child = 0; //number of children of u (previously unvisited)
    disc[u]=low[u]=++currtime; //set discovery time and lowest depth reachable for current node
    visited[u]=true; //mark current vertex as visited

    for(int v: G.vertices[u].adj) //iterate on u's adjacency list
    {
        pair<int, int> curr_edge = {u, v}; //current edge is u->v
        if(!visited[v]){ //if v (target) not already visited
            num_child++; //increase child count
            temp_comp.push(curr_edge); //add current edge to stack
            parent[v] = u; //set v's parent to u
            findArtPt(v, G); //recurse from v
            low[u] = min(low[u], low[v]); //lowest u can reach is min. from the lowest for subtree of v

            if(parent[u]==-1 && num_child>1) //if u is the root (for this dfs run) and more than one child now
            {
                addComp(curr_edge, G); //u is now an articulation point and new biconnected comp found
            }
            else if(parent[u]!=-1 && low[v] >= disc[u]) //if non-root and it's back-edge is below u its an ArtPt
            {
                addComp(curr_edge, G); //add the BCC
            }
        }

        else if(parent[u]!=v){ //if v has already been visited and isn't the direct back-edge of u
            low[u] = min(low[u], disc[v]); //lowest in subtree of u is min of v's discovery time and itself
            if(disc[v] < disc[u]) temp_comp.push(curr_edge); //if it is a proper "back" edge, add it.
        }
    }
}
