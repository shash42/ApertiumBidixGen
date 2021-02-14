//Compare OTIC predictions with input language pairs

#include "../Graph.cpp"
//#include "../Biconnected.cpp"
#include "../DensityAlgo.cpp"
#include "../Compare.cpp"
#include "../CountByPOS.cpp"
#include "../callers.cpp"
#include "../PosstoPred.cpp"

#include<chrono>
#include<experimental/filesystem>

#include<algorithm>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std::chrono;
namespace fs = std::experimental::filesystem;

int main(int argc, char*argv[])
{
    if(argc!=2){
        cerr << "Usage: ./compare <path-to-expt-folder>" << endl;
        return 0;
    }
    int N = 2; //modify - number of language pairs
    /*vector<pair<string, string>> langpairs = {{"eng", "spa"}, {"eng", "cat"}, {"epo", "cat"}, {"epo", "eng"}, 
    {"epo", "fra"}, {"epo", "spa"}, {"fra", "cat"}, {"fra", "spa"}, {"oci", "cat"},
     {"oci", "fra"}, {"oci", "spa"}};*/
    vector<pair<string, string>> langpairs = {{"fra", "spa"}, {"eng", "cat"}}; //list language pairs
    /*vector<pair<string, string>> langpairs;
    ifstream inp; inp.open("LangData/useful3let.txt");
    for(int i = 0; i < N; i++){
        string a, b; inp >> a >> b;
        langpairs.push_back({a, b});
    }*/
    for(int i = 0; i < N; i++){
        string lang = langpairs[i].first + "-" + langpairs[i].second;
        string pred = argv[1] + lang + "/predictions.txt";
        string orig = "LangData/RDFParsed/" + lang + ".txt"; //modify - folder where original data (target bidix) is situated
        string used =  "sampleconfigs/OTIC-paths/" + lang + "/langpathlist.txt"; //modify - path to file where used bidix paths are listed
        string prefix = argv[1] + lang + "/";
        int num_used = 2; //number of input language pairs used
        Compare Cx(langpairs[i].first, langpairs[i].second, pred, orig, used, num_used, prefix);
        CountbyPOS CPx(lang, orig, prefix);
    }
}
