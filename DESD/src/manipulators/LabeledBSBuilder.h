//
// Created by Diego Lorenzi on 26/07/18.
//

#ifndef AUTOMATAMANAGER_LABELEDBSBUILDER_H
#define AUTOMATAMANAGER_LABELEDBSBUILDER_H

#include "../Data/BehavioralSpace.h"
#include "../Data/Network.h"
#include "../Data/SimpleNetwork.h"
#include "../Utils/Logger.h"

class LabeledBSBuilder {
public:
    static std::shared_ptr<BehavioralSpace> build(std::shared_ptr<BehavioralSpace> bs,
                                                  std::shared_ptr<long> maxExecTime);
    
    static std::shared_ptr<BehavioralSpace> build(std::shared_ptr<BehavioralSpace> bs,
    std::shared_ptr<long> maxExecTime, int prefix);
    static std::shared_ptr<BehavioralSpace> build(std::shared_ptr<Network> network, std::shared_ptr<long> maxExecTime,
                                                  std::shared_ptr<SimpleNetwork> observation = nullptr,
                                                  std::shared_ptr<SimpleNetwork> bond = nullptr);
};


#endif //AUTOMATAMANAGER_LABELEDBSBUILDER_H
