#include "DR.h"

#include <assert.h>

static const auto io_speedup = [](){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    return 0;
}();
void DetailRouter::run(){
    lef.parseLEF(lefPath);
    
    def.parseDEF(defPath);
    def.init(lef);
    // std::cout<<"init graph"<<std::endl;
    // graph.init(def);

    std::cout<<"Before: "<<def.totalHPWL()<<std::endl;
    rowReorder(WINDOWSIZE);
    rowReorder(WINDOWSIZE);
    // rowReorder(WINDOWSIZE);
    std::cout<<"After: "<<def.totalHPWL()<<std::endl;

    def.dumpDEF(defPath, outputPath);
}


void DetailRouter::rowReorder(int windowSize){
    for(auto row: def.rows){
        // if(row->name!="CORE_ROW_708"){continue;}
        // int count = 0;
        // for(auto& comp: row->siteComp){if(comp&&comp->name == "bitD06a")++count;}
        // std::cout<<count<<" bitd06a"<<std::endl;
        std::vector<Component*> candidates;
        int start = 0, end;
        int siteId = 0;
        // for(int siteId = 0; siteId < row->numHori; ++siteId){
        while(siteId < row->numHori){
            if(row->siteComp[siteId] == nullptr){++siteId;continue;}
            else if(row != row->siteComp[siteId]->row){
                ++siteId;
                start = siteId;
                // while(siteId < row->numHori && row->siteComp[siteId]->row == row->siteComp[siteId-1]->row){
                //     ++siteId;
                // }
                candidates.clear();
                continue;
            }
            else if(row->siteComp[siteId]->isFixed || row->siteComp[siteId]->cell->cellHeight*lef.unit > row->site->siteHeight*lef.unit){
                // std::cout<<row->siteComp[siteId]->cell->cellHeight*lef.unit <<" "<< row->site->siteHeight <<" "<<row->stepY*lef.unit<<std::endl;
                siteId = row->siteComp[siteId]->right + 1;
                start = siteId;
                candidates.clear();
                continue;
            }
            Component* comp = row->siteComp[siteId];
            candidates.push_back(comp);
            siteId = candidates.back()->right + 1;
            if((int)candidates.size() == WINDOWSIZE){
                end = candidates.back()->right;
                //permutation
                // std::cout<<"start permute: "<<start<<" "<<end<<std::endl;
                // for(auto c: candidates){assert(c->name!="bitD06a");}
                // for(auto c: candidates){if(c->name=="inst772514")std::cout<<start<<" "<<end<<std::endl;}
                start = row->permutation(candidates, start, end);
                // std::cout<<"finish permute"<<std::endl;
                //dp

                candidates.clear();
                siteId = start;
            }
        }
    }
}