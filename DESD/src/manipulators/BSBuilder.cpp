

#include <iostream>
#include "BSBuilder.h"


std::shared_ptr<BehavioralSpace> BSBuilder::build(std::shared_ptr<Network> network, std::shared_ptr<long> maxExecTime,
                                                  std::shared_ptr<SimpleNetwork> observation,
                                                  std::shared_ptr<SimpleNetwork> bond) {

    Logger::section("Behavioral Space Construction");
    std::string filters;
    if (observation) filters += "Observation ";
    if (bond) filters += "Bond";

    if (filters.empty()) Logger::log("No filters.");
    else Logger::log("Selected filters: " + filters);

    auto startingTime = std::chrono::high_resolution_clock::now();
    // resetta la rete allo stato iniziale
    network->reset();
    BehavioralState::nextId = 0;
    BehavioralTransition::nextId = 0;


    // acquisisce lo stato (iniziale) della rete
    std::map<std::string, std::string> componentsState = network->getComponentsState();
    std::map<std::string, std::string> linksState = network->getLinksState();

    if (observation) observation->resetState();
    if (bond) bond->resetState();

    std::shared_ptr<BehavioralState> currentState = std::make_shared<BehavioralState>(componentsState, linksState, true, true);
    if (observation) currentState->observationState = observation->currentState;
    if (bond) currentState->bondState = bond->currentState;

    // inserisce lo stato iniziale nel vettore e nell'automa
    std::queue<std::shared_ptr<BehavioralState>> toVisit;
    auto space = std::make_shared<BehavioralSpace>();
    toVisit.push(currentState);
    space->states.push_back(currentState);

    bool alreadyPresent;

    while (!toVisit.empty()) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();

        if(maxExecTime && elapsed > *maxExecTime){
            std::vector<std::string> errorMessages = {
                "Maximum execution time exceeded during behavioral space construction! (" + std::to_string(elapsed) + ")",
                "  Processed states: " + std::to_string(space->states.size()),
                "  Processed transitions: " + std::to_string(space->transitions.size()),
                "  States in processing queue: " + std::to_string(toVisit.size())
            };
            Logger::err(errorMessages);
            throw std::runtime_error("execution time");
        }
        // prende il primo stato del vettore
        currentState = toVisit.front();
        network->setState(currentState->componentsState, currentState->linksState);
        if (observation) observation->currentState = currentState->observationState;
        if (bond) bond->currentState = currentState->bondState;

        for (const auto &component : network->components) {
            // se una delle transizioni è abilitata la fa scattare
            std::vector<std::shared_ptr<Transition>> qualifiedTransitions = component->qualifiedTransitions(
                    observation ? observation->getQualifiedLabels() : nullptr,
                    bond ? bond->getQualifiedLabels() : nullptr);

            for (const auto &transition : qualifiedTransitions) {
                alreadyPresent = false;

                // acquisisce lo stato così raggiunto dalla rete
                component->performTransition(transition);
                componentsState = network->getComponentsState();
                linksState = network->getLinksState();

                if (observation) observation->performTransition(transition->observabilityLabel);
                if (bond) bond->performTransition(transition->name);

                // crea una nuova transizione dello spazio comportamentale e la aggiunge come transizione in uscita allo
                // stato corrente e all'automa
                auto newTransition = std::make_shared<BehavioralTransition>(transition->name,
                                                                            transition->relevancyLabel,
                                                                            transition->observabilityLabel);

                space->transitions.push_back(newTransition);
                currentState->out.push_back(newTransition);

                // se lo stato raggiunto è già nel grafo aggiunge la transizione a quelle in ingresso
                for (const auto &state : space->states) {
                    bool condition = state->componentsState == componentsState && state->linksState == linksState;
                    if (observation) condition &= state->observationState == observation->currentState;
                    if (bond) condition &= state->bondState == bond->currentState;
                    if (condition) {
                        alreadyPresent = true;
                        state->in.push_back(newTransition);
                        break;
                    }
                }

                // se lo stato raggiunto non è già presente lo crea, vi aggiunge la transizione in ingresso e lo aggiunge
                // al vettore e all'automa
                if (!alreadyPresent) {
                    bool final = true;
                    for (auto linkState : linksState)
                        if (linkState.second != "empty")
                            final = false;
                    if (observation && !observation->reachedFinalState()) final = false;
                    auto newState = std::make_shared<BehavioralState>(componentsState, linksState, false, final);
                    newState->in.push_back(newTransition);
                    if (observation) newState->observationState = observation->currentState;
                    if (bond) newState->bondState = bond->currentState;
                    toVisit.push(newState);
                    space->states.push_back(newState);
                }

                // torna allo stato precedente prima di controllare il componente successivo
                network->setState(currentState->componentsState, currentState->linksState);
                if (observation) observation->currentState = currentState->observationState;
                if (bond) bond->currentState = currentState->bondState;
            }
        }
        // elimina lo stato corrente dal vettore, da questo momento ad esso si possono solo aggiungere nuove transizioni
        // in ingresso
        toVisit.pop();
    }
    int nFinal=0;
    std::pair<int, int> dimensions = space->getDimensions();
    for (auto s :space->states){
        if( s->final){
            nFinal++;
        }
    }
    std::vector<std::string> messages = {
        "BS construction completed.",
        "  States: " + std::to_string(dimensions.first),
         " Final States: "+ std::to_string(nFinal),
        "  Transitions: " + std::to_string(dimensions.second)
    };
    std::string info="BS construction completed.\n  States: " + std::to_string(dimensions.first)+"\n Final States: "+ std::to_string(nFinal)+
