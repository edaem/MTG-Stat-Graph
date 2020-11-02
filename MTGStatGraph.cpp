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

/* outputs jgraph for a line graph for the given color */
void graphcolor(ostream& out, string color, double rgb[], int deckSize, int sources){
    out << "xaxis min 1 max 10 hash 1 mhash 0\nlabel : Turn #\n";
    out << "yaxis min 0 max 100\nlabel : % Chance of Having " << color << " Mana\n";
    out << "newcurve color " << rgb[0] << " " << rgb[1] << " " << rgb[2];
    out << " linetype solid label : One source\n pts ";
    for(int i = 0; i < 10; i++){
        out << i+1 << " " << hdgte(deckSize, sources, 7+i, 1) * 100 << " ";
    }
    out << endl << "newcurve color " << rgb[0] << " " << rgb[1] << " " << rgb[2];
    out << " linetype dashed label : Two sources \n pts ";
    for(int i = 1; i < 10; i++){
        out << i+1 << " " << hdgte(deckSize, sources, 7+i, 2) * 100 << " ";
    }
    out << endl << "newcurve color " << rgb[0] << " " << rgb[1] << " " << rgb[2];
    out << " linetype dotdotdash label : Three sources\npts ";
    for(int i = 2; i < 10; i++){
        out << i+1 << " " << hdgte(deckSize, sources, 7+i, 3) * 100 << " ";
    }
    out << endl << "legend x 7 y 20\n";
}

/* creates a bar that has width equal to the percentage of partial/total */
int graphbar(ostream& out, double rgb[], double x, double y, double partial, double total){
    double size = (partial/total) * 100;
    out << "newline poly pcfill " << rgb[0] << " " << rgb[1] << " " << rgb[2];
    out << " pts " << x << " " << y << " " << x << " " << y-20 << " " << x+size; 
    out << x + size << " " << y-20 << " " << x + size << " " << y << endl;

    /* returns where the next bar should be printed */
    return size + x;
}

int main(int argc, char* argv[]){
    ifstream in;
    json cards;
    string deckFileName;
    string line;
    int count, deckSize, lCount, sCount;
    int wSources, uSources, bSources, rSources, gSources;
    int wSpells, uSpells, bSpells, rSpells, gSpells;

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
    wSpells = uSpells = bSpells = rSpells = gSpells = 0;
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
            lCount += count;
        } 
        /* insert spells into the spells map */
        else{
            vector<string> ci = cards[name]["colorIdentity"];
            for(int i = 0; i < ci.size(); i++){
                //cout << "ci[" << i << "] = " << ci[i] << endl;
                if(ci[i] == "W")
                    wSpells += count;
                else if(ci[i] == "U")
                    uSpells += count;
                else if(ci[i] == "B")
                    bSpells += count;
                else if(ci[i] == "R")
                    rSpells += count;
                else if(ci[i] == "G")
                    gSpells += count;
            }
            sCount += count;
        }
        //cout << count << " " << cards[name]["convertedManaCost"] << endl;
    }
    ofstream out(deckFileName.substr(0,deckFileName.length() - 4) + ".jgr");

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
    if(wSources > 0 && wSpells > 0){
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
    if(uSources > 0 && uSpells > 0){
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
    if(bSources > 0 && bSpells > 0){
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
    if(rSources > 0 && rSpells > 0){
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
    if(gSources > 0 && gSpells > 0){
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
    out << "newgraph\n x_translate " << xtrans << " y_translate " << ytrans << endl;
    out << "xaxis no_draw_axis min 0 max 100 hash 0 yaxis no_draw_axis min 0 max 100 hash 0\n";
    double x = 0, y = 100;
    double allSpells = wSpells + uSpells + bSpells + rSpells + gSpells;
    cout << "allspells: " << allSpells << endl;
    if(wSpells > 0){
        double rgb[3] = {1,1,0};
        x = graphbar(out, rgb, x, 100, wSpells, allSpells);
    }
    if(uSpells > 0){
        double rgb[3] = {0,0,1};
        x = graphbar(out, rgb, x, 100, uSpells, allSpells);
    }
    if(bSpells > 0){
        double rgb[3] = {.5,0,.5};
        x = graphbar(out, rgb, x, 100, bSpells, allSpells);
    }
    if(rSpells > 0){
        double rgb[3] = {1,0,0};
        x = graphbar(out, rgb, x, 100, rSpells, allSpells);
    }
    if(gSpells > 0){
        double rgb[3] = {0,1,0};
        x = graphbar(out, rgb, x, 100, gSpells, allSpells);
    }
    out << "newstring hjc vjc font Times-Italic lgray 1 fontsize 14 x 50 y 90 : Spells\n";

    double allSources = wSources + uSources + bSources + rSources + gSources;
    x = 0;
    if(wSources > 0){
        double rgb[3] = {1,1,0};
        x = graphbar(out, rgb, x, 60, wSources, allSources);
    }
    if(uSources > 0){
        double rgb[3] = {0,0,1};
        x = graphbar(out, rgb, x, 60, uSources, allSources);
    }
    if(bSources > 0){
        double rgb[3] = {.5,0,.5};
        x = graphbar(out, rgb, x, 60, bSources, allSources);
    }
    if(rSources > 0){
        double rgb[3] = {1,0,0};
        x = graphbar(out, rgb, x, 60, rSources, allSources);
    }
    if(gSources > 0){
        double rgb[3] = {0,1,0};
        x = graphbar(out, rgb, x, 60, gSources, allSources);
    }
    out << "newstring hjc vjc font Times-Italic lgray 1 fontsize 14 x 50 y 50 : Lands\n";
    //out << "newline poly pts 0 100 0 80 100 80 100 100\n";
    cout << "Land count: " << lCount << endl << "Spell count: " << sCount << endl;
    cout << "Deck size: " << deckSize << endl;
    cout << "White Spells: " << wSpells << endl;
    cout << "Blue Spells: " << uSpells << endl;
    cout << "Black Spells: " << bSpells << endl;
    cout << "Red Spells: " << rSpells << endl;
    cout << "Green Spells: " << gSpells << endl;
}