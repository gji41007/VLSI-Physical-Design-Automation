#ifndef PARTITION_H
#define PARTITION_H


#include <vector>
#include <list>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <cmath>
#include "graph.h"

class GainBucket {
private:                          
    int numNodes;  
    int pMax;                  
    boost::container::vector<std::list<int>> gainBuckets; 
    boost::container::vector<int> gainTable; 
    boost::container::vector<std::list<int>::iterator> nodePtr;        
    int countTol;
public:
    GainBucket(){}
    GainBucket(int _numNodes, int _pMax) : numNodes(_numNodes), pMax(_pMax) {}
    void initBucket(boost::container::vector<int> gain){
        gainTable = gain;
        gainBuckets.resize(2 * pMax + 1);
        nodePtr.resize((int)gain.size());
        for (int i = 0; i < (int)gain.size(); ++i){//(auto& p : gain) {
            gainBuckets[gain[i] + pMax].push_back(i);
            nodePtr[i] = --gainBuckets[gain[i] + pMax].end(); 
            // isLocked.insert(std::make_pair(i, false));
        }
        countTol = 100;
        // for(int i = 0; i <= 2*pMax; ++i){
        //     std::cout<<i<<": ";
        //     for(auto n : gainBuckets[i]W{std::cout<<n<<" ";}
        //     std::cout<<std::endl;
        // }
    }
    int getGain(int cID){return gainTable[cID];}
    int getMax(boost::container::vector<int> nodePartition, boost::container::vector<bool> unbalanceSet){
        bool isMax = true;
        for(int i = 2*pMax; i >= 0; --i){
            for(auto& cID : gainBuckets[i]){
                if(isMax){
                    if(gainTable[cID] < 0){
                        // return -1;

                        return cID;
                    }
                    isMax = false;
                }
                if(countTol>0 && unbalanceSet[nodePartition[cID]]){--countTol;return cID;}
                if(!unbalanceSet[nodePartition[cID]]){return cID;}
                //else{std::cout<<i_node<<" is not balanceed"<<std::endl;}
            }
        }
        return -1;
    }
    
    void deleteNode(int cID){
        gainBuckets[gainTable[cID] + pMax].erase(nodePtr[cID]);
        // nodePtr[cID] = nullptr;
    }
    void adjust(int cID, int val){
        //validateNodePtrs();
        deleteNode(cID);
        gainTable[cID] += val;
        gainBuckets[gainTable[cID] + pMax].push_back(cID);
        nodePtr[cID] = --gainBuckets[gainTable[cID] + pMax].end();
    }
    void dump(){
        for(int i = 2*pMax; i >= 0; --i){
            std::cout<<i-pMax<<": ";
            for(auto& cID: gainBuckets[i]){std::cout<<cID<<" ";}
            std::cout<<std::endl;
        }
    }
    void dumpSize(){
        std::cout<<"Bucket index i("<<pMax<<" ~ -"<<pMax<<"): num of cell in bucket[i]"<<std::endl;
        for(int i = 2*pMax; i >= 0; --i){
            if(gainBuckets[i].empty()){continue;}
            std::cout<<i-pMax<<": "<<gainBuckets[i].size();
            std::cout<<std::endl;
        }
    }
};


class Partition2{
public:
    Partition2(Graph _graph);
    Partition2(Graph _graph, int _maxWeight);
    void initPartition();
    void initPartitionRandom();
    void initGain();
    void dumpPartition(std::string outputFile);
    void run();
    void moveCell(Cell* movedCell);
    void setBalanceBound(float l, float u){lowerBound = l;upperBound = u;}
    void dumpBucket(){gainbucket.dumpSize();}

    int getCutSize();
    boost::container::vector<int> getPartition(){return cellPartition;}
private:
    int maxWeight; 
    float minRatio = 0.5;
    int sumWeight;  
    float lowerBound = -1;//for Partition4
    float upperBound = -1;//for Partition4
    Graph graph;
    // boost::container::vector<boost::container::vector<Cell*>> Part1, Part2;
    boost::container::vector<int> partSize;
    boost::container::vector<int> cellPartition;
    GainBucket gainbucket;
    boost::container::vector<bool> isLock;
};

class Partition4{
public:
    Partition4(Graph graph){origiGraph = graph;}
    void run();
    void run2();
    int getCutSize();
    void dumpPartition(std::string outputFile);
private:
    int maxWeight;
    Graph origiGraph;
    boost::unordered_map<std::string, int> cellPartition;
    boost::container::vector<boost::container::vector<std::string>> partCells;
};

#endif
