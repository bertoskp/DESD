#include "json.hpp"
#include "../stdIncludes.h"
#include "../Data/Network.h"
#include <ctime>
#include <time.h>

class Generate {
public:
    
    struct ComponentLinks{
        std::shared_ptr<Link> InL;
        std::shared_ptr<Link> OutL;
        std::shared_ptr<Link> InR;
        std::shared_ptr<Link> OutR;
        std::shared_ptr<Component> Compo;
        explicit ComponentLinks(std::shared_ptr<Link> &_InL,std::shared_ptr<Link> &_OutL,std::shared_ptr<Link> &_InR,std::shared_ptr<Link> &_OutR, std::shared_ptr<Component> &_Component) {
        InL=_InL;
        OutL=_OutL;
        InR=_InR;
        OutR=_OutR;
        Compo=_Component;
        }
    };
    struct TransitionEvent{
        std::shared_ptr<Transition> transition;
        std::string InL;
        std::string OutL;
        std::string InR;
        std::string OutR;
        
        explicit TransitionEvent(std::shared_ptr<Transition> &_t, std::string _InL, std::string _OutL, std::string _InR, std::string _OutR){
            transition=_t;
            InL=_InL;
            OutL=_OutL;
            InR=_InR;
            OutR=_OutR;
        }
        
    };
    
    // ****************************************************************
    // LINEARE BIDIREZIONALE
    
    static std::shared_ptr<Network> createNetwork(int numberComponent){
       
        int nEvents=4;
        int countE=0;
        int countL=0;
        int countS=0;
         int countC=0;
        std::shared_ptr<std::vector<std::shared_ptr<TransitionEvent>>> transEvs(new std::vector<std::shared_ptr<TransitionEvent>>());
        std::shared_ptr<std::vector<std::shared_ptr<ComponentLinks>>> compLin(new std::vector<std::shared_ptr<ComponentLinks>>());

        std::vector<std::shared_ptr<Component>> components;
        int countT=0;
        std::vector<std::shared_ptr<Link>> links;
        std::vector<std::string> events;
        for(int i=0; i<nEvents;i++){
            auto e= "e"+std::to_string(i);
            events.push_back(e);
        }
        
        auto lastCompLin=createInitialComponent( countE,countT,countL,countS,links,components,transEvs,compLin);
        for (int i=0;  i<numberComponent;i++){
            lastCompLin= createIntermedialComponent(countE,countT,countL,countS,countC,links,components, transEvs,compLin,lastCompLin);
            
        }
        createFinalComponent(countE, countT, countL, countS, links, components, transEvs, compLin, lastCompLin);
       
        auto network=std::make_shared<Network>(components, links);
        std::shared_ptr<SimpleNetwork> observation = nullptr, bond = nullptr;
               
        std::shared_ptr<long> maxExecutionTime = nullptr;
       
            //auto bs=BSBuilder::build(network, maxExecutionTime, observation, bond);
        return network;
    }


    static std::shared_ptr<ComponentLinks> createInitialComponent(int &countE,int &countT,int &countL,int &countS,std::vector<std::shared_ptr<Link>> &links,        std::vector<std::shared_ptr<Component>> &components,  std::shared_ptr<std::vector<std::shared_ptr<TransitionEvent>>> &transEvs,           std::shared_ptr<std::vector<std::shared_ptr<ComponentLinks>>> &compLin){
        std::vector<std::shared_ptr<State>> states;
         
        std::vector<std::shared_ptr<Transition>> transitions;
       // int nTransition=2;
        //int nStates=2;
        //int id=0;
        std::string componentName="IC";
        //TRANSIZIONI E LINK
        
        
        auto L1=std::make_shared<Link>("L"+std::to_string(countL));
        links.push_back(L1);
        countL=countL+1;
        std::vector<std::shared_ptr<Event>> outEvents;
         std::string e0="e0";
         std::string e2="e2";
         std::string e4="e4";
         std::shared_ptr<Event> inEvent = nullptr;
         
         std::string en4="e4";
         //auto tt=std::make_shared< Transition>("mm",  &inEvent, &outEvents);
         std::string transitionName = "T"+ std::to_string(countT);
         outEvents.push_back(std::make_shared<Event>(L1.get(), e0));
         std::string observation, relevancy;
        observation="o"+ std::to_string(countT);
        relevancy="";
         auto t1=std::make_shared<Transition>(transitionName, inEvent, outEvents, relevancy, observation);
         transitions.push_back(t1);
        
        auto te = TransitionEvent(t1,"","","",e0);
        
         countT=countT+1;
         std::string transitionName2 = "T"+ std::to_string(countT);
        countT=countT+1;
         auto L2=std::make_shared<Link>("L"+std::to_string(countL));
         links.push_back(L2);
       
                countL=countL+1;
         std::shared_ptr<Event> inEvent2 = std::make_shared<Event>(L2.get(), e2);
         std::vector<std::shared_ptr<Event>> outEvents2;
         outEvents2.push_back(std::make_shared<Event>(L1.get(), e4));
        observation="o"+ std::to_string(countT);
        auto t2=std::make_shared<Transition>(transitionName2, inEvent2, outEvents2, relevancy, observation);
        auto te2 = TransitionEvent(t2,"","",e2,e4);
        transitions.push_back(t2);
       //STATI
        
         std::string name = "s"+std::to_string(countS);
                countS=countS+1;
        bool starting = true;

        
        std::vector<std::shared_ptr<Transition>> inTransitions, outTransitions;

        inTransitions.push_back(t2);
        outTransitions.push_back(t1);

        states.push_back(std::make_shared<State>(name, starting, inTransitions, outTransitions));
        
         name = "s"+std::to_string(countS);
                countS=countS+1;
        starting = false;

        
        std::vector<std::shared_ptr<Transition>> inTransitions1, outTransitions1;

        inTransitions1.push_back(t1);
        outTransitions1.push_back(t2);

        states.push_back(std::make_shared<State>(name, starting, inTransitions1, outTransitions1));
        auto component=std::make_shared<Component>(states, transitions, componentName);
        components.push_back(component);
        std::shared_ptr<Link> InL;
        std::shared_ptr<Link> OutL;
        std::shared_ptr<Link> InR;
        std::shared_ptr<Link> OutR;
       
        auto aaa=std::make_shared<ComponentLinks>(InL,OutL,L2,L1,component);
        //compLin.push_back();
         std::string e4n="e4";
        return aaa;
     }
    
