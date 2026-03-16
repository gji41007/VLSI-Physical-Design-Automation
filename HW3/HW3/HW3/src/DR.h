#ifndef _DR_H
#define _DR_H

#include "Lef.h"
#include "Def.h"

#include <cstdlib>

class DetailRouter{
private:
    //filename
    std::string lefPath, defPath, outputPath;


    LEF lef;
    DEF def;
public:
    DetailRouter(std::string lef, std::string def, std::string output){
        lefPath = lef;
        defPath = def;
        outputPath = output;
    }
    void run();
};


#endif