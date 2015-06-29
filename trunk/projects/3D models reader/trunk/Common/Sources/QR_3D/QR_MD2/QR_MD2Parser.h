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
#include "QR_Sealed.h"
#include "QR_Types.h"

//------------------------------------------------------------------------------
// Global defines
//------------------------------------------------------------------------------
#define M_MD2_Mesh_File_Version 8
//------------------------------------------------------------------------------

/**
* Reads and exposes MD2 file content
*@note This class is cross-platform
*@author Jean-Milost Reymond
*/
class QR_MD2Parser
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
            *@param pFile - file to read from
            */
            void Read(std::FILE* pFile);
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
            *@param pFile - file to read from
            */
            void Read(std::FILE* pFile);
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
            *@param pFile - file to read from
            */
            void Read(std::FILE* pFile);
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
            *@param pFile - file to read from
            */
            void Read(std::FILE* pFile);
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
            *@param pFile - file to read from
            *@param header - MD2 file header
            */
            void Read(std::FILE* pFile, const IHeader& header);
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
            *@param pFile - file to read from
            */
            void Read(std::FILE* pFile);
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
        * Loads MD2 file
        *@param fileName - MD2 file name
        *@param version - file version
        *@return true on success, otherwise false
        */
        bool Load(const std::string& fileName,
                  const QR_Float&    version = M_MD2_Mesh_File_Version);
};

#endif // QR_MD2ParserH