     static std::shared_ptr<ComponentLinks> createIntermedialComponent(int &countE,int &countT,int &countL,int &countS,int &countC,std::vector<std::shared_ptr<Link>> &links,        std::vector<std::shared_ptr<Component>> &components,  std::shared_ptr<std::vector<std::shared_ptr<TransitionEvent>>> &transEvs,           std::shared_ptr<std::vector<std::shared_ptr<ComponentLinks>>> &compLin,std::shared_ptr<ComponentLinks> &lastCompLin){
         auto OutR=std::make_shared<Link>("L"+std::to_string(countL));
         countL=countL+1;
         auto InR=std::make_shared<Link>("L"+std::to_string(countL));
         countL=countL+1;
         links.push_back(OutR);
         links.push_back(InR);
         std::vector<std::shared_ptr<State>> states;
          
         std::vector<std::shared_ptr<Transition>> transitions;
         
         auto OutL=lastCompLin->InR;
         auto InL=lastCompLin->OutR;
         std::string e0="e0";
         std::string e2="e2";
         std::string e3="e3";
         std::string e4="e4";
        // transizioni
         //t1
         std::shared_ptr<Event> inEvent = std::make_shared<Event>(InL.get(), e0);
         std::string transitionName = "T"+ std::to_string(countT);
         countT=countT+1;
         std::vector<std::shared_ptr<Event>> outEvents1;
         
        
          std::string observation, relevancy;
         observation="";
         relevancy="";
          auto t1=std::make_shared<Transition>(transitionName, inEvent, outEvents1, relevancy, observation);
          transitions.push_back(t1);
         
         //t2
         std::shared_ptr<Event> inEvent2;
         std::string transitionName2 = "T"+ std::to_string(countT);
         countT=countT+1;

          std::vector<std::shared_ptr<Event>> outEvents2;
          outEvents2.push_back(std::make_shared<Event>(OutR.get(), e0));
          //std::string observation, relevancy;
         relevancy="f2";
          auto t2=std::make_shared<Transition>(transitionName2, inEvent2, outEvents2, relevancy, observation);
          transitions.push_back(t2);
         //t3
         std::shared_ptr<Event> inEvent3 = nullptr;
         std::string transitionName3 = "T"+ std::to_string(countT);
         countT=countT+1;

          std::vector<std::shared_ptr<Event>> outEvents3;
          outEvents3.push_back(std::make_shared<Event>(OutR.get(), e0));
          //std::string observation, relevancy;
         // auto t3=std::make_shared<Transition>(transitionName3, inEvent3, outEvents3, relevancy, "o3");
         relevancy="";
         observation="o3";
         auto t3=std::make_shared<Transition>(transitionName3, inEvent3, outEvents3, relevancy,observation);

         transitions.push_back(t3);
         //t4
         std::shared_ptr<Event> inEvent4 ;
         inEvent4=std::make_shared<Event>(InL.get(), e4);
         std::string transitionName4 = "T"+ std::to_string(countT);
         countT=countT+1;

          std::vector<std::shared_ptr<Event>> outEvents4;
          outEvents4.push_back(std::make_shared<Event>(OutR.get(), e4));
          //std::string observation, relevancy;
         
         relevancy="";
         observation="o4";
          auto t4=std::make_shared<Transition>(transitionName4, inEvent4, outEvents4, relevancy, observation);

         //auto t4=std::make_shared<Transition>(transitionName4, inEvent4, outEvents4, relevancy, "o4");
          transitions.push_back(t4);
         
         //t5
                 std::shared_ptr<Event> inEvent5 ;
         inEvent5=std::make_shared<Event>(InL.get(), e4);
                 std::string transitionName5 = "T"+ std::to_string(countT);
                 countT=countT+1;

                  std::vector<std::shared_ptr<Event>> outEvents5;
                  outEvents5.push_back(std::make_shared<Event>(OutR.get(), e4));
                  //std::string observation, relevancy;
         relevancy="f5";
         observation="o5";
                  auto t5=std::make_shared<Transition>(transitionName5, inEvent5, outEvents5, relevancy, observation);

         //auto t5=std::make_shared<Transition>(transitionName5, inEvent5, outEvents5, "f5", "o5");
                  transitions.push_back(t5);
         //t6
                 std::shared_ptr<Event> inEvent6 ;
                 inEvent6=std::make_shared<Event>(InR.get(), e2);
                 std::string transitionName6 = "T"+ std::to_string(countT);
                 countT=countT+1;

                  std::vector<std::shared_ptr<Event>> outEvents6;
                  outEvents6.push_back(std::make_shared<Event>(OutL.get(), e2));
                  //std::string observation, relevancy;
                  //auto t6=std::make_shared<Transition>(transitionName6, inEvent6, outEvents6, "f6", observation);
                     relevancy="f6";
                     observation="";
                    auto t6=std::make_shared<Transition>(transitionName6, inEvent6, outEvents6, relevancy, observation);

                  transitions.push_back(t6);
         //t7
                 std::shared_ptr<Event> inEvent7 ;
         inEvent7=std::make_shared<Event>(InR.get(), e2);
                 std::string transitionName7 = "T"+ std::to_string(countT);
                 countT=countT+1;

                  std::vector<std::shared_ptr<Event>> outEvents7;
                  outEvents7.push_back(std::make_shared<Event>(OutL.get(), e2));
                  //std::string observation, relevancy;
         relevancy="";
         observation="";
                  auto t7=std::make_shared<Transition>(transitionName7, inEvent7, outEvents7, relevancy, observation);
                  transitions.push_back(t7);
         
         //stati
         //s1
         std::string name = "s"+std::to_string(countS);
         countS=countS+1;
         bool starting = true;

         
         std::vector<std::shared_ptr<Transition>> inTransitions, outTransitions;

         inTransitions.push_back(t3);
         inTransitions.push_back(t5);
         inTransitions.push_back(t7);
         outTransitions.push_back(t1);
         outTransitions.push_back(t5);
         outTransitions.push_back(t6);

         states.push_back(std::make_shared<State>(name, starting, inTransitions, outTransitions));
         //s2
         name = "s"+std::to_string(countS);
          countS=countS+1;
         starting = false;

         
         std::vector<std::shared_ptr<Transition>> inTransitions2, outTransitions2;

         inTransitions2.push_back(t1);
         outTransitions2.push_back(t2);
         outTransitions2.push_back(t3);

         states.push_back(std::make_shared<State>(name, starting, inTransitions2, outTransitions2));
         //s3
         name = "s"+std::to_string(countS);
                 countS=countS+1;
                 starting = false;

                 
                 std::vector<std::shared_ptr<Transition>> inTransitions3, outTransitions3;

                 inTransitions3.push_back(t2);
                 inTransitions3.push_back(t4);
                 inTransitions3.push_back(t6);
                 outTransitions3.push_back(t4);
                 outTransitions3.push_back(t7);
                 

                 states.push_back(std::make_shared<State>(name, starting, inTransitions3, outTransitions3));
          std::string componentName="C"+ std::to_string(countC);
         countC=countC+1;
         auto component=std::make_shared<Component>(states, transitions, componentName);
         components.push_back(component);
         
         
         auto aaa=std::make_shared<ComponentLinks>(InL,OutL,InR,OutR,component);
         //compLin.push_back();
          std::string e4n="e4";
         return aaa;
     }
    
    
    static void createFinalComponent(int &countE,int &countT,int &countL,int &countS,std::vector<std::shared_ptr<Link>> &links,        std::vector<std::shared_ptr<Component>> &components,  std::shared_ptr<std::vector<std::shared_ptr<TransitionEvent>>> &transEvs,           std::shared_ptr<std::vector<std::shared_ptr<ComponentLinks>>> &compLin,std::shared_ptr<ComponentLinks> &lastCompLin){
        auto OutL=lastCompLin->InR;
        auto InL=lastCompLin->OutR;
        std::shared_ptr<Link> InR;
        std::shared_ptr<Link> OutR;
        InR=nullptr;
        OutR=nullptr;
        std::vector<std::shared_ptr<State>> states;
        std::vector<std::shared_ptr<Transition>> transitions;
         
         std::string componentName="IF";
         //TRANSIZIONI E LINK
         
         //t1
         
         std::vector<std::shared_ptr<Event>> outEvents;
          std::string e0="e0";
         std::string e2="e2";

          std::string e3="e3";
          std::string e4="e4";
          std::shared_ptr<Event> inEvent = std::make_shared<Event>(InL.get(), e0);
          
         
          std::string transitionName = "T"+ std::to_string(countT);
          outEvents.push_back(std::make_shared<Event>(OutL.get(), e2));
          std::string observation, relevancy;
        observation="o1";
          auto t1=std::make_shared<Transition>(transitionName, inEvent, outEvents, relevancy, observation);
          transitions.push_back(t1);
         
         auto te = TransitionEvent(t1,e0,e2,"","");
         
          countT=countT+1;
                              
                              
         //t2
          std::string transitionName2 = "T"+ std::to_string(countT);
         
          std::shared_ptr<Event> inEvent2 = std::make_shared<Event>(InL.get(), e4);
          std::vector<std::shared_ptr<Event>> outEvents2;
        observation="o2";

         auto t2=std::make_shared<Transition>(transitionName2, inEvent2, outEvents2, relevancy, observation);
         auto te2 = TransitionEvent(t2,e4,"","","");
         transitions.push_back(t2);
                              
                              
        //STATI
         //s1
         std::string name = "s"+std::to_string(countS);
         countS=countS+1;
         bool starting = true;

         
         std::vector<std::shared_ptr<Transition>> inTransitions, outTransitions;

         inTransitions.push_back(t2);
         outTransitions.push_back(t1);

         states.push_back(std::make_shared<State>(name, starting, inTransitions, outTransitions));
         
        name = "s"+std::to_string(countS);
         countS=countS+1;
         starting = false;

         
         std::vector<std::shared_ptr<Transition>> inTransitions1, outTransitions1;

         inTransitions1.push_back(t1);
         outTransitions1.push_back(t2);

         states.push_back(std::make_shared<State>(name, starting, inTransitions1, outTransitions1));
         auto component=std::make_shared<Component>(states, transitions, componentName);
         components.push_back(component);
        
         auto aaa=std::make_shared<ComponentLinks>(InL,OutL,InR,OutR,component);
    }
    
    //********************************************************
    //FINE LINEARE BIDIREZIONALE
    
    
    
    
    
    
    // ****************************************************************
    //LINEARE UNIDIREZIONALE ZANELLA
    
    
    
    
    
