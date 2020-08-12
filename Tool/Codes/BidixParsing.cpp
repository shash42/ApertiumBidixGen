#include "pugixml-1.10/src/pugixml.cpp"
#include<fstream>
#include<iostream>
#include<vector>
#include<sstream>
#include "filesystem.hpp"
#include "Graph.h"

using namespace std;
using namespace pugi;
namespace fs = std::filesystem;

map<string, string> langcode2to3;

void loadlangcodes(){
    ifstream flang; flang.open("../LangData/ISO639 - Sheet1.tsv");
    string line, cell;
    int linenum = 0;
    while(getline(flang, line, '\n')){
        linenum++;
        if(linenum==1) continue; //ignore first line of the tsv
        /*for(int i = 0; i < line.length(); i++){
            if(line[i]=='\t'){
                cout << "woot" << " ";
            }
        }*/
        line = line.substr(0, line.length()-1); //clean last erroneous character
        istringstream entry(line);
        int colnum=0;
        string let2, let3;
        while(getline(entry, cell, '\t')){
            colnum++;
            if(colnum%3==1) continue; //ignore the name
            else if(colnum%3==2) let2=cell;
            else if(colnum%3==0) let3=cell;
        }
        langcode2to3[let2] = let3;
    }
    flang.close();
}

class BidixParsing{
    map<string, string> ApertToLex;
    string l1, l2;
    vector< pair<wordData, wordData> > translations;
    int num_entries = 0;

    string getMapTag(string &str);
    void populate_POS_Map();
    void getPOS(wordData &word, string &nval);
    wordData getWord(xml_node w, string &lang);
public:
    BidixParsing(){
        populate_POS_Map();
    }
    void run(string _l1, string _l2);
    void Output();
};

string BidixParsing::getMapTag(string &str){
    string ret;
    int idx = 0;
    while(idx < str.length()){
        if(str[idx]==':'){
            idx++;
            if(str[idx]==':') idx++;
            ret = str.substr(idx, str.length());
        }
        idx++;
    }
    return ret;
}

void BidixParsing::populate_POS_Map(){
    string path = "../LangData/apertium-lexinfo-tags-mapping.csv";
    ifstream fin; fin.open(path);
    string line;
    int num = 0;
    while(getline(fin, line)){
        num++;
        if(num<=3) continue;
        istringstream str(line);
        vector<string> v;
        string info;
        while(getline(str, info, ',')){
            v.push_back(info);
        }
        if(!v[3].empty()){
            string apert = getMapTag(v[0]), lex = getMapTag(v[2]);
          //  cout << apert << " " << lex << endl;
            if(lex[0]=='v' && lex[1]=='b') lex = "verb"; //manual change, get every vb to verb.
            ApertToLex[apert] = lex;
        }
    }
    //Manual additions:
    ApertToLex["abbr"] = "properNoun";
}

void BidixParsing::getPOS(wordData &word, string &nval){
    string _info, info;
    if(nval[0]==':') nval = nval.substr(1, nval.length()); //edge-case for nno-nob pair.
    istringstream str(nval);
    string fullpos;
    bool _start=true;
    while(getline(str, _info, '_')){
        if(!_start) fullpos += '_';
        _start=false;
        stringstream tempss = stringstream(_info); //added second check for - to handle edge-case as in pol-szl
        bool dashstart = true;
        while(getline(tempss, info, '-')){
            if(!dashstart) fullpos += '-';
            dashstart = false;
            if(info.empty()) continue;
            fullpos += info;
            if(ApertToLex.find(info)!=ApertToLex.end()) word.pos = ApertToLex[info];
            if(word.pos=="none" && ApertToLex.find(fullpos)!=ApertToLex.end()) word.pos = ApertToLex[fullpos];
        }
    }
}

wordData BidixParsing::getWord(xml_node w, string &lang){
    wordData word;
    word.lang = lang;
    if(langcode2to3.find(lang)!=langcode2to3.end()) word.lang = langcode2to3[lang];
    word.pos = "none";
    for(xml_node chtag = w.first_child(); chtag; chtag = chtag.next_sibling()){
        string tagname = chtag.name();
        if(tagname.empty()){
            if(!word.word_rep.empty()) word.word_rep += " ";
            word.word_rep += chtag.value();
        }
        if(tagname == "g"){
            for(xml_node gch = chtag.first_child(); gch; gch = gch.next_sibling()){
                string gtag = gch.name();
                if(gtag.empty()){
                    if(!word.word_rep.empty()) word.word_rep += " ";
                    word.word_rep += gch.value();
                }
            }
        }
        else if(word.pos=="none" && tagname == "s"){
            string nval = chtag.attribute("n").value();
            if(!nval.empty()){
                getPOS(word, nval);
            }
        }
    }
    word.makesurface();
    return word;
}

