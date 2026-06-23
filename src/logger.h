#pragma once
#include <iostream>
#include <string>
#include <ctime>

class Logger
{
public:
    static void log(const std::string &level, const std::string &message)
    {
        std::time_t now = std::time(nullptr);
        char buf[20];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        std::cout << "[" << buf << "] [" << level << "] " << message << std::endl;
    }
    static void info(const std::string &msg) { log("INFO", msg); }
    static void warn(const std::string &msg) { log("WARN", msg); }
    static void error(const std::string &msg) { log("ERROR", msg); }
};
