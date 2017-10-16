#pragma once

#include <http_request.h>
#include <memory>
#include <string>
#include <vector>

namespace ssorest
{
    class SSORestPlugin
    {
    private:
        const request_rec*       sourceRequest;
        bool                     isEnabled;
        bool                     traceEnabled;
        bool                     useServerNameAsDefault;
        bool                     sendFormParameters;
        std::string              acoName;
        std::string              gatewayUrl;
        std::string              localrootpath;
        std::string              gatewayToken;
        std::string              pluginId;
        std::string              secretKey;
        std::vector<std::string> ssoZone;
        std::vector<std::string> ignoreExt;
        std::vector<std::string> ignoreUrl;
        
    public:
        SSORestPlugin();

    public:
        void setEnable(const bool& flag);
        void setTraceEnable(const bool& flag);
        void setUseServerNameAsDefault(const bool& flag);
        void setSendFormParameters(const bool& flag);
        void setAccountName(const std::string& value);
        void setGatewayUrl(const std::string& value);
        void setLocalRootPath(const std::string& value);
        void setGatewayToken(const std::string& value);
        void setPluginId(const std::string& value);
        void setSecretKey(const std::string& value);
        void addSSOZone(const std::string& value);
        void addIgnoreExt(const std::string& value);
        void addIgnoreUrl(const std::string& value);

        int process(request_rec* incomingRequest);
    };
}