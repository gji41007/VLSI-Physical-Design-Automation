#include "DP.h"


int main(int argc, char* argv[]) {
    // Check command line arguments
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <input.lef> <input.def> <output.def>" << std::endl;
        return 1;
    }

    std::string lefFile = argv[1];
    std::string defFile = argv[2];
    std::string outputFile = argv[3];

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    DetailPlacer DP_Manager(lefFile, defFile, outputFile);
    DP_Manager.run();
}