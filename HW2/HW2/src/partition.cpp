#include "partition.h"
#include <queue>
#include <random>


Partition2::Partition2(Graph _graph){
    graph = _graph;
    //set sumWeight
    maxWeight = -1;
    sumWeight = 0;
    for(auto& c: graph.cells){
        sumWeight += c->size;
        maxWeight = std::max(maxWeight, c->size);
    }
    lowerBound = -1;
    //init bucket
    int pMax = -1;
    for(auto& cellPtr: graph.cells){
        pMax = ((int)cellPtr->nets.size() > pMax)? cellPtr->nets.size():pMax;
    }
    gainbucket = GainBucket(graph.cells.size(), pMax);
}

Partition2::Partition2(Graph _graph, int _maxWeight){
    graph = _graph;
    //set sumWeight
    sumWeight = 0;
    for(auto& c: graph.cells){
        sumWeight += c->size;
    }
    maxWeight = _maxWeight;
    //init bucket
    int pMax = -1;
    for(auto& cellPtr: graph.cells){
        pMax = ((int)cellPtr->nets.size() > pMax)? cellPtr->nets.size():pMax;
    }
    gainbucket = GainBucket(graph.cells.size(), pMax);
}
void Partition2::initPartition(){
    int sep = 100;
    int minCutSize = INT32_MAX;
    boost::container::vector<int> bestPartSize;
    boost::container::vector<int> bestCellPartition;
    for(int startID = 0; startID < graph.numCell; startID += (graph.numCell/sep)){

        partSize = boost::container::vector<int>(5, 0);
        cellPartition = boost::container::vector<int>(graph.cells.size(), 0);



        boost::container::vector<bool> isSelect(graph.cells.size(), false);
        std::queue<Cell*> cellQueue;
        isSelect[startID] = true;
        partSize[1] += graph.cells[startID]->size;
        cellPartition[graph.cells[startID]->id] = 1;
        for(auto& n: graph.cells[startID]->nets){
            for(auto& c: n->cells){
                if(!isSelect[c->id]){cellQueue.push(c);isSelect[c->id] = true;}
            }
        }
        // std::cout<<"pass"<<std::endl;
        while(partSize[1] <= minRatio*sumWeight){
            if(cellQueue.empty()){
                for(int i = 0; i < (int)graph.cells.size(); ++i){
                    if(!isSelect[i]){
                        isSelect[i] = true;
                        partSize[1] += graph.cells[i]->size;
                        cellPartition[graph.cells[i]->id] = 1;
                        for(auto& n: graph.cells[i]->nets){
                            for(auto& c: n->cells){
                                if(!isSelect[c->id]){cellQueue.push(c);isSelect[c->id] = true;}
                            }
                        }
                        break;
                    }
                }
            }
            else{
                Cell* currCell = cellQueue.front();
                // std::cout<<currCell->id<<std::endl;
                cellQueue.pop();
                partSize[1] += graph.cells[currCell->id]->size;
                cellPartition[currCell->id] = 1;
                for(auto& n: graph.cells[currCell->id]->nets){
                    for(auto& c: n->cells){
                        if(!isSelect[c->id]){cellQueue.push(c);isSelect[c->id] = true;}
                    }
                }
            }
        }
        for(auto& c: graph.cells){
            if(cellPartition[c->id] == 0){
                cellPartition[c->id] = 2;
                partSize[2] += c->size;
            }
        }

        if(getCutSize() < minCutSize){
            minCutSize = getCutSize();
            bestPartSize = partSize;
            bestCellPartition = cellPartition;
        }
    }
    partSize = bestPartSize;
    cellPartition = bestCellPartition;
    // std::cout<<minRatio*sumWeight<<" "<<partSize[1]<<" "<<partSize[2]<<std::endl;
}
void Partition2::initPartitionRandom(){

    partSize = boost::container::vector<int>(5, 0);
    cellPartition = boost::container::vector<int>(graph.cells.size(), 0);

    std::random_device rd;
    std::mt19937 generator(rd()); 
    std::vector<int> indices(graph.numCell);
    for (int i = 0; i < graph.numCell; ++i) indices[i] = i;
    std::shuffle(indices.begin(), indices.end(), generator);
    int id = 0;
    for(; partSize[1] < sumWeight * 0.5; ++id){
        cellPartition[indices[id]] = 1;
        partSize[1] += graph.cells[indices[id]]->size;
    }
    for(; id < graph.numCell; ++id){
        cellPartition[indices[id]] = 2;
        partSize[2] += graph.cells[indices[id]]->size;
    }
    // std::cout<<minRatio*sumWeight<<" "<<partSize[1]<<" "<<partSize[2]<<std::endl;
}
void Partition2::initGain(){
    boost::container::vector<int> gain(graph.numCell);
    boost::unordered_map<std::string, int> numCellPart1;
    boost::unordered_map<std::string, int> numCellPart2;
    for(auto& netPtr: graph.nets){
        int num1 = 0, num2 = 0;
        for(auto& cellPtr: netPtr->cells){
            if(cellPartition[cellPtr->id] == 1){++num1;}
            else if(cellPartition[cellPtr->id] == 2){++num2;}
            else{std::cerr<<"error"<<std::endl;exit(-1);}
        }
        numCellPart1[netPtr->name] = num1;
        numCellPart2[netPtr->name] = num2;
    }

    for(auto& cellPtr: graph.cells){
        int sumGain = 0;
        for(auto& netPtr: cellPtr->nets){
            int from, to;
            if(cellPartition[cellPtr->id] == 1){from = numCellPart1[netPtr->name]; to = numCellPart2[netPtr->name];}
            else if(cellPartition[cellPtr->id] == 2){from = numCellPart2[netPtr->name]; to = numCellPart1[netPtr->name];}
            else{std::cerr<<"error"<<std::endl;exit(-1);}
            if(from == 1){++sumGain;}
            if(to == 0){--sumGain;}
        }
        gain[cellPtr->id] = sumGain;
    }

    gainbucket.initBucket(gain);
    // gainbucket.dump();
}
void Partition2::dumpPartition(std::string outputFile){
    std::ofstream outFile(outputFile);  
    if (!outFile) {
        std::cerr << "Error opening output file!" << std::endl;
        exit(-1);
    }

    int numPart1 = 0, numPart2 = 0;
    for(auto& c: graph.cells){
        if(cellPartition[c->id] == 1){++numPart1;}
        else if(cellPartition[c->id] == 2){++numPart2;}
        else{std::cerr<<"error"<<std::endl;exit(-1);}
    }

    outFile<<"CutSize "<<getCutSize()<<std::endl;
    outFile<<"GroupA "<<numPart1<<std::endl;
    for(int i = 0; i < (int)cellPartition.size(); ++i){
        if(cellPartition[i] == 1){outFile<<graph.cells[i]->name<<std::endl;}
    }
    outFile<<"GroupB "<<numPart2<<std::endl;
    for(int i = 0; i < (int)cellPartition.size(); ++i){
        if(cellPartition[i] == 2){outFile<<graph.cells[i]->name<<std::endl;}
    }
}

