#ifndef _DEF_H
#define _DEF_H


#define WINDOWSIZE 4



#include "Lef.h"
#include "util.h"

#include <vector> 
#include <string>

class Net;
class Pin;
class Row;
class Component;

class Row{
public:
    std::string name;
    Site* site;
    std::string siteStr;
    double origiX, origiY;
    std::string FS;
    int numHori, numVert;
    int stepX, stepY;
    std::vector<Component*> siteComp;

    int permutation(std::vector<Component*> candidates, int left, int right);
    int rearrangeDP(std::vector<Component*> candidates, int left, int right);
};

class Component{
public:
    std::string name;
    std::string cellStr;
    StdCell* cell;
    Row* row;
    std::vector<Net*> nets;
    int left, right, size;
    bool isFixed;
    int x, y;
    std::string orientation;


    void resetRange();

    friend std::ostream& operator<<(std::ostream& os, const Component& comp) {
        os << " - " << comp.name
        << " " << comp.cellStr << "\n"
        << "    + " << ((comp.isFixed)? "FIXED":"PLACED")
        << " ( " << comp.x<<" "<<comp.y
        <<" ) " << comp.orientation << " ;";
        return os;    
    }
};

class Pin{
public:
    std::string name;
    Net* net;
    std::string netStr;
    int x1, y1, x2, y2;
    int x, y;
    std::string orientation;
};

class Twopin{
public: 
    std::pair<int, int> p1, p2;
    Twopin(){}
    Twopin(std::pair<int, int> _p1, std::pair<int, int> _p2){
        p1 = _p1;
        p2 = _p2;
    }
    int getHPWL(){return abs(p1.first - p2.first) + abs(p1.second - p2.second);}

};
class Net{
public:
    std::string name;
    std::vector<Pin*> pins;
    std::vector<Component*> comps;
    std::vector<std::string> pinStr;
    std::vector<std::pair<std::string, std::string>> compStr;

    int HPWL();
};

class DEF{
public:
    int unit;
    int x1, x2, y1, y2;

    std::vector<Row*> rows;
    std::vector<Component*> comps;
    std::vector<Pin*> pins;
    std::vector<Net*> nets;

    std::unordered_map<std::string, int> compId, pinId, netId;

    

    void parseDEF(std::string defPath);
    void init(LEF lef);
    void dumpDEF(std::string defPath, std::string outputPath);

    long long int totalHPWL();
};

#endif