

#include "BehavioralTransition.h"

unsigned int BehavioralTransition::nextId = 0;

BehavioralTransition::BehavioralTransition() {
    id = nextId++;
}

BehavioralTransition::BehavioralTransition(const std::string& _name) {
    name = std::move(_name);
    id = nextId++;
}

BehavioralTransition::BehavioralTransition(const std::string& _name, unsigned int _id) {
    name = std::move(_name);
    id = _id;
    if (id >= nextId) nextId = id + 1;
}

BehavioralTransition::BehavioralTransition(const std::string& _name, const std::string& _relevancyLabel,
                                           const std::string& _observabilityLabel)
    : BehavioralTransition(_name){
    observabilityLabel = std::move(_observabilityLabel);
    relevancyLabel = std::move(_relevancyLabel);

}

BehavioralTransition::BehavioralTransition(const std::string& _name, unsigned int _id, const std::string& _relevancyLabel,
                                           const std::string& _observabilityLabel) : BehavioralTransition(_name, _id) {
    observabilityLabel = std::move(_observabilityLabel);
    relevancyLabel = std::move(_relevancyLabel);
}

int BehavioralTransition::getId() {
    return id;
}

std::string BehavioralTransition::getObservabilityLabel() {
    return observabilityLabel;
}

std::string BehavioralTransition::toString() {
    std::string result;
    result = "{ ";
    if (!name.empty()) result += name + " ";
    if (!observabilityLabel.empty()) result += "obs:  " + observabilityLabel + " ";
    if (!relevancyLabel.empty()) result += "rel: " + relevancyLabel + " ";

    result += "}";

    return result;
}

std::string BehavioralTransition::getName(){
    return name;
}
std::string BehavioralTransition::getRelevancyLabel(){
    return relevancyLabel;
}


int BehavioralTransition::getDeepth(){
    return deepth;
}

void BehavioralTransition::setDeepth(int d){
     deepth=d;
}
