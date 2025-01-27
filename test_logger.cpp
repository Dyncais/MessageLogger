#include "logger.h"
#include <cassert>
#include <fstream>
#include <iostream>

void testLogger() {
    const std::string testFilename = "build/test_log.txt";
    Logger logger(testFilename, LevelMessage::LOW);

    // Логируем сообщения
    logger.log("Test message 1", LevelMessage::LOW, std::chrono::system_clock::now());
    logger.log("Test message 2", LevelMessage::MIDDLE, std::chrono::system_clock::now());

    logger.~Logger();

    std::ifstream logFile(testFilename);
    assert(logFile.is_open() && "Log file could not be opened!");

    int logCount = 0;
    std::string line;
    while (std::getline(logFile, line)) {
        logCount++;
    }
    logFile.close();

    // Проверка
    assert(logCount >= 2 && "Log messages were not written correctly!");
    std::cout << "Test passed: All log messages written correctly.\n";
}

int main() {
    try {
        testLogger();
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception.\n";
        return 1;
    }
    return 0;
}
