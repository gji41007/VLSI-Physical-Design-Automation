#ifndef _LEF_H
#define _LEF_H

#include <iostream>
#include <fstream>
#include <vector> 
#include <string>
#include <unordered_map>


class Site{
public:
    std::string name;
    std::string siteClass;
    bool isSymmetry = false;
    double siteWidth, siteHeight;


    Site(){}
    Site(std::string n){name = n;}
    friend std::ostream& operator<<(std::ostream& os, const Site& site) {
        os << "Site(name=" << site.name
        << ", class=" << site.siteClass
        << ", symmetry=" << (site.isSymmetry ? "true" : "false")
        << ", width=" << site.siteWidth
        << ", height=" << site.siteHeight << ")";
        return os;
    }
};
class StdCell{
public:
    std::string name;
    std::string cellClass;
    float origiX, origiY;
    double cellWidth, cellHeight;
    Site* site;



    friend std::ostream& operator<<(std::ostream& os, const StdCell& cell) {
        os << "StdCell(name=" << cell.name
        << ", class=" << cell.cellClass
        << ", origin=(" << cell.origiX << "," << cell.origiY << ")"
        << ", width=" << cell.cellWidth
        << ", height=" << cell.cellHeight
        << ", site=" << (cell.site ? cell.site->name : "null") << ")";
        return os;
    }

};
class LEF{
public:
    int unit;
    std::vector<Site*> sites;
    std::vector<StdCell*> stdCells;
    std::unordered_map<std::string, int> siteId;
    std::unordered_map<std::string, int> cellId;
    void parseLEF(std::string lefFile);
    void dumpSite(){for(auto s:sites)std::cout<<*s<<std::endl;}
    void dumpCell(){for(auto s:stdCells)std::cout<<*s<<std::endl;}
};



#endif