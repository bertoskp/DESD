

#include "DFAConstructor.h"
#include "../Utils/AbduceIO.h"
#include "../Utils/NetworkIO.h"
#include "../manipulators/BSBuilderDFA.h"
#include "../Utils/DictionaryIO.h"
#include "../Utils/DictionaryIO2.h"
#include "../Data/SimpleNetwork.h"


template< typename tPair >
struct second_t {
    typename tPair::second_type operator()( const tPair& p ) const { return     p.second; }
};

template< typename tMap >
second_t< typename tMap::value_type > second( const tMap& m ) { return second_t<     typename tMap::value_type >(); }

void  DFAConstructor::relevanClosure(std::set<std::shared_ptr<NFAStateInterface>> closurep,std::set<std::shared_ptr<NFAStateInterface>> &closure){
    std::queue<std::shared_ptr<NFAStateInterface>> toRemoveStates;
                     for (const auto &stato : closure){
                         bool trOBS=false;
                         auto ts = std::shared_ptr<NFAStateInterface>(stato);
                         if (stato->getFinal()){
                             closurep.insert(ts);
                         }
                         else{
                             for (auto ot:stato->getOutTransitions()){
                                 if (ot->getObservabilityLabel()!=""){
                                     trOBS=true;
                                     break;
                                 }
                             }
                             if (!trOBS){
                                 toRemoveStates.push(stato);
                             }
                         }
                     }
    if (!toRemoveStates.empty()){
        while(!toRemoveStates.empty()){
            
        auto stato = toRemoveStates.front();
           toRemoveStates.pop();
        closure.erase(stato);
        }
    }
    
   
}
bool
compareStateSets(std::set<std::shared_ptr<NFAStateInterface>> lhs, std::set<std::shared_ptr<NFAStateInterface>> rhs);

std::shared_ptr<std::vector<std::shared_ptr<DFAState>>> DFAConstructor::subsetConstruction(
    std::shared_ptr<NFASpaceInterface> inputSpace,bool online, std::shared_ptr<long> maxExecTime) {

    auto startingTime = std::chrono::high_resolution_clock::now();
    std::string message;
    BehavioralTransition::nextId = 0;
    DictionaryState::nextId = 0;

    // Coda che contiene gli stati ancora da visitare
    std::queue<std::set<std::shared_ptr<NFAStateInterface>>> unmarkedStates;
    // Stati DFA costruiti
    std::shared_ptr<std::vector<std::shared_ptr<DFAState>>> dfaStates(new std::vector<std::shared_ptr<DFAState>>());

    // Alfabeto di etichette dello spazio in ingresso
    std::set<std::string> alphabet;

    for (const auto &transition : inputSpace->getTransitions())
        alphabet.insert(transition->getObservabilityLabel());

    std::shared_ptr<NFAStateInterface> startingState = nullptr;

    for (const auto &state : inputSpace->getStates())
        if (state->isStarting()) {
            startingState = std::shared_ptr<NFAStateInterface>(state);
            break;
        }
    if (startingState == nullptr) throw std::invalid_argument("Input NFAStateInterface does not have an initial state");

    // Calcolo la eps-closure iniziale
    std::set<std::shared_ptr<NFAStateInterface>> startingClosure;

    epsilonClosure(*inputSpace, startingState, startingClosure);
    std::set<std::shared_ptr<NFAStateInterface>> startingClosurep;
    if (!online){
        relevanClosure(startingClosurep,startingClosure);
    }
    unmarkedStates.push(startingClosure);
    dfaStates->push_back(std::make_shared<DFAState>(startingClosure));

    // Fino a quando ci sono stati non visitati
    while(!unmarkedStates.empty()){
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        if(maxExecTime && elapsed > *maxExecTime){
            std::vector<std::string> errorMessages = {
                "Maximum execution time exceeded during behavioral subset construction! (" + std::to_string(elapsed) + ")",
                "  Processed DFA states: " + std::to_string(dfaStates->size()),
                "  Unmarked NFA states: " + std::to_string(unmarkedStates.size())
            };
            Logger::err(errorMessages);
            throw std::runtime_error("Time");
        }

        auto currentState = unmarkedStates.front();
        unmarkedStates.pop();

        for (std::string label : alphabet) {
            if(!label.empty()) {
                std::cout << "numero stati diz"+std::to_string(dfaStates->size())+"\n ";
                 std::cout << "numero stati mancanti"+std::to_string(unmarkedStates.size())+"\n ";
                
                bool alreadyPresent = false;
                std::set<std::shared_ptr<NFAStateInterface>> closure,closurep, toStates;

                move(*inputSpace, currentState, toStates, label);

                epsilonClosure(*inputSpace, toStates, closure);
                
                if (!online){
                    relevanClosure(closurep,closure);
                }
                if(!closure.empty()) {
                    auto newTransition = std::make_shared<BehavioralTransition>("", "", label);
                    for (const auto &state : *dfaStates) {
                        if (compareStateSets(state->nfaStates, currentState)) {
                            state->out.push_back(newTransition);
                            break;
                        }
                    }
                    for (const auto &state : *dfaStates)
                        if (compareStateSets(state->nfaStates, closure)) {
                            state->in.push_back(newTransition);
                            alreadyPresent = true;
                            break;
                        }
                    if (!alreadyPresent) {
                        auto newState = std::make_shared<DFAState>(closure);
                        newState->in.push_back(newTransition);
                        dfaStates->push_back(newState);
                        
                        unmarkedStates.push(closure);

                    }
                }
            }
        }
    }

    if (maxExecTime) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        *maxExecTime -= elapsed;
    }
    return dfaStates;
}


std::shared_ptr<std::vector<std::shared_ptr<DFAState>>> DFAConstructor::subsetConstructionPrefix(
        std::shared_ptr<NFASpaceInterface> inputSpace,unsigned int prefix, std::shared_ptr<long> maxExecTime) {

    auto startingTime = std::chrono::high_resolution_clock::now();
    std::string message;

    BehavioralTransition::nextId = 0;
    DictionaryState::nextId = 0;

    // Coda che contiene gli stati ancora da visitare
    std::queue<std::set<std::shared_ptr<NFAStateInterface>>> unmarkedStates;
    // Stati DFA costruiti
    std::shared_ptr<std::vector<std::shared_ptr<DFAState>>> dfaStates(new std::vector<std::shared_ptr<DFAState>>());

    // Alfabeto di etichette dello spazio in ingresso
    std::set<std::string> alphabet;

    for (const auto &transition : inputSpace->getTransitions())
        alphabet.insert(transition->getObservabilityLabel());

    std::shared_ptr<NFAStateInterface> startingState = nullptr;

    for (const auto &state : inputSpace->getStates())
        if (state->isStarting()) {
            startingState = std::shared_ptr<NFAStateInterface>(state);
            break;
        }
    if (startingState == nullptr) throw std::invalid_argument("Input NFAStateInterface does not have an initial state");

    // Calcolo la eps-closure iniziale
    std::set<std::shared_ptr<NFAStateInterface>> startingClosure;

    epsilonClosure(*inputSpace, startingState, startingClosure);
    unmarkedStates.push(startingClosure);
    unsigned int livello=0;
    std::vector<unsigned  int> livelli;
    livelli.push_back(livello);
    dfaStates->push_back(std::make_shared<DFAState>(startingClosure));

    // Fino a quando ci sono stati non visitati
    while(!unmarkedStates.empty()){
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        if(maxExecTime && elapsed > *maxExecTime){
            std::vector<std::string> errorMessages = {
                    "Maximum execution time exceeded during behavioral subset construction! (" + std::to_string(elapsed) + ")",
                    "  Processed DFA states: " + std::to_string(dfaStates->size()),
                    "  Unmarked NFA states: " + std::to_string(unmarkedStates.size())
            };
            Logger::err(errorMessages);
            throw std::runtime_error("Time");
        }

        auto currentState = unmarkedStates.front();
        auto level=livelli.front();
       // unsigned  int indice=0;
        livelli.erase(livelli.begin());
        unmarkedStates.pop();
        if (level<prefix){
            for (std::string label : alphabet) {
                if(!label.empty()) {
                    bool alreadyPresent = false;
                    std::set<std::shared_ptr<NFAStateInterface>> closure, toStates;
                    move(*inputSpace, currentState, toStates, label);
                    epsilonClosure(*inputSpace, toStates, closure);

                    if(!closure.empty() ) {
                        auto newTransition = std::make_shared<BehavioralTransition>("", "", label);
                        for (const auto &state : *dfaStates) {
                            if (compareStateSets(state->nfaStates, currentState)) {
                                state->out.push_back(newTransition);
                                break;
                            }
                        }
                        for (const auto &state : *dfaStates)
                            if (compareStateSets(state->nfaStates, closure)) {
                                state->in.push_back(newTransition);
                                alreadyPresent = true;
                                break;
                            }

                        if (!alreadyPresent and level<prefix) {
                            auto newState = std::make_shared<DFAState>(closure);
                            newState->in.push_back(newTransition);
                            dfaStates->push_back(newState);
                            unmarkedStates.push(closure);
                            livelli.push_back(level+1);
                        }
                    }
                }
            }
        }

    }

    if (maxExecTime) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        *maxExecTime -= elapsed;
    }
    return dfaStates;
}

