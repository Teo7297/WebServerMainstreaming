#include "Server.h"

#include "Cache.h"
#include "Logger.h"

#include "INIreader/INIreader.hpp"
#include "httplib/httplib.h"


namespace MC
{
    Server::Server()
        : m_configLoaded{ false }
        , m_port{ 80 }
        , m_contentDir{ "./content" }
        , m_cacheTtl{ 0 }
        , m_certificate{ "" }
        , m_certificateKey{ "" }
        , m_logPath{ "" }
        , m_cache{ std::make_unique<Cache>() }
        , m_logger{ std::make_unique<Logger>() }
    {
        // Custom handlers definition

        m_fileRequestHandler = [this](const httplib::Request& req, httplib::Response& res)
            {
                std::cout << "File request: " << req.path << std::endl;
                // Send the file as download
                fs::path filePath = m_contentDir / req.path.substr(1); // Remove leading '/'
                if(fs::exists(filePath) && fs::is_regular_file(filePath))
                {
                    std::string fileData;

                    // Check if the file is not cached
                    if(!m_cache->GetCachedFile(filePath, fileData))
                    {
                        // Then try to read the file from disk
                        std::cout << "File not in cache, reading from file..." << std::endl;
                        std::ifstream fileStream(filePath, std::ios::binary);
                        if(!fileStream) {
                            res.status = 500;
                            res.set_content("Failed to open file", "text/plain");
                            std::cerr << "Failed to open file: " << filePath.string() << std::endl;
                            return;
                        }
                        std::ostringstream buffer;
                        buffer << fileStream.rdbuf();
                        fileData = buffer.str();

                        // Add the data to the cache
                        m_cache->AddCachedFile(filePath, fileData);
                    }
                    else
                        std::cout << "File found in cache!" << std::endl;

                    res.set_content(fileData, "application/octet-stream");
                    res.set_header("Content-Disposition", "attachment; filename=\"" + filePath.filename().string() + "\"");
                    res.set_header("File-Size", this->FormatFileSize(fs::file_size(filePath)));
                    res.status = 200;

                    std::cout << "Serving file: " << filePath.string() << std::endl;
                }
                else {
                    res.status = 404;
                    res.set_content("File not found", "text/plain");
                    std::cout << "File not found: " << filePath.string() << std::endl;
                }
            };

        m_errorHandler = [](const httplib::Request& req, httplib::Response& res) {
            auto fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
            char buf[BUFSIZ];
            snprintf(buf, sizeof(buf), fmt, res.status);
            res.set_content(buf, "text/html");
            };

        m_loggerHandler = [this](const httplib::Request& req, const httplib::Response& res)
            {
                std::stringstream ssMsg;
                ssMsg << req.remote_addr << " " << req.path;

                if(res.has_header("File-Size"))
                    ssMsg << " " << res.get_header_value("File-Size");

                m_logger->LogRequest(ssMsg.str());
            };
    }

    Server::Server(const fs::path& configPath)
        : Server()
    {
        bool success = this->LoadConfiguration(configPath);

        if(success)
        {
            this->DisplayConfiguration();
            m_cache->SetTTL(m_cacheTtl);
            m_logger->SetLogFilePath(m_logPath);
        }
    }

    Server::~Server()
    {
    }

    bool Server::LoadConfiguration(const fs::path& configPath)
    {
        INIReader config(configPath.string());

        // Load the configuration file
        if(config.ParseError() != 0)
        {
            std::cerr << "Failed to load configuration file" << std::endl;
            return false;
        }

        // Read configuration values
        m_port = config.GetInteger("server", "listening_port", 80);
        m_contentDir = config.Get("server", "content_directory", "./content");
        m_cacheTtl = config.GetInteger("server", "cache_ttl", 0);
        m_certificate = config.Get("server", "certificate", "");
        m_certificateKey = config.Get("server", "certificate_key", "");
        m_logPath = config.Get("server", "log_path", "");

        // Mark configuration as loaded
        m_configLoaded = true;

        return true;
    }

    void Server::DisplayConfiguration() const
    {
        std::cout << "Server Configuration:" << std::endl;
        std::cout << "  Port: " << m_port << std::endl;
        std::cout << "  Content Directory: " << m_contentDir << std::endl;
        std::cout << "  Cache TTL: " << m_cacheTtl << " seconds" << std::endl;
        std::cout << "  Certificate: " << m_certificate << std::endl;
        std::cout << "  Certificate Key: " << m_certificateKey << std::endl;
        std::cout << "  Log Path: " << m_logPath << std::endl;
    }

    bool Server::CreateHTTPServer()
    {
        if(!m_configLoaded)
        {
            std::cerr << "Configuration not loaded. Cannot create HTTP server." << std::endl;
            return false;
        }

        httplib::Server server;

        // Setup the server with the loaded configuration
        server.set_base_dir(m_contentDir.string());
        server.set_mount_point("/", m_contentDir.string());
        server.set_keep_alive_max_count(100);
        server.set_keep_alive_timeout(60);

        // Setup routes 
        server.Get("/", [this](const httplib::Request& req, httplib::Response& res)
            {
                std::string html = GetHTML(m_contentDir);
                res.set_content(html, "text/html");
            });

        // Setup custom handlers
        server.set_file_request_handler(m_fileRequestHandler);
        server.set_error_handler(m_errorHandler);
        server.set_logger(m_loggerHandler);

        // Bind and listen
        std::cout << "Starting server on port " << m_port << "..." << std::endl;

        // Listen starts the server (blocking call)
        if(!server.listen("0.0.0.0", m_port))
            std::cerr << "Failed to start server on port " << m_port << std::endl;

        std::cout << "Server stopped!" << std::endl;

        return true;
    }

    std::string Server::FormatFileSize(uintmax_t bytes)
    {
        const char* suffix[] = { "B", "KB", "MB", "GB", "TB" };
        double size = static_cast<double>(bytes);
        int i = 0;
        while(size >= 1024 && i < 4)
        {
            size /= 1024;
            i++;
        }

        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << size << " " << suffix[i];
        return ss.str();
    }

    std::vector<fs::path> Server::GetFilesInDirectory(const fs::path& dir)
    {
        std::vector<fs::path> files;
        if(fs::exists(dir) && fs::is_directory(dir))
            for(const auto& entry : fs::directory_iterator(dir))
                if(fs::is_regular_file(entry.status()))
                    files.push_back(entry.path());
        return files;
    }

    std::string Server::GetHTML(fs::path dir)
    {
        std::vector<fs::path> files = this->GetFilesInDirectory(dir);
        std::stringstream html;

        html << "<html><head><title>Content Directory</title></head><body style=\"background-color:#248779\">";
        html << "<h1>Content Directory: " << dir.string() << "</h1>";
        html << "<ul>";

        for(const auto& file : files) {
            if(fs::is_regular_file(file)) {
                auto size = fs::file_size(file);
                html << "<li><a href=\"" << file.filename().string() << "\">"
                    << file.filename().string() << "</a> ("
                    << this->FormatFileSize(size) << ")</li>";
            }
        }
        html << "</ul>";
        html << "</body></html>";

        return html.str();
    }

} // namespace MC