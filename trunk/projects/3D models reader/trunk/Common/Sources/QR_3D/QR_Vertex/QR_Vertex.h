/******************************************************************************
 * ==> QR_Vertex -------------------------------------------------------------*
 ******************************************************************************
 * Description : Vertex descriptor, contains global enumeration and types     *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#ifndef QR_VertexH
#define QR_VertexH

// qr engine
#include "QR_Types.h"

/**
* Vertex descriptor, contains global enumeration and types
*@author Jean-Milost Reymond
*/
class QR_Vertex
{
    public:
        /**
        * Vertex buffer type enumeration
        */
        enum IEType
        {
            IE_VT_Unknown = 0,
            IE_VT_Triangles,
            IE_VT_TriangleStrip,
            IE_VT_TriangleFan,
            IE_VT_Quads,
            IE_VT_QuadStrip,
        };

        /**
        * Vertex format enumeration
        *@note Flags can be combined
        */
        enum IEFormat
        {
            IE_VF_None      = 0x00,
            IE_VF_Normals   = 0x01,
            IE_VF_TexCoords = 0x02,
            IE_VF_Colors    = 0x04,
        };

        /**
        * Vertex content
        */
        enum IEContent
        {
            IE_VC_Unknown = 0,
            IE_VC_XY,
            IE_VC_XYZ,
        };

        QR_BufferSizeType m_Offset;
        QR_BufferSizeType m_Length;
        QR_BufferSizeType m_Stride;
        IEType            m_Type;
        IEFormat          m_Format;
        IEContent         m_Content;

        QR_Vertex();
        virtual ~QR_Vertex();
};

#endif // QR_VertexH
