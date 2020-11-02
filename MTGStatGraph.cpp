#include "json.hpp"
#include "hypergeometric.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <utility>

using namespace std;
using json = nlohmann::json;

enum color {white, blue, black, red, green};

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
    int sources[5] = {0,0,0,0,0};
    int spells[5] = {0,0,0,0,0};

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
                if(ci[i] == "W")
                    sources[white] += count;
                else if(ci[i] == "U")
                    sources[blue] += count;
                else if(ci[i] == "B")
                    sources[black] += count;
                else if(ci[i] == "R")
                    sources[red] += count;
                else if(ci[i] == "G")
                    sources[green] += count;
            }
            lCount += count;
        } 
        /* insert spells into the spells map */
        else{
            vector<string> ci = cards[name]["colorIdentity"];
            for(int i = 0; i < ci.size(); i++){
                //cout << "ci[" << i << "] = " << ci[i] << endl;
                if(ci[i] == "W")
                    spells[white] += count;
                else if(ci[i] == "U")
                    spells[blue] += count;
                else if(ci[i] == "B")
                    spells[black] += count;
                else if(ci[i] == "R")
                    spells[red] += count;
                else if(ci[i] == "G")
                    spells[green] += count;
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
    if(sources[white] > 0 && spells[white] > 0){
        out << "newgraph\n x_translate " << xtrans << " y_translate " << ytrans << endl;
        double rgb[3] = {1,1,0};
        graphcolor(out, "White", rgb, deckSize, sources[white]);
        if(xtrans < 3)
            xtrans += 3.8;
        else{
            ytrans -= 4;
            xtrans = -4.2;
        }
    }
    if(sources[blue] > 0 && spells[blue] > 0){
        out << "newgraph\n x_translate " << xtrans << " y_translate " << ytrans << endl;
        double rgb[3] = {0,0,1};
        graphcolor(out, "Blue", rgb, deckSize, sources[blue]);
        if(xtrans < 3)
            xtrans += 3.8;
        else{
            ytrans -= 4;
            xtrans = -4.2;
        }
    }
    if(sources[black] > 0 && spells[black] > 0){
        out << "newgraph\n x_translate " << xtrans << " y_translate " << ytrans << endl;
        double rgb[3] = {.5,0,.5};
        graphcolor(out, "Black", rgb, deckSize, sources[black]);
        if(xtrans < 3)
            xtrans += 3.8;
        else{
            ytrans -= 4;
            xtrans = -4.2;
        }
    }
    if(sources[red] > 0 && spells[red] > 0){
        out << "newgraph\n x_translate " << xtrans << " y_translate " << ytrans << endl;
        double rgb[3] = {1,0,0};
        graphcolor(out, "Red", rgb, deckSize, sources[red]);
        if(xtrans < 3)
            xtrans += 3.8;
        else{
            ytrans -= 4;
            xtrans = -4.2;
        }
    }
    if(sources[green] > 0 && spells[green] > 0){
        out << "newgraph\n x_translate " << xtrans << " y_translate " << ytrans << endl;
        double rgb[3] = {0,1,0};
        graphcolor(out, "Green", rgb, deckSize, sources[green]);
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
    double allSpells = spells[white] + spells[blue] + spells[black] + spells[red] + spells[green];
    if(spells[white] > 0){
        double rgb[3] = {1,1,0};
        x = graphbar(out, rgb, x, 100, spells[white], allSpells);
    }
    if(spells[blue] > 0){
        double rgb[3] = {0,0,1};
        x = graphbar(out, rgb, x, 100, spells[blue], allSpells);
    }
    if(spells[black] > 0){
        double rgb[3] = {.5,0,.5};
        x = graphbar(out, rgb, x, 100, spells[black], allSpells);
    }
    if(spells[red] > 0){
        double rgb[3] = {1,0,0};
        x = graphbar(out, rgb, x, 100, spells[red], allSpells);
    }
    if(spells[green] > 0){
        double rgb[3] = {0,1,0};
        x = graphbar(out, rgb, x, 100, spells[green], allSpells);
    }
    out << "newstring hjc vjc font Times-Italic lgray 1 fontsize 14 x 50 y 90 : Spells\n";

    double allSources = sources[white] + sources[blue] + sources[black] + sources[red] + sources[green];
    x = 0;
    if(sources[white] > 0){
        double rgb[3] = {1,1,0};
        x = graphbar(out, rgb, x, 60, sources[white], allSources);
    }
    if(sources[blue] > 0){
        double rgb[3] = {0,0,1};
        x = graphbar(out, rgb, x, 60, sources[blue], allSources);
    }
    if(sources[black] > 0){
        double rgb[3] = {.5,0,.5};
        x = graphbar(out, rgb, x, 60, sources[black], allSources);
    }
    if(sources[red] > 0){
        double rgb[3] = {1,0,0};
        x = graphbar(out, rgb, x, 60, sources[red], allSources);
    }
    if(sources[green] > 0){
        double rgb[3] = {0,1,0};
        x = graphbar(out, rgb, x, 60, sources[green], allSources);
    }
    out << "newstring hjc vjc font Times-Italic lgray 1 fontsize 14 x 50 y 50 : Lands\n";
    // cout << "Land count: " << lCount << endl << "Spell count: " << sCount << endl;
    // cout << "Deck size: " << deckSize << endl;
    // cout << "White Spells: " << spells[white] << endl;
    // cout << "Blue Spells: " << spells[blue] << endl;
    // cout << "Black Spells: " << spells[black] << endl;
    // cout << "Red Spells: " << spells[red] << endl;
    // cout << "Green Spells: " << spells[green] << endl;
}