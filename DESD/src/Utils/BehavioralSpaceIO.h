//
// Created by alessandro on 8/6/18.
//

#ifndef AUTOMATAMANAGER_BEHAVIORALSPACEIO_H
#define AUTOMATAMANAGER_BEHAVIORALSPACEIO_H

#include <nlohmann/json.hpp>
#include "../stdIncludes.h"
#include "../Data/BehavioralSpace.h"

class BehavioralSpaceIO {
public:

    static std::shared_ptr<BehavioralSpace> load(const std::string &path) {
        
        nlohmann::json j;
        if (path.empty()) {
            std::cin >> j;
        } else {
            std::ifstream i(path);
            i >> j;
        }

        // Controllo delle meta-informazioni
        auto meta = j["meta"];
        if (meta["type"] != "bs") throw std::invalid_argument("Trying to load a non-BehavioralSpace JSON");
        int version = meta["version"];
        if (version > 2) throw std::invalid_argument("Can't load JSON Behavioral Space with version > 2");

        nlohmann::json jBs = j["behavioralSpace"];

        nlohmann::json jTransitions = jBs["transitions"];
        BehavioralTransition::nextId = 0;
        std::vector<std::shared_ptr<BehavioralTransition>> transitions;

        for (nlohmann::json jTransition : jTransitions) {
            std::string tName = jTransition["name"];
            unsigned int tId = jTransition["id"];
            std::string relevancyLabel, observabilityLabel;
            if (jTransition.count("relevancy") > 0)
                relevancyLabel = jTransition["relevancy"];
            if (jTransition.count("observability") > 0)
                observabilityLabel = jTransition["observability"];
            transitions.push_back(
                std::make_shared<BehavioralTransition>(tName, tId, relevancyLabel, observabilityLabel));
        }

        nlohmann::json jStates = jBs["states"];
        std::vector<std::shared_ptr<BehavioralState>> states;

        for (nlohmann::json jState : jStates) {
            bool starting = jState.count("starting") > 0 && jState["starting"];
            bool final = jState.count("final") > 0 && jState["final"];
            unsigned int id = jState["id"];
            nlohmann::json jComponentsState = jState["componentsState"];
            nlohmann::json jLinksState = jState["linksState"];
            nlohmann::json jIn = jState["in"];
            nlohmann::json jOut = jState["out"];
            int observationState = -1;
            if (jState.count("observationState") > 0) observationState = jState["observationState"];

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
            findTransitions(transitions, jIn, in);
            findTransitions(transitions, jOut, out);

            if (jState.count("relevancyLabels") > 0) {
                std::set<std::string> relevancyLabels;
                for (const std::string label : jState["relevancyLabels"]){
                    relevancyLabels.insert(label);
                }

                states.push_back(std::make_shared<BehavioralState>(componentsState, linksState, starting, final, in, out,
                                                                   relevancyLabels,id));
            } else
                states.push_back(std::make_shared<BehavioralState>(componentsState, linksState, starting, final, in, out,id));

            states.back()->observationState = observationState;
        }

        std::vector<std::string> summary = {
            "Loaded Behavioral Space successfully.",
            "  States: " + std::to_string(states.size()),
            "  Transitions: " + std::to_string(transitions.size())
        };
        
        std::cout << "Loaded Behavioral Space successfully.\n  States:"+std::to_string(states.size())+"\n  Transitions: "+std::to_string(transitions.size());
        Logger::log(summary);
            
        return std::make_shared<BehavioralSpace>(states, transitions);
    }

    static void save(const std::string &path, BehavioralSpace &bs) {
        int numTransitions,finalStates;
        

        nlohmann::json jTransitions;
        for (const auto &transition : bs.transitions) {
            nlohmann::json jTransition;

            jTransition["id"] = transition->id;
            jTransition["name"] = transition->name;

            if (!transition->relevancyLabel.empty())
                jTransition["relevancy"] = transition->relevancyLabel;


            if (!transition->observabilityLabel.empty())
                jTransition["observability"] = transition->observabilityLabel;


            jTransitions.push_back(jTransition);
        }

        nlohmann::json jStates;
            finalStates=0;
        for (const auto &state : bs.states) {
            nlohmann::json jState;
            jState["id"]=state->id;
            if (state->starting) jState["starting"] = true;
            if (state->final){
                jState["final"] = true;
                finalStates++;
            }
            if (state->observationState != -1) jState["observationState"] = state->observationState;
            if (state->bondState != -1) jState["bondState"] = state->bondState;

            nlohmann::json jComponentsState;
            for (auto cs : state->componentsState) {
                nlohmann::json jComponentState;
                jComponentState["component"] = cs.first;
                jComponentState["state"] = cs.second;
                jComponentsState.push_back(jComponentState);
            }

            nlohmann::json jLinksState;
            for (auto ls : state->linksState) {
                nlohmann::json jLinkState;
                jLinkState["link"] = ls.first;
                jLinkState["value"] = ls.second;
                jLinksState.push_back(jLinkState);
            }

            nlohmann::json jIn;
            for (const auto &in : state->in) jIn.push_back(in->id);

            nlohmann::json jOut;
            for (const auto &out : state->out) jOut.push_back(out->id);

            jState["componentsState"] = jComponentsState;
            jState["linksState"] = jLinksState;
            jState["in"] = jIn;
            jState["out"] = jOut;

            if (!state->relevancyLabels.empty()) {
                nlohmann::json jRelevancyLabels;
                for (std::string label : state->relevancyLabels)
                    jRelevancyLabels.push_back(label);
                jState["relevancyLabels"] = jRelevancyLabels;
            }

            jStates.push_back(jState);
        }

        nlohmann::json jBs;
        jBs["states"] = jStates;
        jBs["transitions"] = jTransitions;

        nlohmann::json jMeta;
        jMeta["type"] = "bs";
        jMeta["version"] = 1;

        nlohmann::json j;
        j["behavioralSpace"] = jBs;
        j["meta"] = jMeta;
            
        if (path.empty()) std::cout << std::setw(2) << j << std::endl;
        else {
            std::ofstream o(path);
            o << std::setw(2) << j << std::endl;
        }
    }

    
    
    
    
