#ifndef AUTOMATAMANAGER_LOADDICTIONARY_H
#define AUTOMATAMANAGER_LOADDICTIONARY_H

#include "../stdIncludes.h"
#include <nlohmann/json.hpp>
#include "../Data/Dictionary.h"
#include "../Data/BehavioralSpace.h"
#include "../Data/BehavioralTransition.h"

class DictionaryIO {
public:
    static std::shared_ptr<Dictionary> load(const std::string &path) {
        nlohmann::json j;
        if (path.empty()) std::cin >> j;
        else {
            std::ifstream i(path);
            i >> j;
        }

        BehavioralTransition::nextId = 0;
        DictionaryState::nextId = 0;

        // Controllo delle meta-informazioni
        auto meta = j["meta"];
        if (meta["type"] != "dictionary") throw std::invalid_argument("Trying to load a non-Dictionary JSON");
        int version = meta["version"];
        if (version > 1) throw std::invalid_argument("Can't load JSON Dictionary with version > 1");

        auto jDictionary = j["dictionary"];

        auto jTransitions = jDictionary["transitions"];

        BehavioralTransition::nextId = 0;
        std::vector<std::shared_ptr<NFATransitionInterface>> transitions;

        for (auto jTransition : jTransitions) {
            unsigned int id = jTransition["id"];
            std::string label = jTransition["label"];
            transitions.push_back(std::make_shared<BehavioralTransition>("", id, "", label));
        }
        nlohmann::json jBTransitions = jDictionary["behavioralTransitions"];
        BehavioralTransition::nextId = 0;
        std::vector<std::shared_ptr<NFATransitionInterface>> btransitions;

        for (nlohmann::json jBTransition : jBTransitions) {
            std::string tName = jBTransition["name"];
            unsigned int tId = jBTransition["id"];
            std::string relevancyLabel, observabilityLabel;
            if (jBTransition.count("relevancy") > 0)
                relevancyLabel = jBTransition["relevancy"];
            if (jBTransition.count("observability") > 0)
                observabilityLabel = jBTransition["observability"];
            btransitions.push_back(
                    std::make_shared<BehavioralTransition>(tName, tId, relevancyLabel, observabilityLabel));
        }

        std::shared_ptr<std::vector<std::shared_ptr<transitionsInfo>>>tInfo( new std::vector<std::shared_ptr<transitionsInfo>>());

        nlohmann::json  jTinfo=jDictionary["tinfo"];
        for (nlohmann::json jTi : jTinfo) {
            unsigned int tiId= jTi["id"];
            unsigned int tiS= jTi["start"];
            unsigned int tiD= jTi["destination"];
            std::string label;
            if (jTi.count("label") > 0)
                label = jTi["label"];
            auto tI = std::make_shared<transitionsInfo>(tiId,label,tiS);
            tI->setDestination(tiD);

            tInfo->push_back(tI);
        }
        nlohmann::json jStates = jDictionary["states"];
        std::vector<std::shared_ptr<DictionaryState>> states;

        for (auto jState : jStates) {
            bool starting = jState.count("starting") > 0;
            bool final = jState.count("final") > 0;
            unsigned int id = jState["id"];
            std::vector<std::vector<unsigned int>> pairs = jState["pairs"];
            auto jIn = jState["in"];
            auto jOut = jState["out"];

            std::vector<std::shared_ptr<NFATransitionInterface>> in, out;
            findTransitions(transitions, jIn, in);
            findTransitions(transitions, jOut, out);

            auto jDiagnosis = jState["diagnosis"];
            std::set<std::set<std::string>> diagnosis;
            for (auto jSingleDiagnosis : jDiagnosis) {
                std::set<std::string> singleDiagnosis;
                for (std::string diag : jSingleDiagnosis) singleDiagnosis.insert(diag);
                diagnosis.insert(singleDiagnosis);




            }

            auto jNfaStates = jState["nfaStates"];
            std::vector<std::shared_ptr<BehavioralState>> nfaStates;

            for (nlohmann::json jNfaState : jNfaStates) {
                bool starting = jNfaState.count("starting") > 0 && jState["starting"];
                bool final = jNfaState.count("final") > 0 && jNfaState["final"];
                unsigned int id = jNfaState["id"];
                nlohmann::json jComponentsState = jNfaState["componentsState"];
                nlohmann::json jLinksState = jNfaState["linksState"];
                nlohmann::json jIn = jNfaState["in"];
                nlohmann::json jOut = jNfaState["out"];
                int observationState = -1;
                if (jNfaState.count("observationState") > 0) observationState = jNfaState["observationState"];

                std::map<std::string, std::string> componentsState;
                for (nlohmann::json jComponentState : jComponentsState) {
                    std::string component = jComponentState["component"];
                    std::string state = jComponentState["state"];
                    componentsState.insert({component, state});

                }

                std::map<std::string, std::string> linksState;
                for (nlohmann::json jLinkState : jLinksState) {
                    std::string link = jLinkState["link"];
                    std::string value = jLinkState["value"];
                    linksState.insert({link, value});
                }

                std::vector<std::shared_ptr<BehavioralTransition>> in, out;
                std::vector<std::shared_ptr<BehavioralTransition>> tr;
                for( auto t: btransitions){
                    auto s=t.get();

                    auto newTransition = std::make_shared<BehavioralTransition>(s->getName(),
                                                                                s->getRelevancyLabel(),
                                                                                s->getObservabilityLabel());
                    newTransition->id=s->getId();

                    

                    tr.push_back(newTransition);

                }
                findBehavioralTransitions(tr, jIn, in);
                findBehavioralTransitions(tr, jOut, out);

                if (jNfaState.count("relevancyLabels") > 0) {
                    std::set<std::string> relevancyLabels;
                    for (const std::string label : jNfaState["relevancyLabels"]) {
                        relevancyLabels.insert(label);
                    }

                    nfaStates.push_back(
                            std::make_shared<BehavioralState>(componentsState, linksState, starting, final, in, out,
                                                              relevancyLabels, id));
                } else
                    nfaStates.push_back(
                            std::make_shared<BehavioralState>(componentsState, linksState, starting, final, in, out,
                                                              id));

                nfaStates.back()->observationState = observationState;
            }
            auto dicState=std::make_shared<DictionaryState>(final, starting, in, out, diagnosis, pairs, id);
            std::vector<std::shared_ptr<BehavioralState>> nfaStates2;
            for (auto st :nfaStates){
                auto s= std::make_shared<BehavioralState>();
                s=st;
                nfaStates2.push_back(s);

/*
                nfaStates2.push_back(s);
*/


            }
               
            dicState->setNfaState2(nfaStates2);
            states.push_back(dicState);




/*
            states.push_back(std::make_shared<DictionaryState>(final, starting, in, out, diagnosis, pairs, id));
*/
        }

        nlohmann::json jbhStates = jDictionary["bhState"];

        std::vector<std::shared_ptr<NFAStateInterface>>  bhState;

        for (nlohmann::json jbhState : jbhStates) {
            bool starting = jbhState.count("starting") > 0 && jbhState["starting"];
            bool final = jbhState.count("final") > 0 && jbhState["final"];
            unsigned int id = jbhState["id"];
            nlohmann::json jComponentsState = jbhState["componentsState"];
            nlohmann::json jLinksState = jbhState["linksState"];
            nlohmann::json jIn = jbhState["in"];
            nlohmann::json jOut = jbhState["out"];
            int observationState = -1;
            if (jbhState.count("observationState") > 0) observationState = jbhState["observationState"];

            std::map<std::string, std::string> componentsState;
            for (nlohmann::json jComponentState : jComponentsState) {
                std::string component = jComponentState["component"];
                std::string state = jComponentState["state"];
                componentsState.insert({component, state});

            }

            std::map<std::string, std::string> linksState;
            for (nlohmann::json jLinkState : jLinksState) {
                std::string link = jLinkState["link"];
                std::string value = jLinkState["value"];
                linksState.insert({link, value});
            }

            std::vector<std::shared_ptr<BehavioralTransition>> in, out;
            std::vector<std::shared_ptr<BehavioralTransition>> tr;
            for( auto t: btransitions){
                auto s=t.get();

                auto newTransition = std::make_shared<BehavioralTransition>(s->getName(),
                                                                            s->getRelevancyLabel(),
                                                                            s->getObservabilityLabel());
                newTransition->id=s->getId();

                

                tr.push_back(newTransition);

            }
            findBehavioralTransitions(tr, jIn, in);
            findBehavioralTransitions(tr, jOut, out);

            if (jbhState.count("relevancyLabels") > 0) {
                std::set<std::string> relevancyLabels;
                for (const std::string label : jbhState["relevancyLabels"]) {
                    relevancyLabels.insert(label);
                }

                bhState.push_back(
                        std::make_shared<BehavioralState>(componentsState, linksState, starting, final, in, out,
                                                          relevancyLabels, id));
            } else
                bhState.push_back(
                        std::make_shared<BehavioralState>(componentsState, linksState, starting, final, in, out,
                                                          id));

            //nfaStates.back()->observationState = observationState;
        }

        std::vector<std::string> summary = {
                "Loaded Dictionary successfully.",
                "  States: " + std::to_string(states.size()),
                "  Transitions: " + std::to_string(transitions.size())
        };
        std::cout << "Loaded Dictionary successfully.\n States:"+std::to_string(states.size())+
            
               "\n Transitions: "+std::to_string(transitions.size())+"\n";
        Logger::log(summary);
        auto dictionary =std::make_shared<Dictionary>(states, transitions);
        dictionary->setBehavioralTransitions(btransitions);
        dictionary->setTinfo(tInfo);
        dictionary->setBhState(bhState);
        return dictionary;
    }

    
    

