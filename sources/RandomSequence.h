#pragma once

#include <array>
#include <mutex>
#include <random>
#include <string>

namespace ssorest
{
    class RandomSequence
    {
    private:
        static const std::array<char, 36> kAlphabet;
    
    public:
        static std::string generate(size_t length);
    };
}