std::shared_ptr<Dictionary> DFAConstructor::constructDictionary(std::shared_ptr<NFASpaceInterface> inputSpace,
                                                               bool online, std::shared_ptr<long> maxExecTime) {

    std::string message;

    Logger::section("Dictionary Construction");

    auto dfaStates = subsetConstruction(inputSpace,online, maxExecTime);
    auto startingTime = std::chrono::high_resolution_clock::now();


    std::shared_ptr<NFAStateInterface> startingState = nullptr;

    /*for (const auto &state : inputSpace->getStates())
        if (state->isStarting()) {
            startingState = std::shared_ptr<NFAStateInterface>(state);
            break;
        }*/

    std::shared_ptr<std::vector<std::shared_ptr<transitionsInfo>>>tInfo( new std::vector<std::shared_ptr<transitionsInfo>>());

    for (const auto &state : inputSpace->getStates()){
        if (state->isStarting()) {
            startingState = std::shared_ptr<NFAStateInterface>(state);
        }
        for (auto outT: state->getOutTransitions()){
             auto tI = std::make_shared<transitionsInfo>(outT->getId(),outT->getObservabilityLabel(),state->getIdd());
             for ( auto &state2 : inputSpace->getStates() ){
                 for (auto inT: state2->getInTransitions()){
                     if (inT->getId()==outT->getId()){
                         tI->setDestination(state2->getIdd());
                         break;
                     }
                 }
             }
             tInfo->push_back(tI);
        }
    }


    bool isFinal;
    bool starter = false;

    std::vector<std::shared_ptr<DictionaryState>> states;
    int stato=0;
    for (const auto &state : *dfaStates) {

        //per logger
        std::string messaggio1 = "STATO dfa "+ std::to_string(stato);
        //Logger::log(messaggio1);

        int st=0;
        std::vector<std::shared_ptr<BehavioralState>> statiNfa;
        for (const auto &behavioralState : state->nfaStates){
            std::string transizioniOString="";
            // unsigned int a=behavioralState->getIdd();
            for(const auto &outTra : behavioralState->getOutTransitions()){
                transizioniOString=transizioniOString+outTra->toString();

            }
            std::string transizioniIString="";
            for(const auto &outTra : behavioralState->getInTransitions()){
                transizioniIString=transizioniIString+outTra->toString();

            }
            std::vector<std::string> messaggio = {
                    "STATO"+ std::to_string(st),
                    "IN"+ transizioniIString,
                    "OUT"+ transizioniOString};
            //Logger::log(messaggio);
            st=st+1;
        }

        /*DFAConstructor::createPair(state);*/
        std::set<std::set<std::string>>diagnosis_p=state->getDiagnosis();
        std::vector<std::vector<std::shared_ptr<NFAStateInterface>>> pairs;
       /* pairs=state->getPairs();*/
        stato=stato+1;

        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        if(maxExecTime && elapsed > *maxExecTime){
            std::vector<std::string> errorMessages = {
                "Maximum execution time exceeded during dictionary construction! (" + std::to_string(elapsed) + ")",
                "  Current task: constructing dictionary states from DFA states"
            };
            Logger::err(errorMessages);
            throw std::runtime_error("execution time");
        }
        isFinal = false;

        // Se non ho ancora identificato lo stato iniziale, e questo stato rappresenta
        // anche lo stato dell' NFA iniziale, allora lo prendo come iniziale
        if(!starter && state->nfaStates.find(startingState) != state->nfaStates.end()){
            starter = true;
            for (const auto &behavioralState : state->nfaStates)
                if (behavioralState->isFinal()) {
                    isFinal = true;
                    break;
                }
            std::string messaggio1 = "";
            for( auto sdiag: state->getDiagnosis()){
                messaggio1=messaggio1+"{";
                for (auto diag: sdiag){
                    messaggio1=messaggio1+" "+ diag;
                }
                messaggio1=messaggio1+"}";
            }
            Logger::log(messaggio1);
           // std::shared_ptr<DictionaryState> dicState;
            if( !online and isFinal){
                
                auto dicState=std::make_shared<DictionaryState>(isFinal, starter, state->in, state->out, state->getDiagnosis(),state->getPairs());
                dicState->setNfaState(state->nfaStates);
                           std::vector<std::shared_ptr<BehavioralState>> statiNfa;
                           BehavioralState stato;

                           for(auto ss : dicState->nfaStates){

                               int c=1;

                               c=c+1;
                           }
                           states.push_back(dicState);
            }
            else if (!online and !isFinal){
                std::set<std::set<std::string>> dd;
                auto dicState=std::make_shared<DictionaryState>(isFinal, starter, state->in, state->out, dd,state->getPairs());
                dicState->setNfaState(state->nfaStates);
                           std::vector<std::shared_ptr<BehavioralState>> statiNfa;
                           BehavioralState stato;

                           for(auto ss : dicState->nfaStates){

                               int c=1;

                               c=c+1;
                           }
                           states.push_back(dicState);
            }
            else{
                auto dicState=std::make_shared<DictionaryState>(true, starter, state->in, state->out, state->getDiagnosis(),state->getPairs());
                dicState->setNfaState(state->nfaStates);
                           std::vector<std::shared_ptr<BehavioralState>> statiNfa;
                           BehavioralState stato;

                           for(auto ss : dicState->nfaStates){

                               int c=1;

                               c=c+1;
                           }
                           states.push_back(dicState);
            }
            std::set<std::shared_ptr<NFAStateInterface>> ss;
           /*
            if (!online){
                for ( auto s :state->nfaStates ){
                    bool obs=false;
                    if ((s->isFinal())){
                        ss.insert(s);
                        
                    }
                    
                    else{
                   for(auto o : state->out){
                       if(o->getRelevancyLabel()!=""){
                           ss.insert(s);
                       }
                   }
               }
           }
           dicState->setNfaState(ss);
       }
       else{
           dicState->setNfaState(state->nfaStates);
       }*/
            
            /* appena messo
            dicState->setNfaState(state->nfaStates);
            std::vector<std::shared_ptr<BehavioralState>> statiNfa;
            BehavioralState stato;

            for(auto ss : dicState->nfaStates){

                int c=1;

                c=c+1;
            }
            states.push_back(dicState);
             */
        }
        else {
            for (const auto &behavioralState : state->nfaStates)
                if (behavioralState->isFinal()) {
                    isFinal = true;
                    break;
                }
            std::string messaggio1 = "";
            for( auto sdiag: state->getDiagnosis()){
                messaggio1=messaggio1+"{";
                for (auto diag: sdiag){
                    messaggio1=messaggio1+" "+ diag;
                }
                messaggio1=messaggio1+"}";
            }
            Logger::log(messaggio1);
           // std::shared_ptr<DictionaryState> dicState;
            if( !online and isFinal){
                
               auto dicState=std::make_shared<DictionaryState>(isFinal, false, state->in, state->out, state->getDiagnosis(),state->getPairs());
                dicState->setNfaState(state->nfaStates);
                           states.push_back(dicState);
            }
            else if (!online and !isFinal){
                std::set<std::set<std::string>> dd;
                auto dicState=std::make_shared<DictionaryState>(isFinal, false, state->in, state->out, dd,state->getPairs());
                dicState->setNfaState(state->nfaStates);
                           states.push_back(dicState);
            }
            else{
                 auto dicState=std::make_shared<DictionaryState>(true, false, state->in, state->out, state->getDiagnosis(),state->getPairs());
                dicState->setNfaState(state->nfaStates);
                           states.push_back(dicState);
            }
            
           // auto dicState=std::make_shared<DictionaryState>(isFinal, false, state->in, state->out, state->getDiagnosis(),state->getPairs());
            
            std::set<std::shared_ptr<NFAStateInterface>> ss;
            /*
            if (!online){
                for ( auto s :state->nfaStates ){
                    bool obs=false;
                    if ((s->isFinal())){
                        ss.insert(s);
                        
                    }
                    
                    else{
                        for(auto o : state->out){
                            if(o->getRelevancyLabel()!=""){
                                ss.insert(s);
                            }
                        }
                    }
                }
                dicState->setNfaState(ss);
            }
            else{
                dicState->setNfaState(state->nfaStates);
            }
             */
            
            /*
             dicState->setNfaState(state->nfaStates);
            states.push_back(dicState);
             */
        }
            
    }

    std::set<std::shared_ptr<NFATransitionInterface>> transitionsSet;
    for (const auto &state : states) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        if(maxExecTime && elapsed > *maxExecTime){
            std::vector<std::string> errorMessages = {
                "Maximum execution time exceeded during dictionary construction! (" + std::to_string(elapsed) + ")",
                "  Current task: adding dictionary transitions"
            };
            Logger::err(errorMessages);
            throw std::runtime_error("execution time");
        }
        for (const auto &in : state->in) transitionsSet.insert(in);
        for (const auto &out : state->out) transitionsSet.insert(out);
    }
    std::vector<std::shared_ptr<NFATransitionInterface>> transitions(transitionsSet.begin(), transitionsSet.end());

    std::vector<std::string> messages = {
            "Dictionary construction completed.",
            "  States: " + std::to_string(states.size()),
            "  Transitions: " + std::to_string(transitions.size())
    };
    Logger::log(messages);

    if (maxExecTime) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        *maxExecTime -= elapsed;
    }
    auto dictionary=std::make_shared<Dictionary>(states, transitions);
    auto trans=inputSpace->getBehavioralTransitions();
    dictionary->setBehavioralTransitions(trans);
    auto sss=inputSpace->getStates();
    dictionary->setBhState(sss);
    dictionary->setTinfo(tInfo);
    return dictionary;
}


bool foundTransitionSubgrafh(std::vector<std::shared_ptr<NFAStateInterface>> behavioralStates,std::shared_ptr<NFATransitionInterface> behavioralTransition ){
    bool found=false;
    std::shared_ptr<NFAStateInterface> trovato;
    for(auto state :behavioralStates){
        for( auto tra: state->getInTransitions()){
            if (behavioralTransition->getId()==tra->getId()){
                found=true;
                trovato=state;
                break;
            }
        }
        if (found){
            break;
        }


    }
    //bool rrfound;
    bool refound=false;
    if (found){
        for(auto state :behavioralStates){
            for( auto tra: state->getOutTransitions()){
                if (behavioralTransition->getId()==tra->getId()){
                    refound=true;

                    break;
                }

            }
            if (refound){
                break;
            }


        }
    }

    if (not refound and found){
        std::shared_ptr<BehavioralTransition> *sss = reinterpret_cast<std::shared_ptr<BehavioralTransition> *>(&behavioralTransition);

        trovato->removeInTransition(*sss);
    }

    if (trovato== nullptr){
        return true;
    }
    return refound;

}



std::shared_ptr<Dictionary> DFAConstructor::constructDictionaryPrefix(unsigned  int prefix, std::shared_ptr<NFASpaceInterface> inputSpace,
                                                                std::shared_ptr<long> maxExecTime) {

    std::string message;

    Logger::section("Dictionary Construction");

    auto dfaStates = subsetConstructionPrefix(inputSpace, prefix,maxExecTime);
    auto startingTime = std::chrono::high_resolution_clock::now();


    std::shared_ptr<NFAStateInterface> startingState = nullptr;

    /*for (const auto &state : inputSpace->getStates())
        if (state->isStarting()) {
            startingState = std::shared_ptr<NFAStateInterface>(state);
            break;
        }*/

    std::shared_ptr<std::vector<std::shared_ptr<transitionsInfo>>>tInfo( new std::vector<std::shared_ptr<transitionsInfo>>());

    for (const auto &state : inputSpace->getStates()){
        if (state->isStarting()) {
            startingState = std::shared_ptr<NFAStateInterface>(state);

        }
        for (auto outT: state->getOutTransitions()){
            auto tI = std::make_shared<transitionsInfo>(outT->getId(),outT->getObservabilityLabel(),state->getIdd());
            for ( auto &state2 : inputSpace->getStates() ){
                for (auto inT: state2->getInTransitions()){
                    if (inT->getId()==outT->getId()){
                        tI->setDestination(state2->getIdd());
                        break;
                    }
                }
            }
            tInfo->push_back(tI);
        }
    }


    bool isFinal;
    bool starter = false;

    std::vector<std::shared_ptr<DictionaryState>> states;
    int stato=0;
    for (const auto &state : *dfaStates) {

        //per logger
        std::string messaggio1 = "STATO dfa "+ std::to_string(stato);
        Logger::log(messaggio1);

        int st=0;
        std::vector<std::shared_ptr<BehavioralState>> statiNfa;
        for (const auto &behavioralState : state->nfaStates){
            std::string transizioniOString="";
            //unsigned int a=behavioralState->getIdd();
            for(const auto &outTra : behavioralState->getOutTransitions()){
                transizioniOString=transizioniOString+outTra->toString();

            }
            std::string transizioniIString="";
            for(const auto &outTra : behavioralState->getInTransitions()){
                transizioniIString=transizioniIString+outTra->toString();

            }
            std::vector<std::string> messaggio = {
                    "STATO"+ std::to_string(st),
                    "IN"+ transizioniIString,
                    "OUT"+ transizioniOString};
            Logger::log(messaggio);
            st=st+1;
        }

       // DFAConstructor::createPair(state);
        std::set<std::set<std::string>>diagnosis_p=state->getDiagnosis();
        std::vector<std::vector<std::shared_ptr<NFAStateInterface>>> pairs;
        /* pairs=state->getPairs();*/
        stato=stato+1;

        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        if(maxExecTime && elapsed > *maxExecTime){
            std::vector<std::string> errorMessages = {
                    "Maximum execution time exceeded during dictionary construction! (" + std::to_string(elapsed) + ")",
                    "  Current task: constructing dictionary states from DFA states"
            };
            Logger::err(errorMessages);
            throw std::runtime_error("execution time");
        }
        isFinal = false;

        // Se non ho ancora identificato lo stato iniziale, e questo stato rappresenta
        // anche lo stato dell' NFA iniziale, allora lo prendo come iniziale
        if(!starter && state->nfaStates.find(startingState) != state->nfaStates.end()){
            starter = true;
            for (const auto &behavioralState : state->nfaStates)
                if (behavioralState->isFinal()) {
                    isFinal = true;
                    break;
                }
            std::string messaggio1 = "";
            for( auto sdiag: state->getDiagnosis()){
                messaggio1=messaggio1+"{";
                for (auto diag: sdiag){
                    messaggio1=messaggio1+" "+ diag;
                }
                messaggio1=messaggio1+"}";
            }
            Logger::log(messaggio1);
            auto dicState=std::make_shared<DictionaryState>(isFinal, true, state->in, state->out, state->getDiagnosis(),state->getPairs());
            dicState->setNfaState(state->nfaStates);
            std::vector<std::shared_ptr<BehavioralState>> statiNfa;
            BehavioralState stato;

            for(auto ss : dicState->nfaStates){

                int c=1;

                c=c+1;
            }
            states.push_back(dicState);
        }
        else {
            for (const auto &behavioralState : state->nfaStates)
                if (behavioralState->isFinal()) {
                    isFinal = true;
                    break;
                }
            std::string messaggio1 = "";
            for( auto sdiag: state->getDiagnosis()){
                messaggio1=messaggio1+"{";
                for (auto diag: sdiag){
                    messaggio1=messaggio1+" "+ diag;
                }
                messaggio1=messaggio1+"}";
            }
            Logger::log(messaggio1);
            auto dicState=std::make_shared<DictionaryState>(isFinal, true, state->in, state->out, state->getDiagnosis(),state->getPairs());
            dicState->setNfaState(state->nfaStates);
            states.push_back(dicState);
        }
    }

    std::set<std::shared_ptr<NFATransitionInterface>> transitionsSet;
    for (const auto &state : states) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        if(maxExecTime && elapsed > *maxExecTime){
            std::vector<std::string> errorMessages = {
                    "Maximum execution time exceeded during dictionary construction! (" + std::to_string(elapsed) + ")",
                    "  Current task: adding dictionary transitions"
            };
            Logger::err(errorMessages);
            throw std::runtime_error("execution time");
        }
        for (const auto &in : state->in) transitionsSet.insert(in);
        for (const auto &out : state->out) transitionsSet.insert(out);
    }
    std::vector<std::shared_ptr<NFATransitionInterface>> transitions(transitionsSet.begin(), transitionsSet.end());

    std::vector<std::string> messages = {
            "Dictionary construction completed.",
            "  States: " + std::to_string(states.size()),
            "  Transitions: " + std::to_string(transitions.size())
    };
    Logger::log(messages);

    if (maxExecTime) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        *maxExecTime -= elapsed;
    }
    auto dictionary=std::make_shared<Dictionary>(states, transitions);

    auto bhStates=  foundNfaStateDictionary(dictionary);
    auto trans=foundBehavioralTransitionDictionary(bhStates);
    std::vector<int> indiciT;
    int i=0;
    for( auto tra: trans){

        bool remove= false;
        remove=foundTransitionSubgrafh(bhStates,tra);
        if(not remove){
            indiciT.push_back(i);
        }
        i++;
    }

   //dovrebbe essere int auto i=indiciT.size()-1 ;i>=0 ;i--
   
    for(  int i=indiciT.size()-1 ;i>=0 ;i--){
        trans.erase(trans.begin()+indiciT[i]);
    }
    dictionary->setBhState(bhStates);
    dictionary->setBehavioralTransitions(trans);
    dictionary->setTinfo(tInfo);
    return dictionary;
}