    static std::shared_ptr<Network> createNetworkZ(int numberComponent){
       
        int nEvents=4;
        int countE=0;
        int countL=0;
        int countS=0;
         int countC=0;
        std::shared_ptr<std::vector<std::shared_ptr<TransitionEvent>>> transEvs(new std::vector<std::shared_ptr<TransitionEvent>>());
        std::shared_ptr<std::vector<std::shared_ptr<ComponentLinks>>> compLin(new std::vector<std::shared_ptr<ComponentLinks>>());

        std::vector<std::shared_ptr<Component>> components;
        int countT=0;
        std::vector<std::shared_ptr<Link>> links;
        std::vector<std::string> events;
        for(int i=0; i<nEvents;i++){
            auto e= "e"+std::to_string(i);
            events.push_back(e);
        }
        
        auto lastCompLin=createInitialComponentZ( countE,countT,countL,countS,links,components,transEvs,compLin);
        for (int i=0;  i<numberComponent;i++){
            lastCompLin= createIntermedialComponentZ(countE,countT,countL,countS,countC,links,components, transEvs,compLin,lastCompLin);
            
        }
        createFinalComponentZ(countE, countT, countL, countS, links, components, transEvs, compLin, lastCompLin);
       
        auto network=std::make_shared<Network>(components, links);
        std::shared_ptr<SimpleNetwork> observation = nullptr, bond = nullptr;
               
        std::shared_ptr<long> maxExecutionTime = nullptr;
       
            //auto bs=BSBuilder::build(network, maxExecutionTime, observation, bond);
        return network;
    }
    
    
    static std::shared_ptr<ComponentLinks> createInitialComponentZ(int &countE,int &countT,int &countL,int &countS,std::vector<std::shared_ptr<Link>> &links,        std::vector<std::shared_ptr<Component>> &components,  std::shared_ptr<std::vector<std::shared_ptr<TransitionEvent>>> &transEvs,           std::shared_ptr<std::vector<std::shared_ptr<ComponentLinks>>> &compLin){
        std::vector<std::shared_ptr<State>> states;
         
        std::vector<std::shared_ptr<Transition>> transitions;
       // int nTransition=2;
       // int nStates=2;
        //int id=0;
        std::string componentName="IC";
        //TRANSIZIONI E LINK
        
        
        auto L0=std::make_shared<Link>("L"+std::to_string(countL));
        links.push_back(L0);
        countL=countL+1;
        std::vector<std::shared_ptr<Event>> outEvents;
         std::string e0="e0";
         std::string e2="e2";
         std::string e4="e4";
         std::shared_ptr<Event> inEvent = nullptr;
         
         //T0
         //auto tt=std::make_shared< Transition>("mm",  &inEvent, &outEvents);
         std::string transitionName = "T"+ std::to_string(countT);
         outEvents.push_back(std::make_shared<Event>(L0.get(), e0));
         std::string observation, relevancy;
        observation="o"+ std::to_string(countT);
        relevancy="";
         auto t0=std::make_shared<Transition>(transitionName, inEvent, outEvents, relevancy, observation);
         transitions.push_back(t0);
        
        auto te = TransitionEvent(t0,"","","",e0);
        
         countT=countT+1;
         std::string transitionName2a = "T"+ std::to_string(countT)+"a";
        std::string transitionName2b = "T"+ std::to_string(countT)+"b";
        countT=countT+1;
         //auto L2=std::make_shared<Link>("L"+std::to_string(countL));
         //links.push_back(L2);
       
        
        
        //T1A
         //countL=countL+1;
         std::shared_ptr<Event> inEvent2 = nullptr;
         std::vector<std::shared_ptr<Event>> outEvents2;
         outEvents2.push_back(std::make_shared<Event>(L0.get(), e2));
        observation="";
        auto t1a=std::make_shared<Transition>(transitionName2a, inEvent2, outEvents2, relevancy, observation);
        auto te2 = TransitionEvent(t1a,"","","",e2);
        transitions.push_back(t1a);
        
        
        
        //T1b
         //countL=countL+1;
         std::shared_ptr<Event> inEvent2b = nullptr;
         std::vector<std::shared_ptr<Event>> outEvents2b;
         outEvents2b.push_back(std::make_shared<Event>(L0.get(), e4));
        observation="o"+ std::to_string(countT);
        auto t1b=std::make_shared<Transition>(transitionName2b, inEvent2b, outEvents2b, relevancy, observation);
        auto te3 = TransitionEvent(t1b,"","","",e4);
        transitions.push_back(t1b);
       
        
        //STATI
        
        //S0
         std::string name = "s"+std::to_string(countS);
                countS=countS+1;
        bool starting = true;

        
        std::vector<std::shared_ptr<Transition>> inTransitions, outTransitions;

        //inTransitions.push_back(t2);
        outTransitions.push_back(t0);
         outTransitions.push_back(t1a);

        states.push_back(std::make_shared<State>(name, starting, inTransitions, outTransitions));
        
        
        //S1
         name = "s"+std::to_string(countS);
                countS=countS+1;
        starting = false;

        
        std::vector<std::shared_ptr<Transition>> inTransitions1, outTransitions1;
        inTransitions1.push_back(t0);
        inTransitions1.push_back(t1b);
        //outTransitions1.push_back(t2);

        states.push_back(std::make_shared<State>(name, starting, inTransitions1, outTransitions1));
        
        
        
        //S01
        
        
        
         name = "s01";
               // countS=countS+1;
        starting = false;

        
        std::vector<std::shared_ptr<Transition>> inTransitions01, outTransitions01;

        inTransitions01.push_back(t1a);
        outTransitions01.push_back(t1b);

        states.push_back(std::make_shared<State>(name, starting, inTransitions01, outTransitions01));
        auto component=std::make_shared<Component>(states, transitions, componentName);
        components.push_back(component);
        std::shared_ptr<Link> InL;
        std::shared_ptr<Link> OutL;
        std::shared_ptr<Link> InR;
        std::shared_ptr<Link> OutR;
       
        auto aaa=std::make_shared<ComponentLinks>(InL,OutL,InR,L0,component);
        //compLin.push_back();
         std::string e4n="e4";
        return aaa;
     }
    
