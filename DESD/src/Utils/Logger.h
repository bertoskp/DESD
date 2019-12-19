

#ifndef AUTOMATAMANAGER_LOGGER_H
#define AUTOMATAMANAGER_LOGGER_H

#include "../stdIncludes.h"

class Logger {
public:
    static std::string path;

    static void log(std::string message);

    static void log(std::vector<std::string> messages);

    static void section(std::string title);

    static std::string getTag(std::string tag);

    static void start();

    static void end();

    static void err(std::string message);

    static void err(std::vector<std::string> messages);

};


#endif //AUTOMATAMANAGER_LOGGER_H
