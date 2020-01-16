#include "stdIncludes.h"
#include <chrono>
#include "CLI11.hpp"
#include "whereami.h"
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <cstring>
#include <sys/param.h>
#include <mach-o/dyld.h>

#include <iostream>
#include "manipulators/TopologicalGenerator.h"
#include "manipulators/BSBuilder.h"
#include "manipulators/DFAConstructor.h"
#include "manipulators/LabeledBSBuilder.h"
#include "manipulators/DictionaryMerge.h"
#include "Utils/NetworkIO.h"
#include "Data/SimpleNetwork.h"
#include "Utils/File.h"
#include "Utils/Menu.h"
#include "Utils/Generate.h"
#include "Utils/SimpleNetworkIO.h"
#include "Utils/DictionaryIO.h"
#include "Utils/BehavioralSpaceIO.h"
#include "Utils/AbduceIO.h"
#include <string>
#include <set>
#include <list>
#include <ctype.h>
#include <chrono>
#include <ctime>

#include <filesystem>

#include "tinyfiledialogs.c"




void handleBS(long maxExecutionTime, const std::string &bsObservation, const std::string &bsBond, const std::string &bsOutObservation, bool prune, bool label,
              const std::string &inFile, const std::string &outFile);
void handleLBS(long maxExecutionTime, bool, const std::string &, const std::string &);
void handleDic(long maxExecutionTime, const std::string &, const std::string &,bool online);
void handleDicP(long maxExecutionTime, const std::string &, const std::string &, int lunghezzaPrefisso);
void handleMD(long maxExecutionTime, std::vector<std::string>& inFiles, const std::string &outFile);
void handleDiag(long maxExecutionTime, std::vector<std::string> &diagnosis, const std::string &inFile);
void handleAbd(long maxExecutionTime, std::vector<std::string> &observations, const std::string &inFile,const std::string &outFile);

void handleAbdO(long maxExecutionTime, std::vector<std::string> &observations, const std::string &inFile, const std::string &outFile);
void handleAbdS(long maxExecutionTime, const std::string &dicBond, const std::string &inFile, const std::string &outFile);

void handleLBSPrefix(long time, bool prune, const std::string &inFile, const std::string &outFile,int prefix);

std::vector<std::string> convertObsStringToVector(std::string stringa);

std::string ExePath();



