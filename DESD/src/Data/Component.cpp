

#include <iostream>
#include <algorithm>
#include "Component.h"

Component::Component(){
    currentState = -1;
};

Component::Component(std::vector<std::shared_ptr<State>> &_states,
                     std::vector<std::shared_ptr<Transition>> &_transitions, std::string &_name,
                     int _currentState) {
    states = _states;
    transitions = _transitions;
    name = _name;
    if(_currentState == -1){
        for(int i = 0; i<states.size(); i++){
            if (states[i]->starting) {
                currentState = i;
                break;
            }
        }
    } else currentState = _currentState;
};

std::pair<int, int> Component::getDimensions() {
    return {(int) states.size(), (int) transitions.size()};
}

std::vector<std::shared_ptr<Transition>> Component::qualifiedTransitions() {
    auto qualifiedTransitions = std::vector<std::shared_ptr<Transition>>();

    for (const auto &transition : states[currentState]->outTransitions)
        if (transition->isQualified())
            qualifiedTransitions.push_back(transition);

    return qualifiedTransitions;
}

std::vector<std::shared_ptr<Transition>>
Component::qualifiedTransitions(std::shared_ptr<std::vector<std::string>> oFilters,std::shared_ptr<std::vector<std::string>> nFilters) {
    auto qualifiedTransitions = std::vector<std::shared_ptr<Transition>>();

    for (const auto &transition : states[currentState]->outTransitions)
       if (transition->isQualified()) {
           bool oFilterOk = transition->observabilityLabel.empty();
           oFilterOk |= oFilters == nullptr;
           if (!oFilterOk) oFilterOk |= std::find(oFilters->begin(), oFilters->end(), transition->observabilityLabel) != oFilters->end();

           bool nFilterOk = nFilters == nullptr;
           if (!nFilterOk) nFilterOk |= std::find(nFilters->begin(), nFilters->end(), transition->name) != nFilters->end();

           if (oFilterOk && nFilterOk) qualifiedTransitions.push_back(transition);
       }
    return qualifiedTransitions;
}

void Component::performTransition(std::shared_ptr<Transition> selectedTransition) {
    selectedTransition->perform();
    for (int i = 0; i < states.size(); i++)
        for (const auto &inTransition : states[i]->inTransitions)
            if(selectedTransition == inTransition){
                currentState = i;
                return;
            }
}

std::string Component::toString() {
    std::string result = "\t\t" + name + ":\n";
    result += "\t\t\tStates: \n";
    for (const auto &s : states)
        result += s->toString();
    result += "\t\t\tTransitions: \n";
    for (const auto &t : transitions)
        result += t->toString();
    return result;
}

void Component::reset() {
    for (int i = 0; i < states.size(); i++) {
        if (states[i]->starting) {
            currentState = i;
            return;
        }
    }
}

std::vector<std::shared_ptr<Transition>> Component::getTransitions(){
    return transitions;
}

