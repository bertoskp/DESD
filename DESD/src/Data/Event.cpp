

#include "Event.h"

Event::Event(){
    link = nullptr;
};

Event::Event(Link* _link, std::string& _name){
link = _link;
name = _name;
}

std::string Event::toString() {
    if (!isValid()) return "NULL";
    return name + "(" + link->name + ")";
}

bool Event::isValid() {
    return link != nullptr && link->isValid() && !name.empty();
}
