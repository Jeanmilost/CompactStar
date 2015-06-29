/******************************************************************************
 * ==> QR_MD2Parser ----------------------------------------------------------*
 ******************************************************************************
 * Description : Reads and exposes MD2 file content                           *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#ifndef QR_MD2ParserH
#define QR_MD2ParserH

// std
#include <cstdio>
#include <string>

// qr engine
#include "QR_Types.h"
#include "QR_Sealed.h"
#include "QR_ModelParser.h"
#include "QR_Buffer.h"

/**
* Reads and exposes MD2 file content
*@note This class is cross-platform
*@author Jean-Milost Reymond
*/
class QR_MD2Parser : public QR_ModelParser
{
    public:
        /**
        * MD2 header
        *@note This class cannot be inherited
        */
        struct IHeader : QR_Sealed(IHeader)
        {
            QR_UInt32 m_ID;
            QR_UInt32 m_Version;
            QR_UInt32 m_SkinWidth;
            QR_UInt32 m_SkinHeight;
            QR_UInt32 m_FrameSize;
            QR_UInt32 m_SkinCount;
            QR_UInt32 m_VertexCount;
            QR_UInt32 m_TextureCoordCount;
            QR_UInt32 m_PolygonCount;
            QR_UInt32 m_GlCmdsCount;
            QR_UInt32 m_FrameCount;
            QR_UInt32 m_SkinOffset;
            QR_UInt32 m_TextureCoordOffset;
            QR_UInt32 m_PolygonOffset;
            QR_UInt32 m_FrameOffset;
            QR_UInt32 m_GlCmdsOffset;
            QR_UInt32 m_EndOffset;

            IHeader();
            ~IHeader();
            /**
            * Reads data from file
            *@param buffer - buffer to read from
            */
            void Read(QR_Buffer& buffer);
        };

        /**
        * MD2 skin
        *@note This class cannot be inherited
        */
        struct ISkin : QR_Sealed(ISkin)
        {
            std::string m_Name;

            ISkin();
            ~ISkin();

            /**
            * Reads data from file
            *@param buffer - buffer to read from
            */
            void Read(QR_Buffer& buffer);
        };

        /**
        * MD2 vertex
        *@note This class cannot be inherited
        */
        struct IVertex : QR_Sealed(IVertex)
        {
            QR_UInt8 m_Vertex[3];
            QR_UInt8 m_NormalIndex;

            IVertex();
            ~IVertex();

            /**
            * Reads data from file
            *@param buffer - buffer to read from
            */
            void Read(QR_Buffer& buffer);
        };

        /**
        * MD2 texture coordinate
        *@note This class cannot be inherited
        */
        struct ITextureCoord : QR_Sealed(ITextureCoord)
        {
            QR_UInt16 m_U;
            QR_UInt16 m_V;

            ITextureCoord();
            ~ITextureCoord();

            /**
            * Reads data from file
            *@param buffer - buffer to read from
            */
            void Read(QR_Buffer& buffer);
        };

        /**
        * MD2 frame
        *@note This class cannot be inherited
        */
        struct IFrame : QR_Sealed(IFrame)
        {
            std::string m_Name;
            QR_Float    m_Scale[3];
            QR_Float    m_Translate[3];
            IVertex*    m_pVertex;

            IFrame();
            ~IFrame();

            /**
            * Reads data from file
            *@param buffer - buffer to read from
            *@param header - MD2 file header
            */
            void Read(QR_Buffer& buffer, const IHeader& header);
        };

        /**
        * MD2 polygon
        *@note This class cannot be inherited
        */
        struct IPolygon : QR_Sealed(IPolygon)
        {
            QR_UInt16 m_VertexIndex[3];
            QR_UInt16 m_TextureCoordIndex[3];

            IPolygon();
            ~IPolygon();

            /**
            * Reads data from file
            *@param buffer - buffer to read from
            */
            void Read(QR_Buffer& buffer);
        };

        IHeader        m_Header;
        ISkin*         m_pSkin;
        ITextureCoord* m_pTexCoord;
        IPolygon*      m_pPolygon;
        IFrame*        m_pFrame;
        QR_Int32*      m_pGlCmds;

        QR_MD2Parser();
        virtual ~QR_MD2Parser();

        /**
        * Loads MD2 from file
        *@param fileName - file name
        *@return 0 on success, otherwise warning (positive value) or error (negative value) code
        */
        virtual QR_Int32 Load(const std::string&  fileName);
        virtual QR_Int32 Load(const std::wstring& fileName);

        /**
        * Loads MD2 from buffer
        *@param buffer - buffer
        *@param length - length to read in buffer, in bytes (not used here, can be 0)
        *@return 0 on success, otherwise warning (positive value) or error (negative value) code
        *@note Read will begin from current offset
        */
        virtual QR_Int32 Load(      QR_Buffer&            buffer,
                              const QR_Buffer::ISizeType& length);
};

#endif // QR_MD2ParserH