        static std::shared_ptr<Dictionary> loadOffline(const std::string &path) {
            int finalStates;
            nlohmann::json j;
            if (path.empty()) std::cin >> j;
            else {
                std::ifstream i(path);
                i >> j;
            }

            BehavioralTransition::nextId = 0;
            DictionaryState::nextId = 0;

            // Controllo delle meta-informazioni
            auto meta = j["meta"];
            if (meta["type"] != "dictionary") throw std::invalid_argument("Trying to load a non-Dictionary JSON");
            int version = meta["version"];
            if (version > 1) throw std::invalid_argument("Can't load JSON Dictionary with version > 1");

            auto jDictionary = j["dictionary"];

            auto jTransitions = jDictionary["transitions"];

            BehavioralTransition::nextId = 0;
            std::vector<std::shared_ptr<NFATransitionInterface>> transitions;

            for (auto jTransition : jTransitions) {
                unsigned int id = jTransition["id"];
                std::string label = jTransition["label"];
                transitions.push_back(std::make_shared<BehavioralTransition>("", id, "", label));
            }
            nlohmann::json jBTransitions = jDictionary["behavioralTransitions"];
            BehavioralTransition::nextId = 0;
            std::vector<std::shared_ptr<NFATransitionInterface>> btransitions;

            for (nlohmann::json jBTransition : jBTransitions) {
                std::string tName = jBTransition["name"];
                unsigned int tId = jBTransition["id"];
                std::string relevancyLabel, observabilityLabel;
                if (jBTransition.count("relevancy") > 0)
                    relevancyLabel = jBTransition["relevancy"];
                if (jBTransition.count("observability") > 0)
                    observabilityLabel = jBTransition["observability"];
                btransitions.push_back(
                        std::make_shared<BehavioralTransition>(tName, tId, relevancyLabel, observabilityLabel));
            }

            std::shared_ptr<std::vector<std::shared_ptr<transitionsInfo>>>tInfo( new std::vector<std::shared_ptr<transitionsInfo>>());

            nlohmann::json  jTinfo=jDictionary["tinfo"];
            for (nlohmann::json jTi : jTinfo) {
                unsigned int tiId= jTi["id"];
                unsigned int tiS= jTi["start"];
                unsigned int tiD= jTi["destination"];
                std::string label;
                if (jTi.count("label") > 0)
                    label = jTi["label"];
                auto tI = std::make_shared<transitionsInfo>(tiId,label,tiS);
                tI->setDestination(tiD);

                tInfo->push_back(tI);
            }
            nlohmann::json jStates = jDictionary["states"];
            std::vector<std::shared_ptr<DictionaryState>> states;
            finalStates=0;
            for (auto jState : jStates) {
                bool starting = jState.count("starting") > 0;
                bool final = jState.count("final") > 0;
                if (final){
                    finalStates++;
                }
                unsigned int id = jState["id"];
                std::vector<std::vector<unsigned int>> pairs = jState["pairs"];
                auto jIn = jState["in"];
                auto jOut = jState["out"];

                std::vector<std::shared_ptr<NFATransitionInterface>> in, out;
                findTransitions(transitions, jIn, in);
                findTransitions(transitions, jOut, out);

                auto jDiagnosis = jState["diagnosis"];
                std::set<std::set<std::string>> diagnosis;
                for (auto jSingleDiagnosis : jDiagnosis) {
                    std::set<std::string> singleDiagnosis;
                    for (std::string diag : jSingleDiagnosis) singleDiagnosis.insert(diag);
                    diagnosis.insert(singleDiagnosis);




                }

                auto jNfaStates = jState["nfaStates"];
                std::vector<std::shared_ptr<BehavioralState>> nfaStates;

                for (nlohmann::json jNfaState : jNfaStates) {
                    bool starting = jNfaState.count("starting") > 0 && jState["starting"];
                    bool final = jNfaState.count("final") > 0 && jNfaState["final"];
                    unsigned int id = jNfaState["id"];
                    nlohmann::json jComponentsState = jNfaState["componentsState"];
                    nlohmann::json jLinksState = jNfaState["linksState"];
                    nlohmann::json jIn = jNfaState["in"];
                    nlohmann::json jOut = jNfaState["out"];
                    int observationState = -1;
                    if (jNfaState.count("observationState") > 0) observationState = jNfaState["observationState"];

                    std::map<std::string, std::string> componentsState;
                    for (nlohmann::json jComponentState : jComponentsState) {
                        std::string component = jComponentState["component"];
                        std::string state = jComponentState["state"];
                        componentsState.insert({component, state});

                    }

                    std::map<std::string, std::string> linksState;
                    for (nlohmann::json jLinkState : jLinksState) {
                        std::string link = jLinkState["link"];
                        std::string value = jLinkState["value"];
                        linksState.insert({link, value});
                    }

                    std::vector<std::shared_ptr<BehavioralTransition>> in, out;
                    std::vector<std::shared_ptr<BehavioralTransition>> tr;
                    for( auto t: btransitions){
                        auto s=t.get();

                        auto newTransition = std::make_shared<BehavioralTransition>(s->getName(),
                                                                                    s->getRelevancyLabel(),
                                                                                    s->getObservabilityLabel());
                        newTransition->id=s->getId();

                        int c=1;

                        tr.push_back(newTransition);

                    }
                    findBehavioralTransitions(tr, jIn, in);
                    findBehavioralTransitions(tr, jOut, out);

                    if (jNfaState.count("relevancyLabels") > 0) {
                        std::set<std::string> relevancyLabels;
                        for (const std::string label : jNfaState["relevancyLabels"]) {
                            relevancyLabels.insert(label);
                        }

                        nfaStates.push_back(
                                std::make_shared<BehavioralState>(componentsState, linksState, starting, final, in, out,
                                                                  relevancyLabels, id));
                    } else
                        nfaStates.push_back(
                                std::make_shared<BehavioralState>(componentsState, linksState, starting, final, in, out,
                                                                  id));

                    nfaStates.back()->observationState = observationState;
                }
                auto dicState=std::make_shared<DictionaryState>(final, starting, in, out, diagnosis, pairs, id);
                std::vector<std::shared_ptr<BehavioralState>> nfaStates2;
                for (auto st :nfaStates){
                    auto s= std::make_shared<BehavioralState>();
                    int c=1;
                    s=st;
                    nfaStates2.push_back(s);

    /*
                    nfaStates2.push_back(s);
    */


                }
                   
                dicState->setNfaState2(nfaStates2);
                states.push_back(dicState);




    /*
                states.push_back(std::make_shared<DictionaryState>(final, starting, in, out, diagnosis, pairs, id));
    */
            }

            nlohmann::json jbhStates = jDictionary["bhState"];

            std::vector<std::shared_ptr<NFAStateInterface>>  bhState;

            for (nlohmann::json jbhState : jbhStates) {
                bool starting = jbhState.count("starting") > 0 && jbhState["starting"];
                bool final = jbhState.count("final") > 0 && jbhState["final"];
                unsigned int id = jbhState["id"];
                nlohmann::json jComponentsState = jbhState["componentsState"];
                nlohmann::json jLinksState = jbhState["linksState"];
                nlohmann::json jIn = jbhState["in"];
                nlohmann::json jOut = jbhState["out"];
                int observationState = -1;
                if (jbhState.count("observationState") > 0) observationState = jbhState["observationState"];

                std::map<std::string, std::string> componentsState;
                for (nlohmann::json jComponentState : jComponentsState) {
                    std::string component = jComponentState["component"];
                    std::string state = jComponentState["state"];
                    componentsState.insert({component, state});

                }

                std::map<std::string, std::string> linksState;
                for (nlohmann::json jLinkState : jLinksState) {
                    std::string link = jLinkState["link"];
                    std::string value = jLinkState["value"];
                    linksState.insert({link, value});
                }

                std::vector<std::shared_ptr<BehavioralTransition>> in, out;
                std::vector<std::shared_ptr<BehavioralTransition>> tr;
                for( auto t: btransitions){
                    auto s=t.get();

                    auto newTransition = std::make_shared<BehavioralTransition>(s->getName(),
                                                                                s->getRelevancyLabel(),
                                                                                s->getObservabilityLabel());
                    newTransition->id=s->getId();

                    int c=1;

                    tr.push_back(newTransition);

                }
                findBehavioralTransitions(tr, jIn, in);
                findBehavioralTransitions(tr, jOut, out);

                if (jbhState.count("relevancyLabels") > 0) {
                    std::set<std::string> relevancyLabels;
                    for (const std::string label : jbhState["relevancyLabels"]) {
                        relevancyLabels.insert(label);
                    }

                    bhState.push_back(
                            std::make_shared<BehavioralState>(componentsState, linksState, starting, final, in, out,
                                                              relevancyLabels, id));
                } else
                    bhState.push_back(
                            std::make_shared<BehavioralState>(componentsState, linksState, starting, final, in, out,
                                                              id));

                //nfaStates.back()->observationState = observationState;
            }

            std::vector<std::string> summary = {
                    "Loaded Dictionary successfully.",
                    "  States: " + std::to_string(states.size()),
                    "  Transitions: " + std::to_string(transitions.size())
            };
            std::cout << "Loaded Dictionary successfully.\n States:"+std::to_string(states.size())+
            "\n Final States: "+std::to_string(finalStates)+"\n Transitions: "+std::to_string(transitions.size())+"\n";
            Logger::log(summary);
            auto dictionary =std::make_shared<Dictionary>(states, transitions);
            dictionary->setBehavioralTransitions(btransitions);
            dictionary->setTinfo(tInfo);
            dictionary->setBhState(bhState);
            return dictionary;
        }
    
