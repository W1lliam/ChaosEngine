#pragma once

namespace Chaos
{
    class BitWriter
    {
    public:
        BitWriter() = default;
        BitWriter(uint32_t* p_data, uint32_t p_bytes);
        BitWriter(uint32_t p_bytes);
        ~BitWriter();
    
        void Start(uint32_t* p_data, uint32_t p_bytes);
        void Start(uint32_t p_bytes);
        void Finish();

        void WriteBits(uint32_t p_value, uint8_t p_bits);
        void WriteAlign();
        void WriteBytes(uint8_t* p_value, uint32_t p_bytes);

        void ResizeBuffer(uint32_t p_bytes);

        inline  uint32_t    GetBitsWritten()    const { return m_bitsWritten; }
        inline  uint32_t    GetBitsAvailable()  const { return m_numBits - m_bitsWritten; }
        inline  uint32_t    GetBytesWritten()   const { return (m_bitsWritten + 7) / 8; }
        inline  int32_t     GetAlignBits()      const { return (8 - (m_bitsWritten % 8)) % 8; }
        inline  int32_t     GetTotalBytes()     const { return m_numWords * 4; }
        inline  uint8_t*    GetData()                 { return (uint8_t*)m_data; };
                uint8_t*    GetData()           const;

        inline  std::vector<uint32_t>&    GetBuffer() { return m_buffer; };

    private:
        std::vector<uint32_t>   m_buffer        {};
        uint64_t                m_scratch       {0};
        uint32_t*               m_data          {nullptr};
        uint32_t                m_numBits       {0};
        uint32_t                m_numWords      {0};
        uint32_t                m_bitsWritten   {0};
        uint32_t                m_wordIndex     {0};
        int32_t                 m_scratchBits   {0};
    };
}
