#include "GatewayRequest.h"
#include "TypesConverter.h"
#include "StringProcessor.h"
#include "CurlWrapper.h"
#include "CurlList.h"
#include "URI.h"
#include "Logger.h"
#include <algorithm>
#include <http_request.h>
#include <http_core.h>
#include <httpd.h>
namespace ssorest
{
    const std::string GatewayRequest::acoName("acoName");
    const std::string GatewayRequest::gatewayToken("gatewayToken");
    const std::string GatewayRequest::pluginId("pluginID");
    const std::string GatewayRequest::randomText("randomText");
    const std::string GatewayRequest::randomTextSigned("randomTextSigned");
    const std::string GatewayRequest::content("content");
    const std::string GatewayRequest::contentTimestamp("contentTimestamp");

    const std::vector<std::string> GatewayRequest::jsonAttributesKeys
    ({
        GatewayRequest::acoName, 
        GatewayRequest::gatewayToken,
        GatewayRequest::pluginId,
        GatewayRequest::randomText,
        GatewayRequest::randomTextSigned,
        GatewayRequest::content,
        GatewayRequest::contentTimestamp
    });


    GatewayRequest::GatewayRequest(request_rec* sourceRequest)
    : request(sourceRequest)
    {
        scheme = getScheme(request);
    }
    
