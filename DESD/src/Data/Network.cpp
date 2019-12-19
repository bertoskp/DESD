
#include <iostream>
#include "Network.h"

Network::Network(std::vector<std::shared_ptr<Component>> &_components, std::vector<std::shared_ptr<Link>> &_links) {
    components = _components;
    links = _links;
}

std::map<std::string, std::string> Network::getComponentsState(){
    std::map<std::string, std::string> state;
    for (const auto &component : components)
        state.insert({component->name, component->states[component->currentState]->name});

    return state;
};

std::map<std::string, std::string> Network::getLinksState(){
    std::map<std::string, std::string> state;
    for (const auto &link : links)
        state.insert({link->name, link->buffer.empty() ? "empty" : link->buffer});
    return state;
};

void Network::setState(std::map<std::string, std::string> componentsState, std::map<std::string, std::string> linksState){
    std::string stateName;
    for (const auto &component : components) {
        stateName = componentsState[component->name];
        for (int i = 0; i < (int)component->states.size(); i++) {
            if (component->states[i]->name == stateName) {
                component->currentState = i;
                break;
            }
        }
    }

    for (const auto &link : links) link->buffer = linksState[link->name];

}

std::string Network::toString() {
    std::string result;
    result = "Network:\n";
    result += "\tComponents:\n";
    for (const auto &c : components) result += c->toString();
    result += "\tLinks:\n";
    for (const auto &l : links) result += "\t\t" + l->toString() + "\n";
    return result;
}

void Network::reset() {
    for (const auto &c : components) c->reset();
    for (const auto &l : links) l->buffer = "empty";
}

std::vector<std::shared_ptr<Component>> Network::getComponents(){
    return components;
}
