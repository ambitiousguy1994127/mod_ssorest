#include <algorithm>
#include <curl/curl.h>
#include <memory>
#include <vector>

#include "CurlWrapper.h"
#include <httpd.h>
#include "Logger.h"

namespace ssorest
{       
    CurlWrapper::CurlWrapper() : curl(nullptr)
    {
        curl = ::curl_easy_init();
    }
    
    CurlWrapper::~CurlWrapper()
    {
        ::curl_easy_cleanup(curl);
    }
    
    CurlWrapper::operator CURL*()
    {
        return curl;
    }
    
    void CurlWrapper::verifyResult(CURLcode curlCode)
    {
    }
    
    size_t CurlWrapper::writeCallback(void* newData, size_t elementsQuantity, size_t elementSize, WriteBuffer* outputBuffer)
    {
        auto newDataSize = elementsQuantity * elementSize;
        auto newDataBytes = reinterpret_cast<const char*>(newData);
        outputBuffer->insert(outputBuffer->end(), newDataBytes, newDataBytes + newDataSize);
        return newDataSize;
    }
    int CurlWrapper::traceCurl(CURL *handle, curl_infotype type, void* data, size_t size, void *userp)
    {
        request_rec *r = reinterpret_cast<request_rec*>(userp);
        char* newData = reinterpret_cast<char*>(data);
        const char *text;
        (void)handle; /* prevent compiler warning */
        char buf[200];
        char *p = buf;
        char *pos;

        switch(type) {  
        case CURLINFO_TEXT:
            if ((pos=strchr(newData, '\n')) != NULL)
                *pos = '\0';
            sprintf(buf,  "== Info: %s", newData);
            Logger::emerg(r, "%s", buf);
        default:
            return 0;
        case CURLINFO_HEADER_OUT:
            text = "=> Send header";
            break;
        case CURLINFO_DATA_OUT:
            text = "=> Send data";
            break;
        case CURLINFO_SSL_DATA_OUT:
            text = "=> Send SSL data";
            break;
        case CURLINFO_HEADER_IN:
            text = "<= Recv header";
            break;
        case CURLINFO_DATA_IN:
            text = "<= Recv data";
            break;
        case CURLINFO_SSL_DATA_IN:
            text = "<= Recv SSL data";
            break;
        }

        // Logging
        size_t i;
        size_t c;

        unsigned int width=0x10;
        unsigned char *ptr = reinterpret_cast<unsigned char*> (data);

        sprintf(buf, "%s, %10.10ld bytes (0x%8.8lx)", text, (long)size, (long)size);
        Logger::emerg(r, "%s", buf);

        for(i=0; i<size; i+= width) {
            p = buf;
            sprintf(p, "0x%4.4lx: ", (long)i);
            p+=8;

            for(c = 0; c < width; c++)
            {
                if(i+c < size)
                {
                    sprintf(p, "%02x ", ptr[i+c]);
                    p+=3;
                }
                else
                {
                    sprintf(p, "   ");
                    p+=3;   
                }
            }

            for(c = 0; (c < width) && (i+c < size); c++) {
                sprintf(p, "%c",
                (ptr[i+c]>=0x20) && (ptr[i+c]<0x80)?ptr[i+c]:'.');
                p++;
            }
            *p = '\0';
            Logger::emerg(r, "%s", buf);
        }
        return 0;
    }

    std::string CurlWrapper::getUrlEncodedVersionOf(const std::string& plainText)
    {
        auto rawEncodedText = ::curl_easy_escape(curl, plainText.c_str(), plainText.size());
        std::string encodedText(rawEncodedText);
        ::curl_free(rawEncodedText);
        return encodedText;
    }
}
