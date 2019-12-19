//
// Created by Diego Lorenzi on 24/07/18.
//

#ifndef AUTOMATAMANAGER_BSBUILDER_H
#define AUTOMATAMANAGER_BSBUILDER_H

#include "../stdIncludes.h"
#include "../Data/Network.h"
#include "../Data/BehavioralSpace.h"
#include "../Data/SimpleNetwork.h"
#include "../Utils/Logger.h"

class BSBuilder {
public:

    static std::shared_ptr<BehavioralSpace> build(std::shared_ptr<Network> network, std::shared_ptr<long> maxExecTime,
          std::shared_ptr<SimpleNetwork> observation = nullptr,
          std::shared_ptr<SimpleNetwork> bond = nullptr);
    

    static std::shared_ptr<BehavioralSpace> buildFromDFA(std::shared_ptr<Network> network, std::shared_ptr<long> maxExecTime,
                                                  std::shared_ptr<SimpleNetwork> observation = nullptr,
                                                  std::shared_ptr<SimpleNetwork> bond = nullptr);

    static void prune(std::shared_ptr<BehavioralSpace> space, std::shared_ptr<long> maxExecTime);
    static void  pruneBond(std::shared_ptr<BehavioralSpace> space, std::shared_ptr<long> maxExecTime, unsigned int fin);

};


#endif //AUTOMATAMANAGER_BSBUILDER_H
