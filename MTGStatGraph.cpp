#include "json.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <sstream>
#include <utility>

using namespace std;
using json = nlohmann::json;

int main(int argc, char* argv[]){
    ifstream in;
    json cards;
    string deckFileName;
    string line;
    map<string, int> lands, spells;
    
    int count;

    in.open("cards.json");
    in >> cards;
    in.close();

    if(argc < 2){
        exit(1);
    }

    deckFileName = argv[1];
    in.open(deckFileName);
    
    while(getline(in, line)){
        stringstream ls, ns;
        if(line.length() <= 1)
            break;
        if(line[line.length() - 1] == '\r'){
            line.erase(line.length() - 1, 1);
        }
        //cout << line << " ";
        ls << line;
        ls >> count;
        while(!ls.eof()){
            string temp;
            ls >> temp;
            ns << temp;
            if(!ls.eof())
                ns << " ";
        }
        string name = ns.str();
        string type = cards[name]["type"];
        if(type.find("Land") != string::npos){
            
            lands.insert(pair<string, int>(name, count));
        } else{
            cout << name << " is a spell\n";
            spells.insert(pair<string, int>(name, count));
        }
        //cout << count << " " << cards[name]["convertedManaCost"] << endl;
    }
    cout << "Land count: " << lands.size() << endl << "Spell cout: " << spells.size() << endl;
}