    void GatewayRequest::buildJsonRequest(const std::vector<std::string>& ssoZone, const bool& sendFormParameters)
    {
        // method
        jsonData["method"] = TypesConverter::toStringSafety(request->method);
        
        // url
        jsonData["url"] = scheme + "://" + TypesConverter::toStringSafety(request->hostname) + TypesConverter::toStringSafety(request->unparsed_uri);
        
        // protocol
        jsonData["protocol"] = TypesConverter::toStringSafety(request->protocol);
        
        // characterEncoding
        jsonData["characterEncoding"] = TypesConverter::toStringSafety(request->content_encoding);

        // contentLength
        jsonData["contentLength"] = static_cast<Json::Value::UInt>(request->clength);

        // contentType
        jsonData["contentType"] = TypesConverter::toStringSafety(request->content_type);

        // contextPath
        jsonData["contextPath"] = TypesConverter::toStringSafety(ap_document_root(request));
        
        // localAddr
        jsonData["localAddr"] = TypesConverter::toStringSafety(request->connection->local_ip);

        // localName
        jsonData["localName"] = TypesConverter::toStringSafety(request->server->server_hostname);
        
        // localPort
        jsonData["localPort"] = static_cast<Json::Value::UInt>(getServerPort(request));
        
        // remoteAddr
        jsonData["remoteAddr"] = TypesConverter::toStringSafety(request->useragent_ip);
        
        // remoteHost
        jsonData["remoteHost"] = TypesConverter::toStringSafety(request->useragent_ip);
        
        // remotePort
        jsonData["remotePort"] = static_cast<Json::Value::UInt>(request->useragent_addr->port);
        
        // secure
        jsonData["secure"] = isSecureProtocol();

        // scheme
        jsonData["scheme"] = scheme;

        // serverName
        jsonData["serverName"] = TypesConverter::toStringSafety(request->server->server_hostname);
        
        // serverPort
        jsonData["serverPort"] = static_cast<Json::Value::UInt>(getServerPort(request));
        
        // servletPath
        jsonData["servletPath"] = std::string();

        // Request Header
        auto headers = TypesConverter::toMap(request->headers_in);
        
        // locale
        Json::Value jsonLocales = Json::Value(Json::arrayValue);
        auto locales = StringProcessor::split(headers["Accept-Language"], ",");
        for (const auto& locale : locales)
        {
            auto localValue = StringProcessor::split(locale, ";");
            jsonLocales.append(StringProcessor::trimmed(localValue[0]));
        }
        jsonData["locales"] = jsonLocales;

        // headers Array
        Json::Value jsonHeaders;

        // accept-language
        Json::Value jsonHeaderAcceptLanguage = Json::Value(Json::arrayValue);
        jsonHeaderAcceptLanguage.append(headers["Accept-Language"]);
        jsonHeaders["accept-language"] = jsonHeaderAcceptLanguage;
        
        // COOKIE
        // We skip this because cookies are a separate structure

        // connection
        Json::Value jsonHeaderConnection = Json::Value(Json::arrayValue);
        jsonHeaderConnection.append(headers["Connection"]);
        jsonHeaders["connection"] = jsonHeaderConnection;

        // accept
        Json::Value jsonHeaderAccept = Json::Value(Json::arrayValue);
        jsonHeaderAccept.append(headers["Accept"]);
        jsonHeaders["accept"] = jsonHeaderAccept;

        // host
        Json::Value jsonHeaderHost = Json::Value(Json::arrayValue);
        jsonHeaderHost.append(headers["Host"]);
        jsonHeaders["host"] = jsonHeaderHost;

        // accept-encoding
        Json::Value jsonHeaderAcceptEncoding = Json::Value(Json::arrayValue);
        jsonHeaderAcceptEncoding.append(headers["Accept-Encoding"]);
        jsonHeaders["accept-encoding"] = jsonHeaderAcceptEncoding;

        // user-agent
        Json::Value jsonHeaderUserAgent = Json::Value(Json::arrayValue);
        jsonHeaderUserAgent.append(headers["User-Agent"]);
        jsonHeaders["user-agent"] = jsonHeaderUserAgent;

        // TODO: Authorization Header
        
        jsonData["headers"] = jsonHeaders;

        // cookies Array
        Json::Value jsonCookieArray = Json::Value(Json::arrayValue);
        auto unparsedPairs = StringProcessor::split(headers["Cookie"], ";");
        for (const auto& unparsedPair : unparsedPairs)
        {
            auto cookieKeyValue = StringProcessor::split(unparsedPair, "=");
            if (cookieKeyValue.size() == 2)
            {
                Json::Value jsonCookie;
                std::string cookieName = StringProcessor::trimmed(cookieKeyValue[0]);
                if (!ssoZone.empty())
                {
                    bool flag = true;
                    for (const auto& zone : ssoZone)
                    {
                        if ( cookieName.find(zone) != std::string::npos)
                        {
                            std::string cookieValue = StringProcessor::trimmed(cookieKeyValue[1]);
                            Logger::emerg(request, "Transferring request cookie to JSon payload: %s=%s", cookieName.c_str(), cookieValue.c_str());
                            jsonCookie["name"] = cookieName;
                            jsonCookie["value"] = cookieValue;
                            jsonCookieArray.append(jsonCookie);
                            flag = false;
                            break;
                        }
                    }

                    if(flag)
                        Logger::emerg(request, "Skipping request cookie outside of our zone: %s", cookieName.c_str());
                } 
                else 
                {
                    std::string cookieValue = StringProcessor::trimmed(cookieKeyValue[1]);
                    Logger::emerg(request, "Transferring request cookie to JSon payload: %s=%s", cookieName.c_str(), cookieValue.c_str());
                    jsonCookie["name"] = cookieName;
                    jsonCookie["value"] = cookieValue;
                    jsonCookieArray.append(jsonCookie);
                }
            }
        }
        jsonData["cookies"] = jsonCookieArray;

        // parameter Array
        if (sendFormParameters)
        {
            Json::Value jsonParametersArray = Json::Value(Json::objectValue);
            auto parameters = StringProcessor::split(TypesConverter::toStringSafety(request->parsed_uri.query), "&");
            std::sort(parameters.begin(), parameters.end());
            
            std::vector<std::string> firstParameter;
            std::string previousParamName;
            if (parameters.size() > 0)
            {
                firstParameter = StringProcessor::split(parameters[0], "=");
                previousParamName =  StringProcessor::trimmed(firstParameter[0]);
            }
            Json::Value jsonParameter = Json::Value(Json::arrayValue);
            for (std::vector<std::string>::iterator it = parameters.begin(); it != parameters.end(); ++it)
            {
                auto parameterKeyValue = StringProcessor::split((*it), "=");
                if (parameterKeyValue.size() == 2)
                {
                    auto currentParamName = StringProcessor::trimmed(parameterKeyValue[0]);
                    std::string decodedStr;
                    URI::decode(StringProcessor::trimmed(parameterKeyValue[1]), decodedStr);
                    if(previousParamName != currentParamName)
                    {
                        jsonParametersArray[previousParamName] = jsonParameter;
                        jsonParameter = Json::Value(Json::arrayValue);
                        previousParamName = currentParamName;
                    }
                    jsonParameter.append(decodedStr);
                    if(it == parameters.end() - 1)
                        jsonParametersArray[previousParamName] = jsonParameter;
                }
            }
            jsonData["parameters"] = jsonParametersArray;
        }

        // Attributes Array
        auto attributes = enumerateAttributes();
        for (auto& attribute : attributes)
            jsonData["attributes"][attribute.first] = attribute.second;
    }
    std::string GatewayRequest::getScheme(const request_rec* request)
    {
        auto port = getServerPort(request);
        switch (port)
        {
            case 80: 
                return "http";
            case 443: 
                return "https";
            default: 
                return "";
        }
    }
    
