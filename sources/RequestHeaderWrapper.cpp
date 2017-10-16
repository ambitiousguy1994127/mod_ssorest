#include "RequestHeaderWrapper.h"
#include "Logger.h"
namespace ssorest
{
    RequestHeaderWrapper::RequestHeaderWrapper(request_rec* r)
    : request(r)
    {

    }

    void RequestHeaderWrapper::propagateHeader(const std::map<std::string, std::string>& sourceHeaders, TargetHeader targetHeader)
    {
        auto destinationHeader = (targetHeader == TargetHeader::In ? request->headers_in : request->headers_out);
        std::string strDir = (targetHeader == TargetHeader::In ? "request" : "response");
        for (auto& sourceHeader : sourceHeaders)
        {
            auto& name = sourceHeader.first;
            auto& value = sourceHeader.second;

            if (targetHeader == TargetHeader::In)
                Logger::emerg(request, "Propagating request header: %s=%s", name.c_str(), value.c_str());

            Logger::emerg(request, "Processing %s header from JSon: %s", strDir.c_str(), name.c_str());
            if (name == "gatewayToken")
                continue;
            
            Logger::emerg(request, "Setting %s header to : %s", name.c_str(), value.c_str());
            ::apr_table_set(destinationHeader, name.c_str(), value.c_str());
            
        }
    }

    void RequestHeaderWrapper::propagateCookies(const std::map<std::string, Cookie>& sourceCookies, TargetHeader targetHeader)
    {
        std::string mergedCookies;
        auto destinationHeader = (targetHeader == TargetHeader::In ? request->headers_in : request->headers_out);
        auto rawCurrentCookies = ::apr_table_get(destinationHeader, "Cookie");
        if (rawCurrentCookies)
            mergedCookies.assign(rawCurrentCookies);
        
        for (const auto& sourceCookie : sourceCookies)
        {
            if (!mergedCookies.empty())
                mergedCookies += "; ";
            mergedCookies += sourceCookie.first + "=" + sourceCookie.second.value;

            if (sourceCookie.maxAge > 0)
            {
                mergedCookies += "; expires=";
            }
            else if (sourceCookie.maxAge == 0)
            {
                mergedCookies += "; expires=Thu, 01 Jan 1970 00:00:00 GMT";
            }

            if (!sourceCookie.domain.empty())
            {
                mergedCookies += "; domain=" + sourceCookie.domain;
            }

            if (!sourceCookie.domain.empty())
            {
                mergedCookies += "; domain=" + sourceCookie.domain;
            }

            if (!sourceCookie.domain.empty())
            {
                mergedCookies += "; domain=" + sourceCookie.domain;
            }
            // TODO: Add Additional Cookie Attributes (hint: only in case of response)
        }

        
        ::apr_table_set(destinationHeader, "Cookie", mergedCookies.c_str());
    }
}