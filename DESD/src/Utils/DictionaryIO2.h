#ifndef AUTOMATAMANAGER_LOADDICTIONARY_H
#define AUTOMATAMANAGER_LOADDICTIONARY_H

#include "../stdIncludes.h"
#include <nlohmann/json.hpp>
#include "../Data/Dictionary.h"

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

        nlohmann::json jStates = jDictionary["states"];
        std::vector<std::shared_ptr<DictionaryState>> states;

        for (auto jState : jStates) {
            bool starting = jState.count("starting") > 0;
            bool final = jState.count("final") > 0;
            unsigned int id = jState["id"];
            std::vector<std::vector<unsigned int>> pairs= jState["pairs"];
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

            states.push_back(std::make_shared<DictionaryState>(final, starting, in, out, diagnosis,pairs, id));
        }

        std::vector<std::string> summary = {
            "Loaded Dictionary successfully.",
            "  States: " + std::to_string(states.size()),
            "  Transitions: " + std::to_string(transitions.size())
        };
        Logger::log(summary);

        return std::make_shared<Dictionary>(states, transitions);
    }

    static void save(const std::string &path, Dictionary &dic) {

        nlohmann::json jTransitions;
        for (const auto &transition : dic.transitions) {
            nlohmann::json jTransition;
            jTransition["id"] = transition->getId();
            jTransition["label"] = transition->getObservabilityLabel();
            jTransitions.push_back(jTransition);
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

            jStates.push_back(jState);
        }

        nlohmann::json jDictionary;
        jDictionary["states"] = jStates;
        jDictionary["transitions"] = jTransitions;

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

private:
    static void findTransitions(std::vector<std::shared_ptr<NFATransitionInterface>> transitions, nlohmann::json jTransitions,
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
};


#endif //AUTOMATAMANAGER_LOADDICTIONARY_H
