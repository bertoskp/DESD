

#include "Logger.h"
#include <fstream>

std::string Logger::path = "log.txt";

void Logger::log(std::string message) {
    std::ofstream out;
    out.open(path, std::ios::app);
    out << getTag("LOG") <<  message << std::endl;
    out.close();
}

void Logger::log(std::vector<std::string> messages) {
    for (const auto& m : messages) log(m);
}

void Logger::section(std::string title) {
    std::ofstream out;
    out.open(path, std::ios::app);
    out << getTag("SEC") << "~~~~~~~~~~" << title << std::endl ;
    out.close();
}

std::string Logger::getTag(std::string tag) {
    time_t now;
    time(&now);
    char buf[sizeof "2011-10-08T07:07:09Z"];
    strftime(buf, sizeof buf, "%FT%TZ", gmtime(&now));
    return "[" + tag + " " + buf + "] ";
}

void Logger::start() {
    std::ofstream out;
    out.open(path, std::ios::app);
    out << "---------------------------------- START ----------------------------------\n" ;
    out.close();
}

void Logger::end() {
    std::ofstream out;
    out.open(path, std::ios::app);
    out << "--------------------------------------------------------------------\n\n\n" ;
    out.close();
}

void Logger::err(std::string message) {
    std::ofstream out;
    out.open(path, std::ios::app);
    out << getTag("ERR") <<  message << std::endl;
    out.close();
}

void Logger::err(std::vector<std::string> messages) {
    for (const auto& m : messages) err(m);
}