bool foundBhevioralState(std::vector<std::shared_ptr<NFAStateInterface>> behavioralStates,std::shared_ptr<NFAStateInterface> behavioralState){
    for( auto state: behavioralStates){
        if(state->getIdd()==behavioralState->getIdd()){
            return true;
            break;
        }
    }
    return false;
}

bool foundBhevioralTransition(std::vector<std::shared_ptr<NFATransitionInterface>> behavioralTransitions ,std::shared_ptr<NFATransitionInterface> behavioralTransition){
    for( auto transition: behavioralTransitions){
        if(transition->getId()==behavioralTransition->getId()){
            return true;
            break;
        }
    }
    return false;
}


std::vector<std::shared_ptr<NFATransitionInterface>> DFAConstructor::foundBehavioralTransitionDictionary( std::vector<std::shared_ptr<NFAStateInterface>>  bhStates) {
    std::vector<std::shared_ptr<NFATransitionInterface>> behavioralTransitions;
    for( auto state : bhStates){
        for(auto t: state->getInTransitions()){
            if (not(foundBhevioralTransition(behavioralTransitions,t))){
                behavioralTransitions.push_back(t);
            }

        }
        for(auto t: state->getOutTransitions()){
            if (not(foundBhevioralTransition(behavioralTransitions,t))){
                behavioralTransitions.push_back(t);
            }

        }
    }
    return behavioralTransitions;

}
std::vector<std::shared_ptr<NFAStateInterface>>  DFAConstructor::foundNfaStateDictionary(std::shared_ptr<Dictionary> dictionary){
    std::vector<std::shared_ptr<NFAStateInterface>> behavioralStates;
    for ( auto state: dictionary->states){
        for(auto bhstate: state->nfaStates){
           // int i=0;
            if(not(foundBhevioralState(behavioralStates,bhstate))){
                behavioralStates.push_back(bhstate);
            }

        }
    }

    return behavioralStates;


}


std::shared_ptr<SimpleNetwork> DFAConstructor::constructObservation(
    std::shared_ptr<NFASpaceInterface> inputSpace,bool online, std::shared_ptr<long> maxExecTime) {

    auto startingTime = std::chrono::high_resolution_clock::now();
    Logger::section("Observation Construction");
    std::string message;
    int nextStateID = 0;
    std::map<int, std::shared_ptr<SimpleTransition>> observationTransitions;

    auto dfaStates = subsetConstruction(inputSpace,online ,maxExecTime);
    std::shared_ptr<NFAStateInterface> startingState = nullptr;

    for (const auto &state : inputSpace->getStates())
        if (state->isStarting()) {
            startingState = std::shared_ptr<NFAStateInterface>(state);
            break;
        }

    for(const auto &transition : inputSpace->getTransitions()) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        if(maxExecTime && elapsed > *maxExecTime){
            std::vector<std::string> errorMessages = {
                "Maximum execution time exceeded during observation construction! (" + std::to_string(elapsed) + ")",
                "  Current task: constructing dictionary transitions from DFA transitions"
            };
            Logger::err(errorMessages);
            throw std::runtime_error("execution time");
        }
        std::string label = transition->getObservabilityLabel();
        auto newTransition = std::make_shared<SimpleTransition>(transition->getId(),label);
        observationTransitions.insert(std::pair<int, std::shared_ptr<SimpleTransition>>(transition->getId(), newTransition));
    }



    bool isFinal;
    bool starter = false;

    std::vector<std::shared_ptr<SimpleState>> states;
    for (const auto &state : *dfaStates) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        if(maxExecTime && elapsed > *maxExecTime){
            std::vector<std::string> errorMessages = {
                "Maximum execution time exceeded during observation construction! (" + std::to_string(elapsed) + ")",
                "  Current task: constructing dictionary states from DFA states"
            };
            Logger::err(errorMessages);
            throw std::runtime_error("execution time");
        }
        isFinal = false;
        std::vector<std::shared_ptr<SimpleTransition>> inTransitions = std::vector<std::shared_ptr<SimpleTransition>>();
        std::vector<std::shared_ptr<SimpleTransition>> outTransitions = std::vector<std::shared_ptr<SimpleTransition>>();
        for(auto &inTransition : state->in){
            inTransitions.push_back(observationTransitions[inTransition->getId()]);
        }
        for(auto &outTransition : state->out){
            outTransitions.push_back(observationTransitions[outTransition->getId()]);
        }

        // Se non ho ancora identificato lo stato iniziale, e questo stato rappresenta
        // anche lo stato dell' NFA iniziale, allora lo prendo come iniziale
        if(!starter && state->nfaStates.find(startingState) != state->nfaStates.end()){
            starter = true;
            for (const auto &behavioralState : state->nfaStates)
                if (behavioralState->isFinal()) {
                    isFinal = true;
                    break;
                }
            states.push_back(
                    std::make_shared<SimpleState>(nextStateID++, inTransitions, outTransitions, isFinal, true));

        }
        else {
            for (const auto &behavioralState : state->nfaStates)
                if (behavioralState->isFinal()) {
                    isFinal = true;
                    break;
                }
            states.push_back(
                    std::make_shared<SimpleState>(nextStateID++, inTransitions, outTransitions, isFinal, false));
        }
    }

    std::vector<std::shared_ptr<SimpleTransition>> transitions;
    std::transform( observationTransitions.begin(), observationTransitions.end(), std::back_inserter( transitions ), second(observationTransitions));

    std::vector<std::string> messages = {
            "Observation construction completed.",
            "  States: " + std::to_string(states.size()),
            "  Transitions: " + std::to_string(transitions.size())
    };
    Logger::log(messages);

    if (maxExecTime) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        *maxExecTime -= elapsed;
    }
    return std::make_shared<SimpleNetwork>(states, transitions);
}


void DFAConstructor::epsilonClosure(NFASpaceInterface &space, std::shared_ptr<NFAStateInterface> state,
                                           std::set<std::shared_ptr<NFAStateInterface>> &closure) {
    auto  n= closure.size();
    closure.insert(state);
    //auto nn= closure.size();
    auto OutT=state->getOutTransitions();
    for (const auto &outTransition :OutT ) {
        if (outTransition->getObservabilityLabel().empty()) {
            for (const auto &toState : space.getStates()) {
                for (const auto &inTransition : toState->getInTransitions())
                    if(inTransition == outTransition){
                        auto ts = std::shared_ptr<NFAStateInterface>(toState);
                        closure.insert(ts);
                        //if(ts!=state){
                        auto  m= closure.size();
                        if(m>n){
                        epsilonClosure(space, ts, closure);
                        }
                        break;
                    }
            }
        }
    }
}

void
DFAConstructor::epsilonClosure(NFASpaceInterface &space, std::set<std::shared_ptr<NFAStateInterface>> &states,
                                      std::set<std::shared_ptr<NFAStateInterface>> &closure) {
    for (const auto &state : states)
        epsilonClosure(space, state, closure);

}

void DFAConstructor::move(NFASpaceInterface &space, std::set<std::shared_ptr<NFAStateInterface>> &fromStates,
                                 std::set<std::shared_ptr<NFAStateInterface>> &toStates,
                                 std::string &symbol) {
    for (const auto &fromState : fromStates) {
        for (const auto &outTransition : fromState->getOutTransitions()) {
            if (outTransition->getObservabilityLabel() == symbol) {
                for (const auto &toState : space.getStates()) {
                    for (const auto &inTransition : toState->getInTransitions()) {
                        if(inTransition == outTransition){
                            toStates.insert(std::shared_ptr<NFAStateInterface>(toState));
                            break;
                        }
                    }
                }
            }
        }
    }
}


bool compareStateSets(std::set<std::shared_ptr<NFAStateInterface>> lhs, std::set<std::shared_ptr<NFAStateInterface>> rhs) {
    if (lhs.size() != rhs.size()) return false;
    for (const auto &leftBS : lhs) {
        bool found = false;
        for (const auto &rightBS : rhs)
            if (rightBS == leftBS) {
                found = true;
                break;
            }
        if (!found) return false;
    }
    return true;
}



bool comparePairSets(std::vector<std::vector<std::shared_ptr<NFAStateInterface>>> pairs,
                     std::vector<std::shared_ptr<NFAStateInterface>> pair) {
    if (pairs.size()==0 || (pairs.size() != 0 && pair.size()!= 0)) {
        bool found = false;
        for ( auto &p: pairs) {

            if ((p[0]==pair[0]) && (p[1]==pair[1])){
                found = true;
                break;
            }

        }
        if (!found) return false;

    }
    return true;

}

bool comparePairSets2(std::vector<std::vector<unsigned int>> pairs,
                      std::vector<unsigned int> pair) {
    if (pairs.size()==0 || (pairs.size() != 0 && pair.size()!= 0)) {
        bool found = false;
        for ( auto &p: pairs) {

            if ((p[0]==pair[0]) && (p[1]==pair[1])){
                found = true;
                break;
            }

        }
        if (!found) return false;

    }
    return true;

}


std::vector<std::vector<unsigned int>> convertPairSets(std::vector<std::vector<std::shared_ptr<NFAStateInterface>>> pairs){
    std::vector<std::vector<unsigned int>>   v2(sizeof(pairs), std::vector< unsigned int>(2));
    //int n= sizeof(pairs);
    int i=0;
    int j=0;
    for (auto &pair: pairs ){
        j=0;
        for( auto  &el: pair){


            v2[i][j]=el->getIdd();
            j++;
        }
        i++;
    }
    return v2;
}








std::shared_ptr<std::set<std::set<std::string>>> DFAConstructor::diagnoseFromObservation(
    const std::shared_ptr<Dictionary> inDictionary, std::vector<std::string> &observation,
    std::shared_ptr<long> maxExecTime) {
    auto startingTime = std::chrono::high_resolution_clock::now();
    std::string message;
    std::shared_ptr<std::set<std::set<std::string>>> result(new std::set<std::set<std::string>>());

    Logger::section("Diagnosis Extraction");

    Logger::log("Input Observation:");
    for (int i = 0; i < observation.size(); i++) Logger::log("  " + std::to_string(i) + ". " + observation[i]);

    inDictionary->setToInitialState();
    bool error;
    for(auto &label : observation){
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        if(maxExecTime && elapsed > *maxExecTime){
            std::vector<std::string> messages = {
                "Maximum execution time exceeded during diagnosis extraction! (" + std::to_string(elapsed) + ")",
                "  Current task: observation label " + label
            };
            Logger::err(messages);
            throw std::runtime_error("execution time");
        }
        error = true;

        std::shared_ptr<std::vector<std::shared_ptr<DFAState>>> dfaStates(new std::vector<std::shared_ptr<DFAState>>());
        std::shared_ptr<std::vector<std::shared_ptr<temporalAbduction>>> temporalAbductions(new std::vector<std::shared_ptr<temporalAbduction>>());
        for(auto &outTransition : inDictionary->states[inDictionary->currentState]->out){


            auto tempa= std::make_shared <temporalAbduction> ((inDictionary->states[inDictionary->currentState]));
            temporalAbductions->push_back(tempa );
            std::shared_ptr<DictionaryState> dic=inDictionary->states[inDictionary->currentState];
            if (outTransition->getObservabilityLabel() == label) {
                inDictionary->performTransition(outTransition);
                error = false;
                break;
            }
        }
        if(error){
            if (maxExecTime) {
                elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
                *maxExecTime -= elapsed;
            }
            throw std::invalid_argument("Not a linear observation (doesn't define a valid route)");
        }
    }
    if (inDictionary->states[inDictionary->currentState]->final) {
        result = std::make_shared<std::set<std::set<std::string>>>(inDictionary->states[inDictionary->currentState]->diagnosis);
    }
    else {
        if (maxExecTime) {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
            *maxExecTime -= elapsed;
        }
        throw std::invalid_argument("Not a linear observation (doesn't end on final state)");
    }

    std::vector<std::string> messages = {
        "Diagnosis extraction completed.",
        "  Number of sets: " + std::to_string(result->size())
    };
    Logger::log(messages);

    if (maxExecTime) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        *maxExecTime -= elapsed;
    }
    return result;

}

