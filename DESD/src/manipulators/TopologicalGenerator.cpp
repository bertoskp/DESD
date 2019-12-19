#include "TopologicalGenerator.h"



void TopologicalGenerator::createNetwork(int numberComponent){
    int nLinks=(numberComponent+1)*2;
    int nEvents=4;
    int countE=0;
    int countL=0;
    std::shared_ptr<std::vector<std::shared_ptr<TransitionEvent>>> transEvs(new std::vector<std::shared_ptr<TransitionEvent>>());


    int countT=0;
    std::vector<std::shared_ptr<Component>> components;
    std::vector<std::shared_ptr<Link>> links;
    std::vector<std::string> events;
    for(int i=0; i<nEvents;i++){
        auto e= "e"+std::to_string(i);
        events.push_back(e);
    }
    
    TopologicalGenerator::createInitialComponent( countE,countT,countL,links);
    
   

}


 void TopologicalGenerator::createInitialComponent(int &countE,int &countT,int &countL,std::vector<std::shared_ptr<Link>> &links){
    std::vector<std::shared_ptr<State>> states;
     
    std::vector<std::shared_ptr<Transition>> transitions;
    int nTransition=2;
    int nStates=2;
    int id=0;
    std::string componentName="IC";
    //TRANSIZIONI E LINK
    
    
    auto L1=std::make_shared<Link>("L"+std::to_string(countL));
    links.push_back(L1);
    countL=countL+1;
    std::vector<std::shared_ptr<Event>> outEvents;
     std::string e0="e0";
     std::string e3="e3";
     std::string e4="e4";
     std::shared_ptr<Event> inEvent = nullptr;
     outEvents.push_back(std::make_shared<Event>(L1.get(), e0));
     std::string en4="e4";
     //auto tt=std::make_shared< Transition>("mm",  &inEvent, &outEvents);
     /*
    
    
    countT=countT+1;
    auto L2=std::make_shared<Link>("L"+std::to_string(countL));
    links.push_back(L2);
    countL=countL+1;
    std::vector<std::shared_ptr<Event>> outEvents2;
      */
    /*
    outEvents.push_back(std::make_shared<Event>(L1.get(), e4));
    auto t2=std::make_shared<Transition>("t"+std::to_string(countT), std::make_shared<Event>(L2.get(), e3), outEvents);
    countT=countT+1;
    transitions.push_back(t1);
    transitions.push_back(t2);
    transEvs->push_back(std::make_shared<TransitionEvent>(t1,"", "", "", "e0"));
    transEvs->push_back(std::make_shared<TransitionEvent>(t2,"", "", "e3", "e4"));
    */
    //STATI
    

    //COMPONENTE
    
        //std::make_shared<Event>(std::make_shared<Link>("L"+std::to_string(countL),"L"+std::to_string(countE)));
      //  std::vector<std::shared_ptr<Event>> outEvents;
       
        
}
   


    
       

