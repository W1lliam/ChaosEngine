#include "ChaosCore/Serialization/BitWriter.h"
#include "ChaosCore/Utils/algorithm.h"

namespace Chaos
{
    BitWriter::BitWriter(uint32_t* p_data, uint32_t p_bytes)
    {
        Start(p_data, p_bytes);
    }    
    BitWriter::BitWriter(uint32_t p_bytes)
    {
        Start(p_bytes);
    }


    BitWriter::~BitWriter()
    {
    }

    void BitWriter::Start(uint32_t* p_data, uint32_t p_bytes)
    {
        assert(p_data != nullptr && p_bytes > 0);
        m_data = p_data;
        m_numWords = p_bytes / 4;
        m_numBits = m_numWords * 32;
        m_bitsWritten = 0;
        m_wordIndex = 0;
        m_scratch = 0;
        m_scratchBits = 0;
        memset(reinterpret_cast<uint8_t*>(m_data), 0, m_numWords * 4);
    }

    void BitWriter::Start(uint32_t p_bytes)
    {
        assert(p_bytes > 0);
        m_numWords = p_bytes / 4;
        m_numBits = m_numWords * 32;
        m_buffer.resize(m_numWords);
        m_data = m_buffer.data();

        m_bitsWritten = 0;
        m_wordIndex = 0;
        m_scratch = 0;
        m_scratchBits = 0;
        memset(reinterpret_cast<uint8_t*>(m_data), 0, m_numWords * 4);
    }

    void BitWriter::Finish()
    {
        if (m_scratchBits != 0)
        {
            assert(m_wordIndex < m_numWords);
            m_data[m_wordIndex] =(uint32_t)(m_scratch & 0xFFFFFFFF);
            m_scratch >>= 32;
            m_scratchBits -= 32;
            m_wordIndex++;
            m_bitsWritten += 32 - (m_bitsWritten % 32);
        }
    }

    void BitWriter::WriteBits(uint32_t p_value, uint8_t p_bits)
    {
        assert(p_bits > 0);
        assert(p_bits <= 32);
        if (m_bitsWritten + p_bits > m_numBits)
        {
            ResizeBuffer((m_numWords * 4) * 2);
        }

        assert(m_bitsWritten + p_bits <= m_numBits);

        p_value &= (uint32_t)((((uint64_t)1) << p_bits) - 1);
        m_scratch |= ((uint64_t)p_value) << m_scratchBits;
        m_scratchBits += p_bits;

        if (m_scratchBits >= 32)
        {
            assert(m_wordIndex < m_numWords);
            m_data[m_wordIndex] = (uint32_t)(m_scratch & 0xFFFFFFFF);
            m_scratch >>= 32;
            m_scratchBits -= 32;
            m_wordIndex++;
        }
        m_bitsWritten += p_bits;
    }

    void BitWriter::WriteAlign()
    {
        int remainderBits = (int)(m_bitsWritten % 8);
        if (remainderBits != 0)
        {
            WriteBits(0, 8 - remainderBits);
            assert((m_bitsWritten % 8) == 0);
        }
    }

    void BitWriter::WriteBytes(uint8_t* p_data, uint32_t p_bytes)
    {
        assert(GetAlignBits() == 0);
        if(m_bitsWritten + p_bytes * 8 > m_numBits)
        {
            ResizeBuffer((m_numWords * 4) * 2);
        }
        assert(m_bitsWritten + p_bytes * 8 <= m_numBits);
        assert((m_bitsWritten % 32) == 0 || (m_bitsWritten % 32) == 8 || (m_bitsWritten % 32) == 16 || (m_bitsWritten % 32) == 24);

        uint32_t headBytes = (4 - (m_bitsWritten % 32) / 8) % 4;
        if (headBytes > p_bytes)
            headBytes = p_bytes;
        for (uint32_t i = 0; i < headBytes; ++i)
            WriteBits(p_data[i], 8);
        if (headBytes == p_bytes)
            return;

        Finish();

        assert(GetAlignBits() == 0);

        int numWords = (p_bytes - headBytes) / 4;
        if (numWords > 0)
        {
            assert((m_bitsWritten % 32) == 0);
            memcpy(&m_data[m_wordIndex], p_data + headBytes, numWords * 4);
            m_bitsWritten += numWords * 32;
            m_wordIndex += numWords;
            m_scratch = 0;
        }

        assert(GetAlignBits() == 0);

        int tailStart = headBytes + numWords * 4;
        int tailBytes = p_bytes - tailStart;
        assert(tailBytes >= 0 && tailBytes < 4);
        for (int i = 0; i < tailBytes; ++i)
            WriteBits(p_data[tailStart + i], 8);

        assert(GetAlignBits() == 0);

        assert(headBytes + numWords * 4 + tailBytes == p_bytes);
    }

    void BitWriter::ResizeBuffer(uint32_t p_bytes)
    {
        assert(p_bytes > 0);
        assert(p_bytes / 4 > m_numWords);
        m_numWords = p_bytes / 4;
        m_numBits = m_numWords * 32;
        m_buffer.resize(m_numWords);
        m_data = m_buffer.data();
    }

    uint8_t* BitWriter::GetData() const
    {
        int bytesWritten = GetBytesWritten();
        uint8_t* output = new uint8_t[bytesWritten];
        memcpy(output, m_buffer.data(), bytesWritten);
        return output;
    }
}