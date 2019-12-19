//
// Created by alessandro on 7/31/18.
//

#ifndef AUTOMATAMANAGER_SIMPLETRANSITION_H
#define AUTOMATAMANAGER_SIMPLETRANSITION_H


#include "../stdIncludes.h"

class SimpleTransition {
public:
    unsigned int id;
    std::string label;

    SimpleTransition(unsigned int _id, std::string &_label);

    std::string toString() {
        return std::to_string(id) + "(" + label + ")";
    }
};


#endif //AUTOMATAMANAGER_SIMPLETRANSITION_H
