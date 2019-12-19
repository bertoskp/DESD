//
// Created by alessandro on 8/1/18.
//

#ifndef AUTOMATAMANAGER_NFATRANSITIONINTERFACE_H
#define AUTOMATAMANAGER_NFATRANSITIONINTERFACE_H


class NFATransitionInterface {
public:
    virtual int getId() = 0;

    virtual std::string getObservabilityLabel() = 0;

    virtual std::string toString() = 0;

    virtual std::string getName()=0;



    virtual std::string getRelevancyLabel()=0;



};


#endif //AUTOMATAMANAGER_NFATRANSITIONINTERFACE_H