     static std::shared_ptr<ComponentLinks> createIntermedialComponentZ(int &countE,int &countT,int &countL,int &countS,int &countC,std::vector<std::shared_ptr<Link>> &links,        std::vector<std::shared_ptr<Component>> &components,  std::shared_ptr<std::vector<std::shared_ptr<TransitionEvent>>> &transEvs,           std::shared_ptr<std::vector<std::shared_ptr<ComponentLinks>>> &compLin,std::shared_ptr<ComponentLinks> &lastCompLin){
         auto OutR=std::make_shared<Link>("L"+std::to_string(countL));
         countL=countL+1;
         //auto InR=std::make_shared<Link>("L"+std::to_string(countL));
         countL=countL+1;
         links.push_back(OutR);
         //links.push_back(InR);
         std::vector<std::shared_ptr<State>> states;
          
         std::vector<std::shared_ptr<Transition>> transitions;
         auto InR=lastCompLin->InR;
         auto OutL=lastCompLin->InR;
         auto InL=lastCompLin->OutR;
         std::string e0="e0";
         std::string e2="e2";
         std::string e3="e3";
         std::string e4="e4";
        // transizioni
         //t1
         std::shared_ptr<Event> inEvent = std::make_shared<Event>(InL.get(), e0);
         std::string transitionName = "T"+ std::to_string(countT);
         countT=countT+1;
         std::vector<std::shared_ptr<Event>> outEvents1;
         
        
          std::string observation, relevancy;
         observation="";
         relevancy="";
          auto t1=std::make_shared<Transition>(transitionName, inEvent, outEvents1, relevancy, observation);
          transitions.push_back(t1);
         
         //t2
         std::shared_ptr<Event> inEvent2;
         std::string transitionName2 = "T"+ std::to_string(countT);
         countT=countT+1;

          std::vector<std::shared_ptr<Event>> outEvents2;
          outEvents2.push_back(std::make_shared<Event>(OutR.get(), e0));
          //std::string observation, relevancy;
         relevancy="f2";
          auto t2=std::make_shared<Transition>(transitionName2, inEvent2, outEvents2, relevancy, observation);
          transitions.push_back(t2);
         //t3
         std::shared_ptr<Event> inEvent3 = nullptr;
         std::string transitionName3 = "T"+ std::to_string(countT);
         countT=countT+1;

          std::vector<std::shared_ptr<Event>> outEvents3;
          outEvents3.push_back(std::make_shared<Event>(OutR.get(), e0));
          //std::string observation, relevancy;
         // auto t3=std::make_shared<Transition>(transitionName3, inEvent3, outEvents3, relevancy, "o3");
         relevancy="";
         observation="o3";
         auto t3=std::make_shared<Transition>(transitionName3, inEvent3, outEvents3, relevancy,observation);

         transitions.push_back(t3);
         //t4
         std::shared_ptr<Event> inEvent4 ;
         inEvent4=std::make_shared<Event>(InL.get(), e4);
         std::string transitionName4 = "T"+ std::to_string(countT);
         countT=countT+1;

          std::vector<std::shared_ptr<Event>> outEvents4;
          outEvents4.push_back(std::make_shared<Event>(OutR.get(), e4));
          //std::string observation, relevancy;
         
         relevancy="";
         observation="o4";
          auto t4=std::make_shared<Transition>(transitionName4, inEvent4, outEvents4, relevancy, observation);

         //auto t4=std::make_shared<Transition>(transitionName4, inEvent4, outEvents4, relevancy, "o4");
          transitions.push_back(t4);
         
         //t5
                 std::shared_ptr<Event> inEvent5 ;
         inEvent5=std::make_shared<Event>(InL.get(), e4);
                 std::string transitionName5 = "T"+ std::to_string(countT);
                 countT=countT+1;

                  std::vector<std::shared_ptr<Event>> outEvents5;
                  outEvents5.push_back(std::make_shared<Event>(OutR.get(), e4));
                  //std::string observation, relevancy;
         relevancy="f5";
         observation="o5";
                  auto t5=std::make_shared<Transition>(transitionName5, inEvent5, outEvents5, relevancy, observation);

         //auto t5=std::make_shared<Transition>(transitionName5, inEvent5, outEvents5, "f5", "o5");
                  transitions.push_back(t5);
         //t7a
                 std::shared_ptr<Event> inEvent6=nullptr;
                 //inEvent6=std::make_shared<Event>(InR.get(), e2);
                 std::string transitionName6 = "T7a";
                 

                  std::vector<std::shared_ptr<Event>> outEvents6;
                  outEvents6.push_back(std::make_shared<Event>(OutR.get(), e2));
                
                  
                  //std::string observation, relevancy;
                  //auto t6=std::make_shared<Transition>(transitionName6, inEvent6, outEvents6, "f6", observation);
                     relevancy="";
                     observation="";
                    auto t7a=std::make_shared<Transition>(transitionName6, inEvent6, outEvents6, relevancy, observation);

                  transitions.push_back(t7a);
         //t7b
         std::shared_ptr<Event> inEvent7b=std::make_shared<Event>(InL.get(), e2);
         //inEvent6=std::make_shared<Event>(InR.get(), e2);
         std::string transitionName7b = "T7b";
         
         countT=countT+1;
          std::vector<std::shared_ptr<Event>> outEvents7b;
          
          //std::string observation, relevancy;
          //auto t6=std::make_shared<Transition>(transitionName6, inEvent6, outEvents6, "f6", observation);
             relevancy="f6";
             observation="";
            auto t7b=std::make_shared<Transition>(transitionName7b, inEvent7b, outEvents7b, relevancy, observation);

          transitions.push_back(t7b);
         //t8a
                 std::shared_ptr<Event> inEvent7=nullptr;
                 std::string transitionName7 = "T8a";
                 countT=countT+1;

                  std::vector<std::shared_ptr<Event>> outEvents7;
                  outEvents7.push_back(std::make_shared<Event>(OutR.get(), e2));
                  //std::string observation, relevancy;
         relevancy="";
         observation="";
                  auto t8a=std::make_shared<Transition>(transitionName7, inEvent7, outEvents7, relevancy, observation);
                  transitions.push_back(t8a);
         
         //t8b
                         std::shared_ptr<Event> inEvent8b;
                 inEvent8b=std::make_shared<Event>(InL.get(), e2);
                         std::string transitionName8b = "T8b";
                         

                          std::vector<std::shared_ptr<Event>> outEvents8b;
                          //std::string observation, relevancy;
                 relevancy="";
                 observation="";
                          auto t8b=std::make_shared<Transition>(transitionName8b, inEvent8b, outEvents8b, relevancy, observation);
                          transitions.push_back(t8b);
                 
         
         //stati
         //s1
         std::string name = "s"+std::to_string(countS);
         countS=countS+1;
         bool starting = true;

         
         std::vector<std::shared_ptr<Transition>> inTransitions, outTransitions;

         inTransitions.push_back(t3);
         inTransitions.push_back(t5);
         inTransitions.push_back(t8b);
         outTransitions.push_back(t1);
         outTransitions.push_back(t5);
         outTransitions.push_back(t7a);

         states.push_back(std::make_shared<State>(name, starting, inTransitions, outTransitions));
         //s2
         name = "s"+std::to_string(countS);
          countS=countS+1;
         starting = false;

         
         std::vector<std::shared_ptr<Transition>> inTransitions2, outTransitions2;

         inTransitions2.push_back(t1);
         outTransitions2.push_back(t2);
         outTransitions2.push_back(t3);

         states.push_back(std::make_shared<State>(name, starting, inTransitions2, outTransitions2));
         //s3
         name = "s"+std::to_string(countS);
                 countS=countS+1;
                 starting = false;

                 
                 std::vector<std::shared_ptr<Transition>> inTransitions3, outTransitions3;

                 inTransitions3.push_back(t2);
                 inTransitions3.push_back(t4);
                 inTransitions3.push_back(t7b);
                 outTransitions3.push_back(t4);
                 outTransitions3.push_back(t8a);
                 

                 states.push_back(std::make_shared<State>(name, starting, inTransitions3, outTransitions3));
         
         
         //s248
                 name = "s248";
                         //countS=countS+1;
                         starting = false;

                         
                         std::vector<std::shared_ptr<Transition>> inTransitions4, outTransitions4;

                         inTransitions4.push_back(t8a);
                         //inTransitions3.push_back(t4);
                         //inTransitions3.push_back(t6);
                         outTransitions4.push_back(t8b);
                        // outTransitions3.push_back(t7);
                         

                         states.push_back(std::make_shared<State>(name, starting, inTransitions4, outTransitions4));
         
         //s247
         name = "s247";
                 //countS=countS+1;
                 starting = false;

                 
                 std::vector<std::shared_ptr<Transition>> inTransitions5, outTransitions5;

                 inTransitions5.push_back(t7a);
                 //inTransitions3.push_back(t4);
                 //inTransitions3.push_back(t6);
                 outTransitions5.push_back(t7b);
                // outTransitions3.push_back(t7);
                 

                 states.push_back(std::make_shared<State>(name, starting, inTransitions5, outTransitions5));
          std::string componentName="C"+ std::to_string(countC);
         countC=countC+1;
         auto component=std::make_shared<Component>(states, transitions, componentName);
         components.push_back(component);
         
         
         auto aaa=std::make_shared<ComponentLinks>(InL,OutL,InR,OutR,component);
         //compLin.push_back();
          std::string e4n="e4";
         return aaa;
     }
    
    
    static void createFinalComponentZ(int &countE,int &countT,int &countL,int &countS,std::vector<std::shared_ptr<Link>> &links,        std::vector<std::shared_ptr<Component>> &components,  std::shared_ptr<std::vector<std::shared_ptr<TransitionEvent>>> &transEvs,           std::shared_ptr<std::vector<std::shared_ptr<ComponentLinks>>> &compLin,std::shared_ptr<ComponentLinks> &lastCompLin){
        auto OutL=lastCompLin->InR;
        auto InL=lastCompLin->OutR;
        std::shared_ptr<Link> InR;
        std::shared_ptr<Link> OutR;
        InR=nullptr;
        OutR=nullptr;
        std::vector<std::shared_ptr<State>> states;
        std::vector<std::shared_ptr<Transition>> transitions;
         
         std::string componentName="IF";
         //TRANSIZIONI E LINK
         
         //t1a
         
         std::vector<std::shared_ptr<Event>> outEvents;
          std::string e0="e0";
         std::string e2="e2";

          std::string e3="e3";
          std::string e4="e4";
          std::shared_ptr<Event> inEvent = std::make_shared<Event>(InL.get(), e0);
          
         
          std::string transitionName = "T"+ std::to_string(countT)+"a";
          
          std::string observation, relevancy;
        observation="";
          auto t1a=std::make_shared<Transition>(transitionName, inEvent, outEvents, relevancy, observation);
          transitions.push_back(t1a);
         
         auto te = TransitionEvent(t1a,e0,"","","");
         
        //t1b
         
         std::vector<std::shared_ptr<Event>> outEventsb;
          
          std::shared_ptr<Event> inEventb = std::make_shared<Event>(InL.get(), e2);
          
         
          std::string transitionNameb = "T"+ std::to_string(countT)+"b";

            countT=countT+1;
          
        observation="o1";
          auto t1b=std::make_shared<Transition>(transitionNameb, inEventb, outEventsb, relevancy, observation);
          transitions.push_back(t1b);
         
         auto te4 = TransitionEvent(t1b,"",e2,"","");
                              
                              
         //t2
          std::string transitionName2 = "T"+ std::to_string(countT);
         
          std::shared_ptr<Event> inEvent2 = std::make_shared<Event>(InL.get(), e4);
          std::vector<std::shared_ptr<Event>> outEvents2;
        observation="o2";

         auto t2=std::make_shared<Transition>(transitionName2, inEvent2, outEvents2, relevancy, observation);
         auto te2 = TransitionEvent(t2,e4,"","","");
         transitions.push_back(t2);
                              
                              
        //STATI
         //s1
         std::string name = "s"+std::to_string(countS);
         countS=countS+1;
         bool starting = true;

         
         std::vector<std::shared_ptr<Transition>> inTransitions, outTransitions;

         inTransitions.push_back(t2);
         outTransitions.push_back(t1a);

         states.push_back(std::make_shared<State>(name, starting, inTransitions, outTransitions));
         
        name = "s"+std::to_string(countS);
         countS=countS+1;
         starting = false;

         
         std::vector<std::shared_ptr<Transition>> inTransitions1, outTransitions1;

         inTransitions1.push_back(t1b);
         outTransitions1.push_back(t2);

         states.push_back(std::make_shared<State>(name, starting, inTransitions1, outTransitions1));
        
        
        
        name = "s56"+std::to_string(countS);
               
                starting = false;

                
                std::vector<std::shared_ptr<Transition>> inTransitions2, outTransitions2;

                inTransitions2.push_back(t1a);
                outTransitions2.push_back(t1b);

                states.push_back(std::make_shared<State>(name, starting, inTransitions2, outTransitions2));
         auto component=std::make_shared<Component>(states, transitions, componentName);
         components.push_back(component);
        
         auto aaa=std::make_shared<ComponentLinks>(InL,OutL,InR,OutR,component);
    }
    
    
    
    //*************************************************************
    //FINE LINEARE UNIDIREZIONALE ZANELLA
    
    
    static std::shared_ptr<ComponentLinks> createInitialComponentLin(int &countE,int &countT,int &countL,int &countS,std::vector<std::shared_ptr<Link>> &links,        std::vector<std::shared_ptr<Component>> &components,  std::shared_ptr<std::vector<std::shared_ptr<TransitionEvent>>> &transEvs,           std::shared_ptr<std::vector<std::shared_ptr<ComponentLinks>>> &compLin){
           std::vector<std::shared_ptr<State>> states;
            
           std::vector<std::shared_ptr<Transition>> transitions;
          // int nTransition=2;
           // int nStates=2;
           // int id=0;
           std::string componentName="IC";
           //TRANSIZIONI E LINK
           
           
           auto L1=std::make_shared<Link>("L"+std::to_string(countL));
           links.push_back(L1);
           countL=countL+1;
        
        
        //TRANSIZIONI
        
        //t1
           std::vector<std::shared_ptr<Event>> outEvents;
            std::string e0="e0";
            std::string e2="e2";
            std::string e4="e4";
            std::shared_ptr<Event> inEvent = nullptr;
            
            std::string en4="e4";
            //auto tt=std::make_shared< Transition>("mm",  &inEvent, &outEvents);
            std::string transitionName = "T"+ std::to_string(countT);
            outEvents.push_back(std::make_shared<Event>(L1.get(), e0));
            std::string observation, relevancy;
           observation="o"+ std::to_string(countT);
           relevancy="";
            auto t1=std::make_shared<Transition>(transitionName, inEvent, outEvents, relevancy, observation);
            transitions.push_back(t1);
           
           auto te = TransitionEvent(t1,"","","",e0);
           
            countT=countT+1;
            std::string transitionName2 = "T"+ std::to_string(countT);
            countT=countT+1;
            //auto L2=std::make_shared<Link>("L"+std::to_string(countL));
           // links.push_back(L2);
          
            //       countL=countL+1;
            std::shared_ptr<Event> inEvent2 =nullptr;
            std::vector<std::shared_ptr<Event>> outEvents2;
            outEvents2.push_back(std::make_shared<Event>(L1.get(), e2));
           observation="o"+ std::to_string(countT);
           auto t2=std::make_shared<Transition>(transitionName2, inEvent2, outEvents2, relevancy, observation);
           //auto te2 = TransitionEvent(t2,"","",e2,e4);
           transitions.push_back(t2);
          //STATI
           
            std::string name = "s"+std::to_string(countS);
                   countS=countS+1;
           bool starting = true;

           
           std::vector<std::shared_ptr<Transition>> inTransitions, outTransitions;

           inTransitions.push_back(t2);
           outTransitions.push_back(t1);

           states.push_back(std::make_shared<State>(name, starting, inTransitions, outTransitions));
           
            name = "s"+std::to_string(countS);
                   countS=countS+1;
           starting = false;

           
           std::vector<std::shared_ptr<Transition>> inTransitions1, outTransitions1;

           inTransitions1.push_back(t1);
           outTransitions1.push_back(t2);

           states.push_back(std::make_shared<State>(name, starting, inTransitions1, outTransitions1));
           auto component=std::make_shared<Component>(states, transitions, componentName);
           components.push_back(component);
           std::shared_ptr<Link> InL;
           std::shared_ptr<Link> OutL;
           std::shared_ptr<Link> InR;
           std::shared_ptr<Link> OutR;
          
           auto aaa=std::make_shared<ComponentLinks>(InL,OutL,InR,L1,component);
           //compLin.push_back();
            std::string e4n="e4";
           return aaa;
        }
    