void Partition2::moveCell(Cell* movedCell){
    boost::container::vector<int> updateVal(graph.cells.size(), 0);

    //before move
    for(auto& n: movedCell->nets){
        int to = 0;
        for(auto& c: n->cells){
            if(cellPartition[c->id] != cellPartition[movedCell->id]){++to;}
        }
        if(to == 0){
            for(auto& c: n->cells){
                 ++updateVal[c->id];
            }
        }
        else if(to == 1){
            for(auto& c: n->cells){
                if(cellPartition[c->id] != cellPartition[movedCell->id]){
                    --updateVal[c->id];
                }
            }
        }
    }


    //perform the move
    partSize[cellPartition[movedCell->id]] -= movedCell->size;
    cellPartition[movedCell->id] = (cellPartition[movedCell->id] == 1)? 2:1;
    partSize[cellPartition[movedCell->id]] += movedCell->size;

    //after move
    for(auto& n: movedCell->nets){
        int from = 0;
        for(auto& c: n->cells){
            if(cellPartition[c->id] != cellPartition[movedCell->id]){++from;}
        }
        if(from == 0){
            for(auto& c: n->cells){
                --updateVal[c->id];
            }
        }
        else if(from == 1){
            for(auto& c: n->cells){
                if(cellPartition[c->id] != cellPartition[movedCell->id]){
                    ++updateVal[c->id];
                }
            }
        }
    }

    //update
    for(int i = 0; i < (int)updateVal.size(); ++i){
        if(!isLock[i] && updateVal[i] != 0){
            gainbucket.adjust(graph.cells[i]->id, updateVal[i]);
        }
    }
}
void Partition2::run(){
    int remainNum = graph.cells.size();


    int currGain = 0;
    int bestGain = 0;
    boost::container::vector<int> bestPartSize = partSize;
    boost::container::vector<int> bestCellPartition = cellPartition;
    boost::container::vector<bool> isUnbalance(5, false);
    isLock = boost::container::vector<bool>(graph.cells.size(), false);
    int negTol = graph.numCell*0.01;
    int countTol = negTol;
    for(; remainNum > 0; --remainNum){

        //mark partition if too small
        if(lowerBound == -1){
            if(partSize[1] < 0.9*minRatio*sumWeight){isUnbalance[1] = true;}
            else{isUnbalance[1] = false;}
            if(partSize[2] < 0.9*minRatio*sumWeight){isUnbalance[2] = true;}
            else{isUnbalance[2] = false;}
        }
        else{
            if(partSize[1] < lowerBound || partSize[2] > upperBound){isUnbalance[1] = true;}
            else{isUnbalance[1] = false;}
            if(partSize[2] < lowerBound || partSize[1] > upperBound){isUnbalance[2] = true;}
            else{isUnbalance[2] = false;}
        }

        
        int currID = gainbucket.getMax(cellPartition, isUnbalance);
        if(currID == -1){break;}
        // std::cout<<currName<<std::endl;
        Cell* currCell = graph.cells[currID];
        isLock[currCell->id] = true;
        currGain += gainbucket.getGain(currCell->id);
        if(currGain >= bestGain){countTol = negTol;}
        else{--countTol;}
        if(countTol <= 0){break;}
        gainbucket.deleteNode(currCell->id);

        moveCell(currCell);
        if(lowerBound == -1){
            if(currGain > bestGain && 0.9*minRatio*sumWeight <= partSize[1] && 0.9*minRatio*sumWeight <= partSize[2]){
                bestGain = currGain;
                bestPartSize = partSize;
                bestCellPartition = cellPartition;
            }
        }
        else{
            if(currGain > bestGain && lowerBound <= partSize[1] && partSize[1] <= upperBound 
                                   && lowerBound <= partSize[2] && partSize[2] <= upperBound){
        
                bestGain = currGain;
                bestPartSize = partSize;
                bestCellPartition = cellPartition;
            }
        }
    }
    partSize = bestPartSize;
    cellPartition = bestCellPartition;
}



