#include "DP.h"


#include "Def.h"

#include <assert.h>
#include <algorithm>
#include <set> 
static const auto io_speedup = [](){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    return 0;
}();
void DetailPlacer::run(){
    lef.parseLEF(lefPath);
    def.parseDEF(defPath);
    def.init(lef);

    // std::cout<<"Before: "<<def.totalHPWL()<<std::endl;
    // rowReorder(1);//sliding step = 2
    // GlobalInsert();
    rowReorder(1);//sliding step = 2
    GlobalInsert();
    rowReorder(1);//sliding step = 1
    singleCellAdjust();
    // std::cout<<"After: "<<def.totalHPWL()<<std::endl;

    def.dumpDEF(defPath, outputPath);
}


void DetailPlacer::rowReorder(int windowSize){
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
                // start = row->permutation(candidates, start, end);
                candidates = row->permutation(candidates, start, end);
                // start = candidates[0]->right+1;
                // std::cout<<"finish permute"<<std::endl;
                //dp

                // std::cout<<"start rearrange"<<std::endl;
                // row->rearrangeDP(candidates, start, end);
                start = candidates[windowSize-1]->right+1;
                // assert(t_start >=start && t_start < end);
                // std::cout<<"finish rearrange"<<std::endl;
                // candidates.erase(candidates.begin());
                for(int i = 0; i < windowSize; ++i){
                    candidates.erase(candidates.begin());

                }
                // candidates.clear();
                siteId = candidates.back()->right+1;
            }
        }
    }
}
// int getNetsHPWL(std::set<Net*> nets){
//     int wl = 0;
//     for(auto net: nets){
//         wl += net->HPWL();
//     }
//     return wl;
// }
void DetailPlacer::singleCellAdjust(){
    for(auto comp: def.comps){
        // assert(comp->cell != nullptr);
        if(comp->isFixed){continue;}
        if(comp->cell->cellHeight > comp->row->site->siteHeight){continue;}
        std::set<Net*> connectNet;
        for(auto n: comp->nets){connectNet.insert(n);}
        

        int minWl = getNetsHPWL(connectNet);
        assert(comp->left <= comp->right);
        int l = comp->left;
        while(l > 0 &&comp->row->siteComp[l-1] == nullptr){
            
            comp->x -= comp->row->stepX;
            if(getNetsHPWL(connectNet) < minWl){
                --l;
                minWl = getNetsHPWL(connectNet);
            }
            else{
                comp->x += comp->row->stepX;
                break;
            }
        }
        int r = comp->right;
        while(r < (int)comp->row->siteComp.size() - 1 && comp->row->siteComp[r+1] == nullptr){
            comp->x += comp->row->stepX;
            if(getNetsHPWL(connectNet) < minWl){
                ++r;
                minWl = getNetsHPWL(connectNet);
            }
            else{
                comp->x -= comp->row->stepX;
                break;
            }
        }
        for(int i = comp->left; i <= comp->right; ++i){comp->row->siteComp[i] = nullptr;}
        comp->resetRange();
        for(int i = comp->left; i <= comp->right; ++i){
            assert(comp->row->siteComp[i]==nullptr);
            comp->row->siteComp[i] = comp;
        }
    }
}


void DetailPlacer::GlobalInsert(){
    for(auto comp: def.comps){
        if(comp->isFixed || comp->cell->cellHeight > comp->row->site->siteHeight){continue;}
        std::vector<int> Xs, Ys;
        for(auto net: comp->nets){
            std::vector<std::pair<int, int>> indices;
            for(auto& p: net->pins){
                indices.push_back(std::make_pair(p->x, p->y));
            }
            for(auto& c: net->comps){
                indices.push_back(std::make_pair(c->x, c->y));
            }
            int x1 = INT32_MAX, x2 = -1, y1 = INT32_MAX, y2 = -1;
            for(auto p:indices){
                x1 = std::min(x1, p.first);
                x2 = std::max(x2, p.first);
                y1 = std::min(y1, p.second);
                y2 = std::max(y2, p.second);
            }
            Xs.push_back(x1);
            Xs.push_back(x2);
            Ys.push_back(y1);
            Ys.push_back(y2);
        }

        sort(Xs.begin(), Xs.end());
        sort(Ys.begin(), Ys.end());
        // std::cout<<(int)Xs.size()/2 - 1<<" "<<Xs.size()<<std::endl;
        int optx1 = Xs[Xs.size()/2 - 1];
        int optx2 = Xs[Xs.size()/2];
        int opty1 = Ys[Xs.size()/2 - 1];
        int opty2 = Ys[Xs.size()/2];

        // std::cout<<comp->x<<" "<<comp->y<<std::endl;
        // std::cout<<x1<<" "<<x2<<" "<<y1<<" "<<y2<<std::endl;
        // std::cout<<optx1<<" "<<optx2<<" "<<opty1<<" "<<opty2<<std::endl;


        bool isSwap = false;
        std::vector<Row*>optRows;
        for(auto row: def.rows){
            if(opty1 <= row->origiY && row->origiY <= opty2){
                // std::cout<<"try insert"<<std::endl;
                isSwap = row->insertComp(comp, optx1, optx2);
                optRows.push_back(row);
                // std::cout<<"finish insert"<<std::endl;
            }
            if(isSwap){
                for(int i = comp->left; i <= comp->right; ++i){comp->row->siteComp[i] = nullptr;}
                
                comp->y = row->origiY;
                if(comp->orientation != row->orientation){comp->orientation = row->orientation;}
                for(int i = comp->left; i <= comp->right; ++i){
                    comp->row->siteComp[i] = nullptr;
                }
                comp->resetRange();
                comp->row = row;
                break;
            }
        }
        if(!isSwap){
            int maxSize = comp->size;
            int currId = comp->right+1;
            while(currId < (int)comp->row->siteComp.size() &&comp->row->siteComp[currId] ==nullptr){
                ++currId;
                ++maxSize;
            }

            for(auto row: optRows){
                if(row->swapComp(comp, optx1, optx2, maxSize)){
                    comp->resetRange();
                    if(comp->orientation != comp->row->orientation){comp->orientation = row->orientation;}
                    break;
                }
            }
        }
    }



  
}
