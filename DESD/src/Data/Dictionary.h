//
// Created by Diego Lorenzi on 27/07/18.
//

#ifndef AUTOMATAMANAGER_DICTIONARY_H
#define AUTOMATAMANAGER_DICTIONARY_H

#include "DictionaryState.h"
#include "NFASpaceInterface.h"

struct transitionsInfo {
    unsigned int id;
    unsigned int start;
    unsigned int destination;
    std::string label;
    std::vector<std::shared_ptr<BehavioralState>> bhState;
    std::shared_ptr<std::vector<std::shared_ptr<transitionsInfo>>>tInfo;

    explicit transitionsInfo(unsigned int _id, std::string _label, unsigned int _start) {
        id =_id;
        label=_label;
        start=_start;
    }
    void setId(unsigned int _id){
        id=_id;

    }
    void setStart(unsigned int _start){
        start=_start;

    }
    void setDestination(unsigned int _destination){
        destination=_destination;

    }
    void setLabel(std::string _label){
        label=_label;

    }
    void setTinfoD(    std::shared_ptr<std::vector<std::shared_ptr<transitionsInfo>>>_tInfo   ){
        tInfo=_tInfo;

    }
    void setBhState(    std::vector<std::shared_ptr<BehavioralState>> &_bhState  ){
        bhState=_bhState;

    }

    std::vector<std::shared_ptr<BehavioralState>>  getBhState(){
        return bhState;
    }


    unsigned  int getId(){
        return id;
    }
    std::string getLabel(){
        return label;
    }
    unsigned  int getStart(){
        return start;
    }
    unsigned  int getDestination(){
        return destination;
    }
};



class Dictionary: public NFASpaceInterface {
public:
    std::vector<std::shared_ptr<DictionaryState>> states;
    std::vector<std::shared_ptr<NFATransitionInterface>> transitions;
    std::vector<std::shared_ptr<NFATransitionInterface>>  behavioralTransitions;
    std::vector<std::shared_ptr<NFAStateInterface>>  bhState;
    std::shared_ptr<std::vector<std::shared_ptr<transitionsInfo>>>tInfo;
    std::shared_ptr<std::vector<std::shared_ptr<transitionsInfo>>>tInfoDic;
/*
    std::vector<std::shared_ptr<BehavioralTransition>> behavioralTransitions;
*/
    int currentState;

    Dictionary();

    Dictionary(std::vector<std::shared_ptr<DictionaryState>> states,
               std::vector<std::shared_ptr<NFATransitionInterface>> transitions);

    void setToInitialState();
    void setBhState(std::vector<std::shared_ptr<NFAStateInterface>> &_bhSt);


    void performTransition(std::shared_ptr<NFATransitionInterface> transition);
    void setBehavioralTransitions(std::vector<std::shared_ptr<NFATransitionInterface>>  & _behavioralTransitions);
    void setTinfo(std::shared_ptr<std::vector<std::shared_ptr<transitionsInfo>>>& _tInfo);
    void setTinfoDic(std::shared_ptr<std::vector<std::shared_ptr<transitionsInfo>>>& _tInfoDic);
    void addBehavioralTransition(std::shared_ptr<BehavioralTransition>  &behavioralTransition);

    std::shared_ptr<DictionaryState> getInitialState();

    std::vector<std::shared_ptr<NFATransitionInterface>> getTransitions() override;


    std::vector<std::shared_ptr<NFAStateInterface>> getStates() override;

    std::vector<std::shared_ptr<NFATransitionInterface>> getBehavioralTransitions() override;
};


#endif //AUTOMATAMANAGER_DICTIONARY_H