    static std::shared_ptr<ComponentLinks> createIntermedialComponentLin(int &countE,int &countT,int &countL,int &countS,int &countC,std::vector<std::shared_ptr<Link>> &links,        std::vector<std::shared_ptr<Component>> &components,  std::shared_ptr<std::vector<std::shared_ptr<TransitionEvent>>> &transEvs,           std::shared_ptr<std::vector<std::shared_ptr<ComponentLinks>>> &compLin,std::shared_ptr<ComponentLinks> &lastCompLin){
        auto OutR=std::make_shared<Link>("L"+std::to_string(countL));
        countL=countL+1;
        //auto InR=std::make_shared<Link>("L"+std::to_string(countL));
        countL=countL+1;
        links.push_back(OutR);
       // links.push_back(InR);
        std::vector<std::shared_ptr<State>> states;
         
        std::vector<std::shared_ptr<Transition>> transitions;
        
         std::shared_ptr<Link> OutL=nullptr;
         
         std::shared_ptr<Link> InR=nullptr;
        
         
        auto InL=lastCompLin->OutR;
        std::string e0="e0";
        std::string e2="e2";
        std::string e3="e3";
        std::string e4="e4";
       // transizioni
        //t1
        std::shared_ptr<Event> inEvent = nullptr;
        std::string transitionName = "T"+ std::to_string(countT);
        countT=countT+1;
        std::vector<std::shared_ptr<Event>> outEvents1;
        
       
         std::string observation, relevancy;
        observation="";
        relevancy="";
         auto t1=std::make_shared<Transition>(transitionName, inEvent, outEvents1, relevancy, observation);
         transitions.push_back(t1);
        
        //t2
        std::shared_ptr<Event> inEvent2=std::make_shared<Event>(InL.get(), e0);
        std::string transitionName2 = "T"+ std::to_string(countT);
        countT=countT+1;

         std::vector<std::shared_ptr<Event>> outEvents2;
       // auto aaas= OutR.get();
         outEvents2.push_back(std::make_shared<Event>(OutR.get(), e0));
         //std::string observation, relevancy;
        relevancy="f2";
         auto t2=std::make_shared<Transition>(transitionName2, inEvent2, outEvents2, relevancy, observation);
         transitions.push_back(t2);
        //t3
        std::shared_ptr<Event> inEvent3 = std::make_shared<Event>(InL.get(), e2);
        std::string transitionName3 = "T"+ std::to_string(countT);
        countT=countT+1;

         std::vector<std::shared_ptr<Event>> outEvents3;
         outEvents3.push_back(std::make_shared<Event>(OutR.get(), e2));
         //std::string observation, relevancy;
        // auto t3=std::make_shared<Transition>(transitionName3, inEvent3, outEvents3, relevancy, "o3");
        relevancy="f3";
        observation="o3";
        auto t3=std::make_shared<Transition>(transitionName3, inEvent3, outEvents3, relevancy,observation);

        transitions.push_back(t3);
        //t4
        std::shared_ptr<Event> inEvent4;
        std::string transitionName4 = "T"+ std::to_string(countT);
        countT=countT+1;

         std::vector<std::shared_ptr<Event>> outEvents4;
         
         //std::string observation, relevancy;
        // auto t3=std::make_shared<Transition>(transitionName3, inEvent3, outEvents3, relevancy, "o3");
        relevancy="";
        observation="";
        auto t4=std::make_shared<Transition>(transitionName4, inEvent4, outEvents4, relevancy,observation);
          transitions.push_back(t4);
        //stati
        //s1
        std::string name = "s"+std::to_string(countS);
        countS=countS+1;
        bool starting = true;

        
        std::vector<std::shared_ptr<Transition>> inTransitions, outTransitions;

        inTransitions.push_back(t3);
        inTransitions.push_back(t4);
       
        outTransitions.push_back(t1);
       

        states.push_back(std::make_shared<State>(name, starting, inTransitions, outTransitions));
        //s2
        name = "s"+std::to_string(countS);
         countS=countS+1;
        starting = false;

        
        std::vector<std::shared_ptr<Transition>> inTransitions2, outTransitions2;

        inTransitions2.push_back(t1);
        outTransitions2.push_back(t2);
          outTransitions2.push_back(t4);
        

        states.push_back(std::make_shared<State>(name, starting, inTransitions2, outTransitions2));
        //s3
        name = "s"+std::to_string(countS);
                countS=countS+1;
                starting = false;

                
                std::vector<std::shared_ptr<Transition>> inTransitions3, outTransitions3;

                inTransitions3.push_back(t2);
                
               
                outTransitions3.push_back(t3);
                

                states.push_back(std::make_shared<State>(name, starting, inTransitions3, outTransitions3));
         std::string componentName="C"+ std::to_string(countC);
        countC=countC+1;
        auto component=std::make_shared<Component>(states, transitions, componentName);
        components.push_back(component);
        
        
        auto aaa=std::make_shared<ComponentLinks>(InL,OutL,InR,OutR,component);
        //compLin.push_back();
         std::string e4n="e4";
        return aaa;
    }
    
    
    static void createFinalComponentFinLin(int &countE,int &countT,int &countL,int &countS,std::vector<std::shared_ptr<Link>> &links,        std::vector<std::shared_ptr<Component>> &components,  std::shared_ptr<std::vector<std::shared_ptr<TransitionEvent>>> &transEvs,           std::shared_ptr<std::vector<std::shared_ptr<ComponentLinks>>> &compLin,std::shared_ptr<ComponentLinks> &lastCompLin){
           //auto OutL=lastCompLin->InR;
           auto InL=lastCompLin->OutR;
           std::shared_ptr<Link> InR;
           std::shared_ptr<Link> OutR,OutL;
           InR=nullptr;
           OutR=nullptr;
        OutL=nullptr;
           std::vector<std::shared_ptr<State>> states;
           std::vector<std::shared_ptr<Transition>> transitions;
            
            std::string componentName="IF";
            //TRANSIZIONI E LINK
            
            //t1
            
            std::vector<std::shared_ptr<Event>> outEvents;
             std::string e0="e0";
            std::string e2="e2";

             std::string e3="e3";
             std::string e4="e4";
             std::shared_ptr<Event> inEvent = std::make_shared<Event>(InL.get(), e0);
             
            
             std::string transitionName = "T"+ std::to_string(countT);
             
             std::string observation, relevancy;
           observation="o1";
             auto t1=std::make_shared<Transition>(transitionName, inEvent, outEvents, relevancy, observation);
             transitions.push_back(t1);
            
            auto te = TransitionEvent(t1,e0,e2,"","");
            
             countT=countT+1;
                                 
                                 
            //t2
             std::string transitionName2 = "T"+ std::to_string(countT);
            
             std::shared_ptr<Event> inEvent2 = std::make_shared<Event>(InL.get(), e2);
             std::vector<std::shared_ptr<Event>> outEvents2;
           observation="o2";

            auto t2=std::make_shared<Transition>(transitionName2, inEvent2, outEvents2, relevancy, observation);
            auto te2 = TransitionEvent(t2,e4,"","","");
            transitions.push_back(t2);
                                 
                                 
           //STATI
            //s1
            std::string name = "s"+std::to_string(countS);
            countS=countS+1;
            bool starting = true;

            
            std::vector<std::shared_ptr<Transition>> inTransitions, outTransitions;

            inTransitions.push_back(t2);
            outTransitions.push_back(t1);

            states.push_back(std::make_shared<State>(name, starting, inTransitions, outTransitions));
            
           name = "s"+std::to_string(countS);
            countS=countS+1;
            starting = false;

            
            std::vector<std::shared_ptr<Transition>> inTransitions1, outTransitions1;

            inTransitions1.push_back(t1);
            outTransitions1.push_back(t2);

            states.push_back(std::make_shared<State>(name, starting, inTransitions1, outTransitions1));
            auto component=std::make_shared<Component>(states, transitions, componentName);
            components.push_back(component);
           
            auto aaa=std::make_shared<ComponentLinks>(InL,OutL,InR,OutR,component);
       }
       
    static std::shared_ptr<Network> createNetworkLin(int numberComponent){
          // int nLinks=(numberComponent+1)*2;
           int nEvents=4;
           int countE=0;
           int countL=0;
           int countS=0;
            int countC=0;
           std::shared_ptr<std::vector<std::shared_ptr<TransitionEvent>>> transEvs(new std::vector<std::shared_ptr<TransitionEvent>>());
           std::shared_ptr<std::vector<std::shared_ptr<ComponentLinks>>> compLin(new std::vector<std::shared_ptr<ComponentLinks>>());

           std::vector<std::shared_ptr<Component>> components;
           int countT=0;
           std::vector<std::shared_ptr<Link>> links;
           std::vector<std::string> events;
           for(int i=0; i<nEvents;i++){
               auto e= "e"+std::to_string(i);
               events.push_back(e);
           }
           
           auto lastCompLin=createInitialComponentLin( countE,countT,countL,countS,links,components,transEvs,compLin);
           for (int i=0;  i<numberComponent;i++){
               lastCompLin= createIntermedialComponentLin(countE,countT,countL,countS,countC,links,components, transEvs,compLin,lastCompLin);
               
           }
           createFinalComponentFinLin(countE, countT, countL, countS, links, components, transEvs, compLin, lastCompLin);
          
           auto network=std::make_shared<Network>(components, links);
           std::shared_ptr<SimpleNetwork> observation = nullptr, bond = nullptr;
                  
           std::shared_ptr<long> maxExecutionTime = nullptr;
           long startingMaxExecutionTime = 0;
          

                   auto startingTime = std::chrono::high_resolution_clock::now();
               //auto bs=BSBuilder::build(network, maxExecutionTime, observation, bond);
           return network;
       }

    
    
    
    // ****************************************************************
       