void BidixParsing::Output(){
    cout << num_entries << endl;
    //int corr = 0, err = 0, useful = 0;
    //string correctpath = "../LangData/Parsed/Correct/" + l1 + "-" + l2 + ".txt";
    //string errorpath = "../LangData/Parsed/Error/" + l1 + "-" + l2 + ".txt";
    fs::create_directory("../LangData/Parsed");
    string usefulpath = "../LangData/Parsed/" + l1 + "-" + l2 + ".txt";
    //ofstream fout; fout.open(correctpath);
    //ofstream ferr; ferr.open(errorpath);
    ofstream fuse; fuse.open(usefulpath);
    for(auto &t: translations){
        if(t.first.pos!="none" && t.second.pos!="none" && !t.first.word_rep.empty() && !t.second.word_rep.empty()){
            fuse << t.first.surface << " " << t.second.surface << endl;
            // useful++;
        }
      /*  if(t.first.pos != t.second.pos || t.first.pos=="none" || t.first.word_rep.empty() || t.second.word_rep.empty()){
            ferr << t.first.surface <<  " " << t.second.surface <<  endl;
            err++;
        }
        else{
            corr++;
            fout << t.first.surface <<  " " << t.second.surface <<  endl;
        }*/
    }
//    cout << useful << " " << corr << " " << err << endl;
}

void BidixParsing::run(string _l1, string _l2){
    l1 = _l1; l2 = _l2; //get passed language pair
    translations.clear(); num_entries = 0; //remove data from old runs
    xml_document doc;
    string strpath = "../LangData/Raw/apertium-" + l1 + "-" + l2 + "." + l1 + "-" + l2 + ".dix";
    const char* path = const_cast<char *>(strpath.c_str()); //convert path to const char* as required for result
    xml_parse_result result = doc.load_file(path);
    //cout << result.description() << endl << result.offset << endl; //used to give error details and last char
    xml_node dict = doc.first_child();//dict tag

    for(xml_node child = dict.first_child(); child; child = child.next_sibling()){
        if((string) child.name()!="section") continue; //iterate over sections
        for(xml_node e = child.child("e"); e; e = e.next_sibling("e")){
            num_entries++; //iterate over entries
            wordData left, right; bool valid = false; //valid is used to mark correctly parsed entries(?)
            for(xml_node ech = e.first_child(); ech; ech = ech.next_sibling()){
                string tagname = ech.name(); //iterate over child nodes of <e> tag

                if(tagname=="p"){
                    xml_node p = e.first_child();
                    left = getWord(p.child("l"), l1); right = getWord(p.child("r"), l2);
                    //cout << left.first_child().value() << " " << right.first_child().value() << endl;
                    valid = true;
                }
                else if(tagname=="i"){
                    xml_node i = e.first_child();
                    left = getWord(i, l1); right = getWord(i, l1);
                    valid = true;
                }

                else if((left.pos=="none" || right.pos=="none") && tagname=="par"){
                    string nval = ech.attribute("n").value();
                    if(!nval.empty()){
                        getPOS(left, nval);   getPOS(right, nval);
                        left.makesurface(); right.makesurface();
                    }
                }
            }
            if(valid){
                translations.push_back({left, right});
            }
        }
    }
}

int main(){
    loadlangcodes(); //load 2 digit to 3 digit language code table
    ifstream fin; fin.open("../LangData/langlist.txt"); //list of languages to parse
    //freopen("../LangData/Analysis.txt", "w", stdout);
    BidixParsing BP;
    while(!fin.eof()){
        string l1, l2;
        fin >> l1 >> l2;
        if(l1.length()==0 || l2.length()==0) continue; //added check (ignore) for empty lines/langnames
        //cout << l1 << " " << l2 << endl;
        cerr << l1 << " " << l2 << endl;
        BP.run(l1, l2); //run parsing for the given language pair
        BP.Output(); //output the parse results
    }
}