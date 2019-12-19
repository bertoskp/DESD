
#include <iostream>
#include <utility>
#include "Transition.h"

Transition::Transition(std::string& _name, std::shared_ptr<Event>& _in, std::vector<std::shared_ptr<Event>> &_out) {
    name = std::move(_name);
    in = _in;
    out = _out;
}

 Transition::Transition(std::string& _name, std::shared_ptr<Event>& _in, std::vector<std::shared_ptr<Event>> &_out,
                       std::string& _relevancyLabel, std::string& _observabilityLabel)
    : Transition(_name, _in, _out){
    relevancyLabel = _relevancyLabel;
    observabilityLabel = _observabilityLabel;
}

void Transition::perform(){
    if (in != nullptr) in->link->buffer = "empty";
    if(!out.empty())
        for (const auto &e : out)
            e->link->buffer = e->name;
}

/*bool Transition::isQualified() {
    if (in == nullptr || in->link->buffer == in->name) {
        if(!out.empty()) {
            for (auto &e : out) {
                if (e->link->buffer != "empty" && e->link->buffer != in->name) {
                    return false;
                }
            }
        }
        return true;
    }
    return false;
}*/

bool Transition::isQualified() {
    if (in == nullptr || in->link->buffer == in->name) {
        if(!out.empty()) {
            for (auto &e : out) {
                if (e->link->buffer != "empty" ) {
                    /*if(in != nullptr) {
                        if (e->link->buffer != in->name) {
                            return false;
                        }
                    }*/
                    return false;
                }
            }
        }
        return true;
    }
    return false;
}

std::string Transition::toString() {
    std::string result;
    result = "\t\t\t\t" + name + ":\n";
    result += "\t\t\t\t\tin = " + (in != nullptr ? in->toString() : "---");
    result += "\n\t\t\t\t\tout = [ ";
    for (const auto &e : out)
        result += e->toString() + " ";
    result += "]\n";
    if (!observabilityLabel.empty()) result += "\t\t\t\t\tobservation = " + observabilityLabel + "\n";
    if (!relevancyLabel.empty()) result += "\t\t\t\t\trelevancy   = " + relevancyLabel + "\n";
    return result;
}

void Transition::setInEvent( std::shared_ptr<Event>&_in){
       in=_in;
   }

void Transition::setOutEvent( std::vector<std::shared_ptr<Event>>&_out){
       out=_out;
   }

std::string Transition::getObservabilityLabel(){
    return observabilityLabel;
}