       //stella esplosiva

       
       
       static std::shared_ptr<Network> createNetworkStar(int numberComponent){
          // int nLinks=numberComponent;
           //int nEvents=4;
           //int countE=0;
           //int countL=0;
           int countS=0;
            int countC=0;
           std::shared_ptr<std::vector<std::shared_ptr<TransitionEvent>>> transEvs(new std::vector<std::shared_ptr<TransitionEvent>>());
           std::shared_ptr<std::vector<std::shared_ptr<ComponentLinks>>> compLin(new std::vector<std::shared_ptr<ComponentLinks>>());

           std::vector<std::shared_ptr<Component>> components;
           int countT=0;
           std::vector<std::shared_ptr<Link>> links;
           for ( int i=0; i<numberComponent; i++){
               
               auto L=std::make_shared<Link>("L"+std::to_string(i));
               links.push_back(L);
           }
           
          
           std::vector<std::string> events;
           
           auto e= "op";
           events.push_back(e);
           e= "cl";
           events.push_back(e);
           createCenterComponentStar(links,countT,countS,countC,components);
           for (auto link :links){
               createIntermedialComponentStar(link,  countT, countS,countC,components);
           }
          
          
           auto network=std::make_shared<Network>(components, links);
           std::shared_ptr<SimpleNetwork> observation = nullptr, bond = nullptr;
                  
           std::shared_ptr<long> maxExecutionTime = nullptr;
           long startingMaxExecutionTime = 0;
          

                   auto startingTime = std::chrono::high_resolution_clock::now();
               //auto bs=BSBuilder::build(network, maxExecutionTime, observation, bond);
           return network;
       }
       
       
       
       static void createCenterComponentStar(std::vector<std::shared_ptr<Link>> links,  int &countT, int &countS,int &countC ,     std::vector<std::shared_ptr<Component>> &components){
           
          // links.push_back(InR);
           std::vector<std::shared_ptr<State>> states;
            
           std::vector<std::shared_ptr<Transition>> transitions;
           
            std::vector<std::string> events;
                   
           std::string op="op";
           events.push_back(op);
         std::string cl= "cl";
           events.push_back(cl);
           
            
           
          // transizioni
           //t1
           std::shared_ptr<Event> inEvent;
           std::string transitionName = "T"+ std::to_string(countT);
           countT=countT+1;
           std::vector<std::shared_ptr<Event>> outEvents1;
           
          
            std::string observation, relevancy;
           observation="act";
          
           relevancy="";
           std::vector<std::shared_ptr<Link>> links2;
           
           
           for(auto link: links){
            outEvents1.push_back(std::make_shared<Event>(link.get(), op));
           }
          // outEvents1.push_back(std::make_shared<Event>(lin->get(), op));
            auto t1=std::make_shared<Transition>(transitionName, inEvent, outEvents1, relevancy, observation);
            transitions.push_back(t1);
           
           
           //t2
           std::shared_ptr<Event> inEvent2;
                     transitionName = "T"+ std::to_string(countT);
                     countT=countT+1;
                     std::vector<std::shared_ptr<Event>> outEvents2;
                     
                    
                      
                     observation="act";
                     relevancy="fos";
                     for(auto link: links){
                       outEvents2.push_back(std::make_shared<Event>(link.get(), cl));
                     }
                      auto t2=std::make_shared<Transition>(transitionName, inEvent2, outEvents2, relevancy, observation);
                      transitions.push_back(t2);
                    
                    
                    //t3;
           std::shared_ptr<Event> inEvent3;
                      transitionName = "T"+ std::to_string(countT);
                     countT=countT+1;
                     std::vector<std::shared_ptr<Event>> outEvents3;
                     
                    
                      
                     observation="sby";
                     relevancy="";
                     for(auto link: links){
                       outEvents3.push_back(std::make_shared<Event>(link.get(), cl));
                     }
                      auto t3=std::make_shared<Transition>(transitionName, inEvent3, outEvents3, relevancy, observation);
                      transitions.push_back(t3);
                    
                    //t4
                    std::shared_ptr<Event> inEvent4 = nullptr;
                      transitionName = "T"+ std::to_string(countT);
                     countT=countT+1;
                     std::vector<std::shared_ptr<Event>> outEvents4;
                     
                    
                      
                     observation="sby";
                     relevancy="fcs";
                     for(auto link: links){
                       outEvents4.push_back(std::make_shared<Event>(link.get(), op));
                     }
                      auto t4=std::make_shared<Transition>(transitionName, inEvent4, outEvents4, relevancy, observation);
                      transitions.push_back(t4);
                    
                   
                    //stati
                    //s1
                    std::string name = "s"+std::to_string(countS);
                    countS=countS+1;
                    bool starting = true;

                    
                    std::vector<std::shared_ptr<Transition>> inTransitions, outTransitions;

                    inTransitions.push_back(t3);
                             
                    outTransitions.push_back(t1);
                    outTransitions.push_back(t2);
                   

                    states.push_back(std::make_shared<State>(name, starting, inTransitions, outTransitions));
                    //s2
                    name = "s"+std::to_string(countS);
                     countS=countS+1;
                    starting = false;

                    
                    std::vector<std::shared_ptr<Transition>> inTransitions2, outTransitions2;

                    inTransitions2.push_back(t1);
                    outTransitions2.push_back(t3);
                      outTransitions2.push_back(t4);
                    

                    states.push_back(std::make_shared<State>(name, starting, inTransitions2, outTransitions2));
                    //s3
                    name = "s"+std::to_string(countS);
                            countS=countS+1;
                            starting = false;

                            
                            std::vector<std::shared_ptr<Transition>> inTransitions3, outTransitions3;

                           
                            
                           
                            inTransitions3.push_back(t2);
                     inTransitions3.push_back(t4);
                            

                            states.push_back(std::make_shared<State>(name, starting, inTransitions3, outTransitions3));
                     std::string componentName="S"+ std::to_string(countC);
                    countC=countC+1;
                    auto component=std::make_shared<Component>(states, transitions, componentName);
                    components.push_back(component);
                    
                    
                    //auto aaa=std::make_shared<ComponentLinks>(InL,OutL,InR,OutR,component);
                    //compLin.push_back();
                     std::string e4n="e4";
         
           
       }
    
    static void createIntermedialComponentStar(std::shared_ptr<Link> &link,  int &countT, int &countS, int &countC,     std::vector<std::shared_ptr<Component>> &components){
              
             // links.push_back(InR);
              std::vector<std::shared_ptr<State>> states;
               
              std::vector<std::shared_ptr<Transition>> transitions;
              
               std::vector<std::string> events;
                      
              std::string op= "op";
              events.push_back(op);
              std::string  cl= "cl";
              events.push_back(cl);
              
               
              
             // transizioni
              //t1
              std::shared_ptr<Event> inEvent = std::make_shared<Event>(link.get(), op);
              std::string transitionName = "Tb"+ std::to_string(countT);
              countT=countT+1;
              std::vector<std::shared_ptr<Event>> outEvents1;
              
             
               std::string observation, relevancy;
              observation="opn";
              relevancy="";
             
             
               auto t1=std::make_shared<Transition>(transitionName, inEvent, outEvents1, relevancy, observation);
               transitions.push_back(t1);
              
            //t2
              auto inEvent2 = std::make_shared<Event>(link.get(), op);
                transitionName = "Tb"+ std::to_string(countT);
               countT=countT+1;
               std::vector<std::shared_ptr<Event>> outEvents2;
               
             
               observation="";
               relevancy="fob";
              
              
                auto t2=std::make_shared<Transition>(transitionName, inEvent2, outEvents2, relevancy, observation);
                transitions.push_back(t2);
        
             //t3
              std::shared_ptr<Event> inEvent3 = std::make_shared<Event>(link.get(), cl);
             transitionName = "Tb"+ std::to_string(countT);
              countT=countT+1;
              std::vector<std::shared_ptr<Event>> outEvents3;
              
             
              observation="";
              relevancy="";
             
             
               auto t3=std::make_shared<Transition>(transitionName, inEvent3, outEvents3, relevancy, observation);
               transitions.push_back(t3);
        
                //t4
              std::shared_ptr<Event> inEvent4 = std::make_shared<Event>(link.get(), cl);
               transitionName = "Tb"+ std::to_string(countT);
              countT=countT+1;
              std::vector<std::shared_ptr<Event>> outEvents4;
             
              observation="cls";
              relevancy="";
             
             
               auto t4=std::make_shared<Transition>(transitionName, inEvent4, outEvents4, relevancy, observation);
               transitions.push_back(t4);
        
        //t5
              std::shared_ptr<Event> inEvent5 = std::make_shared<Event>(link.get(), cl);
              transitionName = "Tb"+ std::to_string(countT);
              countT=countT+1;
              std::vector<std::shared_ptr<Event>> outEvents5;
            
              observation="";
              relevancy="fcb";
             
             
               auto t5=std::make_shared<Transition>(transitionName, inEvent5, outEvents5, relevancy, observation);
               transitions.push_back(t5);
        
        //t6
              std::shared_ptr<Event> inEvent6 = std::make_shared<Event>(link.get(), op);
              transitionName = "Tb"+ std::to_string(countT);
              countT=countT+1;
              std::vector<std::shared_ptr<Event>> outEvents6;
          
              observation="";
              relevancy="";
               auto t6=std::make_shared<Transition>(transitionName, inEvent6, outEvents6, relevancy, observation);
               transitions.push_back(t6);
              
              
             
              //stati
              //s1
              std::string name = "s"+std::to_string(countS);
              countS=countS+1;
              bool starting = true;

              
              std::vector<std::shared_ptr<Transition>> inTransitions, outTransitions;

              inTransitions.push_back(t2);
             inTransitions.push_back(t3);
              inTransitions.push_back(t4);
              
                    outTransitions.push_back(t1);
                    outTransitions.push_back(t2);
                  outTransitions.push_back(t3);
                    
             

              states.push_back(std::make_shared<State>(name, starting, inTransitions, outTransitions));
              //s2
              name = "s"+std::to_string(countS);
               countS=countS+1;
              starting = false;

              
              std::vector<std::shared_ptr<Transition>> inTransitions2, outTransitions2;

              inTransitions2.push_back(t1);
              inTransitions2.push_back(t5);
              inTransitions2.push_back(t6);
        
        
              outTransitions2.push_back(t4);
              outTransitions2.push_back(t5);
              outTransitions2.push_back(t6);
              

              states.push_back(std::make_shared<State>(name, starting, inTransitions2, outTransitions2));
           
               std::string componentName="S"+ std::to_string(countC);
              countC=countC+1;
              auto component=std::make_shared<Component>(states, transitions, componentName);
              components.push_back(component);
              
              
              //auto aaa=std::make_shared<ComponentLinks>(InL,OutL,InR,OutR,component);
              //compLin.push_back();
               std::string e4n="e4";
              
          }

    
    
