#include "Cache.h"
#include "cpptime/cpptime.h"

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

        CppTime::Timer t;
        t.add(std::chrono::seconds(m_ttl), [&](CppTime::timer_id) { m_cache.erase(filePath); });

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