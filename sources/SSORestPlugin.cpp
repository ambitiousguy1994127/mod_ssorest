#include "SSORestPlugin.h"
#include "GatewayRequest.h"
#include "GatewayResponse.h"
#include "RequestHeaderWrapper.h"
#include "Logger.h"
#include "StringProcessor.h"
#include "Cryptography/Cryptor.h"
#include "Base64.h"
#include "RandomSequence.h"
#include "URI.h"
#include <http_log.h>
#include <http_core.h>
#include <http_protocol.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

using namespace std;
namespace ssorest
{    
    SSORestPlugin::SSORestPlugin()
    : isEnabled(false)
    , traceEnabled(false)
    , useServerNameAsDefault(false)
    , sendFormParameters(false)
    {
        gatewayToken = std::string();        
    }

    void SSORestPlugin::setEnable(const bool& flag)
    {
        isEnabled = flag;
    }
    void SSORestPlugin::setTraceEnable(const bool& flag)
    {
        traceEnabled = flag;
    }
    void SSORestPlugin::setUseServerNameAsDefault(const bool& flag)
    {
        useServerNameAsDefault = flag;
    }
    void SSORestPlugin::setSendFormParameters(const bool& flag)
    {
        sendFormParameters = flag;
    }
    void SSORestPlugin::setAccountName(const string& value)
    {
        acoName = value;
    }
    void SSORestPlugin::setGatewayUrl(const string& value)
    {
        gatewayUrl = value;
    }
    void SSORestPlugin::setLocalRootPath(const std::string& value)
    {
        localrootpath = value;
    }
    void SSORestPlugin::setGatewayToken(const string& value)
    {
        gatewayToken = value;
    }
    void SSORestPlugin::setPluginId(const string& value)
    {
        pluginId = value;
    }
    void SSORestPlugin::setSecretKey(const string& value)
    {
        secretKey = value;
    }
    void SSORestPlugin::addSSOZone(const string& value)
    {
        ssoZone.push_back(value);
    }

    void SSORestPlugin::addIgnoreExt(const string& value)
    {
        ignoreExt.push_back(value);
    }

    void SSORestPlugin::addIgnoreUrl(const string& value)
    {
        ignoreUrl.push_back(value);
    }  

