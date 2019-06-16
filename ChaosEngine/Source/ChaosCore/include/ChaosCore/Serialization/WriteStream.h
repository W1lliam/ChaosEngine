#pragma once
#include "ChaosCore/Serialization/BitWriter.h"

namespace Chaos
{
    class WriteStream
    {
    public:
        static constexpr bool IsWriting{ true };
        static constexpr bool IsReading{ false };

        WriteStream() = default;
        WriteStream(uint32_t* p_data, uint32_t p_bytes);
        WriteStream(uint32_t p_bytes);
        ~WriteStream();

        void Start(uint32_t* p_data, uint32_t p_bytes);
        void Start(uint32_t p_bytes);
        void Finish();

        bool SerializeSignedInteger(int32_t p_value, int32_t p_min, int32_t p_max);
        bool SerializeUnsignedInteger(uint8_t p_value, uint8_t p_min, uint8_t p_max);
        bool SerializeUnsignedInteger(uint16_t p_value, uint16_t p_min, uint16_t p_max);
        bool SerializeUnsignedInteger(uint32_t p_value, uint32_t p_min, uint32_t p_max);
        bool SerializeBits(uint8_t p_value, uint8_t p_bits);
        bool SerializeBits(uint16_t p_value, uint8_t p_bits);
        bool SerializeBits(uint32_t p_value, uint8_t p_bits);
        bool SerializeBits(uint64_t p_value, uint8_t p_bits);
        bool SerializeBytes(uint8_t* p_data, uint32_t p_bytes);
        bool SerializeString(const std::string& p_string);
        bool SerializeFloat(float p_float);
        bool SerializeDouble(double p_double);
        bool SerializeAlign();

        inline std::vector<uint32_t>&    GetBuffer()  { return m_writer.GetBuffer(); };
        inline uint8_t* GetData()                     { return m_writer.GetData(); }
        inline uint8_t* GetData()               const { return m_writer.GetData(); }
        inline int32_t  GetAlignBits()          const { return m_writer.GetAlignBits(); }
        inline uint32_t GetBytesProcessed()     const { return m_writer.GetBytesWritten(); }
        inline uint32_t GetBitsProcessed()      const { return m_writer.GetBitsWritten(); }
        inline int32_t  GetTotalBytes()         const { return m_writer.GetTotalBytes(); }


    private:
        BitWriter m_writer;
    };
}
