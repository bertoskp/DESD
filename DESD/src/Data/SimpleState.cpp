#include "SimpleState.h"

SimpleState::SimpleState(unsigned int _id, const std::vector<std::shared_ptr<SimpleTransition>> &_in,
                         const std::vector<std::shared_ptr<SimpleTransition>> &_out, bool _final, bool _starting) {
    id = _id;
    in = _in;
    out = _out;
    starting = _starting;
    final = _final;
}

SimpleState::SimpleState(unsigned int _id, bool _final, bool _starting) {
    id = _id;
    starting = _starting;
    final = _final;
}

std::string SimpleState::toString() {
    std::string result;
    result = "\tid: " + std::to_string(id);
    result += "\n";
    if (starting)  result += "\tstarting.\n";
    if (final) result += "\tfinal.\n";
    result += "\tin: [ ";
    for (auto &t : in) {
        result += t->toString();
        result += " ";
    }
    result += "]\n";
    result += "\tout: [ ";
    for (auto &t : out) {
        result += t->toString();
        result += " ";
    }
    result += "]\n";

    return result;
}
