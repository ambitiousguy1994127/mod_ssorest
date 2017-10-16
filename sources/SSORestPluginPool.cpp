#include "SSORestPluginPool.h"

namespace ssorest
{
    std::shared_ptr<SSORestPlugin> SSORestPluginPool::operator[](const std::string& virtualServerName)
    {
        std::lock_guard<std::mutex> locker(poolMutex);
        auto i = pool.find(virtualServerName);
        if (i == pool.cend())
        {
            std::shared_ptr<SSORestPlugin> filter(new SSORestPlugin());
            pool[virtualServerName] = filter;
            return filter;
        }
        else
        {
            return i->second;
        }
    }
}
