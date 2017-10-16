#pragma once

#include <exception>
#include <string>

#include "StringBuilder.h"

namespace ssorest
{
    class SSORestPluginException : public std::exception
    {
    protected:
        std::string description;
        
    public:
        SSORestPluginException(const std::string& description) : description(description) 
        {
        }
        
    public:
        virtual const char* what() const throw() override 
        { 
            return description.c_str(); 
        }
    };

    class AssertionException : public SSORestPluginException
    {
    public:
        AssertionException(const std::string& description) : SSORestPluginException(description)
        {
        }
    };
    
    class JsonCppException : public SSORestPluginException
    {
    public:
        JsonCppException(const std::string& description) : SSORestPluginException(std::string("JsonCppException: ") + description)
        {
        }
    };
    
    class CurlException : public SSORestPluginException
    {
    public:
        CurlException(const std::string& description) : SSORestPluginException(description)
        {
        }
    };
    
    class ApacheException : public SSORestPluginException
    {
    public:
        ApacheException(const std::string& description) : SSORestPluginException(description)
        {
        }
    };

    class IoException : public SSORestPluginException
    {
    public:
        IoException(const std::string& description) : SSORestPluginException(description) { }
    };

    class NetworkException : public IoException
    {
    public:
        NetworkException(const std::string& description) : IoException(description) { }
    };

    class LoadLibraryException : public SSORestPluginException
    {
    public:
        LoadLibraryException(const std::string& description) : SSORestPluginException(description) { }
    };
}
