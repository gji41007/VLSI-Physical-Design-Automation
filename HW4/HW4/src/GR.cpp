#include "GR.h"

#include <algorithm>

void GlobalRouter::run(){
    graph.parse(inputPath);
    graph.init();

    // std::cout<<"Before totalWL:"<<graph.getTotalWL()<<std::endl;
    PatternRoute_Initial();
    graph.addHistCost();
    for(int iter = 0; iter < 5; ++iter){
        PatternRoute();
        graph.addHistCost();
    }
    for(int iter = 0; iter < 5; ++iter){
        MonoRoute();
        graph.addHistCost();
    }


    for(int iter = 0; iter < 20; ++iter){
        if(!AStarRoute()){break;}
        graph.addHistCost();
    }
    // for(int k = 0; k < 5; ++k){
    // for(int k = 0; k < 5; ++k){
    //     if(!HUMRoute(k)){break;}
    //     graph.addHistCost();
    // }
    // }
    MAZERoute();
    // for(int iter = 0; iter < 3; ++iter){
    //     if(!MAZERoute()){break;}
    // }
    
    // for(int iter = 0; iter < 10; ++iter){
    //     for(int k = 0; k < 5; ++k){
    //         if(HUMRoute(k)){break;}
    //     }
    // MAZERoute();
    // }
    // std::cout<<"After totalWL:"<<graph.getTotalWL()<<std::endl;
    // int ofCount = 0;
    // for(int i = 0; i < graph.gridX-1; ++i){
    //     for(int j = 0; j < graph.gridY; ++j){
    //         ofCount += std::max(0, graph.demandHori[i][j]-graph.capHori);
    //     }
    // }
    // for(int i = 0; i < graph.gridX; ++i){
    //     for(int j = 0; j < graph.gridY-1; ++j){
    //         ofCount += std::max(0, graph.demandVert[i][j]-graph.capVert);
    //     }
    // }
    // std::cout<<"Remaining: "<<ofCount<<" overflow after routing"<<std::endl;

    graph.dump(outputPath);
}


void GlobalRouter::PatternRoute_Initial(){
    for(auto net: graph.nets){
        graph.PatternRoute(net);
    }
}

void GlobalRouter::PatternRoute(){
    std::vector<std::pair<Net*, int>> ofNets;
    for(auto net: graph.nets){
        if(graph.isOverflow(net)){
            // graph.ripUp(net);
            // graph.HUMRoute(net, k);
            ofNets.push_back(std::make_pair(net, graph.getPathOverflow(net)));
        }
    }
    std::sort(ofNets.begin(), ofNets.end(), 
        [](const std::pair<Net*, int>& a, const std::pair<Net*, int>& b) {
            if (a.second != b.second) {
                return a.second > b.second;
            }
            
            return a.first->getHPWL() > b.first->getHPWL();
        }
    );
    for(auto p: ofNets){
        Net* net = p.first;
        if(graph.isOverflow(net)){
            graph.ripUp(net);
            graph.PatternRoute(net);
        }
    }
    return;
}
void GlobalRouter::MonoRoute(){
    std::vector<std::pair<Net*, int>> ofNets;
    for(auto net: graph.nets){
        if(graph.isOverflow(net)){
            // graph.ripUp(net);
            // graph.HUMRoute(net, k);
            ofNets.push_back(std::make_pair(net, graph.getPathOverflow(net)));
        }
    }
    std::sort(ofNets.begin(), ofNets.end(), 
        [](const std::pair<Net*, int>& a, const std::pair<Net*, int>& b) {
            if (a.second != b.second) {
                return a.second > b.second;
            }
            return a.first->getHPWL() > b.first->getHPWL();
        }
    );
    for(auto p: ofNets){
        Net* net = p.first;
        if(graph.isOverflow(net)){
            graph.ripUp(net);
            graph.MonoRoute(net);
        }
    }
    return;
}
bool GlobalRouter::HUMRoute(int k){
    std::vector<std::pair<Net*, int>> ofNets;
    bool isOF = false;
    for(auto net: graph.nets){
        if(graph.isOverflow(net)){
            // graph.ripUp(net);
            // graph.HUMRoute(net, k);
            ofNets.push_back(std::make_pair(net, graph.getPathOverflow(net)));
            isOF = true;
        }
    }
    std::sort(ofNets.begin(), ofNets.end(), 
        [](const std::pair<Net*, int>& a, const std::pair<Net*, int>& b) {
            if (a.second != b.second) {
                return a.second > b.second;
            }
            
            return a.first->getHPWL() > b.first->getHPWL(); 
        }
    );
    for(auto p: ofNets){
        Net* net = p.first;
        if(graph.isOverflow(net)){
            graph.ripUp(net);
            graph.HUMRoute(net, k);
        }
    }
    return isOF;
}
bool GlobalRouter::AStarRoute(){
    std::vector<std::pair<Net*, int>> ofNets;
    bool isOF = false;
    for(auto net: graph.nets){
        if(graph.isOverflow(net)){
            // graph.ripUp(net);
            // graph.HUMRoute(net, k);
            ofNets.push_back(std::make_pair(net, graph.getPathOverflow(net)));
            isOF = true;
        }
    }
    std::sort(ofNets.begin(), ofNets.end(), 
        [](const std::pair<Net*, int>& a, const std::pair<Net*, int>& b) {
            if (a.second != b.second) {
                return a.second > b.second;
            }
            
            return a.first->getHPWL() > b.first->getHPWL();
        }
    );
    for(auto p: ofNets){
        Net* net = p.first;
        if(graph.isOverflow(net)){
            graph.ripUp(net);
            graph.AStarRoute(net);
        }
    }
    return isOF;
    // bool isOF = false;
    // for(auto net: graph.nets){
    //     if(graph.isOverflow(net)){
    //         graph.ripUp(net);
    //         graph.AStarRoute(net);
    //         isOF = true;
    //     }
    // }
    // return isOF;
}

bool GlobalRouter::MAZERoute(){
    std::vector<std::pair<Net*, int>> ofNets;
    bool isOF = false;
    for(auto net: graph.nets){
        if(graph.isOverflow(net)){
            // graph.ripUp(net);
            // graph.HUMRoute(net, k);
            ofNets.push_back(std::make_pair(net, graph.getPathOverflow(net)));
            isOF = true;
        }
    }
    std::sort(ofNets.begin(), ofNets.end(), 
        [](const std::pair<Net*, int>& a, const std::pair<Net*, int>& b) {
            if (a.second != b.second) {
                return a.second < b.second;
            }
            return a.first->getHPWL() > b.first->getHPWL(); 
        }
    );
    for(auto p: ofNets){
        Net* net = p.first;
        if(graph.isOverflow(net)){
            graph.ripUp(net);
            graph.MazeRoute(net);
        }
    }
    return isOF;
    // bool isOF = false;
    // for(auto net: graph.nets){
    //     if(graph.isOverflow(net)){
    //         graph.ripUp(net);
    //         graph.MazeRoute(net);
    //         isOF = true;
    //     }
    // }
    // return isOF;
}

