//
// Created by Diego Lorenzi on 24/07/18.
//

#ifndef AUTOMATAMANAGER_BEHAVIORALSPACE_H
#define AUTOMATAMANAGER_BEHAVIORALSPACE_H

#include "../stdIncludes.h"
#include "BehavioralState.h"
#include "NFASpaceInterface.h"

class BehavioralSpace : public NFASpaceInterface {
public:
    std::vector<std::shared_ptr<BehavioralState>> states;
    std::vector<std::shared_ptr<BehavioralTransition>> transitions;
    int currentState;

    BehavioralSpace() = default;

    BehavioralSpace(std::vector<std::shared_ptr<BehavioralState>> &_states,
                    std::vector<std::shared_ptr<BehavioralTransition>> &_transitions);

    std::pair<int, int> getDimensions();

    void setToInitialState();

    std::shared_ptr<BehavioralState> getCurrentState();

    void performTransition(std::shared_ptr<BehavioralTransition> transition);

    std::vector<std::shared_ptr<NFATransitionInterface>> getTransitions() override;
    std::vector<std::shared_ptr<NFATransitionInterface>> getBehavioralTransitions() override;

    std::vector<std::shared_ptr<NFAStateInterface>> getStates() override;

};


#endif //AUTOMATAMANAGER_BEHAVIORALSPACE_H
