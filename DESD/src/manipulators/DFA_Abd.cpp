//
// Created by Nicola Bertoglio on 2019-06-10.
//
/*

std::shared_ptr<std::vector<std::shared_ptr<temporalAbduction>>> DFAConstructor::diagnoseFromAbduce(
        const std::shared_ptr<Dictionary> inDictionary, std::vector<std::string> &observation,
        std::shared_ptr<long> maxExecTime) {
    auto startingTime = std::chrono::high_resolution_clock::now();
    std::string message;
    std::shared_ptr<std::set<std::set<std::string>>> result(new std::set<std::set<std::string>>());

    Logger::section("Diagnosis Extraction");

    Logger::log("Input Observation:");
    for (int i = 0; i < observation.size(); i++) Logger::log("  " + std::to_string(i) + ". " + observation[i]);
*/
/*
    std::shared_ptr<std::vector<std::shared_ptr<temporalAbduction>>> newAb;
*//*

    std::shared_ptr<std::vector<std::shared_ptr<temporalAbduction>>> newAb(new std::vector<std::shared_ptr<temporalAbduction>>());


    inDictionary->setToInitialState();
    int starting= inDictionary->currentState;
    bool start= true;
    bool error;
    for (auto &label : observation) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - startingTime).count();
        if (maxExecTime && elapsed > *maxExecTime) {
            std::vector<std::string> messages = {
                    "Maximum execution time exceeded during diagnosis extraction! (" + std::to_string(elapsed) + ")",
                    "  Current task: observation label " + label
            };
            Logger::err(messages);
            throw std::runtime_error("execution time");
        }
        error = true;
        for (auto &outTransition : inDictionary->states[inDictionary->currentState]->out) {
            if (outTransition->getObservabilityLabel() == label) {
                int const prevState = inDictionary->currentState;
                inDictionary->performTransition(outTransition);
                error = false;

                auto tA = std::make_shared<temporalAbduction>(inDictionary->states[inDictionary->currentState]);
                std::vector<unsigned int> reachState;

                for (auto &state: inDictionary->states[inDictionary->currentState]->nfaStates2) {
                    for (auto out :state->getInTransitions()) {
                        if (out->getObservabilityLabel() == label) {
                            reachState.push_back(state->getIdd());
                        }
                    }

                }
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
                    */
/* tA->setState(inDictionary->states[prevState]);*//*

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
                    */
/*tA->setState(inDictionary->states[prevState]);*//*

                    tA->setState(inDictionary->states[inDictionary->currentState]);
                    tA->setBhState(inDictionary->states[inDictionary->currentState]->nfaStates2);
                    newAb->push_back(tA);
                    break;
                }
            }
            for (auto a: *newAb){
                int c=1;
                c++;
            }
        }
        if (error) {
            if (maxExecTime) {
                elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::high_resolution_clock::now() - startingTime).count();
                *maxExecTime -= elapsed;
            }
            throw std::invalid_argument("Not a linear observation (doesn't define a valid route)");
        }


    }
    if (inDictionary->states[inDictionary->currentState]->final || true) {




        */
/*void setAcceptingState(unsigned int _acceptingState) {
            acceptingState=_acceptingState;
        }
        unsigned int getAcceptingState(){
            return acceptingState;
        }*//*

        result = std::make_shared<std::set<std::set<std::string>>>(
                inDictionary->states[inDictionary->currentState]->diagnosis);
    } else {
        if (maxExecTime) {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::high_resolution_clock::now() - startingTime).count();
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
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - startingTime).count();
        *maxExecTime -= elapsed;
    }

    std::shared_ptr<std::vector<std::shared_ptr<transitionsInfo>>>tInfoDic( new std::vector<std::shared_ptr<transitionsInfo>>());
    for(auto dicState: inDictionary->states){

        for(auto  out: dicState->getOutTransitions()){
            std::shared_ptr<std::vector<std::shared_ptr<transitionsInfo>>>tInfoBh( new std::vector<std::shared_ptr<transitionsInfo>>());
            for ( auto dicStateDestination:inDictionary->states){
                for(auto outD: dicStateDestination->getInTransitions()){
                    if (out==outD){
                        if(out->getObservabilityLabel()!=""){
                            auto tI = std::make_shared<transitionsInfo>(out->getId(),out->getObservabilityLabel(),dicState->getIdd());
                            tI->setDestination(dicStateDestination->getIdd());
                            std::vector<unsigned  int> nfaS;
                            for(auto s: dicState->nfaStates2) {
                                nfaS.push_back(s->getIdd());
                            }
                            for(auto state: dicState->nfaStates2){
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
    inDictionary->setTinfoDic(tInfoDic);

    DFAConstructor::compareAbduction2(newAb,inDictionary,observation);
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
    }

    return newAb;
}
*/