int main(int argc, char *argv[]) {
    std::string bsObservation, bsBond, bsOutObservation,inFile, outFile,dicBond;
    bool observation = false,scenario = false;
   // bool prune, label,saveFile;
    long maxExecutionTime = 0;
    //int depth= 0;
    std::string temp1 ,temp2;
    std::vector<std::string> inFiles;
    std::vector<std::string> observations;

    std::cout<<"Working directory"<<std::endl;
    std::string cwd = std::filesystem::current_path();
    std::cout<<cwd+"\n"<<std::endl;
   
    
    
     /*char const * lTmp;
       char const * lTheSaveFileName;
       char const * lTheOpenFileName;
       char const * lTheSelectFolderName;
       char const * lTheHexColor;
       char const * lWillBeGraphicMode;
       unsigned char lRgbColor[3];
       FILE * lIn;
       char lBuffer[1024];
       char lThePassword[1024];
       char const * lFilterPatterns[2] = { "*.txt", "*.text" };

       lWillBeGraphicMode = tinyfd_inputBox("tinyfd_query", NULL, NULL);
       
       lTheSelectFolderName = tinyfd_selectFolderDialog(
           "let us just select a directory", NULL);

       if (!lTheSelectFolderName)
       {
           tinyfd_messageBox(
               "Error",
               "Select folder name is NULL",
               "ok",
               "error",
               1);
           return 1;
       }

       tinyfd_messageBox("The selected folder is",
           lTheSelectFolderName, "ok", "info", 1);

     
  */
           
    static int choice;
    do{
        
       
        //menu generale scelta offline o online
        choice=Menu::menuFirst();
        switch (choice) {
        case 1:{
            int choice2;

            do {
//MENU OFFLINE
                choice2=Menu::menuSecond();
                switch (choice2) {
                    case 1:{
                        
                        //VISUALIZZA NOMI CARTELLE DI PROGETTO OFFLINE ESISTENTI
                        auto directories=File::get_directories("offline");
                        Menu::MenuOfflineCaricamento(directories);
                        break;
                        }
                    
                    
                        //CARICA UNA NUOVA RETE MODALITA OFFLINE
                    case 2:{
                        
                        Menu::MenuCaricamentoOfflineNuovaRete(observation,scenario,maxExecutionTime);
                        break;
                    }
                        
                        //INDIETRO
                        case 3:{
                            break;
                           
                        }
                        default:{
                            std::cout << "Not a Valid Choice. \n"
                            << "Choose again.\n";
                                
                               
                        }
                        }

                    } while (choice2 != 3);
                    break;
            }
//MENU ONLINE
            case 2:{
                int choice5;
                do {
                    choice5=Menu::menuSecond();
                    switch (choice5){
                        case 1:{
                            //VISUALIZZA CARTELLE DI PORGETTI ONline
                            auto directories= File::get_directories("online");
                            Menu::MenuOnlineCaricamento( directories,maxExecutionTime);
                            break;
                        }
                            
                        case 2:{
               //CARICA Nuova rete
                            Menu::MenuCaricamentoOnlineNuovaRete( observation, scenario, maxExecutionTime);
                            
                           
                            break;
                        }
                        case 3:{
                            break;
                        }
                        default:{
                                    std::cout << "Not a Valid Choice. \n"
                                              << "Choose again.\n";
                        }
                    }
                }while(choice5!=3);
                break;
            }

            case 3:{
               auto network= Generate::createNetwork(1);
                NetworkIO::save("retenew.json", network);
                auto networkz= Generate::createNetworkZ(1);
                NetworkIO::save("z.json", networkz);
                auto network2= Generate::createNetworkLin(1);
                NetworkIO::save("retelin.json", network2);
                
                auto network3=Generate::createNetworkStar(4);
                  NetworkIO::save("retestrar.json", network3);
                break;
                
            }
            case 4:{
               

                std::cout << "End of Program.\n";
                break;
                
            }
            default:{
                std::cout << "Not a Valid Choice. \n"
                << "Choose again.\n";}



            }

            }while(choice!=4);



    return 0;
}




void creaSpaziO(std::string inFile,std::string offline,std::string projcetName,std::string bsObservation ,std::string bsOutObservation,std::string bsBond,std::string nomeO,std::string nomeS,std::string nome ,std::string outFile, bool observation,bool scenario,bool saveFile,bool label,bool prune,long maxExecutionTime){
        std::cout << "Insert network's name \n";
                                               std::cin >> inFile;
                                               std::filesystem::path sourceFile = inFile;
                                               std::filesystem::path targetParent = offline+projcetName;
                                               File::copyfile( inFile , targetParent,projcetName);
                                            observation = Menu::menuYN("Do you want to bond behavioral space with observation?");
                                               if (observation) {
                                                   std::cout << "Insert observation's name \n";
                                                   std::cin >> bsObservation;
                                               }
                                               if(!observation){
                                                   scenario = Menu::menuYN("Do you want to bond behavioral space with scenario?");
                                                   if (scenario) {
                                                       std::cout << "Insert scenario's name \n";
                                                       std::cin >> bsBond;
                                                   }
                                               }
                                                label = Menu::menuYN("Do you want to decorate behavioral space before pruning ?");
                                                prune = Menu::menuYN("Do you want to prune the behavioral space ?");
                                                saveFile = Menu::menuYN("Do you want to save behavioral space ?");
                                               std::string nomefin;
                                               if (saveFile) {
                                                   nomeO="";
                                                   nomeS="";
                                                   nome="bs_";
                                                   
                                                   if(observation){
                                                       nomeO="_"+bsObservation+"_obs";
                                                   }
                                                   if(scenario){
                                                       nomeS="_"+bsBond+"_sce";
                                                   }
                                                   nomefin=projcetName+nomeS+nomeO+".json";
                                                   outFile=offline+projcetName+"/"+nome+nomefin;
                                               }
                                               handleBS(maxExecutionTime, bsObservation, bsBond, bsOutObservation, prune,
                                                label, inFile, outFile);
                                               std::cout << "Decoration in progess..\n";
                                               
                                      
                                               prune = false;
                                               saveFile = Menu::menuYN("Do you want to save behavioral space decorated?");
                                               std::string decbh;
                                               if (saveFile) {
                                                   nome="bsd_";
                                                   decbh=offline+projcetName+"/"+nome+projcetName+nomeS+nomeO+".json";
                                              }
                                              handleLBS(maxExecutionTime, prune, outFile, decbh);
                                              std::cout << "Dictionary creation... \n";
                                              saveFile = Menu::menuYN("Do you want to save the dictionary ?");
                                               std::string dic;
                                              if (saveFile) {
                                                  dic=offline+projcetName+"/"+"dic_"+projcetName+".json";
                                              }
                                              handleDic(maxExecutionTime, decbh, dic,false);
                                             
    }