void DFAConstructor::createPair(std::shared_ptr<DFAState> state) {
   /* std::vector<std::vector<std::shared_ptr<NFAStateInterface>>> pairs;*/
    std::vector<std::vector<unsigned  int>> pairs;

    for (const auto &behavioralState : state->nfaStates) {
        for (const auto &transition: behavioralState->getInTransitions()){
            std::string result =transition->getObservabilityLabel();
            if (result!= ""){

                std::shared_ptr<NFAStateInterface> ts,tf;

                ts=behavioralState;
                tf= nullptr;
                move3(ts,tf,state,pairs);

            }
        }




    }
    pairs=state->getPairs();
   /* convertPairSets(pairs);*/

}


/*
void DFAConstructor::move2(std::shared_ptr<NFAStateInterface> ts,
                           std::shared_ptr<NFAStateInterface> tf,
                           std::shared_ptr<DFAState> state,
                           std::vector<std::vector<std::shared_ptr<NFAStateInterface>>> pairs) {
    if (tf != nullptr) {
        for (const auto &outTransition : tf->getOutTransitions()) {
            if (outTransition->getObservabilityLabel() != "") {
                std::vector<std::shared_ptr<NFAStateInterface>> pair;
                pair.push_back(ts);

                pair.push_back(tf);
                *//*if (!(comparePairSets(state->pairs, pair))) {*//*
                if (!(comparePairSets(state->pairs, pair))) {
                    state->addPair(pair);

                }


            } else {
                bool found;
                std::shared_ptr<NFAStateInterface> nextState;
                found = false;
                for (const auto &toState : state->nfaStates) {


                    for (const auto &inTransition : toState->getInTransitions()) {
                        if (inTransition == outTransition) {
                            found = true;
                            nextState = toState;
                            break;
                        }
                        if (found && nextState != nullptr) {
                            move2(ts, nextState, state, pairs);

                        }
                    }

                }


            }
        }

    } else {
        for (const auto &outTransition : ts->getOutTransitions()) {
            if (outTransition->getObservabilityLabel() != "") {
                std::vector<std::shared_ptr<NFAStateInterface>> pair;
                pair.push_back(ts);
                if (tf == nullptr) {
                    tf = ts;
                }
                pair.push_back(tf);
                if (!(comparePairSets(state->pairs, pair))) {
                    state->addPair(pair);

                }


            } else {
                bool found;
                std::shared_ptr<NFAStateInterface> nextState;
                found = false;
                for (const auto &toState : state->nfaStates) {


                    for (const auto &inTransition : toState->getInTransitions()) {
                        if (inTransition == outTransition) {
                            found = true;
                            nextState = toState;
                            break;
                        }
                        if (found && nextState != nullptr) {
                            move2(ts, nextState, state, pairs);

                        }
                    }

                }


            }
        }

    }
}*/


void DFAConstructor::move3(std::shared_ptr<NFAStateInterface> ts,
                           std::shared_ptr<NFAStateInterface> tf,
                           std::shared_ptr<DFAState> state,
                           std::vector<std::vector<unsigned  int>> pairs) {
    if (tf != nullptr) {
        for (const auto &outTransition : tf->getOutTransitions()) {
            if (outTransition->getObservabilityLabel() != "") {
                std::vector<unsigned int> pair;
                pair.push_back(ts->getIdd());

                pair.push_back(tf->getIdd());
                if (!(comparePairSets2(state->pairs, pair))) {
                    state->addPair(pair);

                }


            } else {
                bool found;
                std::shared_ptr<NFAStateInterface> nextState;
                found = false;
                for (const auto &toState : state->nfaStates) {


                    for (const auto &inTransition : toState->getInTransitions()) {
                        if (inTransition == outTransition) {
                            found = true;
                            nextState = toState;
                            break;
                        }
                        if (found && nextState != nullptr) {
                            
                            move3(ts, nextState, state, pairs);

                        }
                    }

                }


            }
        }

    } else {
        for (const auto &outTransition : ts->getOutTransitions()) {
            if (outTransition->getObservabilityLabel() != "") {
                /*std::vector<std::shared_ptr<NFAStateInterface>> pair;*/
                std::vector<unsigned int> pair;
                pair.push_back(ts->getIdd());
                if (tf == nullptr) {
                    tf = ts;
                }
                pair.push_back(tf->getIdd());
                if (!(comparePairSets2(state->pairs, pair))) {
                    state->addPair(pair);

                }


            } else {
                bool found;
                std::shared_ptr<NFAStateInterface> nextState;
                found = false;
                for (const auto &toState : state->nfaStates) {


                    for (const auto &inTransition : toState->getInTransitions()) {
                        if (inTransition == outTransition) {
                            found = true;
                            nextState = toState;
                            break;
                        }
                        if (found && nextState != nullptr) {
                            move3(ts, nextState, state, pairs);

                        }
                    }

                }


            }
        }

    }
}







void abduce( const std::shared_ptr<Dictionary> inDictionary,std::vector<std::string> &observation,std::string nextObservation,
        std::shared_ptr<std::vector<std::shared_ptr<temporalAbduction>>> state){
    std::shared_ptr<std::vector<std::shared_ptr<temporalAbduction>>> newAb(new std::vector<std::shared_ptr<temporalAbduction>>());

    for (auto &temporal: *state){

        auto newT=temporal;
        newAb->push_back(newT);

    }
}

 /*void DFAConstructor::compareAbduction(std::shared_ptr<std::vector<std::shared_ptr<temporalAbduction>>> newAb, const std::shared_ptr<Dictionary> inDictionary){
    int i=0;
    int j=1;
    auto ss=newAb->size();
    j=ss-1;
    i=j-1;
    auto transition=inDictionary->getBehavioralTransitions();
     std::vector< std::vector<std::vector<std::string>>> diagnn;
    std::set<std::set<std::string>> s;
    bool starting;
    starting=true;
     for( auto t : *newAb){


        std::vector<std::string> set;

        std::vector<std::vector<std::string>> diagn;
         int c=1;
         if (starting){
             set.push_back("esp");
             diagn.push_back(set);

             t.get()->setDiagn(diagn);
             starting=false;

         }
         s=t->diagnosis;


         bool firstSet = true;
         for (auto diagSet : s) {



             bool firstLabel = true;
             std::vector<std::string> set;
             for (const auto &diag : diagSet) {

                 if (diag.size()==0 ) {
                     std::string esp = "esp";
                     set.push_back(esp);
                 }
                 else{
                     set.push_back(diag);
                 }
                 firstLabel = false;
             }
             if (set.size()==0){
                 set.push_back("esp");
             }



             diagn.push_back(set);
             t.get()->setDiagn(diagn);


             firstSet = false;
         }

             diagnn.push_back(diagn);


       *//* s=d->getDiagnosis();*//*
    }
     auto dia = newAb.get()[0];
     i=0;
     j=1;
    while (j<=ss-1){

        auto diaI=dia[i];
        auto diaJ = dia[j];
        int h=0;
        std::vector<int> remov;
        for ( auto &candidate: diagnn[i]){

            bool found;

            found=compareDiagnosis(candidate, diagnn[j]);

            if (!found){
                remov.push_back(h);

            }

                h++;

                    *//*bool back=false;
                    int found=0;
                    for(auto el: candidate){
                        found= false;
                        for( auto &candidateNext: diagnn[j]){
                        for (auto  el2: candidateNext){
                            if (el!=el2){
                                if (diagnn[j].size()<diagnn[i].size()){
                                   back=true;
                                }
                        }
                }*//*
                }
                bool eliminato=false;
                while (remov.size()>0){
                    auto h =remov.back();
                    remov.pop_back();

                    auto dicState= inDictionary->states[dia[i]->acceptingState];
                    auto reach=dia[i]->reachStates[0];
                    std::vector<int> remReach;
                    for (auto state :reach){
                        auto bhState=inDictionary->states[dia[i]->acceptingState]->nfaStates2;
                        bool foundR=false;
                        for ( auto bh: bhState){
                            if( bh->getIdd() == state ){
                                auto la= bh->getRelevancyLabels();
                                int c=0;
                                for (auto diagSet : la) {
                                   auto dd= diagnn[i];

                                   for (auto d :dd){
                                       if (d[0]==diagSet){
                                           foundR= true;

                                       }

                                   }
                                    *//*if (diagSet==){

                                    }*//*



                                }

                            }
                        }
                    if (foundR){
                        remReach.push_back(state);
                    }

                    }
                    for (auto n :remReach){
                        int z=0;

                        for(auto nn:  dia[i]->reachStates[0]){

                            if(nn==n){
                                dia[i]->reachStates[0].erase(dia[i]->reachStates[0].begin()+z) ;
                            }
                            z++;
                        }
                    }
                    diagnn[i].erase(diagnn[i].begin()+h);
                    dia[i]->setDiagn(diagnn[i]);
                    eliminato=true;
                }


        if (eliminato){
            i=0;
            j=1;
        }
        else{
            i++;
            j++;

        }

*//*
            }


       auto d= diaI.back()->getDiagnosis();
*//*


    }}*/


std::shared_ptr<transitionsInfo> getTransitionFromObs(std::string obs,unsigned int source,std::shared_ptr<std::vector<std::shared_ptr<transitionsInfo>>> tInfo){

    for(  auto t: *tInfo){
        std::string  label=t->getLabel();
        if( t->getStart()==source && label==obs){
            return t;
        }
    }
    return nullptr;
}

bool foundTransition(std::vector< unsigned int> destination,int source,std::shared_ptr<std::vector<std::shared_ptr<transitionsInfo>>> tInfo){

    for(  auto t: *tInfo){
        for(auto state:destination){
            if(t->start==source && t->destination==state ){
                return true;
            }
        }

    }
    return false;
}
bool foundStateInVector(int source,std::vector< unsigned int> nfaSource)    {
    for(auto e :nfaSource){
        if (e==source){
            return true;
            break;
        }
    }
    return false;
}
bool foundTransition2(std::vector< unsigned int> destination,int source,std::vector< unsigned int> nfaSource,std::shared_ptr<std::vector<std::shared_ptr<transitionsInfo>>> tInfo){

    for(  auto t: *tInfo){
        for(auto state:destination){
            if(t->start==source ){
                if(t->destination==state ) {
                    return true;
                }
                else if(foundStateInVector(t->destination,nfaSource)){
                    foundTransition2(destination,t->start,nfaSource,tInfo);
                    break;

            }

            }
        }

    }



    return false;

    }

bool foundTransition3(std::vector< unsigned int> destination,int source,std::vector< unsigned int> nfaSource,std::shared_ptr<std::vector<std::shared_ptr<transitionsInfo>>> tInfo) {
    std::shared_ptr<std::vector<std::shared_ptr<transitionsInfo>>> tInfoNew;
    std::vector<unsigned int> newNfa;
    for (auto t: *tInfo) {
        for (auto state:destination) {
            if (t->start == source && t->destination == state) {
                return true;
            }
            if (t->start == source && foundStateInVector(t->destination, nfaSource) &&
                !foundStateInVector(t->destination, newNfa)) {
                newNfa.push_back(t->destination);
            }
        }

    }
    while (newNfa.size() > 0) {
        std::vector<unsigned int>  tempNewNfa;
        for (auto s: newNfa) {
            for (auto t: *tInfo) {
                for (auto state:destination) {
                    if (t->start == s && t->destination == state) {
                        return true;
                    }
                    if (t->start == s && foundStateInVector(t->destination, nfaSource) &&
                        !foundStateInVector(t->destination, newNfa)) {
                        tempNewNfa.push_back(t->destination);
                    }
                }
            }

        }
        newNfa=tempNewNfa;

    }
    return false;
}

