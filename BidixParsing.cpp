#include "pugixml-1.10/src/pugixml.cpp"
#include<fstream>
#include<iostream>
#include<vector>
#include<sstream>
#include "Graph.h"

using namespace std;
using namespace pugi;

map<string, string> ApertToLex;

string getMapTag(string &str){
    string ret;
    int idx = 0;
    while(idx < str.length()){
        if(str[idx]==':'){
            idx++;
            if(str[idx]==':') idx++;
            ret = str.substr(idx, str.length());
        }
    }
    return ret;
}

void populate_POS_Map(){
    string path = "../Bidixes/apertium-lexinfo-mapping.csv";
    ifstream fin; fin.open(path);
    string line;
    while(getline(fin, line)){
        istringstream str(line);
        vector<string> v;
        string info;
        while(getline(str, info, ',')){
            cout << info << endl;
            v.push_back(info);
        }
        if(!v[3].empty()){
            string apert = getMapTag(v[0]), lex = getMapTag(v[2]);
            cout << apert << " " << lex << endl;
            ApertToLex[apert] = lex;
        }
    }
}
void getPOS(wordData &word, string &nval){
    string info;
    istringstream str(nval);
    while(getline(str, info, '_')){
        if(info[0] == '_') info = info.substr(1, info.length());
        if(ApertToLex.find(info)!=ApertToLex.end()) word.pos = ApertToLex[info];
    }
}

wordData getWord(xml_node w, string &lang){
    wordData word;
    word.lang = lang; word.pos = "none";
    for(xml_node chtag = w.first_child(); chtag; chtag = chtag.next_sibling()){
        string tagname = chtag.name();
        if(tagname.empty()){
            if(!word.word_rep.empty()) word.word_rep += " ";
            word.word_rep += chtag.value();
        }
        else if(tagname == "s"){
            string nval = chtag.attribute("n").value();
            if(!nval.empty()){
                getPOS(word, nval);
            }
        }
    }
    word.makesurface();
    return word;
}

int main(){
    string l1 = "eng", l2 = "cat";
    xml_document doc;
    xml_parse_result result = doc.load_file("../Bidixes/Raw/apertium-eng-cat.eng-cat.dix");
    cout << result.description() << endl;
    cout << result.offset << endl;
    populate_POS_Map();
    vector< pair<wordData, wordData> > translations;
    xml_node dict = doc.first_child();
    for(xml_node child = dict.first_child(); child; child = child.next_sibling()){
        if((string) child.name()!="section") continue;
        for(xml_node e = child.child("e"); e; e = e.next_sibling("e")){

            wordData left, right; bool valid = false;
            for(xml_node ech = e.first_child(); ech; ech = ech.next_sibling()){
                string tagname = ech.name();
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

                else if(tagname=="par"){
                    string nval = ech.attribute("n").value();
                    if(!nval.empty()){
                        getPOS(left, nval);   getPOS(right, nval);
                    }
                }
            }
            if(valid){
                translations.push_back({left, right});
            }
        }
    }
    cout << translations.size() << endl;
    int i = 0;
    freopen("../Bidixes/Parsed/tempparse-eng-cat.txt", "w", stdout);
    for(auto t: translations){
        i++;
        cout << i << ": " << t.first.surface <<  " " << t.second.surface <<  endl;
    }
}