std::vector<std::string> convertObsStringToVector(std::string stringa){
    std::stringstream ss(stringa);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> vstrings(begin, end);
    std::copy(vstrings.begin(), vstrings.end(), std::ostream_iterator<std::string>(std::cout, "\n"));
    return vstrings;
}

//creazione spazio comportamentale a partire dalla rete
void handleBS(long time, const std::string &bsObservation, const std::string &bsBond, const std::string &bsOutObservation, bool prune, bool label,
              const std::string &inFile, const std::string &outFile) {
    auto network = NetworkIO::load(inFile);
    std::shared_ptr<SimpleNetwork> observation = nullptr, bond = nullptr;
    if (!bsObservation.empty()) observation = SimpleNetworkIO::load(bsObservation);
    if (!bsBond.empty()) bond = SimpleNetworkIO::load(bsBond);

    std::shared_ptr<long> maxExecutionTime = nullptr;
    long startingMaxExecutionTime = 0;
    if (time > 0) {
        maxExecutionTime = std::make_shared<long>(time);
        startingMaxExecutionTime = time;
    }

    auto startingTime = std::chrono::high_resolution_clock::now();


    std::shared_ptr<BehavioralSpace> bs;

    try {
        if (label) bs = LabeledBSBuilder::build(network, maxExecutionTime, observation, bond);
       
       
        else bs = BSBuilder::build(network, maxExecutionTime, observation, bond);

        if (prune) BSBuilder::prune(bs, maxExecutionTime);

        if (!bsOutObservation.empty()) {
            auto constructedObservation = DFAConstructor::constructObservation(bs,true ,maxExecutionTime);
            SimpleNetworkIO::save(bsOutObservation, *constructedObservation);
        }

        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        Logger::log("Time elapsed (ms): " + std::to_string(elapsed));
        if (maxExecutionTime) Logger::log("Time remaining to reach limit (ms): " + std::to_string(startingMaxExecutionTime - elapsed));

        BehavioralSpaceIO::save(outFile, *bs);

    } catch(std::runtime_error& e) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        Logger::err("Time elapsed (ms): " + std::to_string(elapsed));
    }
}




