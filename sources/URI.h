#include <string>
namespace ssorest
{
    class URI
    {
    public:
        static void encode(const std::string& str, std::string& encodedStr);
        static void decode(const std::string& str, std::string& decodedStr);
    };
}