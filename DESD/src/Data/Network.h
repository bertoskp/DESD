//
// Created by Diego Lorenzi on 24/07/18.
//

#ifndef AUTOMATAMANAGER_NETWORK_H
#define AUTOMATAMANAGER_NETWORK_H

#include "../stdIncludes.h"
#include "Component.h"

class Network {
public:
    std::vector<std::shared_ptr<Component>> components;
    std::vector<std::shared_ptr<Link>> links;

    Network() = default;

    Network(std::vector<std::shared_ptr<Component>> &_components, std::vector<std::shared_ptr<Link>> &_links);

    std::map<std::string, std::string> getComponentsState();
    std::map<std::string, std::string> getLinksState();
    void setState(std::map<std::string, std::string> componentsState, std::map<std::string, std::string> linksState);

    std::string toString();
    std::vector<std::shared_ptr<Component>> getComponents();
    void reset();
};


#endif //AUTOMATAMANAGER_NETWORK_H
