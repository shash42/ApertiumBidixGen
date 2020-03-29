#include<vector>
#include<queue>
#include<string>
#include<set>
#include<map>
#include<iostream>
#include<fstream>
#include<algorithm>

using namespace std;

const int max_nodes = 500;
const int cycle_length = 7;
const int context_depth = 3;
const double conf_threshold = 0.7;

struct Node //each node in the graph is a word-language pair.
{
    string word_rep;
    string lang;
    string pos;
    //int lang_num; //the mapped integer for the language
    vector<int> adj; //stores nodes which have edges to this node in the graph
    map<int, double> word_confidences;
} vertex_list[max_nodes];

int num_nodes=0, num_edges=0, num_langs=0;

//Initialize the new node
void initNode(string s)
{
    string word = "", pos = "", lang = "";
    int count_dash = 0; //Counts the number of dashes in the string till now
    for(int s_idx = 0; s_idx < s.length(); s_idx++)
    {
        if(s[s_idx] == '-') count_dash++;
        else if(count_dash==0) //If 0 dash have come its the root-word
        {
            word += s[s_idx];
        }
        else if(count_dash==1) //If 1 its POS
        {
            pos += s[s_idx];
        }
        else                   //Else (2) it is the language  
        {
            lang += s[s_idx];   
        }            
    }

    vertex_list[num_nodes].word_rep = word;
    vertex_list[num_nodes].pos = pos;
    vertex_list[num_nodes].lang = lang;
}

/*Get the current index of the lemma represented in string s
or create a new node if it doesnt already exist*/
int getIdx(string s, map<string, int> &idx_of_word)
{
    int idx;
    //If it is already in the map, get its index
    if(idx_of_word.find(s) != idx_of_word.end())
    {
        idx = idx_of_word[s];
    }
    else
    {
        //Otherwise add a new node to the map and take its index
        num_nodes++;
        initNode(s);
        idx_of_word[s] = num_nodes;
        idx = num_nodes;
    }
    return idx;
}

void addEdge(string u, string v, map<string, int> &idx_of_word)
{
    int u_idx = getIdx(u, idx_of_word);
    int v_idx = getIdx(v, idx_of_word);
    //Add to to the vertex adjacency list
    vertex_list[u_idx].adj.push_back(v_idx);
}

//Get data from sample_name.txt
void getData(string sample_name, ofstream &fout)
{
    ifstream fin; //input file variable;
    map<string, int> idx_of_word; //map that stores index of each word
    string edge; //stores one edge information, i.e. one line in input file
    string input_name = sample_name + ".txt";
    fin.open(input_name);
    while(getline(fin, edge)) //get edges till EOF
    {
        //cerr << edge << endl;
        num_edges += 1;
        //We first separate the two lemmas in the input line
        string SLw="", TLw=""; //Source lang word and Target lang word
        bool inTLw = false; //If the space has come and we are reading TL word
        for(int i = 0; i < edge.length(); i++)
        {
            if(edge[i] == ' ') //When space comes set to TL word read
            {
                inTLw = true;
            }
            else if(!inTLw) //Add to SL word if no space yet
            {
                SLw += edge[i];
            }
            else
            {
                TLw += edge[i]; //Else add to TL word
            }
        }
        addEdge(SLw, TLw, idx_of_word); //Add the edge
    }
    set<string> langs;
    for(int i = 1; i <= num_nodes; i++)
    {
        langs.insert(vertex_list[i].lang);
    }   
    fout << "Number of words: " << num_nodes << endl;
    fout << "Number of edges: " << num_edges << endl;
    fout << "Number of languages: " << langs.size() << endl;
}

/*Finds the context of a word w, which here
is distance <= 3*/
pair< vector<int>, map< int, set<int> > > find_context(int w)
{
    map<int, set<int> > context_graph;
    queue< pair<int, int> > bfs; // Stores the queue for bfs order
    set<int> visited;
    bfs.push({w, 0}); //Start at the word w, with 0 distance

    while(bfs.size())
    {
        pair<int, int> curr = bfs.front(); 
        bfs.pop(); //Take front node from queue
        visited.insert(curr.first); //Mark it as visited
        //cerr << curr.first << endl;

        //Push adjacent vertices into the queue
        for(auto v: vertex_list[curr.first].adj)
        {
            //Push adjacent vertices into the context graph
            context_graph[curr.first].insert(v);
            //The vertices pushed should have distance <= 3 and not be visited
            if(visited.find(v) == visited.end() && curr.second < context_depth)
            {
                bfs.push({v, curr.second + 1});
            }
        }
    }

    //Is the list of nodes that are in the context
    vector<int> context_nodes;
    for(auto itr: visited)
    {
        context_nodes.push_back(itr);
    }
    return {context_nodes, context_graph};
}

/*Function to compute the confidence score (density)
of the cycle given the context graph*/
double compute_score(int w, vector<int> &cycle_stack, map<int, set<int> > &context)
{
    /*Count every edge in the cycle twice (u->v)
    and (v->u). Divide by size*(size-1) for density*/
    int numerator = 0;
    int denominator = cycle_stack.size() * (cycle_stack.size() - 1);

    //Iterate over all pairs of unequalcycle nodes
    set<string> langs;
    for(auto u: cycle_stack)
    {
        //No two words should be from same language in cycle
        if(langs.find(vertex_list[u].lang)!=langs.end())
        {
            return 0;
        }
        langs.insert(vertex_list[u].lang); 
        //No other word should be from the same language as starting word w
        /*if(u!=w && vertex_list[u].lang == vertex_list[w].lang)
        {
            return 0;
        }*/       
        for(auto v: cycle_stack)
        {
            if(u==v) continue;
            //If u->v is an edge, add one to the numerator
            if(context[u].find(v) != context[u].end())
            {
                numerator += 1;
            }
        }
    }
    return ( (double) numerator) / denominator;
}

