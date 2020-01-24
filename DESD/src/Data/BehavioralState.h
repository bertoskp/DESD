//
// Created by Diego Lorenzi on 24/07/18.
//

#ifndef AUTOMATAMANAGER_BEHAVIORALSTATE_H
#define AUTOMATAMANAGER_BEHAVIORALSTATE_H

#include "../stdIncludes.h"
#include "BehavioralTransition.h"
#include "NFAStateInterface.h"

class BehavioralState : public NFAStateInterface {
public:
    unsigned int id;
    static unsigned int nextId;
    std::map<std::string, std::string> componentsState;
    std::map<std::string, std::string> linksState;
    std::vector<std::shared_ptr<BehavioralTransition>> in;
    std::vector<std::shared_ptr<BehavioralTransition>> out;
    std::set<std::string> relevancyLabels;
    bool starting;
    int observationState, bondState;
    bool final; // Dynamic!

    BehavioralState();

    BehavioralState(std::map<std::string, std::string>& _componentsState,
                    std::map<std::string, std::string> &_linksState, bool _starting, bool _final);

    BehavioralState(std::map<std::string, std::string> &_componentsState,
                    std::map<std::string, std::string> &_linksState, bool _starting, bool _final,
                    std::vector<std::shared_ptr<BehavioralTransition>> &_in,
                    std::vector<std::shared_ptr<BehavioralTransition>> &_out);

    BehavioralState(std::map<std::string, std::string>& _componentsState,
                    std::map<std::string, std::string> &_linksState, bool _starting, bool _final,
                    std::set<std::string>& _relevancyLabels);

    BehavioralState(std::map<std::string, std::string>& _componentsState,
                    std::map<std::string, std::string> &_linksState, bool _starting, bool _final,
                    std::vector<std::shared_ptr<BehavioralTransition>> &_in,
                    std::vector<std::shared_ptr<BehavioralTransition>> &_out,
                    std::set<std::string>& _relevancyLabels);

    BehavioralState(std::map<std::string, std::string>& _componentsState,
                    std::map<std::string, std::string> &_linksState, bool _starting, bool _final,
                    std::vector<std::shared_ptr<BehavioralTransition>> &_in,
                    std::vector<std::shared_ptr<BehavioralTransition>> &_out,
                    std::set<std::string>& _relevancyLabels,unsigned int _id);


    BehavioralState(std::map<std::string, std::string>& _componentsState,
                    std::map<std::string, std::string> &_linksState, bool _starting, bool _final,
                    std::vector<std::shared_ptr<BehavioralTransition>> &_in,
                    std::vector<std::shared_ptr<BehavioralTransition>> &_out,
                    unsigned int _id);

    BehavioralState(std::map<std::string, std::string>& _componentsState,
                    std::map<std::string, std::string> &_linksState, bool _starting, bool _final,unsigned int _id);

    bool operator==(BehavioralState const &lhs) {
        bool areTheyEqual = lhs.componentsState == componentsState &&
                            lhs.linksState == linksState &&
                            lhs.starting == starting &&
                            lhs.relevancyLabels == relevancyLabels;
        if (lhs.observationState != -1 || observationState != -1) areTheyEqual = areTheyEqual && observationState ==
                                                                                                 lhs.observationState;
        return areTheyEqual;
    }

    bool isStarting() override;

    bool isFinal() override;
    
    void setId(unsigned int _id);

    std::vector<std::shared_ptr<NFATransitionInterface>> getInTransitions() override;

    std::vector<std::shared_ptr<NFATransitionInterface>> getOutTransitions() override;
    std::vector<std::shared_ptr<BehavioralTransition>> getOutTransitions2();
    std::set<std::set<std::string>> getDiagnosis() override;

    std::string toString() override;

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
    void resetId();
    void addInTransition(std::shared_ptr<BehavioralTransition> transizioneOsservabile)override;;
    void addOutTransition(std::shared_ptr<BehavioralTransition> transizioneOsservabile)override;;
    void removeInTransition(std::shared_ptr<BehavioralTransition> transizione) override;;

};


#endif //AUTOMATAMANAGER_BEHAVIORALSTATE_H
