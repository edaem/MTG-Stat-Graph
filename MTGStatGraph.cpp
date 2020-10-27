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

    /* load cards.json */
    in.open("cards.json");
    in >> cards;
    in.close();

    /* exits if there isn't at least 1 argument */
    if(argc < 2){
        exit(1);
    }

    /* opens given decklist file */
    deckFileName = argv[1];
    in.open(deckFileName);
    
    /* procceses each line of the file */
    while(getline(in, line)){
        stringstream ls, ns;

        /* breaks on a blank line (indicates that the sideboard follows) */
        if(line.length() <= 1)
            break;
        
        /* checks if the file is using Window's style line breaks, removes extraneous character if so */
        if(line[line.length() - 1] == '\r'){
            line.erase(line.length() - 1, 1);
        }
        //cout << line << " ";

        /* get the amount of the card in deck */
        ls << line;
        ls >> count;

        /* rest of the line is the card name, extracts it */
        while(!ls.eof()){
            string temp;
            ls >> temp;
            ns << temp;
            if(!ls.eof())
                ns << " ";
        }
        string name = ns.str();
        string type = cards[name]["type"];
        /* insert lands into the lands map */
        if(type.find("Land") != string::npos){
            
            lands.insert(pair<string, int>(name, count));
        } 
        /* insert spells into the spells map */
        else{
            cout << name << " is a spell\n";
            spells.insert(pair<string, int>(name, count));
        }
        //cout << count << " " << cards[name]["convertedManaCost"] << endl;
    }
    cout << "Land count: " << lands.size() << endl << "Spell cout: " << spells.size() << endl;
}