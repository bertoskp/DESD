

#include "Observation.h"


Observation::Observation(std::set<std::set<std::string>> _obs) {
    obs=_obs;
}


std::set<std::set<std::string>> Observation::getObs() {
    return obs;
}
