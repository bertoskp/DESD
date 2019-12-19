//
// Created by alessandro on 8/1/18.
//

#ifndef AUTOMATAMANAGER_DICTIONARYMERGE_H
#define AUTOMATAMANAGER_DICTIONARYMERGE_H

#include "../stdIncludes.h"
#include "../Data/Dictionary.h"
#include "../Utils/Logger.h"

class DictionaryMerge {
public:
    static std::shared_ptr<Dictionary> merge(std::vector<std::shared_ptr<Dictionary>> dictionaries,
                                             std::shared_ptr<long> maxExecTime);
};


#endif //AUTOMATAMANAGER_DICTIONARYMERGE_H