int Partition2::getCutSize(){
    int sumCutsize = 0;
    for(auto& netPtr: graph.nets){
        bool in1 = false;
        bool in2 = false;
        for(auto& cellPtr: netPtr->cells){
            if(cellPartition[cellPtr->id] == 1){in1 = true;}
            else if(cellPartition[cellPtr->id] == 2){in2 = true;}
            else{std::cerr<<"error"<<std::endl;exit(-1);}
            if(in1 && in2){
                ++sumCutsize;
                break;
            }
        }
    }
    return sumCutsize;
}

// void Partition4::run(){
//     partCells = boost::container::vector<boost::container::vector<std::string>>(5);
//     Graph graph = origiGraph.copyGraph();
//     graph.setCellArray();
//     cellPartition.clear();

    
//     //A + BCD
    
//     Partition2 partition2(graph, 0.25);
//     partition2.initPartition();
//     partition2.initGain();
//     partition2.run();

//     boost::container::vector<int> tempCellPartition = partition2.getPartition();
//     for(int i = 0; i < (int)tempCellPartition.size(); ++i){
//         if(tempCellPartition[i] == 1){
//             partCells[1].push_back(graph.cells[i]->name);
//             cellPartition[graph.cells[i]->name] = 1;
//             graph.removeCell(i);
//         }
//     }
//     graph.resetGraph();

//     //B + CD
//     partition2 = Partition2(graph, 0.33);
//     partition2.initPartition();
//     partition2.initGain();
//     partition2.run();

//     tempCellPartition = partition2.getPartition();
//     for(int i = 0; i < (int)tempCellPartition.size(); ++i){
//         if(tempCellPartition[i] == 1){
//             partCells[2].push_back(graph.cells[i]->name);
//             cellPartition[graph.cells[i]->name] = 2;
//             graph.removeCell(i);
//         }
//     }
//     graph.resetGraph();
//     //C + D
//     partition2 = Partition2(graph, 0.5);
//     partition2.initPartition();
//     partition2.initGain();
//     partition2.run();

//     tempCellPartition = partition2.getPartition();
//     for(int i = 0; i < (int)tempCellPartition.size(); ++i){
//         if(tempCellPartition[i] == 1){
//             partCells[3].push_back(graph.cells[i]->name);
//             cellPartition[graph.cells[i]->name] = 3;
//         }
//         else{
//             partCells[4].push_back(graph.cells[i]->name);
//             cellPartition[graph.cells[i]->name] = 4;
//         }
//     }

