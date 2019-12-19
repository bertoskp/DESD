

#include "Link.h"
#include <utility>

Link::Link(std::string& _buffer, std::string& _name){
    buffer = _buffer;
    name = _name;
}

Link::Link(std::string _name){
    buffer = "";
    name = std::move(_name);
}

std::string Link::toString() {
    if (!isValid()) return "INVALID";
    return "Link " + name + " = " + (!buffer.empty() ? buffer : "~EMPTY~");
}

bool Link::isValid() {
    return !name.empty();
}