//Recursive procedure that finds cycles starting/ending at w
void dfs(int u, int w, int depth, map< int, set <int> > &context, vector<bool> &visited, map<int, double> &best_scores, vector<int> &cycle_stack)
{
    //cout << u << endl;
    visited[u]=true; //Mark the vertex as visited
    cycle_stack.push_back(u); //Put it in the stack
    
    for(auto v: context[u]) //Iterate over adjacent vertices
    {
        /*Only if the adjacent vertex is not visited
        and the current cycle length is not equal to maximum
        Visit this node in the dfs*/
        if(visited[v] == false)
        {
            if(depth < cycle_length)
            {
                dfs(v, w, depth+1, context, visited, best_scores, cycle_stack);
            }
        }
        /*Otherwise it is a visited node so a cycle is formed
        If it is w itself and the cycle is not a single edge going back,
        it is a cycle that needs to be considered*/
        else if(depth != 1 && v == w)
        {
            //Compute confidence score of the cycle (density)
            double confidence = compute_score(w, cycle_stack, context);
            //Iterate over nodes in the cycle
            for(auto cycle_node: cycle_stack)
            {
                /*If this confidence is higher than the best one for the node
                seen till now, make this the node's current confidence*/
                if(confidence > best_scores[cycle_node])
                {
                    best_scores[cycle_node] = confidence;
                }
            }
        }
    }
    visited[u]=false;
    cycle_stack.pop_back();
    return;
}

map<int, double> find_best_cycle(int w, vector<int> &context_nodes, map< int, set <int> > &context)
{
    map<int, double> best_scores;
    vector<bool> visited(num_nodes+1); //Tracks if the nodes have been explored in the DFS
    
    for(auto u: context_nodes) //Initializes confidence scores
    {
        // Score is 1 if there is a direct edge from w to u.
        // It is initialized to 0 otherwise.
        best_scores[u] = 0;
        if( context[w].find(u) != context[w].end() )
        {
            best_scores[u] = 1;
        }
        visited[u]=false; //Initializing all nodes to not visited
    }
    best_scores[w]=1;
    
    vector<int> cycle_stack; //Stack to store current path
    //I use the typical dfs + stack recursive backtracking cycle finding implementation
    dfs(w, w, 0, context, visited, best_scores, cycle_stack);
    
    return best_scores;
}

/*A structure used for output format
is_new is true if the translation is inferred
rep stores the ouput formatted string
*/
struct output_element
{
    bool is_new;
    string rep;
    double conf_score; 
};

/*Sorts output vector so that inferred translations
are printed first and sorting within is by confidence score*/
struct confSortDesc
{
    bool operator()(const output_element &a, const output_element &b)
    {
        if(a.is_new==b.is_new) return a.conf_score > b.conf_score;
        return a.is_new > b.is_new;
    }
};

//Output best scores for this word in descending order in <sample_name>_o.txt
void outputFull(int w, ofstream &fout, map< int, set <int> > &context)
{
    fout << endl << "Confidence score matchings for lemma: ";
    //Print string representation of w as in Input file
    fout << vertex_list[w].word_rep << "-" << vertex_list[w].pos << "-" << vertex_list[w].lang << endl;

    //Transfer from the map to a vector for sorting
    vector< output_element > confidence_list;
    for(auto iter: vertex_list[w].word_confidences)
    {
        if(iter.first == w) continue; //ignore if the potential target is the same word
        bool is_new_val = false;
        if(context[w].find(iter.first) == context[w].end())
        {
            if(iter.second >= conf_threshold) is_new_val = true;
            /*it is a new val if confidence score is higher than threshold
            and the two words were'nt connected in the input*/
        }
        string word_to_add;
        word_to_add = vertex_list[iter.first].word_rep + "-" + vertex_list[iter.first].pos
                     + "-" + vertex_list[iter.first].lang;
        confidence_list.push_back({is_new_val, word_to_add, iter.second});
    }
    
    //Sort confidence list in descending order of confidence scores
    sort(confidence_list.begin(), confidence_list.end(), confSortDesc());
    //Print the list
    for(auto word: confidence_list)
    {
        if(word.is_new)
        {
            fout << "Inferred: ";
        }
        else if(word.conf_score == 1)
        {
            fout << "Existing: ";
        }
        else
        {
            fout << "In context: ";
        }
        fout << word.rep << ": " << word.conf_score << endl;
    }
}

int main()
{
    string sample_name;
    ofstream fout; //initialize an output file object
    cout << "Enter the word whose sample file you want to use (without .txt): ";
    cin >> sample_name;
    string output_name = sample_name + "_o.txt";
    fout.open(output_name);
    getData(sample_name, fout); //Setup the graph from data
    
    for(int w = 1; w <= num_nodes; w++) //Iterate over all the words
    {
        //cerr << w << endl;
        pair< vector<int>, map<int, set<int> > > context; //The context graph for this word
        map<int, double> best_scores; //Best scores for each node in it's context
        context = find_context(w); //Get the context of the word w
        best_scores = find_best_cycle(w, context.first, context.second);
        vertex_list[w].word_confidences = best_scores; //store the best scores corresponding to this word
        //Finds the confidence score with w for each word in the context
        outputFull(w, fout, context.second);
    }
}