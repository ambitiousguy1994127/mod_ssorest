#include <vector>
#include "Cryptor.h"

namespace ssorest
{  
    void Cryptor::computeHMACSHA1(const std::string& message, const std::string& key, Cryptor::HMACSHA1Digest& hash)
    {
        CHMAC_SHA1 hasher;
        auto rawMessage = reinterpret_cast<const unsigned char*>(message.c_str());
        auto rawKey = reinterpret_cast<const unsigned char*>(key.c_str());
        std::vector<unsigned char> modifiableKey(rawKey, rawKey + key.size());
        hasher.HMAC_SHA1(rawMessage, message.size(), modifiableKey.data(), modifiableKey.size(), hash.data());
    }
}
