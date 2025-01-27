#include "logger.h"
#include <iostream>
#include <string>
#include <sstream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>

void printHelp(const std::string& programName) {
    std::cout << "Usage: " << programName << " <log_file_name> <default_log_level>\n\n";
    std::cout << "Options:\n";
    std::cout << "  --help               Show this help message and exit.\n\n";
    std::cout << "Commands during runtime:\n";
    std::cout << "  exit                 Stop the application.\n";
    std::cout << "  loglevel <level>     Change the default log level (LOW, MIDDLE, HIGH).\n";
    std::cout << "  mssetlevel <level>   Change the default message level (LOW, MIDDLE, HIGH).\n\n";
    std::cout << "Input format for logs:\n";
    std::cout << "  <message> [<level>]  Log a message with an optional level (default is the current level).\n";
}

struct LogMessage {
    std::string text;
    LevelMessage level;
    std::chrono::system_clock::time_point timestamp;
};

std::queue<LogMessage> messageQueue; //очередь сообщений
std::mutex queueMutex;
std::condition_variable queueCondition;
std::atomic<bool> running{true};

void logWorker(Logger& logger) {
    while (running || !messageQueue.empty()) {
        std::unique_lock<std::mutex> lock(queueMutex);
        queueCondition.wait(lock, [] { return !messageQueue.empty() || !running; }); //ожидание пока не будет события - отправка сообщения

        while (!messageQueue.empty()) {
            LogMessage log = messageQueue.front();
            messageQueue.pop();
            lock.unlock();

            logger.log(log.text, log.level, log.timestamp);

            lock.lock();
        }
    }
}

LevelMessage parseLogLevel(const std::string& input) {
    if (input == "LOW") return LevelMessage::LOW;
    if (input == "MIDDLE") return LevelMessage::MIDDLE;
    if (input == "HIGH") return LevelMessage::HIGH;
    throw std::invalid_argument("Invalid log level");
}

int main(int argc, char* argv[]) {

    if (argc == 2 && std::string(argv[1]) == "--help") {
        printHelp(argv[0]);
        return 0;
    }

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <log_file_name> <default_log_level>\n";
        return 1;
    }

    std::string logFileName = argv[1];

     if (logFileName.empty())
    {
        std::cerr << "Must not be empty filename";
        return 1;
    }

    LevelMessage defaultLevel;

    try {
        defaultLevel = parseLogLevel(argv[2]);
    } catch (const std::invalid_argument&) {
        std::cerr << "Invalid default log level. Use LOW, MIDDLE, or HIGH.\n";
        return 1;
    }

    Logger logger(logFileName, defaultLevel);

    std::thread worker(logWorker, std::ref(logger));

    std::string input;

    while (running) {
        std::getline(std::cin, input);
        if (input == "exit") {
            running = false;
            queueCondition.notify_all();
            break;
        }

        if (input.rfind("loglevel ", 0) == 0) { //уровень в логгере
            std::string levelString = input.substr(9);
            try {
                LevelMessage newLevel = parseLogLevel(levelString);
                logger.setLogLevel(newLevel);
                std::cout << "Default log level changed to: " << levelString << "\n";
            } catch (const std::invalid_argument&) {
                std::cerr << "Invalid log level. Use LOW, MIDDLE, or HIGH.\n";
            }
            continue;
        }

        if (input.rfind("mssetlevel ", 0) == 0) { //текущий уровень сообщений(если не указывать вручную)
            std::string levelString = input.substr(11);
            try {
                LevelMessage newLevel = parseLogLevel(levelString);
                defaultLevel = newLevel;
                std::cout << "Default message level changed to: " << levelString << "\n";
            } catch (const std::invalid_argument&) {
                std::cerr << "Invalid message level. Use LOW, MIDDLE, or HIGH.\n";
            }
            continue;
        }

        std::istringstream iss(input);
        std::string message, levelString;
        LevelMessage level = defaultLevel;

        if (std::getline(iss, message, ' ')) {
            if (std::getline(iss, levelString, ' ')) {
                try {
                    level = parseLogLevel(levelString);
                } catch (const std::invalid_argument&) {
                    std::cerr << "Invalid log level. Using default level.\n";
                }
            }
        }

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            messageQueue.push({message, level, std::chrono::system_clock::now()});
        }

        queueCondition.notify_one(); //будить wait, чтоб обработал событие
    }

    worker.join();

    std::cout << "Application terminated.\n";
    return 0;
}
