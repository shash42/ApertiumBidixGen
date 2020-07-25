#include "Graph.h"
#include "Biconnected.h"
#include "DensityAlgo.h"
#include<iostream>
#include<queue>

void DensityAlgo::findContext(int source)
{
    queue< int > bfs; // Stores the queue (node, depth) for bfs order
    map<int, int> depth; //stores the depth of nodes
    bfs.push(source); //Start at the word w, with 0 distance
    depth[source] = 0; //source depth is initialized as 0

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
                bfs.push(vidx); //push target vertex into bfs queue
                depth[vidx] = depth[uidx]+1; //depth of target node is 1 + depth of source node.
            }
        }
    }
}
void DensityAlgo::getMetrics(int source) {
    set<int> vertex_list; //list of vertices in this cycle
    for(auto uidx: cycle_stack) //can take all vertices because cycle started and ends at first vertex.
    {
        vertex_list.insert(uidx);
    }
    vector<int> deg(cycle_stack.size()); //stores degree within the cycle for each vertex
    int num_edges = 0; //number of edges within the cycle, double-counted. O(edges in dfsG)

    for(int i = 0; i < cycle_stack.size(); i++){
        int uidx = cycle_stack[i];
        for(auto vidx: dfsG.vertices[uidx].adj){
            //if the edge going out is to a vertex within the cycle, count the edge (add to degree and num_edges)
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
        int tlcount = lang_in_cyc.count(dfsG.vertices[vidx].rep.lang); //number of times target lang comes in cycle
        if(!config.st_lang_repeat && tlcount > 1){
            continue; //if target lang repeat is not allowed don't consider this cycle
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

void DensityAlgo::dfs(int uidx, int source, int depth)
{
    visited[uidx]=true; //Mark the vertex as visited
    cycle_stack.push_back(uidx); //Put it in the stack
    wordNode &u = dfsG.vertices[uidx];
    lang_in_cyc.insert(u.rep.lang); //Put language in the set of languages taken

    for(auto vidx: u.adj) //Iterate over adjacent vertices of current vertex
    {
        wordNode &v = dfsG.vertices[vidx];
        //if source lang repeat is not allowed and v is of same lang is u, don't go to v.
        //Use first IF if wordData is dynamic
        //if(source!=vidx && !config.st_lang_repeat && v.rep.info["lang"] == dfsG.vertices[source].rep.info["lang"])
        if(source!=vidx && !config.st_lang_repeat && v.rep.lang == dfsG.vertices[source].rep.lang) {
            continue;
        }
        //Only if the adjacent vertex is not visited
        if(visited[vidx] == false)
        { //and the current cycle length is not equal to maximum
            if(cycle_stack.size() < config.max_cycle_length - 1)
            {
                //If language repetition is allowed or v's language has not been visited.
                if(config.any_lang_repeat || lang_in_cyc.find(dfsG.vertices[vidx].rep.lang) == lang_in_cyc.end()){
                    dfs(vidx, source, depth+1);
                    //Visit this node in the dfs
                }
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
    cycle_stack.pop_back(); //remove vertex from the stack
    lang_in_cyc.erase(u.rep.lang); //remove it's language from the current language set.
}
void DensityAlgo::findCycles(Graph &C, int source){
    cycle_stack.clear(); //clear stack
    lang_in_cyc.clear();
    visited.resize(C.vertices.size()); //clear visited and set size = no. of nodes in dfsG
    visited.assign(C.vertices.size(), false);
    dfsG = C; //currently, dfsG will be a reference to the context graph
    dfs(source, source, 0);
}

int DensityAlgo::findTrans(int source, map<string, Graph> &pred, map<pair<wordData, wordData>, float> &entries)
{ //the source integer passed is it's index in context graph (dfsG)
    int num_trans = 0; //stores number of predicted translations
    //fout << "Confidence score matchings for lemma: " << dfsG.vertices[source].rep.surface << endl;
    wordNode u = G.vertices[source_idx_inG];

    for(int i = 0; i < dfsG.vertices.size(); i++) //iterate over all vertices
    {
        if(i==source) continue; //if it is the source itself ignore
        vector<Metrics> &cycles = M[source_idx_inG][i]; //the cycle metrics for this particular target
        float confidence=0; //stores the final confidence for output

        for(auto cyc: cycles){ //iterate over the metrics for different cycles
            float curr_den = cyc.density; //density of cycle (temporary confidence variable)
            if(cyc.tldeg > 2) //if target word degree is > 2 multiply the current confidence
            {
                curr_den *= config.deg_gt2_multiplier;
                if(curr_den > 1.0) curr_den = 1; //cap it at 1.0
            }
            if(curr_den > confidence) confidence = curr_den; //if this was higher than current confidence, keep it
        }

        if(dfsG.vertices[source].adj.find(i)!=dfsG.vertices[source].adj.end())
        { //if existing translation in input graph
            //       fout << "Existing: " << dfsG.vertices[i].rep.surface << endl;
            continue;
        }

        wordNode v = dfsG.vertices[i]; //

        if(confidence > 0) {
            if (entries.find({v.rep, u.rep}) != entries.end()) {
                entries[{v.rep, u.rep}] = max(confidence, entries[{v.rep, u.rep}]);
            } else {
                entries[{u.rep, v.rep}] = confidence;
            }
        }
        if(confidence >= config.conf_threshold)
        { //if it is higher than the threshold required predict this as a new translation
            /*string langpairUv = u.rep.info["lang"] + "-" + v.rep.info["lang"];
            string langpairVu = v.rep.info["lang"] + "-" + u.rep.info["lang"]; uncomment when dynamic needed */
            string langpairUv = u.rep.lang + "-" + v.rep.lang;
            string langpairVu = v.rep.lang + "-" + u.rep.lang;
            bool isnew = false;
            if(pred.find(langpairVu)!=pred.end()){ //if V->U is a language pair
                isnew = pred[langpairVu].addEdge(u.rep, v.rep);
            }
            else{ //otherwise U->V (if already exists great, otherwise make)
                isnew = pred[langpairUv].addEdge(u.rep, v.rep);
            }
            if(isnew) num_trans++; //increase count of predicted translations
            //     fout << "New Translation!: " << v.rep.surface << " = " << confidence << " - " << M[source_idx_inG][i].size();
        }

        else{
            //otherwise just output as an in-context word along with confidence, not a predicted translation
            //   fout << "In context: " << v.rep.surface << " = " << confidence << " - " << M[source_idx_inG][i].size();
        }
        //    fout << endl;
    }
    //fout << endl;
    return num_trans;
}

//Finds translations using transitive closure (all nodes in context within depth are translations)
int DensityAlgo::findTransitive(int source, map<string, Graph> &pred, map<pair<wordData, wordData>, float> &entries) {
    int num_trans=0;
    for(int j = 0; j < dfsG.vertices.size(); j++){
        if(j!=source && source_connected.find(j)==source_connected.end()){
            wordNode u = dfsG.vertices[source], v = dfsG.vertices[j];
            if(entries.find({v.rep, u.rep})!=entries.end()){
                entries[{v.rep, u.rep}] = max((float) 1, entries[{v.rep, u.rep}]);
            }
            else{
                entries[{u.rep, v.rep}] = 1;
            }
            string langpairUv = u.rep.lang + "-" + v.rep.lang;
            string langpairVu = v.rep.lang + "-" + u.rep.lang;
            bool isnew;
            if(pred.find(langpairVu)!=pred.end()){ //if V->U is a language pair
                isnew = pred[langpairVu].addEdge(u.rep, v.rep);
            }
            else{ //otherwise U->V (if already exists great, otherwise make)
                isnew = pred[langpairUv].addEdge(u.rep, v.rep);
            }
            if(isnew) num_trans++; //increase count of predicted translations
        }
    }
    return num_trans;
}
bool DensityAlgo::wordIsReq(wordNode &u, InfoSets &reqd){
    bool ret = true;
    for(auto info: reqd.infolist){
        //if(!reqd.condOR[info].empty() && reqd.condOR[info].find(u.rep.info[info])==reqd.condOR[info].end()){
        //above if using dynamic
        string uinfo = u.rep.word_rep;
        if(info=="pos") uinfo = u.rep.pos;
        if(info=="lang") uinfo = u.rep.lang;
        if(!reqd.condOR[info].empty() && reqd.condOR[info].find(uinfo)==reqd.condOR[info].end()){
            /*if(u.rep.info["lang"] == "en"){
                //cout << info << " " << u.rep.info[info] << endl;
            }*/
            ret = false;
        }
    }
    return ret;
}
//Run the algorthm
/*Usage of reqdPred:
 * Fill the ObjectSets with the language/POS/words you want. Translations for a lang/POS/word
 * will be searched if it is in it's respective set (unless that set is empty)
 */
int DensityAlgo::run(string &passedfile, map<string, Graph> &pred, InfoSets &reqdPred, map<pair<wordData, wordData>, float> &entries)
{
    //fout.open(passedfile, ios::app);
    //fout << G.vertices.size() << " " << G.num_edges << endl;
    M.resize(G.vertices.size()); //dim1 of M = no. of vertices in this graph - flag
    int num_trans = 0;

    for(int i = 0; i < G.vertices.size(); i++)
    {
        if(i%50000==0 && i) {
            cout << i << endl; //output every 1000th node just to check progress
        }
        if(!wordIsReq(G.vertices[i], reqdPred)) continue;
        //cout << G.vertices[i].rep.surface << " " << num_trans << endl;

        //Define config
        config = configlist[0]; //by default first element of configlist
        string currpos = G.vertices[i].rep.pos; //POS of current source word
        if(POS_to_config.find(currpos)!=POS_to_config.end()){ //if a special config is defined for this POS
            config = configlist[POS_to_config[currpos]]; //then config for finding target word is POS specific
        }

        source_idx_inG = i;
        dfsG.reset();
        findContext(i); //get the context graph
        int source_idx_inC = dfsG.idx_of_word[G.vertices[i].rep];
        source_connected.clear();
        for(auto vidx: dfsG.vertices[source_idx_inC].adj){
            source_connected.insert(vidx);
        }
        //cout << G.vertices[i].rep.surface << " " << dfsG.vertices.size() << endl;

        if(config.transitive == 1){
            num_trans += findTransitive(source_idx_inC, pred, entries);
        }
        else if(config.transitive == 0){
            M[i].resize(dfsG.vertices.size()); //dim2 of M = no. of vertices in context of word (dfsG)
            findCycles(dfsG, source_idx_inC); //this word is 0 in context-graph as it will be the first word added
            int cyccount = 0; //total cycle count - might be useful for debugging
            for (auto met: M[i]) { //iterate over metric objects for each target word
                cyccount += met.size(); //add the number of valid metrics found to cycle count
            }
            //cout << G.vertices[i].rep.surface << " " << cyccount << endl;
            num_trans += findTrans(source_idx_inC, pred, entries);
        }
    }
    //fout.close();
    return num_trans;
}