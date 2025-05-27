#include "Cache.h"

namespace MC
{
    Cache::Cache()
        : m_cache{}
        , m_ttl{ 3600 }
    {
    }

    bool Cache::AddCachedFile(const fs::path& filePath, const std::string& fileData)
    {
        if(m_cache.contains(filePath))
            return false;

        m_cache[filePath] = fileData;

        m_timer.add(std::chrono::seconds(m_ttl), [this, filePath](CppTime::timer_id) { std::cout << "Erasing " << std::quoted(filePath.filename().string()) << " from cache\n"; m_cache.erase(filePath); });

        return true;
    }

    bool Cache::GetCachedFile(const fs::path& filePath, std::string& outData)
    {
        if(!m_cache.contains(filePath))
            return false;

        outData = m_cache[filePath];
        return true;
    }
}