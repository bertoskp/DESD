#include "DictionaryMerge.h"
#include "DFAConstructor.h"


/*
 * Struttura dati per salvare i nodi gia' visitati.
 * oldState e dictionary servono per risalire al vecchio stato, mentre
 * newState punta allo stato che andra' nel nuovo dizionario
 */
struct Node {
    std::shared_ptr<DictionaryState> newState;
    std::shared_ptr<DictionaryState> oldState;

    Node(const std::shared_ptr<DictionaryState> &newState, const std::shared_ptr<DictionaryState> &oldState) : newState(
        newState), oldState(oldState) {}
};

//TODO: aggiustare controlli sull'esecuzione
std::shared_ptr<Dictionary> DictionaryMerge::merge(std::vector<std::shared_ptr<Dictionary>> dictionaries,
                                                   std::shared_ptr<long> maxExecTime) {

    auto startingTime = std::chrono::high_resolution_clock::now();
    std::string message;
    std::vector<std::shared_ptr<NFATransitionInterface>> transitions;
    std::vector<std::shared_ptr<DictionaryState>> states;

    Logger::section("Dictionary Merge");

    DictionaryState::nextId = 0;
    BehavioralTransition::nextId = 0;

    std::shared_ptr<DictionaryState> parent = std::make_shared<DictionaryState>(false, true);
    states.push_back(parent);

    for (const auto &dictionary : dictionaries) {

        std::queue<std::shared_ptr<Node>> toVisit;
        std::vector<std::shared_ptr<Node>> visitedNodes;

        // Trovo il nodo iniziale del dizionario, e lo copio in un nuovo stato
        auto initialState = dictionary->getInitialState();
        auto newInitialState = std::make_shared<DictionaryState>(initialState->isFinal(), false);
        newInitialState->diagnosis = initialState->diagnosis;

        // Dato che e' il nodo iniziale, aggiungo la eps-transizione dal parent
        auto newTransitionFromParent = std::make_shared<BehavioralTransition>("", "", "");
        parent->out.push_back(newTransitionFromParent);
        newInitialState->in.push_back(newTransitionFromParent);
        transitions.push_back(newTransitionFromParent);

        // Salvo questo nodo come il primo da visitare
        toVisit.push(std::make_shared<Node>(newInitialState, initialState));


        while(!toVisit.empty()) {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
            if(maxExecTime && elapsed > *maxExecTime){
                std::vector<std::string> errorMessages = {
                    "Maximum execution time exceeded during dictionary merge! (" + std::to_string(elapsed) + ")",
                    "  States in processing queue: " + std::to_string(toVisit.size())
                };
                Logger::err(errorMessages);
                throw std::runtime_error("execution time");
            }
            // Estraggo un nodo dalla coda
            auto currentNode = toVisit.front();
            toVisit.pop();
            visitedNodes.push_back(currentNode);

            // Per ogni sua transizione in output, visito i nodi e li metto in toVisit se sono nuovi
            for (const auto &outTransition : currentNode->oldState->out) {
                // Cerco lo stato con questa transizione in ingresso
                std::shared_ptr<DictionaryState> found = nullptr;
                for (const auto &s : dictionary->states)
                    for (const auto &t : s->in)
                        if (t == outTransition) {
                            found = s;
                            break;
                        }
                if (found == nullptr) throw std::logic_error("Found a transition without a target state");

                auto newTransition =  std::make_shared<BehavioralTransition>("", "", outTransition->getObservabilityLabel());
                transitions.push_back(newTransition);
                currentNode->newState->out.push_back(newTransition);

                std::shared_ptr<DictionaryState> newFound = nullptr;
                for (const auto &n : visitedNodes)
                    if (n->oldState == found) {
                        newFound = n->newState;
                        break;
                    }

                if (newFound == nullptr) {
                    // Lo stato non e' ancora stato visitato
                    newFound = std::make_shared<DictionaryState>(found->isFinal(), false);
                    newFound->diagnosis = found->diagnosis;
                    toVisit.push(std::make_shared<Node>(newFound, found));
                }
                // In ogni caso, aggiungi la transizione
                newFound->in.push_back(newTransition);
            }
        }

        // Una volta visitati tutti i nodi, aggiungo a states quelli trovati
        for (const auto &n : visitedNodes) states.push_back(n->newState);

    }

    auto merged = std::make_shared<Dictionary>(states, transitions);

    auto determinized = DFAConstructor::constructDictionary(merged, false,maxExecTime);

    std::vector<std::string> messages = {
            "Dictionary merge completed.",
            "  States: " + std::to_string(states.size()),
            "  Transitions: " + std::to_string(transitions.size())
    };
    Logger::log(messages);

    if (maxExecTime) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();
        *maxExecTime -= elapsed;
    }
    return determinized;
}