    int SSORestPlugin::process(request_rec* r)
    {
        sourceRequest = r;
        if(!isEnabled)
        {
            Logger::emerg(r, "SSO/Rest Plugin is disabled");
            return DECLINED;
        }
        
        Logger::emerg(r, "Processing new request");

        if (r->uri)
        {
            /* 1.Check if the request uri matches with ignored extension */
            std::string filename(r->uri);
            std::string fileext(filename.substr(filename.find_last_of(".") + 1));
            if (!fileext.empty() && filename != fileext)
            {
                if (std::find(ignoreExt.begin(), ignoreExt.end(), fileext) != ignoreExt.end())
                {
                    Logger::emerg(r, "Ignore Extension Matched");
                    return OK;
                }
            }

            /* 2.Check if the request uri matches with ignored url */
            for (auto it = ignoreUrl.begin() ; it != ignoreUrl.end(); ++it)
            {
                std::string uri(r->uri);
                if (uri.find(*it) != std::string::npos)
                {
                    Logger::emerg(r, "Ignore Url Matched");
                    return OK;
                }
            }

            if(gatewayUrl.empty())
            {
                Logger::emerg(r, "No SSORestGatewayUrl in configuration");
                return (HTTP_FORBIDDEN);
            }

            /* 3. Make A call to SSO Gateway */
            GatewayRequest gatewayRequest(r);
            gatewayRequest.set(GatewayRequest::acoName, acoName);
            gatewayRequest.set(GatewayRequest::pluginId, pluginId);
            gatewayRequest.set(GatewayRequest::gatewayToken, gatewayToken);
            gatewayRequest.buildJsonRequest(ssoZone, sendFormParameters);

            Logger::styledDebug(r, "Sending JSon request to Gateway:" + gatewayRequest.getPayload().toStyledString());
            
            GatewayResponse response(gatewayRequest.sendTo(gatewayUrl, traceEnabled));
            auto jsonResponse = response.getJsonResponse();
            auto responseHeaders = response.getJsonResponseHeader();
            auto status = response.getJsonResponseStatus();

            Logger::styledDebug(r, "Parsed reply from Gateway: " + jsonResponse.toStyledString());
            Logger::emerg(r, "Gateway provided response status = %d", status);

            auto gatewayTokenIterator = responseHeaders.find("gatewayToken");
            if (gatewayTokenIterator != responseHeaders.cend())
            {
                gatewayToken = gatewayTokenIterator->second;
                Logger::emerg(r, "Plugin stored gatwayToken=%s, len=%d", gatewayToken.c_str(), gatewayToken.size());
            }
            
            if (status == HTTP_CONTINUE)
            {
                Logger::emerg(r, "Entering handleAllowContinue");
                
                RequestHeaderWrapper requestHeaderWrapper(r);
                
                // Transfer REQUEST headers (hint: not responses!)
                auto requestHeaders = response.getJsonRequestHeader();
                std::map<std::string, std::string>::iterator itr = requestHeaders.begin();
                while (itr != requestHeaders.end()) {
                    if (StringProcessor::toUpperCase((*itr).first) == "COOKIE") {
                        itr = requestHeaders.erase(itr);
                    } else {
                        ++itr;
                    }
                }
                requestHeaderWrapper.propagateHeader(requestHeaders, RequestHeaderWrapper::TargetHeader::In);
                
                // TODO: Transfer new request cookies too! And only the name=value pairs (not the addl cookie attributes)

                //Transfer any new cookies to the response
                auto additionalCookies = response.getJsonResponseCookies();
                if (additionalCookies.size())
                    requestHeaderWrapper.propagateCookies(additionalCookies, RequestHeaderWrapper::TargetHeader::Out);
                else
                    Logger::emerg(r, "No cookies in JSON response"); 
                    
                    
                Logger::emerg(r, "Exiting handleAllowContinue");
                return (OK);
            }
            else if (status == HTTP_NOT_EXTENDED)
            {
                auto responseBody = response.getResponseBody();
                if (responseBody.find("Signature Needed") != std::string::npos)
                {
                    // Generate RandomText and RandomTextSigned String
                    auto randomText = RandomSequence::generate(32);
                    Logger::emerg(r, "Generated randomText: %s", randomText.c_str());
                    Cryptor::HMACSHA1Digest digest;
                    Cryptor::computeHMACSHA1(randomText, secretKey, digest);
                    auto signedRandomText = base64_encode(digest.data(), static_cast<unsigned int>(digest.size()));
                    Logger::emerg(r, "Generated HMAC: %s", signedRandomText.c_str());
                    
                    std::string encodedSignedRandomText;
                    URI::encode(signedRandomText, encodedSignedRandomText);
                    
                    // Add Value to current json
                    gatewayRequest.set(GatewayRequest::randomText, randomText);
                    gatewayRequest.set(GatewayRequest::randomTextSigned, encodedSignedRandomText);
                }
                else
                {
                    std::string fileName = localrootpath + std::string(r->uri);
                    struct stat statBuffer;
                    ::stat(fileName.c_str(), &statBuffer);

                    std::ifstream ifs(fileName);
                    std::string fileContent((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
                    gatewayRequest.set(GatewayRequest::content, base64_encode(fileContent.c_str(), fileContent.length()));

                    // TODO: Add Timestamp in GatewayJson
                    gatewayRequest.set(GatewayRequest::contentTimestamp, std::to_string(statBuffer.st_mtime));
                }
                return process(r);
            }
            else
            {
                RequestHeaderWrapper requestHeaderWrapper(r);
                if (responseHeaders.size())
                {
                    requestHeaderWrapper.propagateHeader(responseHeaders, RequestHeaderWrapper::TargetHeader::Out);
                }
                else 
                {
                    Logger::emerg(r, "No headers in JSON response");
                }
                
                auto additionalCookies = response.getJsonResponseCookies();
                if (additionalCookies.size())
                {
                    requestHeaderWrapper.propagateCookies(additionalCookies, RequestHeaderWrapper::TargetHeader::Out);
                }                    
                else 
                {
                    Logger::emerg(r, "No cookies in JSON response");
                }

                if(response.isResponseBodySet())
                {
                    auto moduleResponse = base64_decode(response.getResponseBody());
                    ap_rwrite(moduleResponse.c_str(), static_cast<int>(moduleResponse.size()), r);
                }
                return status;
            }
        }
        return OK;
    }

}
