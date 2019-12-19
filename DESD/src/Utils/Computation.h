
 class Computation {
    
    
public:
    static std::vector<std::string> convertObsStringToVector(std::string stringa){
         std::stringstream ss(stringa);
         std::istream_iterator<std::string> begin(ss);
         std::istream_iterator<std::string> end;
         std::vector<std::string> vstrings(begin, end);
         std::copy(vstrings.begin(), vstrings.end(), std::ostream_iterator<std::string>(std::cout, "\n"));
         return vstrings;
     }
     
     
     
     //creazione spazio comportamentale a partire dalla rete
     static void handleBS(long time, const std::string &bsObservation, const std::string &bsBond, const std::string &bsOutObservation, bool prune, bool label,
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
                 auto constructedObservation = DFAConstructor::constructObservation(bs, true,maxExecutionTime);
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




    static void handleBSPref(long time, const std::string &bsObservation, const std::string &bsBond, const std::string &bsOutObservation, bool prune, bool label,
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
     static void handleLBS(long time, bool prune, const std::string &inFile, const std::string &outFile) {
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
    static void handleDic(long time, const std::string &inFile, const std::string &outFile, bool online) {
         auto lbs = BehavioralSpaceIO::load(inFile);
        
         std::shared_ptr<long> maxExecutionTime = nullptr;
         long startingMaxExecutionTime = 0;
         if (time > 0) {
             maxExecutionTime = std::make_shared<long>(time);
             startingMaxExecutionTime = time;
         }
         auto startingTime = std::chrono::high_resolution_clock::now();
        //auto dictionary = DFAConstructor::constructDictionary(lbs,online, maxExecutionTime);
        
        try {
             auto dictionary = DFAConstructor::constructDictionary(lbs,online, maxExecutionTime);
             auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
             
             std::cout << "Time elapsed (ms): " + std::to_string(elapsed);
             Logger::log("Time elapsed (ms): " + std::to_string(elapsed));
             if (maxExecutionTime) Logger::log("Time remaining to reach limit (ms): " + std::to_string(startingMaxExecutionTime - elapsed));
             DictionaryIO::save(outFile, *dictionary);
         } catch (std::runtime_error &e) {
             auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
             Logger::err("Time elapsed (ms): " + std::to_string(elapsed));
         }
     }


     //creazione di un prefisso del dizionario
    static void handleDicP(long time, const std::string &inFile, const std::string &outFile, int lunghezzaPrefisso) {
         
         
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
    static void handleMD(long time, std::vector<std::string>& inFiles, const std::string &outFile) {
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
   static  void handleDiag(long time, std::vector<std::string> &diagnosis, const std::string &inFile) {

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
    static void handleAbd(long time, std::vector<std::string> &observations, const std::string &inFile,const std::string &outFile) {

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
     static void handleAbdO(long time, std::vector<std::string> &observations, const std::string &inFile,const std::string &outFile) {

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


    static void handleAbdS(long time, const std::string &dicBond, const std::string &inFile,const std::string &outFile) {

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

};
