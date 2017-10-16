#pragma once

#include <array>
#include <string>

#include "HMAC_SHA1.h"

namespace ssorest
{
    class Cryptor
    {
    public:
        typedef std::array<unsigned char, CHMAC_SHA1::SHA1_DIGEST_LENGTH> HMACSHA1Digest;
    
    public:
        static void computeHMACSHA1(const std::string& message, const std::string& key, HMACSHA1Digest& hash);
    };
}
