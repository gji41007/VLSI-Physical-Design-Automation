#ifndef GRAPH_H
#define GRAPH_H


#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <unordered_map>
#include <memory>
#include <limits>

#include <boost/unordered_map.hpp>
#include <boost/container/vector.hpp>

class Net;
class Cell;
class Graph;

class Cell{
public:
    std::string name;
    int size;
    int id = -1;
    boost::container::vector<Net*> nets;
    Cell(std::string _name, int _size, int _id){
        name = _name;
        size = _size;
        id = _id;
    }
};

class Net{
public:
    std::string name;
    int numCell;
    int id;
    boost::container::vector<Cell*> cells;
    Net(std::string _name, int _numCell, int _id, boost::container::vector<Cell*> _cells){
        name = _name;
        numCell = _numCell;
        id = _id;
        cells = _cells;
    }
};

class Graph{
public:
    int numCell;
    int numNet;
    boost::container::vector<Cell*> cells;
    boost::container::vector<Net*> nets;
    boost::unordered_map<std::string, int> cellID;
    Graph(){
        cells.clear();
        nets.clear();
        cellID.clear();
    }
    Graph copyGraph(){
        Graph newGraph;
        newGraph.numCell = numCell;
        newGraph.numNet = numNet;
        for(auto& c: cells){
            Cell* tempCell = new Cell(c->name, c->size, c->id);
            newGraph.cells.push_back(tempCell);
            newGraph.cellID.insert(std::make_pair(c->name, c->id));
        }
        for(int nid = 0 ; nid < newGraph.numNet; ++nid){
            boost::container::vector<Cell*> netCells;
            for(auto& c: nets[nid]->cells){
                netCells.push_back(newGraph.cells[newGraph.cellID[c->name]]);
            }
            Net* tempNet = new Net(nets[nid]->name, nets[nid]->numCell, nid, netCells);
            newGraph.nets.push_back(tempNet);
        }
        return newGraph;
    }
    void removeCell(int cID){
        // if (cellID.find(cellName) == cellID.end()) {
        //     std::cerr << "Cell '" << cellName << "' not found while removed" << std::endl;
        //     return;
        // }

        int removeId = cID;
        Cell* cellToRemove = cells[cID];

        for (auto& n : cellToRemove->nets) {
            auto& netCells = n->cells;
            netCells.erase(std::remove(netCells.begin(), netCells.end(), cellToRemove), netCells.end());
            --n->numCell; 
        }
        // delete cells[removeId];
        cells[removeId] = nullptr;

        --numCell;
    }
    void resetGraph() {
        boost::container::vector<Cell*> tempCells;
        for (Cell* c : cells) {
            if (c != nullptr) {tempCells.push_back(c);}
        }
        cells = tempCells;

        boost::container::vector<Net*> tempNets;
        for (Net* n : nets) {
            if (n->cells.size() > 0) {tempNets.push_back(n);} 
            // else {delete n;}
        }
        nets = tempNets;

        numCell = cells.size();
        numNet = nets.size();

        cellID.clear();
        for (int i = 0; i < (int)cells.size(); ++i) {
            cells[i]->id = i; 
            cellID[cells[i]->name] = i;
        }

        for (int i = 0; i < (int)nets.size(); ++i) {
            nets[i]->id = i;
        }

        for(auto& c: cells){
            c->nets.clear();
        }
        setCellArray();
    }
    void readFile(std::string InputFile){
        std::ifstream inFile(InputFile);
        if (!inFile) {
            std::cerr << "Error opening input file!" << std::endl;
            exit(-1);
        }

        std::string s;

        //read cell info
        inFile>>s>>numCell;
        inFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        cells.reserve(numCell);
        for(int id = 0 ; id < numCell; ++id){
            std::string name;
            int size;
            inFile>>s>>name>>size;
            Cell* tempCell = new Cell(name, size, id);
            cells.push_back(tempCell);
            inFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cellID.insert(std::make_pair(name, id));
            // std::cout<<id<<": "<<name<<" "<<size<<std::endl;
        }


        //read net info
        inFile>>s>>numNet;
        inFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        nets.reserve(numNet);
        for(int nid = 0 ; nid < numNet; ++nid){
            std::string netName;
            int netSize;
            boost::container::vector<Cell*> netCells;
            inFile>>s>>netName>>netSize;
            inFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            //read cell on the net
            for(int _ = 0; _ < netSize; ++_){
                std::string name;
                inFile>>s>>name;
                inFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                netCells.push_back(cells[cellID[name]]);
            }
            Net* tempNet = new Net(netName, netSize, nid, netCells);
            // nets[nid] = tempNet;
            nets.push_back(tempNet);
        }

        // for(auto& n: nets){
        //     for(auto& c: n.cells){
        //         std::cout<<c.id<<" "<<c.size<<std::endl;
        //     }
        // }
        inFile.close(); 
    }
    void setCellArray(){
        for(auto& net: nets){
            for(auto& cellPtr: net->cells){
                cellPtr->nets.push_back(net);
            }
        }
    }
};


#endif