    static void save(const std::string &path, Dictionary &dic) {

        nlohmann::json jTransitions;
        for (const auto &transition : dic.transitions) {
            nlohmann::json jTransition;
            jTransition["id"] = transition->getId();
            jTransition["label"] = transition->getObservabilityLabel();
            jTransitions.push_back(jTransition);
        }
        nlohmann::json jTinfo;
        for (const auto &tIn : *dic.tInfo) {
            nlohmann::json jTin;
            jTin["id"] = tIn->getId();
            jTin["start"] = tIn->getStart();
            jTin["destination"] = tIn->getDestination();
            jTin["label"] = tIn->getLabel();
            jTinfo.push_back(jTin);
        }

        nlohmann::json jBehavioralTransitions;
        for (const auto &behavioralTransition : dic.behavioralTransitions) {
            nlohmann::json jBehavioralTransition;

            jBehavioralTransition["id"] = behavioralTransition->getId();
           jBehavioralTransition["name"] = behavioralTransition->getName();

            if (!behavioralTransition->getRelevancyLabel().empty())
                jBehavioralTransition["relevancy"] = behavioralTransition->getRelevancyLabel();


            if (!behavioralTransition->getObservabilityLabel().empty())
                jBehavioralTransition["observability"] = behavioralTransition->getObservabilityLabel();


            jBehavioralTransitions.push_back(jBehavioralTransition);
        }


        nlohmann::json jStates;
        for (const auto &state : dic.states) {
            nlohmann::json jState;

            jState["id"] = state->id;
            jState["pairs"] = state->pairs;

            if (state->starting) jState["starting"] = true;
            if (state->final) jState["final"] = true;

            nlohmann::json jIn;
            for (const auto &in : state->in) jIn.push_back(in->getId());

            nlohmann::json jOut;
            for (const auto &out : state->out) jOut.push_back(out->getId());

            jState["in"] = jIn;
            jState["out"] = jOut;

            nlohmann::json jDiagnosis;
            for (auto singleDiagnosis : state->diagnosis) {
                nlohmann::json jSingleDiagnosis;
                for (std::string diag : singleDiagnosis)
                    jSingleDiagnosis.push_back(diag);
                jDiagnosis.push_back(singleDiagnosis);
            }

            jState["diagnosis"] = jDiagnosis;


            nlohmann::json jNfaStates;
            for (const auto &nstate : state->nfaStates) {
                nlohmann::json jNfaState;
                jNfaState["id"] = nstate->getIdd();

                if (nstate->getStarting()) jNfaState["starting"] = true;
                if (nstate->getFinal()) jNfaState["final"] = true;
                if (nstate->getObservationState() != -1) jNfaState["observationState"] = nstate->getObservationState();
                if (nstate->getBondState() != -1) jNfaState["bondState"] = nstate->getBondState();

                nlohmann::json jComponentsState;
                for (auto cs : nstate->getComponentsState()) {
                    nlohmann::json jComponentState;
                    jComponentState["component"] = cs.first;
                    jComponentState["state"] = cs.second;
                    jComponentsState.push_back(jComponentState);
                }

                nlohmann::json jLinksState;
                for (auto ls : nstate->getLinksState()) {
                    nlohmann::json jLinkState;
                    jLinkState["link"] = ls.first;
                    jLinkState["value"] = ls.second;
                    jLinksState.push_back(jLinkState);
                }

                nlohmann::json jIn;
                for (const auto &in : nstate->getInTransitions()) jIn.push_back(in->getId());

                nlohmann::json jOut;
                for (const auto &out : nstate->getOutTransitions()) jOut.push_back(out->getId());

                jNfaState["componentsState"] = jComponentsState;
                jNfaState["linksState"] = jLinksState;
                jNfaState["in"] = jIn;
                jNfaState["out"] = jOut;

                if (!nstate->getRelevancyLabels().empty()) {
                    nlohmann::json jRelevancyLabels;
                    for (std::string label : nstate->getRelevancyLabels())
                        jRelevancyLabels.push_back(label);
                    jNfaState["relevancyLabels"] = jRelevancyLabels;
                }

                jNfaStates.push_back(jNfaState);
            }

            jState["nfaStates"] = jNfaStates;
            jStates.push_back(jState);





        }

        nlohmann::json jBehavioralStates;
        for (const auto &behavioralState : dic.bhState) {
            nlohmann::json jBehavioralState;


            jBehavioralState["id"] = behavioralState->getIdd();

            if (behavioralState->getStarting()) jBehavioralState["starting"] = true;
            if (behavioralState->getFinal()) jBehavioralState["final"] = true;
            if (behavioralState->getObservationState() != -1) jBehavioralState["observationState"] = behavioralState->getObservationState();
            if (behavioralState->getBondState() != -1) jBehavioralState["bondState"] = behavioralState->getBondState();

            nlohmann::json jComponentsState;
            for (auto cs : behavioralState->getComponentsState()) {
                nlohmann::json jComponentState;
                jComponentState["component"] = cs.first;
                jComponentState["state"] = cs.second;
                jComponentsState.push_back(jComponentState);
            }

            nlohmann::json jLinksState;
            for (auto ls : behavioralState->getLinksState()) {
                nlohmann::json jLinkState;
                jLinkState["link"] = ls.first;
                jLinkState["value"] = ls.second;
                jLinksState.push_back(jLinkState);
            }

            nlohmann::json jIn;
            for (const auto &in : behavioralState->getInTransitions()) jIn.push_back(in->getId());

            nlohmann::json jOut;
            for (const auto &out : behavioralState->getOutTransitions()) jOut.push_back(out->getId());

            jBehavioralState["componentsState"] = jComponentsState;
            jBehavioralState["linksState"] = jLinksState;
            jBehavioralState["in"] = jIn;
            jBehavioralState["out"] = jOut;

            if (!behavioralState->getRelevancyLabels().empty()) {
                nlohmann::json jRelevancyLabels;
                for (std::string label : behavioralState->getRelevancyLabels())
                    jRelevancyLabels.push_back(label);
                jBehavioralState["relevancyLabels"] = jRelevancyLabels;
            }



            jBehavioralStates.push_back(jBehavioralState);
        }

        nlohmann::json jDictionary;
        jDictionary["states"] = jStates;
        jDictionary["transitions"] = jTransitions;
        jDictionary["tinfo"]= jTinfo;
        jDictionary["behavioralTransitions"] = jBehavioralTransitions;
        jDictionary["bhState"] = jBehavioralStates;

        nlohmann::json jMeta;
        jMeta["type"] = "dictionary";
        jMeta["version"] = 1;

        nlohmann::json j;
        j["dictionary"] = jDictionary;
        j["meta"] = jMeta;

        if (path.empty()) std::cout << std::setw(2) << j << std::endl;
        else {
            std::ofstream o(path);
            o << std::setw(2) << j << std::endl;
        }
    }