void handleBSPref(long time, const std::string &bsObservation, const std::string &bsBond, const std::string &bsOutObservation, bool prune, bool label,
              const std::string &inFile, const std::string &outFile,int prefix) {
    auto network = NetworkIO::load(inFile);
    std::shared_ptr<SimpleNetwork> observation = nullptr, bond = nullptr;
    if (!bsObservation.empty()) observation = SimpleNetworkIO::load(bsObservation);
    if (!bsBond.empty()) bond = SimpleNetworkIO::load(bsBond);

    std::shared_ptr<long> maxExecutionTime = nullptr;
    long startingMaxExecutionTime = 0;
    if (time > 0) {
        maxExecutionTime = std::make_shared<long>(time);
        startingMaxExecutionTime = time;
    }

    auto startingTime = std::chrono::high_resolution_clock::now();


    std::shared_ptr<BehavioralSpace> bs;

    try {
        if (label) bs = LabeledBSBuilder::build(network, maxExecutionTime, observation, bond);
        else bs = BSBuilder::build(network, maxExecutionTime, observation, bond);

        if (prune) BSBuilder::prune(bs, maxExecutionTime);

        if (!bsOutObservation.empty()) {
            auto constructedObservation = DFAConstructor::constructObservation(bs,true, maxExecutionTime);
            SimpleNetworkIO::save(bsOutObservation, *constructedObservation);
        }

        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        Logger::log("Time elapsed (ms): " + std::to_string(elapsed));
        if (maxExecutionTime) Logger::log("Time remaining to reach limit (ms): " + std::to_string(startingMaxExecutionTime - elapsed));

        BehavioralSpaceIO::save(outFile, *bs);

    } catch(std::runtime_error& e) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        Logger::err("Time elapsed (ms): " + std::to_string(elapsed));
    }
}
void handleLBSPrefix(long time, bool prune, const std::string &inFile, const std::string &outFile,int prefix) {
    auto bs = BehavioralSpaceIO::load(inFile);
    std::shared_ptr<long> maxExecutionTime = nullptr;
    long startingMaxExecutionTime = 0;
    if (time > 0) {
        maxExecutionTime = std::make_shared<long>(time);
        startingMaxExecutionTime = time;
    }
    auto startingTime = std::chrono::high_resolution_clock::now();

    try {
        auto lbs = LabeledBSBuilder::build(bs, maxExecutionTime,prefix);
        if (prune) BSBuilder::prune(lbs, maxExecutionTime);

        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        Logger::log("Time elapsed (ms): " + std::to_string(elapsed));
        if (maxExecutionTime) Logger::log("Time remaining to reach limit (ms): " + std::to_string(startingMaxExecutionTime - elapsed));
        BehavioralSpaceIO::save(outFile, *lbs);
    } catch (std::runtime_error &e) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        Logger::err("Time elapsed (ms): " + std::to_string(elapsed));
    }
}

//etichettatura spazio comportamentale a partire da uno spazio comportamentale non etichettato
void handleLBS(long time, bool prune, const std::string &inFile, const std::string &outFile) {
    auto bs = BehavioralSpaceIO::load(inFile);
    std::shared_ptr<long> maxExecutionTime = nullptr;
    long startingMaxExecutionTime = 0;
    if (time > 0) {
        maxExecutionTime = std::make_shared<long>(time);
        startingMaxExecutionTime = time;
    }
    auto startingTime = std::chrono::high_resolution_clock::now();

    try {
        auto lbs = LabeledBSBuilder::build(bs, maxExecutionTime);
        if (prune) BSBuilder::prune(lbs, maxExecutionTime);

        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        Logger::log("Time elapsed (ms): " + std::to_string(elapsed));
        if (maxExecutionTime) Logger::log("Time remaining to reach limit (ms): " + std::to_string(startingMaxExecutionTime - elapsed));
        BehavioralSpaceIO::save(outFile, *lbs);
    } catch (std::runtime_error &e) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        Logger::err("Time elapsed (ms): " + std::to_string(elapsed));
    }
}

//creazione dizionario partendo da uno spazio comportamentale etichettatto
void handleDic(long time, const std::string &inFile, const std::string &outFile, bool online) {
    auto lbs = BehavioralSpaceIO::load(inFile);
    std::shared_ptr<long> maxExecutionTime = nullptr;
    long startingMaxExecutionTime = 0;
    if (time > 0) {
        maxExecutionTime = std::make_shared<long>(time);
        startingMaxExecutionTime = time;
    }
    auto startingTime = std::chrono::high_resolution_clock::now();
    try {
        auto dictionary = DFAConstructor::constructDictionary(lbs,online, maxExecutionTime);
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        Logger::log("Time elapsed (ms): " + std::to_string(elapsed));
        if (maxExecutionTime) Logger::log("Time remaining to reach limit (ms): " + std::to_string(startingMaxExecutionTime - elapsed));
        DictionaryIO::save(outFile, *dictionary);
    } catch (std::runtime_error &e) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        Logger::err("Time elapsed (ms): " + std::to_string(elapsed));
    }
}


