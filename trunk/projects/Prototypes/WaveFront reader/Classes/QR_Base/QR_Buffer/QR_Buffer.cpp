/******************************************************************************
 * ==> QR_Buffer -------------------------------------------------------------*
 ******************************************************************************
 * Description : Buffer interface. Buffers can contain binary data, text, ... *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#include "QR_Buffer.h"

// std
#include <stddef.h>

//------------------------------------------------------------------------------
// QR_Buffer - c++ cross-platform
//------------------------------------------------------------------------------
QR_Buffer::QR_Buffer()
{}
//------------------------------------------------------------------------------
QR_Buffer::QR_Buffer(QR_Buffer& other)
{
    Copy(other);
}
//------------------------------------------------------------------------------
QR_Buffer::~QR_Buffer()
{}
//------------------------------------------------------------------------------
QR_Buffer& QR_Buffer::operator =(QR_Buffer& other)
{
    Copy(other);
    return *this;
}
//------------------------------------------------------------------------------
void QR_Buffer::Copy(QR_Buffer& other)
{
    Copy(&other);
}
//------------------------------------------------------------------------------
void QR_Buffer::Copy(QR_Buffer* pOther)
{
    // clear current buffer content
    Clear();

    // no buffer to copy from?
    if (!pOther)
        return;

    IDataType* pBuffer = NULL;

    try
    {
        // create copy buffer
        pBuffer = new IDataType[pOther->GetSize()];

        // store current source offset
        ISizeType srcOffset = pOther->GetOffset();

        // seek source to start
        pOther->Seek(0, 0);

        // get size to copy
        const ISizeType sizeToCopy = pOther->GetSize();

        // read source buffer content
        Read(pBuffer, sizeToCopy);

        // seek source to previous offset
        pOther->Seek(0, srcOffset);

        // write source data to buffer
        Write(pBuffer, sizeToCopy);

        // seek to start position
        Seek(0, 0);
    }    catch (...)
    {
        // delete copy buffer
        if (pBuffer)
            delete pBuffer;

        // re-throw exception
        throw;
    }

    // delete copy buffer
    if (pBuffer)
        delete pBuffer;
}
//------------------------------------------------------------------------------