/*void DFAConstructor::compareAbduction2(std::shared_ptr<std::vector<std::shared_ptr<temporalAbduction>>> newAb, const std::shared_ptr<Dictionary> inDictionary,std::vector<std::string> &observation){
    int i=0;
    int j=1;
    auto ss=newAb->size();
    j=ss-1;
    i=j-1;
    auto transition=inDictionary->getBehavioralTransitions();
    std::vector< std::vector<std::vector<std::string>>> diagnn;
    std::set<std::set<std::string>> s;
    bool starting;
    starting=true;
    for( auto t : *newAb){


        std::vector<std::string> set;

        std::vector<std::vector<std::string>> diagn;
        int c=1;
        if (starting){
            set.push_back("esp");
            diagn.push_back(set);

            t.get()->setDiagn(diagn);
            starting=false;

        }
        s=t->diagnosis;


        bool firstSet = true;
        for (auto diagSet : s) {



            bool firstLabel = true;
            std::vector<std::string> set;
            for (const auto &diag : diagSet) {

                if (diag.size()==0 ) {
                    std::string esp = "esp";
                    set.push_back(esp);
                }
                else{
                    set.push_back(diag);
                }
                firstLabel = false;
            }
            if (set.size()==0){
                set.push_back("esp");
            }



            diagn.push_back(set);
            t.get()->setDiagn(diagn);


            firstSet = false;
        }

        diagnn.push_back(diagn);


        *//* s=d->getDiagnosis();*//*
    }
    auto dia = newAb.get()[0];
    i=0;
    j=1;
    bool start=true;
    auto tInformations=inDictionary->tInfo;
    int hh=0;
    int prec;
    while (j<=ss-1) {
        if (i==3){
            int ci=2;
        }

        auto diaI = dia[i];
        auto diaJ = dia[j];
        int h = 0;
        bool reset = false;
        std::vector<int> remov;
        std::vector<int> remain;
        *//* verifyReach(inDictionary,dia[i]->reachStates[0],dia[j]->reachStates[0]);*//*
        if (i > 0) {
            std::shared_ptr<transitionsInfo> dicInf;
            dicInf = getTransitionFromObs(observation[hh], dia[i - 1]->st->getIdd(), inDictionary->tInfoDic);
            auto reachI = dia[i]->reachStates[0];
            auto reachJ = dia[j]->reachStates[0];
            std::vector<unsigned int> nfast;
            for (auto s:dia[i]->bhState){
                nfast.push_back(s->getIdd());
            }
            for (auto stateI: reachI) {

                bool condition = foundTransition3(reachJ, stateI,nfast, tInformations);
                if (!condition) {
                    remov.push_back(stateI);
                }
                   *//*bool found=false;
                   for(auto transForState: *dicInf->tInfo)    {
                       if(transForState->getStart()==stateI){
                           found=true;
                       }
                   }
                   if(found){
                       remov.push_back(stateI);
                   }*//*
            }

            for (auto bhSt:dia[i]->bhState) {
                bool found = false;
                *//*for (auto transForState: *dicInf->tInfo) {
                    if (transForState->getDestination() == bhSt->getIdd()) {
                        found = true;
                    }

                }*//*
                found=foundTransition3(reachJ, bhSt->getIdd(),nfast, tInformations);

                if (!found) {
                    bool insert = true;
                    for (auto n: remov) {
                        if (n == bhSt->getIdd()) {
                            insert = false;
                            break;
                        }
                    }
                    if (insert)
                        remov.push_back(bhSt->getIdd());
                }
                *//*else{
                    bool insert=true;
                    for ( auto n: remain){
                        if(n==bhSt->getIdd()){
                            insert= false;
                            break;
                        }
                    }
                    if (insert)
                        remain.push_back(bhSt->getIdd());
                }*//*
            }
            for (auto el2 : dia[i]->bhState) {
                bool found = false;
                for (auto el: remov) {
                    if (el2->getIdd() == el) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    remain.push_back(el2->getIdd());
                }
            }
            std::vector<std::vector<std::string>> diagnosis;
            bool modify = false;
            for (auto n :remain) {
                modify = true;
                int z = 0;
                for (auto bhSt:dia[i]->bhState) {
                    bool condition=false;
                    dicInf = getTransitionFromObs(observation[hh+1], dia[i]->st->getIdd(), inDictionary->tInfoDic);
                    for(auto dd:*dicInf->tInfo){
                        if (dd->start==bhSt->getIdd()){
                            condition=true;
                        }
                    }
                    if(condition){
                        if (bhSt->getIdd() == n) {
                            auto diagn = bhSt->getDiagnosis();
                            if (diagn.size() == 0) {
                                std::vector<std::string> set;
                                set.push_back("esp");
                                diagnosis.push_back(set);
                            } else {
                                for (auto diagSet : diagn) {
                                    bool firstLabel = true;
                                    std::vector<std::string> set;
                                    for (const auto &diag : diagSet) {
                                        if (diag.size() == 0) {
                                            std::string esp = "esp";
                                            set.push_back(esp);
                                        } else {
                                            set.push_back(diag);
                                        }
                                        firstLabel = false;
                                    }
                                    if (set.size() == 0) {
                                        set.push_back("esp");
                                    }
                                    diagnosis.push_back(set);
                                }
                            }

                        }
                    }

                }
            }
            if (modify) {
                if(dia[i].get()->diagn.size()!=diagnosis.size()){
                    dia[i].get()->setDiagn(diagnosis);
                    reset = true;
                }

            }
            for (auto n :remov) {
                int z = 0;
                for (auto nn:  dia[i]->reachStates[0]) {
                    if (nn == n) {
                        dia[i]->reachStates[0].erase(dia[i]->reachStates[0].begin() + z);
                        reset = true;
                    }
                    z++;
                }
                int zz=0;
                for(auto bh : dia[i]->bhState){
                    if(bh->getIdd()==n){
                        dia[i]->bhState.erase(dia[i]->bhState.begin() + zz);
                        break;

                    }
                    zz++;
                }
            }
            hh++;

        }
        if (reset) {
            i = 0;
            j = 1;
            hh=0;

        } else {
            prec = i;
            i++;
            j++;



        }
    }

*//*
        for ( auto &candidate: diagnn[i]){

            bool found;

            found=compareDiagnosis(candidate, diagnn[j]);

            if (!found){
                remov.push_back(h);

            }

            h++;

            bool back=false;
            int found=0;
            for(auto el: candidate){
                found= false;
                for( auto &candidateNext: diagnn[j]){
                for (auto  el2: candidateNext){
                    if (el!=el2){
                        if (diagnn[j].size()<diagnn[i].size()){
                           back=true;
                        }
                }
        }*//*
        }*/
        /*bool eliminato=false;
        while (remov.size()>0){
            auto h =remov.back();
            remov.pop_back();
            auto dicState= inDictionary->states[dia[i]->acceptingState];
            auto reach=dia[i]->reachStates[0];
            std::vector<int> remReach;
            for (auto state :reach){
                auto bhState=inDictionary->states[dia[i]->acceptingState]->nfaStates2;
                bool foundR=false;
                for ( auto bh: bhState){
                    if( bh->getIdd() == state ){
                        auto la= bh->getRelevancyLabels();
                        int c=0;
                        for (auto diagSet : la) {
                            auto dd= diagnn[i];

                            for (auto d :dd){
                                if (d[0]==diagSet){
                                    foundR= true;
                                }
                            }
                            *//*if (diagSet==){

                            }*//*

                        }

                    }
                }
                if (foundR){
                    remReach.push_back(state);
                }

            }
            for (auto n :remReach){
                int z=0;

                for(auto nn:  dia[i]->reachStates[0]){

                    if(nn==n){
                        dia[i]->reachStates[0].erase(dia[i]->reachStates[0].begin()+z) ;
                    }
                    z++;
                }
            }
            diagnn[i].erase(diagnn[i].begin()+h);
            dia[i]->setDiagn(diagnn[i]);
            eliminato=true;
        }*/




/*
            }


       auto d= diaI.back()->getDiagnosis();
*/



bool DFAConstructor::compareDiagnosis(std::vector<std::string> diagnosis, std::vector<std::vector<std::string>> diagnosisNext){
    bool found=true;
    for(auto candi: diagnosisNext){

        for( auto candidate: diagnosis){
            found=false;
            for( auto candidateNext: candi){
                if (candidate==candidateNext or candidate=="esp"){
                   found=true;
                }

            }
        }
        if (found)
            return found;
    }
    return found;


}

/*bool DFAConstructor::compareDiagnosis2(std::vector<std::string> diagnosis, std::vector<std::vector<std::string>> diagnosisNext){
    bool found;
    for(auto candi: diagnosisNext){
        if candi
        for( auto candidate: diagnosis){
            found=false;
            for( auto candidateNext: candi){
                if (candidate==candidateNext or candidate=="esp"){
                    found=true;
                }

            }
        }
        if (found)
            return found;
    }
    return found;


}*/

bool verifyReach(    std::vector<std::shared_ptr<BehavioralState>> bhState,std::shared_ptr<BehavioralState> start,std::shared_ptr<BehavioralState> destination, std::shared_ptr<std::vector<std::shared_ptr<transitionsInfo>>>tInfo){
    std::queue<std::shared_ptr<BehavioralState>> toVisit;
    toVisit.push(start);
    std::vector<unsigned int> stati;
    for (auto s:bhState){
        stati.push_back(s->getIdd());
    }

        if(start==destination){
            return true;

        }

    while (!toVisit.empty()) {
        auto state=toVisit.front();
        for(auto t :*tInfo){
            if(t->start==state->getIdd() and t->label=="" and foundStateInVector(t->getDestination(),stati)){
                if(t->getDestination()==destination->getIdd()){
                    return  true;
                }
                for (auto s :bhState){
                    if(s->getIdd()==t->getDestination()){
                        toVisit.push(s);
                    }
                }

            }


        }
        toVisit.pop();
    }
    return false;


    }
