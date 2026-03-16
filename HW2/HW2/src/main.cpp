#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <limits>
#include <algorithm>
#include <map>
#include "graph.h"
#include "partition.h"

using namespace std;

int main(int argc, char* argv[]) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if (argc != 4) {
        std::cerr << "Usage: ./hw2 [input_file] [output_file] [number_of_partition]" << std::endl;
        exit(-1);
    }

    string InputFile = argv[1];
    string OutputFile = argv[2];
    int numPart = stoi(argv[3]);
    if (numPart != 2 && numPart != 4) {
        std::cerr << "Usage: ./hw2 [input_file] [output_file] [number_of_partition]" << std::endl;
        std::cerr << "[number_of_partition] must be 2 or 4" << std::endl;
        exit(-1);
    }
    



    Graph graph;
    graph.readFile(InputFile);
    graph.setCellArray();

    if(numPart == 2){
        Partition2 partition(graph);
        partition.initPartition();
        partition.initGain();
        // partition.dumpBucket();
        partition.run();
        partition.dumpPartition(OutputFile);
    }
    else{
        Partition4 partition4(graph);
        partition4.run2();
        partition4.dumpPartition(OutputFile);

    }
    

    // sort(netlist.begin(), netlist.end(), 
    //     [](const std::vector<int>& a, const std::vector<int>& b) {
    //         return a.size() > b.size(); 
    //     });

    
    


    return 0; 
}