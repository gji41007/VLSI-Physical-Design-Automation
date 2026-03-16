#include "Def.h"

#include <sstream>
#include <algorithm>
#include <set>
#include <assert.h>
#include <random>
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
              >>row->orientation>>s>>row->numHori
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
                int right = (comp->x + lef.unit*comp->cell->cellWidth - x1-  1)/row->stepX;
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
    // std::cout<<"Init totalWl ="<<totalWl<<std::endl;
    // std::cout<<lef.sites.size()<<std::endl;
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


std::vector<Component*> Row::permutation(std::vector<Component*> candidates, int left, int right){
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
    std::vector<Component*> minPermutation = candidates;
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
            minPermutation = permutation;
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


    return minPermutation;
}

int Row::rearrangeDP(std::vector<Component*> candidates, int left, int right){
    int totalSize = 0;
    for(auto cand: candidates){totalSize += cand->size;}
    // std::cout<<totalSize<<" "<<right<<" "<<left<<std::endl; 
    assert(totalSize <= right - left + 1);
    for(int i = 1; i < (int)candidates.size(); ++i){assert(candidates[i-1]->x<candidates[i]->x );}
    Component* firstComp = candidates.front();//siteComp[currId];
    int reserveSpace = totalSize - firstComp->size;
    // assert(firstComp->size + reserveSpace <= right - left + 1);

    std::set<Net*> connectNet, allNets;
    for(auto n: firstComp->nets){connectNet.insert(n);}
    for(auto cand: candidates){
        for(auto net: cand->nets){
            allNets.insert(net);
        }
    }
    

    int minX = firstComp->x, minWl = getNetsHPWL(connectNet);
    for(int currId = left; currId + firstComp->size - 1 < candidates[1]->left; ++currId){
        firstComp->x = origiX + currId * stepX;
        if(getNetsHPWL(connectNet) < minWl){
            minWl = getNetsHPWL(connectNet);
            minX = firstComp->x;
        }
    }
    firstComp->x = minX;
    for(int i = firstComp->left; i <= firstComp->right; ++i){siteComp[i] = nullptr;}
    firstComp->resetRange();
    assert(firstComp->row->name == name);
    for(int i = firstComp->left; i <= firstComp->right; ++i){
        siteComp[i] = firstComp;
    }
    // std::cout<<left<<" "<<minX<<" "<<right<<" "<<candidates.front()->left<<std::endl;
    for(int i = 1; i < (int)candidates.size(); ++i){assert(candidates[i]->left > candidates[i-1]->right);}
    assert(candidates.back()->right <= right);
    assert(candidates.front()->left >= left);
    // int minX = firstComp->x, minWl = getNetsHPWL(allNets);
    // for(int currId = left; currId + firstComp->size - 1 <= right-reserveSpace; ++currId){
    //     firstComp->x = origiX + currId * stepX;
    //     for(int i = 1; i < (int)candidates.size(); ++i){
    //     if(candidates[i]->x <= candidates[i-1]->x+ candidates[i-1]->size*stepX)
    //             candidates[i]->x = candidates[i-1]->x + candidates[i-1]->size*stepX;
    //     }
    //     if(getNetsHPWL(allNets) < minWl){
    //         minWl = getNetsHPWL(allNets);
    //         minX = firstComp->x;
    //     }
    // }
    // firstComp->x = minX;
    // for(int i = 1; i < (int)candidates.size(); ++i){
    //     if(candidates[i]->x <= candidates[i-1]->x+ candidates[i-1]->size*stepX)
    //         candidates[i]->x = candidates[i-1]->x + candidates[i-1]->size*stepX;
    // }

    // for(int i = left; i <= right; ++i){siteComp[i] = nullptr;}
    // for(auto comp: candidates){
    //     comp->resetRange();
    //     for(int i = comp->left; i <= comp->right; ++i){
    //         siteComp[i] = comp;
    //     }
    //     // std::cout<<comp->left<<" "<<comp->right<<" "<<comp->size<<std::endl;
    // }
    // // std::cout<<left<<" "<<candidates.front()->left<<" "<<right<<" "<<minX<<std::endl;
    // // std::cout<<"---"<<std::endl;
    // for(int i = 1; i < (int)candidates.size(); ++i){assert(candidates[i]->left > candidates[i-1]->right);}
    // assert(candidates.back()->right <= right);
    // assert(candidates.front()->left >= left);
    return firstComp->right+1;

}



