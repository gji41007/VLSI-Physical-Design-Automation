#include "DR.h"


void DetailRouter::run(){
    lef.parseLEF(lefPath);
    
    def.parseDEF(defPath);
    def.init(lef);
    def.dumpDEF(defPath, outputPath);
}