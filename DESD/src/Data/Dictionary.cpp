
#include "Dictionary.h"

Dictionary::Dictionary() {
    currentState = -1;
}

Dictionary::Dictionary(std::vector<std::shared_ptr<DictionaryState>> _states,
                       std::vector<std::shared_ptr<NFATransitionInterface>> _transitions) {
    states = _states;
    transitions = _transitions;
}

void Dictionary::setToInitialState() {
    for(int i = 0; i < states.size(); i++)
        if(states[i]->starting){
            currentState = i;
            break;
        }
}

std::shared_ptr<DictionaryState> Dictionary::getInitialState() {
    for (auto s : states)
        if (s->isStarting())
            return s;
    throw std::logic_error("Could not find Dictionary initial state");
}

void Dictionary::performTransition(std::shared_ptr<NFATransitionInterface> transition) {
    for(int i = 0; i < states.size(); i++){
        for(auto &inTransition : states[i]->in)
            if(inTransition == transition) {
                currentState = i;
                break;
            }
    }
}
void Dictionary::setTinfo(std::shared_ptr<std::vector<std::shared_ptr<transitionsInfo>>> &_tInfo) {
    tInfo=_tInfo;
}

void Dictionary::setTinfoDic(std::shared_ptr<std::vector<std::shared_ptr<transitionsInfo>>> &_tInfoDic) {
    tInfoDic=_tInfoDic;
}
void Dictionary::setBhState(std::vector<std::shared_ptr<NFAStateInterface>> &_bhSt) {
    bhState=_bhSt;
}

std::vector<std::shared_ptr<NFATransitionInterface>> Dictionary::getTransitions() {
    std::vector<std::shared_ptr<NFATransitionInterface>> output;
    for (const auto &t : transitions) output.push_back(t);
    return output;
}
std::vector<std::shared_ptr<NFATransitionInterface>> Dictionary::getBehavioralTransitions() {
    std::vector<std::shared_ptr<NFATransitionInterface>> output;
    for (const auto &t : behavioralTransitions) output.push_back(t);
    return output;
}


void Dictionary::setBehavioralTransitions(std::vector<std::shared_ptr<NFATransitionInterface>>  & _behavioralTransitions){
    behavioralTransitions=_behavioralTransitions;
}

std::vector<std::shared_ptr<NFAStateInterface>> Dictionary::getStates() {
    std::vector<std::shared_ptr<NFAStateInterface>> output;
    for (const auto &s : states) output.push_back(s);
    return output;
}

void Dictionary::addBehavioralTransition(std::shared_ptr<BehavioralTransition> &behavioralTransition) {
    behavioralTransitions.push_back(behavioralTransition);

}