//creazione di un prefisso del dizionario
void handleDicP(long time, const std::string &inFile, const std::string &outFile, int lunghezzaPrefisso) {
    
    
    std::shared_ptr<long> maxExecutionTime = nullptr;
    auto bs = BehavioralSpaceIO::load(inFile);
    
    long startingMaxExecutionTime = 0;
    if (time > 0) {
        maxExecutionTime = std::make_shared<long>(time);
        startingMaxExecutionTime = time;
    }
    auto startingTime = std::chrono::high_resolution_clock::now();
    try {
        auto lbs = LabeledBSBuilder::build(bs, maxExecutionTime,lunghezzaPrefisso);
        auto dictionary = DFAConstructor::constructDictionaryPrefix(lunghezzaPrefisso,lbs, maxExecutionTime);
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        Logger::log("Time elapsed (ms): " + std::to_string(elapsed));
        if (maxExecutionTime) Logger::log("Time remaining to reach limit (ms): " + std::to_string(startingMaxExecutionTime - elapsed));
        DictionaryIO::save(outFile, *dictionary);
    } catch (std::runtime_error &e) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        Logger::err("Time elapsed (ms): " + std::to_string(elapsed));
    }
}


//unione di dizionari parziali
void handleMD(long time, std::vector<std::string>& inFiles, const std::string &outFile) {
    std::vector<std::shared_ptr<Dictionary>> inputs;
    for (const auto &s : inFiles) inputs.push_back(DictionaryIO::load(s));
    std::shared_ptr<long> maxExecutionTime = nullptr;
    long startingMaxExecutionTime = 0;
    if (time > 0) {
        maxExecutionTime = std::make_shared<long>(time);
        startingMaxExecutionTime = time;
    }
    auto startingTime = std::chrono::high_resolution_clock::now();
    try {
        auto merged = DictionaryMerge::merge(inputs, maxExecutionTime);
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        Logger::log("Time elapsed (ms): " + std::to_string(elapsed));
        if (maxExecutionTime) Logger::log("Time remaining to reach limit (ms): " + std::to_string(startingMaxExecutionTime - elapsed));
        DictionaryIO::save(outFile, *merged);
    } catch (std::runtime_error &e) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        Logger::err("Time elapsed (ms): " + std::to_string(elapsed));
    }
}


//mostra diagnosi relativa ad un osservazione
void handleDiag(long time, std::vector<std::string> &diagnosis, const std::string &inFile) {

    auto dictionary = DictionaryIO::load(inFile);
    std::shared_ptr<long> maxExecutionTime = nullptr;
    long startingMaxExecutionTime = 0;
    if (time > 0) {
        maxExecutionTime = std::make_shared<long>(time);
        startingMaxExecutionTime = time;
    }
    auto startingTime = std::chrono::high_resolution_clock::now();

    try {
        auto result = DFAConstructor::diagnoseFromObservation(dictionary, diagnosis, maxExecutionTime);

        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        Logger::log("Time elapsed (ms): " + std::to_string(elapsed));
        if (maxExecutionTime) Logger::log("Time remaining to reach limit (ms):  " + std::to_string(startingMaxExecutionTime - elapsed));

        std::cout << "{\"diagnosis\": [";
        bool firstSet = true;
        for (auto diagSet : *result) {
            if (!firstSet) std::cout << ", ";
            bool firstLabel = true;
            std::cout << "[";
            for (const auto &diag : diagSet) {
                if (!firstLabel) std::cout << ",";
                std::cout << "\"" << diag << "\"";
                firstLabel = false;
            }
            std::cout << "]";
            firstSet = false;
        }
        std::cout << "]";
    } catch (std::invalid_argument &e) {
        std::cout << R"({"error": ")" + std::string(e.what()) + "\"}";
        Logger::log("Could not diagnose. Error: " + std::string(e.what()));
    } catch (std::runtime_error &e) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        Logger::err("Time elapsed (ms): " + std::to_string(elapsed));
    }
}


