//
// Created by alessandro on 8/6/18.
//

#ifndef AUTOMATAMANAGER_SIMPLENETWORKIO_H
#define AUTOMATAMANAGER_SIMPLENETWORKIO_H

#include <nlohmann/json.hpp>
#include "../stdIncludes.h"
#include "../Data/SimpleNetwork.h"

class SimpleNetworkIO {
public:

    static std::shared_ptr<SimpleNetwork> load(const std::string &path) {
        nlohmann::json j;
        if (path.empty()) std::cin >> j;
        else {
            std::ifstream i(path);
            i >> j;
        }

        // Controllo delle meta-informazioni
        auto meta = j["meta"];
        if (meta["type"] != "simplenetwork") throw std::invalid_argument("Trying to load a non-SimpleNetwork JSON");
        int version = meta["version"];
        if (version > 1) throw std::invalid_argument("Can't load JSON SimpleNetwork with version > 1");

        auto jNetwork = j["network"];

        auto jTransitions = jNetwork["transitions"];

        std::vector<std::shared_ptr<SimpleTransition>> transitions;

        for (auto jTransition : jTransitions) {
            unsigned int id = jTransition["id"];
            std::string label = jTransition["label"];
            transitions.push_back(std::make_shared<SimpleTransition>(id, label));
        }

        nlohmann::json jStates = jNetwork["states"];
        std::vector<std::shared_ptr<SimpleState>> states;

        for (auto jState : jStates) {
            bool starting = jState.count("starting") > 0 && jState["starting"];
            bool final = jState.count("final") > 0 && jState["final"];
            unsigned int id = jState["id"];

            auto jIn = jState["in"];
            auto jOut = jState["out"];

            std::vector<std::shared_ptr<SimpleTransition>> in, out;
            findTransitions(transitions, jIn, in);
            findTransitions(transitions, jOut, out);

            states.push_back(std::make_shared<SimpleState>(id, in, out, final, starting));
        }

        std::vector<std::string> summary = {
            "Loaded Simple Network successfully.",
            "  States: " + std::to_string(states.size()),
            "  Transitions: " + std::to_string(transitions.size())
        };
        Logger::log(summary);

        return std::make_shared<SimpleNetwork>(states, transitions);
    }

    static void save(const std::string &path, SimpleNetwork &network) {

        nlohmann::json jTransitions;
        for (const auto &transition : network.transitions) {
            nlohmann::json jTransition;
            jTransition["id"] = transition->id;
            jTransition["label"] = transition->label;
            jTransitions.push_back(jTransition);
        }

        nlohmann::json jStates;
        for (const auto &state : network.states) {
            nlohmann::json jState;
            jState["id"] = state->id;
            if (state->starting) jState["starting"] = true;
            if (state->final) jState["final"] = true;

            nlohmann::json jIn;
            for (const auto &in : state->in) jIn.push_back(in->id);

            nlohmann::json jOut;
            for (const auto &out : state->out) jOut.push_back(out->id);

            jState["in"] = jIn;
            jState["out"] = jOut;
            jStates.push_back(jState);
        }

        nlohmann::json jNetwork;
        jNetwork["states"] = jStates;
        jNetwork["transitions"] = jTransitions;

        nlohmann::json jMeta;
        jMeta["type"] = "simplenetwork";
        jMeta["version"] = 1;

        nlohmann::json j;
        j["network"] = jNetwork;
        j["meta"] = jMeta;

        if (path.empty()) std::cout << std::setw(2) << j << std::endl;
        else {
            std::ofstream o(path);
            o << std::setw(2) << j << std::endl;
        }
    }



    static void save2(const std::string &path,const std::shared_ptr<SimpleNetwork> network) {

        nlohmann::json jTransitions;
        for (const auto &transition : network->transitions) {
            nlohmann::json jTransition;
            jTransition["id"] = transition->id;
            jTransition["label"] = transition->label;
            jTransitions.push_back(jTransition);
        }

        nlohmann::json jStates;
        for (const auto &state : network->states) {
            nlohmann::json jState;
            jState["id"] = state->id;
            if (state->starting) jState["starting"] = true;
            if (state->final) jState["final"] = true;

            nlohmann::json jIn;
            for (const auto &in : state->in) jIn.push_back(in->id);

            nlohmann::json jOut;
            for (const auto &out : state->out) jOut.push_back(out->id);

            jState["in"] = jIn;
            jState["out"] = jOut;
            jStates.push_back(jState);
        }

        nlohmann::json jNetwork;
        jNetwork["states"] = jStates;
        jNetwork["transitions"] = jTransitions;

        nlohmann::json jMeta;
        jMeta["type"] = "simplenetwork";
        jMeta["version"] = 1;

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
    static void findTransitions(const std::vector<std::shared_ptr<SimpleTransition>> &transitions, nlohmann::json jTransitions,
                         std::vector<std::shared_ptr<SimpleTransition>> &out) {
        for (unsigned int transitionId : jTransitions) {
            std::shared_ptr<SimpleTransition> found = nullptr;
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


#endif //AUTOMATAMANAGER_SIMPLENETWORKIO_H
