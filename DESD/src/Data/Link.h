//
// Created by Diego Lorenzi on 24/07/18.
//

#ifndef AUTOMATAMANAGER_LINK_H
#define AUTOMATAMANAGER_LINK_H

#include "../stdIncludes.h"

class Link {
public:

    std::string buffer;
    std::string name;

    Link() = default;

    Link(std::string& _buffer, std::string& _name);

    explicit Link(std::string _name);

    std::string toString();

    bool isValid();
};


#endif //AUTOMATAMANAGER_LINK_H