std::vector<std::shared_ptr<temporalAbduction>> computeTemporalAbduce(std::vector<std::shared_ptr<temporalAbduction>> newAb,std::shared_ptr<Dictionary> inDictionary, std::vector<std::string> &observation) {
    auto starting = inDictionary->states[inDictionary->currentState];
    int i = 0;
    int j = 0;
    auto nextObs = observation[i];
    std::vector<unsigned int> reachState;
    std::vector<unsigned int> out;
    bool connected=false;
    auto tA = std::make_shared<temporalAbduction>(inDictionary->states[inDictionary->currentState]);
    for (auto &state: inDictionary->states[inDictionary->currentState]->nfaStates2) {
        reachState.push_back(state->getIdd());

        for (auto t: state->getOutTransitions2()) {
            if (t->getObservabilityLabel() == nextObs) {
                out.push_back(state->getIdd());
                connected=true;
            }
        }
    }

    if (!connected){
        std::cout << "not connected";
        return newAb;
    }
    tA->addReachState(reachState);
    tA->addOutState(out);
    tA->setDiagnosis(inDictionary->states[inDictionary->currentState]->diagnosis);

    tA->setAcceptingState(inDictionary->states[inDictionary->currentState]->getIdd());
    std::set<std::set<std::string>> jjj = inDictionary->states[inDictionary->currentState]->diagnosis;

   // auto stato = inDictionary->currentState;
    bool found = false;
    newAb.push_back(tA);
    for (int h = 0; h < observation.size(); h++) {
        found = false;
        std::shared_ptr<NFATransitionInterface> transInter;
        nextObs=observation[h];
        for (auto transition: inDictionary->states[inDictionary->currentState]->getOutTransitions()) {
            if (transition->getObservabilityLabel() == nextObs) {
                transInter = transition;
                found = true;
                break;
            }
        }
        if (found) {
            inDictionary->performTransition(transInter);
            auto tA = std::make_shared<temporalAbduction>(inDictionary->states[inDictionary->currentState]);
            std::vector<unsigned int> reachState;
            std::vector<unsigned int> in;
            std::vector<unsigned int> out;
            for (auto &state: inDictionary->states[inDictionary->currentState]->nfaStates2) {
                for (auto inT :state->getInTransitions()) {
                    if (inT->getObservabilityLabel() == nextObs) {

                        in.push_back(state->getIdd());
                        break;
                    }
                }
                for (auto outT :state->getOutTransitions()) {
                    if (outT->getObservabilityLabel() != "") {

                        out.push_back(state->getIdd());
                        break;
                    }
                }
            }
                tA->addReachState(in);
                tA->addInState(in);
                tA->addOutState(out);
                tA->setAcceptingState(inDictionary->states[inDictionary->currentState]->getIdd());
                tA->setDiagnosis(inDictionary->states[inDictionary->currentState]->diagnosis);
                newAb.push_back(tA);

                for (j = h; j >= 0; j--) {
                    std::vector<unsigned int> outN;
                    auto outPrec = newAb[j]->out;
                    auto apcState = newAb[j]->acceptingState;
                    std::vector<std::shared_ptr<BehavioralState>> uscite;
                    for (int k = 0; k < outPrec.size(); k++) {
                        auto stati = inDictionary->states[apcState]->nfaStates2;
                        for (auto sta:  stati) {
                            if (sta->getIdd()==outPrec[k]){
                                for(auto oT:sta->getOutTransitions2()){
                                    if (oT->getObservabilityLabel()==observation[j]){
                                        uscite.push_back(sta);
                                        outN.push_back(sta->getIdd());
                                        break;
                                    }
                                }

                            }

                        }
                    }


                    auto out = std::set<std::set<std::string>>();
                    for (const auto &nfaState : uscite)
                        /*if (nfaState->isFinal())*/
                        for (auto &d : nfaState->getDiagnosis())
                            out.insert(d);
                    auto diagn=newAb[j]->getDiagnosis2();




                    if(diagn==out){
                        break;
                    }

                    else{



                        newAb[j]->setDiagnosis(out);
                        newAb[j]->out=outN;
                        std::vector<std::shared_ptr<BehavioralState>> starts;
                        for(auto h: newAb[j]->in){
                            for(auto s :inDictionary->states[newAb[j]->acceptingState]->nfaStates2){
                                if (s->getIdd()==h){
                                    starts.push_back(s);
                                    break;
                                }
                            }
                        }
                        std::vector<std::shared_ptr<BehavioralState>> destinations;
                        for(auto h: newAb[j]->out){
                            for(auto s :inDictionary->states[newAb[j]->acceptingState]->nfaStates2){
                                if (s->getIdd()==h){
                                    destinations.push_back(s);
                                    break;
                                }
                            }
                        }
                        std::vector<unsigned int> newIn;
                        for(auto start :starts){

                            for(auto destination:destinations){
                                if(verifyReach( inDictionary->states[newAb[j]->acceptingState]->nfaStates2 ,start, destination, inDictionary->tInfo)){
                                    newIn.push_back(start->getIdd());
                                    break;
                                }
                            }
                        }


                        ///verifica se è connesso
                        newAb[j]->setReach(newIn);
                    }

                }
                }
        else{
            return newAb;
        }
            }





        return newAb;
    }

std::vector<std::shared_ptr<temporalAbduction>> DFAConstructor::diagnoseFromAbduce(
        const std::shared_ptr<Dictionary> inDictionary, std::vector<std::string> &observation,
        std::shared_ptr<long> maxExecTime) {
    auto startingTime = std::chrono::high_resolution_clock::now();
    std::string message;
    std::shared_ptr<std::set<std::set<std::string>>> result(new std::set<std::set<std::string>>());

    Logger::section("Diagnosis Extraction");

    Logger::log("Input Observation:");
    for (int i = 0; i < observation.size(); i++) Logger::log("  " + std::to_string(i) + ". " + observation[i]);
/*
std::shared_ptr<std::vector<std::shared_ptr<temporalAbduction>>> newAb;
*/
    std::vector<std::shared_ptr<temporalAbduction>> newAb;
    for (auto state :inDictionary->states) {
        if (state->starting) {
            inDictionary->currentState = state->id;
            break;
        }
    }
    newAb = computeTemporalAbduce(newAb, inDictionary, observation);
    int indice=0;
    for (auto &t : newAb){
        std::cout << "\"Abduce\": "+ std::to_string(indice);
        //bool firstSet = true;
        std::cout << "{\"Reach State\":[ ";
        if(t->reachStates.size()>0){
            for (auto re : t->reachStates) {

                std::cout <<  std::to_string(re)+" ,";
            }
            std::cout << "]";
        }


        for (const auto &di : t->getDiagnosis2()) {
            std::cout << "{";
            for(const auto &d : di){


                std::cout << "\"" << d << ",\"";

            }
            std::cout << "}";


        }
        std::cout << "]\n";
        indice++;
    }

    return newAb;
}



/*

std::shared_ptr<std::vector<std::shared_ptr<temporalAbduction>>> computeTemporalAbduce(std::vector<std::shared_ptr<temporalAbduction>> newAb,std::shared_ptr<Dictionary> inDictionary, std::vector<std::string> &observation){
    auto starting=inDictionary->states[inDictionary->currentState];
    int i=0;
    int j=0;
    std::vector<unsigned int> reachState;
    auto tA = std::make_shared<temporalAbduction>(inDictionary->states[inDictionary->currentState]);
    for (auto &state: inDictionary->states[inDictionary->currentState]->nfaStates2) {
                reachState.push_back(state->getIdd());

    }


    tA->addReachState(reachState);

    tA->setDiagnosis(inDictionary->states[inDictionary->currentState]->diagnosis);

    tA->setAcceptingState(inDictionary->states[inDictionary->currentState]->getIdd());
    std::set<std::set<std::string>> jjj= inDictionary->states[inDictionary->currentState]->diagnosis;
    auto nextObs=observation[i+1];
    auto stato=inDictionary->currentState;
    bool found=false;
    newAb.push_back(tA);
    std::shared_ptr<NFATransitionInterface> transInter;
    for( auto transition: inDictionary->states[inDictionary->currentState]->getOutTransitions() ){
        if( transition->getObservabilityLabel()==nextObs){
            transInter=transition;
            found=true;
            break;

        }
    }
    if (found){
        inDictionary->performTransition(transInter);
        auto tA = std::make_shared<temporalAbduction>(inDictionary->states[inDictionary->currentState]);
        std::vector<unsigned int> reachState;

        for (auto &state: inDictionary->states[inDictionary->currentState]->nfaStates2) {
            for (auto out :state->getInTransitions()) {
                if (out->getObservabilityLabel() == nextObs) {
                    reachState.push_back(state->getIdd());
                }
            }
        tA->addReachState(reachState);
        tA->setAcceptingState(inDictionary->states[inDictionary->currentState]->getIdd());
        tA->setDiagnosis(inDictionary->states[inDictionary->currentState]->diagnosis);
        newAb.push_back(tA);
        j=i+1;
        for(j=i; j>=0;j--){
            auto reach= newAb[j]->reachStates;
            auto apcState=newAb[j]->acceptingState;
            std::vector<std::shared_ptr<BehavioralState>> uscite;
            for (int k=0; k<reach.size();k++){
                auto stati=inDictionary->states[apcState]->nfaStates2;
                for (auto sta:  stati){
                    bool fff=false;
                    if (reach[k]== sta->getIdd()){
                        for( auto t:sta->getOutTransitions2()){
                            if( t->getObservabilityLabel()==observation[j]){
                                fff=true;
                                break;

                            }
                        }
                        if(fff){
                            uscite.push_back(sta);
                        }
                    }
                }
            }


        auto out = std::set<std::set<std::string>>();
        for (const auto &nfaState : uscite)
            */
/*if (nfaState->isFinal())*//*

            for (auto &d : nfaState->getDiagnosis())
                out.insert(d);





        }
        }

    }



    return newAb;
}
*/




//std::vector<int> DFAConstructor::verifyReach(std::vector<std::shared_ptr<temporalAbduction>> current,
//                                             std::vector<std::shared_ptr<temporalAbduction>> next) {

  //  for (auto c :next) {
        /* foundTransition(c, inDictionary);*/
    //    int d = 1;

    //}
    //return 0;
    //}





int verificaEsistenzaStatoDic(std::shared_ptr<Dictionary> inDictionary,
                              std::vector<std::shared_ptr<BehavioralState>> statiNfa) {
    auto statiDizionario = inDictionary->states;
    int id = -1;
    for (auto dState: statiDizionario) {
        if (dState->nfaStates2.size() == statiNfa.size()) {
            bool trovato = false;
            for (auto statoN :statiNfa) {
                trovato = false;
                for (auto statoD :dState->nfaStates2) {
                    if (statoD->getIdd() == statoN->getIdd()) {
                        trovato = true;
                        break;
                    }
                }


            }
            if (trovato) {
                return dState->getIdd();
            }


        }
    }
    return id;
}



std::set<std::set<std::string>> getDiagnosisNewDicState(std::vector<std::shared_ptr<BehavioralState>> totBhs) {
    auto out = std::set<std::set<std::string>>();
    for ( auto nfaState : totBhs) {


        /*if (nfaState->isFinal())*/
        for (auto d : nfaState->getDiagnosis())
            out.insert(d);
    }
    //prima out senza niente riga sotto
    for (auto   di : out) {
        std::cout << "funzioe{";
        for(auto   d : di){


            std::cout << "\"" << d << ",\"";

        }
        std::cout << "}";


    }
    std::cout << "]\n";
    return out;
}

