#ifndef _GRAPH_H
#define _GRAPH_H

#include <iostream>
#include <vector>
#include <string>
#include <assert.h>

class Pin{
public:
    std::string name;
    int x, y; 

};
class Net{
public:
    std::string name;
    Pin* p1, * p2;

    std::vector<std::pair<int, int>> path;
    int getHPWL(){return abs(p1->x-p2->x) + abs(p1->y-p2->y);}

    friend std::ostream& operator<<(std::ostream& os, const Net& net) {
        os << "Net " << net.name << "\n";
        for(int i = 0; i < (int)net.path.size()-1; ++i){
            os << "Segment " << net.path[i].first<<" "<<net.path[i].second<<" "
                             <<net.path[i+1].first<<" "<<net.path[i+1].second << "\n";
        }
        return os;    
    }
};


class Graph{
public:
    int gridX, gridY;
    int capHori, capVert;
    int numNet;
    
    std::vector<Net*> nets;
    std::vector<std::vector<int>> demandHori, demandVert;
    std::vector<std::vector<int>> histHori, histVert;

    void parse(std::string inputPath);
    void dump(std::string outputPath);
    void init();

    double getCost(std::pair<int, int> p1, std::pair<int, int > p2);
    double getPathCost(std::vector<std::pair<int, int>>& path);
    long long int getTotalWL();

    void setPath(Net* n, std::vector<std::pair<int, int>> path);
    void ripUp(Net* n);
    bool isOverflow(Net* n);
    int getPathOverflow(Net* n);
    void addHistCost();

    void PatternRoute(Net* n);
    void MonoRoute(Net* n);
    void HUMRoute(Net* n, int k);
    void AStarRoute(Net* n);
    void MazeRoute(Net* n);
};

#endif