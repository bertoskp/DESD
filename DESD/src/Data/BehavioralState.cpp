
#include <iostream>
#include "BehavioralState.h"



unsigned int BehavioralState::nextId = 0;

BehavioralState::BehavioralState() {
    id = nextId++;
    starting = false;
}
/*
BehavioralState::BehavioralState(){
    starting = false;
};*/

BehavioralState::BehavioralState(std::map<std::string, std::string>& _componentsState,
                                 std::map<std::string, std::string> &_linksState, bool _starting, bool _final) {
    componentsState = _componentsState;
    linksState = _linksState;
    starting = _starting;
    final = _final;
    observationState = -1;
    bondState = -1;
    id = nextId++;
}

BehavioralState::BehavioralState(std::map<std::string, std::string> &_componentsState,
                                 std::map<std::string, std::string> &_linksState, bool _starting, bool _final,
                                 std::vector<std::shared_ptr<BehavioralTransition>> &_in,
                                 std::vector<std::shared_ptr<BehavioralTransition>> &_out)
        : BehavioralState(_componentsState, _linksState, _starting, _final) {
    in = _in;
    out = _out;
}

BehavioralState::BehavioralState(std::map<std::string, std::string>& _componentsState,
                                 std::map<std::string, std::string> &_linksState, bool _starting, bool _final,
                                 std::set<std::string> &_relevancyLabels) : BehavioralState(_componentsState,
                                                                                            _linksState, _starting,
                                                                                            _final) {
    relevancyLabels = _relevancyLabels;
}

BehavioralState::BehavioralState(std::map<std::string, std::string>& _componentsState,
                                 std::map<std::string, std::string> &_linksState, bool _starting, bool _final,
                                 std::vector<std::shared_ptr<BehavioralTransition>> &_in,
                                 std::vector<std::shared_ptr<BehavioralTransition>> &_out,
                                 std::set<std::string>& _relevancyLabels): BehavioralState(_componentsState, _linksState, _starting, _final, _in, _out) {
    relevancyLabels = _relevancyLabels;

}

BehavioralState::BehavioralState(std::map<std::string, std::string>& _componentsState,
                                 std::map<std::string, std::string> &_linksState, bool _starting, bool _final,
                                 std::vector<std::shared_ptr<BehavioralTransition>> &_in,
                                 std::vector<std::shared_ptr<BehavioralTransition>> &_out,
                                 std::set<std::string>& _relevancyLabels,unsigned int _id)
        : BehavioralState(_componentsState, _linksState, _starting, _final, _in, _out) {
    id = _id;
    if (nextId <= id) nextId = id + 1;
    relevancyLabels = _relevancyLabels;
}

BehavioralState::BehavioralState(std::map<std::string, std::string>& _componentsState,
                                 std::map<std::string, std::string> &_linksState, bool _starting, bool _final,
                                 std::vector<std::shared_ptr<BehavioralTransition>> &_in,
                                 std::vector<std::shared_ptr<BehavioralTransition>> &_out,
                                 unsigned int _id)
        : BehavioralState(_componentsState, _linksState, _starting, _final, _in, _out) {
    id = _id;
    if (nextId <= id) nextId = id + 1;
}


BehavioralState::BehavioralState(std::map<std::string, std::string>& _componentsState,
                                 std::map<std::string, std::string> &_linksState, bool _starting, bool _final,unsigned int _id) : BehavioralState(_componentsState, _linksState, _starting, _final) {
    id = _id;
    if (nextId <= id) nextId = id + 1;
}

bool BehavioralState::isStarting() {
    return starting;
}

bool BehavioralState::isFinal() {
    return final;
}

std::vector<std::shared_ptr<NFATransitionInterface>> BehavioralState::getInTransitions() {
    std::vector<std::shared_ptr<NFATransitionInterface>> output;
    for (const auto &t : in) output.push_back(t);
    return output;
}

std::vector<std::shared_ptr<NFATransitionInterface>> BehavioralState::getOutTransitions() {
    std::vector<std::shared_ptr<NFATransitionInterface>> output;
    for (const auto &t : out) output.push_back(t);
    
    return output;
}


void BehavioralState::addInTransition(std::shared_ptr<BehavioralTransition> transizioneOsservabile) {
    in.push_back(transizioneOsservabile);
}

void BehavioralState::addOutTransition(std::shared_ptr<BehavioralTransition> transizioneOsservabile) {
    out.push_back(transizioneOsservabile);
}

std::vector<std::shared_ptr<BehavioralTransition>> BehavioralState::getOutTransitions2() {
    std::vector<std::shared_ptr<BehavioralTransition>> output;
    for (const auto &t : out) output.push_back(t);
    return output;
}

std::set<std::set<std::string>> BehavioralState::getDiagnosis() {
    std::set<std::set<std::string>> output;
    output.insert(relevancyLabels);
    return output;
}

std::string BehavioralState::toString() {
    std::string result;

    std::string name = "\t";
    for(auto component : componentsState)
        name += component.second + " ";
    for(auto link: linksState)
        name += link.second + " ";
    if (observationState != -1) name += std::to_string(observationState);
    if (bondState != -1) name += std::to_string(bondState);

    result = name + ": { ";
    if (starting) result += "starting, ";
    if (final) result += "final, ";
    result += "in = [ ";
    for (const auto &t : in) {
        if (t != nullptr && !t->name.empty())
            result += t->name + "(" + t->observabilityLabel + ") ";
    }
    result += "], out = [ ";
    if(!out.empty())
        for (const auto &t : out)
            result += t->name + "(" + t->observabilityLabel + ") ";
    result += "], ";
    result += "labels = [ ";
    if(!relevancyLabels.empty())
        for (auto const &l : relevancyLabels)
            result += l + " ";
    result += "] }\n";

    return result;
}



int BehavioralState::getIdd() {
    return id;
}



std::map<std::string, std::string> BehavioralState:: getComponentsState(){

    return componentsState ;
}

std::map<std::string, std::string>  BehavioralState::getLinksState(){

    return  linksState;
}

std::vector<std::shared_ptr<BehavioralTransition>>BehavioralState:: GetIn(){


    return in;
}


std::vector<std::shared_ptr<BehavioralTransition>> BehavioralState::GetOut(){

    return out;
}
std::set<std::string>BehavioralState:: getRelevancyLabels(){

    return relevancyLabels;
}
bool BehavioralState::getStarting(){
    return starting;
}
int BehavioralState::getObservationState(){
    return observationState;
}
int BehavioralState:: getBondState(){
    return bondState;
}
bool BehavioralState::getFinal(){
    return final;
}


int BehavioralState::getNextId(){
    return nextId;
}


/*
void BehavioralState::addInT(NFATransitionInterface i){
    in.push_back(i)
}
*/

void BehavioralState::resetId(){
    nextId=0;
}
void BehavioralState::removeInTransition(std::shared_ptr<BehavioralTransition> transizione){
    int i=0;
    for (auto tra :in){
        if (tra==transizione){
            in.erase(in.begin()+i);
            break;

        }
        i++;
    }
}