std::shared_ptr<dictabd> DFAConstructor::extendDictionaryFromObs(
        const std::shared_ptr<Dictionary> inDictionary, std::vector<std::string> &observation,std::string &net,
        std::shared_ptr<long> maxExecTime) {

    auto startingTime = std::chrono::high_resolution_clock::now();
    std::string message;
    std::shared_ptr<std::set<std::set<std::string>>> result(new std::set<std::set<std::string>>());

    Logger::section("Diagnosis Extraction");
    auto transictionId = inDictionary->transitions.size();
    //int lastIdDictionary = inDictionary->states.size() - 1;
    Logger::log("Input Observation:");
    for (int i = 0; i < observation.size(); i++) Logger::log("  " + std::to_string(i) + ". " + observation[i]);
/*
std::shared_ptr<std::vector<std::shared_ptr<temporalAbduction>>> newAb;
*/
    std::shared_ptr<std::vector<std::shared_ptr<temporalAbduction>>> newAb(
            new std::vector<std::shared_ptr<temporalAbduction>>());


    inDictionary->setToInitialState();
    //associazione stato osservazione poszione i con stato del dizionario contentuto
    std::vector<int> obsLabel;
    int starting = inDictionary->currentState;
    //bool start = true;
    bool error;
    obsLabel.push_back(starting);
    auto lunghezza = observation.size();

    /*for (auto &label : observation) {*/
    for (int i = 0; i < lunghezza; i++) {
        auto label = observation[i];

        error = true;
        for (auto &outTransition : inDictionary->states[inDictionary->currentState]->out) {
            if (outTransition->getObservabilityLabel() == label) {
                //int const prevState = inDictionary->currentState;
                inDictionary->performTransition(outTransition);
                error = false;
                obsLabel.push_back(inDictionary->currentState);

                auto tA = std::make_shared<temporalAbduction>(inDictionary->states[inDictionary->currentState]);
                std::vector<unsigned int> reachState;

                for (auto &state: inDictionary->states[inDictionary->currentState]->nfaStates2) {
                    for (auto out :state->getInTransitions()) {
                        if (out->getObservabilityLabel() == label) {
                            reachState.push_back(state->getIdd());
                        }
                    }
                }/*
                if (start){
                auto tAstart = std::make_shared<temporalAbduction>(inDictionary->states[starting]);
                tAstart->setAcceptingState(inDictionary->states[starting]->getIdd());
                std::set<std::set<std::string>> jjj= inDictionary->states[inDictionary->currentState]->diagnosis;

                tAstart->setState(inDictionary->states[starting]);
                tAstart->setBhState(inDictionary->states[starting]->nfaStates2);
                tAstart->setDiagnosis(inDictionary->states[starting]->diagnosis);
                tAstart->setAcceptingState(inDictionary->states[starting]->getIdd());
                start=false;
                newAb->push_back(tAstart);

                tA->addReachState(reachState);
                tA->setAcceptingState(inDictionary->states[inDictionary->currentState]->getIdd());
                tA->setDiagnosis(inDictionary->states[inDictionary->currentState]->diagnosis);
                *//* tA->setState(inDictionary->states[prevState]);*//*
                tA->setState(inDictionary->states[inDictionary->currentState]);
                tA->setBhState(inDictionary->states[inDictionary->currentState]->nfaStates2);
                newAb->push_back(tA);
                break;
                }
                else{
                tA->addReachState(reachState);
                tA->setAcceptingState(inDictionary->states[inDictionary->currentState]->getIdd());
                std::set<std::set<std::string>> jjj= inDictionary->states[inDictionary->currentState]->diagnosis;
                tA->setDiagnosis(inDictionary->states[inDictionary->currentState]->diagnosis);
                *//*tA->setState(inDictionary->states[prevState]);*//*
                tA->setState(inDictionary->states[inDictionary->currentState]);
                tA->setBhState(inDictionary->states[inDictionary->currentState]->nfaStates2);
                newAb->push_back(tA);
                break;
                }*/
            }
            for (auto a: *newAb) {
                int c = 1;
                c++;
            }
        }
        // aggiungere stato e transizione
        if (error) {
            auto network = NetworkIO::load(net);
            auto newTransition = std::make_shared<BehavioralTransition>("", transictionId, "", label);
            transictionId++;
            inDictionary->states[obsLabel[obsLabel.size() - 1]]->out.push_back(newTransition);
            std::vector<std::shared_ptr<BehavioralState>> nfaState = inDictionary->states[obsLabel[obsLabel.size() -
                                                                                                   1]]->nfaStates2;
            std::vector<std::shared_ptr<BehavioralTransition>> nuoveTransizioni;
            auto sss = inDictionary->getBehavioralTransitions();
            // std::vector<std::shared_ptr<BehavioralTransition>> *nuoveTransizioni = reinterpret_cast<std::vector<std::shared_ptr<BehavioralTransition>> *>(&sss);

            auto nuoviNfa = BSBuilderDFA::silentClosure(network, nfaState, label, inDictionary->bhState,
                                                        nuoveTransizioni, inDictionary);
            auto indiceDizionario = verificaEsistenzaStatoDic(inDictionary, nuoviNfa);

            if (indiceDizionario == -1) {
                auto dicState = std::make_shared<DictionaryState>(true, false);
                dicState->in.push_back(newTransition);
                dicState->setNfaState2(nuoviNfa);



                dicState->setDiangosis(getDiagnosisNewDicState(nuoviNfa));






                inDictionary->transitions.push_back(newTransition);
                inDictionary->states.push_back(dicState);


                inDictionary->currentState = dicState->getIdd();

                obsLabel.push_back(dicState->getIdd());

            } else {
                inDictionary->states[indiceDizionario]->in.push_back(newTransition);
                inDictionary->transitions.push_back(newTransition);
                inDictionary->currentState = indiceDizionario;
                obsLabel.push_back(indiceDizionario);
            }
        }
    }
    if (inDictionary->states[inDictionary->currentState]->final || true) {


        result = std::make_shared<std::set<std::set<std::string>>>(
                inDictionary->states[inDictionary->currentState]->diagnosis);
    }

    std::vector<std::string> messages = {
            "Diagnosis extraction completed.",
            "  Number of sets: " + std::to_string(result->size())
    };
    Logger::log(messages);

    if (maxExecTime) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - startingTime).count();
        *maxExecTime -= elapsed;
    }

    std::shared_ptr<std::vector<std::shared_ptr<transitionsInfo>>> tInfoDic(
            new std::vector<std::shared_ptr<transitionsInfo>>());
    for (auto dicState: inDictionary->states) {
        auto sta=dicState->nfaStates2;
        std::set<std::shared_ptr<NFAStateInterface>>  sss ;
        for(auto s: sta){
            sss.insert(s);
        }
        dicState->setNfaState(sss);

        for (auto out: dicState->getOutTransitions()) {
            std::shared_ptr<std::vector<std::shared_ptr<transitionsInfo>>> tInfoBh(
                    new std::vector<std::shared_ptr<transitionsInfo>>());
            for (auto dicStateDestination:inDictionary->states) {
                for (auto outD: dicStateDestination->getInTransitions()) {
                    if (out == outD) {
                        if (out->getObservabilityLabel() != "") {
                            auto tI = std::make_shared<transitionsInfo>(out->getId(), out->getObservabilityLabel(),
                                                                        dicState->getIdd());
                            tI->setDestination(dicStateDestination->getIdd());
                            std::vector<unsigned int> nfaS;
                            for (auto s: dicState->nfaStates2) {
                                nfaS.push_back(s->getIdd());
                            }
                            for (auto state: dicState->nfaStates2) {
                                for (auto tinf: *inDictionary->tInfo) {
                                    if (tinf->getStart() == state->getIdd()) {


                                        if (tinf->getLabel() == out->getObservabilityLabel()) {
                                            auto tIbh = std::make_shared<transitionsInfo>(tinf->getId(),
                                                                                          tinf->getLabel(),
                                                                                          tinf->getStart());
                                            tIbh->setDestination(tinf->getDestination());
                                            tInfoBh->push_back(tIbh);
                                        }


                                    }

                                }
                            }
                            tI->setTinfoD(tInfoBh);
                            tInfoDic->push_back(tI);

                        }

                    }
                }
            }
        }
    }

    std::shared_ptr<std::vector<std::shared_ptr<transitionsInfo>>>tInfo( new std::vector<std::shared_ptr<transitionsInfo>>());

    for (const auto &state : inDictionary->bhState){
        if (state->isStarting()) {
            auto startingState = std::shared_ptr<NFAStateInterface>(state);

        }
        for (auto outT: state->getOutTransitions()){
            auto tI = std::make_shared<transitionsInfo>(outT->getId(),outT->getObservabilityLabel(),state->getIdd());
            bool found=false;
            for ( auto &state2 : inDictionary->bhState ){
                for (auto inT: state2->getInTransitions()){
                    if (inT->getId()==outT->getId()){
                        tI->setDestination(state2->getIdd());
                        found=true;
                        break;
                    }
                }
            }
            if(found){
                tInfo->push_back(tI);

            }
        }
    }
    inDictionary->setTinfo(tInfo);
    inDictionary->setTinfoDic(tInfoDic);



    /*DFAConstructor::compareAbduction2(newAb,inDictionary,observation);
    int indice=0;
    for (auto &t : *newAb){
        std::cout << "\"Abduce\": "+ std::to_string(indice);
        bool firstSet = true;
        std::cout << "{\"Reach State\":[ ";
        if(t->reachStates.size()>0){
            for (auto re : t->reachStates[0]) {

                std::cout <<  std::to_string(re)+" ,";
            }
            std::cout << "]";
        }


        for (const auto di : t->diagn) {
            std::cout << "{";
            for(const auto d : di){


                std::cout << "\"" << d << ",\"";

            }
            std::cout << "}";


        }
        std::cout << "]\n";
        indice++;
    }*/
    auto strucD = std::make_shared<dictabd>(inDictionary, newAb);
    return strucD;
}

struct Node {
    std::shared_ptr<DictionaryState> statoDizionario;
    std::shared_ptr<DictionaryState> statoScenario;

    Node(const std::shared_ptr<DictionaryState> &statoDizionario, const std::shared_ptr<DictionaryState> &statoScenario) : statoDizionario(
            statoDizionario), statoScenario(statoScenario) {}
};

struct NodeTransition {
    std::shared_ptr<DictionaryState> statoPartenza;
    std::shared_ptr<DictionaryState> statoRaggiunto;
    std::shared_ptr<NFATransitionInterface> transition;

    NodeTransition(const std::shared_ptr<DictionaryState> &statoPartenza,const std::shared_ptr<DictionaryState> &statoRaggiunto, const std::shared_ptr<NFATransitionInterface> &transition) : statoPartenza(statoPartenza),statoRaggiunto(
            statoRaggiunto), transition(transition) {}
};


std::shared_ptr<DictionaryState> getCorrispondentDictionaryState( std::vector<std::shared_ptr<Node>> corrispondenza, std::shared_ptr<DictionaryState> statoS ){
    for(auto coppia: corrispondenza){
        if(coppia->statoScenario->getIdd()==statoS->getIdd()){
            return coppia->statoDizionario;
        }
    }
    return nullptr;
}

bool compareDifferentDictionaryState(std::shared_ptr<DictionaryState> stato1,std::shared_ptr<DictionaryState> stato2){
    auto nfa1= stato1->nfaStates2;
    auto nfa2= stato2->nfaStates2;
    if(nfa1.size()!=nfa2.size()){
        return  false;
    }
    else{
        for(auto n1 :nfa1){
            bool found=false;
            for(auto n2: nfa2){
                if(n1->getComponentsState()==n2->getComponentsState()){
                    found=true;
                    break;
                }
            }
            if (!found){
                return false;
            }

        }
    }
    return true;
}

bool foundCoppiaInque(std::queue<std::shared_ptr<NodeTransition>> toVisit,std::shared_ptr<NodeTransition> nod){
   for( auto iterator =toVisit.front() ;iterator != toVisit.back();){
       if((*iterator).statoRaggiunto==nod->statoRaggiunto and (*iterator).transition==nod->transition and nod->statoPartenza==(*iterator).statoPartenza){


            return true;
        }
       if(toVisit.front() ==toVisit.back() ){
           return false;
       }
    }
    return false;
}

struct NodeScen{
    std::shared_ptr<DictionaryState> stato;
    std::shared_ptr<SimpleState> statoScenario;


    NodeScen(const std::shared_ptr<DictionaryState> &stato,const std::shared_ptr<SimpleState> &statoScenario) : stato(stato),statoScenario(
            statoScenario) {}
};


