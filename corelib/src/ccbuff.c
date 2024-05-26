#include "ccbuff.h"

// Checks if the given cyclic buffer is considered empty or not
bool CCBEmpty(const CCBBuff* buffer)
{
    if(buffer->m_data == NULL && buffer->m_widx == 0)
    {
        return true;
    }

    return false;
}

bool CCBReadEnd(const CCBBuff* buffer)
{
    return buffer->m_ridx == buffer->m_widx;
}

void CCBInit(CCBBuff* buffer, const uint32_t size)
{
    if(!CCBEmpty(buffer))
    {
        CCBFree(buffer);
    }

    buffer->m_data = malloc(size * sizeof(uint8_t));;
    buffer->m_size = size;
    buffer->m_ridx = 0;
    buffer->m_widx = 0;
}

void CCBFree(CCBBuff* buffer)
{
    free(buffer->m_data);
    buffer->m_data = NULL;
    buffer->m_size = 0;
    buffer->m_widx = 0;
    buffer->m_ridx = 0;
}

void CCBRead(CCBBuff* buffer, char* outData)
{
    *outData = buffer->m_data[buffer->m_ridx];
    buffer->m_ridx = (buffer->m_ridx + 1) % buffer->m_size;
}

void CCBWrite(CCBBuff* buffer, const char data)
{
    buffer->m_data[buffer->m_widx] = data;
    buffer->m_widx = (buffer->m_widx + 1) % buffer->m_size;
}