#include "graph.h"


#include <fstream>
#include <algorithm>
#include <cmath>
#include <cfloat>
#include <queue>

void Graph::parse(std::string inputPath){
    std::ifstream inFile(inputPath);
    if (!inFile.is_open()) {
        std::cerr << "Error: Cannot open file " << inputPath << std::endl;
        return;
    }

    //read info
    std::string s;
    inFile>>s>>gridX>>gridY;
    inFile>>s>>capHori>>capVert;
    inFile>>s>>numNet;
    
    
    //read net
    for(int i = 0; i < numNet; ++i){
        Net* n = new Net;
        Pin* p1 = new Pin;
        Pin* p2 = new Pin;
        inFile>>s>>n->name>>s;
        inFile>>s>>p1->name>>p1->x>>p1->y;
        inFile>>s>>p2->name>>p2->x>>p2->y;
        n->p1 = p1;
        n->p2 = p2;
        nets.push_back(n);

    }
     
}
void Graph::dump(std::string outputPath){
    std::ofstream outFile(outputPath);
    outFile<<"Wirelength "<<getTotalWL()<<std::endl;
    for(auto net: nets){
        outFile<<(*net);
    }
}
void Graph::init(){
    demandHori = std::vector<std::vector<int>>(gridX-1, std::vector<int>(gridY, 0));
    demandVert = std::vector<std::vector<int>>(gridX, std::vector<int>(gridY-1, 0));
    histHori = std::vector<std::vector<int>>(gridX-1, std::vector<int>(gridY, 0));
    histVert = std::vector<std::vector<int>>(gridX, std::vector<int>(gridY-1, 0));
    //sorting by increasing HPWL
    std::sort(nets.begin(), nets.end(), 
    [](Net* a, Net* b){
        return a->getHPWL() < b->getHPWL();
    }
    );
    // std::cout<<"finish init"<<std::endl;
}

double Graph::getCost(std::pair<int, int> p1, std::pair<int, int > p2){
    /* param */
    double h1 = 5;
    double h2 = 55;
    double k = 1;
    double histWeight = 1;
    double histCost = 0;

    /*
    0 0 0 
    double h1 = 5;
    double h2 = 55;
    double k = 1;
    double histWeight = 1;
    double histCost = 0;
    */
    
    
    int x1 = p1.first;
    int x2 = p2.first;
    int y1 = p1.second;
    int y2 = p2.second;
    
    //go right
    int d;
    int cap;
    if(x1 + 1 == x2 && y1 == y2){
        d = demandHori[x1][y1];
        cap = capHori;
        histCost = histHori[x1][y1];
    }
    //go left
    else if(x1 == x2 + 1 && y1 == y2){
        d = demandHori[x2][y2];
        cap = capHori;
        histCost = histHori[x2][y2];
    }
    //go up
    else if(x1 == x2 && y1 + 1 == y2){
        d = demandVert[x1][y1];
        cap = capVert;
        histCost = histVert[x1][y1];
    }
    //go down
    else if(x1 == x2 && y1 == y2 + 1){
        d = demandVert[x2][y2];
        cap = capVert;
        histCost = histVert[x2][y2];
    }
    else{
        std::cerr<<"error in getCost(): not adjecnt grid\n";
        exit(-1);
    }
    ++d;

    
    histCost = (histCost) * (((d)/cap) * ((d)/cap) * ((d)/cap));
    double exponent = -k * (d - cap); 
    double denominator = 1 + std::exp(exponent); 
    return h1 + (h2 / denominator) + histWeight*histCost;
}
double Graph::getPathCost(std::vector<std::pair<int, int>>& path){
    double totalCost = 0;
    for(int i = 0; i < (int)path.size()-1; ++i){
        totalCost += getCost(path[i], path[i+1]);
    }
    return totalCost;
}
long long int Graph::getTotalWL(){
    long long int wl = 0;
    for(auto net: nets){
        wl += (net->path.size()-1);
    }
    return wl;
}

void Graph::setPath(Net* n, std::vector<std::pair<int, int>> path){
    assert(n->path.empty());
    assert(n != nullptr);
    n->path = path;
    for(int i = 0; i < (int)path.size()-1; ++i){
        int x1 = path[i].first;
        int x2 = path[i+1].first;
        int y1 = path[i].second;
        int y2 = path[i+1].second;
        //go right
        if(x1 + 1 == x2 && y1 == y2){
            ++demandHori[x1][y1];
            // ++histHori[x1][y1];
        }
        //go left
        else if(x1 == x2 + 1 && y1 == y2){
            ++demandHori[x2][y2];
            // ++histHori[x2][y2];
        }
        //go up
        else if(x1 == x2 && y1 + 1 == y2){
            ++demandVert[x1][y1];
            // ++histVert[x1][y1];
        }
        //go down
        else if(x1 == x2 && y1 == y2 + 1){
            ++demandVert[x2][y2];
            // ++histVert[x2][y2];
        }
        else{
            std::cerr<<"error in getCost(): not adjecnt grid\n";
            exit(-1);
        }
    }
}

