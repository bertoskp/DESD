//
// Created by Diego Lorenzi on 24/07/18.
//

#ifndef AUTOMATAMANAGER_BEHAVIORALTRANSITION_H
#define AUTOMATAMANAGER_BEHAVIORALTRANSITION_H

#include "../stdIncludes.h"
#include "NFATransitionInterface.h"


class BehavioralTransition : public NFATransitionInterface {
public:

    std::string name;
    unsigned int id;
    static unsigned int nextId;

    std::string relevancyLabel;
    std::string observabilityLabel;
    int deepth;

    BehavioralTransition();

    explicit BehavioralTransition(const std::string& _name);

    BehavioralTransition(const std::string& _name, unsigned int _id);

    BehavioralTransition(const std::string& _name, const std::string& _relevancyLabel, const std::string& _observabilityLabel);

    BehavioralTransition(const std::string& _name, unsigned int _id, const std::string& _relevancyLabel,
                         const std::string& _observabilityLabel);

    std::string toString() override;

    int getId() override;

    std::string getObservabilityLabel() override;
    int getDeepth();
    void setDeepth(int d);
    std::string getName()override;
    std::string getRelevancyLabel()override;
};


#endif //AUTOMATAMANAGER_BEHAVIORALTRANSITION_H
