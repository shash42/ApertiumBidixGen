#include "Compare.cpp"
#include "CountByPOS.cpp"

void convert(string exptno, float threshold, bool compare=false){
    float pr = 1e-5;
    int numpairs = 11;
    string l1[] = {"en", "en", "fr", "fr", "eo", "eo", "eo", "eo", "oc", "oc", "oc"};
    string l2[] = {"es", "ca", "es", "ca", "fr", "ca", "en", "es", "ca", "es", "fr"};
    for(int i = 0; i < numpairs; i++){
        string lp1 = l1[i] + "-" + l2[i], lp2= l2[i] + "-" + l1[i]; //language pair to get predictions for
        string dirpath = "../Results/Expts/" + exptno + "/Analysis/" + lp1;
        string prefix= "rem_" + lp1; //output file
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
        cout << "done" << endl;
        file_poss.close(); file_pred.close();

        if(compare) {
            //Do analysis of predictions
            Compare C(l1[i], l2[i], i, exptno);
            CountbyPOS cntPOS(exptno, lp1);
        }
    }
}