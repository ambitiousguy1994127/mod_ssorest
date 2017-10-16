#pragma once

#include <string>

namespace ssorest
{
    struct Cookie
    {
        std::string name;
        std::string value;
        std::string path;
        std::string domain;
        int maxAge;
        int version;
        bool secure;
        bool httpOnly;
        std::string expires;
    };
}