std::shared_ptr<NodeScen> findCorrespondingNode(std::vector<std::shared_ptr<NodeScen>> associazione, std::shared_ptr<SimpleState> nodoScenario){
    for (auto e:associazione){
        if(e->statoScenario->id==nodoScenario->id){
            return e;
        }
    }
    return nullptr;
}
std::shared_ptr<Dictionary> DFAConstructor::extendDictionaryFromScenario(
        const std::shared_ptr<Dictionary> inDictionary, const std::shared_ptr<SimpleNetwork> scenario,
        std::shared_ptr<long> maxExecTime) {

    auto startingTime = std::chrono::high_resolution_clock::now();
    std::string message;
    std::shared_ptr<std::set<std::set<std::string>>> result(new std::set<std::set<std::string>>());

    Logger::section("Diagnosis Extraction");
    auto transictionId = inDictionary->transitions.size();
    //int lastIdDictionary = inDictionary->states.size() - 1;
    Logger::log("Input Observation:");

    std::shared_ptr<std::vector<std::shared_ptr<temporalAbduction>>> newAb(
            new std::vector<std::shared_ptr<temporalAbduction>>());
    inDictionary->setToInitialState();
    //associazione stato osservazione poszione i con stato del dizionario contentuto

    int starting = inDictionary->currentState;
   // bool start = true;
    bool error;


    std::shared_ptr<SimpleState> iniziale;
    for(auto a: scenario->states){
        if (a->final) {
            iniziale=a;
            break;
        }
        }


    std::vector<std::shared_ptr<SimpleState>> visitati;
    std::vector<std::shared_ptr<NodeScen>> associazione;

    std::queue<std::shared_ptr<SimpleState>> toVisit;
    associazione.push_back(std::make_shared<NodeScen>(inDictionary->states[starting], iniziale));
    toVisit.push(iniziale);
    while(!toVisit.empty()){
        auto stato=toVisit.front();
        std::vector<std::shared_ptr<SimpleState>> daVisitare;
        auto corresponding=findCorrespondingNode(associazione,stato);
        for(auto outS:stato->out ){
            scenario->currentState=stato->id;
            auto label = outS->label;
            inDictionary->currentState= corresponding->stato->id;
            error = true;
            for (auto &outTransition : inDictionary->states[inDictionary->currentState]->out) {
                if (outTransition->getObservabilityLabel() == label) {
                    scenario->currentState=stato->id;
                    inDictionary->currentState= corresponding->stato->id;
                    inDictionary->performTransition(outTransition);
                    error = false;
                    scenario->performTransition(label);
                    associazione.push_back(std::make_shared<NodeScen>(inDictionary->states[inDictionary->currentState],scenario->states[scenario->currentState]));

                    daVisitare.push_back(scenario->states[scenario->currentState]);
                    //obsLabel.push_back(inDictionary->currentState);

                    auto tA = std::make_shared<temporalAbduction>(inDictionary->states[inDictionary->currentState]);
                    std::vector<unsigned int> reachState;

                    for (auto &state: inDictionary->states[inDictionary->currentState]->nfaStates2) {
                        for (auto out :state->getInTransitions()) {
                            if (out->getObservabilityLabel() == label) {
                                reachState.push_back(state->getIdd());
                            }
                        }
                    }

                }

            }
            // aggiungere stato e transizione
            if (error) {
                auto network = NetworkIO::load("reteL3.json");
                auto newTransition = std::make_shared<BehavioralTransition>("", transictionId, "", label);
                transictionId++;
                inDictionary->states[corresponding->stato->id]->out.push_back(newTransition);
                std::vector<std::shared_ptr<BehavioralState>> nfaState = inDictionary->states[corresponding->stato->id]->nfaStates2;
                std::vector<std::shared_ptr<BehavioralTransition>> nuoveTransizioni;
                auto sss = inDictionary->getBehavioralTransitions();
                // std::vector<std::shared_ptr<BehavioralTransition>> *nuoveTransizioni = reinterpret_cast<std::vector<std::shared_ptr<BehavioralTransition>> *>(&sss);

                auto nuoviNfa = BSBuilderDFA::silentClosure(network, nfaState, label, inDictionary->bhState,
                                                            nuoveTransizioni, inDictionary);
                auto indiceDizionario = verificaEsistenzaStatoDic(inDictionary, nuoviNfa);

                if (indiceDizionario == -1) {
                    auto dicState = std::make_shared<DictionaryState>(true, false);
                    dicState->in.push_back(newTransition);
                    dicState->setNfaState2(nuoviNfa);
                    dicState->setDiangosis(getDiagnosisNewDicState(nuoviNfa));
                    inDictionary->transitions.push_back(newTransition);
                    inDictionary->states.push_back(dicState);
                    inDictionary->currentState = dicState->getIdd();

                    scenario->performTransition(label);
                    daVisitare.push_back(scenario->states[scenario->currentState]);
                    associazione.push_back(std::make_shared<NodeScen>(dicState,scenario->states[scenario->currentState]));


                } else {
                    inDictionary->states[indiceDizionario]->in.push_back(newTransition);
                    inDictionary->transitions.push_back(newTransition);
                    inDictionary->currentState = indiceDizionario;

                    scenario->performTransition(label);
                    daVisitare.push_back(scenario->states[scenario->currentState]);
                    associazione.push_back(std::make_shared<NodeScen>(inDictionary->states[inDictionary->currentState],scenario->states[scenario->currentState]));
                }
            }
            /*scenario->currentState=stato->id;*/




        }

        toVisit.pop();
        visitati.push_back(stato);
        for(auto el : daVisitare){
            bool found=false;
            for (auto e: visitati){
                if( e==el){
                    found=true;
                    break;
                }
            }
            if (!found) toVisit.push(el);
        }



    }
    if (inDictionary->states[inDictionary->currentState]->final || true) {


        result = std::make_shared<std::set<std::set<std::string>>>(
                inDictionary->states[inDictionary->currentState]->diagnosis);
    }

    std::vector<std::string> messages = {
            "Diagnosis extraction completed.",
            "  Number of sets: " + std::to_string(result->size())
    };
    Logger::log(messages);

    if (maxExecTime) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - startingTime).count();
        *maxExecTime -= elapsed;
    }

    std::shared_ptr<std::vector<std::shared_ptr<transitionsInfo>>> tInfoDic(
            new std::vector<std::shared_ptr<transitionsInfo>>());
    for (auto dicState: inDictionary->states) {
        auto sta=dicState->nfaStates2;
        std::set<std::shared_ptr<NFAStateInterface>>  sss ;
        for(auto s: sta){
            sss.insert(s);
        }
        dicState->setNfaState(sss);

        for (auto out: dicState->getOutTransitions()) {
            std::shared_ptr<std::vector<std::shared_ptr<transitionsInfo>>> tInfoBh(
                    new std::vector<std::shared_ptr<transitionsInfo>>());
            for (auto dicStateDestination:inDictionary->states) {
                for (auto outD: dicStateDestination->getInTransitions()) {
                    if (out == outD) {
                        if (out->getObservabilityLabel() != "") {
                            auto tI = std::make_shared<transitionsInfo>(out->getId(), out->getObservabilityLabel(),
                                                                        dicState->getIdd());
                            tI->setDestination(dicStateDestination->getIdd());
                            std::vector<unsigned int> nfaS;
                            for (auto s: dicState->nfaStates2) {
                                nfaS.push_back(s->getIdd());
                            }
                            for (auto state: dicState->nfaStates2) {
                                for (auto tinf: *inDictionary->tInfo) {
                                    if (tinf->getStart() == state->getIdd()) {


                                        if (tinf->getLabel() == out->getObservabilityLabel()) {
                                            auto tIbh = std::make_shared<transitionsInfo>(tinf->getId(),
                                                                                          tinf->getLabel(),
                                                                                          tinf->getStart());
                                            tIbh->setDestination(tinf->getDestination());
                                            tInfoBh->push_back(tIbh);
                                        }


                                    }

                                }
                            }
                            tI->setTinfoD(tInfoBh);
                            tInfoDic->push_back(tI);

                        }

                    }
                }
            }
        }
    }

    std::shared_ptr<std::vector<std::shared_ptr<transitionsInfo>>>tInfo( new std::vector<std::shared_ptr<transitionsInfo>>());

    for (const auto &state : inDictionary->bhState){
        if (state->isStarting()) {
            auto startingState = std::shared_ptr<NFAStateInterface>(state);

        }
        for (auto outT: state->getOutTransitions()){
            auto tI = std::make_shared<transitionsInfo>(outT->getId(),outT->getObservabilityLabel(),state->getIdd());
            bool found=false;
            for ( auto &state2 : inDictionary->bhState ){
                for (auto inT: state2->getInTransitions()){
                    if (inT->getId()==outT->getId()){
                        tI->setDestination(state2->getIdd());
                        found=true;
                        break;
                    }
                }
            }
            if(found){
                tInfo->push_back(tI);

            }
        }
    }
    inDictionary->setTinfo(tInfo);
    inDictionary->setTinfoDic(tInfoDic);







    /*std::string message;
    std::shared_ptr<std::set<std::set<std::string>>> result(new std::set<std::set<std::string>>());

    Logger::section("Diagnosis Extraction");
    auto transictionId = inDictionary->transitions.size();
    int lastIdDictionary = inDictionary->states.size() - 1;
    Logger::log("Input Observation:");

*//*
std::shared_ptr<std::vector<std::shared_ptr<temporalAbduction>>> newAb;
*//*
    std::shared_ptr<std::vector<std::shared_ptr<temporalAbduction>>> newAb(
            new std::vector<std::shared_ptr<temporalAbduction>>());

    scenario->setToInitialState();
    inDictionary->setToInitialState();
    std::vector<std::shared_ptr<Node>> corrispondenza;
    auto initialStateScenario = scenario->getInitialState();

    auto initialStateDizionario = inDictionary->getInitialState();
    corrispondenza.push_back(std::make_shared<Node>(initialStateDizionario, initialStateScenario));
    std::queue<std::shared_ptr<NodeTransition>> toVisit;
    std::queue<std::shared_ptr<NodeTransition>> visited;
    auto currentScenarioState=scenario->currentState;
    for(auto t: initialStateScenario->out){
        scenario->performTransition(t);

        toVisit.push(std::make_shared<NodeTransition>(initialStateDizionario,scenario->states[scenario->currentState], t));
        scenario->currentState=currentScenarioState;
    }

    while(!toVisit.empty() or visited.size()==(scenario->states.size()+inDictionary->states.size())){


        auto nodoTransizione=toVisit.front();
        auto PrecStateScenario=nodoTransizione->statoPartenza;
        auto statoDiz=getCorrispondentDictionaryState(corrispondenza,PrecStateScenario);
        auto transizioneScenario=nodoTransizione->transition;
        auto currentStateScenario= nodoTransizione->statoRaggiunto;
        bool found=false;
        for(auto tD: statoDiz->out){
            if(tD->getObservabilityLabel()==transizioneScenario->getObservabilityLabel()){
                inDictionary->currentState=statoDiz->id;
                inDictionary->performTransition(tD);
                corrispondenza.push_back(std::make_shared<Node>(inDictionary->states[inDictionary->currentState], nodoTransizione->statoRaggiunto));
                found=true;
                break;

            }
        }
        if(found){
            scenario->currentState=currentStateScenario->id;
            for(auto t: currentStateScenario->out){
                scenario->performTransition(t);
                auto cpp=std::make_shared<NodeTransition>(PrecStateScenario,scenario->states[scenario->currentState], t);
                if(! foundCoppiaInque(toVisit,cpp)){
                    if(!visited.empty()){
                        if(! foundCoppiaInque(visited,cpp)) {
                            toVisit.push(std::make_shared<NodeTransition>(PrecStateScenario,scenario->states[scenario->currentState], t));
                        }

                    }
                    else{
                        toVisit.push(std::make_shared<NodeTransition>(PrecStateScenario,scenario->states[scenario->currentState], t));
                    }

                }

                scenario->currentState=currentStateScenario->id;
            }

        }
        else{
            //verifica se esiste gia quello stato prima
            bool trovato= false;
           for(auto d: inDictionary->states){
               trovato=compareDifferentDictionaryState( d,currentStateScenario);
               if (trovato){
                   auto newTransition =  std::make_shared<BehavioralTransition>("", "", transizioneScenario->getObservabilityLabel());
                   inDictionary->transitions.push_back(newTransition);
                   d->in.push_back(newTransition);
                   statoDiz->out.push_back(newTransition);

                   break;
               }
           }
           if(!trovato){
               //se non esiste lo creo
               auto newDicState= std::make_shared<DictionaryState>(currentStateScenario->isFinal(), false);

               auto newTransition =  std::make_shared<BehavioralTransition>("", "", transizioneScenario->getObservabilityLabel());
               newDicState->in.push_back(newTransition);
               statoDiz->out.push_back(newTransition);
               inDictionary->states.push_back(newDicState);
               inDictionary->transitions.push_back(newTransition);
               scenario->currentState=currentStateScenario->id;

               corrispondenza.push_back(std::make_shared<Node>(newDicState, currentStateScenario));
               for(auto t: currentStateScenario->out){
                   scenario->performTransition(t);
                   auto cpp=std::make_shared<NodeTransition>(PrecStateScenario,scenario->states[scenario->currentState], t);
                   if(! foundCoppiaInque(toVisit,cpp)){
                       if(!visited.empty()){
                           if(! foundCoppiaInque(visited,cpp)) {
                               toVisit.push(std::make_shared<NodeTransition>(PrecStateScenario,scenario->states[scenario->currentState], t));
                           }

                       }
                       else{
                           toVisit.push(std::make_shared<NodeTransition>(PrecStateScenario,scenario->states[scenario->currentState], t));
                       }
                   }


                   scenario->currentState=currentStateScenario->id;
               }
           }
           }

        visited.push(nodoTransizione);
        auto a=toVisit.size();
        toVisit.pop();
        auto b=toVisit.size();
        auto c=1;


    }*/




    return inDictionary;
}

void findTransitions(const std::vector<std::shared_ptr<NFATransitionInterface>> &transitions,
                            std::vector<std::shared_ptr<SimpleTransition>> &out, std::vector<std::shared_ptr<SimpleTransition>> &tot) {
    for (auto t : transitions) {
        std::shared_ptr<SimpleTransition> found = nullptr;
        for (const auto &transition : tot)
            if (transition->id == t->getId()) {
                found = transition;
                break;
            }
        if (found == nullptr) throw std::invalid_argument("Input JSON attempts to use an undefined transition");
        out.push_back(found);
    }
}


std::shared_ptr<SimpleNetwork> DFAConstructor::convertScenarioToSimpleNetwork(const std::shared_ptr<Dictionary> scenario){

    std::vector<std::shared_ptr<SimpleTransition>> transitions;
    std::vector<std::shared_ptr<SimpleState>> states;

    for (auto transition: scenario->transitions) {

        auto id=transition->getId();
        auto label=transition->getObservabilityLabel();
        transitions.push_back(std::make_shared<SimpleTransition>(id, label));
    }




    for (auto state : scenario->states) {
        bool starting = state->starting;
        bool final = state->final;
        unsigned int id = state->id;
        std::vector<std::shared_ptr<SimpleTransition>> in, out;
        auto inS =state->in;
        auto outS =state->out;
        findTransitions(inS, in, transitions);
        findTransitions(outS,out,transitions);

        states.push_back(std::make_shared<SimpleState>(id, in, out, final, starting));
    }

    return std::make_shared<SimpleNetwork>(states, transitions);


}



