#pragma once

#include "Defines.h"
#include "cpptime/cpptime.h"

namespace MC
{
    /// @brief This class represets the server side file cache system. Files can be read from disk using a data stream and stored in a common std::string.
    /// This class offers a simple TTL functionality to manage data lifetime.
    class Cache
    {
        using FileData = std::string;

    public:
        Cache();
        ~Cache() = default;

        /// @brief Set the ttl for cached files
        /// @param seconds time in seconds
        void SetTTL(int seconds) { m_ttl = seconds; }

        /// @brief Add a file to the cache
        /// @param[in] filePath Path of the file on disk, used as key to store the data
        /// @param[out] outData Output data stored in a std::string
        /// @return Success
        bool AddCachedFile(const fs::path& filePath, const std::string& fileData);

        /// @brief Gets a file from the cache, if present
        /// @param[in] filePath Path of the file on disk, used as key to store the data
        /// @param[out] outData Output data stored in a std::string
        /// @return Success
        bool GetCachedFile(const fs::path& filePath, std::string& outData);

    private:
        std::unordered_map<fs::path, FileData> m_cache;
        int m_ttl;
        CppTime::Timer m_timer;
    };

}