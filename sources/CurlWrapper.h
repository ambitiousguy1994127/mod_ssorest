#pragma once

#include <algorithm>
#include <curl/curl.h>
#include <memory>
#include <vector>


namespace ssorest
{
    class CurlWrapper
    {
    public:
        typedef std::vector<char> WriteBuffer;
            
    private:
        CURL* curl;
        
    public:
        CurlWrapper();
        CurlWrapper(const CurlWrapper&) = delete;
        CurlWrapper& operator=(const CurlWrapper&) = delete;
        ~CurlWrapper();
        
    public:
        operator CURL*();
        
    public:
        static void verifyResult(CURLcode curlCode);
        static size_t writeCallback(void* newData, size_t elementsQuantity, size_t elementSize, WriteBuffer* outputBuffer);
        static int traceCurl(CURL *handle, curl_infotype type, void* data, size_t size, void *userp);
        std::string getUrlEncodedVersionOf(const std::string& plainText);
    };
}
