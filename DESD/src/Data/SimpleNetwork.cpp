

#include "SimpleNetwork.h"

SimpleNetwork::SimpleNetwork(std::vector<std::shared_ptr<SimpleState>> &_states,
                         std::vector<std::shared_ptr<SimpleTransition>> &_transitions) {
    states = _states;
    transitions = _transitions;

}

std::string SimpleNetwork::toString() {
    std::string result;
    result = "States: \n";
    for (const auto &s : states) {
        result += s->toString();
        result += "\n";
    }
    return result;
}

void SimpleNetwork::resetState() {
    for (const auto &s : states)
        if (s->starting) {
            currentState = s->id;
            return;
        }
    throw std::logic_error("No starting state found.");
}

std::shared_ptr<std::vector<std::string>> SimpleNetwork::getQualifiedLabels() {
    auto qualifiedLabels = std::make_shared<std::vector<std::string>>();
    for (const auto &state : states)
        if (state->id == currentState)
            for (const auto &t : state->out) qualifiedLabels->push_back(t->label);
    return qualifiedLabels;
}

void SimpleNetwork::performTransition(std::string &label) {
    for (const auto &state : states)
        if (state->id == currentState)
            for (const auto &t : state->out)
                if (t->label == label)
                    for (const auto &s2 : states)
                        for (const auto &t2 : s2->in)
                            if (t2->id == t->id) {
                                currentState = s2->id;
                                return;
                            }
}

bool SimpleNetwork::reachedFinalState() {
    for (const auto &state : states)
        if (state->id == currentState)
            return state->final;
    return false;
}
