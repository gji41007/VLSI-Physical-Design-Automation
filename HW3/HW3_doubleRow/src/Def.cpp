#include "Def.h"

#include <sstream>
#include <algorithm>
#include <set>
#include <assert.h>
static const auto io_speedup = [](){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    return 0;
}();
void DEF::parseDEF(std::string defPath){
    std::ifstream inFile(defPath);
    if (!inFile.is_open()) {
        std::cerr << "Error: Cannot open file " << defPath << std::endl;
        return;
    }
    //read unit
    std::string s;
    while(inFile >> s){
        if(s == "UNITS"){break;}
    }
    inFile>>s>>s;
    inFile>>unit;
    //read basic info
    while(s != "DIEAREA"){
        inFile>>s;
    }
    inFile>>s>>x1>>y1>>s>>s>>x2>>y2;
    std::getline(inFile, s);
    // std::cout<<x1<<x2<<y1<<y2<<s;

    //read row
    while(inFile>>s){
        if(s != "ROW"){break;}
        Row* row = new Row();
        inFile>>row->name>>row->siteStr
              >>row->origiX>>row->origiY
              >>row->FS>>s>>row->numHori
              >>s>>row->numVert>>s>>row->stepX>>row->stepY>>s;
        rows.push_back(row);

    }


    //read component
    while(s != "COMPONENTS"){
        inFile>>s;
    }
    getline(inFile, s);
    
    // std::cout<<"pass"<<std::endl;
    while(inFile>>s){
        if(s != "-"){break;}
        Component* comp = new Component();
        std::string fixStr;
        inFile>>comp->name>>comp->cellStr>>s>>fixStr>>s
              >>comp->x>>comp->y>>s>>comp->orientation>>s;
        comp->isFixed = (fixStr == "FIXED"); 
        comps.push_back(comp);
        compId[comp->name] = comps.size() - 1;
    }


    //read pin
    while(s != "PINS"){
        inFile>>s;
    }
    std::getline(inFile, s);
    while(inFile>>s){
        if(s != "-"){break;}
        Pin* pin = new Pin();
        std::string fixStr;
        inFile>>pin->name>>s>>s>>pin->netStr>>s;
        std::getline(inFile, s);
        inFile>>s>>s>>s>>s>>pin->x1>>y1>>s>>s>>x2>>y2>>s
              >>s>>fixStr>>s>>pin->x>>pin->y>>s>>pin->orientation>>s;
        pins.push_back(pin);
        pinId[pin->name] = pins.size() - 1;
    }

    //read net
    while(s != "NETS"){
        inFile>>s;
    }
    std::getline(inFile, s);
    while(inFile>>s){
        if(s != "-"){break;}
        Net* net = new Net();
        inFile>>net->name;
        // std::cout<<net->name<<std::endl;
        while(inFile>>s){
            if(s == ";"){break;}
            std::string kw, tempStr;
            inFile>>kw;
            if(kw == "PIN"){
                inFile>>tempStr;
                net->pinStr.push_back(tempStr);
            }
            else{
                inFile>>tempStr;
                net->compStr.push_back(std::make_pair(kw, tempStr));
            }
            inFile>>s;
        }
        nets.push_back(net);
        netId[net->name] = nets.size() - 1;
    }
}

void DEF::init(LEF lef){
    for(auto row: rows){
        row->site = lef.sites[lef.siteId[row->siteStr]];
        row->siteComp = std::vector<Component*> (row->numHori, nullptr);
    }
    for(auto pin: pins){
        if(netId.find(pin->netStr) == netId.end()){
            continue;
        }
        pin->net = nets[netId[pin->netStr]];
    }
    for(auto comp: comps){
        // assert(lef.cellId.find(comp->cellStr) != lef.cellId.end());
        // assert(lef.cellId[comp->cellStr] < lef.stdCells.size());
        comp->cell = lef.stdCells[lef.cellId[comp->cellStr]];
    }
    for(auto net: nets){
        for(auto pStr: net->pinStr){
            net->pins.push_back(pins[pinId[pStr]]);
        }
        for(auto cStr: net->compStr){
            net->comps.push_back(comps[compId[cStr.first]]);
            comps[compId[cStr.first]]->nets.push_back(net);
        }
    }
    //init row->siteComp
    for(auto comp: comps){
        for(auto row: rows){
            int x1 = row->origiX, y1 = row->origiY;
            int x2 = x1 + row->numHori*row->stepX;//lef.unit*row->numHori*row->site->siteWidth;
            int y2 = y1 + row->numVert*row->stepY;//lef.unit*row->numVert*row->site->siteHeight;
            int compx2 = comp->x + lef.unit*comp->cell->cellWidth;
            int compy2 = comp->y + lef.unit*comp->cell->cellHeight;
            // if(x1<= comp->x && comp->x <= x2 && y1<= comp->y && comp->y <= y2){
            if(std::min(x2, compx2) > std::max(x1, comp->x) &&
               comp->y <= y1 && y1< compy2){
                // std::cout<<row->name<<std::endl;
                // std::cout<<x1<<" "<<x2<<" "<<y1<<" "<<y2<<std::endl;
                // std::cout<<comp->x<<" "<<comp->y<<std::endl;
                // std::cout<<comp->site->siteWidth<<" "<<comp->site->siteHeight<<std::endl;
                int left = (comp->x - x1)/row->stepX;
                int right = (comp->x + lef.unit*comp->cell->cellWidth - x1 - 1)/row->stepX;
                for(int siteId = left; siteId <= right; ++siteId){
                    assert(row->siteComp[siteId] == nullptr);
                    row->siteComp[siteId] = comp;
                    comp->left = left;
                    comp->right = right;
                    comp->size = right - left + 1;
                }
                if(y1 == comp->y){
                    comp->row = row;
                }
                // std::cout<<left<<" "<<right<<std::endl;
                // exit(-1);
            }
        }
    }


    long long int totalWl = 0;
    for(auto& n:nets){
        totalWl += n->HPWL();
    }
    std::cout<<"Init totalWl ="<<totalWl<<std::endl;
    std::cout<<lef.sites.size()<<std::endl;
}