void Graph::ripUp(Net* n){
    assert(n != nullptr);
    assert(!n->path.empty());
    for(int i = 0; i < (int)n->path.size()-1; ++i){
        int x1 = n->path[i].first;
        int x2 = n->path[i+1].first;
        int y1 = n->path[i].second;
        int y2 = n->path[i+1].second;
        //go right
        if(x1 + 1 == x2 && y1 == y2){
            --demandHori[x1][y1];
        }
        //go left
        else if(x1 == x2 + 1 && y1 == y2){
            --demandHori[x2][y2];
        }
        //go up
        else if(x1 == x2 && y1 + 1 == y2){
            --demandVert[x1][y1];
        }
        //go down
        else if(x1 == x2 && y1 == y2 + 1){
            --demandVert[x2][y2];
        }
        else{
            std::cerr<<"error in getCost(): not adjecnt grid\n";
            exit(-1);
        }
    }
    n->path.clear();
}
void Graph::addHistCost(){
    for(int x = 0; x < gridX-1; ++x){
        for(int y = 0; y < gridY; ++y){
            if(demandHori[x][y] > capHori){
                // histHori[x][y] *= 0.95;
                histHori[x][y] += (demandHori[x][y]-capHori);
            }
        }
    }
    for(int x = 0; x < gridX; ++x){
        for(int y = 0; y < gridY-1; ++y){
            if(demandVert[x][y] > capVert){
                // histVert[x][y] *= 0.95;
                histVert[x][y] += (demandVert[x][y]-capVert);
            }
        }
    }
}
bool Graph::isOverflow(Net* n){
    assert(n != nullptr);
    assert(!n->path.empty());
    for(int i = 0; i < (int)n->path.size()-1; ++i){
        int x1 = n->path[i].first;
        int x2 = n->path[i+1].first;
        int y1 = n->path[i].second;
        int y2 = n->path[i+1].second;
        //go right
        if(x1 + 1 == x2 && y1 == y2){
            if(demandHori[x1][y1] > capHori)return true;
        }
        //go left
        else if(x1 == x2 + 1 && y1 == y2){
            if(demandHori[x2][y2] > capHori)return true;
        }
        //go up
        else if(x1 == x2 && y1 + 1 == y2){
            if(demandVert[x1][y1] > capVert)return true;
        }
        //go down
        else if(x1 == x2 && y1 == y2 + 1){
            if(demandVert[x2][y2] > capVert)return true;
        }
        else{
            std::cerr<<"error in getCost(): not adjecnt grid\n";
            exit(-1);
        }
    }
    return false;
}
int Graph::getPathOverflow(Net* n){
    assert(n != nullptr);
    assert(!n->path.empty());
    int totalOF = 0;
    for(int i = 0; i < (int)n->path.size()-1; ++i){
        int x1 = n->path[i].first;
        int x2 = n->path[i+1].first;
        int y1 = n->path[i].second;
        int y2 = n->path[i+1].second;
        //go right
        if(x1 + 1 == x2 && y1 == y2){
            if(demandHori[x1][y1] > capHori)++totalOF;
        }
        //go left
        else if(x1 == x2 + 1 && y1 == y2){
            if(demandHori[x2][y2] > capHori)++totalOF;
        }
        //go up
        else if(x1 == x2 && y1 + 1 == y2){
            if(demandVert[x1][y1] > capVert)++totalOF;
        }
        //go down
        else if(x1 == x2 && y1 == y2 + 1){
            if(demandVert[x2][y2] > capVert)++totalOF;
        }
        else{
            std::cerr<<"error in getCost(): not adjecnt grid\n";
            exit(-1);
        }
    }
    return totalOF;
}

