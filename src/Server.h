#pragma once

#include "Defines.h"

namespace httplib
{
    struct Request;
    struct Response;
}

namespace MC
{
    class Cache;
    class Logger;
    class Server
    {
    public:
        Server();
        Server(const fs::path& configPath);
        ~Server();

        /// @brief Loads the server configuration using a specified configuration file
        /// @param configPath Configuration file
        /// @return Success
        bool LoadConfiguration(const fs::path& configPath);

        /// @brief Prints to the console the current server configuration
        void DisplayConfiguration() const;

        /// @brief Checks if a configuration has been loaded
        /// @return Bool result
        bool IsConfigLoaded() const { return m_configLoaded; }

        /// @brief Creates and runs the HTTP server using the pre-setted configuration.
        /// @return False if a configuration was not set before calling this function. True otherwise.
        bool CreateHTTPServer();

    private:
        /// @brief Obtain a more readable string that represents the file size 
        /// @param bytes File size in bytes count 
        /// @return Formatted string
        std::string FormatFileSize(uintmax_t bytes);

        /// @brief Obtain a list of the files inside the given directory path
        /// @param dir Path to the desired directory
        /// @return List of file paths 
        std::vector<fs::path> GetFilesInDirectory(const fs::path& dir);

        /// @brief Obtain the HTML code necessary to render the page that serves the static files in the requested directory
        /// @param dir Path to the desired directory
        /// @return String containing the HTML code
        std::string GetHTML(fs::path dir);

    private:

        // Server Configuration
        bool m_configLoaded;
        int m_port;
        fs::path m_contentDir;
        int m_cacheTtl;
        std::string m_certificate;
        std::string m_certificateKey;
        fs::path m_logPath;

        // Cache
        std::unique_ptr<Cache> m_cache;

        // Logger
        std::unique_ptr<Logger> m_logger;

        // Custom handlers
        std::function<void(const httplib::Request& req, httplib::Response& res)> m_fileRequestHandler;
        std::function<void(const httplib::Request& req, httplib::Response& res)> m_errorHandler;
        std::function<void(const httplib::Request& req, const httplib::Response& res)> m_loggerHandler;
    };
}