    static void save3(const std::string &path,  std::vector<std::shared_ptr<std::vector<std::shared_ptr<temporalAbduction>>>> newAb,std::vector<std::vector<std::string>> &observation) {
        nlohmann::json jabduce;
        int count=0;
        nlohmann::json jels;
        nlohmann::json jel;
        nlohmann::json jdiagnosi;
        nlohmann::json jobs;
        for(auto ab: newAb) {
            int cc = 0;
           
            /*auto osservazione=observation[count];*/

            
            //ciclo sul temporal abduce relativo a un sintomo
            for (auto t : *ab) {

                std::string st = "";

                int conta = 0;
                for ( auto di : t->diagn) {

                    int contatore = 1;
                    std::string sep = "";
                    for ( auto d : di) {
                        if (contatore < di.size()) {
                            sep = " ";

                        } else {
                            sep = "";
                        }
                        st = st + d + sep;
                        contatore++;
                        std::cout << "\"" << d << ",\"";

                    }
                    if (conta < t->diagn.size()-1) {
                        st = st + ", ";
                    }
                    std::cout << "}";


                }

                    /*jobs["obs"] = osservazione[conta];
                    jel.push_back(jobs);*/
                    jdiagnosi["diagnosi"] = st;
                    jel.push_back(jdiagnosi);
                    jels["el"]=jel;

                    /*jels.push_back(jel);*/


                cc++;
                conta++;


                nlohmann::json jTemporalAbudce;

                /*jabduce.push_back(jels);*/





                count++;
            }
            /*jabduce["abduce"]=jels;*/
            jels.push_back(jels);
            jabduce["abduce"] = jels;


        }
        nlohmann::json j;
        j["Temporal abduces"] = jabduce;

        if (path.empty()) std::cout << std::setw(2) << j << std::endl;
        else {
            std::ofstream o(path);
            o << std::setw(2) << j << std::endl;
        }



    }



