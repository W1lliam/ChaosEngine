#pragma once
#include "ChaosCore/Serialization/BitReader.h"

namespace Chaos
{
    class ReadStream
    {
    public:
        static constexpr bool IsWriting{ false };
        static constexpr bool IsReading{ true };

        ReadStream() = default;
        ReadStream(uint8_t* p_data, uint32_t p_size);
        ~ReadStream();

        void Start(uint8_t* p_data, uint32_t p_size);
        void Finish();

        bool SerializeSignedInteger(int32_t& o_value, int32_t p_min, int32_t p_max);
        bool SerializeUnsignedInteger(uint8_t& o_value, uint8_t p_min, uint8_t p_max);
        bool SerializeUnsignedInteger(uint16_t& o_value, uint16_t p_min, uint16_t p_max);
        bool SerializeUnsignedInteger(uint32_t& o_value, uint32_t p_min, uint32_t p_max);
        bool SerializeBits(uint8_t& o_value, uint8_t p_bits);
        bool SerializeBits(uint16_t& o_value, uint8_t p_bits);
        bool SerializeBits(uint32_t& o_value, uint8_t p_bits);
        bool SerializeBits(uint64_t& o_value, uint8_t p_bits);
        bool SerializeBytes(uint8_t* o_data, uint32_t p_bytes);
        bool SerializeString(std::string& o_string);
        bool SerializeFloat(float& o_float);
        bool SerializeDouble(double& o_double);
        bool SerializeAlign();

        inline uint8_t* GetData() { return m_reader.GetData(); }
        inline uint8_t* GetData()           const { return m_reader.GetData(); }
        inline int32_t  GetAlignBits()      const { return m_reader.GetAlignBits(); }
        inline uint32_t GetBitsProcessed()  const { return m_bitsRead; }
        inline uint32_t GetBytesProcessed() const { return (m_bitsRead + 7) / 8; }
        inline int32_t  GetTotalBytes()     const { return m_reader.GetTotalBytes(); }



    private:
        BitReader   m_reader;
        int         m_bitsRead = 0;
    };
}
