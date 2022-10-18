#include <ctime>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "Logger.h"

using namespace std;

Logger::Logger(const std::string& file) :
    mExit(false), filePath(file)
{
    if (std::filesystem::exists(filePath.c_str()))
    {
        auto fSize = std::filesystem::file_size(filePath.c_str());
        if (fSize > MAX_LOG_SIZE )
        {
            std::filesystem::remove(filePath.c_str());
        }
    }
    mThread = thread(&Logger::processEntries, this);
}

Logger::~Logger()
{
    {
        unique_lock<mutex> lock(mMutex);
        // Gracefully shut down the thread by setting mExit
        // to true and notifying the thread
        mExit = true;
        // Notify condition variable to wake up thread
        mCondVar.notify_all();
    }
    // Wait until thread is shut down. This should be outside the above code
    // block becuase the lock on mMutex must be release before calling join()!!
    mThread.join();
}

void Logger::log(const std::string& entry, LogType type)
{
    time_t now = time(0);
    std::string typeStr = "";
    switch (type)
    {
        case LogType::INFO:
            typeStr = "INFO";
            break;
        case LogType::ERR:
            typeStr = "ERROR";
            break;
        case LogType::WARN:
            typeStr = "WARN";
            break;
    }
    string timeString = "";
    char time[100];
    if (strftime(time, sizeof(time), "%Y/%m/%d %H:%M:%S", localtime(&now)))
    {
        timeString = string(time);
    }
    else
    {
        cerr << "Logger found trouble to convert date and time" << endl;
    }
    ostringstream ss;
    ss << timeString << " " << typeStr << " " << entry;
    std::string msg = ss.str();    

    unique_lock<mutex> lock(mMutex);
    mQueue.push( msg );
    mCondVar.notify_all();
}

void Logger::processEntries()
{
    ofstream ofs(filePath.c_str(), std::ios_base::app);
    if (ofs.fail()) {
        cerr << "Failed to open logfile: " << filePath << endl;
        return;
    }

    // Processing loop.
    unique_lock<mutex> lock(mMutex);
    while (true)
    {
        // Only wait for notification if we don't have to exit
        if (!mExit)
        {
            mCondVar.wait(lock);
        }
        // Condition variable is notified, something in the queue
        lock.unlock();
        while (true)
        {
            lock.lock();
            if (mQueue.empty())
            {
                break;
            }
            else
            {
                ofs << mQueue.front() << endl;
                mQueue.pop();
            }
            lock.unlock();
        }
        if (mExit)
        {
            break;
        }
    }
}



