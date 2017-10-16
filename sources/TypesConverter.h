#pragma once

#include <httpd.h>
#include <map>
#include <string>
#include <vector>

namespace ssorest
{
    class TypesConverter
    {
    public:
        static std::string toStringSafety(const char* cString);
        static std::map<std::string, std::string> toMap(const apr_table_t* arpTable);
        static std::vector<std::string> toVector(const apr_array_header_t* arpArray);
    };
}