// creazione temporal abduce(Monitoraggio) relativo ad un osservazione del dizionario
void handleAbd(long time, std::vector<std::string> &observations, const std::string &inFile,const std::string &outFile) {

    auto dictionary = DictionaryIO::load(inFile);
    std::shared_ptr<long> maxExecutionTime = nullptr;
    long startingMaxExecutionTime = 0;
    if (time > 0) {
        maxExecutionTime = std::make_shared<long>(time);
        startingMaxExecutionTime = time;
    }
    auto startingTime = std::chrono::high_resolution_clock::now();
    std::vector<std::shared_ptr<std::vector<std::shared_ptr<temporalAbduction>>>> abduces;
    std::vector< std::vector<std::string>> osservazioni;
    /*try {*/

        //machine learingin
       /* std::string line;
        std::ifstream myfile ("data99992.txt");
        if (myfile.is_open())
        {
            while ( getline (myfile,line) )
            {
                std::istringstream iss(line);
                std::vector<std::string> osservazione((std::istream_iterator<std::string>(iss)),
                                                 std::istream_iterator<std::string>());
                int it=1;

                auto result = DFAConstructor::diagnoseFromAbduce(dictionary,osservazione, maxExecutionTime);
                abduces.push_back(result);
                osservazioni.push_back(osservazione);

                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
                Logger::log("Time elapsed (ms): " + std::to_string(elapsed));
                if (maxExecutionTime) Logger::log("Time remaining to reach limit (ms): " + std::to_string(startingMaxExecutionTime - elapsed));
            }
            myfile.close();
        }

        */

               // int it=1;

            auto result = DFAConstructor::diagnoseFromAbduce(dictionary,observations, maxExecutionTime);
            DictionaryIO::saveAbduce(outFile, result,observations);
               /* abduces.push_back(result);
                osservazioni.push_back(diagnosis);*/


        /*}*/



       // DictionaryIO::save2("abduce.json", abduces,osservazioni);







   /* } catch (std::invalid_argument &e) {
        std::cout << R"({"error": ")" + std::string(e.what()) + "\"}";
        Logger::log("Could not diagnose. Error: " + std::string(e.what()));
    } catch (std::runtime_error &e) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        Logger::err("Time elapsed (ms): " + std::to_string(elapsed));
    }*/
}


