
#include <iostream>
#include "BSBuilderDFA.h"



void BSBuilderDFA::prune(std::shared_ptr<BehavioralSpace> space, std::shared_ptr<long> maxExecTime) {

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
    Logger::log(messages);

    if (maxExecTime) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        *maxExecTime -= elapsed;
    }
}


/*std::shared_ptr<BehavioralSpace> BSBuilderDFA::build(std::shared_ptr<Network> network, std::vector<std::shared_ptr<BehavioralState>> nfaStates,std::string label,std::vector<std::shared_ptr<NFAStateInterface>> totalBhs) {

    Logger::section("Behavioral Space Construction");
    std::string filters;


    if (filters.empty()) Logger::log("No filters.");
    else Logger::log("Selected filters: " + filters);

    auto startingTime = std::chrono::high_resolution_clock::now();
    // resetta la rete allo stato iniziale
    network->reset();
    BehavioralState::nextId = 0;
    BehavioralTransition::nextId = 0;
    std::vector<std::shared_ptr<BehavioralState>> statiIniziali;
    std::vector<std::vector<std::shared_ptr<BehavioralTransition>>>transizioniUscenti;
    bool trovato=false;
    for(auto stato : nfaStates){

        std::vector<std::shared_ptr<BehavioralTransition>> transizioniUscentiStato;
        for(auto o :stato->getOutTransitions2() ){
            if(o->getObservabilityLabel()==label){
                trovato=true;

                transizioniUscentiStato.push_back(o);

            }
        }
        if (trovato){
            statiIniziali.push_back(stato);
            transizioniUscenti.push_back(transizioniUscentiStato);
        }
        trovato=false;
    }
    int contatore=0;
    for(auto statoIniziale: statiIniziali){

        auto currentState=statoIniziale;
        std::queue<std::shared_ptr<BehavioralState>> toVisit;
        toVisit.push(currentState);
        bool alreadyPresent;
        while (!toVisit.empty()) {
            currentState = toVisit.front();
            network->setState(currentState->componentsState, currentState->linksState);


            for (const auto &component : network->components) {
                // se una delle transizioni è abilitata la fa scattare
                std::vector<std::shared_ptr<Transition>> qualifiedTransitions = component->qualifiedTransitions();
                for (const auto &transition : qualifiedTransitions) {


            }

        }
        contatore++;
    }
    // acquisisce lo stato (iniziale) della rete
    std::map<std::string, std::string> componentsState = network->getComponentsState();
    std::map<std::string, std::string> linksState = network->getLinksState();



    std::shared_ptr<BehavioralState> currentState = std::make_shared<BehavioralState>(componentsState, linksState, true, true);


    // inserisce lo stato iniziale nel vettore e nell'automa
    std::queue<std::shared_ptr<BehavioralState>> toVisit;
    auto space = std::make_shared<BehavioralSpace>();
    toVisit.push(currentState);
    space->states.push_back(currentState);

    bool alreadyPresent;

    while (!toVisit.empty()) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();


        // prende il primo stato del vettore
        currentState = toVisit.front();
        network->setState(currentState->componentsState, currentState->linksState);


        for (const auto &component : network->components) {
            // se una delle transizioni è abilitata la fa scattare
            std::vector<std::shared_ptr<Transition>> qualifiedTransitions = component->qualifiedTransitions(


            for (const auto &transition : qualifiedTransitions) {
                alreadyPresent = false;

                // acquisisce lo stato così raggiunto dalla rete
                component->performTransition(transition);
                componentsState = network->getComponentsState();
                linksState = network->getLinksState();



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

                    auto newState = std::make_shared<BehavioralState>(componentsState, linksState, false, final);
                    newState->in.push_back(newTransition);

                    toVisit.push(newState);
                    space->states.push_back(newState);
                }

                // torna allo stato precedente prima di controllare il componente successivo
                network->setState(currentState->componentsState, currentState->linksState);

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



}*/

std::shared_ptr<BehavioralTransition> trovaTransizioniCoerenti( std::shared_ptr<Transition> qualifiedTransition,   std::vector<std::shared_ptr<BehavioralTransition>> transitions){
    for( auto tran: transitions){
        if(qualifiedTransition->name==tran->name){
            return tran;
        }
    }
    return nullptr;
}

