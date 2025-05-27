#pragma once

#include "Defines.h"

namespace MC
{
    /// @brief Simple logger class that manages two things: a main thread log queue and a logging thread that deals with I/O write operations. 
    class Logger
    {
    public:
        Logger();
        ~Logger();

        /// @brief Set the log file for this session
        /// @param logFilePath Path where the log file will be created / overwritten
        void SetLogFilePath(const fs::path& logFilePath) { m_logFilePath = logFilePath; }

        /// @brief Queue a message to the logger thread
        /// @param req Request message
        void LogRequest(const std::string& req);

    private:

        fs::path m_logFilePath;
        std::queue<std::string> m_logQueue;
        std::unique_ptr<std::thread> m_logThread;
        std::mutex m_mtx;
        std::atomic_bool m_shouldStop;
    };
}