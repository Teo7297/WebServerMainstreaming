#include "Logger.h"

namespace MC
{
    Logger::Logger()
        : m_logFilePath{ }
        , m_logQueue{}
        , m_logThread{ nullptr }
        , m_mtx{}
        , m_shouldStop{ false }
    {
        m_logThread = std::make_unique<std::thread>([&]()
            {

                while(!m_shouldStop)
                {
                    std::lock_guard<std::mutex> lock(m_mtx);
                    if(!m_logQueue.empty())
                    {
                        // Read the message
                        std::string req = m_logQueue.front(); 
                        m_logQueue.pop();

                        // Write to file
                        if(!m_logFilePath.empty())
                        {
                            std::ofstream logFile(m_logFilePath, std::ios::app);
                            if(logFile.is_open())
                            {
                                logFile << req << std::endl;
                                logFile.close();
                            }
                            else
                            {
                                std::cerr << "Failed to open log file: " << m_logFilePath.string() << " probably the log folder does not exist" << std::endl;
                            }
                        }
                        else
                        {
                            // If no log file path is set, just print to console
                            std::cout << req << std::endl;
                        }
                    }
                    else
                    {
                        // Sleep for a short duration to avoid busy waiting
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    }
                }

                std::cout << "Logger thread stopping..." << std::endl;
            }
        );
    }

    Logger::~Logger()
    {
        m_shouldStop = true;
        m_logThread->join();
    }

    void Logger::LogRequest(const std::string& req)
    {
        std::lock_guard<std::mutex> lock(m_mtx);

        m_logQueue.push(req);
    }
}