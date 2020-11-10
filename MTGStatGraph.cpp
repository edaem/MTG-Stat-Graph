/*
    MTGStatGraph.cpp
    Chauncey Meade
    November 2nd, 2020

    This file implements the MTGStatGraph program. The program accepts the name of
    a Magic: the Gathering decklist from the command line and produces a jgraph
    file that will depict various stats about that deck.
*/
#include "nlohmann/json.hpp"
#include "hypergeometric.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <utility>

using namespace std;
using json = nlohmann::json;

enum color {white, blue, black, red, green};

/* Creates jgraph code for a line graph that depicts the percentage chance to have
one, two, or three sources of the given color. Uses hypergeometric distribution for
the probabilty calculation (see hypergeometric.hpp) */
void graphcolor(ostream& out, string color, double rgb[], int deckSize, int sources){
    /* Axis set up */
    out << "xaxis min 1 max 10 hash 1 mhash 0\nlabel : Turn #\n";
    out << "yaxis min 0 max 100\nlabel : % Chance of Having " << color << " Mana\n";

    /* Curve for having a single source of a color */
    out << "newcurve color " << rgb[0] << " " << rgb[1] << " " << rgb[2];
    out << " linetype solid label : One source\n pts ";
    for(int i = 0; i < 10; i++){
        out << i+1 << " " << hdgte(deckSize, sources, 7+i, 1) * 100 << " ";
    }

    /* Curve for having two sources of a color */
    out << endl << "newcurve color " << rgb[0] << " " << rgb[1] << " " << rgb[2];
    out << " linetype dashed label : Two sources \n pts ";
    for(int i = 1; i < 10; i++){
        out << i+1 << " " << hdgte(deckSize, sources, 7+i, 2) * 100 << " ";
    }

    /* Curve for having three sources of a color */
    out << endl << "newcurve color " << rgb[0] << " " << rgb[1] << " " << rgb[2];
    out << " linetype dotdotdash label : Three sources\npts ";
    for(int i = 2; i < 10; i++){
        out << i+1 << " " << hdgte(deckSize, sources, 7+i, 3) * 100 << " ";
    }

    /* Places the legend in the bottom right of the graph */
    out << endl << "legend x 7 y 20\n";
}

/* creates a bar that has width equal to the percentage of partial/total */
int graphbar(ostream& out, double rgb[], double x, double y, double partial, double total){
    double size = (partial/total) * 100;
    out << "newline poly pcfill " << rgb[0] << " " << rgb[1] << " " << rgb[2];
    out << " pts " << x << " " << y << " " << x << " " << y-20 << " "; 
    out << x + size << " " << y-20 << " " << x + size << " " << y << endl;

    /* returns where the next bar should be printed */
    return size + x;
}

