#include "pugixml-1.10/src/pugixml.cpp"
#include<iostream>
#include<vector>
#include "filesystem.hpp"
#include "Graph.h"
#include "Convert2to3.cpp"

using namespace std;
using namespace pugi;
namespace fs = std::filesystem;

//Class used to parse .dix bidixes to .txt internally usable format for the algorithm
class BidixParsing{
    map<string, string> ApertToLex; //used to store mapping from apertium tag to actual POS
    string l1, l2; //the langauge names
    vector< pair<wordData, wordData> > translations; //contains all extracted translations
    int num_entries = 0; //number of entries in the bidix

    string getMapTag(string &str); //get the actual apertium/lexinfo tag from the CSV
    void populate_POS_Map(); //populate ApertToLex using CSV
    void getPOS(wordData &word, string &nval); //assigned POS based on nval to the word object
    wordData getWord(xml_node w, string &lang, LangCodes &LC); //get a wordData object from the xml <l>, <r> or <i> tag
public:
    BidixParsing(){ //constructor
        populate_POS_Map();
    }
    void run(string path, string _l1, string _l2, LangCodes &LC); //Parse the bidix of l1-l2
    void Output(); //Generate the output after parsing
};

//Get the actual reqd info from the passed string (CSV cell value column 1, 3)
string BidixParsing::getMapTag(string &str){
    string ret;
    int idx = 0;
    while(idx < str.length()){
        if(str[idx]==':'){ //in the CSV the format is apertium:X or lexinfo:X
            idx++;
            if(str[idx]==':') idx++; //sometimes theres 2 : (as in ::)
            ret = str.substr(idx, str.length()); //the rest of the string is the tag
        }
        idx++;
    }
    return ret;
}

//Map from apertium tag to POS [using an external lexinfo csv for mapping]
void BidixParsing::populate_POS_Map(){
    string path = "../LangData/apertium-lexinfo-tags-mapping.csv";
    ifstream fin; fin.open(path);
    string line;
    int num = 0;
    while(getline(fin, line)){
        num++;
        if(num<=3) continue; //first 3 lines are meta information so ignore
        istringstream str(line);
        vector<string> v; //stores all the columns of this line
        string info;
        while(getline(str, info, ',')){ //get data from CSV
            v.push_back(info);
        }
        if(!v[3].empty()){ //if the last column is not empty it is a POS tag
            string apert = getMapTag(v[0]), lex = getMapTag(v[2]); //get the apert name and the lexinfo POS of the entries
          //  cout << apert << " " << lex << endl;
            if(lex[0]=='v' && lex[1]=='b') lex = "verb"; //manual change, get every vb to verb.
            ApertToLex[apert] = lex; //assign the apertium tag to the lexinfo POS
        }
    }
    //Manual additions: Some edge-cases based on aanlyzed bidixes
    ApertToLex["abbr"] = "properNoun";
}

//get the acutaly POS using the wordData and passed apertium tag(s) in n=""
void BidixParsing::getPOS(wordData &word, string &nval){
    string _info, info;
    if(nval[0]==':') nval = nval.substr(1, nval.length()); //edge-case for nno-nob pair.
    istringstream str(nval);
    string fullpos;
    bool _start=true;
    while(getline(str, _info, '_')){ //multiple tags are often separated by '__'
        if(!_start) fullpos += '_';
        _start=false;
        stringstream tempss = stringstream(_info); //added second check for - to handle edge-case as in pol-szl
        bool dashstart = true;
        while(getline(tempss, info, '-')){ //in pol-szl they are separated by '-' instead, so handling that
            if(!dashstart) fullpos += '-';
            dashstart = false;
            if(info.empty()) continue;
            fullpos += info; //add the info (tag) to the list
            if(ApertToLex.find(info)!=ApertToLex.end()) word.pos = ApertToLex[info]; //if the extracted tag can be mapped to a POS, do it
            //otherwise if the POS is still empty and prefix till now can be mapped, do it
            if(word.pos=="none" && ApertToLex.find(fullpos)!=ApertToLex.end()) word.pos = ApertToLex[fullpos];
        }
    }
}

//Get the word representation and POS from xml node (language has to be passed as parameter can be 2 letter code)
wordData BidixParsing::getWord(xml_node w, string &lang, LangCodes &LC){
    wordData word; //word object that will be returned
    word.lang = lang; //assign the 3 letter lang code
    word.pos = "none"; //initially POS is none
    for(xml_node chtag = w.first_child(); chtag; chtag = chtag.next_sibling()){ //iterate through children node in xml
        string tagname = chtag.name();
        if(tagname.empty()){ //if the tagname is empty, it's a word representation
            if(!word.word_rep.empty()) word.word_rep += " "; //if it's not currently empty, add a space
            word.word_rep += chtag.value(); //add the tag value to the word representation
        }
        if(tagname == "g"){ //if it's a g tag, that means another level of iteration is required
            for(xml_node gch = chtag.first_child(); gch; gch = gch.next_sibling()){
                string gtag = gch.name(); //name of child tag of g
                if(gtag.empty()){ //same stuff as getting word representation above
                    if(!word.word_rep.empty()) word.word_rep += " ";
                    word.word_rep += gch.value();
                }
            }
        }
        else if(word.pos=="none" && tagname == "s"){ //if the word pos is empty and s tag get POS (so only first s used)
            string nval = chtag.attribute("n").value();
            if(!nval.empty()){
                getPOS(word, nval);
            }
        }
    }
    word.makesurface();
    return word;
}

