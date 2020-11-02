#include "json.hpp"
#include "hypergeometric.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <sstream>
#include <utility>

using namespace std;
using json = nlohmann::json;

void graphcolor(ostream& out, string color, double rgb[], int deckSize, int sources){
    out << "xaxis min 1 max 10 hash 1 mhash 0\nlabel : Turn #\n";
    out << "yaxis min 0 max 100\nlabel : % Chance of Having " << color << " Mana\n";
    out << "newcurve color " << rgb[0] << " " << rgb[1] << " " << rgb[2] << " linetype solid pts ";
    for(int i = 0; i < 10; i++){
        out << i+1 << " " << hdgte(deckSize, sources, 7+i, 1) * 100 << " ";
    }
    out << "newcurve color " << rgb[0] << " " << rgb[1] << " " << rgb[2] << " linetype dashed pts ";
    for(int i = 1; i < 10; i++){
        out << i+1 << " " << hdgte(deckSize, sources, 7+i, 2) * 100 << " ";
    }
    out << "newcurve color " << rgb[0] << " " << rgb[1] << " " << rgb[2] << " linetype dotdotdash pts ";
    for(int i = 2; i < 10; i++){
        out << i+1 << " " << hdgte(deckSize, sources, 7+i, 3) * 100 << " ";
    }
    out << endl;
}

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
    ofstream out("test.jgr");

    /* Land Drop Graph */
    out << "newgraph\n x_translate -4.2 y_translate 4\n";
    out << "xaxis min 1 max 10 hash 1 mhash 0\nlabel : Turn #\n" ;
    out << "yaxis min 0 max 100\nlabel : % Chance of Hitting Land Drop\n";
    out << "newcurve linetype solid pts ";
    for(int i = 0; i < 10; i++){
        out << i+1 <<" " << hdgte(deckSize, lCount, 7+i, i+1) * 100 << " ";
    }
    out << endl;
    double xtrans = -0.6, ytrans = 4;
    if(wSources > 0){
        out << "newgraph\n x_translate " << xtrans << " y_translate " << ytrans << endl;
        double rgb[3] = {1,1,0};
        graphcolor(out, "White", rgb, deckSize, wSources);
        if(xtrans < 3)
            xtrans += 3.8;
        else{
            ytrans -= 4;
            xtrans = -4.2;
        }
    }
    if(uSources > 0){
        out << "newgraph\n x_translate " << xtrans << " y_translate " << ytrans << endl;
        double rgb[3] = {0,0,1};
        graphcolor(out, "Blue", rgb, deckSize, uSources);
        if(xtrans < 3)
            xtrans += 3.8;
        else{
            ytrans -= 4;
            xtrans = -4.2;
        }
    }
    if(bSources > 0){
        out << "newgraph\n x_translate " << xtrans << " y_translate " << ytrans << endl;
        double rgb[3] = {.5,0,.5};
        graphcolor(out, "Black", rgb, deckSize, bSources);
        if(xtrans < 3)
            xtrans += 3.8;
        else{
            ytrans -= 4;
            xtrans = -4.2;
        }
    }
    if(rSources > 0){
        out << "newgraph\n x_translate " << xtrans << " y_translate " << ytrans << endl;
        double rgb[3] = {1,0,0};
        graphcolor(out, "Red", rgb, deckSize, rSources);
        if(xtrans < 3)
            xtrans += 3.8;
        else{
            ytrans -= 4;
            xtrans = -4.2;
        }
    }
    if(gSources > 0){
        out << "newgraph\n x_translate " << xtrans << " y_translate " << ytrans << endl;
        double rgb[3] = {0,1,0};
        graphcolor(out, "Green", rgb, deckSize, gSources);
        if(xtrans < 3.4)
            xtrans += 3.8;
        else{
            ytrans -= 4;
            xtrans = -4.2;
        }
    }
    cout << "Land count: " << lCount << endl << "Spell count: " << sCount << endl;
    cout << "Deck size: " << deckSize << endl;
    cout << "White sources: " << wSources << endl;
    cout << "Blue sources: " << uSources << endl;
    cout << "Black sources: " << bSources << endl;
    cout << "Red sources: " << rSources << endl;
    cout << "Green sources: " << gSources << endl;
}