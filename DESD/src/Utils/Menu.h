#ifdef menu_c
#define EXTERN
#else
#define EXTERN extern
#endif


EXTERN std::string offline="offline/";
EXTERN std::string nomeO ="",nomeS ="", nome="";
EXTERN std::string bsObservation ="", bsBond ="", bsOutObservation ="",inFile ="", outFile="",dicBond="";
EXTERN bool observation=false;
EXTERN  bool scenario=false;
EXTERN bool saveFile=false;
EXTERN   bool label=false;
EXTERN bool prune=false;
EXTERN std::string projcetName="";
EXTERN std::string decbh="";
EXTERN  std::string online="online/";
EXTERN int depth=0;
EXTERN std::string nameSpace="";
EXTERN std::string nameObs="";

#import "Computation.h"
#import "Generate.h"
class Menu{
private:
    
     
    public:
   
    
    
   
    // menu iniziale scelta iniziale
     static int menuFirst(){
       
        int choice;
        std::cout << std::endl
                 << " 1 - Offline Diagnosis.\n"
                 << " 2 - Online Diagnosis.\n"
                 << " 3 - generate.\n"
                 << " 4 - Exit.\n"

                 << " Enter your choice and press enter: ";
       std::cin >> choice;
        return choice;
    }
    
    //menu scelta creazione o caricamento
    static int menuSecond(){
        int choice;
        std::cout << std::endl
                  << " 1 - Load a project.\n"
                  << " 2 - Create New Project.\n"
                  << " 3 - Back.\n";
        std::cin >> choice;
            return choice;
        
    }
   
    
    //menu offline 
    static int loadOffline(){
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
    
    
    static int newOffline(){
        int choice;
        std::cout <<"creata"<< std::endl;
        std::cout << std::endl
                  << " 0 - Compute a Dictionary(BS) from a given Network.\n"
                  << " 1 - Compute a Behavioral Space (BS) from a given Network.\n"
                  << " 2 - Labels an input Behavioral Space.\n"
                  << " 3 - Compute a Dictionary from a given Labeled Behavioral Space.\n"
                  << " 4 - Merges two or more dictionaries into a single one.\n"
                  << " 5 - Diagnoses a problem, given a dictionary and an observation.\n"
                  << " 6 - Compute a Behavioral Space (BS) bonded by observataion.\n"
                    << " 7 - Compute a Behavioral Space (BS) bonded by scenario.\n"
                 << " 8 - Compute a Dictionary from a given Labeled Behavioral Space.\n"
                  << " 9 - Back.\n"
                  << " Enter your choice and press return: ";
        std::cin >> choice;
        return choice;

    }

   static int newOnline(){
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
                  << " 10 - Create observations.\n"
                  << " 11 - Exit.\n"
                  << " Enter your choice and press return: ";
        std::cin >> choice;
        return choice;
    }
    
    
    static int loadOnline(){
        
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
    
    
    static bool menuYN( std::string question){
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
    
    //MENU PRINCIPALE PER CARICARE PROGRETTI OFFLINE
    
    static void MenuOfflineCaricamento(std::vector<std::string> directories){
        while(true){
                int choice3;
                int h=1;
                bool verifica=false;
                //VISUALIZZA NOMI
                for( auto e :directories){
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
                    std::string namePrj=directories[choice3-1];
                    int choice8;
                    do {
                        choice8=Menu::loadOffline();
                        
                        //CARICA SPAZI COMPORTAMENTALI
                        if(choice8>=1 and choice8<=3) {
                            File::readFileInProject(false,  namePrj, choice8);
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
    }
    
    //MENU PRINCIPALE PER CARICARE PROGRETTI ONLINE
    static void MenuOnlineCaricamento(std::vector<std::string> directories){
        while(true){
            int h=1;
            int choice5;
            for( auto e :directories){
                std::string s=std::to_string(h);
                std::cout <<s +" - "+e+".\n"<< std::endl;
                h++;
            }
            std::string s=std::to_string(h);
            std::cout << s +"  - indietro.\n"<< std::endl;
            std::cin >> choice5;
            if( choice5>0 &&choice5<=h-1){
                do {
                    choice5=Menu::loadOnline();
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
    }
    
    //MENU PRINCIPALE PER CARICARE NUOVA RETE OFFLINE
    static void MenuCaricamentoOfflineNuovaRete(bool observation,bool scenario,long maxExecutionTime){
        std::string temp1 ,temp2;
        std::vector<std::string> inFiles;
        std::vector<std::string> observations;
        
        std::string bsObservation, bsBond, bsOutObservation,inFile, outFile,dicBond;
        bool prune, label,saveFile;
        std::cout <<"Insert the name of the project"<< std::endl;
        std::cin >> projcetName;
        
        auto percorso=offline+projcetName;
        
        
        //VERIFICa UNIVOCITA NOME
        if(!  File::path_exists(percorso)){
            mkdir( percorso.c_str(),S_IRWXU);
            //MENU OFFLINE
            int choice4;
            do {
                choice4=Menu::newOffline();
                std::string nomeO="",nomeS="", nome="";
                switch (choice4){
                        
                        //CALCOLO SPAZIO COMPORTAMENTALE OFFLINE
                    case 0:{
                      
                        if(behavioralSpaceOffline( maxExecutionTime)){
                             decoratedBehavioralSpaceOffline( maxExecutionTime);
                                createDictionaryOffline(  maxExecutionTime);
                        }
                        
                       
                     break;
                }
                        
                case 1:{
                    behavioralSpaceOffline( maxExecutionTime);
                   
                    
                    break;
                    }
                    case 2:{
                        decoratedBehavioralSpaceOffline( maxExecutionTime);
                        break;
                        }
                    case 3:{
                        createDictionaryOffline( maxExecutionTime);
                        break;
                    }
                    case 4:{
                        std::cout << "Insert the name of the first dictionary \n";
                        std::cin >> temp1;
                        inFiles.push_back(temp1);
                        std::cout << "Insert the name of the first dictionary \n";
                        std::cin >> temp1;
                        inFiles.push_back(temp1);
                        saveFile = Menu::menuYN("Do you want to save the dictionary's merged?");
                        if (saveFile) {
                            std::cout << "Insert name \n";
                            std::cin >> outFile;
                        }
                        Computation::handleMD(maxExecutionTime, inFiles, outFile);
                        break;
                    }
                    case 5:{
                        std::cout << "Insert the name of the dictionary \n";
                        std::cin >> inFile;
                        std::cout
                                << "Insert the name of the observation \n(each observation in the sequence must be  separte with blank character) \n ex: o1 o2 o3\n";
                        std::cin >> temp1;
                        getline(std::cin, temp2);
                        observations = Computation::convertObsStringToVector(temp1 + temp2);
                        Computation::handleDiag(maxExecutionTime, observations, inFile);
                        break;
                    }
                        
                case 6:{
                behavioralSpaceOfflineObservation( maxExecutionTime);
                                               bool dic=Menu::menuYN("Do you want create this dictionary?");
                                              if(dic){
                                                  decoratedBehavioralSpaceOfflineObservation_Scenario(nameSpace, nameObs, maxExecutionTime);
                                                  createDictionaryOfflineObservation( nameObs, maxExecutionTime);
                                                  
                                              }
                  
                   
                   break;
                   }
                        
                        case 7:{
                         behavioralSpaceOfflineScenario( maxExecutionTime);
                             bool dic=Menu::menuYN("Do you want create this dictionary?");
                            if(dic){
                                decoratedBehavioralSpaceOfflineObservation_Scenario(nameSpace, nameObs, maxExecutionTime);
                                createDictionaryOfflineObservation( nameObs, maxExecutionTime);
                                
                            }
                         
                         break;
                         }
                        
                        case 8:{
                            std::string temp33;
                            std::cout << "Insert";
                            std::cin >> temp33;
                            
                            Computation::handleDic(maxExecutionTime, temp33, "9dic,json",false);
                                               
                                               break;
                                           }
                    case 9:{
                        std::cout << "End of Program.\n";
                        break;
                    }
                    default:{
                            std::cout << "Not a Valid Choice. \n"
                                      << "Choose again.\n";
                    }
                }
            } while (choice4!=9);
        }
        else{
            
            std::cout <<"Nome gia esistente"<< std::endl;

        }
        
    }
    
    //MENU PRINCIPALE PER CARICARE PROGRETTI ONLINE
    
    static void MenuCaricamentoOnlineNuovaRete(bool observation,bool scenario,long maxExecutionTime){
       std::string temp1 ,temp2;
       std::vector<std::string> inFiles;
       std::vector<std::string> observations;
      std::cout <<"Insert the name of the project"<< std::endl;
      std::cin >> projcetName;
     
      auto percorso=online+projcetName;
        
      //Verifica univocità nome
      if(! File::path_exists(percorso)){
          mkdir( percorso.c_str(),S_IRWXU);
          std::cout <<"creata"<< std::endl;
          int choice6=0;
         do {
               choice6=Menu::newOnline();
              
               switch (choice6) {
                  case 0:{
                      if(behavioralSpaceOnline(maxExecutionTime)){
                         decoratedBehavioralSpaceOnline( maxExecutionTime);
                          createDictionaryOnline( maxExecutionTime);
                      }
                      break;
                  }
                  case 1:{
                      behavioralSpaceOnline(maxExecutionTime);
                      break;
                  }
                  case 2:{
                      decoratedBehavioralSpaceOnline( maxExecutionTime);
                          break;
                      }
                  case 3:{
                         createDictionaryOnline( maxExecutionTime);

                          break;
                  }
                  case 4:{
                      createDictionaryPrefixOnline(maxExecutionTime);
                          break;
                  }
                  case 5:{
                          std::cout << "Insert the name of the first dictionary \n";
                          std::cin >> temp1;
                          inFiles.push_back(temp1);
                          std::cout << "Insert the name of the first dictionary \n";
                          std::cin >> temp1;
                          inFiles.push_back(temp1);
                          saveFile = Menu::menuYN("Do you want to save the dictionary's merged?");


                          if (saveFile) {
                              std::cout << "Insert name \n";
                              std::cin >> outFile;
                          }
                          Computation::handleMD(maxExecutionTime, inFiles, outFile);
                          break;
                  }
                  case 6:{
                          std::cout << "Insert the name of the dictionary \n";
                          std::cin >> inFile;
                          std::cout
                                  << "Insert the name of the observation \n(each observation in the sequence must be  separte with blank character) \n ex: o1 o2 o3\n";
                          std::cin >> temp1;

                          getline(std::cin, temp2);


                          observations = Computation::convertObsStringToVector(temp1 + temp2);

                          Computation::handleDiag(maxExecutionTime, observations, inFile);
                  
                      break;
                      
                      }
                  case 7:{
                          std::cout << "Insert the name of the dictionary \n";
                          std::cin >> inFile;
                          std::cout
                                  << "Insert the name of the observation \n(each observation in the sequence must be  separte with blank character) \n ex: o1 o2 o3\n";
                          std::cin >> temp1;
                          getline(std::cin, temp2);
                          observations = Computation::convertObsStringToVector(temp1 + temp2);
                          saveFile = Menu::menuYN("Do you want to save the abduction?");


                          if (saveFile) {
                              std::cout << "Insert name \n";
                              std::cin >> outFile;
                          }
                          Computation::handleAbd(maxExecutionTime, observations, inFile, outFile);
                          break;
                  }
                  case 8:{
                          std::cout << "Insert the name of the dictionary to extend \n";
                          std::cin >> inFile;
                          std::cout
                                  << "Insert the name of the observation \n(each observation in the sequence must be  separte with blank character) \n ex: o1 o2 o3\n";
                          std::cin >> temp1;
                          getline(std::cin, temp2);
                          observations = Computation::convertObsStringToVector(temp1 + temp2);
                          saveFile = Menu::menuYN("Do you want to save the extended dictionary?");


                          if (saveFile) {
                              std::cout << "Insert name \n";
                              std::cin >> outFile;
                          }

                          Computation::handleAbdO(maxExecutionTime, observations, inFile, outFile);

                          break;
                      
                  }
                  case 9:{

                          std::cout << "Insert the name of the dictionary to extend \n";
                          std::cin >> inFile;
                          std::cout << "Insert the name scenario name\n";
                          std::cin >> dicBond;

                          saveFile = Menu::menuYN("Do you want to save the extended dictionary?");


                          if (saveFile) {
                              std::cout << "Insert name \n";
                              std::cin >> outFile;
                          }
                          Computation::handleAbdS(maxExecutionTime, dicBond, inFile,
                                     outFile);//extra information about the mode and the controlle
                          break;
                      
                      
                  }
                       
                   case 10:{
                       std::vector<std::vector<std::string>> obss;
                       std::cout << "Insert network's name \n";
                       std::cin >> inFile;
                       std::set<std::string> alphabet;
                       auto network = NetworkIO::load(inFile);
                       for (const auto &comp : network->getComponents()){
                           for( const auto &tran: comp->getTransitions()){
                               if (tran->getObservabilityLabel()!=""){
                                   alphabet.insert(tran->getObservabilityLabel());

                               }
                           }
                      }
                       for (int i=0; i<10;i++){
                           auto ob=Generate::generateRandomObs(alphabet);
                           obss.push_back(ob);
                       }
                       saveFile = Menu::menuYN("Do you want to save the abduction?");

                       for (auto ob: obss){
                       
                       std::cout << "Insert the name of the dictionary \n";
                        std::cin >> inFile;
                        
                        

                        if (saveFile) {
                            std::cout << "Insert abname \n";
                            std::cin >> outFile;
                        }
                        Computation::handleAbd(maxExecutionTime, ob, inFile, outFile);
                       }
                       
                       break;
                   }
                  case 11:{
                              std::cout << "End of Program.\n";
                              break;
                  }
                  default:{
                          std::cout << "Not a Valid Choice. \n"
                                    << "Choose again.\n";
                  }
        }
                         
          }while (choice6 != 11);
    }
        else{
                                       std::cout <<"Nome gia esistente"<< std::endl;
                                   }
 }
    
    static bool behavioralSpaceOffline(long maxExecutionTime){
        
              std::cout << "Insert network's name \n";
              std::cin >> inFile;
              std::filesystem::path sourceFile = inFile;
              std::filesystem::path targetParent = offline+projcetName;
            if(File::path_exists(inFile)){
            
              File::copyfile( inFile , targetParent,projcetName);
               /*
                observation = Menu::menuYN("Do you want to bond behavioral space with observation?");
               
              if (observation) {
                  std::cout << "Insert observation's name \n";
                  std::cin >> bsObservation;
              }
                scenario=false;
              if(!observation){
                  scenario = Menu::menuYN("Do you want to bond behavioral space with scenario?");
                  if (scenario) {
                      std::cout << "Insert scenario's name \n";
                      std::cin >> bsBond;
                  }
              }
              label = Menu::menuYN("Do you want to decorate behavioral space before pruning ?");
              prune = Menu::menuYN("Do you want to prune the behavioral space ?");
                
                  nomeO="";
                  nomeS="";
                  nome="bs_";
                  
                  if(observation){
                      nomeO="_"+bsObservation+"_obs";
                  }
                  if(scenario){
                      nomeS="_"+bsBond+"_sce";
                  }
                 
                 */
                std::string nomefin;
                  nomefin=projcetName+".json";
                  outFile=offline+projcetName+"/bs_"+nomefin;
             
                label = false;
                prune = true;
                bsOutObservation="";
                bsBond="";
              Computation::handleBS(maxExecutionTime, bsObservation, bsBond, bsOutObservation, prune,
              label, inFile, outFile);
                return true;
            }
            else{
                 std::cout << "This network doesn't exist \n";
                return false;
             
        }
    }
    
    static void behavioralSpaceOfflineObservation(long maxExecutionTime){
        
              std::cout << "Insert network's name \n";
              std::cin >> inFile;
              std::filesystem::path sourceFile = inFile;
              std::filesystem::path targetParent = offline+projcetName;
            if(File::path_exists(inFile)){
            
              File::copyfile( inFile , targetParent,projcetName);
              
                  std::cout << "Insert observation's name \n";
                  std::cin >> bsObservation;
              if(!File::path_exists(bsObservation)){
              std::cout << "Observation not valid\n";
                  return     ;
                  
              }
                
              
             label = false;
                prune = true;
             std::string nomefin;
                  nomeO="";
                  nomeS="";
                  nome="bs_";
                  
                 
                      nomeO="_"+bsObservation+"_obs";
                 
                      
                  
                  nomefin=projcetName+nomeO+".json";
                  outFile=offline+projcetName+"/"+nome+nomefin;
             nameSpace=outFile;
             
             nameObs= nomeO="_"+bsObservation+"_obs";
              Computation::handleBS(maxExecutionTime, bsObservation, bsBond, bsOutObservation, prune,
              label, inFile, outFile);
                
            bool dic = Menu::menuYN("Do you want create this dictionary?");
                
                
            }
               
            
            else{
                 std::cout << "This network doesn't exist \n";
                return ;
             
        }
    }
    
    static void behavioralSpaceOfflineScenario(long maxExecutionTime){
        
              std::cout << "Insert network's name \n";
              std::cin >> inFile;
              std::filesystem::path sourceFile = inFile;
              std::filesystem::path targetParent = offline+projcetName;
            if(File::path_exists(inFile)){
            
              File::copyfile( inFile , targetParent,projcetName);
              
                  std::cout << "Insert scenario's name \n";
                  std::cin >> bsBond;
              if(!File::path_exists(offline+projcetName+bsObservation)){
              std::cout << "Observation not valid\n";
                  return     ;
                  
              }
                
              
             label = false;
                prune = true;
             std::string nomefin;
                  nomeO="";
                  nomeS="";
                  nome="bs_";
                  
                 
                      nomeO="_"+bsObservation+"_sce";
                 
                      
                  
                  nomefin=projcetName+nomeO+".json";
                  outFile=offline+projcetName+"/"+nome+nomefin;
             nameSpace=outFile;
             
             nameObs= nomeO="_"+bsBond+"_sce";
              Computation::handleBS(maxExecutionTime, bsObservation, bsBond, bsOutObservation, prune,
              label, inFile, outFile);
                
            bool dic = Menu::menuYN("Do you want create this dictionary?");
                
                
            }
               
            
            else{
                 std::cout << "This network doesn't exist \n";
                return ;
             
        }
    }
        
        static bool behavioralSpaceOnline(long maxExecutionTime){
            std::cout << "Insert network's name \n";
            std::cin >> inFile;
            std::filesystem::path sourceFile = inFile;
            std::filesystem::path targetParent = online+projcetName;
            if(File::path_exists(inFile)){
            File::copyfile( inFile , targetParent,projcetName);
                
            /*
            observation = Menu::menuYN(
                    "Do you want to bond behavioral space with observation?");
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
                std::string nomefin;
           
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
            */
                
                
            std::string nomefin;
            nomefin=projcetName+".json";
            outFile=online+projcetName+"/bs_"+nomefin;
            Computation::handleBS(maxExecutionTime, bsObservation, bsBond, bsOutObservation, false,
                     label, inFile, outFile);
               return true;
            }
            else{
                 std::cout << "This network doesn't exist \n";
                return false;
                
            }
        }
    
    static void decoratedBehavioralSpaceOffline(long maxExecutionTime){

         std::cout << "\n Decoration in progess..\n";
         prune = false;
         auto bsIn=offline+projcetName+"/"+"bs_"+projcetName+".json";
             nome="bsd_";
             decbh=offline+projcetName+"/"+nome+projcetName+nomeS+nomeO+".json";
            if(!File::path_exists(bsIn)){
                std::cout << "Beavioral space don't create yet... Creating now....\n";
                behavioralSpaceOffline( maxExecutionTime);
            }
        Computation::handleLBS(maxExecutionTime, prune, bsIn, decbh);
    }
    
                         
     static void decoratedBehavioralSpaceOnline(long maxExecutionTime){
      std::cout << "Decoration in progess..\n";
       prune = false;
       std::string decbh;
       auto bsIn=online+projcetName+"/"+"bs_"+projcetName+".json";
           nome="bsd_";
           decbh=online+projcetName+"/"+nome+projcetName+nomeS+nomeO+".json";
         
         if(!File::path_exists(bsIn
                               )){
             std::cout << "Beavioral space doesn't create yet... Creating now....\n";
             behavioralSpaceOnline( maxExecutionTime);
         }
       
       
         Computation::handleLBS(maxExecutionTime, prune, bsIn, decbh);
  }
    
    
    
    static void decoratedBehavioralSpaceOfflineObservation_Scenario(std::string nameSpace,std::string nameObs,long maxExecutionTime){

           std::cout << "Decoration in progess..\n";
           
           
               nome="bsd_";
               decbh=offline+projcetName+"/"+nome+projcetName+nameObs+".json";
              
              auto bsIn=nameSpace;
                prune=false;
          Computation::handleLBS(maxExecutionTime, prune, bsIn, decbh);
      }
      
                           
      
    static void createDictionaryOffline(long maxExecutionTime){

         std::cout << "\n Dictionary creation... \n";
         
          std::string dic;
          auto dbsIn=offline+projcetName+"/"+"bsd_"+projcetName+".json";
              dic=offline+projcetName+"/"+"dic_"+projcetName+".json";
         if(!File::path_exists(dbsIn)){
            
             
             
                 decoratedBehavioralSpaceOffline(maxExecutionTime);
             

         }
          
        
          Computation::handleDic(maxExecutionTime, dbsIn, dic,false);
         }
            
    
    
        static void createDictionaryOfflineObservation(std::string nameObs,long maxExecutionTime){

         std::cout << "Dictionary creation... \n";
         
          std::string dic;
          auto dbsIn=offline+projcetName+"/"+"bsd_"+projcetName+nameObs+".json";
              dic=offline+projcetName+"/"+"dic_"+projcetName+nameObs+".json";
         if(!File::path_exists(dbsIn)){
            
             
             
                 decoratedBehavioralSpaceOffline(maxExecutionTime);
             

         }
          
        
          Computation::handleDic(maxExecutionTime, dbsIn, dic,false);
         }

    
    

     static void createDictionaryOnline(long maxEcecutionTime){
      std::cout << "Dictionary creation... \n";
      
      std::string dic;
         
             dic=online+projcetName+"/"+"dic_"+projcetName+".json";
         
         auto dbsIn=online+projcetName+"/"+"bsd_"+projcetName+".json";
         if(!File::path_exists(dbsIn)){
             std::cout << " Decorated Beavioral space  doesn't create yet... Creating now....\n";
                        
             decoratedBehavioralSpaceOnline(maxEcecutionTime);
         }
        
             
         
     
         Computation::handleDic(maxEcecutionTime, dbsIn, dic,true);
      
  }
    
    
    static void createDictionaryPrefixOnline(long maxExecutionTime){
       
        std::cout << "Insert depth of dictionary \n";
        std::cin >> depth;
        auto bsIn=online+projcetName+"/"+"bs_"+projcetName+".json";
        std::string out_string;
        std::stringstream ss;
        ss << depth;
     auto dicOut=online+projcetName+"/"+"dic_"+projcetName+"_prefix"+ss.str()+".json";
        std::cout << "Computation \n";
             Computation::handleDicP(maxExecutionTime, bsIn, dicOut, depth);
    }
    
    
    
};