// }
void Partition4::run2(){
    partCells = boost::container::vector<boost::container::vector<std::string>>(5);
    Graph graph = origiGraph.copyGraph();
    Graph CD = origiGraph.copyGraph();
    graph.setCellArray();
    CD.setCellArray();
    cellPartition.clear();
    maxWeight = -1;
    int totalSumWeight = 0;
    for(auto& c: graph.cells){
        totalSumWeight += c->size;
        maxWeight = std::max(maxWeight, c->size);
    }
    float lowerBound = 0.225 * totalSumWeight;
    float upperBound = 0.275 * totalSumWeight;
    // std::cout<<lowerBound<<std::endl;
    //AB + CD
    
    Partition2 partition2(graph, maxWeight);
    partition2.setBalanceBound(std::ceil(lowerBound)*2, std::floor(upperBound)*2);
    partition2.initPartition();
    partition2.initGain();
    partition2.run();

    boost::container::vector<int> tempCellPartition = partition2.getPartition();
    for(int i = 0; i < (int)tempCellPartition.size(); ++i){
        if(tempCellPartition[i] == 1){
            // partCells[1].push_back(graph.cells[i]->name);
            // cellPartition[graph.cells[i]->name] = 1;
            graph.removeCell(i);
        }
        else{
            CD.removeCell(i);
        }
    }
    graph.resetGraph();
    CD.resetGraph();

    //A + B
    partition2 = Partition2(graph, maxWeight);
    partition2.setBalanceBound(lowerBound, upperBound);
    partition2.initPartition();
    partition2.initGain();
    partition2.run();

    tempCellPartition = partition2.getPartition();
    for(int i = 0; i < (int)tempCellPartition.size(); ++i){
        if(tempCellPartition[i] == 1){
            partCells[1].push_back(graph.cells[i]->name);
            cellPartition[graph.cells[i]->name] = 1;
            // graph.removeCell(i);
        }
        else{
            partCells[2].push_back(graph.cells[i]->name);
            cellPartition[graph.cells[i]->name] = 2;
        }
    }
    //C + D
    partition2 = Partition2(CD, maxWeight);
    partition2.setBalanceBound(lowerBound, upperBound);
    partition2.initPartition();
    partition2.initGain();
    partition2.run();

    tempCellPartition = partition2.getPartition();
    for(int i = 0; i < (int)tempCellPartition.size(); ++i){
        if(tempCellPartition[i] == 1){
            partCells[3].push_back(CD.cells[i]->name);
            cellPartition[CD.cells[i]->name] = 3;
            // graph.removeCell(i);
        }
        else{
            partCells[4].push_back(CD.cells[i]->name);
            cellPartition[CD.cells[i]->name] = 4;
        }
    }

}
int Partition4::getCutSize(){
    int sumCutsize = 0;

    for(auto& netPtr: origiGraph.nets){
        // std::cout<<netPtr->cells.size()<<std::endl;
        bool in1 = false;
        bool in2 = false;
        bool in3 = false;
        bool in4 = false;
        for(auto& cellPtr: netPtr->cells){
            if(cellPartition[cellPtr->name] == 1){in1 = true;}
            else if(cellPartition[cellPtr->name] == 2){in2 = true;}
            else if(cellPartition[cellPtr->name] == 3){in3 = true;}
            else if(cellPartition[cellPtr->name] == 4){in4 = true;}
            else{std::cerr<<"error: cName "<<cellPtr->name<<" partition is "<<cellPartition[cellPtr->name]<<std::endl;exit(-1);}
            if((in1 + in2 + in3 + in4) >= 2){++sumCutsize;break;}
        }
    }
    return sumCutsize;
}

void Partition4::dumpPartition(std::string outputFile){
    std::ofstream outFile(outputFile);  
    if (!outFile) {
        std::cerr << "Error opening output file!" << std::endl;
        exit(-1);
    }

    // for(auto& v: partCells){
    //     std::cout<<v.size()<<" ";
    // }
    // int tcount[5] = {0, 0, 0, 0, 0};
    // for(auto&p: cellPartition){
    //     tcount[p.second]++;
    // }
    // for(auto n:tcount){std::cout<<n<<" ";}
    // std::cout<<std::endl;
    // for(int i = 1; i <= 4; ++i){
    //     boost::container::vector<int> v = partCells[i]; 
    //     for(auto& id: v){
    //         if(cellPartition[id] != i){
    //             // std::cerr<<"error: cID "<<id<<" partition is "<<i<<" not "<<cellPartition[id]<<std::endl;
    //         }
    //     }
    // }
    outFile<<"CutSize "<<getCutSize()<<std::endl;
    for(int i = 1; i <= 4; ++i){
        outFile<<"Group"<<(char)('A'+i-1)<<" "<<partCells[i].size()<<std::endl;
        for(auto& cName: partCells[i]){
            outFile<<cName<<std::endl;
        }
    }
}