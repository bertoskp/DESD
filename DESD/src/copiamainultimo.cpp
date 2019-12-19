
#include "stdIncludes.h"
#include <chrono>
#include "CLI11.hpp"
#include "whereami.h"
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <cstring>
#include <sys/param.h>
#include <mach-o/dyld.h>

#include <iostream>

#include "manipulators/BSBuilder.h"
#include "manipulators/DFAConstructor.h"
#include "manipulators/LabeledBSBuilder.h"
#include "manipulators/DictionaryMerge.h"
#include "Utils/NetworkIO.h"
#include "Data/SimpleNetwork.h"

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
bool menuYN( std::string question);
std::vector<std::string> get_directories(const std::string& s);
bool path_exists(const std::string& path_src);
void copyfile( std::filesystem::path sourceFile ,  std::filesystem::path targetParent ,std::string name);
std::vector<std::string> convertObsStringToVector(std::string stringa);
int menuFirst();
int menuSecond();
int loadOffline();
int newOffline();

int loadOnline();
int newOnline();

std::string ExePath();
void readFileInProject(bool online, std::string projName, int space);

int main(int argc, char *argv[]) {
     std::string bsObservation, bsBond, bsOutObservation,inFile, outFile,dicBond;
    bool observation,scenario = false;
    bool prune, label,saveFile;
    long maxExecutionTime = 0;
    int depth= 0;
    std::string temp1 ,temp2;
    std::vector<std::string> inFiles;
    std::vector<std::string> observations;

    std::cout<<"Working directory"<<std::endl;
    std::string cwd = std::filesystem::current_path();
    std::cout<<cwd+"\n"<<std::endl;
           
    static int choice;
    do{
        
       
        //menu generale scelta offline o online
        choice=menuFirst();
        switch (choice) {
        case 1:{
            int choice2;

            do {
//MENU OFFLINE
                choice2=menuSecond();
                switch (choice2) {
                    case 1:{
                        
                        //VISUALIZZA NOMI CARTELLE DI PROGETTO OFFLINE ESISTENTI
                        auto d= get_directories("offline");
                        while(true){
                            int choice3;
                            int h=1;
                            bool verifica=false;
                            //VISUALIZZA NOMI
                            for( auto e :d){
                                verifica=true;
                                std::string s=std::to_string(h);
                                std::cout <<s +" - "+e+".\n"<< std::endl;
                                h++;
                            }
                            if(verifica==false){
                                std::cout << "there are no existing projects"<< std::endl;
                                break;
                                
                            }
                            std::string s=std::to_string(h);
                            std::cout << s +"  - back.\n"<< std::endl;
                            std::cin >> choice3;
                            
                            if( choice3>0 &&choice3<=h-1){
                                std::string namePrj=d[choice-1];
                                int choice8;
                                do {
                                    choice8=loadOffline();
                                    
                                    //CARICA SPAZI COMPORTAMENTALI
                                    if(choice8>=1 and choice8<=3) {
                                        readFileInProject(false,  namePrj, choice8);
                                    }
                                }while(choice8!=8);
                            }
                            
                            else if(choice3==h){
                                std::cout  << "indietro.\n"<< std::endl;
                                break;
                            }
                            
                            else{
                                std::cout  << "not valid,select a valid choice.\n" << std::endl;
                                
                            }
                        }
                        break;
                        }
                    
                    
                        //CARICA UNA NUOVA RETE MODALITA OFFLINE
                        case 2:{
                            std::string projcetName;
                            std::cout <<"Insert the name of the project"<< std::endl;
                            std::cin >> projcetName;
                            std::string offline="offline/";
                            auto percorso=offline+projcetName;
                            //VERIFICa UNIVOCITA NOME
                            if(! path_exists(percorso)){
                                mkdir( percorso.c_str(),S_IRWXU);
                                //MENU OFFLINE
                                int choice4;
                                
                                do {
                                    choice4=newOffline();
                                    std::string nomeO="",nomeS="", nome="";
                                    switch (choice4){
                                            
                                            //CALCOLO SPAZIO COMPORTAMENTALE OFFLINE
                                        case 0:{
                                          std::cout << "Insert network's name \n";
                                          std::cin >> inFile;
                                          std::filesystem::path sourceFile = inFile;
                                          std::filesystem::path targetParent = offline+projcetName;
                                        if(path_exists(inFile)){
                                        
                                          copyfile( inFile , targetParent,projcetName);
                                          observation = menuYN("Do you want to bond behavioral space with observation?");
                                          if (observation) {
                                              std::cout << "Insert observation's name \n";
                                              std::cin >> bsObservation;
                                          }
                                            scenario=false;
                                          if(!observation){
                                              scenario = menuYN("Do you want to bond behavioral space with scenario?");
                                              if (scenario) {
                                                  std::cout << "Insert scenario's name \n";
                                                  std::cin >> bsBond;
                                              }
                                          }
                                          label = menuYN("Do you want to decorate behavioral space before pruning ?");
                                          prune = menuYN("Do you want to prune the behavioral space ?");
                                          saveFile = menuYN("Do you want to save behavioral space ?");
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
                                          saveFile = menuYN("Do you want to save behavioral space decorated?");
                                          std::string decbh;
                                          if (saveFile) {
                                              nome="bsd_";
                                              decbh=offline+projcetName+"/"+nome+projcetName+nomeS+nomeO+".json";
                                         }
                                         handleLBS(maxExecutionTime, prune, outFile, decbh);
                                         std::cout << "Dictionary creation... \n";
                                         saveFile = menuYN("Do you want to save the dictionary ?");
                                         std::string dic;
                                         if (saveFile) {
                                             dic=offline+projcetName+"/"+"dic_"+projcetName+".json";
                                         }
                                         handleDic(maxExecutionTime, decbh, dic,false);
                                        }
                                        else{
                                             std::cout << "This network doesn't exist \n";
                                        }
                                         break;
                                    }
                                            
                                    case 1:{
                                        std::cout << "Insert network's name \n";
                                        std::cin >> inFile;
                                        if(path_exists(inFile)){
                                            //copiafile(inFile, online+projcetName+inFile);
                                            observation = menuYN(
                                                    "Do you want to bond behavioral space with observation?");
                                            if (observation) {
                                                std::cout << "Insert observation's name \n";
                                                std::cin >> bsObservation;
                                            }
                                            scenario=false;
                                            if(!observation){
                                            scenario = menuYN("Do you want to bond behavioral space with scenario?");
                                            if (scenario) {
                                                std::cout << "Insert scenario's name \n";
                                                std::cin >> bsBond;
                                            }
                                            }
                                            label = menuYN("Do you want to decorate behavioral space before pruning ?");
                                            prune = menuYN("Do you want to prune the behavioral space ?");
                                            saveFile = menuYN("Do you want to save behavioral space ?");
                                            if (saveFile) {
                                                std::string nomeO="";
                                                std::string nomeS="";
                                                std::string nome="bs_";
                                                
                                                if(observation){
                                                    nomeO="_"+bsObservation+"_obs";
                                                }
                                                if(scenario){
                                                    nomeS="_"+bsBond+"_sce";
                                                }
                                                                                                    
                                                outFile=offline+nome+projcetName+nomeS+nomeO+".json";
                                            }
                                            handleBS(maxExecutionTime, bsObservation, bsBond, bsOutObservation, prune,
                                                     label, inFile, outFile);
                                        }
                                        else{
                                            std::cout << "This network doesn't exist \n";
                                        }
                                        break;
                                        }
                                        case 2:{
                                            std::cout << "Insert behavioral space's name \n";
                                            std::cin >> inFile;
                                            prune = menuYN("Do you want to prune the behavioral space ?");
                                            saveFile = menuYN("Do you want to save behavioral space ?");
                                            nome="bsd_";
                                            if (saveFile) {
                                                outFile=offline+nome+projcetName+nomeS+nomeO+".json";
                                                std::cout << "Insert name \n";
                                                std::cin >> outFile;
                                            }
                                            handleLBS(maxExecutionTime, prune, inFile, outFile);
                                            //handleLBSPrefix(maxExecutionTime, prune, inFile, outFile,4);
                                            break;
                                            }
                                        case 3:{
                                            std::cout << "Insert  decorated behavioral space's name \n";
                                            std::cin >> inFile;
                                            saveFile = menuYN("Do you want to save the dictionary ?");
                                            if (saveFile) {
                                                std::cout << "Insert name \n";
                                                std::cin >> outFile;
                                            }
                                            handleDic(maxExecutionTime, inFile, outFile,true);
                                            break;
                                        }
                                        case 4:{
                                            std::cout << "Insert the name of the first dictionary \n";
                                            std::cin >> temp1;
                                            inFiles.push_back(temp1);
                                            std::cout << "Insert the name of the first dictionary \n";
                                            std::cin >> temp1;
                                            inFiles.push_back(temp1);
                                            saveFile = menuYN("Do you want to save the dictionary's merged?");
                                            if (saveFile) {
                                                std::cout << "Insert name \n";
                                                std::cin >> outFile;
                                            }
                                            handleMD(maxExecutionTime, inFiles, outFile);
                                            break;
                                        }
                                        case 5:{
                                            std::cout << "Insert the name of the dictionary \n";
                                            std::cin >> inFile;
                                            std::cout
                                                    << "Insert the name of the observation \n(each observation in the sequence must be  separte with blank character) \n ex: o1 o2 o3\n";
                                            std::cin >> temp1;
                                            getline(std::cin, temp2);
                                            observations = convertObsStringToVector(temp1 + temp2);
                                            handleDiag(maxExecutionTime, observations, inFile);
                                            break;
                                        }
                                        case 6:{
                                            std::cout << "End of Program.\n";
                                            break;
                                        }
                                        default:{
                                                std::cout << "Not a Valid Choice. \n"
                                                          << "Choose again.\n";
                                        }
                                    }
                                } while (choice4!=6);
                            }
                            else{
                                
                                std::cout <<"Nome gia esistente"<< std::endl;

                            }
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

                    } while (choice2 != 3);
                    break;
            }
//MENU ONLINE
            case 2:{
                int choice5;
                do {
                    choice5=menuSecond();
                    switch (choice5){
                        case 1:{
                            //VISUALIZZA CARTELLE DI PORGETTI ONline
                            auto d= get_directories("online");
                            while(true){
                                int h=1;
                                for( auto e :d){
                                    std::string s=std::to_string(h);
                                    std::cout <<s +" - "+e+".\n"<< std::endl;
                                    h++;
                                }
                                std::string s=std::to_string(h);
                                std::cout << s +"  - indietro.\n"<< std::endl;
                                std::cin >> choice5;
                                if( choice5>0 &&choice5<=h-1){
                                    do {
                                        choice5=loadOnline();
                                    }while(choice5==8);
                                }
                                else if(choice5==h){
                                    std::cout  << "indietro.\n"<< std::endl;
                                    break;
                                }
                                else{
                                    std::cout  << "no valid.\n" << std::endl;
                                }
                            }
                            break;
                        }
                            
                        case 2:{
               //CARICA GIA ESISTENTE
                            std:: string projcetName;
                            std::cout <<"Insert the name of the project"<< std::endl;
                            std::cin >> projcetName;
                            std::string online="online/";
                            auto percorso=online+projcetName;
                            if(! path_exists(percorso)){
                                mkdir( percorso.c_str(),S_IRWXU);
                                std::cout <<"creata"<< std::endl;
                                int choice6=0;
                               do {
                                     choice6=newOnline();
                                     std::string nomeO="",nomeS="", nome="";
                                     switch (choice6) {
                                        case 0:{
                                            std::cout << "Insert network's name \n";
                                            std::cin >> inFile;
                                            std::filesystem::path sourceFile = inFile;
                                            std::filesystem::path targetParent = online+projcetName;
                                            copyfile( inFile , targetParent,projcetName);
                                            observation = menuYN(
                                                    "Do you want to bond behavioral space with observation?");
                                            if (observation) {
                                                std::cout << "Insert observation's name \n";
                                                std::cin >> bsObservation;
                                            }
                                            if(!observation){
                                            scenario = menuYN("Do you want to bond behavioral space with scenario?");
                                                if (scenario) {
                                                    std::cout << "Insert scenario's name \n";
                                                    std::cin >> bsBond;
                                                }
                                            }
                                            label = menuYN("Do you want to decorate behavioral space before pruning ?");
                                            prune = menuYN("Do you want to prune the behavioral space ?");
                                            saveFile = menuYN("Do you want to save behavioral space ?");
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
                                                outFile=online+projcetName+"/"+nome+nomefin;
                                            }
                                            handleBS(maxExecutionTime, bsObservation, bsBond, bsOutObservation, prune,
                                                     label, inFile, outFile);
                                            
                                            
                                            std::cout << "Decoration in progess..\n";
                                            prune = false;
                                            saveFile = menuYN("Do you want to save behavioral space decorated?");
                                            std::string decbh;
                                            if (saveFile) {
                                                nome="bsd_";
                                                decbh=online+projcetName+"/"+nome+projcetName+nomeS+nomeO+".json";
                                            }
                                            handleLBS(maxExecutionTime, prune, outFile, decbh);
                                            //handleLBSPrefix(maxExecutionTime, prune, outFile, decbh,4);
                                            std::cout << "Dictionary creation... \n";
                                            saveFile = menuYN("Do you want to save the dictionary ?");
                                            std::string dic;
                                               if (saveFile) {
                                                   dic=online+projcetName+"/"+"dic_"+projcetName+".json";
                                               }
                                            handleDic(maxExecutionTime, decbh, dic,true);
                                            break;
                                                                                          
                                           }
                                                     
                                                
                                            case 1:{
                                            std::cout << "Insert network's name \n";
                                            std::cin >> inFile;
                                            //copiafile(inFile, online+projcetName+inFile);
                                            observation = menuYN(
                                                    "Do you want to bond behavioral space with observation?");
                                            if (observation) {
                                                std::cout << "Insert observation's name \n";
                                                std::cin >> bsObservation;
                                            }
                                            if(!observation){
                                            scenario = menuYN("Do you want to bond behavioral space with scenario?");
                                            if (scenario) {
                                                std::cout << "Insert scenario's name \n";
                                                std::cin >> bsBond;
                                            }
                                            }
                                            label = menuYN("Do you want to decorate behavioral space before pruning ?");
                                            prune = menuYN("Do you want to prune the behavioral space ?");
                                            saveFile = menuYN("Do you want to save behavioral space ?");
                                            if (saveFile) {
                                                std::string nomeO="";
                                                std::string nomeS="";
                                                std::string nome="bs_";
                                                
                                                if(observation){
                                                    nomeO="_"+bsObservation+"_obs";
                                                }
                                                if(scenario){
                                                    nomeS="_"+bsBond+"_sce";
                                                }
                                                                                                    
                                                outFile=online+nome+projcetName+nomeS+nomeO+".json";
                                            }


                                            handleBS(maxExecutionTime, bsObservation, bsBond, bsOutObservation, prune,
                                                     label, inFile, outFile);
                                                
                                                break;
                                            }
                                            case 2:{
                                                    std::cout << "Insert behavioral space's name \n";
                                                    std::cin >> inFile;
                                                    prune = menuYN("Do you want to prune the behavioral space ?");
                                                    saveFile = menuYN("Do you want to save behavioral space ?");
                                                    if (saveFile) {
                                                        std::cout << "Insert name \n";
                                                        std::cin >> outFile;
                                                    }
                                                    handleLBS(maxExecutionTime, prune, inFile, outFile);
                                                    break;
                                                }
                                            case 3:{
                                                    std::cout << "Insert  decorated behavioral space's name \n";
                                                    std::cin >> inFile;
                                                    saveFile = menuYN("Do you want to save the dictionary ?");
                                                    if (saveFile) {
                                                        std::cout << "Insert name \n";
                                                        std::cin >> outFile;
                                                    }
                                                    handleDic(maxExecutionTime, inFile, outFile,true);

                                                    break;
                                            }
                                            case 4:{
                                                    std::cout << "Insert  decorated behavioral space's name \n";
                                                    std::cin >> inFile;
                                                    std::cout << "Insert depth of dictionary \n";
                                                    std::cin >> depth;
                                                    saveFile = menuYN("Do you want to save the dictionary's prefix?");


                                                    if (saveFile) {
                                                        std::cout << "Insert name \n";
                                                        std::cin >> outFile;
                                                    }
                                                    handleDicP(maxExecutionTime, inFile, outFile, depth);
                                                    break;
                                            }
                                            case 5:{
                                                    std::cout << "Insert the name of the first dictionary \n";
                                                    std::cin >> temp1;
                                                    inFiles.push_back(temp1);
                                                    std::cout << "Insert the name of the first dictionary \n";
                                                    std::cin >> temp1;
                                                    inFiles.push_back(temp1);
                                                    saveFile = menuYN("Do you want to save the dictionary's merged?");


                                                    if (saveFile) {
                                                        std::cout << "Insert name \n";
                                                        std::cin >> outFile;
                                                    }
                                                    handleMD(maxExecutionTime, inFiles, outFile);
                                                    break;
                                            }
                                            case 6:{
                                                    std::cout << "Insert the name of the dictionary \n";
                                                    std::cin >> inFile;
                                                    std::cout
                                                            << "Insert the name of the observation \n(each observation in the sequence must be  separte with blank character) \n ex: o1 o2 o3\n";
                                                    std::cin >> temp1;

                                                    getline(std::cin, temp2);


                                                    observations = convertObsStringToVector(temp1 + temp2);

                                                    handleDiag(maxExecutionTime, observations, inFile);
                                            
                                                break;
                                                
                                                }
                                            case 7:{
                                                    std::cout << "Insert the name of the dictionary \n";
                                                    std::cin >> inFile;
                                                    std::cout
                                                            << "Insert the name of the observation \n(each observation in the sequence must be  separte with blank character) \n ex: o1 o2 o3\n";
                                                    std::cin >> temp1;
                                                    getline(std::cin, temp2);
                                                    observations = convertObsStringToVector(temp1 + temp2);
                                                    saveFile = menuYN("Do you want to save the abduction?");


                                                    if (saveFile) {
                                                        std::cout << "Insert name \n";
                                                        std::cin >> outFile;
                                                    }
                                                    handleAbd(maxExecutionTime, observations, inFile, outFile);
                                                    break;
                                            }
                                            case 8:{
                                                    std::cout << "Insert the name of the dictionary to extend \n";
                                                    std::cin >> inFile;
                                                    std::cout
                                                            << "Insert the name of the observation \n(each observation in the sequence must be  separte with blank character) \n ex: o1 o2 o3\n";
                                                    std::cin >> temp1;
                                                    getline(std::cin, temp2);
                                                    observations = convertObsStringToVector(temp1 + temp2);
                                                    saveFile = menuYN("Do you want to save the extended dictionary?");


                                                    if (saveFile) {
                                                        std::cout << "Insert name \n";
                                                        std::cin >> outFile;
                                                    }

                                                    handleAbdO(maxExecutionTime, observations, inFile, outFile);

                                                    break;
                                                
                                            }
                                            case 9:{

                                                    std::cout << "Insert the name of the dictionary to extend \n";
                                                    std::cin >> inFile;
                                                    std::cout << "Insert the name scenario name\n";
                                                    std::cin >> dicBond;

                                                    saveFile = menuYN("Do you want to save the extended dictionary?");


                                                    if (saveFile) {
                                                        std::cout << "Insert name \n";
                                                        std::cin >> outFile;
                                                    }
                                                    handleAbdS(maxExecutionTime, dicBond, inFile,
                                                               outFile);//extra information about the mode and the controlle
                                                    break;
                                            }
                                            case 10:{
                                                        std::cout << "End of Program.\n";
                                                        break;
                                            }
                                            default:{
                                                    std::cout << "Not a Valid Choice. \n"
                                                              << "Choose again.\n";
                                            }
                                    }
                                }while (choice6 != 10);
                            }
                            else{
                                std::cout <<"Nome gia esistente"<< std::endl;
                            }
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
                }while(choice!=3);
                break;
            }

            case 3:{
                std::cout << "End of Program.\n";
                break;
                
            }
            default:{
                std::cout << "Not a Valid Choice. \n"
                << "Choose again.\n";}



                    }

            }while(choice!=3);



    return 0;


/*
    CLI::App app("Automata Manager");
    app.require_subcommand(1);

    // Opzioni principali applicazione
    app.set_help_all_flag("--help-all", "Expand help");
    std::string fileIn, fileOut;

    // Sottocomandi
    CLI::App *bs   = app.add_subcommand("bs",   "Compute a Behavioral Space (BS) from a given Network");
    CLI::App *lbs  = app.add_subcommand("lbs",  "Labels an input Behavioral Space");
    CLI::App *dic  = app.add_subcommand("dic",  "Compute a Dictionary from a given Labeled Behavioral Space");
    CLI::App *dicp = app.add_subcommand("dicp", "Compute a Dictionary from a given Labeled Behavioral Space and depth");
    CLI::App *md   = app.add_subcommand("md",   "Merges two or more dictionaries into a single one.");
    CLI::App *diag = app.add_subcommand("diag", "Diagnoses a problem, given a dictionary and an observation");
    CLI::App *abd = app.add_subcommand("abd", "Temporal abduce from a given dictionary and an observation");
    CLI::App *abdo = app.add_subcommand("abdo", "Extende Partial dictionary from obs");
    CLI::App *abds = app.add_subcommand("abds", "Extende Partial dictionary from scenario");

    long maxExecutionTime = 0;
    app.add_option("-t,--time", maxExecutionTime, "Maximum execution time (expressed in milliseconds). "
                                                  "If the execution of an algorithm continues after this value, it preemptively stops.");

    std::vector<std::string> mdFiles;
    // Aggiunta in/out ad ognuno
    bs->add_option("-i,--in", fileIn, "Input (Network JSON). If none provided, reads input from standard input")->expected(1);
    bs->add_option("-o,--out", fileOut, "Output (BehavioralSpace JSON). If none provided, writes output to standard output")->expected(1);
    lbs->add_option("-i,--in", fileIn, "Input (BehavioralSpace JSON). If none provided, reads input from standard input")->expected(1);
    lbs->add_option("-o,--out", fileOut, "Output (BehavioralSpace JSON). If none provided, writes output to standard output")->expected(1);
    dic->add_option("-i,--in", fileIn, "Input (BehavioralSpace JSON). If none provided, reads input from standard input")->expected(1);
    dic->add_option("-o,--out", fileOut, "Output (Dictionary JSON). If none provided, writes output to standard output")->expected(1);
    dicp->add_option("-i,--in", fileIn, "Input (BehavioralSpace JSON). If none provided, reads input from standard input")->expected(1);
    dicp->add_option("-o,--out", fileOut, "Output (Dictionary JSON). If none provided, writes output to standard output")->expected(1);
    md->add_option( "-i, --in", mdFiles, "List of dictionary files to merge (Dictionary JSON). This subcommand does not allow standard input.")->required()->expected(-2);
    md->add_option("-o,--out", fileOut, "Output (Dictionary JSON). If none provided, writes output to standard output")->expected(1);
    diag->add_option("-i,--in", fileIn, "Input (Dictionary JSON). If none provided, reads input from standard input")->expected(1);
    abd->add_option("-i,--in", fileIn, "Input (Dictionary JSON). If none provided, reads input from standard input")->expected(1);
    abd->add_option("-o,--out", fileOut, "Output (Abduce JSON). If none provided, writes output to standard output")->expected(1);
    abdo->add_option("-i,--in", fileIn, "Input (Partial Dictionary JSON). If none provided, reads input from standard input")->expected(1);
    abdo->add_option("-o,--out", fileOut, "Output (Dictionary extended JSON). If none provided, writes output to standard output")->expected(1);
    abds->add_option("-i,--in", fileIn, "Input (Partial Dictionary JSON). If none provided, reads input from standard input")->expected(1);
    abds->add_option("-o,--out", fileOut, "Output (Dictionary extended JSON). If none provided, writes output to standard output")->expected(1);


    // Spazio Comportamentale
    std::string bsObservation, bsBond, bsOutObservation,dicBond;
    int lunghezzaPrefisso;
    bool bsPrune = false, bsLabel = false;
    bs->add_flag("-p,--prune", bsPrune, "Prunes the computed Behavioral Space");
    bs->add_flag(
        "-l,--label",
        bsLabel,
        "While computing the Behavioral Space, automatically label it. This returns a Labeled Behavioral Space."
        );

    bs->add_option(
        "--observation",
        bsObservation,
        "Computes a Partial Behavioral Space, restricted by this observation. Expects a JSON SimpleNetwork file "
        "representing the observation."
        )->expected(1);

    auto bondOption = bs->add_option(
        "--bond",
        bsBond,
        "Computes a Partial Behavioral Space, restricted by this bond. Expects a JSON SimpleNetwork file representing "
        "the bond."
        )->expected(1);

    bs->add_option(
        "--out-observation",
        bsOutObservation,
        "After computing the Behavioral Space, constructs a Physically Possible Observation from it."
        "Expects name of the output file"
        )->needs(bondOption)->expected(1);


    // Etichettatura
    bool lbsPrune = false;
    lbs->add_flag("-p,--prune", lbsPrune, "Prunes the computed Labeled Behavioral Space");

    //prefisso del dizionario
    dicp->add_option(
            "--lunghezza",
            lunghezzaPrefisso,
            "After computing the Behavioral Space, constructs a Physically Possible Observation from it."
            "Expects name of the output file"
    )->required();

    // Diagnosi
    std::vector<std::string> diagDiagnosis;
    diag->add_option(
        "observation",
        diagDiagnosis,
        "Input observation. Must only contain observable labels from the input dictionary, separated by a space. "
        "For example: o3 o2 o3")->required();
    // ABDUCE
    std::vector<std::string> abdDiagnosis;
    abd->add_option(
            "observation",
            abdDiagnosis,
            "Input observation. Must only contain observable labels from the input dictionary, separated by a space. "
            "For example: o3 o2 o3")->required();

    // ABDUCE from obs
    std::vector<std::string> abdoDiagnosis;
    abdo->add_option(
            "observation",
            abdoDiagnosis,
            "Input observation. Must only contain observable labels from the input dictionary, separated by a space. "
            "For example: o3 o2 o3")->required();

    std::vector<std::string> abdsDiagnosis;
    auto bondOptionD = abds->add_option(
            "--bond",
            dicBond,
            "Computes a Partial Behavioral Space, restricted by this bond. Expects a JSON SimpleNetwork file representing "
            "the bond."
    )->required();

    CLI11_PARSE(app, argc, argv);

    if (bs->parsed()) handleBS(maxExecutionTime, bsObservation, bsBond, bsOutObservation, bsPrune, bsLabel, fileIn, fileOut);
    if (lbs->parsed()) handleLBS(maxExecutionTime, lbsPrune, fileIn, fileOut);
    if (dic->parsed()) handleDic(maxExecutionTime, fileIn, fileOut);
    if (dicp->parsed()) handleDicP(maxExecutionTime, fileIn, fileOut,lunghezzaPrefisso);
    if (md->parsed()) handleMD(maxExecutionTime, mdFiles, fileOut);
    if (diag->parsed()) handleDiag(maxExecutionTime, diagDiagnosis, fileIn);
    if (abd->parsed()) handleAbd(maxExecutionTime, abdDiagnosis, fileIn,fileOut);
    if (abdo->parsed()) handleAbdO(maxExecutionTime, abdoDiagnosis, fileIn,fileOut);
    if (abds->parsed()) handleAbdS(maxExecutionTime, dicBond, fileIn,fileOut);
    Logger::end();
    return 0;*/
}



void creaSpaziO(std::string inFile,std::string offline,std::string projcetName,std::string bsObservation ,std::string bsOutObservation,std::string bsBond,std::string nomeO,std::string nomeS,std::string nome ,std::string outFile, bool observation,bool scenario,bool saveFile,bool label,bool prune,long maxExecutionTime){
        std::cout << "Insert network's name \n";
                                               std::cin >> inFile;
                                               std::filesystem::path sourceFile = inFile;
                                               std::filesystem::path targetParent = offline+projcetName;
                                               copyfile( inFile , targetParent,projcetName);
                                            observation = menuYN("Do you want to bond behavioral space with observation?");
                                               if (observation) {
                                                   std::cout << "Insert observation's name \n";
                                                   std::cin >> bsObservation;
                                               }
                                               if(!observation){
                                                   scenario = menuYN("Do you want to bond behavioral space with scenario?");
                                                   if (scenario) {
                                                       std::cout << "Insert scenario's name \n";
                                                       std::cin >> bsBond;
                                                   }
                                               }
                                                label = menuYN("Do you want to decorate behavioral space before pruning ?");
                                                prune = menuYN("Do you want to prune the behavioral space ?");
                                                saveFile = menuYN("Do you want to save behavioral space ?");
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
                                               saveFile = menuYN("Do you want to save behavioral space decorated?");
                                               std::string decbh;
                                               if (saveFile) {
                                                   nome="bsd_";
                                                   decbh=offline+projcetName+"/"+nome+projcetName+nomeS+nomeO+".json";
                                              }
                                              handleLBS(maxExecutionTime, prune, outFile, decbh);
                                              std::cout << "Dictionary creation... \n";
                                              saveFile = menuYN("Do you want to save the dictionary ?");
                                               std::string dic;
                                              if (saveFile) {
                                                  dic=offline+projcetName+"/"+"dic_"+projcetName+".json";
                                              }
                                              handleDic(maxExecutionTime, decbh, dic,false);
                                             
    }

int menuFirst(){
    int choice;
    std::cout << std::endl
             << " 1 - Offline Diagnosis.\n"
             << " 2 - Online Diagnosis.\n"
             << " 3 - Exit.\n"

             << " Enter your choice and press enter: ";
   std::cin >> choice;
    return choice;
}
int menuSecond(){
    int choice;
    std::cout << std::endl
              << " 1 - Load a project.\n"
              << " 2 - Create New Project.\n"
              << " 3 - Back.\n";
    std::cin >> choice;
        return choice;
    
}
int loadOffline(){
    int choice;
   std::cout << std::endl
             << " 1 - Load a Behavioral Space .\n"
             << " 2 - Load Decorate Behavioral Space.\n"
             << " 3 - Load Dictionary.\n"
             << " 4 - Merges two or more dictionaries into a single one.\n"
             << " 5 - Create a Behavioral Space from a Observation.\n"
             << " 6 - Create a Behavioral Space from a Given Scenario.\n"
             << " 7 - Search a Observation in a Dictionary.\n"
             << " 8 - Exit.\n"
             << " Enter your choice and press return: ";
    std::cin >> choice;
    return choice;

    }

void readFileInProject(bool online, std::string projName, int space){
    std::string path;
    std::size_t psub=projName.find("/");
    std::string sub=projName.substr(psub+1) ;
   
    switch (space) {
    case 1:{
        path=path+projName+"/bs_"+sub+".json";
        auto bs = BehavioralSpaceIO::loadOffline(path);
      
      
        break;
    }
    case 2:
        {
            path=path+projName+"/bsd_"+sub+".json";
        auto lbh = BehavioralSpaceIO::loadOffline(path);
           
            break;
        }
    case 3:
        {
        path=path+projName+"/dic_"+sub+".json";
         auto dictionary = DictionaryIO::loadOffline(path);
        break;
        }
    default:
        ;
    }
    
}
int newOffline(){
    int choice;
    std::cout <<"creata"<< std::endl;
    std::cout << std::endl
              << " 0 - Compute a Dictionary(BS) from a given Network.\n"
              << " 1 - Compute a Behavioral Space (BS) from a given Network.\n"
              << " 2 - Labels an input Behavioral Space.\n"
              << " 3 - Compute a Dictionary from a given Labeled Behavioral Space.\n"
              << " 4 - Merges two or more dictionaries into a single one.\n"
              << " 5 - Diagnoses a problem, given a dictionary and an observation.\n"
              
              << " 6 - Back.\n"
              << " Enter your choice and press return: ";
    std::cin >> choice;
    return choice;

}

int newOnline(){
    int choice;
    std::cout << std::endl
              << " 0 - Compute a Dictionary(BS) from a given Network.\n"
              << " 1 - Compute a Behavioral Space (BS) from a given Network.\n"
              << " 2 - Labels an input Behavioral Space.\n"
              << " 3 - Compute a Dictionary from a given Labeled Behavioral Space.\n"
              << " 4 - Compute a Dictionary from a given Labeled Behavioral Space and depth.\n"
              << " 5 - Merges two or more dictionaries into a single one.\n"
              << " 6 - Diagnoses a problem, given a dictionary and an observation.\n"
              << " 7 - Temporal abduce from a given dictionary and an observation.\n"
              << " 8 - Extende Partial dictionary from obs.\n"
              << " 9 - Extende Partial dictionary from scenario.\n"
              << " 10 - Exit.\n"
              << " Enter your choice and press return: ";
    std::cin >> choice;
    return choice;
}
int loadOnline(){
    
    int choice;
    std::cout << std::endl
             << " 1 - Load a Behavioral Space .\n"
             << " 2 - Load Decorate Behavioral Space.\n"
             << " 3 - Load Dictionary.\n"
             << " 4 - Merges two or more dictionaries into a single one.\n"
             << " 5 - Create a Behavioral Space from a Observation.\n"
             << " 6 - Create a Behavioral Space from a Given Scenario.\n"
             << " 7 - Search a Observation in a Dictionary.\n"
             << " 8 - Exit.\n"
             << " Enter your choice and press return: ";
   std::cin >> choice;
    return choice;
}


std::vector<std::string> get_directories(const std::string& s) {
    std::vector<std::string> r;
    for(auto& p : std::filesystem::directory_iterator(s))
        if(p.status().type() == std::filesystem::file_type::directory)
            r.push_back(p.path().string());
    return r;
}


bool path_exists(const std::string& path_src) {
    const  std::filesystem::path path = path_src;
    return exists(path);
}


/*
std::string get_last_chunk(const std::string &str, const char *delimiter) {
    char to_chunk[str.size()+1];
    strcpy(to_chunk, str.c_str());
    std::vector<std::string> chunks;

    char * pch;
    pch = strtok(to_chunk, delimiter);
    while (pch != nullptr){
        chunks.push_back(pch);
        pch = strtok(nullptr, delimiter);
    }
    return chunks[chunks.size()-1];
}
bool path_exists(const std::string& path_src) {
    const path path = path_src;
    return exists(path);
}

void read_session( const std::string &path) {
    if (!path_exists(path)) {
        std::cout << "La cartella indicata non esiste" << std::endl;
        return;
    }

    std::string net_name = get_last_chunk(path, "/");
    std::string net_path = path + "/network_" + net_name + ".json";

    //container.networks[net_name] = {};
    //container.networks[net_name].net = network_reader(net_path);
  
    std::string joined_path = path + "/joined_" + net_name + ".json";
    //container.networks[net_name].joined = dictionary_reader(joined_path);

    std::string dir_path = path + "/MONO";
    read_data( net_name, dir_path, MONO);

    dir_path = path + "/OBS";
    if (path_exists(dir_path)) {
        std::vector<std::string> dirs = get_directories(dir_path);

        for (auto &d : dirs)
            read_data( net_name, d, OBS);
    }

    dir_path = path + "/SCEN";
    if (path_exists(dir_path)) {
        std::vector<std::string> dirs = get_directories(dir_path);

        for (auto& d : dirs)
            read_data( net_name, d, SCEN);
    }
}
*/

bool menuYN( std::string question){
    char choice;
    while(true)
    {
        std::cout<<question+"(Y/N)\n"<<std::endl;
        std::cin >> choice;
        if(choice == 'Y'|| choice =='y'){
            return true;
        }else if(choice =='N'||choice =='n'){
            return false;
        }
    }


}
void copyfile( std::filesystem::path sourceFile ,    std::filesystem::path targetParent,std::string name){
    
    auto target = targetParent.u8string()+ "/"+ name+".json"; // sourceFile.filename() returns "sourceFile.ext".

    try // If you want to avoid exception handling, then use the error code overload of the following functions.
    {
        std::filesystem::create_directories(targetParent); // Recursively create target directory if not existing.
        std::filesystem::copy_file(sourceFile, target, std::filesystem::copy_options::overwrite_existing);
    }
    catch (std::exception& e) // Not using fs::filesystem_error since std::bad_alloc can throw too.
    {
        std::cout << e.what();
    }
}

/*
void read_data( const std::string &net_name, const std::string &dir_path, int mode) {
    if (path_exists(dir_path)) {
        //Container::net_data std::data = {};
        std::string bs_path = dir_path + "/bs_" + net_name + ".json";
        if (path_exists(bs_path))
            //data.bs = behavioural_space_reader(bs_path);

        std::string d_bs_path = dir_path + "/d_bs_" + net_name + ".json";
        if (path_exists(bs_path))
            //std::data.d_bs = behavioural_space_reader(d_bs_path);

            std::string dict_path = dir_path + "/dict_" + net_name + ".json";
        if (path_exists(bs_path))
            //data.dict = dictionary_reader(dict_path);

        //container.networks[net_name].net_datas[mode].push_back(move(data));

            std::string rec_name = get_last_chunk(dir_path, "/");
        std::string rec_path = dir_path + "/obs_" + rec_name + ".json";
        if (path_exists(rec_path)) {
           // container.observations.push_back(recognizer_reader(rec_path, "Observation"));
            //std::data.recognizer_index = static_cast<int>(container.observations.size() - 1);
        }

        rec_name = get_last_chunk(dir_path, "/");
        rec_path = dir_path + "/scen_" + rec_name + ".json";
        if (path_exists(rec_path)) {
            //container.scenarios.push_back(recognizer_reader(rec_path, "Scenario"));
            //data.recognizer_index = static_cast<int>(container.scenarios.size() - 1);
        }
    }
}


*/
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
            auto constructedObservation = DFAConstructor::constructObservation(bs, maxExecutionTime);
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
            auto constructedObservation = DFAConstructor::constructObservation(bs, maxExecutionTime);
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
        if (maxExecutionTime) Logger::log("Time remaining to reach limit (ms): " + std::to_string(startingMaxExecutionTime - elapsed));

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

                int it=1;

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
void handleAbdO(long time, std::vector<std::string> &observations, const std::string &inFile,const std::string &outFile) {

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

                auto result = DFAConstructor::extendDictionaryFromObs(dictionary,osservazione, maxExecutionTime);
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