    static void save2(const std::string &path, std::vector<std::shared_ptr<std::vector<std::shared_ptr<temporalAbduction>>>> newAb,std::vector<std::vector<std::string>> &observation) {


        int indiceoss=0;
        nlohmann::json jAbduce;
        for (auto abd : newAb) {
            nlohmann::json jState;

            int conteggio=0;
            nlohmann::json jelems;
            for (auto cs : *abd) {
                nlohmann::json jel;


                std::string st = "";
                int conta = 0;
                for (const auto di : cs->diagn) {
                    int contatore = 1;
                    std::string sep = "";

                    for (const auto d : di) {
                        if (contatore < di.size()) {
                            sep = " ";

                        } else {
                            sep = "";
                        }
                        st = st + d + sep;
                        contatore++;


                    }
                    if (conta < cs->diagn.size() - 1) {
                        st = st + ", ";
                    }
                    conta++;
                }
                if(conteggio>0){
                    jel["diagnosi"] = st;
                    jel["osservazione"]=observation[indiceoss][conteggio-1];

                    jelems.push_back(jel);
                }

                conteggio++;
            }



            jState["elements"] = jelems;



            jAbduce.push_back(jState);
            indiceoss++;
        }

        nlohmann::json jBs;
        jBs["Temporal Abduce"] = jAbduce;


        nlohmann::json jMeta;
        jMeta["type"] = "bs";
        jMeta["version"] = 1;

        nlohmann::json j;
        j["Abduces"] = jBs;
        j["meta"] = jMeta;

        if (path.empty()) std::cout << std::setw(2) << j << std::endl;
        else {
            std::ofstream o(path);
            o << std::setw(2) << j << std::endl;
        }
    }

