

#include "LabeledBSBuilder.h"

std::shared_ptr<BehavioralSpace> LabeledBSBuilder::build(std::shared_ptr<BehavioralSpace> bs,
                                                         std::shared_ptr<long> maxExecTime) {
    auto labeledSpace = std::make_shared<BehavioralSpace>();
    auto startingTime = std::chrono::high_resolution_clock::now();

    Logger::section("Behavioral Space Labeling");

    std::queue<std::pair<std::shared_ptr<BehavioralState>, int>> toVisit;

    // conta le prossime transizioni a partire da 0
    BehavioralTransition::nextId = 0;

    // acquisisce lo stato iniziale dello spazio comportamentale
    bs->setToInitialState();

    auto initialState = bs->getCurrentState();
    initialState->resetId();
    auto firstState = std::make_shared<BehavioralState>(initialState->componentsState, initialState->linksState,
                                                        initialState->starting, initialState->final);
    firstState->observationState = initialState->observationState;

    // inserisce lo stato iniziale nella mappa e nell'automa
    toVisit.push({firstState, bs->currentState});
    labeledSpace->states.push_back(firstState);

    while (!toVisit.empty()) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        if(maxExecTime && elapsed > *maxExecTime){
            std::vector<std::string> errorMessages = {
                "Maximum execution time exceeded during behavioral space labeling! (" + std::to_string(elapsed) + ")",
                "  Processed states: " + std::to_string(labeledSpace->states.size()),
                "  Processed transitions: " + std::to_string(labeledSpace->transitions.size()),
                "  States in processing queue: " + std::to_string(toVisit.size())
            };
            Logger::err(errorMessages);
            throw std::runtime_error("execution time");
        }
        // prende il primo stato della mappa, e lo usa per inizializzare lo spazio comportamentale non decorato
        auto currentState = toVisit.front().first;
        bs->currentState = toVisit.front().second;

        // fa scattare tutte le transizioni in uscita dallo stato corrente dello spazio comportamentale non decorato
        auto currentTransitions = bs->getCurrentState()->out;

        int startingState = toVisit.front().second;

        for (auto &transition : currentTransitions) {

            bs->currentState = startingState;
            bs->performTransition(transition);

            // acquisisce lo stato così raggiunto dalla rete
            auto componentsState = bs->getCurrentState()->componentsState;
            auto linksState = bs->getCurrentState()->linksState;
            auto newRelevancyLabels = currentState->relevancyLabels;
            bool final = bs->states[bs->currentState]->final;

            auto observationState = bs->getCurrentState()->observationState;

            // se ha acquisito una nuova etichetta di rilevanza, la aggiunge allo stato corrente
            if (!transition->relevancyLabel.empty())
                newRelevancyLabels.insert(transition->relevancyLabel);

            // crea una nuova transizione dello spazio comportamentale decorato e la aggiunge come transizione in uscita allo
            // stato corrente e all'automa
            auto newTransition = std::make_shared<BehavioralTransition>(transition->name, transition->relevancyLabel,
                                                                        transition->observabilityLabel);

            labeledSpace->transitions.push_back(newTransition);
            currentState->out.push_back(newTransition);

            bool alreadyPresent = false;
            // se lo stato raggiunto è già nel grafo aggiunge la transizione a quelle in ingresso
            for (const auto &state : labeledSpace->states) {
                bool condition = state->componentsState == componentsState && state->linksState == linksState &&
                                 state->relevancyLabels == newRelevancyLabels;
                if (observationState != -1) condition = condition && state->observationState == observationState;
                if (condition) {
                    alreadyPresent = true;
                    newTransition->setStato(state);

                    state->in.push_back(newTransition);
                    break;
                }
            }


            // se lo stato raggiunto non è già presente lo crea, vi aggiunge la transizione in ingresso e lo aggiunge
            // al vettore e all'automa
            if (!alreadyPresent) {
                auto newState = std::make_shared<BehavioralState>(componentsState, linksState, false, final,
                                                                  newRelevancyLabels);
                if (observationState != -1) newState->observationState = observationState;
                newState->in.push_back(newTransition);
                newTransition->setStato(newState);

                toVisit.push({newState, bs->currentState});
                labeledSpace->states.push_back(newState);
            }
        }

        // elimina lo stato corrente dalla mappa, da questo momento ad esso si possono solo aggiungere nuove transizioni
        // in ingresso
        toVisit.pop();
    }
    auto dimensions = labeledSpace->getDimensions();
    std::vector<std::string> messages = {
            "Labeled BS construction completed.",
            "  States: " + std::to_string(dimensions.first),
            "  Transitions: " + std::to_string(dimensions.second)
    };
    Logger::log(messages);

    if (maxExecTime) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        *maxExecTime -= elapsed;
    }
    return labeledSpace;
}