void Graph::PatternRoute(Net* n){
    int x1 = n->p1->x;
    int x2 = n->p2->x;
    int y1 = n->p1->y;
    int y2 = n->p2->y;
    std::vector<std::pair<int, int>> path1, path2;
    // std::cout<<x1<<" "<<x2<<" "<<y1<<" "<<y2<<std::endl;
    //straight line
    if(x1 == x2){
        //go up
        if(y1 < y2){
            for(int y = y1; y <= y2; ++y){
                path1.push_back(std::make_pair(x1, y));
            }
        }
        else{
            for(int y = y1; y >= y2; --y){
                path1.push_back(std::make_pair(x1, y));
            }
        }
        setPath(n, path1);
        return;
    }
    else if(y1 == y2){
        //go up
        if(x1 < x2){
            for(int x = x1; x <= x2; ++x){
                path1.push_back(std::make_pair(x, y1));
            }
        }
        else{
            for(int x = x1; x >= x2; --x){
                path1.push_back(std::make_pair(x, y1));
            }
        }
        setPath(n, path1);
        return;
    }
    //L shape
    if(x1 < x2){
        for(int x = x1; x <= x2; ++x){
            path1.push_back(std::make_pair(x, y1));
            path2.push_back(std::make_pair(x, y2));
        }
        std::reverse(path2.begin(), path2.end());
    }
    else{
        for(int x = x1; x >= x2; --x){
            path1.push_back(std::make_pair(x, y1));
            path2.push_back(std::make_pair(x, y2));
        }
        std::reverse(path2.begin(), path2.end());
    }

    if(y1 < y2){
        for(int y = y1+1; y <= y2; ++y){
            path1.push_back(std::make_pair(x2, y));
        }
        for(int y = y2-1; y >= y1; --y){
            path2.push_back(std::make_pair(x1, y));
        }
    }
    else{
        for(int y = y1-1; y >= y2; --y){
            path1.push_back(std::make_pair(x2, y));
        }
        for(int y = y2+1; y <= y1; ++y){
            path2.push_back(std::make_pair(x1, y));
        }
    }
    
    double cost1 = getPathCost(path1);
    double cost2 = getPathCost(path2);
    if(cost1 < cost2){
        setPath(n, path1);
    }
    else{
        setPath(n, path2);
    }
}
void Graph::MonoRoute(Net* n){
    assert(n != nullptr);
    assert(n->path.empty());
    int s_x = n->p1->x, s_y = n->p1->y;
    int t_x = n->p2->x, t_y = n->p2->y;
    if(s_x == t_x || s_y == t_y){
        PatternRoute(n);
        return;
    }
    std::vector<std::vector<std::pair<int, int>>> pMap = std::vector<std::vector<std::pair<int, int>>>(gridX, std::vector<std::pair<int, int>>(gridY, std::make_pair(-1, -1)));
    std::vector<std::vector<double>> costMap = std::vector<std::vector<double>>(gridX, std::vector<double>(gridY, 0));
    pMap[s_x][s_y] = std::make_pair(s_x, s_y);
    costMap[s_x][s_y] = 0;
    
    // setting point cost on the same row
    if(s_x < t_x){
        for(int i = s_x + 1; i <= t_x; ++i){
            costMap[i][s_y] = (costMap[i-1][s_y] + getCost(std::make_pair(i-1, s_y), std::make_pair(i, s_y)));
            pMap[i][s_y] = std::make_pair(i-1, s_y);
        }
    }
    else{
        for(int i = s_x - 1; i >= t_x; --i){
            costMap[i][s_y] = (costMap[i+1][s_y] + getCost(std::make_pair(i+1, s_y), std::make_pair(i, s_y)));
            pMap[i][s_y] = std::make_pair(i+1, s_y);
        }
    }
    // setting point cost on the same col
    if(s_y < t_y){
        for(int i = s_y + 1; i <= t_y; ++i){
            costMap[s_x][i] = (costMap[s_x][i-1] + getCost(std::make_pair(s_x, i-1), std::make_pair(s_x, i)));
            pMap[s_x][i] = std::make_pair(s_x, i-1);
        }
    }
    else{
        for(int i = s_y - 1; i >= t_y; --i){
            costMap[s_x][i] = (costMap[s_x][i+1] + getCost(std::make_pair(s_x, i+1), std::make_pair(s_x, i)));
            pMap[s_x][i] = std::make_pair(s_x, i+1);
        }
    }

    /*
    +-t
    s-+
    */
   if(s_x < t_x && s_y < t_y){
        for(int x = s_x + 1; x <= t_x; ++x){
            for(int y = s_y + 1; y <= t_y; ++y){
                double cost1 = costMap[x][y-1] + getCost(std::make_pair(x, y-1), std::make_pair(x, y));
                double cost2 = costMap[x-1][y] + getCost(std::make_pair(x-1, y), std::make_pair(x, y));
                if(cost1 < cost2){
                    costMap[x][y] = costMap[x][y-1] + getCost(std::make_pair(x, y-1), std::make_pair(x, y));
                    pMap[x][y] = std::make_pair(x, y-1);
                }
                else{
                    costMap[x][y] = costMap[x-1][y] + getCost(std::make_pair(x-1, y), std::make_pair(x, y));
                    pMap[x][y] = std::make_pair(x-1, y);
                }
            }
        }
   }

    /*
    s-+
    +-t
    */
    if(s_x < t_x && s_y > t_y){
        for(int x = s_x + 1; x <= t_x; ++x){
            for(int y = s_y - 1; y >= t_y; --y){
                double cost1 = costMap[x][y+1] + getCost(std::make_pair(x, y+1), std::make_pair(x, y));
                double cost2 = costMap[x-1][y] + getCost(std::make_pair(x-1, y), std::make_pair(x, y));
             
                
                if(cost1 < cost2){
                    costMap[x][y] = costMap[x][y+1] + getCost(std::make_pair(x, y+1), std::make_pair(x, y));
                    pMap[x][y] = std::make_pair(x, y+1);
                }
                else{
                    costMap[x][y] = costMap[x-1][y] + getCost(std::make_pair(x-1, y), std::make_pair(x, y));
                    pMap[x][y] = std::make_pair(x-1, y);
                }
            }
        }
    }
    /*
    +-s
    t-+
    */
    if(s_x > t_x && s_y > t_y){
            for(int x = s_x - 1; x >= t_x; --x){
                for(int y = s_y - 1; y >= t_y; --y){
                    double cost1 = costMap[x][y+1] + getCost(std::make_pair(x, y+1), std::make_pair(x, y));
                    double cost2 = costMap[x+1][y] + getCost(std::make_pair(x+1, y), std::make_pair(x, y));
                    if(cost1 < cost2){
                        costMap[x][y] = costMap[x][y+1] + getCost(std::make_pair(x, y+1), std::make_pair(x, y));
                        pMap[x][y] = std::make_pair(x, y+1);
                    }
                    else{
                        costMap[x][y] = costMap[x+1][y] + getCost(std::make_pair(x+1, y), std::make_pair(x, y));
                        pMap[x][y] = std::make_pair(x+1, y);
                    }
                }
            }
    }
    /*
    t-+
    +-s
    */
    if(s_x > t_x && s_y < t_y){
        for(int x = s_x - 1; x >= t_x; --x){
            for(int y = s_y + 1; y <= t_y; ++y){
                double cost1 = costMap[x][y-1] + getCost(std::make_pair(x, y-1), std::make_pair(x, y));
                double cost2 = costMap[x+1][y] + getCost(std::make_pair(x+1, y), std::make_pair(x, y));
                if(cost1 < cost2){
                    costMap[x][y] = costMap[x][y-1] + getCost(std::make_pair(x, y-1), std::make_pair(x, y));
                    pMap[x][y] = std::make_pair(x, y-1);
                }
                else{
                    costMap[x][y] = costMap[x+1][y] + getCost(std::make_pair(x+1, y), std::make_pair(x, y));
                    pMap[x][y] = std::make_pair(x+1, y);
                }
            }
        }
    }
    
    std::vector<std::pair<int, int>> path;
    std::pair<int, int> curr = std::make_pair(t_x, t_y);
    while(pMap[curr.first][curr.second] != curr){
        path.push_back(curr);
        curr = pMap[curr.first][curr.second];
    }
    path.push_back(curr);
    setPath(n, path);
}
void Graph::HUMRoute(Net* n, int k){
    assert(n != nullptr);
    assert(n->path.empty());
    int x1 = n->p1->x;
    int x2 = n->p2->x;
    int y1 = n->p1->y;
    int y2 = n->p2->y;
    int leftX = std::max((std::min(x1, x2)-k), 0);
    int rightX = std::min((std::max(x1, x2)+k), gridX-1);
    int botY = std::max((std::min(y1, y2)-k), 0);
    int topY = std::min((std::max(y1, y2)+k), gridY-1);

    std::vector<std::vector<double>> horiCost1(rightX-leftX+1, std::vector<double>(topY-botY+1, DBL_MAX));
    std::vector<std::vector<double>> horiCost2(rightX-leftX+1, std::vector<double>(topY-botY+1, DBL_MAX));
    std::vector<std::vector<double>> vertCost1(rightX-leftX+1, std::vector<double>(topY-botY+1, DBL_MAX));
    std::vector<std::vector<double>> vertCost2(rightX-leftX+1, std::vector<double>(topY-botY+1, DBL_MAX));

    std::vector<std::vector<std::pair<int, int>>> horiparent1(rightX-leftX+1, std::vector<std::pair<int, int>>(topY-botY+1, std::make_pair(-1, -1)));
    std::vector<std::vector<std::pair<int, int>>> horiparent2(rightX-leftX+1, std::vector<std::pair<int, int>>(topY-botY+1, std::make_pair(-1, -1)));
    std::vector<std::vector<std::pair<int, int>>> vertparent1(rightX-leftX+1, std::vector<std::pair<int, int>>(topY-botY+1, std::make_pair(-1, -1)));
    std::vector<std::vector<std::pair<int, int>>> vertparent2(rightX-leftX+1, std::vector<std::pair<int, int>>(topY-botY+1, std::make_pair(-1, -1)));

    horiCost1[x1-leftX][y1-botY] = 0;  
    horiCost2[x2-leftX][y2-botY] = 0; 
    vertCost1[x1-leftX][y1-botY] = 0;  
    vertCost2[x2-leftX][y2-botY] = 0; 

    horiparent1[x1-leftX][y1-botY] = std::make_pair(x1, y1);  
    horiparent2[x2-leftX][y2-botY] = std::make_pair(x2, y2); 
    vertparent1[x1-leftX][y1-botY] = std::make_pair(x1, y1);  
    vertparent2[x2-leftX][y2-botY] = std::make_pair(x2, y2); 

    struct Compare {
        bool operator()(const std::pair<int, double>& a, const std::pair<int, double>& b) {
            return a.second > b.second; 
        }
    };
    struct NodeRecord {
        int x, y;           
        int px, py;            
        double cost;            
    };
    struct NodeCompare {
        bool operator()(const NodeRecord& a, const NodeRecord& b) const {
            return a.cost > b.cost; 
        }
    };
    //hori mono p1
    for(int x = x1-1; x >= leftX; --x){
        horiCost1[x-leftX][y1-botY] = horiCost1[x-leftX+1][y1-botY] + getCost(std::make_pair(x, y1), std::make_pair(x+1, y1));
        horiparent1[x-leftX][y1-botY] = std::make_pair(x+1, y1);
    }
    for(int x = x1+1; x <= rightX; ++x){
        horiCost1[x-leftX][y1-botY] = horiCost1[x-leftX-1][y1-botY] + getCost(std::make_pair(x, y1), std::make_pair(x-1, y1));
        horiparent1[x-leftX][y1-botY] = std::make_pair(x-1, y1);
    }

    for(int y = y1+1; y <= topY; ++y){//above
        std::vector<double> rowCost(rightX - leftX + 1, DBL_MAX);
        std::vector<bool> isVisit(rightX - leftX + 1, false); 
        // std::priority_queue<std::pair<int, double>, std::vector<std::pair<int, double>>, Compare> pq;
        std::priority_queue<NodeRecord, std::vector<NodeRecord>, NodeCompare> pq;
        int offsetY = y - botY;
        //setting initial cost with verical edge  
        for(int x = leftX; x <= rightX; ++x){
            double cost = horiCost1[x-leftX][offsetY-1] + getCost(std::make_pair(x, y), std::make_pair(x, y-1));
            pq.push({x, y, x, y-1, cost});
        }

        int remain = rightX - leftX + 1;
        while(remain > 0){
            assert(!pq.empty());
            NodeRecord record = pq.top();
            pq.pop();
            int offsetX = record.x - leftX;
            if(isVisit[offsetX]){continue;}
            horiparent1[offsetX][offsetY] = std::make_pair(record.px, record.py);
            rowCost[offsetX] = record.cost;
            isVisit[offsetX] = true;
            if(record.x > leftX){
                double cost = rowCost[offsetX] + getCost(std::make_pair(record.x, y), std::make_pair(record.x-1, y));
                pq.push({record.x-1, y, record.x, y, cost});
            }
            if(record.x < rightX){
                double cost = rowCost[offsetX] + getCost(std::make_pair(record.x, y), std::make_pair(record.x+1, y));
                pq.push({record.x + 1, y, record.x, y, cost});
            }
            --remain;
        }
        for(int i = 0; i < (int)rowCost.size(); ++i){
            horiCost1[i][offsetY] = rowCost[i];
        }
    }

    for(int y = y1-1; y >= botY; --y){ // below
        std::vector<double> rowCost(rightX - leftX + 1, DBL_MAX);
        std::vector<bool> isVisit(rightX - leftX + 1, false); 
        std::priority_queue<NodeRecord, std::vector<NodeRecord>, NodeCompare> pq;
        int offsetY = y - botY;
        for(int x = leftX; x <= rightX; ++x){
            double cost = horiCost1[x-leftX][offsetY+1] + getCost(std::make_pair(x, y), std::make_pair(x, y+1));
            pq.push({x, y, x, y+1, cost});
        }
        int remain = rightX - leftX + 1;
        while(remain > 0){
            assert(!pq.empty());
            NodeRecord record = pq.top();
            pq.pop();

            int offsetX = record.x - leftX;
            if(isVisit[offsetX]) continue;

            horiparent1[offsetX][offsetY] = std::make_pair(record.px, record.py);
            rowCost[offsetX] = record.cost;
            isVisit[offsetX] = true;

            if(record.x > leftX){
                double cost = rowCost[offsetX] + getCost({record.x, y}, {record.x-1, y});
                pq.push({record.x-1, y, record.x, y, cost});
            }

            if(record.x < rightX){
                double cost = rowCost[offsetX] + getCost({record.x, y}, {record.x+1, y});
                pq.push({record.x+1, y, record.x, y, cost});
            }
            --remain;
        }

        for(int i = 0; i < (int)rowCost.size(); ++i){
            horiCost1[i][offsetY] = rowCost[i];
        }
    }
    //hori mono p2
    for(int x = x2-1; x >= leftX; --x){
        horiCost2[x-leftX][y2-botY] = horiCost2[x-leftX+1][y2-botY] + getCost(std::make_pair(x, y2), std::make_pair(x+1, y2));
        horiparent2[x-leftX][y2-botY] = std::make_pair(x+1, y2);
    }
    for(int x = x2+1; x <= rightX; ++x){
        horiCost2[x-leftX][y2-botY] = horiCost2[x-leftX-1][y2-botY] + getCost(std::make_pair(x, y2), std::make_pair(x-1, y2));
        horiparent2[x-leftX][y2-botY] = std::make_pair(x-1, y2);
    }

    for(int y = y2+1; y <= topY; ++y){//above
        std::vector<double> rowCost(rightX - leftX + 1, DBL_MAX);
        std::vector<bool> isVisit(rightX - leftX + 1, false); 
        std::priority_queue<NodeRecord, std::vector<NodeRecord>, NodeCompare> pq;
        int offsetY = y-botY;
        //setting initial cost with verical edge  
        for(int x = leftX; x <= rightX; ++x){
            double cost = horiCost2[x-leftX][offsetY-1] + getCost(std::make_pair(x, y), std::make_pair(x, y-1));
            pq.push({x, y, x, y-1, cost});
        }

        int remain = rightX - leftX + 1;
        while(remain > 0){
            assert(!pq.empty());
            NodeRecord record = pq.top();
            pq.pop();
            int offsetX = record.x - leftX;
            if(isVisit[offsetX]){continue;}
            horiparent2[offsetX][offsetY] = std::make_pair(record.px, record.py);
            rowCost[offsetX] = record.cost;
            isVisit[offsetX] = true;
            if(record.x > leftX){
                double cost = rowCost[offsetX] + getCost(std::make_pair(record.x, y), std::make_pair(record.x-1, y));
                pq.push({record.x-1, y, record.x, y, cost});
            }
            if(record.x < rightX){
                double cost = rowCost[offsetX] + getCost(std::make_pair(record.x, y), std::make_pair(record.x+1, y));
                pq.push({record.x+1, y, record.x, y, cost});
            }
            --remain;
        }
        for(int i = 0; i < (int)rowCost.size(); ++i){
            horiCost2[i][offsetY] = rowCost[i];
        }
    }

    for(int y = y2-1; y >= botY; --y){//below
        std::vector<double> rowCost(rightX - leftX + 1, DBL_MAX);
        std::vector<bool> isVisit(rightX - leftX + 1, false); 
        std::priority_queue<NodeRecord, std::vector<NodeRecord>, NodeCompare> pq;
        int offsetY = y-botY;
        //setting initial cost with verical edge  
        for(int x = leftX; x <= rightX; ++x){
            double cost = horiCost2[x-leftX][offsetY+1] + getCost(std::make_pair(x, y), std::make_pair(x, y+1));
            pq.push({x, y, x, y+1, cost});
        }

        int remain = rightX - leftX + 1;
        while(remain > 0){
            assert(!pq.empty());
            NodeRecord record = pq.top();
            pq.pop();
            int offsetX = record.x - leftX;
            if(isVisit[offsetX]){continue;}
            horiparent2[offsetX][offsetY] = std::make_pair(record.px, record.py);
            rowCost[offsetX] = record.cost;
            isVisit[offsetX] = true;
            if(record.x > leftX){
                double cost = rowCost[offsetX] + getCost(std::make_pair(record.x, y), std::make_pair(record.x-1, y));
                pq.push({record.x-1, y, record.x, y, cost});
            }
            if(record.x < rightX){
                double cost = rowCost[offsetX] + getCost(std::make_pair(record.x, y), std::make_pair(record.x+1, y));
                pq.push({record.x+1, y, record.x, y, cost});
            }
            --remain;
        }
        for(int i = 0; i < (int)rowCost.size(); ++i){
            horiCost2[i][offsetY] = rowCost[i];
        }
    }

    // for(auto r: horiCost1){
    //     for(auto d: r){
    //         std::cout<<d<<" ";
    //     }
    //     std::cout<<std::endl;
    // }
    // std::cout<<"----------"<<std::endl;
    // for(auto r: horiCost2){
    //     for(auto d: r){
    //         std::cout<<d<<" ";
    //     }
    //     std::cout<<std::endl;
    // }
    // std::cout<<"----------"<<std::endl;



    // vert mono p1
    for(int y = y1-1; y >= botY; --y){
        vertCost1[x1-leftX][y-botY] = vertCost1[x1-leftX][y-botY+1] + getCost(std::make_pair(x1, y), std::make_pair(x1, y+1));
        vertparent1[x1-leftX][y-botY] = std::make_pair(x1, y+1);
    }
    for(int y = y1+1; y <= topY; ++y){
        vertCost1[x1-leftX][y-botY] = vertCost1[x1-leftX][y-botY-1] + getCost(std::make_pair(x1, y), std::make_pair(x1, y-1));
        vertparent1[x1-leftX][y-botY] = std::make_pair(x1, y-1);
    }

    for(int x = x1+1; x <= rightX; ++x){ // right
        std::vector<double> colCost(topY - botY + 1, DBL_MAX);
        std::vector<bool> isVisit(topY - botY + 1, false);
        std::priority_queue<NodeRecord, std::vector<NodeRecord>, NodeCompare> pq;
        int offsetX = x - leftX;

        for(int y = botY; y <= topY; ++y){
            double cost = vertCost1[offsetX-1][y-botY] + getCost(std::make_pair(x, y), std::make_pair(x-1, y));
            pq.push({x, y, x-1, y, cost});
        }

        int remain = topY - botY + 1;
        while(remain > 0){
            assert(!pq.empty());
            NodeRecord record = pq.top(); pq.pop();
            int offsetY = record.y - botY;
            if(isVisit[offsetY]) continue;
            vertparent1[offsetX][offsetY] = std::make_pair(record.px, record.py);
            colCost[offsetY] = record.cost;
            isVisit[offsetY] = true;

            if(record.y > botY){
                double cost = colCost[offsetY] + getCost(std::make_pair(x, record.y), std::make_pair(x, record.y-1));
                pq.push({x, record.y-1, x, record.y, cost});
            }
            if(record.y < topY){
                double cost = colCost[offsetY] + getCost(std::make_pair(x, record.y), std::make_pair(x, record.y+1));
                pq.push({x, record.y+1, x, record.y, cost});
            }
            --remain;
        }
        for(int i = 0; i < (int)colCost.size(); ++i)
            vertCost1[offsetX][i] = colCost[i];
    }

    for(int x = x1-1; x >= leftX; --x){ // left
        std::vector<double> colCost(topY - botY + 1, DBL_MAX);
        std::vector<bool> isVisit(topY - botY + 1, false);
        std::priority_queue<NodeRecord, std::vector<NodeRecord>, NodeCompare> pq;
        int offsetX = x - leftX;

        for(int y = botY; y <= topY; ++y){
            double cost = vertCost1[offsetX+1][y-botY] + getCost(std::make_pair(x, y), std::make_pair(x+1, y));
            pq.push({x, y, x+1, y, cost});
        }
        int remain = topY - botY + 1;
        while(remain > 0){
            assert(!pq.empty());
            NodeRecord record = pq.top(); pq.pop();
            int offsetY = record.y - botY;
            if(isVisit[offsetY]) continue;
            vertparent1[offsetX][offsetY] = std::make_pair(record.px, record.py);
            colCost[offsetY] = record.cost;
            isVisit[offsetY] = true;

            if(record.y > botY){
                double cost = colCost[offsetY] + getCost(std::make_pair(x, record.y), std::make_pair(x, record.y-1));
                pq.push({x, record.y-1, x, record.y, cost});
            }
            if(record.y < topY){
                double cost = colCost[offsetY] + getCost(std::make_pair(x, record.y), std::make_pair(x, record.y+1));
                pq.push({x, record.y+1, x, record.y, cost});
            }

            --remain;
        }
        for(int i = 0; i < (int)colCost.size(); ++i)
            vertCost1[offsetX][i] = colCost[i];
    }
    // vert mono p2
    for(int y = y2-1; y >= botY; --y){
        vertCost2[x2-leftX][y-botY] = vertCost2[x2-leftX][y-botY+1] + getCost(std::make_pair(x2, y), std::make_pair(x2, y+1));
        vertparent2[x2-leftX][y-botY] = std::make_pair(x2, y+1);
    }
    for(int y = y2+1; y <= topY; ++y){
        vertCost2[x2-leftX][y-botY] = vertCost2[x2-leftX][y-botY-1] + getCost(std::make_pair(x2, y), std::make_pair(x2, y-1));
        vertparent2[x2-leftX][y-botY] = std::make_pair(x2, y-1);
    }

    for(int x = x2+1; x <= rightX; ++x){ // right
        std::vector<double> colCost(topY - botY + 1, DBL_MAX);
        std::vector<bool> isVisit(topY - botY + 1, false);
        std::priority_queue<NodeRecord, std::vector<NodeRecord>, NodeCompare> pq;
        int offsetX = x - leftX;

        for(int y = botY; y <= topY; ++y){
            double cost = vertCost2[offsetX-1][y-botY] + getCost(std::make_pair(x, y), std::make_pair(x-1, y));
            pq.push({x, y, x-1, y, cost});
        }

        int remain = topY - botY + 1;
        while(remain > 0){
            assert(!pq.empty());
            NodeRecord record = pq.top(); pq.pop();
            int offsetY = record.y - botY;
            if(isVisit[offsetY]) continue;
            vertparent2[offsetX][offsetY] = std::make_pair(record.px, record.py);
            colCost[offsetY] = record.cost;
            isVisit[offsetY] = true;

            if(record.y > botY){
                double cost = colCost[offsetY] + getCost(std::make_pair(x, record.y), std::make_pair(x, record.y-1));
                pq.push({x, record.y-1, x, record.y, cost});
            }
            if(record.y < topY){
                double cost = colCost[offsetY] + getCost(std::make_pair(x, record.y), std::make_pair(x, record.y+1));
                pq.push({x, record.y+1, x, record.y, cost});
            }
            --remain;
        }
        for(int i = 0; i < (int)colCost.size(); ++i)
            vertCost2[offsetX][i] = colCost[i];
    }

    for(int x = x2-1; x >= leftX; --x){ // left
        std::vector<double> colCost(topY - botY + 1, DBL_MAX);
        std::vector<bool> isVisit(topY - botY + 1, false);
        std::priority_queue<NodeRecord, std::vector<NodeRecord>, NodeCompare> pq;
        int offsetX = x - leftX;

        for(int y = botY; y <= topY; ++y){
            double cost = vertCost2[offsetX+1][y-botY] + getCost(std::make_pair(x, y), std::make_pair(x+1, y));
            pq.push({x, y, x+1, y, cost});
        }

        int remain = topY - botY + 1;
        while(remain > 0){
            assert(!pq.empty());
            NodeRecord record = pq.top(); pq.pop();
            int offsetY = record.y - botY;
            if(isVisit[offsetY]) continue;
            vertparent2[offsetX][offsetY] = std::make_pair(record.px, record.py);
            colCost[offsetY] = record.cost;
            isVisit[offsetY] = true;

            if(record.y > botY){
                double cost = colCost[offsetY] + getCost(std::make_pair(x, record.y), std::make_pair(x, record.y-1));
                pq.push({x, record.y-1, x, record.y, cost});
            }
            if(record.y < topY){
                double cost = colCost[offsetY] + getCost(std::make_pair(x, record.y), std::make_pair(x, record.y+1));
                pq.push({x, record.y+1, x, record.y, cost});
            }

            --remain;
        }
        for(int i = 0; i < (int)colCost.size(); ++i){
            vertCost2[offsetX][i] = colCost[i];
        }
    }

    // for(auto r: vertCost1){
    //     for(auto d: r){
    //         std::cout<<d<<" ";
    //     }
    //     std::cout<<std::endl;
    // }
    // std::cout<<"----------"<<std::endl;
    // for(auto r: vertCost2){
    //     for(auto d: r){
    //         std::cout<<d<<" ";
    //     }
    //     std::cout<<std::endl;
    // }
    // std::cout<<"-========"<<std::endl;

    // std::cout<<"pass"<<std::endl;

    //create path
    std::vector<std::pair<int, int>> path;
    double minCost = DBL_MAX;
    std::pair<int, int> minPoint(-1, -1);
    for(int x = leftX; x <= rightX; ++x){
        for(int y = botY; y <= topY; ++y){
            double cost1 = std::min(horiCost1[x-leftX][y-botY], vertCost1[x-leftX][y-botY]);
            double cost2 = std::min(horiCost2[x-leftX][y-botY], vertCost2[x-leftX][y-botY]);
            if(cost1 + cost2 < minCost){
                minPoint = std::make_pair(x, y);
                minCost = cost1 + cost2;
            }
        }
    }
    // std::cout<<minPoint.first<<" "<<minPoint.second<<std::endl;
    // minPoint.first += leftX;
    // minPoint.second += botY;
    if(horiCost1[minPoint.first-leftX][minPoint.second-botY] < vertCost1[minPoint.first-leftX][minPoint.second-botY]){
        std::pair<int, int> curr = minPoint;
        while(horiparent1[curr.first-leftX][curr.second-botY] != curr){
            path.push_back(curr);
            curr = horiparent1[curr.first-leftX][curr.second-botY];
        }
        path.push_back(curr);
    }
    else{
        std::pair<int, int> curr = minPoint;
        while(vertparent1[curr.first-leftX][curr.second-botY] != curr){
            path.push_back(curr);
            curr = vertparent1[curr.first-leftX][curr.second-botY];
        }
        path.push_back(curr);
    }
    std::reverse(path.begin(), path.end());
    path.pop_back();
    if(horiCost2[minPoint.first-leftX][minPoint.second-botY] < vertCost2[minPoint.first-leftX][minPoint.second-botY]){
        std::pair<int, int> curr = minPoint;
        while(horiparent2[curr.first-leftX][curr.second-botY] != curr){
            path.push_back(curr);
            curr = horiparent2[curr.first-leftX][curr.second-botY];
        }
        path.push_back(curr);
    }
    else{
        std::pair<int, int> curr = minPoint;
        while(vertparent2[curr.first-leftX][curr.second-botY] != curr){
            path.push_back(curr);
            curr = vertparent2[curr.first-leftX][curr.second-botY];
        }
        path.push_back(curr);
    }

    // std::cout<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<std::endl;
    // std::cout<<minPoint.first<<" "<<minPoint.second<<std::endl;
    // std::cout<<horiparent1[minPoint.first-leftX][minPoint.second-botY].first<<" "<<horiparent1[minPoint.first-leftX][minPoint.second-botY].second<<std::endl;
    // std::cout<<vertparent1[minPoint.first-leftX][minPoint.second-botY].first<<" "<<vertparent1[minPoint.first-leftX][minPoint.second-botY].second<<std::endl;
    // std::cout<<horiparent2[minPoint.first-leftX][minPoint.second-botY].first<<" "<<horiparent2[minPoint.first-leftX][minPoint.second-botY].second<<std::endl;
    // std::cout<<vertparent2[minPoint.first-leftX][minPoint.second-botY].first<<" "<<vertparent2[minPoint.first-leftX][minPoint.second-botY].second<<std::endl;
    // for(auto p: horiparent1){
    //     for(auto pa: p){
    //         std::cout<<pa.first<<","<<pa.second<<" ";
    //     }
    //     std::cout<<std::endl;
    // }
    // std::cout<<path.size()<<std::endl;
    // for(auto p: path){
    //     std::cout<<p.first<<" "<<p.second<<std::endl;
    // }
    // std::cout<<"------"<<std::endl;
    setPath(n, path);
}

