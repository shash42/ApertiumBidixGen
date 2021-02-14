//Generate predictions for an OTIC possiblities file

#include<bits/stdc++.h>

using namespace std;

//requires appropriate foldername with appropriate
void convert(string exptno, float threshold, int numfolders, vector<string> &foldernames, vector<string> &l1, vector<string> &l2){
    int num_info = 3; //number of columns to extract per word
    float pr = 1e-5;
    for(int i = 0; i < numfolders; i++){
        string dirpath = exptno + foldernames[i];
        ifstream file_poss;  ofstream file_pred;
        file_poss.open(dirpath + "/possibilities.txt");
        file_pred.open(dirpath + "/predictions.txt");
        file_pred << setprecision(2) << fixed;
        string edge;
        int col=0;
        string cell, sw, tw, pos, conf;
        while(getline(file_poss, edge)){
            for(int j = 0; j < edge.length(); j++){
                if(edge[j]=='\t'){
                    if(col==0){
                        sw = cell;
                    }   
                    else if(col==1){
                        tw = cell;
                    }
                    else if(col==2){
                        pos = cell;
                    }
                    else if(col==4){
                        conf = cell;
                        float confidence = stof(conf);
                        if(confidence + pr > threshold){
                            file_pred << sw << "\t" << pos << "\t" << l1[i] << "\t" << tw << "\t" << pos << "\t" << l2[i] << endl;
                        }
                        col=0;
                        cell="";
                        break;
                    }
                    col++;
                    cell = "";
                }
                else{
                    cell += edge[j];
                }
            }
        }
        cerr << foldernames[i] << " done!" << endl;
        file_poss.close(); file_pred.close();
    }
}

int main(int argc, char*argv[])
{
    if(argc!=2){
        cerr << "Usage: " << argv[0] <<  " <path-to-results-folder>" << endl;
        return 0;
    }
    float threshold = 0.6; //confidence threshold
    int numfolders = 2; //modify - number of language pairs to generated predictions for
    vector<string> foldernames = {"fra-spa", "eng-cat"}; //modify - list of folder names to get preds for
    vector<string> l1 = {"fra", "eng"}, l2 = {"spa", "cat"}; //modify - language 1 and language 2 in same order as above.
    convert(argv[1], threshold, numfolders, foldernames, l1, l2);
}