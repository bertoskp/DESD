//
// Created by Diego Lorenzi on 27/07/18.
//

#ifndef AUTOMATAMANAGER_SUBSETCONSTRUCTION_H
#define AUTOMATAMANAGER_SUBSETCONSTRUCTION_H

#endif //AUTOMATAMANAGER_SUBSETCONSTRUCTION_H

#include "../Data/BehavioralSpace.h"
#include "../Data/Dictionary.h"
#include "../Data/NFASpaceInterface.h"
#include "../manipulators/BSBuilderDFA.h"
#include "../Data/SimpleNetwork.h"

#include <iostream>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <bitset>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <queue>
#include <set>
#include <memory>
#include "../Utils/Logger.h"

struct DFAState{
    std::set<std::shared_ptr<NFAStateInterface>> nfaStates;
    std::vector<std::shared_ptr<NFATransitionInterface>> in;
    std::vector<std::shared_ptr<NFATransitionInterface>> out;
   /* std::vector<std::vector<std::shared_ptr<NFAStateInterface>>> pairs;*/
   std::vector<std::vector<unsigned int>> pairs;

    explicit DFAState(std::set<std::shared_ptr<NFAStateInterface>> &_nfaStates) {
        nfaStates = _nfaStates;
    }

    void toString() {
        for (const auto &s : nfaStates)
            s->toString();
    }

    std::set<std::set<std::string>> getDiagnosis() {
        auto out = std::set<std::set<std::string>>();
        for (const auto &nfaState : nfaStates)
            /*if (nfaState->isFinal())*/
                for (auto &d : nfaState->getDiagnosis())
                    out.insert(d);
        return out;
    }

    /*std::vector<std::vector<std::shared_ptr<NFAStateInterface>>> getPairs() {
        return pairs;
    }

    void addPair(std::vector<std::shared_ptr<NFAStateInterface>> pair) {
        pairs.push_back(pair);


    }*/

    std::vector<std::vector<unsigned int>> getPairs() {
        return pairs;
    }

    void addPair(std::vector<unsigned int> pair) {
        pairs.push_back(pair);


    }
};

struct temporalAbduction{
    std::shared_ptr<DFAState> state;
    std::shared_ptr<DictionaryState> st;
    std::vector<unsigned int> reachStates;
  std::vector<unsigned int> in;
    std::vector<unsigned int> out;
    unsigned int acceptingState;
    std::string diagnosisString;
    std::vector<std::shared_ptr<BehavioralState>> bhState;
    std::set<std::set<std::string>> diagnosis;
    std::vector<std::vector<std::string>> diagn;


    explicit temporalAbduction(std::shared_ptr<DictionaryState> &_st) {
      st =_st;
    }

    void addReachState(std::vector<unsigned int> reachState) {
        reachStates=reachState;
    }
    void addInState(std::vector<unsigned int> reachState) {
        in=reachState;
    }

    void setReach(std::vector<unsigned int> eachState) {
        reachStates= eachState;
    }
    void addOutState(std::vector<unsigned int> reachState) {
        out=reachState;
    }
    void setAcceptingState(unsigned int _acceptingState) {
        acceptingState=_acceptingState;
    }
    unsigned int getAcceptingState(){
        return acceptingState;
    }
    void setState(  std::shared_ptr<DictionaryState>  &_st){
        st=_st;
    }

    void setBhState(    std::vector<std::shared_ptr<BehavioralState>> &_bhState  ){
        bhState=_bhState;

    }

    std::vector<std::shared_ptr<BehavioralState>>  getBhState(){
        return bhState;
    }

   std::vector<unsigned int> getReachStates(){
        return reachStates;
    }
    std::vector<unsigned int> getInState(){
        return in;
    }

    std::vector<unsigned int> getOutState(){
        return out;
    }
    std::set<std::set<std::string>> getDiagnosis2() {
        return diagnosis;
    }
    std::string getDiagnosisString() {
        return diagnosisString;
    }

    std::set<std::set<std::string>> getDiagnosis() {
        auto out = std::set<std::set<std::string>>();
       /* for (const auto &nfaState : state->nfaStates)*/
        for (const auto &nfaState : st->nfaStates2)
            /*if (nfaState->isFinal())*/
            for (auto &d : nfaState->getDiagnosis())
                out.insert(d);
        return out;
    }

    void setDiagnosis(std::set<std::set<std::string>> &_diagnosis){
        diagnosis=_diagnosis;
    }
    void setDiagnosisString(std::string &_diagnosis){
        diagnosisString=_diagnosis;
    }
    void setDiagn(std::vector<std::vector<std::string>> &_diagn){
        diagn=_diagn;
    }
};

struct dictabd{
    std::shared_ptr<Dictionary> inDictionary;
    std::shared_ptr<std::vector<std::shared_ptr<temporalAbduction>>> newAb;


