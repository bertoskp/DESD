//
// Created by Nicola Bertoglio on 2019-04-03.
//

#ifndef AUTOMATAMANAGER_OBSERVATION_H
#define AUTOMATAMANAGER_OBSERVATION_H


#include "../stdIncludes.h"

class Observation {
public:
    std::set<std::set<std::string>> obs;

    Observation(std::set<std::set<std::string>> obs);

    std::set<std::set<std::string>> getObs();

};


#endif //AUTOMATAMANAGER_OBSERVATION_H
