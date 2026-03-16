#include "Graph.h"


void DieGraph::init(DEF def){
    isValid = std::vector<std::vector<bool>>(def.x2-def.x1, std::vector<bool>(def.y2-def.y1, true));
    for(auto comp: def.comps){
        for(int x = comp->x; x < comp->x + def.unit*comp->cell->cellWidth; ++x){
            for(int y = comp->y; y < comp->y + def.unit*comp->cell->cellHeight; ++y){
                isValid[x][y] = false;
            }
        }
    }
}