    // ***** stella implosiva
    
     static void createCenterComponentStarI(std::vector<std::shared_ptr<Link>> links,  int &countT, int &countS,int &countC ,     std::vector<std::shared_ptr<Component>> &components){
        
       // links.push_back(InR);
        std::vector<std::shared_ptr<State>> states;
         
        std::vector<std::shared_ptr<Transition>> transitions;
        
         std::vector<std::string> events;
                
        std::string op="op";
        events.push_back(op);
      std::string cl= "cl";
        events.push_back(cl);
        
         
        
       // transizioni
        //t1
         std::shared_ptr<Transition>t1,t2,t3,t4;
         std::vector<std::shared_ptr<Transition>> inTransitions1, outTransitions1, inTransitions2, outTransitions2,inTransitions3, outTransitions3;
         for(auto link: links){
        std::shared_ptr<Event> inEvent;
        std::string transitionName = "T"+ std::to_string(countT);
        countT=countT+1;
        std::vector<std::shared_ptr<Event>> outEvents1;
        
       
         std::string observation, relevancy;
        observation="act";
       
        relevancy="";
        std::vector<std::shared_ptr<Link>> links2;
        
        inEvent=std::make_shared<Event>(link.get(), op);
        
       // outEvents1.push_back(std::make_shared<Event>(lin->get(), op));
          t1=std::make_shared<Transition>(transitionName, inEvent, outEvents1, relevancy, observation);
         transitions.push_back(t1);
        
        
        //t2
        std::shared_ptr<Event> inEvent2;
                  transitionName = "T"+ std::to_string(countT);
                  countT=countT+1;
                  std::vector<std::shared_ptr<Event>> outEvents2;
                  
                 
                   
                  observation="act";
                  relevancy="fos";
                  inEvent2= std::make_shared<Event>(link.get(), cl);
                  
                    t2=std::make_shared<Transition>(transitionName, inEvent2, outEvents2, relevancy, observation);
                   transitions.push_back(t2);
                 
                 
                 //t3;
                std::shared_ptr<Event> inEvent3;
                   transitionName = "T"+ std::to_string(countT);
                  countT=countT+1;
                  std::vector<std::shared_ptr<Event>> outEvents3;
                  
                 
                   
                  observation="sby";
                  relevancy="";
                  
                    inEvent3= std::make_shared<Event>(link.get(), cl);
                    t3=std::make_shared<Transition>(transitionName, inEvent3, outEvents3, relevancy, observation);
                   transitions.push_back(t3);
                 
                 //t4
                 std::shared_ptr<Event> inEvent4 ;
                   transitionName = "T"+ std::to_string(countT);
                  countT=countT+1;
                  std::vector<std::shared_ptr<Event>> outEvents4;
                  
                 
                   
                  observation="sby";
                  relevancy="fcs";
                  
                    inEvent4 =std::make_shared<Event>(link.get(), op);
                 
                    t4=std::make_shared<Transition>(transitionName, inEvent4, outEvents4, relevancy, observation);
                   transitions.push_back(t4);
             
             

             inTransitions1.push_back(t3);
                      
             outTransitions1.push_back(t1);
             outTransitions1.push_back(t2);
             inTransitions2.push_back(t1);
                             outTransitions2.push_back(t3);
             outTransitions2.push_back(t4);

                    inTransitions3.push_back(t2);
             inTransitions3.push_back(t4);
         }
                
                 //stati
                 //s1
                 std::string name = "s"+std::to_string(countS);
                 countS=countS+1;
                 bool starting = true;

                 
                

                

                 states.push_back(std::make_shared<State>(name, starting, inTransitions1, outTransitions1));
                 //s2
                 name = "s"+std::to_string(countS);
                  countS=countS+1;
                 starting = false;


                 states.push_back(std::make_shared<State>(name, starting, inTransitions2, outTransitions2));
                 //s3
                 name = "s"+std::to_string(countS);
                         countS=countS+1;
                         starting = false;

                         
                        

                        
                         
                        
                         

                         states.push_back(std::make_shared<State>(name, starting, inTransitions3, outTransitions3));
                  std::string componentName="S"+ std::to_string(countC);
                 countC=countC+1;
                 auto component=std::make_shared<Component>(states, transitions, componentName);
                 components.push_back(component);
                 
                 
                 //auto aaa=std::make_shared<ComponentLinks>(InL,OutL,InR,OutR,component);
                 //compLin.push_back();
                  std::string e4n="e4";
      
        
    }
    
    
    
    
    static void createIntermedialComponentStarI(std::shared_ptr<Link> &link,  int &countT, int &countS, int &countC,     std::vector<std::shared_ptr<Component>> &components){
          
         // links.push_back(InR);
          std::vector<std::shared_ptr<State>> states;
           
          std::vector<std::shared_ptr<Transition>> transitions;
          
           std::vector<std::string> events;
                  
          std::string op= "op";
          events.push_back(op);
          std::string  cl= "cl";
          events.push_back(cl);
          
           
          
         // transizioni
          //t1
          std::shared_ptr<Event> inEvent;
          std::string transitionName = "Tb"+ std::to_string(countT);
          countT=countT+1;
          std::vector<std::shared_ptr<Event>> outEvents1;
          
           outEvents1.push_back(  std::make_shared<Event>(link.get(), op));
           std::string observation, relevancy;
          observation="opn";
          relevancy="";
         
         
           auto t1=std::make_shared<Transition>(transitionName, inEvent, outEvents1, relevancy, observation);
           transitions.push_back(t1);
          
        //t2
          std::shared_ptr<Event>  inEvent2 ;
            transitionName = "Tb"+ std::to_string(countT);
           countT=countT+1;
           std::vector<std::shared_ptr<Event>> outEvents2;
           
         
           observation="";
           relevancy="fob";
           outEvents2.push_back(  std::make_shared<Event>(link.get(), op));
                     
          
            auto t2=std::make_shared<Transition>(transitionName, inEvent2, outEvents2, relevancy, observation);
            transitions.push_back(t2);
    
         //t3
          std::shared_ptr<Event> inEvent3 ;
         transitionName = "Tb"+ std::to_string(countT);
          countT=countT+1;
          std::vector<std::shared_ptr<Event>> outEvents3;
          
                               outEvents3.push_back( std::make_shared<Event>(link.get(), cl));
          observation="";
          relevancy="";
         
         
           auto t3=std::make_shared<Transition>(transitionName, inEvent3, outEvents3, relevancy, observation);
           transitions.push_back(t3);
    
            //t4
          std::shared_ptr<Event> inEvent4 ;
           transitionName = "Tb"+ std::to_string(countT);
          countT=countT+1;
          std::vector<std::shared_ptr<Event>> outEvents4;
         
          observation="cls";
          relevancy="";
         
        outEvents4.push_back(std::make_shared<Event>(link.get(), cl));
           auto t4=std::make_shared<Transition>(transitionName, inEvent4, outEvents4, relevancy, observation);
           transitions.push_back(t4);
    
    //t5
          std::shared_ptr<Event> inEvent5 ;
          transitionName = "Tb"+ std::to_string(countT);
          countT=countT+1;
          std::vector<std::shared_ptr<Event>> outEvents5;
        
          observation="";
        outEvents5.push_back( std::make_shared<Event>(link.get(), cl));
          relevancy="fcb";
         
         
           auto t5=std::make_shared<Transition>(transitionName, inEvent5, outEvents5, relevancy, observation);
           transitions.push_back(t5);
    
    //t6
          std::shared_ptr<Event> inEvent6 ;
          transitionName = "Tb"+ std::to_string(countT);
          countT=countT+1;
          std::vector<std::shared_ptr<Event>> outEvents6;
      
          observation="";
          relevancy="";
          outEvents6.push_back( std::make_shared<Event>(link.get(), op));
           auto t6=std::make_shared<Transition>(transitionName, inEvent6, outEvents6, relevancy, observation);
           transitions.push_back(t6);
          
          
         
          //stati
          //s1
          std::string name = "s"+std::to_string(countS);
          countS=countS+1;
          bool starting = true;

          
          std::vector<std::shared_ptr<Transition>> inTransitions, outTransitions;

          inTransitions.push_back(t2);
         inTransitions.push_back(t3);
          inTransitions.push_back(t4);
          
                outTransitions.push_back(t1);
                outTransitions.push_back(t2);
              outTransitions.push_back(t3);
                
         

          states.push_back(std::make_shared<State>(name, starting, inTransitions, outTransitions));
          //s2
          name = "s"+std::to_string(countS);
           countS=countS+1;
          starting = false;

          
          std::vector<std::shared_ptr<Transition>> inTransitions2, outTransitions2;

          inTransitions2.push_back(t1);
          inTransitions2.push_back(t5);
          inTransitions2.push_back(t6);
    
    
          outTransitions2.push_back(t4);
          outTransitions2.push_back(t5);
          outTransitions2.push_back(t6);
          

          states.push_back(std::make_shared<State>(name, starting, inTransitions2, outTransitions2));
       
           std::string componentName="S"+ std::to_string(countC);
          countC=countC+1;
          auto component=std::make_shared<Component>(states, transitions, componentName);
          components.push_back(component);
          
          
          //auto aaa=std::make_shared<ComponentLinks>(InL,OutL,InR,OutR,component);
          //compLin.push_back();
           std::string e4n="e4";
          
      }
    
    
    static std::shared_ptr<Network> createNetworkStarI(int numberComponent){
             // int nLinks=numberComponent;
              //int nEvents=4;
              //int countE=0;
              //int countL=0;
              int countS=0;
               int countC=0;
              std::shared_ptr<std::vector<std::shared_ptr<TransitionEvent>>> transEvs(new std::vector<std::shared_ptr<TransitionEvent>>());
              std::shared_ptr<std::vector<std::shared_ptr<ComponentLinks>>> compLin(new std::vector<std::shared_ptr<ComponentLinks>>());

              std::vector<std::shared_ptr<Component>> components;
              int countT=0;
              std::vector<std::shared_ptr<Link>> links;
              for ( int i=0; i<numberComponent; i++){
                  
                  auto L=std::make_shared<Link>("L"+std::to_string(i));
                  links.push_back(L);
              }
              
             
              std::vector<std::string> events;
              
              auto e= "op";
              events.push_back(e);
              e= "cl";
              events.push_back(e);
              createCenterComponentStarI(links,countT,countS,countC,components);
              for (auto link :links){
                  createIntermedialComponentStarI(link,  countT, countS,countC,components);
              }
             
             
              auto network=std::make_shared<Network>(components, links);
              std::shared_ptr<SimpleNetwork> observation = nullptr, bond = nullptr;
                     
              std::shared_ptr<long> maxExecutionTime = nullptr;
              long startingMaxExecutionTime = 0;
             

                      auto startingTime = std::chrono::high_resolution_clock::now();
                  //auto bs=BSBuilder::build(network, maxExecutionTime, observation, bond);
              return network;
          }
          
          
    
    
    
