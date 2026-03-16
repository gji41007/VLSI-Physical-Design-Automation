#include "GR.h"



int main(int argc, char* argv[]){
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input.txt> <output.out>" << std::endl;
        return 1;
    }

    std::string inpuPath = argv[1];
    std::string outputPath = argv[2];

    GlobalRouter GR_manager(inpuPath, outputPath);
    GR_manager.run();
}