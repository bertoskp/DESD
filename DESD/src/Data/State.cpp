

#include <iostream>
#include <utility>
#include "State.h"

State::State(){
    starting = false;
};

State::State(std::string &_name, bool _starting, std::vector<std::shared_ptr<Transition>> &_inTransitions,
             std::vector<std::shared_ptr<Transition>> &_outTransitions) {
    name = _name;
starting = _starting;
    inTransitions = _inTransitions;
    outTransitions = _outTransitions;
}

std::string State::toString() {
    std::string result;
    result = "\t\t\t\t" + name + ":\n";
    if (starting) result += "\t\t\t\t\tStarting!\n";
    result += "\t\t\t\t\tin = [ ";
    for (auto &t : inTransitions) {
        if (t != nullptr && !t->name.empty())
            result += t->name + " ";
    }
    result += "]\n\t\t\t\t\tout = [ ";
    if(!outTransitions.empty())
        for (auto &t : outTransitions)
            result += t->name + " ";
    result += "]\n";
    return result;
}
