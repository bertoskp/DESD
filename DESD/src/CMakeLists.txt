cmake_minimum_required(VERSION 2.6)
set(CMAKE_CXX_STANDARD 17)

INCLUDE_DIRECTORIES(../include)
add_executable(am
        main.cpp
        manipulators/BSBuilder.cpp
        manipulators/BSBuilder.h
        manipulators/BSBuilderDFA.cpp
        manipulators/BSBuilderDFA.h
        Data/Link.cpp
        Data/Link.h
        stdIncludes.h
        Data/Event.cpp
        Data/Event.h
        Data/Transition.cpp
        Data/Transition.h
        Data/State.cpp
        Data/State.h
        Data/Component.cpp
        Data/Component.h
        Data/Network.cpp
        Data/Network.h
        Data/BehavioralTransition.cpp
        Data/BehavioralTransition.h
        Data/BehavioralState.cpp
        Data/BehavioralState.h
        Data/BehavioralSpace.cpp
        Data/BehavioralSpace.h
        manipulators/LabeledBSBuilder.cpp
        manipulators/LabeledBSBuilder.h
        manipulators/DFAConstructor.h
        Data/DictionaryState.cpp
        Data/DictionaryState.h
        Data/Dictionary.cpp
        Data/Dictionary.h
        manipulators/DFAConstructor.cpp
        Utils/DictionaryIO.h
        Utils/BehavioralSpaceIO.h
        Data/SimpleState.cpp
        Data/SimpleState.h
        Data/SimpleTransition.cpp
        Data/SimpleTransition.h
        Data/SimpleNetwork.cpp
        Data/SimpleNetwork.h
        Data/NFAStateInterface.h
        Data/NFATransitionInterface.h
        Data/NFASpaceInterface.h
        manipulators/DictionaryMerge.cpp
        manipulators/DictionaryMerge.h
        Utils/SimpleNetworkIO.h
        Utils/NetworkIO.h
        Utils/Logger.cpp
        Utils/Logger.h Data/Observation.cpp Data/Observation.h manipulators/DFA_Abd.cpp)


target_link_libraries(am PUBLIC -g -fsanitize=address -fno-omit-frame-pointer)
