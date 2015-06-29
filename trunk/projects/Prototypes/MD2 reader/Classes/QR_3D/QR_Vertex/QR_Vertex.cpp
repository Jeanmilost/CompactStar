/******************************************************************************
 * ==> QR_Vertex -------------------------------------------------------------*
 ******************************************************************************
 * Description : Vertex global enumeration and types                          *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#include "QR_Vertex.h"

//------------------------------------------------------------------------------
// Class QR_Vertex - c++ cross-platform
//------------------------------------------------------------------------------
QR_Vertex::QR_Vertex() :
    m_Offset(0),
    m_Length(0),
    m_Stride(0),
    m_Type(IE_VT_Unknown),
    m_Format(IE_VF_None),
    m_CoordType(IE_VC_XYZ)
{}
//------------------------------------------------------------------------------
QR_Vertex::~QR_Vertex()
{}
//------------------------------------------------------------------------------
bool QR_Vertex::CompareFormat(const QR_Vertex& other) const
{
    return (m_Stride    == other.m_Stride &&
            m_Type      == other.m_Type   &&
            m_Format    == other.m_Format &&
            m_CoordType == other.m_CoordType);
}
//------------------------------------------------------------------------------
bool QR_Vertex::VerifyAlignment()
{
    #if defined(__BORLANDC__) || defined(__CODEGEARC__)
        #pragma warn -8008
        #pragma warn -8066
    #endif

    // check if vertex buffer data type is 4 bytes aligned (to avoid slow draw)
    return !(sizeof(M_Precision) % 4);

    #if defined(__BORLANDC__) || defined(__CODEGEARC__)
        #pragma warn .8066
        #pragma warn .8008
    #endif
}
//------------------------------------------------------------------------------