//Print parsed output into target file
void BidixParsing::Output(){
    cout << num_entries << endl; //output number of entries {temporary}
    //int corr = 0, err = 0, useful = 0; //number of correct, error, useful entries
    //string correctpath = "../LangData/Parsed/Correct/" + l1 + "-" + l2 + ".txt"; //correct entries
    //string errorpath = "../LangData/Parsed/Error/" + l1 + "-" + l2 + ".txt"; //error entries
    fs::create_directory("../LangData/Parsed"); //create the directory
    string usefulpath = "../LangData/Parsed/" + l1 + "-" + l2 + ".txt";
    //ofstream fout; fout.open(correctpath);
    //ofstream ferr; ferr.open(errorpath);
    ofstream fuse; fuse.open(usefulpath);
    for(auto &t: translations){ //iterate over translations
        if(t.first.pos!="none" && t.second.pos!="none" && !t.first.word_rep.empty() && !t.second.word_rep.empty()){
            //if the POS on both sides has been successfully extracted and the representation is not empty
            fuse << t.first.surface << " " << t.second.surface << endl; //output to the target file
            // useful++;
        }
      /*  if(t.first.pos != t.second.pos || t.first.pos=="none" || t.first.word_rep.empty() || t.second.word_rep.empty()){
       * //if the POS mismatches, or not succesfully extracted, or representation is empty
            ferr << t.first.surface <<  " " << t.second.surface <<  endl; //output to error file
            err++;
        }
        else{ //purely correct (no POS-mismatch also)
            corr++;
            fout << t.first.surface <<  " " << t.second.surface <<  endl; //output to correct file
        }*/
    }
//    cout << useful << " " << corr << " " << err << endl;
}

//Pass the original form itself (2 letter codes allowed, converts to 3 internally)
void BidixParsing::run(string strpath, string _l1, string _l2, LangCodes &LC){
    translations.clear(); num_entries = 0; //remove data from old runs
    xml_document doc;
    const char* path = const_cast<char *>(strpath.c_str()); //convert path to const char* as required for result
    xml_parse_result result = doc.load_file(path);
    //cout << result.description() << endl << result.offset << endl; //used to give error details and last char
    xml_node dict = doc.first_child();//dict tag
    l1 = _l1; if(LC.langcode2to3.find(_l1)!=LC.langcode2to3.end()) l1 = LC.langcode2to3[_l1]; //convert to 3 letter code
    l2 = _l2; if(LC.langcode2to3.find(_l2)!=LC.langcode2to3.end()) l2 = LC.langcode2to3[_l2]; //convert to 3 letter code

    for(xml_node child = dict.first_child(); child; child = child.next_sibling()){
        if((string) child.name()!="section") continue; //iterate over sections
        for(xml_node e = child.child("e"); e; e = e.next_sibling("e")){
            num_entries++; //iterate over entries
            wordData left, right; bool valid = false; //valid is used to mark correctly parsed entries(?)
            for(xml_node ech = e.first_child(); ech; ech = ech.next_sibling()){
                string tagname = ech.name(); //iterate over child nodes of <e> tag

                if(tagname=="p"){ //paragraph tag, has l, r inside it
                    xml_node p = e.first_child();
                    left = getWord(p.child("l"), l1, LC); right = getWord(p.child("r"), l2, LC);
                    //cout << left.first_child().value() << " " << right.first_child().value() << endl;
                    valid = true;
                }
                else if(tagname=="i"){ //identity tag, both left and right half have same representation
                    xml_node i = e.first_child();
                    left = getWord(i, l1, LC); right = getWord(i, l1, LC);
                    valid = true;
                }

                else if((left.pos=="none" || right.pos=="none") && tagname=="par"){ //if POS is empty, maybe it's in par tag
                    string nval = ech.attribute("n").value(); //accessing the n part of <par n="info">
                    if(!nval.empty()){ //if it's not empty
                        getPOS(left, nval);   getPOS(right, nval); //extract POS
                        left.makesurface(); right.makesurface(); //necessary to do whenever info of wordData object is changed
                    }
                }
            }
            if(valid){
                translations.push_back({left, right}); //if it's a valid translation entry push it into the list
            }
        }
    }
}

int main(int argc, char *argv[]){
    if(argc!=2){
        cerr << "1 argument, path to language-pair list required." << endl;
        return 0;
    }
    if(!fs::exists(argv[1])){
        cerr << "Language-pair list file path not found!!" << endl;
        return 0;
    }
    LangCodes LC; //load 2 digit to 3 digit language code table
    ifstream fin; fin.open(argv[1]); //list of languages to parse
    //freopen("../LangData/Analysis.txt", "w", stdout);
    BidixParsing BP;
    while(!fin.eof()){
        string path, l1, l2;
        getline(fin, path);
        getline(fin, l1, ' '); getline(fin, l2);
        if(l1.length()==0 || l2.length()==0) continue; //added check (ignore) for empty lines/langnames
        //cout << l1 << " " << l2 << endl;
        cout << l1 << " " << l2 << endl;
        BP.run(path, l1, l2, LC); //run parsing for the given language pair
        BP.Output(); //output the parse results
    }
}