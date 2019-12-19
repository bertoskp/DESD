//
// Created by alessandro on 7/31/18.
//

#ifndef AUTOMATAMANAGER_OBSERVATION_H
#define AUTOMATAMANAGER_OBSERVATION_H


#include "../stdIncludes.h"
#include "SimpleState.h"

class SimpleNetwork {
public:
    std::vector<std::shared_ptr<SimpleState>> states;
    std::vector<std::shared_ptr<SimpleTransition>> transitions;
    int currentState;

    SimpleNetwork(std::vector<std::shared_ptr<SimpleState>> &_states,
                std::vector<std::shared_ptr<SimpleTransition>> &_transitions);

    void resetState();

    std::string toString();

    std::shared_ptr<std::vector<std::string>> getQualifiedLabels();

    void performTransition(std::string &label);

    bool reachedFinalState();
};


#endif //AUTOMATAMANAGER_OBSERVATION_H
