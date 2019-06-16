#include "ChaosCore/Serialization/BitReader.h"
#include "ChaosCore/Utils/algorithm.h"

namespace Chaos
{
    BitReader::BitReader(uint8_t* p_data, uint32_t p_bytes)
    {
        Start(p_data, p_bytes);
    }


    BitReader::~BitReader()
    {
    }

    void BitReader::Start(uint8_t* p_data, uint32_t p_bytes)
    {
        m_numWords = (p_bytes + 3) / 4;
        m_numBits = p_bytes * 8;
        m_bitsRead = 0;
        m_scratch = 0;
        m_scratchBits = 0;
        m_wordIndex = 0;
        m_data = new uint32_t[m_numWords];
        memcpy(m_data, p_data, p_bytes);
    }

    void BitReader::Finish()
    {
    }

    uint32_t BitReader::ReadBits(uint8_t p_bits)
    {
        assert(p_bits > 0);
        assert(p_bits <= 32);
        assert(m_bitsRead + p_bits <= m_numBits);

        m_bitsRead += p_bits;

        assert(m_scratchBits >= 0 && m_scratchBits <= 64);

        if (m_scratchBits < p_bits)
        {
            assert(m_wordIndex < m_numWords);
            m_scratch |= (uint64_t)(m_data[m_wordIndex]) << m_scratchBits;
            m_scratchBits += 32;
            m_wordIndex++;
        }

        assert(m_scratchBits >= p_bits);

        uint32_t output = (uint32_t)(m_scratch & ((((uint64_t)1) << p_bits) - 1));

        m_scratch >>= p_bits;
        m_scratchBits -= p_bits;

        return output;
    }

    bool BitReader::ReadAlign()
    {
        int remainderBits = m_bitsRead % 8;
        if (remainderBits != 0)
        {
            uint32_t value = ReadBits(8 - remainderBits);
            assert(m_bitsRead % 8 == 0);
            if (value != 0)
                return false;
        }
        return true;
    }

    void BitReader::ReadBytes(uint8_t* o_data, uint32_t p_bytes)
    {
        assert(GetAlignBits() == 0);
        assert(m_bitsRead + p_bytes * 8 <= m_numBits);
        assert((m_bitsRead % 32) == 0 || (m_bitsRead % 32) == 8 || (m_bitsRead % 32) == 16 || (m_bitsRead % 32) == 24);


        uint32_t headBytes = (4 - (m_bitsRead % 32) / 8) % 4;
        if (headBytes > p_bytes)
            headBytes = p_bytes;
        for (uint32_t i = 0; i < headBytes; ++i)
            o_data[i] = (uint8_t)ReadBits(8);
        if (headBytes == p_bytes)
            return;

        assert(GetAlignBits() == 0);

        int numWords = (p_bytes - headBytes) / 4;
        if (numWords > 0)
        {
            assert((m_bitsRead % 32) == 0);
            memcpy(o_data + headBytes, &m_data[m_wordIndex], numWords * 4);
            m_bitsRead += numWords * 32;
            m_wordIndex += numWords;
            m_scratchBits = 0;
        }

        assert(GetAlignBits() == 0);

        int tailStart = headBytes + numWords * 4;
        int tailBytes = p_bytes - tailStart;
        assert(tailBytes >= 0 && tailBytes < 4);
        for (int i = 0; i < tailBytes; ++i)
            o_data[tailStart + i] = (uint8_t)ReadBits(8);

        assert(GetAlignBits() == 0);

        assert(headBytes + numWords * 4 + tailBytes == p_bytes);
    }

    uint8_t* BitReader::GetData() const
    {
        int bytesRead = GetBytesRead();
        uint8_t* output = new uint8_t[bytesRead];
        memcpy(output, m_data, bytesRead);
        return output;
    }
}