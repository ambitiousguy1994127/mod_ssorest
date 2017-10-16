#pragma once

#include <httpd.h>
#include <http_log.h>
#include <http_request.h>
#include <string>
#include <vector>
#include "StringProcessor.h"
namespace ssorest
{
    extern int getApacheModuleIndex();

    class Logger
    {
    public:
        template <typename... Arguments> static void emerg(const request_rec* request, const char* fmt, Arguments... arguments)
        {
            emerg(request->server, fmt, arguments...);
        }   
        template <typename... Arguments> static void emerg(const server_rec* server, const char* fmt, Arguments... arguments)
        {
            write(APLOG_EMERG, server, fmt, arguments...);
        }

        template <typename... Arguments> static void alert(const request_rec* request, const char* fmt, Arguments... arguments)
        {
            alert(request->server, fmt, arguments...);
        }
        template <typename... Arguments> static void alert(const server_rec* server, const char* fmt, Arguments... arguments)
        {
            write(APLOG_ALERT, server, fmt, arguments...);
        }

        template <typename... Arguments> static void crit(const request_rec* request, const char* fmt, Arguments... arguments)
        {
            crit(request->server, fmt, arguments...);
        }
        template <typename... Arguments> static void crit(const server_rec* server, const char* fmt, Arguments... arguments)
        {
            write(APLOG_CRIT, server, fmt, arguments...);
        }

        template <typename... Arguments> static void error(const request_rec* request, const char* fmt, Arguments... arguments)
        {
            alert(request->server, fmt, arguments...);
        }
        template <typename... Arguments> static void error(const server_rec* server, const char* fmt, Arguments... arguments)
        {
            write(APLOG_ERR, server, fmt, arguments...);
        }

        template <typename... Arguments> static void warning(const request_rec* request, const char* fmt, Arguments... arguments)
        {
            warning(request->server, fmt, arguments...);
        }
        template <typename... Arguments> static void warning(const server_rec* server, const char* fmt, Arguments... arguments)
        {
            write(APLOG_WARNING, server, fmt, arguments...);
        }

        template <typename... Arguments> static void notice(const request_rec* request, const char* fmt, Arguments... arguments)
        {
            notice(request->server, fmt, arguments...);
        }
        template <typename... Arguments> static void notice(const server_rec* server, const char* fmt, Arguments... arguments)
        {
            write(APLOG_NOTICE, server, fmt, arguments...);
        }

        template <typename... Arguments> static void info(const request_rec* request, const char* fmt, Arguments... arguments)
        {
            info(request->server, fmt, arguments...);
        }
        template <typename... Arguments> static void info(const server_rec* request, const char* fmt, Arguments... arguments)
        {
            write(APLOG_INFO, request, fmt, arguments...);
        }

        template <typename... Arguments> static void debug(const request_rec* request, const char* fmt, Arguments... arguments)
        {
            debug(request->server, fmt, arguments...);
        }
        template <typename... Arguments> static void debug(const server_rec* server, const char* fmt, Arguments... arguments)
        {
            write(APLOG_DEBUG, server, fmt, arguments...);
        }

        template <typename... Arguments> static void write(int level, const server_rec* server, const char* fmt, Arguments... arguments)
        {
            ap_log_error(__FILE__, __LINE__, getApacheModuleIndex(), level, 0, server, fmt, arguments...);
        }

        static void styledDebug(const server_rec* server, const std::string& value)
        {
            auto lines = StringProcessor::split(value, "\n");
            for(const auto& line : lines)
            {
                emerg(server, "%s", line.c_str());
            }

        }

        static void styledDebug(const request_rec* request, const std::string& value)
        {
            auto lines = StringProcessor::split(value, "\n");
            for(const auto& line : lines)
            {
                emerg(request, "%s", line.c_str());
            }

        }
    };
}