void DEF::dumpDEF(std::string defPath, std::string outputPath){
    std::ifstream inFile(defPath);
    if (!inFile.is_open()) {
        std::cerr << "Error: Cannot open file " << defPath << std::endl;
        return;
    }
    std::ofstream outFile(outputPath);
    std::string line;
    //read and output until COMPONENTS
    while (std::getline(inFile, line)) {
        outFile<<line<<'\n';
        std::istringstream iss(line);
        std::string firstWord;
        iss >> firstWord;     
        if (firstWord == "COMPONENTS") {
            break;
        }
    }
    //output component
    std::string s;
    while(inFile >> s){
        if(s == "END"){
            outFile<<"END COMPONENTS\n";
            std::getline(inFile, line);
            break;
        }
        std::string compName;
        inFile>>compName;
        // std::cout<<compName<<std::endl;
        outFile<<*(comps[compId[compName]])<<'\n';
        std::getline(inFile, line);
        std::getline(inFile, line);
    }
    //read and output remaining
    while (std::getline(inFile, line)) {
        outFile<<line<<'\n';
    }
    
    // std::cout<<rows.size()<<std::endl;
    // std::cout<<comps.size()<<std::endl;
    // std::cout<<pins.size()<<std::endl;
    // std::cout<<nets.size()<<std::endl;
    // std::cout<<comps[compId["bitD06a"]]->cellStr<<" "<<comps[compId["bitD06a"]]->cell->name<<" "<<comps[compId["bitD06a"]]->size<<" "<<comps[compId["bitD06a"]]->cell->cellWidth<<std::endl;
    // std::cout<<comps[compId["bitD06a"]]->row->name<<" "<<comps[compId["bitD06a"]]->row->site->siteHeight<<" "<<comps[compId["bitD06a"]]->cell->cellHeight<<std::endl;
    // for(auto row: rows){
    //     bool flag = false;
    //     for(int i = 0; i < row->siteComp.size(); ++i){
    //         if(row->siteComp[i] != nullptr && row->siteComp[i]->name == "bitD06a"){
    //             flag = true;
    //         }
    //     }
    //     if (flag){
    //         std::cout<<row->name<<": ";
    //         for(int i = 0; i < row->siteComp.size(); ++i){
    //             if(row->siteComp[i] != nullptr && row->siteComp[i]->name == "bitD06a"){
    //                 std::cout<<row->origiX+row->stepX*i<<" ";
    //             }
    //         }
    //         std::cout<<std::endl;

    //     }
    // }
}

