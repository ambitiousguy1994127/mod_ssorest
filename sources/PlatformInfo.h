#pragma once

#ifdef _WIN32
#define ISSORestPlugin_WINDOWS
#elif __linux__
#define SSORestPlugin_LINUX
#else
#error "Unknown operating system"
#endif

namespace ssorest
{
    class PlatformInfo
    {
    public:
        static bool isLinux()
        {
#ifdef SSORestPlugin_LINUX
            return true;
#else
            return false;
#endif
        }

        static bool isWindows()
        {
#ifdef SSORestPlugin_WINDOWS
            return true;
#else
            return false;
#endif
        }
    };
}
