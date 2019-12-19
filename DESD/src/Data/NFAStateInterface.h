//
// Created by alessandro on 8/1/18.
//

#ifndef AUTOMATAMANAGER_NFASTATEINTERFACE_H
#define AUTOMATAMANAGER_NFASTATEINTERFACE_H

#include "../stdIncludes.h"
#include "NFATransitionInterface.h"

class NFAStateInterface {
public:
    virtual bool isStarting() = 0;

    virtual bool isFinal() = 0;

    virtual std::set<std::set<std::string>> getDiagnosis() = 0;

    virtual std::vector<std::shared_ptr<NFATransitionInterface>> getInTransitions() = 0;

    virtual std::vector<std::shared_ptr<NFATransitionInterface>> getOutTransitions() = 0;
    //virtual std::vector<std::shared_ptr<BehavioralTransition>> getOutTransitions2() = 0;

    virtual std::string toString() = 0;

    virtual int getIdd()=0;

    /*virtual std::set<std::set<std::string>> getObs() = 0*/;

    virtual int getNextId()=0;
    virtual std::map<std::string, std::string> getComponentsState()=0;

    virtual std::map<std::string, std::string> getLinksState()=0;
    virtual std::vector<std::shared_ptr<BehavioralTransition>> GetIn()=0;
    virtual std::vector<std::shared_ptr<BehavioralTransition>> GetOut()=0;
    virtual std::set<std::string> getRelevancyLabels()=0;
    virtual bool getStarting()=0;
    virtual int getObservationState()=0;
    virtual int getBondState()=0;
    virtual bool getFinal()=0; // Dynamic!
    virtual void addInTransition(std::shared_ptr<BehavioralTransition> transizioneOsservabile)=0;
    virtual void addOutTransition(std::shared_ptr<BehavioralTransition> transizioneOsservabile)=0;
    virtual void removeInTransition(std::shared_ptr<BehavioralTransition> transizione) =0;

};


#endif //AUTOMATAMANAGER_NFASTATEINTERFACE_H
