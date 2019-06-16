#include "ChaosCore/Serialization/WriteStream.h"
#include "ChaosCore/Serialization/Serializer.h"
#include "ChaosCore/Utils/algorithm.h"

namespace Chaos
{
    WriteStream::WriteStream(uint32_t* p_data, uint32_t p_bytes) : m_writer(p_data, p_bytes)
    {
    }    
    WriteStream::WriteStream(uint32_t p_bytes) : m_writer(p_bytes)
    {
    }

    WriteStream::~WriteStream()
    {
    }

    void WriteStream::Start(uint32_t* p_data, uint32_t p_bytes)
    {
        m_writer.Start(p_data, p_bytes);
    }

    void WriteStream::Start(uint32_t p_bytes)
    {
        m_writer.Start(p_bytes);
    }

    void WriteStream::Finish()
    {
        m_writer.Finish();
    }

    bool WriteStream::SerializeSignedInteger(int32_t p_value, int32_t p_min, int32_t p_max)
    {
        assert(p_min < p_max);
        assert(p_value >= p_min);
        assert(p_value <= p_max);
        int bits = Utils::BitsRequired(p_min, p_max);
        uint32_t unsignedValue = (uint32_t)(p_value - p_min);
        m_writer.WriteBits(unsignedValue, bits);
        return true;
    }

    bool WriteStream::SerializeUnsignedInteger(uint8_t p_value, uint8_t p_min, uint8_t p_max)
    {
        assert(p_min < p_max);
        assert(p_value >= p_min);
        assert(p_value <= p_max);
        int32_t bits = Utils::BitsRequired(p_min, p_max);
        uint16_t unsignedValue = p_value - p_min;
        m_writer.WriteBits(unsignedValue, bits);
        return true;
    }

    bool WriteStream::SerializeUnsignedInteger(uint16_t p_value, uint16_t p_min, uint16_t p_max)
    {
        assert(p_min < p_max);
        assert(p_value >= p_min);
        assert(p_value <= p_max);
        int32_t bits = Utils::BitsRequired(p_min, p_max);
        uint16_t unsignedValue = p_value - p_min;
        m_writer.WriteBits(unsignedValue, bits);
        return true;
    }

    bool WriteStream::SerializeUnsignedInteger(uint32_t p_value, uint32_t p_min, uint32_t p_max)
    {
        assert(p_min < p_max);
        assert(p_value >= p_min);
        assert(p_value <= p_max);
        int32_t bits = Utils::BitsRequired(p_min, p_max);
        uint32_t unsignedValue = p_value - p_min;
        m_writer.WriteBits(unsignedValue, bits);
        return true;
    }

    bool WriteStream::SerializeBits(uint8_t p_value, uint8_t p_bits)
    {
        assert(p_bits > 0);
        assert(p_bits <= 8);
        assert(p_bits == 8 || (p_value < (1 << p_bits)));
        m_writer.WriteBits(p_value, p_bits);
        return true;
    }

    bool WriteStream::SerializeBits(uint16_t p_value, uint8_t p_bits)
    {
        assert(p_bits > 0);
        assert(p_bits <= 16);
        assert(p_bits == 16 || (p_value < (1 << p_bits)));
        m_writer.WriteBits(p_value, p_bits);
        return true;
    }

    bool WriteStream::SerializeBits(uint32_t p_value, uint8_t p_bits)
    {
        assert(p_bits > 0);
        assert(p_bits <= 32);
        assert(p_bits == 32 || (p_value < (uint32_t)(1 << p_bits)));
        m_writer.WriteBits(p_value, p_bits);
        return true;
    }

    bool WriteStream::SerializeBits(uint64_t p_value, uint8_t p_bits)
    {
        assert(p_bits > 0);
        assert(p_bits <= 64);
        assert(p_bits == 64 || (p_value < (1ULL << (uint64_t)p_bits)));

        uint32_t loword = (uint32_t)p_value;
        uint32_t hiword = (uint32_t)(p_value >> 32);

        if (p_bits <= 32)
        {
            m_writer.WriteBits(loword, p_bits);
        }
        else
        {
            m_writer.WriteBits(loword, 32);
            m_writer.WriteBits(hiword, p_bits - 32);
        }
        return true;
    }

    bool WriteStream::SerializeBytes(uint8_t* p_data, uint32_t p_bytes)
    {
        assert(p_data != nullptr);
        assert(p_bytes >= 0);
        if (!SerializeAlign())
            return false;
        m_writer.WriteBytes(p_data, p_bytes);
        return true;
    }

    bool WriteStream::SerializeString(const std::string & p_string)
    {
        if (!SerializeAlign())
            return false;
        int stringLength = (int)p_string.length();
        assert(stringLength <= ISerializable::MAX_STRING_LENGTH);
        m_writer.WriteBits((uint8_t)stringLength, Utils::BitsRequired(0, ISerializable::MAX_STRING_LENGTH));
        for (int i = 0; i < stringLength; ++i)
        {
            char charValue = p_string[i];
            m_writer.WriteBits(charValue, 8);
        }
        return true;
    }

    bool WriteStream::SerializeFloat(float p_float)
    {
        uint8_t* byteArray = reinterpret_cast<uint8_t*>(&p_float);
        for (int i = 0; i < 4; ++i)
            m_writer.WriteBits(byteArray[i], 8);
        return true;
    }

    bool WriteStream::SerializeDouble(double p_double)
    {
        uint8_t* byteArray = reinterpret_cast<uint8_t*>(&p_double);
        for (int i = 0; i < 8; ++i)
            m_writer.WriteBits(byteArray[i], 8);
        return true;
    }

    bool WriteStream::SerializeAlign()
    {
        m_writer.WriteAlign();
        return true;
    }
}