void Graph::AStarRoute(Net* n){
    assert(n != nullptr);
    assert(n->path.empty());
    int x1 = n->p1->x;
    int x2 = n->p2->x;
    int y1 = n->p1->y;
    int y2 = n->p2->y;


    std::vector<std::vector<double>> gVal(gridX, std::vector<double>(gridY, DBL_MAX));
    std::vector<std::vector<bool>> isVisit(gridX, std::vector<bool>(gridY, false));
    std::vector<std::vector<std::pair<int, int>>> parent(gridX, std::vector<std::pair<int, int>>(gridY, std::make_pair(-1, -1)));
    
    gVal[x1][y1] = 0;
    parent[x1][y1] = std::make_pair(x1, y1);

    struct NodeRecord {
        int x, y;           
        double f_cost;
    };
    struct NodeCompare {
        bool operator()(const NodeRecord& a, const NodeRecord& b) const {
            return a.f_cost > b.f_cost; 
        }
    };

    std::priority_queue<NodeRecord, std::vector<NodeRecord>, NodeCompare> pq;
    double startH = std::abs(x1 - x2) + std::abs(y1 - y2);
    pq.push({x1, y1, startH});

    int dx[4] = {-1, 1, 0, 0};
    int dy[4] = {0, 0, -1, 1};
    
    bool found = false; 

    while(!pq.empty()){
        NodeRecord record = pq.top();
        pq.pop();
        int x = record.x;
        int y = record.y;


        if(x == x2 && y == y2){
            found = true;
            break;
        }

        if(isVisit[x][y]){ continue; }
        isVisit[x][y] = true;

        for(int direct = 0; direct < 4; ++direct){
            int nx = x + dx[direct];
            int ny = y + dy[direct];

            if(nx >= 0 && nx < gridX && ny >= 0 && ny < gridY) {
                if(isVisit[nx][ny]) continue;

                double edgeCost = getCost(std::make_pair(nx, ny), std::make_pair(x, y));
                double new_g = gVal[x][y] + edgeCost;

                if (new_g < gVal[nx][ny]) {
                    gVal[nx][ny] = new_g;            
                    parent[nx][ny] = std::make_pair(x, y);
                    
                    double h = std::abs(nx - x2) + std::abs(ny - y2);
                    double f = new_g + h;
                    

                    pq.push({nx, ny, f});
                }
            }
        }
    }

    if (found) {
        std::vector<std::pair<int, int>> path;
        std::pair<int, int> currPoint(x2, y2);
        
        while(parent[currPoint.first][currPoint.second] != currPoint){
            path.push_back(currPoint);
            currPoint = parent[currPoint.first][currPoint.second];
        }
        path.push_back(currPoint);
        setPath(n, path);
    } 
}
void Graph::MazeRoute(Net* n){
    assert(n != nullptr);
    assert(n->path.empty());
    int x1 = n->p1->x;
    int x2 = n->p2->x;
    int y1 = n->p1->y;
    int y2 = n->p2->y;

    std::vector<std::vector<double>> overflow(gridX, std::vector<double>(gridY, DBL_MAX));
    std::vector<std::vector<bool>> isVisit(gridX, std::vector<bool>(gridY, false));
    std::vector<std::vector<std::pair<int, int>>> parent(gridX, std::vector<std::pair<int, int>>(gridY, std::make_pair(-1, -1)));
    overflow[x1][y1] = 0;
    parent[x1][y1] = std::make_pair(x1, y1);


    struct NodeRecord {
        int x, y;           
        int px, py;            
        double cost;            
    };
    struct NodeCompare {
        bool operator()(const NodeRecord& a, const NodeRecord& b) const {
            return a.cost > b.cost; 
        }
    };

    std::priority_queue<NodeRecord, std::vector<NodeRecord>, NodeCompare> pq;
    pq.push({x1, y1, x1, y1, 0});

    while(!pq.empty()){
        NodeRecord record = pq.top();
        pq.pop();
        int x = record.x;
        int y = record.y;
        if(isVisit[x][y]){continue;}
        parent[x][y] = std::make_pair(record.px, record.py);
        overflow[x][y] = record.cost;
        isVisit[x][y] = true;
        if(x == x2 && y == y2){break;}
        if(x > 0){
            double of = overflow[x][y] + std::max(demandHori[x-1][y]+1 - capHori, 0);
            pq.push({x-1, y, x, y, of});
        }
        if(x < gridX-1){
            double of = overflow[x][y] + std::max(demandHori[x][y]+1 - capHori, 0);
            pq.push({x+1, y, x, y, of});
        }
        if(y > 0){
            double of = overflow[x][y] + std::max(demandVert[x][y-1]+1 - capVert, 0);
            pq.push({x, y-1, x, y, of});
        }
        if(y < gridY-1){
            double of = overflow[x][y] + std::max(demandVert[x][y]+1 - capVert, 0);
            pq.push({x, y+1, x, y, of});
        }
    }
    std::vector<std::pair<int, int>> path;
    std::pair<int, int> currPoint(x2, y2);
    while(parent[currPoint.first][currPoint.second] != currPoint){
        path.push_back(currPoint);
        currPoint = parent[currPoint.first][currPoint.second];
    }
    path.push_back(currPoint);
    setPath(n, path);
}