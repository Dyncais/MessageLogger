#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <chrono>

enum class LevelMessage {
    LOW,
    MIDDLE,
    HIGH
};

class Logger {

private:
    std::ofstream logFile;
    LevelMessage currentFilter; //уровень не ниже которого должны быть сообщения

    std::string levelToString(LevelMessage level);
    std::string timeToString(std::chrono::system_clock::time_point time);
    
public:
    Logger(const std::string& filename, LevelMessage defaultLevel = LevelMessage::LOW);

    void log(const std::string& message, LevelMessage level = LevelMessage::LOW, std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now());

    void setLogLevel(LevelMessage newLevel);

    ~Logger();
};

#endif 