/******************************************************************************
 * ==> QR_FileBuffer ---------------------------------------------------------*
 ******************************************************************************
 * Description : File buffer interface                                        *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#include "QR_FileBuffer.h"

//------------------------------------------------------------------------------
// QR_FileBuffer - c++ cross-platform
//------------------------------------------------------------------------------
QR_FileBuffer::QR_FileBuffer() :
    QR_Buffer(),
    m_Mode(IE_M_Unknown)
{}
//------------------------------------------------------------------------------
QR_FileBuffer::~QR_FileBuffer()
{}
//------------------------------------------------------------------------------
bool QR_FileBuffer::Open(const std::string& fileName, IEMode mode)
{
    m_Mode = mode;
    return true;
}
//------------------------------------------------------------------------------
bool QR_FileBuffer::Open(const std::wstring& fileName, IEMode mode)
{
    m_Mode = mode;
    return true;
}
//------------------------------------------------------------------------------

