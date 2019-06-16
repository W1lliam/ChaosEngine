#include "ChaosCore/Utils/StringID.h"

Chaos::StringID Chaos::operator"" _sid(const char* p_string, size_t p_length)
{
    return Chaos::StringID(p_string, p_length + 1);
};

std::ostream& operator<<(std::ostream& os, const Chaos::StringID &p_stringID)
{
    return os << p_stringID.string;
}