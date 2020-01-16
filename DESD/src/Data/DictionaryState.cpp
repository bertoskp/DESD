

#include "DictionaryState.h"

unsigned int DictionaryState::nextId = 0;

DictionaryState::DictionaryState() {
    id = nextId++;
}

DictionaryState::DictionaryState(bool _final, bool _starting) : DictionaryState() {

    final = _final;
    starting = _starting;
}

DictionaryState::DictionaryState(bool _final, bool _starting,
                                 const std::vector<std::shared_ptr<NFATransitionInterface>>& _in,
                                 const std::vector<std::shared_ptr<NFATransitionInterface>>& _out,
                                 const std::set<std::set<std::string>>& _diagnosis,
                                /* const std::vector<std::vector<std::shared_ptr<NFAStateInterface>>>& _pairs*/
                                 const std::vector<std::vector<unsigned int>> &_pairs) : DictionaryState(_final, _starting){
    in = _in;
    out = _out;
    diagnosis = _diagnosis;
    pairs=_pairs;
}

DictionaryState::DictionaryState(bool _final, bool _starting,
                                 const std::vector<std::shared_ptr<NFATransitionInterface>>& _in,
                                 const std::vector<std::shared_ptr<NFATransitionInterface>>& _out,
                                 const std::set<std::set<std::string>>& _diagnosis,
                                 /*const std::vector<std::vector<std::shared_ptr<NFAStateInterface>>>& _pairs*/
                                 const std::vector<std::vector<unsigned int>> &_pairs , unsigned int _id) : DictionaryState(_final, _starting, _in, _out, _diagnosis, _pairs){
    id = _id;
    if (nextId <= id) nextId = id + 1;
}

std::string DictionaryState::toString() {
    std::string result;
    std::string name = "\t";
    result = std::to_string(id) + ": { ";
    if (starting) result += "starting, ";
    if (final) result += "final, ";
    result += "in = [ ";
    for (const auto &t : in) {
        if (t != nullptr)
            result += t->toString();
    }
    result += "], out = [ ";
    if(!out.empty())
        for (auto &t : out)
            result += t->toString();
    result += "], ";
    result += "labels = [ ";
    if(!diagnosis.empty())
        for (auto const &lVec : diagnosis){
            result += "{";
            for(auto const &l : lVec){
                result += l + " ";
            }
            result += "}";
    }

    result += "] }\n";

    return result;
}

bool DictionaryState::isStarting() {
    return starting;
}

bool DictionaryState::isFinal() {
    return final;
}

int DictionaryState::getIdd() {
    return id;
}


std::map<std::string, std::string> DictionaryState:: getComponentsState(){
    std::map<std::string, std::string> n;
    return n ;
}

std::map<std::string, std::string>DictionaryState:: getLinksState(){
    std::map<std::string, std::string> n;
    return  n;
}

std::vector<std::shared_ptr<BehavioralTransition>>DictionaryState:: GetIn(){

    std::vector<std::shared_ptr<BehavioralTransition>> n;
    return n;
}


std::vector<std::shared_ptr<BehavioralTransition>>DictionaryState:: GetOut(){
    std::vector<std::shared_ptr<BehavioralTransition>> n;
    return n;
}
std::set<std::string> DictionaryState::getRelevancyLabels(){
    std::set<std::string> s;
    return s;
}
bool DictionaryState:: getStarting(){
return true;
}
int DictionaryState:: getObservationState(){
return true;
}
int DictionaryState:: getBondState(){
    return 1;
}
bool DictionaryState:: getFinal(){
    return true;
}

int DictionaryState::getNextId(){
    return 1;
}
std::vector<std::shared_ptr<NFATransitionInterface>> DictionaryState::getInTransitions() {
    std::vector<std::shared_ptr<NFATransitionInterface>> output;
    for (const auto &t : in) output.push_back(t);
    return output;
}

std::vector<std::shared_ptr<NFATransitionInterface>> DictionaryState::getOutTransitions() {
    std::vector<std::shared_ptr<NFATransitionInterface>> output;
    for (const auto &t : out) output.push_back(t);
    return output;
}




std::set<std::set<std::string>> DictionaryState::getDiagnosis() {
    return diagnosis;
}


void DictionaryState::setNfaState(std::set<std::shared_ptr<NFAStateInterface>> &_nfaStates){
    nfaStates=_nfaStates;
    std::vector<std::shared_ptr<BehavioralState>> bh;
    for ( auto s: (nfaStates)){


    }
};

void DictionaryState::setNfaState2(std::vector<std::shared_ptr<BehavioralState>> &_nfaStates){
    nfaStates2=_nfaStates;
};


void DictionaryState::addInTransition(std::shared_ptr<BehavioralTransition> transizioneOsservabile){
    //int i=0;
};
void DictionaryState::addOutTransition(std::shared_ptr<BehavioralTransition> transizioneOsservabile){
   // int i=0;
};
void DictionaryState::removeInTransition(std::shared_ptr<BehavioralTransition> transizione) {
   // int i=0;
};

void DictionaryState::setDiangosis(std::set<std::set<std::string>> dia){
    diagnosis=dia;
}