" \n Transitions: " + std::to_string(dimensions.second)+"\n";
     std::cout << info;
    Logger::log(messages);

    if (maxExecTime) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        *maxExecTime -= elapsed;
    }
    if(bond!= nullptr){
        unsigned int fin = 0;
        for (auto s:bond->states){
            if( s->final){
                fin=s->id;

            }
        }
        pruneBond(space,maxExecTime,fin);

    }

    return space;
}

void BSBuilder::prune(std::shared_ptr<BehavioralSpace> space, std::shared_ptr<long> maxExecTime) {
    std::cout << "Pruning...";
    std::string message;
    auto startingTime = std::chrono::high_resolution_clock::now();

    Logger::section("Behavioral Space Pruning");
    bool repeat = true;
    bool removedTransition;
    int prunedStates = 0, prunedTransitions = 0;

    // ripete finchè non smette di rimuovere stati
    while(repeat) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        if(maxExecTime && elapsed > *maxExecTime){
            std::vector<std::string> errorMessages = {
                "Maximum execution time exceeded during space pruning! (" + std::to_string(elapsed) + ")",
                "  Pruned states: " + std::to_string(prunedStates),
                "  Pruned transitions: " + std::to_string(prunedTransitions),
            };
            Logger::err(errorMessages);
            throw std::runtime_error("execution time");
        }
        repeat = false;
        for (auto iterator = space->states.begin(); iterator != space->states.end();) {
            if (!(*iterator)->final && (*iterator)->out.empty()) {
                auto inTransitions = (*iterator)->in;
                prunedTransitions += inTransitions.size();
                prunedStates++;

                // elimina le transizioni in ingresso allo stato da quelle in uscita dagli stati predecessori
                for (const auto &innerState : space->states) {
                    for(int i = 0; i < innerState->out.size();){
                        removedTransition = false;
                        for (const auto &transition : inTransitions) {
                            if (innerState->out[i] == transition) {
                                innerState->out.erase(innerState->out.begin() + i);
                                removedTransition = true;
                                break;
                            }
                        }
                        if(!removedTransition)
                            i++;
                    }
                }

                // elimina le transizioni in ingresso allo stato dal vettore di transizioni
                for (int i = 0; i < space->transitions.size(); i++) {
                    for (const auto &inTransition : inTransitions)
                        if (space->transitions[i] == inTransition)
                            space->transitions.erase(space->transitions.begin() + i);
                }
                repeat = true;
                iterator = space->states.erase(iterator);
            } else iterator++;
        }
    }

    std::vector<std::string> messages = {
        "Pruning completed.",
        "  Pruned states: " + std::to_string(prunedStates),
        "  Pruned transitions: " + std::to_string(prunedTransitions)
    };
    std::string mess= "Pruning completed.\n  Pruned states: " + std::to_string(prunedStates)+"\n Pruned transitions: " + std::to_string(prunedTransitions);
    std::cout << mess;
    Logger::log(messages);

    if (maxExecTime) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        *maxExecTime -= elapsed;
    }
}



