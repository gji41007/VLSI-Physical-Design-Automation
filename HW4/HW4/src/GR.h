#ifndef _GR_H
#define _GR_H



#include "graph.h"


class GlobalRouter{
private:
    std::string inputPath, outputPath;
    Graph graph;
public:
    void run();
    GlobalRouter(std::string i, std::string o){inputPath = i; outputPath = o;}
    void PatternRoute_Initial();
    void PatternRoute();
    void MonoRoute();
    bool HUMRoute(int k);
    bool AStarRoute();
    bool MAZERoute();
};


#endif