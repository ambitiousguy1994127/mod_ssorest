#include "URI.h"

namespace ssorest
{
    void URI::encode(const std::string& str, std::string& encodedStr)
    {
        static const char lookup[] = "0123456789abcdef";
        for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
        {
            char c = *it;
            if ( (c >= 'a' && c <= 'z') || 
                 (c >= 'A' && c <= 'Z') ||
                 (c >= '0' && c <= '9') ||
                 c == '-' || c == '_'   ||
                 c == '.' || c == '~')
            {
                encodedStr += c;
            }
            else if (c == ' ')
            {
                encodedStr += '+';
            }
            else 
            {
                encodedStr += '%';
                encodedStr += lookup[(c&0xF0) >> 4];
                encodedStr += lookup[(c&0x0F)];
            }
        }
    }

    void URI::decode(const std::string& str, std::string& decodedStr)
    {
        std::string::const_iterator it = str.begin();
        std::string::const_iterator end = str.end();

        while (it != end)
        {
            char c = *it++;
            if (c == '+')
                c = ' ';
            else if (c == '%')
            {
                if (it == end)
                {
                    // TODO: Error Handling
                    // Unexpected end of stream
                }
                char high = *it++;

                if (it == end)
                {
                    // TODO: Error Handling
                    // Unexpected end of stream
                }
                char low = *it++;

                if (high >= '0' && high <= '9')
                    c = high - '0';
                else if (high >= 'A' && high <= 'F')
                    c = high - 'A' + 10;
                else if (high >= 'a' && high <= 'f')
                    c = high - 'a' + 10;
                else 
                {
                    // TODO: Error Handling
                    // Unexpected input string
                }

                c *= 16;
                if (low >= '0' && low <= '9')
                    c += low - '0';
                else if (low >= 'A' && low <= 'F')
                    c += low - 'A' + 10;
                else if (low >= 'a' && low <= 'f')
                    c += low - 'a' + 10;
                else 
                {
                    // TODO: Error Handling
                    // Unexpected input string
                }
            }
            decodedStr += c;
        }
    }
}