std::shared_ptr<BehavioralSpace> LabeledBSBuilder::build(std::shared_ptr<Network> network,
                                                         std::shared_ptr<long> maxExecTime,
                                                         std::shared_ptr<SimpleNetwork> observation,
                                                         std::shared_ptr<SimpleNetwork> bond) {
    Logger::section("Behavioral Space Construction + Labeling");
    auto startingTime = std::chrono::high_resolution_clock::now();

    // resetta la rete allo stato iniziale
    network->reset();

    BehavioralTransition::nextId = 0;

    // acquisisce lo stato (iniziale) della rete
    std::map<std::string, std::string> componentsState = network->getComponentsState();
    std::map<std::string, std::string> linksState = network->getLinksState();
    std::set<std::string> relevancyState;

    if (observation) observation->resetState();
    if (bond) bond->resetState();

    std::shared_ptr<BehavioralState> currentState = std::make_shared<BehavioralState>(componentsState, linksState, true, true, relevancyState);
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
                "Maximum execution time exceeded during labeled behavioral space construction! (" + std::to_string(elapsed) + ")",
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

                //resetta le etichette di rilevanza allo stato di partenza e aggiunge l'eventuale nuova etichetta
                relevancyState = currentState->relevancyLabels;
                if(!transition->relevancyLabel.empty())
                    relevancyState.insert(transition->relevancyLabel);

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
                    bool condition = state->componentsState == componentsState && state->linksState == linksState
                            && state->relevancyLabels == relevancyState;
                    if (observation) condition &= state->observationState == observation->currentState;
                    if (bond) condition &= state->bondState == bond->currentState;
                    if (condition) {
                        alreadyPresent = true;
                        state->in.push_back(newTransition);
                        newTransition->setStato(state);

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
                    auto newState = std::make_shared<BehavioralState>(componentsState, linksState, false, final, relevancyState);
                    newState->in.push_back(newTransition);
                    if (observation) newState->observationState = observation->currentState;
                    if (bond) newState->bondState = bond->currentState;
                    toVisit.push(newState);
                    newTransition->setStato(newState);

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

    auto dimensions = space->getDimensions();
    std::vector<std::string> messages = {
            "Labeled BS construction completed.",
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


std::shared_ptr<BehavioralSpace> LabeledBSBuilder::build(std::shared_ptr<BehavioralSpace> bs,
std::shared_ptr<long> maxExecTime, int prefix) {
    auto labeledSpace = std::make_shared<BehavioralSpace>();
    auto startingTime = std::chrono::high_resolution_clock::now();

    Logger::section("Behavioral Space Labeling");

    std::queue<std::pair<std::shared_ptr<BehavioralState>, int>> toVisit;
    std::queue<int> toVisitDeepth;

    // conta le prossime transizioni a partire da 0
    BehavioralTransition::nextId = 0;

    // acquisisce lo stato iniziale dello spazio comportamentale
    bs->setToInitialState();

    auto initialState = bs->getCurrentState();

    auto firstState = std::make_shared<BehavioralState>(initialState->componentsState, initialState->linksState,
                                                        initialState->starting, initialState->final);
    firstState->observationState = initialState->observationState;

    // inserisce lo stato iniziale nella mappa e nell'automa
    toVisit.push({firstState, bs->currentState});
    labeledSpace->states.push_back(firstState);
    toVisitDeepth.push(0);
    while (!toVisit.empty()) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        if(maxExecTime && elapsed > *maxExecTime){
            std::vector<std::string> errorMessages = {
                "Maximum execution time exceeded during behavioral space labeling! (" + std::to_string(elapsed) + ")",
                "  Processed states: " + std::to_string(labeledSpace->states.size()),
                "  Processed transitions: " + std::to_string(labeledSpace->transitions.size()),
                "  States in processing queue: " + std::to_string(toVisit.size())
            };
            Logger::err(errorMessages);
            throw std::runtime_error("execution time");
        }
        // prende il primo stato della mappa, e lo usa per inizializzare lo spazio comportamentale non decorato
        auto currentState = toVisit.front().first;
        bs->currentState = toVisit.front().second;
        auto currentDeepth=toVisitDeepth.front();

        // fa scattare tutte le transizioni in uscita dallo stato corrente dello spazio comportamentale non decorato
        auto currentTransitions = bs->getCurrentState()->out;

        int startingState = toVisit.front().second;
        //int deepth=0;
        for (auto &transition : currentTransitions) {

            bs->currentState = startingState;
            bs->performTransition(transition);

            // acquisisce lo stato così raggiunto dalla rete
            auto componentsState = bs->getCurrentState()->componentsState;
            auto linksState = bs->getCurrentState()->linksState;
            auto newRelevancyLabels = currentState->relevancyLabels;
            bool final = bs->states[bs->currentState]->final;

            auto observationState = bs->getCurrentState()->observationState;

            // se ha acquisito una nuova etichetta di rilevanza, la aggiunge allo stato corrente
            if (!transition->relevancyLabel.empty())
                newRelevancyLabels.insert(transition->relevancyLabel);
            int newDeepth;
            if (transition->observabilityLabel!=""){
                newDeepth=currentDeepth+1;
            }
            else
                newDeepth=currentDeepth;
            // crea una nuova transizione dello spazio comportamentale decorato e la aggiunge come transizione in uscita allo
            // stato corrente e all'automa
            auto newTransition = std::make_shared<BehavioralTransition>(transition->name, transition->relevancyLabel,
                                                                        transition->observabilityLabel);

            labeledSpace->transitions.push_back(newTransition);
            currentState->out.push_back(newTransition);

            bool alreadyPresent = false;
            // se lo stato raggiunto è già nel grafo aggiunge la transizione a quelle in ingresso
            for (const auto &state : labeledSpace->states) {
                bool condition = state->componentsState == componentsState && state->linksState == linksState &&
                                 state->relevancyLabels == newRelevancyLabels;
                if (observationState != -1) condition = condition && state->observationState == observationState;
                if (condition) {
                    alreadyPresent = true;
                    newTransition->setStato(state);

                    state->in.push_back(newTransition);
                    break;
                }
            }


            // se lo stato raggiunto non è già presente lo crea, vi aggiunge la transizione in ingresso e lo aggiunge
            // al vettore e all'automa
            if (!alreadyPresent && newDeepth<=prefix+1) {
                auto newState = std::make_shared<BehavioralState>(componentsState, linksState, false, final,
                                                                  newRelevancyLabels);
                if (observationState != -1) newState->observationState = observationState;
                newState->in.push_back(newTransition);
                toVisit.push({newState, bs->currentState});
                labeledSpace->states.push_back(newState);
                newTransition->setStato(newState);

                toVisitDeepth.push(newDeepth);
            }
        }

        // elimina lo stato corrente dalla mappa, da questo momento ad esso si possono solo aggiungere nuove transizioni
        // in ingresso
        toVisitDeepth.pop();
        toVisit.pop();
    }
    auto dimensions = labeledSpace->getDimensions();
    std::vector<std::string> messages = {
            "Labeled BS construction completed.",
            "  States: " + std::to_string(dimensions.first),
            "  Transitions: " + std::to_string(dimensions.second)
    };
    Logger::log(messages);

    if (maxExecTime) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        *maxExecTime -= elapsed;
    }
    return labeledSpace;
}