// estensione online di un dizionario partendo da un osservazione lineare
void handleAbdO(long time, std::vector<std::string> &observations, const std::string &inFile,const std::string &outFile,std::string &net) {

    auto dictionary = DictionaryIO::load(inFile);
    std::shared_ptr<long> maxExecutionTime = nullptr;
    long startingMaxExecutionTime = 0;
    if (time > 0) {
        maxExecutionTime = std::make_shared<long>(time);
        startingMaxExecutionTime = time;
    }
    auto startingTime = std::chrono::high_resolution_clock::now();

    std::vector<std::shared_ptr<std::vector<std::shared_ptr<temporalAbduction>>>> abduces;

    try {

                std::vector<std::string> osservazione;
                for (auto &el : observations){
                    osservazione.push_back(el);
                }

                auto result = DFAConstructor::extendDictionaryFromObs(dictionary,osservazione,net, maxExecutionTime);
                auto dic=result->inDictionary.get();
                DictionaryIO::save(outFile, *dic);
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
                Logger::log("Time elapsed (ms): " + std::to_string(elapsed));
                if (maxExecutionTime) Logger::log("Time remaining to reach limit (ms): " + std::to_string(startingMaxExecutionTime - elapsed));

                //DictionaryIO::saveAbduce("abduce.json", result,osservazione);



      /*  DictionaryIO::save2("abduce.json", abduces,osservazioni);*/



        /*auto result = DFAConstructor::diagnoseFromAbduce(dictionary, diagnosis, maxExecutionTime);

        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        Logger::log("Time elapsed (ms): " + std::to_string(elapsed));
        if (maxExecutionTime) Logger::log("Time remaining to reach limit (ms): " + std::to_string(startingMaxExecutionTime - elapsed));*/

        /*std::cout << "{\"diagnosis\": [";
        bool firstSet = true;
        for (auto diagSet : *result) {
            if (!firstSet) std::cout << ", ";
            bool firstLabel = true;
            std::cout << "[";
            for (const auto &diag : diagSet) {
                if (!firstLabel) std::cout << ",";
                std::cout << "\"" << diag << "\"";
                firstLabel = false;
            }
            std::cout << "]";
            firstSet = false;
        }
        std::cout << "]";*/
    } catch (std::invalid_argument &e) {
        std::cout << R"({"error": ")" + std::string(e.what()) + "\"}";
        Logger::log("Could not diagnose. Error: " + std::string(e.what()));
    } catch (std::runtime_error &e) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        Logger::err("Time elapsed (ms): " + std::to_string(elapsed));
    }
}


void handleAbdS(long time, const std::string &dicBond, const std::string &inFile,const std::string &outFile) {

    auto dictionary = DictionaryIO::load(inFile);
    auto scenario = DictionaryIO::load(dicBond);
    auto scenarioAutoma=DFAConstructor::convertScenarioToSimpleNetwork(scenario);
    SimpleNetworkIO::save2("scenarioAutoma.json",scenarioAutoma);
    std::shared_ptr<long> maxExecutionTime = nullptr;
    long startingMaxExecutionTime = 0;
    if (time > 0) {
        maxExecutionTime = std::make_shared<long>(time);
        startingMaxExecutionTime = time;
    }
    auto startingTime = std::chrono::high_resolution_clock::now();

    std::vector<std::shared_ptr<std::vector<std::shared_ptr<temporalAbduction>>>> abduces;

    try {



        auto result = DFAConstructor::extendDictionaryFromScenario(dictionary,scenarioAutoma, maxExecutionTime);

        DictionaryIO::save(outFile, *result);
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        Logger::log("Time elapsed (ms): " + std::to_string(elapsed));
        if (maxExecutionTime) Logger::log("Time remaining to reach limit (ms): " + std::to_string(startingMaxExecutionTime - elapsed));

        //DictionaryIO::saveAbduce("abduce.json", result,osservazione);



        /*  DictionaryIO::save2("abduce.json", abduces,osservazioni);*/



        /*auto result = DFAConstructor::diagnoseFromAbduce(dictionary, diagnosis, maxExecutionTime);

        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        Logger::log("Time elapsed (ms): " + std::to_string(elapsed));
        if (maxExecutionTime) Logger::log("Time remaining to reach limit (ms): " + std::to_string(startingMaxExecutionTime - elapsed));*/

        /*std::cout << "{\"diagnosis\": [";
        bool firstSet = true;
        for (auto diagSet : *result) {
            if (!firstSet) std::cout << ", ";
            bool firstLabel = true;
            std::cout << "[";
            for (const auto &diag : diagSet) {
                if (!firstLabel) std::cout << ",";
                std::cout << "\"" << diag << "\"";
                firstLabel = false;
            }
            std::cout << "]";
            firstSet = false;
        }
        std::cout << "]";*/
    } catch (std::invalid_argument &e) {
        std::cout << R"({"error": ")" + std::string(e.what()) + "\"}";
        Logger::log("Could not diagnose. Error: " + std::string(e.what()));
    } catch (std::runtime_error &e) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        Logger::err("Time elapsed (ms): " + std::to_string(elapsed));
    }



}