    //offline
    static std::shared_ptr<BehavioralSpace> loadOffline(const std::string &path) {
        int finalStates=0;
           nlohmann::json j;
           if (path.empty()) {
               std::cin >> j;
           } else {
               std::ifstream i(path);
               i >> j;
           }

           // Controllo delle meta-informazioni
           auto meta = j["meta"];
           if (meta["type"] != "bs") throw std::invalid_argument("Trying to load a non-BehavioralSpace JSON");
           int version = meta["version"];
           if (version > 2) throw std::invalid_argument("Can't load JSON Behavioral Space with version > 2");

           nlohmann::json jBs = j["behavioralSpace"];

           nlohmann::json jTransitions = jBs["transitions"];
           BehavioralTransition::nextId = 0;
           std::vector<std::shared_ptr<BehavioralTransition>> transitions;

           for (nlohmann::json jTransition : jTransitions) {
               std::string tName = jTransition["name"];
               unsigned int tId = jTransition["id"];
               std::string relevancyLabel, observabilityLabel;
               if (jTransition.count("relevancy") > 0)
                   relevancyLabel = jTransition["relevancy"];
               if (jTransition.count("observability") > 0)
                   observabilityLabel = jTransition["observability"];
               transitions.push_back(
                   std::make_shared<BehavioralTransition>(tName, tId, relevancyLabel, observabilityLabel));
           }

           nlohmann::json jStates = jBs["states"];
           std::vector<std::shared_ptr<BehavioralState>> states;

           for (nlohmann::json jState : jStates) {
               bool starting = jState.count("starting") > 0 && jState["starting"];
               bool final = jState.count("final") > 0 && jState["final"];
               if (final) finalStates++;
               unsigned int id = jState["id"];
               nlohmann::json jComponentsState = jState["componentsState"];
               nlohmann::json jLinksState = jState["linksState"];
               nlohmann::json jIn = jState["in"];
               nlohmann::json jOut = jState["out"];
               int observationState = -1;
               if (jState.count("observationState") > 0) observationState = jState["observationState"];

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
               findTransitions(transitions, jIn, in);
               findTransitions(transitions, jOut, out);

               if (jState.count("relevancyLabels") > 0) {
                   std::set<std::string> relevancyLabels;
                   for (const std::string label : jState["relevancyLabels"]){
                       relevancyLabels.insert(label);
                   }

                   states.push_back(std::make_shared<BehavioralState>(componentsState, linksState, starting, final, in, out,
                                                                      relevancyLabels,id));
               } else
                   states.push_back(std::make_shared<BehavioralState>(componentsState, linksState, starting, final, in, out,id));

               states.back()->observationState = observationState;
           }

           std::vector<std::string> summary = {
               "Loaded Behavioral Space successfully.",
               "  States: " + std::to_string(states.size()),
               "  Transitions: " + std::to_string(transitions.size())
           };
           
           std::cout << "Loaded Behavioral Space successfully.\n  States:"+std::to_string(states.size())+
       "\n Final States:"+std::to_string(finalStates)+
        "\n  Transitions: "+std::to_string(transitions.size())+"\n";
           Logger::log(summary);
               
           return std::make_shared<BehavioralSpace>(states, transitions);
       }
private:

    static void findTransitions(std::vector<std::shared_ptr<BehavioralTransition>> transitions, nlohmann::json jTransitions,
                                std::vector<std::shared_ptr<BehavioralTransition>> &out) {
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


#endif //AUTOMATAMANAGER_BEHAVIORALSPACEIO_H
