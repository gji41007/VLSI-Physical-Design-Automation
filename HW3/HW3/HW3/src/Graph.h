#ifndef _GRAPH_H
#define _GRAPH_H

#include "Def.h"

class DieGraph{
public:
    int x, y;
    std::vector<std::vector<bool>> isValid;



    DieGraph(){}
    DieGraph(int _x, int _y){
        x = _x;
        y = _y;
    }
    void init(DEF def);
};


#endif