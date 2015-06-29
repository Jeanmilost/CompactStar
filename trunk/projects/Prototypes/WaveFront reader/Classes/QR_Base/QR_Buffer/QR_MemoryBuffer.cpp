/******************************************************************************
 * ==> QR_MemoryBuffer -------------------------------------------------------*
 ******************************************************************************
 * Description : Buffer keeping data in memory                                *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#include "QR_MemoryBuffer.h"

// std
#include <cstring>

//------------------------------------------------------------------------------
// QR_MemoryBuffer - c++ cross-platform
//------------------------------------------------------------------------------
QR_MemoryBuffer::QR_MemoryBuffer() :
    QR_Buffer(),
    m_Offset(0L)
{}
//------------------------------------------------------------------------------
QR_MemoryBuffer::~QR_MemoryBuffer()
{}
//------------------------------------------------------------------------------
void QR_MemoryBuffer::Clear()
{
    // clear data buffer
    m_Data.clear();

    // reset values
    m_Offset = 0L;
}
//------------------------------------------------------------------------------
const QR_Buffer::IDataType* QR_MemoryBuffer::GetPtr() const
{
    // are data empty?
    if (m_Data.empty())
        return NULL;

    return &m_Data[0];
}
//------------------------------------------------------------------------------
QR_Buffer::ISizeType QR_MemoryBuffer::GetOffset() const
{
    return m_Offset;
}
//------------------------------------------------------------------------------
QR_Buffer::ISizeType QR_MemoryBuffer::GetSize() const
{
    return m_Data.size();
}
//------------------------------------------------------------------------------
QR_Buffer::ISizeType QR_MemoryBuffer::Seek(const ISizeType& start, const ISizeType& delta)
{
    // calculate new offset position
    m_Offset = (start + delta);

    // get data size
    const ISizeType dataSize = m_Data.size();

    // is offset out of bounds?
    if (m_Offset >= dataSize)
        // extend data buffer to be able to contain data up to the offset
        m_Data.resize((m_Offset + 1) - dataSize);

    return m_Offset;
}
//------------------------------------------------------------------------------
QR_Buffer::ISizeType QR_MemoryBuffer::Read(void* pBuffer, const ISizeType& length)
{
    // no destination buffer to copy to?
    if (!pBuffer)
        return 0L;

    // no length to copy?
    if (!length)
        return 0L;

    // get data size
    const ISizeType dataSize = m_Data.size();

    // no source data?
    if (!dataSize)
        return 0L;

    // is offset out of buffer?
    if (m_Offset >= dataSize)
        return 0L;

    // calculate end offset
    ISizeType endOffset = m_Offset + length;
    // is end offset out of bounds?
    if (endOffset > dataSize)
        endOffset = dataSize;

    // calculate length to copy
    const ISizeType lengthToCopy = endOffset - m_Offset;

    // copy buffer content to destination
    std::memcpy(reinterpret_cast<void*>(pBuffer),
                reinterpret_cast<const void*>(m_Data[m_Offset]),
                lengthToCopy * sizeof(IDataType));

    // update offset position
    m_Offset = endOffset;

    return lengthToCopy;
}
//------------------------------------------------------------------------------
QR_Buffer::ISizeType QR_MemoryBuffer::Write(void* pBuffer, const ISizeType& length)
{
    // no source buffer to copy from?
    if (!pBuffer)
        return 0L;

    // no length to copy?
    if (!length)
        return 0L;

    // get data size
    const ISizeType dataSize = m_Data.size();

    // calculate end offset
    ISizeType endOffset = m_Offset + length;
    // is end offset out of bounds?
    if (endOffset > dataSize)
        // extend buffer to contain new data to write
        m_Data.resize(endOffset - dataSize);

    // calculate length to copy
    const ISizeType lengthToCopy = endOffset - m_Offset;

    // copy buffer content to destination
    std::memcpy(reinterpret_cast<void*>(m_Data[m_Offset]),
                reinterpret_cast<const void*>(pBuffer),
                lengthToCopy * sizeof(IDataType));

    // update offset position
    m_Offset = endOffset;

    return lengthToCopy;
}
//------------------------------------------------------------------------------

