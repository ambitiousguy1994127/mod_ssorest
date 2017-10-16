#include "TypesConverter.h"

namespace ssorest
{
    std::string TypesConverter::toStringSafety(const char* cString)
    {
        return std::string(cString ? cString : "");
    }
    
    std::map<std::string, std::string> TypesConverter::toMap(const apr_table_t* arpTable)
    {
        std::map<std::string, std::string> result;
        if (arpTable)
        {
            auto arrayHeader = ::apr_table_elts(arpTable);
            auto tableEntry = reinterpret_cast<const apr_table_entry_t*>(arrayHeader->elts);
            for (auto i = 0; i < arrayHeader->nelts; ++i) 
            {
                auto key = tableEntry[i].key;
                auto value = tableEntry[i].val;
                result[key] = value;
            }
        }
        return result;
    }
    
    std::vector<std::string> TypesConverter::toVector(const apr_array_header_t* arpArray)
    {
        std::vector<std::string> result;
        if (arpArray)
        {
            auto elementsQuantity = arpArray->nelts;
            result.reserve(elementsQuantity);
            for (auto i = 0; i < elementsQuantity; i++) 
            {
                auto rawString = reinterpret_cast<const char**>(arpArray->elts)[i];
                result.emplace_back(rawString);
            }
        }
        return result;
    }
}
