#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "SSORestPlugin.h"

namespace ssorest
{
    class SSORestPluginPool
    {
    private:
        std::map<std::string, std::shared_ptr<SSORestPlugin>> pool;
        std::mutex poolMutex;
        
    public:
        std::shared_ptr<SSORestPlugin> operator[](const std::string& virtualServerName);
    };
}
