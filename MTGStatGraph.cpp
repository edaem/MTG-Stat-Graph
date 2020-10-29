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
    int count, deckSize, lCount, sCount;
    int wSources, uSources, bSources, rSources, gSources;

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
    
    lCount = sCount = deckSize = 0;
    wSources = uSources = bSources = rSources = gSources = 0;
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
        deckSize += count;
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
            /* adds to the appropriate source variable */
            vector<string> ci = cards[name]["colorIdentity"];
            for(int i = 0; i < ci.size(); i++){
                //cout << "ci[" << i << "] = " << ci[i] << endl;
                if(ci[i] == "W")
                    wSources += count;
                else if(ci[i] == "U")
                    uSources += count;
                else if(ci[i] == "B")
                    bSources += count;
                else if(ci[i] == "R")
                    rSources += count;
                else if(ci[i] == "G")
                    gSources += count;
            }
            lands.insert(pair<string, int>(name, count));
            lCount += count;
        } 
        /* insert spells into the spells map */
        else{
            spells.insert(pair<string, int>(name, count));
            sCount += count;
        }
        //cout << count << " " << cards[name]["convertedManaCost"] << endl;
    }
    cout << "Land count: " << lCount << endl << "Spell cout: " << sCount << endl;
    cout << "Deck size: " << deckSize << endl;
    cout << "White sources: " << wSources << endl;
    cout << "Blue sources: " << uSources << endl;
    cout << "Black sources: " << bSources << endl;
    cout << "Red sources: " << rSources << endl;
    cout << "Green sources: " << gSources << endl;
}