#include <algorithm>
#include <cctype>
#include <functional>
#include "StringProcessor.h"

namespace ssorest
{
    void StringProcessor::trimLeft(std::string& s)
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    }

    void StringProcessor::trimRight(std::string& s)
    {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    }

    void StringProcessor::trim(std::string& modifiedString)
    {
        trimLeft(modifiedString);
        trimRight(modifiedString);
    }

    std::string StringProcessor::trimmed(const std::string& source)
    {
        auto result = source;
        trim(result);
        return result;
    }

    std::string StringProcessor::toUpperCase(const std::string& source)
    {
        auto result = source;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }
    
    std::string StringProcessor::toLowerCase(const std::string& source)
    {
        auto result = source;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    void StringProcessor::replace(std::string& source, char oldChar, char newChar)
    {
        size_t oldCharPosition = -1;
        while (true)
        {
            oldCharPosition = source.find(oldChar, oldCharPosition + 1);
            if (oldCharPosition == std::string::npos) break;
            source[oldCharPosition] = newChar;
        }
    }
    
    std::vector<std::string> StringProcessor::split(const std::string& source, const std::string& delimiter)
    {
        std::vector<std::string> result;
        auto tail = source;
        while (!tail.empty())
        {
            auto delimiterPosition = tail.find(delimiter);
            if (delimiterPosition != std::string::npos)
            {
                if (delimiterPosition != 0)
                {
                    result.emplace_back(tail.cbegin(), tail.cbegin() + delimiterPosition);
                    tail.erase(0, delimiterPosition + delimiter.size());
                }
                else
                {
                    result.emplace_back();
                    tail.erase(0, delimiter.size());
                }
            }
            else
            {
                result.emplace_back(std::move(tail));
            }
        }
        return result;
    }
    
    std::string StringProcessor::getExtension(const std::string& source, char pathDelimiter)
    {
        auto dotPosition = source.rfind('.');
        if (dotPosition != std::string::npos)
        {
            auto delimiterPosition = source.rfind(pathDelimiter);
            if (delimiterPosition != std::string::npos)
            {
                if (delimiterPosition > dotPosition)
                    return std::string();
            }
            return std::string(source.cbegin() + dotPosition + 1, source.cend());
        }
        return std::string();
    }
}
