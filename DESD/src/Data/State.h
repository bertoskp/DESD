//
// Created by Diego Lorenzi on 24/07/18.
//

#ifndef AUTOMATAMANAGER_STATE_H
#define AUTOMATAMANAGER_STATE_H

#include "../stdIncludes.h"
#include "Transition.h"

class State {
public:
    std::string name;
    bool starting;
    std::vector<std::shared_ptr<Transition>> inTransitions;
    std::vector<std::shared_ptr<Transition>> outTransitions;

    State();

    State(std::string &_name, bool _starting, std::vector<std::shared_ptr<Transition>> &_inTransitions,
          std::vector<std::shared_ptr<Transition>> &_outTransitions);

    std::string toString();
};


#endif //AUTOMATAMANAGER_STATE_H
