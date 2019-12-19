#include "stdIncludes.h"
#include <chrono>
#include <CLI11.hpp>
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


void handleBS(long maxExecutionTime, const std::string &bsObservation, const std::string &bsBond, const std::string &bsOutObservation, bool prune, bool label,
              const std::string &inFile, const std::string &outFile);
void handleLBS(long maxExecutionTime, bool, const std::string &, const std::string &);
void handleDic(long maxExecutionTime, const std::string &, const std::string &);
void handleDicP(long maxExecutionTime, const std::string &, const std::string &, int lunghezzaPrefisso);
void handleMD(long maxExecutionTime, std::vector<std::string>& inFiles, const std::string &outFile);
void handleDiag(long maxExecutionTime, std::vector<std::string> &diagnosis, const std::string &inFile);
void handleAbd(long maxExecutionTime, std::vector<std::string> &observations, const std::string &inFile,const std::string &outFile);

void handleAbdO(long maxExecutionTime, std::vector<std::string> &observations, const std::string &inFile, const std::string &outFile);
void handleAbdS(long maxExecutionTime, const std::string &dicBond, const std::string &inFile, const std::string &outFile);


int main(int argc, char *argv[]) {






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
    return 0;
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
void handleDic(long time, const std::string &inFile, const std::string &outFile) {
    auto lbs = BehavioralSpaceIO::load(inFile);
    std::shared_ptr<long> maxExecutionTime = nullptr;
    long startingMaxExecutionTime = 0;
    if (time > 0) {
        maxExecutionTime = std::make_shared<long>(time);
        startingMaxExecutionTime = time;
    }
    auto startingTime = std::chrono::high_resolution_clock::now();
    try {
        auto dictionary = DFAConstructor::constructDictionary(lbs, maxExecutionTime);
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
    auto lbs = BehavioralSpaceIO::load(inFile);
    std::shared_ptr<long> maxExecutionTime = nullptr;
    long startingMaxExecutionTime = 0;
    if (time > 0) {
        maxExecutionTime = std::make_shared<long>(time);
        startingMaxExecutionTime = time;
    }
    auto startingTime = std::chrono::high_resolution_clock::now();
    try {
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