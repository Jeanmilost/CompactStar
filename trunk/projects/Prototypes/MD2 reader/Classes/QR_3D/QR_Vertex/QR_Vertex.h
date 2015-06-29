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
        * Vertex coordinate type
        */
        enum IECoordType
        {
            IE_VC_Unknown = 0,
            IE_VC_XY,
            IE_VC_XYZ,
        };

        typedef std::vector<M_Precision> IBuffer;

        QR_BufferSizeType m_Offset;    // start offset, in bytes, in case vertex is refering to an external buffer
        QR_BufferSizeType m_Length;    // data length, in bytes, in case vertex is refering to an external buffer
        QR_BufferSizeType m_Stride;    // vertex stride (i.e. length between each vertex) in bytes
        IEType            m_Type;      // vertex type (i.e. how vertex is organized: triangle list, triangle fan, ...)
        IEFormat          m_Format;    // vertex format (i.e. what data vertex contains: position, normal, texture, ...)
        IECoordType       m_CoordType; // vertex coordinate type (i.e. 2D coords, 3D coords, ...)
        IBuffer           m_Buffer;    // vertex buffer (in case it's used internally)

        QR_Vertex();
        virtual ~QR_Vertex();

        /**
        * Compares vertex and determine if their format are equivalent
        *@param other - other vertex to compare with
        *@return true if both vertex header are equivalent, otherwise false
        */
        bool CompareFormat(const QR_Vertex& other) const;

        /**
        * Verifies buffer alignment
        *@return true if buffer is correctly aligned, otherwise false
        */
        static bool VerifyAlignment();
};

/**
* Vertex list, also named vertices
*@author Jean-Milost Reymond
*/
typedef std::vector<QR_Vertex*> QR_Vertices;

#endif // QR_VertexH

