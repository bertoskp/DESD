//
// Created by alessandro on 8/6/18.
//

#ifndef AUTOMATAMANAGER_NETWORKIO_H
#define AUTOMATAMANAGER_NETWORKIO_H

#include "json.hpp"
#include "../stdIncludes.h"
#include "../Data/Network.h"

class NetworkIO {
public:

    static std::shared_ptr<Network> load(const std::string &path) {
        nlohmann::json j;
        if (path.empty()) std::cin >> j;
        else {
            std::ifstream i(path);
            i >> j;
        }

        // Contatori utilizzati per il riassunto finale
        int statesCount = 0, transitionsCount = 0;

        // Controllo delle meta-informazioni
        auto meta = j["meta"];
        if (meta["type"] != "network") throw std::invalid_argument("Trying to load a non-network JSON");
        int version = meta["version"];
        if (version > 2) throw std::invalid_argument("Can't load JSON Network with version > 2");

        nlohmann::json jNetwork = j["network"];
        nlohmann::json jLinks = jNetwork["links"];
        nlohmann::json jComps = jNetwork["components"];

        std::vector<std::shared_ptr<Component>> components;
        std::vector<std::shared_ptr<Link>> links;

        for (std::string linkName : jLinks) links.push_back(std::make_shared<Link>(linkName));

        for (const nlohmann::json &jComponent : jComps) {
            std::string componentName = jComponent["name"];
            nlohmann::json jStates = jComponent["states"];
            nlohmann::json jTransitions = jComponent["transitions"];

            std::vector<std::shared_ptr<State>> states;
            std::vector<std::shared_ptr<Transition>> transitions;

            // Costruisco le transizioni
            for (auto jTransition: jTransitions) {
                std::string transitionName = jTransition["name"];
                nlohmann::json jInEvent = jTransition["in"];
                nlohmann::json jOutEvents = jTransition["out"];

                std::shared_ptr<Event> inEvent = nullptr;
                std::vector<std::shared_ptr<Event>> outEvents;

                if (!jInEvent.is_null()) inEvent = constructEvent(jInEvent, links);

                for (const auto &jEvent : jOutEvents) outEvents.push_back(constructEvent(jEvent, links));

                std::string observation, relevancy;
                if (jTransition.count("observation") > 0) observation = jTransition["observation"];
                if (jTransition.count("relevancy") > 0) relevancy = jTransition["relevancy"];
                transitions.push_back(
                    std::make_shared<Transition>(transitionName, inEvent, outEvents, relevancy, observation));

            }

            // Costruisco gli stati
            for (auto jState: jStates) {
                std::string name = jState["name"];
                bool starting = jState.find("starting") != jState.end();

                nlohmann::json jInTransitions = jState["in"];
                nlohmann::json jOutTransitions = jState["out"];
                std::vector<std::shared_ptr<Transition>> inTransitions, outTransitions;

                findTransitions(transitions, jInTransitions, inTransitions);
                findTransitions(transitions, jOutTransitions, outTransitions);

                states.push_back(std::make_shared<State>(name, starting, inTransitions, outTransitions));
            }

            statesCount += states.size();
            transitionsCount += transitions.size();
            components.push_back(std::make_shared<Component>(states, transitions, componentName));
        }


        std::vector<std::string> summary = {
            "Loaded Network successfully.",
            "  States: " + std::to_string(statesCount),
            "  Transitions: " + std::to_string(transitionsCount),
            "  Links: " + std::to_string(links.size()),
            "  Components: " + std::to_string(components.size())
        };
        Logger::log(summary);


        return std::make_shared<Network>(components, links);
    }

    
    static void save(const std::string &path, std::shared_ptr<Network> &network) {
           //int numStates,numTransitions,finalStates;
           nlohmann::json jLinks;
        for (const auto &link: network->links) jLinks.push_back(link->name);
            
      
        
        nlohmann::json  jComponents;
         for (const auto &component : network->components) {
           nlohmann::json jComponent;
             jComponent["name"]=component->name;
             nlohmann::json jTransitions;
             for (const auto &transition : component->transitions) {
                 nlohmann::json jTransition;
                 
                 jTransition["name"]=transition->name;
                 nlohmann::json jInEvent;
                 if(transition->in!=nullptr){
                 jInEvent["link"]=transition->in->link->name;
                 jInEvent["value"]=transition->in->name;
                 jTransition["in"] = jInEvent;
                 }
                 else{
                    jTransition["in"] = jInEvent;

                 }
                 nlohmann::json jOutEvents;
                 if (transition->out.size()>0){
                 for(const auto &Eout: transition->out){
                     if(Eout!=nullptr){
                     nlohmann::json jOutEvent;
                     jOutEvent["link"]=Eout->link->name;
                     jOutEvent["value"]=Eout->name;
                     jOutEvents.push_back(jOutEvent);
                 }
                 }
                     jTransition["out"] = jOutEvents;
                 }
                     else{
                        
                      jTransition["out"] = jOutEvents;
;
                     }
                    
                if(transition->observabilityLabel!="") jTransition["observation"]=transition->observabilityLabel;
                
                 
                 if(transition->relevancyLabel!="") jTransition["relevancy"]=transition->relevancyLabel;
                 jTransitions.push_back(jTransition);
                 
                 
                 
             }
             jComponent["transitions"]=jTransitions;
             nlohmann::json jStates;
             for (const auto &state : component->states) {
              nlohmann::json jState;
                 jState["name"]=state->name;
                 if(state->starting){
                     jState["starting"]=state->starting;
                 }
                 nlohmann::json jIn;
                 for(const auto &inT: state->inTransitions) jIn.push_back(inT->name);
                nlohmann::json jOut;
                for(const auto &Out: state->outTransitions) jOut.push_back(Out->name);
                 jState["in"] = jIn;
                 jState["out"] = jOut;
                 jStates.push_back(jState);
                              
             }
             jComponent["states"]=jStates;
             jComponents.push_back(jComponent);


         }
        nlohmann::json jNetwork;
        jNetwork["components"] = jComponents;
        jNetwork["links"] = jLinks;
               
           

           
           nlohmann::json jMeta;
           jMeta["type"] = "network";
           jMeta["version"] = 2;

           nlohmann::json j;
           j["network"] = jNetwork;
           j["meta"] = jMeta;
               
           if (path.empty()) std::cout << std::setw(2) << j << std::endl;
           else {
               std::ofstream o(path);
               o << std::setw(2) << j << std::endl;
           }
       }
private:
    static std::shared_ptr<Event> constructEvent(const nlohmann::json &jEvent, std::vector<std::shared_ptr<Link>> &links) {
        std::string linkName = jEvent["link"];
        std::string value = jEvent["value"];

        std::shared_ptr<Link> found = nullptr;
        for (const auto &l : links)
            if (l->name == linkName) found = l;

        if (found == nullptr)
            throw std::invalid_argument("Malformed input file: attempting to use a Link without defining it");

        return std::make_shared<Event>(found.get(), value);
    }

    static void findTransitions(std::vector<std::shared_ptr<Transition>> &transitions, const nlohmann::json &jTransitions,
                                std::vector<std::shared_ptr<Transition>> &toFill) {
        for (const std::string name : jTransitions) {
            bool found = false;
            for (const auto &t : transitions)
                if (!found && t->name == name) {
                    toFill.push_back(t);
                    found = true;
                }
            if (!found) throw std::invalid_argument("Malformed input file: attempting to use a Transition without defining it");
        }
    }
};


#endif //AUTOMATAMANAGER_NETWORKIO_H
