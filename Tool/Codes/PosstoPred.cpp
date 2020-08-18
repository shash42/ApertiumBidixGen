#include "Compare.cpp"
#include "CountByPOS.cpp"

//requires appropriate foldername with appropriate
void convert(string exptno, float threshold, int numfolders, vector<string> &foldernames){
    float pr = 1e-5;
    for(int i = 0; i < numfolders; i++){
        string dirpath = "../Results/Expts/" + exptno + "/Analysis/" + foldernames[i];
        ifstream file_poss;  ofstream file_pred;
        file_poss.open(dirpath + "/possibilities.txt");
        file_pred.open(dirpath + "/predictions.txt");
        string edge;
        while(getline(file_poss, edge)){
            for(int j = edge.length()-1; j >= 0; j--){
                if(edge[j]==' '){
                    string conf = edge.substr(j+1, edge.length());
                    float confidence = stof(conf);
                    if(confidence + pr > threshold){
                        file_pred << edge.substr(0, j) << endl;
                    }
                    break;
                }
            }
        }
        cout << foldernames[i] << " done!" << endl;
        file_poss.close(); file_pred.close();
    }
}

void predtoanalysis(string exptno, int numpairs, string l1[], string l2[]){
    for(int i = 0; i < numpairs; i++){
        string lp1 = l1[i] + "-" + l2[i];
        Compare(l1[i], l2[i], -1, exptno);
        CountbyPOS(exptno, lp1);
    }
};