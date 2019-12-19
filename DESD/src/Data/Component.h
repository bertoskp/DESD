//
// Created by Diego Lorenzi on 24/07/18.
//

#ifndef AUTOMATAMANAGER_COMPONENT_H
#define AUTOMATAMANAGER_COMPONENT_H

#include "../stdIncludes.h"
#include "State.h"

class Component {
public:
    std::vector<std::shared_ptr<State>> states;
    std::vector<std::shared_ptr<Transition>> transitions;
    std::string name;
    int currentState;

    Component();

    //inizializza il componente, se lo stato corrente non viene specificato esso è impostato allo stato iniziale
    //precondizione: (almeno) uno degli stati è contrassegnato come iniziale
    Component(std::vector<std::shared_ptr<State>> &_states, std::vector<std::shared_ptr<Transition>> &_transitions,
              std::string &_name,
              int _currentState = -1);

    std::pair<int, int> getDimensions();

    std::vector<std::shared_ptr<Transition>> qualifiedTransitions();

    std::vector<std::shared_ptr<Transition>> qualifiedTransitions(std::shared_ptr<std::vector<std::string>> observabilityFilters, std::shared_ptr<std::vector<std::string>> nameFilters);

    void performTransition(std::shared_ptr<Transition> selectedTransition);

    std::string toString();

    void reset();
    
    std::vector<std::shared_ptr<Transition>> getTransitions();
};


#endif //AUTOMATAMANAGER_COMPONENT_H
