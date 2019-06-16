#include "ChaosCore/Serialization/ReadStream.h"
#include "ChaosCore/Serialization/Serializer.h"
#include "ChaosCore/Utils/algorithm.h"

namespace Chaos
{
    ReadStream::ReadStream(uint8_t* p_data, uint32_t p_size) : m_reader(p_data, p_size)
    {
    }


    ReadStream::~ReadStream()
    {
    }

    void ReadStream::Start(uint8_t* p_data, uint32_t p_size)
    {
        m_reader.Start(p_data, p_size);
    }

    void ReadStream::Finish()
    {
        m_reader.Finish();
    }

    bool ReadStream::SerializeSignedInteger(int32_t& o_value, int32_t p_min, int32_t p_max)
    {
        assert(p_min < p_max);
        int bits = Utils::BitsRequired(p_min, p_max);
        if (m_reader.WouldOverflow(bits))
        {
            o_value = 0;
            return false;
        }
        o_value = (int32_t)(m_reader.ReadBits(bits) + p_min);
        m_bitsRead += bits;
        return true;
    }

    bool ReadStream::SerializeUnsignedInteger(uint8_t & o_value, uint8_t p_min, uint8_t p_max)
    {
        assert(p_min < p_max);
        int bits = Utils::BitsRequired(p_min, p_max);
        if (m_reader.WouldOverflow(bits))
        {
            o_value = 0;
            return false;
        }
        o_value = (uint8_t)(m_reader.ReadBits(bits) + p_min);
        m_bitsRead += bits;
        return true;
    }

    bool ReadStream::SerializeUnsignedInteger(uint16_t & o_value, uint16_t p_min, uint16_t p_max)
    {
        assert(p_min < p_max);
        int bits = Utils::BitsRequired(p_min, p_max);
        if (m_reader.WouldOverflow(bits))
        {
            o_value = 0;
            return false;
        }
        o_value = (uint16_t)(m_reader.ReadBits(bits) + p_min);
        m_bitsRead += bits;
        return true;
    }

    bool ReadStream::SerializeUnsignedInteger(uint32_t& o_value, uint32_t p_min, uint32_t p_max)
    {
        assert(p_min < p_max);
        int bits = Utils::BitsRequired(p_min, p_max);
        if (m_reader.WouldOverflow(bits))
        {
            o_value = 0;
            return false;
        }
        o_value = m_reader.ReadBits(bits) + p_min;
        m_bitsRead += bits;
        return true;
    }

    bool ReadStream::SerializeBits(uint8_t& o_value, uint8_t p_bits)
    {
        assert(p_bits > 0);
        assert(p_bits <= 8);
        if (m_reader.WouldOverflow(p_bits))
        {
            o_value = 0;
            return false;
        }
        o_value = (uint8_t)m_reader.ReadBits(p_bits);
        m_bitsRead += p_bits;
        return true;
    }

    bool ReadStream::SerializeBits(uint16_t& o_value, uint8_t p_bits)
    {
        assert(p_bits > 0);
        assert(p_bits <= 16);
        if (m_reader.WouldOverflow(p_bits))
        {
            o_value = 0;
            return false;
        }
        o_value = (uint16_t)m_reader.ReadBits(p_bits);
        m_bitsRead += p_bits;
        return true;
    }

    bool ReadStream::SerializeBits(uint32_t& o_value, uint8_t p_bits)
    {
        assert(p_bits > 0);
        assert(p_bits <= 32);
        if (m_reader.WouldOverflow(p_bits))
        {
            o_value = 0;
            return false;
        }
        o_value = m_reader.ReadBits(p_bits);
        m_bitsRead += p_bits;
        return true;
    }

    bool ReadStream::SerializeBits(uint64_t& o_value, uint8_t p_bits)
    {
        assert(p_bits > 0);
        assert(p_bits <= 64);

        if (m_reader.WouldOverflow(p_bits))
        {
            o_value = 0;
            return false;
        }

        if (p_bits <= 32)
        {
            o_value = (uint64_t)m_reader.ReadBits(p_bits);
        }
        else
        {
            o_value = ((uint64_t)m_reader.ReadBits(32)) | (((uint64_t)m_reader.ReadBits(p_bits - 32)) << 32);
        }

        return true;
    }

    bool ReadStream::SerializeBytes(uint8_t* o_data, uint32_t p_bytes)
    {
        if (!SerializeAlign())
            return false;
        if (m_reader.WouldOverflow(p_bytes * 8))
        {
            return false;
        }
        m_reader.ReadBytes(o_data, p_bytes);
        m_bitsRead += p_bytes * 8;
        return true;
    }

    bool ReadStream::SerializeString(std::string& o_string)
    {
        if (!SerializeAlign())
        {
            o_string = "";
            return false;
        }

        int stringLength;
        if (!SerializeSignedInteger(stringLength, 0, ISerializable::MAX_STRING_LENGTH))
        {
            o_string = "";
            return false;
        }

        if (m_reader.WouldOverflow(stringLength * 8))
        {
            o_string = "";
            return false;
        }

        o_string.clear();
        o_string.reserve(stringLength);

        for (int i = 0; i < stringLength; ++i)
        {
            o_string.push_back((char)m_reader.ReadBits(8));
        }

        return true;
    }

    bool ReadStream::SerializeFloat(float& o_float)
    {
        if (m_reader.WouldOverflow(32))
        {
            o_float = 0.0f;
            return false;
        }

        uint8_t m_floatBytes[4];
        for (int i = 0; i < 4; ++i)
            m_floatBytes[i] = (uint8_t)m_reader.ReadBits(8);

        o_float = *reinterpret_cast<float*>(m_floatBytes);

        return true;
    }

    bool ReadStream::SerializeDouble(double& o_double)
    {
        if (m_reader.WouldOverflow(64))
        {
            o_double = 0.0;
            return false;
        }

        uint8_t m_doubleBytes[8];
        for (int i = 0; i < 8; ++i)
            m_doubleBytes[i] = (uint8_t)m_reader.ReadBits(8);

        o_double = *reinterpret_cast<double*>(m_doubleBytes);
        return true;
    }

    bool ReadStream::SerializeAlign()
    {
        int alignBits = m_reader.GetAlignBits();
        if (m_reader.WouldOverflow(alignBits))
        {
            return false;
        }
        if (!m_reader.ReadAlign())
        {
            return false;
        }
        m_bitsRead += alignBits;
        return true;
    }
}