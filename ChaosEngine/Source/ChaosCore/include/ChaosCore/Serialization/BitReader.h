#pragma once

namespace Chaos
{
    class BitReader
    {
    public:
        BitReader() = default;
        BitReader(uint8_t* p_data, uint32_t p_bytes);
        ~BitReader();

        void Start(uint8_t* p_data, uint32_t p_bytes);
        void Finish();

        uint32_t    ReadBits(uint8_t p_bits);
        bool        ReadAlign();
        void        ReadBytes(uint8_t* o_data, uint32_t p_bytes);

        inline  bool        WouldOverflow(int bits) const { return m_bitsRead + bits > m_numBits; }
        inline  uint32_t    GetAlignBits()          const { return (8 - m_bitsRead % 8) % 8; }
        inline  uint32_t    GetBitsRead()           const { return m_bitsRead; }
        inline  uint32_t    GetBytesRead()          const { return m_wordIndex * 4; }
        inline  int32_t     GetBitsRemaining()      const { return m_numBits - m_bitsRead; }
        inline  int32_t     GetBytesRemaining()     const { return GetBitsRemaining() / 8; }
        inline  int32_t     GetTotalBytes()         const { return m_numWords * 4; }
        inline  uint8_t*    GetData() { return (uint8_t*)m_data; };
        uint8_t*    GetData()               const;

    private:
        uint64_t    m_scratch{ 0 };
        uint32_t*   m_data{ nullptr };
        uint32_t    m_numBits{ 0 };
        uint32_t    m_numWords{ 0 };
        uint32_t    m_bitsRead{ 0 };
        uint32_t    m_wordIndex{ 0 };
        int32_t     m_scratchBits{ 0 };
    };
}

