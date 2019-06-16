#pragma once

#include "ChaosCore/Utils/CRC.h"
#include <spdlog/fmt/ostr.h>

namespace  Chaos
{
    using StringHash = uint32_t;
    static_assert(std::is_literal_type<StringHash>::value);
}


namespace  Chaos
{
    struct StringID
    {
        StringHash  hash        {0};
        const char* string      {nullptr};
        bool        allocated   {false};

        StringID()
        {
            string = new char[128];
            allocated = true;
            memset(const_cast<char*>(string), 0, 128);
        };

        ~StringID() { if(allocated && string) delete[] string; };
        StringID(const char* p_string, size_t p_length) : string(p_string), hash(CHAOS_CRC32(p_string, p_length - 1))
        {
        }        
        
        StringID(const std::string& p_string) : hash(CHAOS_CRC32(p_string.c_str(), p_string.size()))
        {
            string = new char[p_string.size() + 1];
            allocated = true;
            strcpy_s(const_cast<char*>(string), p_string.size() + 1, p_string.c_str());
        }
        StringID(const StringID& p_other) : hash(p_other.hash)
        {
            string = new char[128];
            allocated = true;
            strcpy_s(const_cast<char*>(string), 128, p_other.string);
        }

        void operator= (const StringID& p_other)
        {
            hash = p_other.hash;
            string = new char[128];
            allocated = true;
            strcpy_s(const_cast<char*>(string), 128, p_other.string);
        }

        constexpr bool  operator==(const StringID& p_other) const { return hash == p_other.hash; }
        constexpr bool  operator!=(const StringID& p_other) const { return hash != p_other.hash; }
        constexpr bool  operator< (const StringID& p_other) const { return hash <  p_other.hash; }
        constexpr bool  operator<=(const StringID& p_other) const { return hash <= p_other.hash; }
        constexpr bool  operator> (const StringID& p_other) const { return hash >  p_other.hash; }
        constexpr bool  operator>=(const StringID& p_other) const { return hash >= p_other.hash; }
        constexpr       operator uint32_t()                 const { return hash; }
        constexpr       operator const char*()              const { return string; }
    };


    StringID operator"" _sid(const char* p_string, size_t p_length);
}

std::ostream& operator<<(std::ostream& os, const Chaos::StringID &p_stringID);


namespace std
{
    template<> struct hash<Chaos::StringID>
    {
        typedef Chaos::StringID argument_type;
        typedef Chaos::StringHash result_type;
        result_type operator()(argument_type const& s) const noexcept
        {
            return s.hash;
        }
    };
}
