#ifndef AUTOMATAMANAGER_SIMPLESTATE_H
#define AUTOMATAMANAGER_SIMPLESTATE_H

#include "../stdIncludes.h"
#include "SimpleTransition.h"

class SimpleState {
public:
    unsigned int id;
    std::vector<std::shared_ptr<SimpleTransition>> in;
    std::vector<std::shared_ptr<SimpleTransition>> out;
    bool starting;
    bool final;

    SimpleState(unsigned int _id, const std::vector<std::shared_ptr<SimpleTransition>> &_in,
                const std::vector<std::shared_ptr<SimpleTransition>> &_out, bool _final, bool _starting);
    SimpleState(unsigned int _id, bool _final, bool _starting);
    std::string toString();
};


#endif //AUTOMATAMANAGER_SIMPLESTATE_H
