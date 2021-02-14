//Converts a possibilities file to a predictions file

#include "Compare.cpp"
#include "CountByPOS.cpp"
#include "Graph.cpp"
#include <iomanip>

void insertbtag(string &str){
    int i=0;
    while(i < str.length()){
        if(str[i]==' '){
            int rem_len = str.length()-(i+1);
            str = str.substr(0, i) + "<b/>" + str.substr(i+1, rem_len);
        }
        i++;
    }
}
void predictionout(ofstream &file_pred, string edgeline, int num_info, float confidence){
    int i = 0;
    wordData SLw(i, edgeline, num_info);
    wordData TLw(i, edgeline, num_info);
    insertbtag(SLw.word_rep); insertbtag(TLw.word_rep);
    if(SLw.word_rep != TLw.word_rep){
        file_pred << "<e a=\"BidixGen\" c=\"score:" << confidence << "\">\t<p><l>" << SLw.word_rep <<  "<s n=\"" << SLw.pos << "\"/></l>\t";
        file_pred << "<r>" << TLw.word_rep <<  "<s n=\"" << TLw.pos << "\"/></r></p></e>\n";
    }
    else{
        file_pred << "<e a=\"BidixGen\"  c=\"score:" << confidence << "\">\t<p><i>" << SLw.word_rep <<  "<s n=\"" << SLw.pos << "\"/></i></p></e>\n";
    }
}

//requires appropriate foldername with appropriate
void convert(string exptno, float threshold, int numfolders, vector<string> &foldernames, bool bidixoutput){
    int num_info = 3; //number of columns to extract per word
    float pr = 1e-5;
    for(int i = 0; i < numfolders; i++){
        string dirpath = "Results/Expts/" + exptno + "/Analysis/" + foldernames[i];
        ifstream file_poss;  ofstream file_pred;
        file_poss.open(dirpath + "/possibilities.txt");
        file_pred.open(dirpath + "/predictions.txt");
        file_pred << setprecision(2) << fixed;
        string edge;
        while(getline(file_poss, edge)){
            for(int j = edge.length()-1; j >= 0; j--){
                if(edge[j]=='\t'){
                    string conf = edge.substr(j+1, edge.length());
                    float confidence = stof(conf);
                    if(confidence + pr > threshold){
                        if(bidixoutput) predictionout(file_pred, edge.substr(0, j), num_info, confidence);
                        else file_pred << edge.substr(0, j) << endl;
                    }
                    break;
                }
            }
        }
        cerr << foldernames[i] << " done!" << endl;
        file_poss.close(); file_pred.close();
    }
}