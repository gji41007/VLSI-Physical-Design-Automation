#ifndef _DP_H
#define _DP_H


#include "Lef.h"
#include "Def.h"
#include "Graph.h"

#include <cstdlib>

class DetailPlacer{
private:
    //filename
    std::string lefPath, defPath, outputPath;


    LEF lef;
    DEF def;
    DieGraph graph;
public:
    DetailPlacer(std::string lef, std::string def, std::string output){
        lefPath = lef;
        defPath = def;
        outputPath = output;
    }
    void run();
    void rowReorder(int windowSize);
    void singleCellAdjust();
    void GlobalInsert();
};


#endif