void BSBuilder::pruneBond(std::shared_ptr<BehavioralSpace> space, std::shared_ptr<long> maxExecTime, unsigned int fin) {

    std::string message;
    auto startingTime = std::chrono::high_resolution_clock::now();

    Logger::section("Behavioral Space Pruning");
    bool repeat = true;
    bool removedTransition;
    int prunedStates = 0, prunedTransitions = 0;

    // ripete finchè non smette di rimuovere stati
    while(repeat) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        if(maxExecTime && elapsed > *maxExecTime){
            std::vector<std::string> errorMessages = {
                    "Maximum execution time exceeded during space pruning! (" + std::to_string(elapsed) + ")",
                    "  Pruned states: " + std::to_string(prunedStates),
                    "  Pruned transitions: " + std::to_string(prunedTransitions),
            };
            Logger::err(errorMessages);
            throw std::runtime_error("execution time");
        }
        repeat = false;
        for (auto iterator = space->states.begin(); iterator != space->states.end();) {
            if ((*iterator)->id==5){
                
            }
            //bool cond=(*iterator)->bondState!=fin;

            if (((*iterator)->bondState!=fin) && (*iterator)->out.empty()) {
                auto inTransitions = (*iterator)->in;
                prunedTransitions += inTransitions.size();
                prunedStates++;

                // elimina le transizioni in ingresso allo stato da quelle in uscita dagli stati predecessori
                for (const auto &innerState : space->states) {
                    for(int i = 0; i < innerState->out.size();){
                        removedTransition = false;
                        for (const auto &transition : inTransitions) {
                            if (innerState->out[i] == transition) {
                                innerState->out.erase(innerState->out.begin() + i);
                                removedTransition = true;
                                break;
                            }
                        }
                        if(!removedTransition)
                            i++;
                    }
                }

                // elimina le transizioni in ingresso allo stato dal vettore di transizioni
                for (int i = 0; i < space->transitions.size(); i++) {
                    for (const auto &inTransition : inTransitions)
                        if (space->transitions[i] == inTransition)
                            space->transitions.erase(space->transitions.begin() + i);
                }
                repeat = true;
                iterator = space->states.erase(iterator);
            } else iterator++;
        }
    }

    std::vector<std::string> messages = {
            "Pruning completed.",
            "  Pruned states: " + std::to_string(prunedStates),
            "  Pruned transitions: " + std::to_string(prunedTransitions)
    };
    Logger::log(messages);

    if (maxExecTime) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        *maxExecTime -= elapsed;
    }
}





