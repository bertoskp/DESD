

#ifndef AUTOMATAMANAGER_BSBUILDERDFA_H
#define AUTOMATAMANAGER_BSBUILDERDFA_H

#include "../stdIncludes.h"
#include "../Data/Network.h"
#include "../Data/BehavioralSpace.h"
#include "../Data/Dictionary.h"
#include "../Data/SimpleNetwork.h"
#include "../Utils/Logger.h"

class BSBuilderDFA {
public:


    static void build(std::shared_ptr<Network> network,std::vector<std::shared_ptr<BehavioralState>> nfaStates,std::string label,std::vector<std::shared_ptr<NFAStateInterface>> totalBhs);


    static std::set<std::shared_ptr<BehavioralState>>  silentClosure(std::shared_ptr<Network> network,std::vector<std::shared_ptr<BehavioralState>> nfaStates,
            std::string label,std::vector<std::shared_ptr<NFAStateInterface>> totalBhs, std::vector<std::shared_ptr<BehavioralTransition>> nuoveTransizioni, std::shared_ptr<Dictionary> inDictionary);

    static void prune(std::shared_ptr<BehavioralSpace> space, std::shared_ptr<long> maxExecTime);


};


#endif //AUTOMATAMANAGER_BSBUILDER_H
