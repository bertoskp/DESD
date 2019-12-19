//
// Created by Diego Lorenzi on 27/07/18.
//

#ifndef AUTOMATAMANAGER_DFASTATE_H
#define AUTOMATAMANAGER_DFASTATE_H

#include "../stdIncludes.h"
#include "BehavioralTransition.h"
#include "NFAStateInterface.h"
#include "BehavioralState.h"

class DictionaryState : public NFAStateInterface {
public:
    unsigned int id;
    static unsigned int nextId;
    bool final;
    bool starting;
    std::vector<std::shared_ptr<NFATransitionInterface>> in;
    std::vector<std::shared_ptr<NFATransitionInterface>> out;
    std::set<std::set<std::string>> diagnosis;
    std::set<std::shared_ptr<NFAStateInterface>> nfaStates;
    std::vector<std::shared_ptr<BehavioralState>> nfaStates2;
   /* std::vector<std::vector<std::shared_ptr<NFAStateInterface>>> pairs;*/
   std::vector<std::vector<unsigned int>> pairs;

    DictionaryState();

    DictionaryState(bool _final, bool _starting);

    DictionaryState(bool _isFinal, bool _starting, const std::vector<std::shared_ptr<NFATransitionInterface>>& _in,
                    const std::vector<std::shared_ptr<NFATransitionInterface>>& _out,
                    const std::set<std::set<std::string>>& diagnosis,
                    /*const std::vector<std::vector<std::shared_ptr<NFAStateInterface>>>& _pairs*/
                    const std::vector<std::vector<unsigned int>> &_pairs);

    DictionaryState(bool _isFinal, bool _starting,
                    const std::vector<std::shared_ptr<NFATransitionInterface>>& _in,
                    const std::vector<std::shared_ptr<NFATransitionInterface>>& _out,
                    const std::set<std::set<std::string>>& diagnosis,
                   /* const std::vector<std::vector<std::shared_ptr<NFAStateInterface>>>& _pairs*/
                    const std::vector<std::vector<unsigned int>> &_pairs, unsigned int _id);

    std::string toString() override;

    void setNfaState(std::set<std::shared_ptr<NFAStateInterface>> &_nfaStates);
    void setNfaState2(std::vector<std::shared_ptr<BehavioralState>> &_nfaStates);

    bool isStarting() override;

    bool isFinal() override;


    void setDiangosis(std::set<std::set<std::string>> dia);


    std::vector<std::shared_ptr<NFATransitionInterface>> getInTransitions() override;

    std::vector<std::shared_ptr<NFATransitionInterface>> getOutTransitions() override;
    //std::vector<std::shared_ptr<BehavioralTransition>> getOutTransitions2() override;


    std::set<std::set<std::string>> getDiagnosis() override;
    int getIdd() override;
    int getNextId() override;
    std::map<std::string, std::string> getComponentsState()override;
    std::map<std::string, std::string> getLinksState()override;;
    std::vector<std::shared_ptr<BehavioralTransition>> GetIn()override;;
    std::vector<std::shared_ptr<BehavioralTransition>> GetOut()override;;
    std::set<std::string> getRelevancyLabels()override;;
    bool getStarting()override;
    int getObservationState()override;;
    int getBondState()override;;
    bool getFinal()override;; // Dynamic!
    void addInTransition(std::shared_ptr<BehavioralTransition> transizioneOsservabile)override;;
    void addOutTransition(std::shared_ptr<BehavioralTransition> transizioneOsservabile)override;;

    void removeInTransition(std::shared_ptr<BehavioralTransition> transizione) override;;





};





#endif //AUTOMATAMANAGER_DFASTATE_H
