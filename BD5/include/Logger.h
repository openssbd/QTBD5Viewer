/**
 * @file    Logger.h
 * @author  Riken. Center for Biosystems Dynamics Research. Laboratory for Developmental Dynamics
 * @brief   A simple multithreading logger system
 * @version 0.1
 * @date 2022-10-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <iostream>
#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>

const int MAX_LOG_SIZE = 500000000;    // In bytes around 500 Mb

// For now we only consider three log categories.
/**
 * @brief   Type of log messages
 * 
 */
enum class LogType
{
    INFO,
    ERR,
    WARN,
};

/**
 * @brief   Class defining a simple logger system. The class consider to operate
 *          on a multithreaded application 
 * 
 */
class Logger
{
public:
    /**
     * @brief   Construct a new Logger object
     * 
     * @param   file full path of the logger file 
     */
    Logger(const std::string& file);
    /**
     * @brief   Destroy the Logger object
     * 
     */
    virtual ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    /**
     * @brief   Insert a message to the logger file
     * 
     * @param entry     Message
     * @param type      The type of log message
     */
    void log(const std::string& entry, LogType type = LogType::INFO);
private:
    void processEntries();
    std::atomic<bool> mExit;
    std::mutex mMutex;
    std::condition_variable mCondVar;
    std::queue<std::string> mQueue;
    std::thread mThread;
    std::string filePath;
};
