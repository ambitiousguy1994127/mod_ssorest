#include "RandomSequence.h"

namespace ssorest
{
    const std::array<char, 36> RandomSequence::kAlphabet = 
    {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 
        'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 
        'u', 'v', 'w', 'x', 'y', 'z'
    };
    
    std::string RandomSequence::generate(size_t length)
    {
        std::string sequence;
        sequence.reserve(length);
        
        static std::uniform_int_distribution<> distribution(0, kAlphabet.size() - 1);
        static std::default_random_engine randomEngine;
        static std::mutex distrubutionMutex;
        std::lock_guard<std::mutex> distributionLocker(distrubutionMutex);
        for (size_t i = 0; i < length; ++i)
        {
            int characterIndex = distribution(randomEngine);
            sequence += kAlphabet[characterIndex];
        }
        return sequence;
    }
}
