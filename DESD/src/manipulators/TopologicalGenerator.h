#include "../stdIncludes.h"
#include "../Data/Network.h"
#include "../Data/Transition.h"

class TopologicalGenerator{
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
   //static std::shared_ptr<Network> createNetwork(int numberComponent);
    
    static void createNetwork(int numberComponent);
    static void createInitialComponent(int &countE,int &countT,int &countL,std::vector<std::shared_ptr<Link>> &links);
    

};