    // ********fine stella implosiva
    
    
    static std::vector<std::string> generateRandomObs(std::shared_ptr<Network> network){
   
        
        // Iterator to the beginning of set
        /*
        std::set<std::string>::iterator iter = alphabet.begin();
        std::vector<std::string> obs;
        int r = (rand() % 10);
        for (int i=0; i<r; i++){
            auto l= alphabet.size()-1;
            int r1 = (rand() % l);
            std::advance(iter, r1);
            obs.push_back(*iter);
            iter = alphabet.begin();
            
        }
        return obs;
         */
        int numOBS=0;
       
        std::shared_ptr<SimpleNetwork> observation;
        std::shared_ptr<SimpleNetwork> bond;
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);

        /* using nano-seconds instead of seconds */
        srand((time_t)ts.tv_nsec);
       //  srand ( time(NULL) );
        int r = (rand() % 50);
        std::vector<std::string> obs;

            Logger::section("Behavioral Space Construction");
            std::string filters;
            if (observation) filters += "Observation ";
            if (bond) filters += "Bond";

            if (filters.empty()) Logger::log("No filters.");
            else Logger::log("Selected filters: " + filters);

            auto startingTime = std::chrono::high_resolution_clock::now();
            // resetta la rete allo stato iniziale
            network->reset();
            BehavioralState::nextId = 0;
            BehavioralTransition::nextId = 0;


            // acquisisce lo stato (iniziale) della rete
            std::map<std::string, std::string> componentsState = network->getComponentsState();
            std::map<std::string, std::string> linksState = network->getLinksState();

            if (observation) observation->resetState();
            if (bond) bond->resetState();

            std::shared_ptr<BehavioralState> currentState = std::make_shared<BehavioralState>(componentsState, linksState, true, true);
            if (observation) currentState->observationState = observation->currentState;
            if (bond) currentState->bondState = bond->currentState;
 
            // inserisce lo stato iniziale nel vettore e nell'automa
            std::queue<std::shared_ptr<BehavioralState>> toVisit;
            auto space = std::make_shared<BehavioralSpace>();
            toVisit.push(currentState);
            space->states.push_back(currentState);

            bool alreadyPresent;

            while (!toVisit.empty() and numOBS!=r) {
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-startingTime).count();

                
                // prende il primo stato del vettore
                currentState = toVisit.front();
                network->setState(currentState->componentsState, currentState->linksState);
                if (observation) observation->currentState = currentState->observationState;
                if (bond) bond->currentState = currentState->bondState;
                std::vector<std::shared_ptr<Transition>> qt;
                std::vector<int> comp;
                int cont=0;
                for (const auto &component : network->components) {
                    // se una delle transizioni è abilitata la fa scattare
                    std::vector<std::shared_ptr<Transition>> qualifiedTransitions = component->qualifiedTransitions(
                            observation ? observation->getQualifiedLabels() : nullptr,
                            bond ? bond->getQualifiedLabels() : nullptr);
                    
                    if(qualifiedTransitions.size()>0 ){
                        qt.insert(qt.end(),qualifiedTransitions.begin(),qualifiedTransitions.end());
                         
                    comp.push_back(qualifiedTransitions.size()-1+ cont);
                        cont=qt.size();
                    
                       }
                    else{
                        comp.push_back(-1);
                    }
                    network->setState(currentState->componentsState, currentState->linksState);
                }
                    
                    if (qt.size()>0){
                        int l= qt.size();
                       struct timespec ts;
                        clock_gettime(CLOCK_MONOTONIC, &ts);

                        /* using nano-seconds instead of seconds */
                        srand((time_t)ts.tv_nsec);
                        int r1 = (rand() % l);
                        if (r1>=l) r1=l-1;
                        std::vector<std::shared_ptr<Transition>>:: iterator transition1= qt.begin()+r1;
                        auto transition=*transition1;
                        int indComp =-1;
                        for( auto int i=0; i<comp.size();i++){
                            if (i==0){
                                if (r1>=0 && r1<= comp[i] && comp[i]!=-1){
                                    
                                    indComp=i;
                                    break;
                                }
                            }
                            else {
                                if (r1>comp[i-1] && r1<= comp[i] && comp[i]!=-1){
                                indComp=i;
                                break;
                                }
                                
                            }
                            
                        }
                        const auto &componentit=network->components.begin()+indComp;
                        auto component=*componentit;
                        alreadyPresent = false;

                        // acquisisce lo stato così raggiunto dalla rete
                        component->performTransition(transition);
                        componentsState = network->getComponentsState();
                        linksState = network->getLinksState();
                    

                        if (observation) observation->performTransition(transition->observabilityLabel);
                        if (bond) bond->performTransition(transition->name);

                        // crea una nuova transizione dello spazio comportamentale e la aggiunge come transizione in uscita allo
                        // stato corrente e all'automa
                        auto newTransition = std::make_shared<BehavioralTransition>(transition->name,
                                                                                    transition->relevancyLabel,
                                                                                   transition->observabilityLabel);

                        space->transitions.push_back(newTransition);
                    if (transition->observabilityLabel != ""){
                        obs.push_back(transition->observabilityLabel);
                        numOBS=numOBS+1;
                    }
                        currentState->out.push_back(newTransition);

                        // se lo stato raggiunto è già nel grafo aggiunge la transizione a quelle in ingresso
                        for (const auto &state : space->states) {
                            bool condition = state->componentsState == componentsState && state->linksState == linksState;
                            if (observation) condition &= state->observationState == observation->currentState;
                            if (bond) condition &= state->bondState == bond->currentState;
                            if (condition) {
                                alreadyPresent = true;
                                toVisit.push(state);
                                state->in.push_back(newTransition);
                                break;
                            }
                        }

                        // se lo stato raggiunto non è già presente lo crea, vi aggiunge la transizione in ingresso e lo aggiunge
                        // al vettore e all'automa
                        if (!alreadyPresent) {
                            bool final = true;
                            for (auto linkState : linksState)
                                if (linkState.second != "empty")
                                    final = false;
                            if (observation && !observation->reachedFinalState()) final = false;
                            auto newState = std::make_shared<BehavioralState>(componentsState, linksState, false, final);
                            newState->in.push_back(newTransition);
                            if (observation) newState->observationState = observation->currentState;
                            if (bond) newState->bondState = bond->currentState;
                            toVisit.push(newState);
                            space->states.push_back(newState);
                        }

                       //2  torna allo stato precedente prima di controllare il componente successivo
                        
                        //network->setState(currentState->componentsState, currentState->linksState);
                       // if (observation) observation->currentState = currentState->observationState;
                       //if (bond) bond->currentState = currentState->bondState;
                         
                    
                    
                }
                // elimina lo stato corrente dal vettore, da questo momento ad esso si possono solo aggiungere nuove transizioni
                // in ingresso
                toVisit.pop();
            }
            int nFinal=0;
            std::pair<int, int> dimensions = space->getDimensions();
            for (auto s :space->states){
                if( s->final){
                    nFinal++;
                }
            }
            std::vector<std::string> messages = {
                "BS construction completed.",
                "  States: " + std::to_string(dimensions.first),
                 " Final States: "+ std::to_string(nFinal),
                "  Transitions: " + std::to_string(dimensions.second)
            };
            std::string info="BS construction completed.\n  States: " + std::to_string(dimensions.first)+"\n Final States: "+ std::to_string(nFinal)+
        " \n Transitions: " + std::to_string(dimensions.second)+"\n";
             std::cout << info;
            Logger::log(messages);


            return obs;
        }
       
        
    
    
};