    static void saveAbduce(const std::string &path, std::vector<std::shared_ptr<temporalAbduction>> newAb,std::vector<std::string> &observation) {


        int indiceoss=0;
        nlohmann::json jAbduce;
        nlohmann::json jelems;
        int conteggio=0;

        for (auto abd : newAb) {




                nlohmann::json jel;


                std::string st = "";
                int conta = 0;
                for (const auto di : abd->getDiagnosis2()) {
                    int contatore = 1;
                    std::string sep = "";

                    for (const auto d : di) {
                        if (contatore < di.size()) {
                            sep = " ";

                        } else {
                            sep = "";
                        }
                        st = st + d + sep;
                        contatore++;


                    }
                    if (conta < abd->getDiagnosis2().size() - 1) {
                        st = st + ", ";
                    }
                    conta++;
                }
                if(conteggio==0){
                    jel["diagnosi"] = st;
                    jel["osservazione"]=" ";

                    jelems.push_back(jel);
                }
                if(conteggio>0){
                    jel["diagnosi"] = st;
                    jel["osservazione"]=observation[conteggio-1];

                    jelems.push_back(jel);
                }

                conteggio++;
        }



        jAbduce["elements"] = jelems;




            indiceoss++;


        nlohmann::json jBs;
        jBs["Temporal Abduce"] = jAbduce;


        nlohmann::json jMeta;
        jMeta["type"] = "bs";
        jMeta["version"] = 1;

        nlohmann::json j;
        j["Abduces"] = jBs;
        j["meta"] = jMeta;

        if (path.empty()) std::cout << std::setw(2) << j << std::endl;
        else {
            std::ofstream o(path);
            o << std::setw(2) << j << std::endl;
        }
    }



private:
    static void
    findTransitions(std::vector<std::shared_ptr<NFATransitionInterface>> transitions, nlohmann::json jTransitions,
                    std::vector<std::shared_ptr<NFATransitionInterface>> &out) {
        for (unsigned int transitionId : jTransitions) {
            std::shared_ptr<NFATransitionInterface> found = nullptr;
            for (const auto &transition : transitions)
                if (transition->getId() == transitionId) {
                    found = transition;
                    break;
                }
            if (found == nullptr) throw std::invalid_argument("Input JSON attempts to use an undefined transition");
            out.push_back(found);
        }
    }





private:

    static void
    findBehavioralTransitions(std::vector<std::shared_ptr<BehavioralTransition>> transitions, nlohmann::json jTransitions,std::vector<std::shared_ptr<BehavioralTransition>> &out) {
        for (unsigned int transitionId : jTransitions) {
            std::shared_ptr<BehavioralTransition> found = nullptr;
            for (const auto &transition : transitions)
                if (transition->id == transitionId) {
                    found = transition;
                    break;
                }
            if (found == nullptr) throw std::invalid_argument("Input JSON attempts to use an undefined transition");
            out.push_back(found);
        }
    }


};
#endif //AUTOMATAMANAGER_LOADDICTIONARY_H
