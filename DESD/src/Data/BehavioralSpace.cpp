

#include "BehavioralSpace.h"
#include "BehavioralState.h"

//1unsigned int BehavioralState::nextId = 0;


BehavioralSpace::BehavioralSpace(std::vector<std::shared_ptr<BehavioralState>> &_states,
                                 std::vector<std::shared_ptr<BehavioralTransition>> &_transitions) {
    states = _states;
    transitions = _transitions;
    setToInitialState();
}

std::pair<int, int> BehavioralSpace::getDimensions(){
    return std::pair<int, int>((int)states.size(), (int)transitions.size());
};

void BehavioralSpace::setToInitialState() {
    for(int i = 0; i < states.size(); i++)
        if(states[i]->starting){
        currentState = i;
        break;
    }
}

std::shared_ptr<BehavioralState> BehavioralSpace::getCurrentState() {
    return states[currentState];
}

void BehavioralSpace::performTransition(std::shared_ptr<BehavioralTransition> transition) {
    for(int i = 0; i < states.size(); i++){
        for (const auto &inTransition : states[i]->in)
            if(inTransition == transition) {
                currentState = i;
                break;
            }
    }
}

std::vector<std::shared_ptr<NFATransitionInterface>> BehavioralSpace::getTransitions() {
    std::vector<std::shared_ptr<NFATransitionInterface>> output;
    for (const auto &t : transitions) output.push_back(t);
    return output;
}


std::vector<std::shared_ptr<NFATransitionInterface>> BehavioralSpace::getBehavioralTransitions() {
    std::vector<std::shared_ptr<NFATransitionInterface>> output;
    for (const auto &t : transitions) output.push_back(t);
    return output;
}

std::vector<std::shared_ptr<NFAStateInterface>> BehavioralSpace::getStates() {
    std::vector<std::shared_ptr<NFAStateInterface>> output;
    for (const auto &s : states) output.push_back(s);
    return output;
}