int main(int argc, char* argv[]){
    ifstream in;    /* used for reading in the json card list and the decklist file */
    ofstream out;   /* used for writing the jgraph file */
    json cards;     /* json container for the card file */
    string deckFileName;    /* file name for the decklist */
    string line;            /* for processing lines from the decklist */
    int count;              /* holds the number of copies of a card when processing a line */
    int deckSize;           /* number of cards in the deck */
    int lCount;             /* number of lands in the deck */
    int sCount;             /* number of spells in the deck */
    double sources[5] = {0,0,0,0,0};    /* tracks the number of sources for each color */
    double spells[5] = {0,0,0,0,0};     /* tracks the number of spells of each color */
    double rgb[5][3];       /* used for storing and passing rgb values for each color */
    double xPos;            /* tracks the starting xPos for the bar graph */
    double xTrans;          /* tracks the x transposition value for drawing multiple graphs */
    double yTrans;          /* tracks the Y transposition value for drawing multiple graphs */
    string colorStr[5];     /* holds strings for each color name, used in graph loop */

    /* Color settings for white graphs/polygons */
    rgb[0][0] = 1; rgb[0][1] = 1; rgb[0][2] = 0;
    colorStr[0] = "White";

    /* Color settings for blue graphs/polygons */
    rgb[1][0] = 0; rgb[1][1] = 0; rgb[1][2] = 1;
    colorStr[1] = "Blue";

    /* Color settings for black graphs/polygons */
    rgb[2][0] = .5; rgb[2][1] = 0; rgb[2][2] = .5;
    colorStr[2] = "Black";

    /* Color settings for red graphs/polygons */
    rgb[3][0] = 1; rgb[3][1] = 0; rgb[3][2] = 0;
    colorStr[3] = "Red";

    /* Color settings for green graphs/polygons */
    rgb[4][0] = 0; rgb[4][1] = 1; rgb[4][2] = 0;
    colorStr[4] = "Green";

    /* load cards.json */
    in.open("cards.json");
    in >> cards;
    in.close();

    /* exits if there isn't 1 argument */
    if(argc != 2){
        cout << "usage: MTGStatGraph decklistFileName\n";
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
        /* updates appropriate info if card is a land */
        if(type.find("Land") != string::npos){
            /* adds to the appropriate source counters */
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
        /* updates appropriate info if card is a spell */
        else{
            /* adds to the appropriate spells counters */
            vector<string> ci = cards[name]["colorIdentity"];
            for(int i = 0; i < ci.size(); i++){
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
    }

    /* Opens the jgraph file. Name will be whatever the decklist file name is but
    with the file extension changed to .jgr */
    out.open(deckFileName.substr(0,deckFileName.length() - 4) + ".jgr");

    /* Land Drop Graph */
    out << "newgraph\n x_translate -4.2 y_translate 4\n";
    out << "xaxis min 1 max 10 hash 1 mhash 0\nlabel : Turn #\n" ;
    out << "yaxis min 0 max 100\nlabel : % Chance of Hitting Land Drop\n";
    out << "newcurve linetype solid pts ";
    for(int i = 0; i < 10; i++){
        out << i+1 <<" " << hdgte(deckSize, lCount, 7+i, i+1) * 100 << " ";
    }
    out << endl;
    xTrans = -0.6;
    yTrans = 4;

    /* Creates graphs for each color in the deck that display the percentange chance of
    having one, two, or three sources of that color on turns 1 through 10 */
    for(int i = 0; i < 5; i++){
        if(sources[i] > 0 && spells[i] > 0){
            out << "newgraph\n x_translate " << xTrans << " y_translate " << yTrans << endl;
            graphcolor(out, colorStr[i], rgb[i], deckSize, sources[i]);
            if(xTrans < 3)
                xTrans += 3.8;
            else{
                yTrans -= 4;
                xTrans = -4.2;
            }
        }
    }
    
    /* Creates bar representation of the percentage of spells of each color */
    out << "newgraph\n x_translate " << xTrans << " y_translate " << yTrans << endl;
    out << "xaxis no_draw_axis min 0 max 100 hash 0 yaxis no_draw_axis min 0 max 100 hash 0\n";
    xPos = 0;
    double totalSpells = spells[white] + spells[blue] + spells[black] + spells[red] + spells[green];

    for(int i = 0; i < 5; i++){
        if(spells[i] > 0){
            xPos = graphbar(out, rgb[i], xPos, 100, spells[i], totalSpells);
        }
    }
    out << "newstring hjc vjc font Helvetica  fontsize 14 x 50 y 90 : Spells\n";

    /* Creates bar representation of the percentage of lands of each color */
    double totalSources = sources[white] + sources[blue] + sources[black] + sources[red] + sources[green];
    xPos = 0;
    for(int i = 0; i < 5; i++){
        if(sources[i] > 0){
            xPos = graphbar(out, rgb[i], xPos, 60, sources[i], totalSources);
        }
    }
    out << "newstring hjc vjc font Helvetica fontsize 14 x 50 y 50 : Lands\n";
    out.close();

    return 0;
}