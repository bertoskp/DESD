

#ifndef AUTOMATAMANAGER_TRANSITION_H
#define AUTOMATAMANAGER_TRANSITION_H

#include "../stdIncludes.h"
#include "Event.h"
#include <utility>

class Transition {
public:
    std::string name;
    std::shared_ptr<Event> in;
    std::vector<std::shared_ptr<Event>> out;

    std::string relevancyLabel;
    std::string observabilityLabel;

    

    explicit Transition(std::string& _name, std::shared_ptr<Event>& _in, std::vector<std::shared_ptr<Event>> &_out);

    Transition(std::string& _name, std::shared_ptr<Event>& _in, std::vector<std::shared_ptr<Event>> &_out,
    std::string& _relevancyLabel, std::string& _observabilityLabel);

    
    
    
    
    void perform();

    bool isQualified();

    std::string toString();
    void setInEvent( std::shared_ptr<Event>&_in);
    void setOutEvent( std::vector<std::shared_ptr<Event>>&_out);
    
    std::string getObservabilityLabel();
};


#endif //AUTOMATAMANAGER_TRANSITION_H