    bool GatewayRequest::isSecureProtocol() const
    {
        return (scheme == "https");
    }
    
    int GatewayRequest::getServerPort(const request_rec* request)
    {
        auto port = request->server->port;
        if (port == 0)
        {
            auto addresses = request->server->addrs;
            if (addresses)
                // NOTICE: Maybe here we should enumerate all address
                port = addresses->host_port;
        }
        return port;
    }

    const Json::Value& GatewayRequest::getPayload() const
    {
        return jsonData;
    }

    std::string GatewayRequest::sendTo(const std::string& gatewayUrl, bool isTraceEnabled) const
    {
        CurlWrapper curl;
        CurlWrapper::verifyResult(::curl_easy_setopt(curl, CURLOPT_URL, gatewayUrl.c_str()));
        CurlList headers;
        headers.append("Accept: application/json");
        headers.append("Content-Type: application/json");
        headers.append("Charsets: utf-8");
        headers.append("User-Agent: mod_idfc/0.0");
        CurlWrapper::verifyResult(::curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers.data()));
    
        auto postData = jsonData.toStyledString();
        CurlWrapper::verifyResult(::curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "POST"));
        CurlWrapper::verifyResult(::curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str()));
        CurlWrapper::verifyResult(::curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, postData.size()));
        CurlWrapper::verifyResult(::curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, FALSE));
        CurlWrapper::verifyResult(::curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30));
        CurlWrapper::verifyResult(::curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1));
    
        if (isTraceEnabled)
        {
            CurlWrapper::verifyResult(::curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, CurlWrapper::traceCurl));
            CurlWrapper::verifyResult(::curl_easy_setopt(curl, CURLOPT_DEBUGDATA, request));
            CurlWrapper::verifyResult(::curl_easy_setopt(curl, CURLOPT_VERBOSE, 1));
        }

        CurlWrapper::WriteBuffer rawResponse;
        CurlWrapper::verifyResult(::curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrapper::writeCallback));
        CurlWrapper::verifyResult(::curl_easy_setopt(curl, CURLOPT_WRITEDATA, &rawResponse));
        
        CurlWrapper::verifyResult(::curl_easy_perform(curl));
        std::string response(rawResponse.data(), rawResponse.size());       
        return response;
    }

    void GatewayRequest::set(const std::string& key, const std::string& value)
    {
        auto detachedValue = new std::string(value);
        auto status = ::apr_pool_userdata_set(detachedValue, key.c_str(), cleanupAttributes, request->pool);
        if (status != APR_SUCCESS)
        {
            // TODO: Error Handling here?
        }
    }

    apr_status_t GatewayRequest::cleanupAttributes(void* attribute)
    {
        auto customAttribute = reinterpret_cast<std::string*>(attribute);
        delete customAttribute;
        return APR_SUCCESS;
    }

    std::string GatewayRequest::get(const std::string& key) const
    {
        std::string* valuePointer = nullptr;
        auto status = ::apr_pool_userdata_get(reinterpret_cast<void**>(&valuePointer), key.c_str(), request->pool);
        if (status != APR_SUCCESS)
        {
            // TODO: Error Handling here?
        }
        
        std::string value;
        if (valuePointer)
            value.assign(*valuePointer);
        return value;
    }

    std::map<std::string, std::string> GatewayRequest::enumerateAttributes() const
    {
        std::map<std::string, std::string> result;
        for (auto& attributeKey : jsonAttributesKeys)
        {
            auto value = get(attributeKey);
            if (!value.empty())
                result[attributeKey] = value;
        }
        return result;
    }
}