bool Row::insertComp(Component* comp, int x1, int x2){
    if(x2 <= origiX || x1 > origiX + stepX * (numHori-1)){return false;}
    if(comp->cell->site != site){return false;}
    if(comp->cell->cellHeight > site->siteHeight){return false;}

    int left = (x1 - origiX)/stepX;
    int right = (x2 - origiX)/stepX;

    for(int currId = left; currId <= right; ++currId){
        if(currId + comp->size - 1 >= siteComp.size()){break;}
        bool isAvail = true;
        for(int s = 0; s < comp->size; ++s){
            if(siteComp[currId+s] != nullptr){
                isAvail = false;
                break;
            }
        }
        if(isAvail){
            for(int s = 0; s < comp->size; ++s){
                assert(siteComp[currId+s] == nullptr);
                siteComp[currId+s] = comp;
            }
            comp->x = origiX + stepX * currId;
            return true;
        }
    }
    return false;
}


bool Row::swapComp(Component* comp, int x1, int x2, int maxSize){
    if(x2 <= origiX || x1 > origiX + stepX * (numHori-1)){return false;}
    if(comp->cell->site != site){return false;}
    if(comp->cell->cellHeight > site->siteHeight){return false;}

    int left = (x1 - origiX)/stepX;
    int right = (x2 - origiX)/stepX;

    int currId = left;
    int currStart = left;
    int frontSpace = 0, compSize = 0, rearSpace = 0;
    // while(currId <= right && siteComp[currId] == nullptr){
    //     ++currId;
    //     ++frontSpace;
    // }

    while(currId <= right){
        if(currId + comp->size - 1 >= siteComp.size()){break;}
        if(siteComp[currId] == nullptr){++currId; continue;}
        Component* swapCandidate = siteComp[currId];
        int totalSpace = swapCandidate->size;
        currId = siteComp[currId]->right+1;
        while(currId < siteComp.size() && siteComp[currId] == nullptr){
            ++totalSpace;
            ++currId;
        }
        assert(totalSpace == currId - swapCandidate->left);
        bool isAvail = true;
        if(totalSpace < comp->size || swapCandidate->size > maxSize || comp == swapCandidate){continue;}
        // std::cout<<comp->name<<" "<<swapCandidate->name<<std::endl;

        std::set<Net*> connectNet;
        for(auto n: comp->nets){connectNet.insert(n);}
        for(auto n: swapCandidate->nets){connectNet.insert(n);}
        int before = getNetsHPWL(connectNet);
        std::swap(swapCandidate->x, comp->x);
        std::swap(swapCandidate->y, comp->y);
        if(getNetsHPWL(connectNet) > before){
            std::swap(swapCandidate->x, comp->x);
            std::swap(swapCandidate->y, comp->y);
            continue;
        }


        for(int sid = swapCandidate->left; sid <= swapCandidate->right; ++sid){
            siteComp[sid] = nullptr;
        }
        for(int sid = comp->left; sid <= comp->right; ++sid){
            comp->row->siteComp[sid] = nullptr;
        }
        for(int s = 0; s < comp->size; ++s){
            assert(siteComp[swapCandidate->left+s] != swapCandidate);
            assert(siteComp[swapCandidate->left+s] == nullptr);
            siteComp[swapCandidate->left+s] = comp;
        }
        for(int s = 0; s < swapCandidate->size; ++s){
            assert(comp->row->siteComp[comp->left+s] != comp);
            assert(comp->row->siteComp[comp->left+s] == nullptr);
            comp->row->siteComp[comp->left+s] = swapCandidate;
        }
        int record = comp->x, swaprecord = swapCandidate->x;
        // std::swap(swapCandidate->x, comp->x);
        // std::swap(swapCandidate->y, comp->y);
        std::swap(swapCandidate->row, comp->row);
        // swapCandidate->row = comp->row;
        // assert(comp->x == swaprecord);
        // assert(swapCandidate->x == record);
        swapCandidate->resetRange();
        if(swapCandidate->orientation != swapCandidate->row->orientation){swapCandidate->orientation = swapCandidate->row->orientation;}
        return true;
    }
    return false;
}