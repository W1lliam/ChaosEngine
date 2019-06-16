#pragma once
#include "ChaosCore/Serialization/WriteStream.h"
#include "ChaosCore/Serialization/ReadStream.h"
#include "ChaosCore/Utils/StringID.h"
#include <ChaosMaths.h>

namespace Chaos
{
    class ISerializable
    {
    public:
        static constexpr uint8_t MAX_STRING_LENGTH {UINT8_MAX};

        virtual bool Load(class ReadStream& p_stream)  = 0;
        virtual bool Save(class WriteStream& p_stream) = 0;
    };

#define DECLARE_VIRTUAL_SERIALIZE_FUNCTIONS()                                           \
        bool Load(class ReadStream& p_stream)  override { return Serialize(p_stream); } \
        bool Save(class WriteStream& p_stream) override { return Serialize(p_stream); } \


    template <typename Stream>
    bool SerializeBool(Stream& p_stream, bool& p_value)
    {
        uint8_t unsigned_value = (p_value == true) ? 1 : 0;
        if(!p_stream.SerializeBits(unsigned_value, 1))
            return false;
        if (Stream::IsReading)
            p_value = unsigned_value;
        return true;
    }

    template <typename Stream>
    bool SerializeSignedInt(Stream& p_stream, int64_t& p_value, int64_t p_min, int64_t p_max)
    {
        assert(p_min < p_max);
        if (!p_stream.SerializeSignedInteger(p_value, p_min, p_max))
            return false;
        if (Stream::IsReading)
        {
            if (p_value < p_min || p_value > p_max)
                return false;
        }
        return true;
    }
    

    template <typename Stream>
    bool SerializeSignedInt(Stream& p_stream, int32_t& p_value, int32_t p_min, int32_t p_max)
    {
        assert(p_min < p_max);
        if (!p_stream.SerializeSignedInteger(p_value, p_min, p_max))
            return false;
        if (Stream::IsReading)
        {
            if (p_value < p_min || p_value > p_max)
                return false;
        }
        return true;
    }

    template <typename Stream>
    bool SerializeUnsignedInt(Stream& p_stream, uint8_t& p_value, uint8_t p_min, uint8_t p_max)
    {
        assert(p_min < p_max);
        if (!p_stream.SerializeUnsignedInteger(p_value, p_min, p_max))
            return false;
        if (Stream::IsReading)
        {
            if (p_value < p_min || p_value > p_max)
                return false;
        }
        return true;
    }

    template <typename Stream>
    bool SerializeUnsignedInt(Stream& p_stream, uint16_t& p_value, uint16_t p_min, uint16_t p_max)
    {
        assert(p_min < p_max);
        if (!p_stream.SerializeUnsignedInteger(p_value, p_min, p_max))
            return false;
        if (Stream::IsReading)
        {
            if (p_value < p_min || p_value > p_max)
                return false;
        }
        return true;
    }

    template <typename Stream>
    bool SerializeUnsignedInt(Stream& p_stream, uint32_t& p_value, uint32_t p_min, uint32_t p_max)
    {
        assert(p_min < p_max);
        if (!p_stream.SerializeUnsignedInteger(p_value, p_min, p_max))
            return false;
        if (Stream::IsReading)
        {
            if (p_value < p_min || p_value > p_max)
                return false;
        }
        return true;
    }

    template <typename Stream>
    bool SerializeUnsignedInt(Stream& p_stream, uint64_t& p_value, uint64_t p_min, uint64_t p_max)
    {
        assert(p_min < p_max);
        if (!p_stream.SerializeUnsignedInteger(p_value, p_min, p_max))
            return false;
        if (Stream::IsReading)
        {
            if (p_value < p_min || p_value > p_max)
                return false;
        }
        return true;
    }

    template <typename Stream>
    bool SerializeBits(Stream& p_stream, uint8_t& p_value, uint8_t p_bits)
    {
        assert(p_bits > 0);
        assert(p_bits <= 8);
        return p_stream.SerializeBits(p_value, p_bits);
    }

    template <typename Stream>
    bool SerializeBits(Stream& p_stream, uint16_t& p_value, uint8_t p_bits)
    {
        assert(p_bits > 0);
        assert(p_bits <= 16);
        return p_stream.SerializeBits(p_value, p_bits);
    }

    template <typename Stream>
    bool SerializeBits(Stream& p_stream, uint32_t& p_value, uint8_t p_bits)
    {
        assert(p_bits > 0);
        assert(p_bits <= 32);
        return p_stream.SerializeBits(p_value, p_bits);
    }

    template <typename Stream>
    bool SerializeBits(Stream& p_stream, uint64_t& p_value, uint8_t p_bits)
    {
        assert(p_bits > 0);
        assert(p_bits <= 64);
        return p_stream.SerializeBits(p_value, p_bits);
    }

    template <typename Stream>
    bool SerializeBytes(Stream& p_stream, uint8_t* p_data, uint32_t p_bytes)
    {
        return p_stream.SerializeBytes(p_data, p_bytes);
    }

    template <typename Stream>
    bool SerializeString(Stream& p_stream, std::string& p_value)
    {
        return p_stream.SerializeString(p_value);
    }

    template <typename Stream>
    bool SerializeStringID(Stream& p_stream, StringID& p_value)
    {
        std::string value = p_value;
        bool result = SerializeString(p_stream, value);
        if (Stream::IsReading)
        {
            p_value = operator""_sid(value.c_str(), value.size());
        }
        return result;
    }

    template <typename Stream>
    bool SerializeFloat(Stream& p_stream, float& p_value)
    {
        return p_stream.SerializeFloat(p_value);
    }
    template <typename Stream>
    bool SerializeDouble(Stream& p_stream, double& p_value)
    {
        return p_stream.SerializeDouble(p_value);
    }

    template <typename Stream>
    bool SerializeVector3(Stream& p_stream, Chaos::Maths::Vector3& p_value)
    {
        p_stream.SerializeFloat(p_value.x);
        p_stream.SerializeFloat(p_value.y);
        return p_stream.SerializeFloat(p_value.z);
    }

    template <typename Stream>
    bool SerializeVector4(Stream& p_stream, Chaos::Maths::Vector4& p_value)
    {
        p_stream.SerializeFloat(p_value.x);
        p_stream.SerializeFloat(p_value.y);
        p_stream.SerializeFloat(p_value.z);
        return p_stream.SerializeFloat(p_value.w);
    }

    template <typename Stream>
    bool SerializeQuaternion(Stream& p_stream, Chaos::Maths::Quaternion& p_value)
    {
        float x = p_value.GetX();
        float y = p_value.GetY();
        float z = p_value.GetZ();
        float w = p_value.GetRealValue();
        p_stream.SerializeFloat(x);
        p_stream.SerializeFloat(y);
        p_stream.SerializeFloat(z);
        p_stream.SerializeFloat(w);
        if constexpr (Stream::IsReading)
        {
            p_value = Chaos::Maths::Quaternion(x, y, z, w);
        }
        return true;
    }

    template <typename Stream>
    bool SerializeObject(Stream& p_stream, ISerializable& p_object)
    {
        if constexpr (Stream::IsWriting)
            return p_object.Save(p_stream);
        else
            return p_object.Load(p_stream);
    }
}