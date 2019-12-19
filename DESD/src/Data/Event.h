//
// Created by Diego Lorenzi on 24/07/18.
//

#ifndef AUTOMATAMANAGER_EVENT_H
#define AUTOMATAMANAGER_EVENT_H

#include "../stdIncludes.h"
#include "Link.h"

class Event {
public:
    Link* link;
    std::string name;

    Event();

    Event(Link* _link, std::string& _name);

    bool isValid();

    std::string toString();
};


#endif //AUTOMATAMANAGER_EVENT_H
