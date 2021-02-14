//Analyze language graph to select language pairs for input (eg: by checking biconnected components, distance etc.)

#include<bits/stdc++.h>
#include "../Biconnected.cpp"
#include "../Graph.cpp"

using namespace std;

const int N = 101; //Max langs

int countlines(string path)
{
    int cnt = 0;
    ifstream fin; fin.open(path); string tmp;
    while(getline(fin, tmp)){
        cnt++;
    }
    return cnt;
}

void findBicomps(Graph G, ofstream &results){
    results << "Biconnected Components: " << endl;
    Biconnected B;
    B.findBicomps(G);
    for(auto SG: G.subGraphs){
        for(auto u: SG.vertices){
            results << u.rep.lang << " ";
        }
        results << endl;
    }
    results << endl;
}

int dist[N][N], direct[N][N];
void bfs(int src, Graph &G)
{
    for(int i = 0; i < N; i++){
        dist[src][i] = -1;
    }
    queue<int> q;
    q.push(src);
    dist[src][src] = 0;
    while(q.size())
    {
        wordNode u = G.vertices[q.front()];
        int uidx = q.front();
        q.pop();
        for(auto vidx: u.adj){
            if(dist[src][vidx]==-1){
                dist[src][vidx] = dist[src][uidx] + 1;
                q.push(vidx);
            }
            if(dist[src][vidx]==1 && direct[src][vidx]==0 && dist[src][uidx]>0){
                direct[src][vidx] = dist[src][uidx]+1;
            }
        }
    }
}

void computeDist(Graph &G){
    for(int i = 0; i < G.vertices.size(); i++){
        bfs(i, G);
        //cout << i << " " << G.vertices[i].rep.lang << endl;
    }
}

void Output(Graph &G, ofstream &results){
    for(int i = 0; i < G.vertices.size(); i++){
        for(int j = i+1; j < G.vertices.size(); j++){
            if(dist[i][j]>2){
                results << "Distance >2: " << G.vertices[i].rep.lang << " " << G.vertices[j].rep.lang << endl;
            }
            /*if(direct[i][j]==1){
                results << G.vertices[i].rep.lang << " - " << G.vertices[j].rep.lang << endl;
            }*/
            if(dist[i][j]==1){
                results << "Directly connected: " << G.vertices[i].rep.lang << " " << G.vertices[j].rep.lang << ", otherwise distance: " << direct[i][j] << endl;
            }
        }
    }
}

void Input(Graph &G, int n, string langlist, string datapath){
    ifstream listlangs; listlangs.open(langlist);
    for(int i = 0; i < n; i++){
        string a, b;
        listlangs >> a >> b;
        string filepath = datapath + a + "-" + b + ".txt";
        /*int linecnt = countlines(filepath);
        if(linecnt < 25000){
            cout << a<< "-" << b << endl;
            continue;
        }
        cout << a << " " << b << endl;*/
        wordData A, B;
        A.lang = a; B.lang = b;
        A.word_rep = "-", B.word_rep = "-", A.pos = "noun", B.pos = "noun";
        A.makesurface(); B.makesurface();
        G.addEdge(A, B);
    }
}

int main(int argc, char*argv[])
{
    if(argc!=4){
        cerr << "Usage: " << argv[0] <<  " <path-to-langlist> <path-to-input-data-folder> <path-to-result-file>" << endl;
        return 0;
    }

    int N = 27; //modify - number of language pairs
    Graph G;
    Input(G, N, argv[1], argv[2]);
    cout << "|V|: " << G.vertices.size() << " |E|: " << G.num_edges << endl;
    
    computeDist(G);
    ofstream results; results.open(argv[3]);
    findBicomps(G, results);
    Output(G, results);
}