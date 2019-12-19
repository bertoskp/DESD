#ifndef AUTOMATAMANAGER_LOADDICTIONARY_H
#define AUTOMATAMANAGER_LOADDICTIONARY_H

#include "../stdIncludes.h"
#include <nlohmann/json.hpp>
#include "../Data/Observation.h"

class ObservationIO {
public:
    static std::shared_ptr<Observation> load(const std::string &path) {
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
        if (meta["type"] != "observation") throw std::invalid_argument("Trying to load a non-Dictionary JSON");
        int version = meta["version"];
        if (version > 1) throw std::invalid_argument("Can't load JSON Dictionary with version > 1");

        auto jObservation = j["obs"];
        std::set<std::set<std::string>> observation;
        for (auto jSingleObs : jObservation) {
            std::set<std::string> singleObs;

            observation.insert(singleObs);
        }


        std::vector<std::string> summary = {
            "Loaded Obs successfully.",
            "  OBs: " + std::to_string(observation.size()))
        };
        Logger::log(summary);

        return std::make_shared<Observation>(observation);
    }
/*
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
};*/


#endif //AUTOMATAMANAGER_LOADDICTIONARY_H
