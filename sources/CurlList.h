#pragma once

#include <curl/curl.h>

namespace ssorest
{
    class CurlList
    {
    private:
        curl_slist* rawList;
        
    public:
        CurlList() : rawList(nullptr)
        {
        }
        
        ~CurlList()
        {
            ::curl_slist_free_all(rawList);
        }
        
    public:
        void append(const char* item)
        {
            rawList = ::curl_slist_append(rawList, item);
        }
        
    public:
        curl_slist* data()
        {
            return rawList;
        }
    };
}