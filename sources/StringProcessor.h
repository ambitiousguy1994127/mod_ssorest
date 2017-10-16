#pragma once

#include <string>
#include <vector>

namespace ssorest
{
    class StringProcessor
    {
    public:
        static void trimLeft(std::string& s);
        static void trimRight(std::string& s);
        static void trim(std::string& modifiedString);
        static std::string trimmed(const std::string& source);
        static std::string toUpperCase(const std::string& source);
        static std::string toLowerCase(const std::string& source);
        static void replace(std::string& source, char oldChar, char newChar);
        static std::vector<std::string> split(const std::string& source, const std::string& delimiter);
        static std::string getExtension(const std::string& source, char pathDelimiter);
    };
}