int Net::HPWL(){
    //all pin location in net
    std::vector<std::pair<int, int>> indices;
    for(auto& p: pins){
        indices.push_back(std::make_pair(p->x, p->y));
    }
    for(auto& c: comps){
        indices.push_back(std::make_pair(c->x, c->y));
    }


    int x1 = INT32_MAX, x2 = -1, y1 = INT32_MAX, y2 = -1;
    for(auto p:indices){
        x1 = std::min(x1, p.first);
        x2 = std::max(x2, p.first);
        y1 = std::min(y1, p.second);
        y2 = std::max(y2, p.second);
    }
    return x2 - x1 + y2 - y1;
}
long long int DEF::totalHPWL(){
    long long int totalWl = 0;
    for(auto net:nets){
        totalWl += net->HPWL();
    }
    return totalWl;
}
std::vector<std::vector<Component*>> getPermutation(const std::vector<Component*>& candidates) {
    std::vector<std::vector<Component*>> result;
    result.reserve(24);

    Component* a = candidates[0];
    Component* b = candidates[1];
    Component* c = candidates[2];
    Component* d = candidates[3];

    result.push_back(std::vector<Component*>{a, b, c, d});
    result.push_back(std::vector<Component*>{a, b, d, c});
    result.push_back(std::vector<Component*>{a, c, b, d});
    result.push_back(std::vector<Component*>{a, c, d, b});
    result.push_back(std::vector<Component*>{a, d, b, c});
    result.push_back(std::vector<Component*>{a, d, c, b});
    
    result.push_back(std::vector<Component*>{b, a, c, d});
    result.push_back(std::vector<Component*>{b, a, d, c});
    result.push_back(std::vector<Component*>{b, c, a, d});
    result.push_back(std::vector<Component*>{b, c, d, a});
    result.push_back(std::vector<Component*>{b, d, a, c});
    result.push_back(std::vector<Component*>{b, d, c, a});
    
    result.push_back(std::vector<Component*>{c, a, b, d});
    result.push_back(std::vector<Component*>{c, a, d, b});
    result.push_back(std::vector<Component*>{c, b, a, d});
    result.push_back(std::vector<Component*>{c, b, d, a});
    result.push_back(std::vector<Component*>{c, d, a, b});
    result.push_back(std::vector<Component*>{c, d, b, a});
    
    result.push_back(std::vector<Component*>{d, a, b, c});
    result.push_back(std::vector<Component*>{d, a, c, b});
    result.push_back(std::vector<Component*>{d, b, a, c});
    result.push_back(std::vector<Component*>{d, b, c, a});
    result.push_back(std::vector<Component*>{d, c, a, b});
    result.push_back(std::vector<Component*>{d, c, b, a});

    return result;
}
int getNetsHPWL(std::set<Net*> nets){
    int wl = 0;
    for(auto net: nets){
        wl += net->HPWL();
    }
    return wl;
}

void Component::resetRange(){
    left = (x - row->origiX)/row->stepX;
    right = left + size - 1;
}


int Row::permutation(std::vector<Component*> candidates, int left, int right){
    std::vector<std::vector<Component*>> permutes = getPermutation(candidates);
    std::set<Net*> connectNet;
    int totalSize = 0;
    for(auto cand: candidates){totalSize += cand->size;}
    int blankSize = (right - left + 1 - totalSize)/(WINDOWSIZE-1);

    assert(totalSize <= right - left + 1);
    assert(blankSize*3 + totalSize <= right-  left + 1);
    // if((blankSize*3 + totalSize > right-  left + 1)){
    //     std::cout<<right - left + 1<<" "<<totalSize<<" "<<blankSize<<std::endl;
    // }
    for(auto cand: candidates){
        for(auto net: cand->nets){
            connectNet.insert(net);
        }
    }

    //start compute permutation hpwl
    int minWl = getNetsHPWL(connectNet);
    // std::cout<<minWl<<std::endl<<"-----"<<std::endl;
    std::vector<std::pair<Component*, int>> minConfig;
    for(auto comp: candidates){minConfig.push_back(std::make_pair(comp, comp->x));}
    for(auto permutation: permutes){
        // int currleft = left;
        int currX = origiX + left * stepX;
        for(int i = 0; i < (int)permutation.size(); ++i){
            permutation[i]->x = currX;
            currX += (permutation[i]->size + blankSize)*stepX;
            // permutation[i]->left = currleft;
            // permutation[i]->right = currleft + permutation[i]->size - 1;
            // currleft = permutation[i]->right + blankSize + 1;
        }
        // std::cout<<getNetsHPWL(connectNet)<<std::endl;
        if(getNetsHPWL(connectNet) < minWl){
            minWl = getNetsHPWL(connectNet);
            minConfig.clear();
            // std::cout<<"seccess"<<std::endl;
            for(auto comp: permutation){minConfig.push_back(std::make_pair(comp, comp->x));}
        }
    }
    //reset config
    for(int i = left; i <= right; ++i){siteComp[i] = nullptr;}
    for(auto cfg: minConfig){
        cfg.first->x = cfg.second;
    }
    for(auto comp: candidates){
        comp->resetRange();
        for(int i = comp->left; i <= comp->right; ++i){
            comp->row->siteComp[i] = comp;
        }
    }


    
    //return the end(right) of the first comp
    int tid = left;
    while(siteComp[tid] == nullptr){++tid;}
    return siteComp[tid]->right + 1;
}

int Row::rearrangeDP(std::vector<Component*> candidates, int left, int right){
    int totalSize = 0;
    for(auto cand: candidates){totalSize += cand->size;}
    
    int currId = left;
    while(siteComp[currId] == nullptr){++currId;}
    Component* firstComp = siteComp[currId];
    int reserveSpace = totalSize - firstComp->size;

    std::set<Net*> connectNets;
    for(auto n: firstComp->nets){connectNets.insert(n);}


    int minX, minHpwl;
    for(int currId = left; currId + firstComp->size - 1<= right-reserveSpace; ++currId){
        firstComp = 
    }


}

