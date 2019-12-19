/*
#ifndef AUTOMATAMANAGER_LOADDICTIONARY_H
#define AUTOMATAMANAGER_LOADDICTIONARY_H

#include "../stdIncludes.h"
#include <nlohmann/json.hpp>
#include "../Data/Dictionary.h"
#include "../Data/BehavioralSpace.h"
#include "../Data/BehavioralTransition.h"

class AbduceIO {
public:

    static void save(const std::string &path,  std::shared_ptr<std::vector<std::shared_ptr<std::vector<std::shared_ptr<temporalAbduction>>>>> newAb,std::vector<std::vector<std::string>> &observation) {
        nlohmann::json jabduce;
        int count=0;
        for(auto ab:*newAb) {
            int cc = 0;
            auto osservazione=observation[count];
            nlohmann::json jels;
            for (const auto t : *ab) {
                nlohmann::json jel;
                nlohmann::json jdiagnosi;
                nlohmann::json jobs;
                std::string st = "";

                int conta = 1;
                for (const auto di : t->diagn) {

                    int contatore = 1;
                    std::string sep = "";
                    for (const auto d : di) {
                        if (contatore < di.size()) {
                            sep = " ";

                        } else {
                            sep = "";
                        }
                        st = st + d + sep;
                        contatore++;
                        std::cout << "\"" << d << ",\"";

                    }
                    if (conta < t->diagn.size()) {
                        st = st + ", ";
                    }
                    std::cout << "}";
                    conta++;

                }
                if (cc > 1) {
                    jobs["obs"] = osservazione[conta - 1];
                    jel.push_back(jobs);
                    jdiagnosi["diagnosi"] = st;
                    jel.push_back(jdiagnosi);
                    jels.push_back(jel);
                }
                cc++;


                nlohmann::json jTemporalAbudce;
                jTemporalAbudce["elements"] = jels;


                nlohmann::json j;


        count++;
        }

        }
        nlohmann::json j;
        j["abduces"] = jabduce;
        if (path.empty()) std::cout << std::setw(2) << j << std::endl;
        else {
            std::ofstream o(path,std::ios::app);
            o << std::setw(2) << j << std::endl;
        }



    }
};

#endif //AUTOMATAMANAGER_LOADDICTIONARY_H
*/
