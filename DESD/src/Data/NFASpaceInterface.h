//
// Created by alessandro on 8/1/18.
//

#ifndef AUTOMATAMANAGER_NFASPACEINTERFACE_H
#define AUTOMATAMANAGER_NFASPACEINTERFACE_H

#include "../stdIncludes.h"
#include "NFATransitionInterface.h"
#include "NFAStateInterface.h"


class NFASpaceInterface {
public:

    virtual std::vector<std::shared_ptr<NFATransitionInterface>> getTransitions() = 0;
    virtual std::vector<std::shared_ptr<NFATransitionInterface>> getBehavioralTransitions() = 0;

    virtual std::vector<std::shared_ptr<NFAStateInterface>> getStates() = 0;

};


#endif //AUTOMATAMANAGER_NFASPACEINTERFACE_H