std::shared_ptr<BehavioralSpace> BSBuilder::buildFromDFA(std::shared_ptr<Network> network, std::shared_ptr<long> maxExecTime,
                                                  std::shared_ptr<SimpleNetwork> observation,
                                                  std::shared_ptr<SimpleNetwork> bond) {

    Logger::section("Behavioral Space Construction");
    std::string filters;
    if (observation) filters += "Observation ";
    if (bond) filters += "Bond";

    if (filters.empty()) Logger::log("No filters.");
    else Logger::log("Selected filters: " + filters);

    auto startingTime = std::chrono::high_resolution_clock::now();
    // resetta la rete allo stato iniziale
    network->reset();
    BehavioralState::nextId = 0;
    BehavioralTransition::nextId = 0;


    // acquisisce lo stato (iniziale) della rete
    std::map<std::string, std::string> componentsState = network->getComponentsState();
    std::map<std::string, std::string> linksState = network->getLinksState();

    if (observation) observation->resetState();
    if (bond) bond->resetState();

    std::shared_ptr<BehavioralState> currentState = std::make_shared<BehavioralState>(componentsState, linksState, true, true);
    if (observation) currentState->observationState = observation->currentState;
    if (bond) currentState->bondState = bond->currentState;

    // inserisce lo stato iniziale nel vettore e nell'automa
    std::queue<std::shared_ptr<BehavioralState>> toVisit;
    auto space = std::make_shared<BehavioralSpace>();
    toVisit.push(currentState);
    space->states.push_back(currentState);

    bool alreadyPresent;

    while (!toVisit.empty()) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();

        if(maxExecTime && elapsed > *maxExecTime){
            std::vector<std::string> errorMessages = {
                    "Maximum execution time exceeded during behavioral space construction! (" + std::to_string(elapsed) + ")",
                    "  Processed states: " + std::to_string(space->states.size()),
                    "  Processed transitions: " + std::to_string(space->transitions.size()),
                    "  States in processing queue: " + std::to_string(toVisit.size())
            };
            Logger::err(errorMessages);
            throw std::runtime_error("execution time");
        }
        // prende il primo stato del vettore
        currentState = toVisit.front();
        network->setState(currentState->componentsState, currentState->linksState);
        if (observation) observation->currentState = currentState->observationState;
        if (bond) bond->currentState = currentState->bondState;

        for (const auto &component : network->components) {
            // se una delle transizioni è abilitata la fa scattare
            std::vector<std::shared_ptr<Transition>> qualifiedTransitions = component->qualifiedTransitions(
                    observation ? observation->getQualifiedLabels() : nullptr,
                    bond ? bond->getQualifiedLabels() : nullptr);

            for (const auto &transition : qualifiedTransitions) {
                alreadyPresent = false;

                // acquisisce lo stato così raggiunto dalla rete
                component->performTransition(transition);
                componentsState = network->getComponentsState();
                linksState = network->getLinksState();

                if (observation) observation->performTransition(transition->observabilityLabel);
                if (bond) bond->performTransition(transition->name);

                // crea una nuova transizione dello spazio comportamentale e la aggiunge come transizione in uscita allo
                // stato corrente e all'automa
                auto newTransition = std::make_shared<BehavioralTransition>(transition->name,
                                                                            transition->relevancyLabel,
                                                                            transition->observabilityLabel);

                space->transitions.push_back(newTransition);
                currentState->out.push_back(newTransition);

                // se lo stato raggiunto è già nel grafo aggiunge la transizione a quelle in ingresso
                for (const auto &state : space->states) {
                    bool condition = state->componentsState == componentsState && state->linksState == linksState;
                    if (observation) condition &= state->observationState == observation->currentState;
                    if (bond) condition &= state->bondState == bond->currentState;
                    if (condition) {
                        alreadyPresent = true;
                        state->in.push_back(newTransition);
                        break;
                    }
                }

                // se lo stato raggiunto non è già presente lo crea, vi aggiunge la transizione in ingresso e lo aggiunge
                // al vettore e all'automa
                if (!alreadyPresent) {
                    bool final = true;
                    for (auto linkState : linksState)
                        if (linkState.second != "empty")
                            final = false;
                    if (observation && !observation->reachedFinalState()) final = false;
                    auto newState = std::make_shared<BehavioralState>(componentsState, linksState, false, final);
                    newState->in.push_back(newTransition);
                    if (observation) newState->observationState = observation->currentState;
                    if (bond) newState->bondState = bond->currentState;
                    toVisit.push(newState);
                    space->states.push_back(newState);
                }

                // torna allo stato precedente prima di controllare il componente successivo
                network->setState(currentState->componentsState, currentState->linksState);
                if (observation) observation->currentState = currentState->observationState;
                if (bond) bond->currentState = currentState->bondState;
            }
        }
        // elimina lo stato corrente dal vettore, da questo momento ad esso si possono solo aggiungere nuove transizioni
        // in ingresso
        toVisit.pop();
    }
    std::pair<int, int> dimensions = space->getDimensions();
    std::vector<std::string> messages = {
            "BS construction completed.",
            "  States: " + std::to_string(dimensions.first),
            "  Transitions: " + std::to_string(dimensions.second)
    };
    Logger::log(messages);

    if (maxExecTime) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        *maxExecTime -= elapsed;
    }
    return space;
}
