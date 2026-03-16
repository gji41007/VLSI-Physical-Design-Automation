#include "Lef.h"

#include <regex>
#include <sstream>
static const auto io_speedup = [](){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    return 0;
}();
void LEF::parseLEF(std::string lefFile){
    std::ifstream inFile(lefFile);
    if (!inFile.is_open()) {
        std::cerr << "Error: Cannot open file " << lefFile << std::endl;
        return;
    }

    // std::stringstream buffer;
    // buffer << inFile.rdbuf();
    // std::string rawCode = buffer.str();
    std::string s;
    while(inFile >> s){
        if(s == "UNITS"){break;}
    }
    inFile>>s>>s;
    inFile>>unit;
    //read site
    // std::stringstream buffer;
    // buffer << inFile.rdbuf();
    // std::string rawFile = buffer.str();


    int sid = 0;
    int cid = 0;
    while(inFile >> s){
        if(s == "MACRO"){//read first macro
            std::string siteStr; 
            StdCell* cell = new StdCell();
            inFile>>cell->name;
            // std::cout<<cell->name<<std::endl;
            std::getline(inFile, s);
            inFile>>s>>cell->cellClass;
            if(cell->cellClass == "CORE"){
                std::getline(inFile, s);
                std::getline(inFile, s);//FOREIGN
                inFile>>s>>cell->origiX>>cell->origiY;
                std::getline(inFile, s);
                inFile>>s>>cell->cellWidth>>s>>cell->cellHeight;
                std::getline(inFile, s);
                std::getline(inFile, s);//SYMMETRY
                inFile>>s>>siteStr;
                cell->site = sites[siteId[siteStr]];
            }
            else {
                std::getline(inFile, s);
                inFile>>s>>cell->origiX>>cell->origiY;
                std::getline(inFile, s);
                std::getline(inFile, s);//FOREIGN
                inFile>>s>>cell->cellWidth>>s>>cell->cellHeight;
                std::getline(inFile, s);
                std::getline(inFile, s);//SYMMETRY
                cell->site = nullptr;
            }
            stdCells.push_back(cell);
            cellId[cell->name] = cid++;
            break;
        }
        else if(s != "SITE"){continue;}
        // if(sid==1)break;
        std::string siteName;
        inFile>>siteName;
        // std::cout<<siteName<<std::endl;

        Site* site = new Site(siteName);
        siteId[siteName] = sid++;
        sites.push_back(site);

        std::string kw;
        while(inFile>>kw){
            if(kw == "SYMMTRY"){
                std::getline(inFile, s);
            }
            else if(kw == "CLASS"){
                inFile>>site->siteClass>>s;
            }
            else if(kw == "SIZE"){
                inFile>>site->siteWidth>>s>>site->siteHeight;
            }
            else if (kw == "END"){
                break;
            }
        } 
    }



    //read cell
    while(inFile>>s){
        if(s != "MACRO"){continue;}
        std::string siteStr; 
        StdCell* cell = new StdCell();
        inFile>>cell->name;
        // std::cout<<cell->name<<std::endl;
        std::getline(inFile, s);
        inFile>>s>>cell->cellClass;
        if(cell->cellClass == "CORE"){
            std::getline(inFile, s);
            std::getline(inFile, s);//FOREIGN
            inFile>>s>>cell->origiX>>cell->origiY;
            std::getline(inFile, s);
            inFile>>s>>cell->cellWidth>>s>>cell->cellHeight;
            std::getline(inFile, s);
            std::getline(inFile, s);//SYMMETRY
            inFile>>s>>siteStr;
            cell->site = sites[siteId[siteStr]];
        }
        else {
            std::getline(inFile, s);
            inFile>>s>>cell->origiX>>cell->origiY;
            std::getline(inFile, s);
            std::getline(inFile, s);//FOREIGN
            inFile>>s>>cell->cellWidth>>s>>cell->cellHeight;
            std::getline(inFile, s);
            std::getline(inFile, s);//SYMMETRY
            cell->site = nullptr;
        }
        stdCells.push_back(cell);
        cellId[cell->name] = cid++;
    }
    // std::cout<<sites.size()<<std::endl;
    // std::cout<<stdCells.size()<<std::endl;

    // for(auto p: stdCells){
    //     std::cout<<p->name<<std::endl;
    // }
    // std::cout<<"pass"<<std::endl;
    // std::cout<<sites.size()<<" "<<stdCells.size()<<std::endl;
    // dumpSite();
    // dumpCell();
}