std::shared_ptr<BehavioralState>  checkIfStateExist(std::shared_ptr<Dictionary> inDictionary, std::shared_ptr<BehavioralState> s){
    auto linkState=s->getLinksState();
    auto componentState=s->componentsState;
    auto rel=s->relevancyLabels;
    for( auto  state :inDictionary->bhState){
        if ( linkState== state->getLinksState() && componentState==state->getComponentsState() && rel==state->getRelevancyLabels()){
           std::shared_ptr<BehavioralState> *sss = reinterpret_cast<std::shared_ptr<BehavioralState> *>(&state);
            return *sss;
        }
    }
    return s;
}
std::vector<std::shared_ptr<BehavioralState>> BSBuilderDFA::silentClosure(std::shared_ptr<Network> network,std::vector<std::shared_ptr<BehavioralState>> nfaStates,std::string label,std::vector<std::shared_ptr<NFAStateInterface>> totalBhs,std::vector<std::shared_ptr<BehavioralTransition>> nuoveTransizioni, std::shared_ptr<Dictionary> inDictionary) {



    auto startingTime = std::chrono::high_resolution_clock::now();
    // resetta la rete allo stato iniziale
    std::vector<std::shared_ptr<BehavioralState>> nuoviStatiNfa;

    //std::vector<std::shared_ptr<BehavioralTransition>> nuoveTransizioni;
    bool observableOutTransition ;
    std::queue<std::shared_ptr<BehavioralState>> toVisit;
    for(auto state: nfaStates) {
        //setto lo stato nella rete
        std::shared_ptr<BehavioralTransition> transizioneOsservabile;
        auto stateTransition = state->getOutTransitions2();
        observableOutTransition = false;


        for (auto &transition :stateTransition) {

            if (transition->getObservabilityLabel() == label) {
                observableOutTransition = true;
                break;

            }
        }

        //l'osservazione si può verificare
        if (observableOutTransition) {
            network->setState(state->componentsState, state->linksState);

            auto currentState = state;

            bool alreadyPresent;
            std::map<std::string, std::string> componentsState = network->getComponentsState();
            std::map<std::string, std::string> linksState = network->getLinksState();
            bool firstObs = true;
            // prende il primo stato del vettore
            currentState = state;
            network->setState(currentState->componentsState, currentState->linksState);
            auto newRelevancyLabels = currentState->relevancyLabels;
            for (const auto &component : network->components) {
            // se una delle transizioni è abilitata la fa scattare
            std::vector<std::shared_ptr<Transition>> qualifiedTransitions = component->qualifiedTransitions();
                for (const auto &transition : qualifiedTransitions) {
                    alreadyPresent = false;
                    if (firstObs) {
                        if (transition->observabilityLabel == label) {
                            // acquisisce lo stato così raggiunto dalla rete
                            component->performTransition(transition);
                            componentsState = network->getComponentsState();
                            linksState = network->getLinksState();

                            // crea una nuova transizione dello spazio comportamentale e la aggiunge come transizione in uscita allo
                            // stato corrente e all'automa
                            auto newTransition =trovaTransizioniCoerenti(transition,state->getOutTransitions2());
                            for ( auto state : totalBhs) {
                                bool condition = state->getComponentsState() == componentsState && state->getLinksState() == linksState;
                                if (condition) {
                                    alreadyPresent = true;
                                    state->addInTransition(newTransition);
                                    //auto ss= state.get();

                                    std::shared_ptr<BehavioralState> *sss = reinterpret_cast<std::shared_ptr<BehavioralState> *>(&state);

                                    auto newDiagnosi=newRelevancyLabels;
                                    if (!newTransition->relevancyLabel.empty()){
                                        newDiagnosi.insert(newTransition->relevancyLabel);
                                    }
                                    auto di=sss->get()->relevancyLabels;
                                    for (auto el: di){
                                        newDiagnosi.insert(el);
                                    }
                                    sss->get()->relevancyLabels=newDiagnosi;
                                    toVisit.push(*sss);

                                    nuoviStatiNfa.push_back(*sss);
                                    //int i = 0;
                                    break;
                                    }

                            }
                            if (!alreadyPresent) {
                                bool final = true;
                                for (auto linkState : linksState)
                                    if (linkState.second != "empty")
                                        final = false;
                                auto newDiagnosi=newRelevancyLabels;
                                if (!newTransition->relevancyLabel.empty()){
                                    newDiagnosi.insert(newTransition->relevancyLabel);
                                }

                                auto newState = std::make_shared<BehavioralState>(componentsState, linksState,
                                  
                                                                                  false, final);
                                newState->relevancyLabels=newDiagnosi;
                                newState=checkIfStateExist(inDictionary,newState);
                                //newState->relevancyLabels=newDiagnosi;

                                newState->in.push_back(newTransition);

                                totalBhs.push_back(newState);
                                toVisit.push(newState);
                                nuoviStatiNfa.push_back(newState);
                            }

                                // torna allo stato precedente prima di controllare il componente successivo
                                network->setState(currentState->componentsState, currentState->linksState);
                            }

                        }

                        }
                    }

                }

            }

    if(toVisit.size()==0){
        for(auto state: nfaStates) {
            auto currentState = state;
            bool alreadyPresent;
            std::map<std::string, std::string> componentsState = network->getComponentsState();
            std::map<std::string, std::string> linksState = network->getLinksState();
           // bool firstObs = true;
            // prende il primo stato del vettore
            currentState = state;
            network->setState(currentState->componentsState, currentState->linksState);
            auto newRelevancyLabels = currentState->relevancyLabels;
            for (const auto &component : network->components) {
                // se una delle transizioni è abilitata la fa scattare
                std::vector<std::shared_ptr<Transition>> qualifiedTransitions = component->qualifiedTransitions();

                for (const auto &transition : qualifiedTransitions) {

                    if(transition->observabilityLabel==label){
                        alreadyPresent = false;
                        // acquisisce lo stato così raggiunto dalla rete
                        component->performTransition(transition);
                        componentsState = network->getComponentsState();
                        linksState = network->getLinksState();
                        // crea una nuova transizione dello spazio comportamentale e la aggiunge come transizione in uscita allo
                        // stato corrente e all'automa
                        auto newTransition = std::make_shared<BehavioralTransition>(transition->name,
                                                                                    transition->relevancyLabel,
                                                                                    transition->observabilityLabel);
                        // ricorda AGGIUNGERE TRANZISIONI A DIZIONARIO
                        nuoveTransizioni.push_back(newTransition);
                        /*space->transitions.push_back(newTransition);*/
                        currentState->out.push_back(newTransition);
                        // se lo stato raggiunto è già nel grafo aggiunge la transizione a quelle in ingresso
                        for ( auto state : totalBhs) {
                            bool condition = state->getComponentsState()== componentsState && state->getLinksState() == linksState;

                            if (condition) {
                                alreadyPresent = true;
                                state->addInTransition(newTransition);

                                std::shared_ptr<BehavioralState> *sss = reinterpret_cast<std::shared_ptr<BehavioralState> *>(&state);
                                auto newDiagnosi=newRelevancyLabels;
                                if (!newTransition->relevancyLabel.empty()){
                                    newDiagnosi.insert(newTransition->relevancyLabel);
                                }
                                auto di=sss->get()->relevancyLabels;
                                for (auto el: di){
                                    newDiagnosi.insert(el);
                                }
                                sss->get()->relevancyLabels=newDiagnosi;

                                toVisit.push(*sss);
                                nuoviStatiNfa.push_back(*sss);
                                break;
                            }
                            //break; PRIMA ERA QUI IL BREAK SOPORA VERIFICARE
                            }


                        // se lo stato raggiunto non è già presente lo crea, vi aggiunge la transizione in ingresso e lo aggiunge
                        // al vettore e all'automa
                        if (!alreadyPresent) {
                            bool final = true;
                            for (auto linkState : linksState)
                                if (linkState.second != "empty")
                                    final = false;

                                auto newDiagnosi=newRelevancyLabels;
                                if (!newTransition->relevancyLabel.empty()){
                                    newDiagnosi.insert(newTransition->relevancyLabel);
                                }

                                auto newState = std::make_shared<BehavioralState>(componentsState, linksState, false, final);
                                newState=checkIfStateExist(inDictionary,newState);
                                newState->in.push_back(newTransition);
                                newState->relevancyLabels=newDiagnosi;
                                toVisit.push(newState);
                                nuoviStatiNfa.push_back(newState);

                        }
                        // torna allo stato precedente prima di controllare il componente successivo
                        network->setState(currentState->componentsState, currentState->linksState);

                    }
                    }
                }
            }
        }





    // stati con transizioni non osservabili
    bool alreadyPresent;
    std::shared_ptr<BehavioralState> currentState;
    std::map<std::string, std::string> componentsState = network->getComponentsState();
    std::map<std::string, std::string> linksState = network->getLinksState();
    bool empty;
    empty=toVisit.empty();
    while (!toVisit.empty()) {

        // prende il primo stato del vettore
        currentState = toVisit.front();
        network->setState(currentState->componentsState, currentState->linksState);

        auto newRelevancyLabels = currentState->relevancyLabels;
        for (const auto &component : network->components) {
            // se una delle transizioni è abilitata la fa scattare
            std::vector<std::shared_ptr<Transition>> qualifiedTransitions = component->qualifiedTransitions();

            for (const auto &transition : qualifiedTransitions) {
                alreadyPresent = false;

                // acquisisce lo stato così raggiunto dalla rete
                component->performTransition(transition);
                componentsState = network->getComponentsState();
                linksState = network->getLinksState();




                // crea una nuova transizione dello spazio comportamentale e la aggiunge come transizione in uscita allo
                // stato corrente e all'automa
                auto newTransition = std::make_shared<BehavioralTransition>(transition->name,
                                                                            transition->relevancyLabel,
                                                                            transition->observabilityLabel);
                // ricorda AGGIUNGERE TRANZISIONI A DIZIONARIO
                    nuoveTransizioni.push_back(newTransition);
                /*space->transitions.push_back(newTransition);*/
                currentState->out.push_back(newTransition);
                auto dd=currentState->relevancyLabels;

                dd.insert(newTransition->relevancyLabel);

                // se lo stato raggiunto è già nel grafo aggiunge la transizione a quelle in ingresso
                for ( auto state : totalBhs) {
                    bool condition = state->getComponentsState()== componentsState && state->getLinksState() == linksState && dd==state->getRelevancyLabels();

                    if (condition) {
                        alreadyPresent = true;
                        //bool condition2=true;

                        for(auto in: state->getInTransitions()){

                        }
                        /*for (int i = 0; i < toVisit.size(); i++){
                            if(state == toVisit[i]){
                                condition2= false;
                            }

                        }
*/
                        if(newTransition->getObservabilityLabel()=="" ) {
                            state->addInTransition(newTransition);

                            std::shared_ptr<BehavioralState> *sss = reinterpret_cast<std::shared_ptr<BehavioralState> *>(&state);
                            auto newDiagnosi=newRelevancyLabels;
                            if (!newTransition->relevancyLabel.empty()){
                                newDiagnosi.insert(newTransition->relevancyLabel);
                            }
                            auto di=sss->get()->relevancyLabels;
                            for (auto el: di){
                                newDiagnosi.insert(el);
                            }
                            sss->get()->relevancyLabels=newDiagnosi;
                            toVisit.push(*sss);
                            nuoviStatiNfa.push_back(*sss);
                        }


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
                    if (newTransition->getObservabilityLabel()==""){


                        auto newDiagnosi=newRelevancyLabels;
                        if (!newTransition->relevancyLabel.empty()){
                            newDiagnosi.insert(newTransition->relevancyLabel);
                        }

                        auto newState = std::make_shared<BehavioralState>(componentsState, linksState, false, final);
                        newState->relevancyLabels=newDiagnosi;
                        newState=checkIfStateExist(inDictionary,newState);
                        newState->in.push_back(newTransition);
                       // newState->relevancyLabels=newDiagnosi;
                        newState->in.push_back(newTransition);

                        toVisit.push(newState);
                        nuoviStatiNfa.push_back(newState);
                    }

                }

                // torna allo stato precedente prima di controllare il componente successivo
                network->setState(currentState->componentsState, currentState->linksState);

            }
        }
        // elimina lo stato corrente dal vettore, da questo momento ad esso si possono solo aggiungere nuove transizioni
        // in ingresso
      // auto ss= toVisit.size();
        toVisit.pop();
        //auto sa= toVisit.size();
        empty=toVisit.empty();
        // int aaa=0;


    }

    for(auto t:nuoveTransizioni){
        inDictionary->addBehavioralTransition(t);
    }

    return nuoviStatiNfa;


    }