    explicit dictabd(std::shared_ptr<Dictionary> _inDictionary,
    std::shared_ptr<std::vector<std::shared_ptr<temporalAbduction>>> _newAb) {
        inDictionary= _inDictionary;
        newAb=_newAb;
    }

};

class DFAConstructor{
public:
    static std::shared_ptr<std::vector<std::shared_ptr<DFAState>>> subsetConstruction(
        std::shared_ptr<NFASpaceInterface> inputSpace,bool online, std::shared_ptr<long> maxExecTime);
    static std::shared_ptr<std::vector<std::shared_ptr<DFAState>>> subsetConstructionPrefix(
            std::shared_ptr<NFASpaceInterface> inputSpace, unsigned int prefix, std::shared_ptr<long> maxExecTime);

    static std::shared_ptr<Dictionary> constructDictionary(std::shared_ptr<NFASpaceInterface> inputSpace,
                                                           bool online,std::shared_ptr<long> maxExecTime);


    static std::shared_ptr<Dictionary> constructDictionaryPrefix(unsigned  int prefix, std::shared_ptr<NFASpaceInterface> inputSpace,
                                                                         std::shared_ptr<long> maxExecTime);

    static std::shared_ptr<SimpleNetwork> constructObservation(std::shared_ptr<NFASpaceInterface> inputSpace,bool online,
                                                               std::shared_ptr<long> maxExecTime);

    static void epsilonClosure(NFASpaceInterface &space, std::shared_ptr<NFAStateInterface> state,
                               std::set<std::shared_ptr<NFAStateInterface>> &closure);
   static void  relevanClosure(std::set<std::shared_ptr<NFAStateInterface>> closurep,std::set<std::shared_ptr<NFAStateInterface>> &closure);


    static void epsilonClosure(NFASpaceInterface &space, std::set<std::shared_ptr<NFAStateInterface>> &states,
                               std::set<std::shared_ptr<NFAStateInterface>> &closure);

    static void move(NFASpaceInterface &space, std::set<std::shared_ptr<NFAStateInterface>> &fromStates,
                     std::set<std::shared_ptr<NFAStateInterface>> &toStates, std::string &symbol);

    static void createPair(std::shared_ptr<DFAState> nfaStates);

    static void move2(std::shared_ptr<NFAStateInterface> ts, std::shared_ptr<NFAStateInterface> tf,
                      std::shared_ptr<DFAState> state,
                      std::vector<std::vector<std::shared_ptr<NFAStateInterface>>> pairs);

    static void move3(std::shared_ptr<NFAStateInterface> ts, std::shared_ptr<NFAStateInterface> tf,
                      std::shared_ptr<DFAState> state,
                      std::vector<std::vector<unsigned int >> pairs);

    static std::shared_ptr<std::set<std::set<std::string>>> diagnoseFromObservation(
        const std::shared_ptr<Dictionary> inDictionary,
        std::vector<std::string> &observation, std::shared_ptr<long> maxExecTime);

    static std::vector<std::shared_ptr<temporalAbduction>> diagnoseFromAbduce(
            const std::shared_ptr<Dictionary> inDictionary,
            std::vector<std::string> &observation, std::shared_ptr<long> maxExecTime);

    static std::shared_ptr <dictabd> extendDictionaryFromObs(
            const std::shared_ptr<Dictionary> inDictionary,
            std::vector<std::string> &observation, std::shared_ptr<long> maxExecTime);



    static std::shared_ptr<Dictionary> extendDictionaryFromScenario(
            const std::shared_ptr<Dictionary> inDictionary, const std::shared_ptr<SimpleNetwork> scenario,
            std::shared_ptr<long> maxExecTime);

    static void  compareAbduction(std::shared_ptr<std::vector<std::shared_ptr<temporalAbduction>>> newAb,const std::shared_ptr<Dictionary> inDictionary);

    static void  compareAbduction2(std::shared_ptr<std::vector<std::shared_ptr<temporalAbduction>>> newAb,const std::shared_ptr<Dictionary> inDictionary,std::vector<std::string> &observation);

    static bool compareDiagnosis(std::vector<std::string> diagnosis, std::vector< std::vector<std::string>> diagnosisNext);


    static std::vector<int> verifyReach(std::vector<std::shared_ptr<temporalAbduction>> current,std::vector<std::shared_ptr<temporalAbduction>> next);

    static std::vector<std::shared_ptr<NFATransitionInterface>> foundBehavioralTransitionDictionary( std::vector<std::shared_ptr<NFAStateInterface>>  bhStates) ;
    static std::vector<std::shared_ptr<NFAStateInterface>>  foundNfaStateDictionary(std::shared_ptr<Dictionary> dictionary);

    static std::shared_ptr<SimpleNetwork> convertScenarioToSimpleNetwork( const std::shared_ptr<Dictionary> scenario);
    //static int verificaEsistenzaStatoDic(std::shared_ptr<Dictionary> inDictionary ,std::vector<std::shared_ptr<BehavioralState>> statiNfa );
};

