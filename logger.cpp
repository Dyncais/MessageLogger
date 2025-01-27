#include "logger.h"
#include <iomanip>
#include <stdexcept>
#include <sstream>
#include <filesystem> 

std::string Logger::levelToString(LevelMessage level) {
    switch (level) {
        case LevelMessage::LOW: return "LOW";
        case LevelMessage::MIDDLE: return "MIDDLE";
        case LevelMessage::HIGH: return "HIGH";
        default: return "UNKNOWN";
    }
}

std::string Logger::timeToString(std::chrono::system_clock::time_point t) {  // chrono к string, чтоб хранить в файле
  std::time_t time = std::chrono::system_clock::to_time_t(t);
  std::string time_str = std::ctime(&time);
  time_str.resize(time_str.size() - 1);
  return time_str;
}


Logger::Logger(const std::string& filename, LevelMessage defaultLevel)
    : currentFilter(defaultLevel) {
    bool fileExists = std::filesystem::exists(filename); 
    
    logFile.open(filename, std::ios::app);
    if (!logFile.is_open()) {
        throw std::runtime_error("Failed to open log file: " + filename);
    }

    if (fileExists) {
        logFile << "[" << timeToString(std::chrono::system_clock::now()) //если именованный файл с логами уже есть, то открывается, иначе создается.
                << "] Logger opened with default level: " << levelToString(currentFilter) << "\n";
    } else {
        logFile << "[" << timeToString(std::chrono::system_clock::now())
                << "] Logger created with default level: " << levelToString(currentFilter) << "\n";
    }
}

Logger::~Logger() {   // для безопасного закрытия файла, чтоб не блокировался 
    if (logFile.is_open()) {
        logFile << "[" << timeToString(std::chrono::system_clock::now()) << "] Logger closed.\n";
        logFile.close();
    }
}

void Logger::log(const std::string& message, LevelMessage level, std::chrono::system_clock::time_point timestamp) { // отправка сообщений

    if (level < currentFilter) {
        return; 
    }

    logFile << "[" << timeToString(timestamp) << "] "
            << "[" << levelToString(level) << "] "
            << message << "\n";
            
    if (!logFile)
        throw std::runtime_error("Failed to write to log file.");
}

void Logger::setLogLevel(LevelMessage newLevel) {
    currentFilter = newLevel;

    logFile << "[" << timeToString(std::chrono::system_clock::now())
            << "] Default log level changed to: " << levelToString(currentFilter) << "\n";
}


