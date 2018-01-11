/******************************************************************************
 * ==> QR_MD2Parser ----------------------------------------------------------*
 ******************************************************************************
 * Description : Reads and exposes MD2 file content                           *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#include "QR_MD2Parser.h"

// std
#include <memory>

#ifdef _WIN32
    // needed for some std::memset implementation, e.g. in CodeBlocks where
    // compiler will not found it otherwise
    #include <cstring>
#endif

// qr engine
#include "QR_Exception.h"

//------------------------------------------------------------------------------
// Global defines
//------------------------------------------------------------------------------
#define M_MD2_ID (('2' << 24) + ('P' << 16) + ('D' << 8) + 'I')
//------------------------------------------------------------------------------
// QR_MD2Parser::IHeader - c++ cross-platform
//------------------------------------------------------------------------------
QR_MD2Parser::IHeader::IHeader() :
    QR_SealedBase<QR_MD2Parser::IHeader>(),
    m_ID(0),
    m_Version(0),
    m_SkinWidth(0),
    m_SkinHeight(0),
    m_FrameSize(0),
    m_SkinCount(0),
    m_VertexCount(0),
    m_TextureCoordCount(0),
    m_PolygonCount(0),
    m_GlCmdsCount(0),
    m_FrameCount(0),
    m_SkinOffset(0),
    m_TextureCoordOffset(0),
    m_PolygonOffset(0),
    m_FrameOffset(0),
    m_GlCmdsOffset(0),
    m_EndOffset(0)
{}
//------------------------------------------------------------------------------
QR_MD2Parser::IHeader::~IHeader()
{}
//------------------------------------------------------------------------------
void QR_MD2Parser::IHeader::Read(std::FILE* pFile)
{
    M_ASSERT(pFile);

    #ifdef _DEBUG
        #if defined(__BORLANDC__) || defined(__CODEGEARC__)
            #pragma warn -8008
            #pragma warn -8066
        #endif

        // cross-platform security, check if QR_UInt32 is 4 bytes long
        M_ASSERT(sizeof(QR_UInt32) == 4);

        #if defined(__BORLANDC__) || defined(__CODEGEARC__)
            #pragma warn .8066
            #pragma warn .8008
        #endif
    #endif

    // read header from file
    std::fread(&m_ID,                 sizeof(QR_UInt32), 1, pFile);
    std::fread(&m_Version,            sizeof(QR_UInt32), 1, pFile);
    std::fread(&m_SkinWidth,          sizeof(QR_UInt32), 1, pFile);
    std::fread(&m_SkinHeight,         sizeof(QR_UInt32), 1, pFile);
    std::fread(&m_FrameSize,          sizeof(QR_UInt32), 1, pFile);
    std::fread(&m_SkinCount,          sizeof(QR_UInt32), 1, pFile);
    std::fread(&m_VertexCount,        sizeof(QR_UInt32), 1, pFile);
    std::fread(&m_TextureCoordCount,  sizeof(QR_UInt32), 1, pFile);
    std::fread(&m_PolygonCount,       sizeof(QR_UInt32), 1, pFile);
    std::fread(&m_GlCmdsCount,        sizeof(QR_UInt32), 1, pFile);
    std::fread(&m_FrameCount,         sizeof(QR_UInt32), 1, pFile);
    std::fread(&m_SkinOffset,         sizeof(QR_UInt32), 1, pFile);
    std::fread(&m_TextureCoordOffset, sizeof(QR_UInt32), 1, pFile);
    std::fread(&m_PolygonOffset,      sizeof(QR_UInt32), 1, pFile);
    std::fread(&m_FrameOffset,        sizeof(QR_UInt32), 1, pFile);
    std::fread(&m_GlCmdsOffset,       sizeof(QR_UInt32), 1, pFile);
    std::fread(&m_EndOffset,          sizeof(QR_UInt32), 1, pFile);
}
//------------------------------------------------------------------------------
// QR_MD2Parser::ISkin - c++ cross-platform
//------------------------------------------------------------------------------
QR_MD2Parser::ISkin::ISkin() : QR_SealedBase<QR_MD2Parser::ISkin>()
{}
//------------------------------------------------------------------------------
QR_MD2Parser::ISkin::~ISkin()
{}
//------------------------------------------------------------------------------
void QR_MD2Parser::ISkin::Read(std::FILE* pFile)
{
    M_ASSERT(pFile);

    #ifdef _DEBUG
        #if defined(__BORLANDC__) || defined(__CODEGEARC__)
            #pragma warn -8008
            #pragma warn -8066
        #endif

        // cross-platform security, check if QR_Int8 is 1 byte long
        M_ASSERT(sizeof(QR_Int8) == 1);

        #if defined(__BORLANDC__) || defined(__CODEGEARC__)
            #pragma warn .8066
            #pragma warn .8008
        #endif
    #endif

    QR_Int8 name[64];

    // read skin name from file
    std::fread(&name, sizeof(QR_Int8), 64, pFile);

    // set skin name
    m_Name = name;
}
//------------------------------------------------------------------------------
// QR_MD2Parser::IVertex - c++ cross-platform
//------------------------------------------------------------------------------
QR_MD2Parser::IVertex::IVertex() :
    QR_SealedBase<QR_MD2Parser::IVertex>(),
    m_NormalIndex(0)
{
    std::memset(&m_Vertex, 0, sizeof(m_Vertex));
}
//------------------------------------------------------------------------------
QR_MD2Parser::IVertex::~IVertex()
{}
//------------------------------------------------------------------------------
void QR_MD2Parser::IVertex::Read(std::FILE* pFile)
{
    M_ASSERT(pFile);

    #ifdef _DEBUG
        #if defined(__BORLANDC__) || defined(__CODEGEARC__)
            #pragma warn -8008
            #pragma warn -8066
        #endif

        // cross-platform security, check if QR_UInt8 is 1 byte long
        M_ASSERT(sizeof(QR_UInt8) == 1);

        #if defined(__BORLANDC__) || defined(__CODEGEARC__)
            #pragma warn .8066
            #pragma warn .8008
        #endif
    #endif

    // read vertex from file
    std::fread(&m_Vertex,      sizeof(QR_UInt8), 3, pFile);
    std::fread(&m_NormalIndex, sizeof(QR_UInt8), 1, pFile);
}
//------------------------------------------------------------------------------
// QR_MD2Parser::ITextureCoord - c++ cross-platform
//------------------------------------------------------------------------------
QR_MD2Parser::ITextureCoord::ITextureCoord() :
    QR_SealedBase<QR_MD2Parser::ITextureCoord>(),
    m_U(0),
    m_V(0)
{}
//------------------------------------------------------------------------------
QR_MD2Parser::ITextureCoord::~ITextureCoord()
{}
//------------------------------------------------------------------------------
void QR_MD2Parser::ITextureCoord::Read(std::FILE* pFile)
{
    M_ASSERT(pFile);

    #ifdef _DEBUG
        #if defined(__BORLANDC__) || defined(__CODEGEARC__)
            #pragma warn -8008
            #pragma warn -8066
        #endif

        // cross-platform security, check if QR_UInt16 is 2 bytes long
        M_ASSERT(sizeof(QR_UInt16) == 2);

        #if defined(__BORLANDC__) || defined(__CODEGEARC__)
            #pragma warn .8066
            #pragma warn .8008
        #endif
    #endif

    // read texture coordinates from file
    std::fread(&m_U, sizeof(QR_UInt16), 1, pFile);
    std::fread(&m_V, sizeof(QR_UInt16), 1, pFile);
}
//------------------------------------------------------------------------------
// QR_MD2Parser::IFrame - c++ cross-platform
//------------------------------------------------------------------------------
QR_MD2Parser::IFrame::IFrame() :
    QR_SealedBase<QR_MD2Parser::IFrame>(),
    m_pVertex(NULL)
{
    std::memset(&m_Scale,     0, sizeof(m_Scale));
    std::memset(&m_Translate, 0, sizeof(m_Translate));
}
//------------------------------------------------------------------------------
QR_MD2Parser::IFrame::~IFrame()
{
    // delete vertex
    if (m_pVertex)
        delete[] m_pVertex;
}
//------------------------------------------------------------------------------
void QR_MD2Parser::IFrame::Read(std::FILE* pFile, const IHeader& header)
{
    M_ASSERT(pFile);

    #ifdef _DEBUG
        #if defined(__BORLANDC__) || defined(__CODEGEARC__)
            #pragma warn -8008
            #pragma warn -8066
        #endif

        // cross-platform security, check if types sizes match with file
        M_ASSERT(sizeof(QR_Float) == 8);
        M_ASSERT(sizeof(QR_Int8)  == 1);

        #if defined(__BORLANDC__) || defined(__CODEGEARC__)
            #pragma warn .8066
            #pragma warn .8008
        #endif
    #endif

    // previously vertex buffer exists?
    if (m_pVertex)
    {
        delete[] m_pVertex;
        m_pVertex = NULL;
    }

    // read vertex transformations
    std::fread(m_Scale,     sizeof(QR_Float), 3, pFile);
    std::fread(m_Translate, sizeof(QR_Float), 3, pFile);

    // read frame name
    QR_Int8 name[16];
    std::fread(name, sizeof(QR_Int8), 16, pFile);
    m_Name = name;

    // create frame vertex buffer
    m_pVertex = new IVertex[header.m_VertexCount];

    // read frame vertexes
    for (QR_UInt32 i = 0; i < header.m_VertexCount; ++i)
        m_pVertex[i].Read(pFile);
}
//------------------------------------------------------------------------------
// QR_MD2Parser::IPolygon - c++ cross-platform
//------------------------------------------------------------------------------
QR_MD2Parser::IPolygon::IPolygon() : QR_SealedBase<QR_MD2Parser::IPolygon>()
{
    std::memset(&m_VertexIndex,       0, sizeof(m_VertexIndex));
    std::memset(&m_TextureCoordIndex, 0, sizeof(m_TextureCoordIndex));
}
//------------------------------------------------------------------------------
QR_MD2Parser::IPolygon::~IPolygon()
{}
//------------------------------------------------------------------------------
void QR_MD2Parser::IPolygon::Read(std::FILE* pFile)
{
    M_ASSERT(pFile);

    #ifdef _DEBUG
        #if defined(__BORLANDC__) || defined(__CODEGEARC__)
            #pragma warn -8008
            #pragma warn -8066
        #endif

        // cross-platform security, check if QR_UInt16 is 2 bytes long
        M_ASSERT(sizeof(QR_UInt16) == 2);

        #if defined(__BORLANDC__) || defined(__CODEGEARC__)
            #pragma warn .8066
            #pragma warn .8008
        #endif
    #endif

    // read polygon from file
    std::fread(&m_VertexIndex,       sizeof(m_VertexIndex),       1, pFile);
    std::fread(&m_TextureCoordIndex, sizeof(m_TextureCoordIndex), 1, pFile);
}
//------------------------------------------------------------------------------
// QR_MD2Parser - c++ cross-platform
//------------------------------------------------------------------------------
QR_MD2Parser::QR_MD2Parser() :
    m_pSkin(NULL),
    m_pTexCoord(NULL),
    m_pPolygon(NULL),
    m_pFrame(NULL),
    m_pGlCmds(NULL)
{}
//------------------------------------------------------------------------------
QR_MD2Parser::~QR_MD2Parser()
{
    // delete skins
    if (m_pSkin)
        delete[] m_pSkin;

    // delete texture coordinates
    if (m_pTexCoord)
        delete[] m_pTexCoord;

    // delete polygons
    if (m_pPolygon)
        delete[] m_pPolygon;

    // delete OpenGL commands
    if (m_pGlCmds)
        delete[] m_pGlCmds;

    // delete frames
    if (m_pFrame)
        delete[] m_pFrame;
}
//------------------------------------------------------------------------------
bool QR_MD2Parser::Load(const std::string& fileName, const QR_Float& version)
{
    // is file name empty?
    if (fileName.empty())
        return false;

    // try to open file
    std::FILE* pFile = std::fopen(fileName.c_str(), "rb");

    // succeeded?
    if (!pFile)
        return false;

    // read file header
    m_Header.Read(pFile);

    // is MD2 file and version is correct?
    if ((m_Header.m_ID != M_MD2_ID) || ((QR_Float)m_Header.m_Version != version))
    {
        // close file
        std::fclose(pFile);
        return false;
    }

    // create mesh buffers
    m_pSkin     = new ISkin        [m_Header.m_SkinCount];
    m_pTexCoord = new ITextureCoord[m_Header.m_TextureCoordCount];
    m_pPolygon  = new IPolygon     [m_Header.m_PolygonCount];
    m_pGlCmds   = new QR_Int32     [m_Header.m_GlCmdsCount];
    m_pFrame    = new IFrame       [m_Header.m_FrameCount];

    // go to skins offset
    std::fseek(pFile, m_Header.m_SkinOffset, SEEK_SET);

    // read skins
    for (QR_UInt32 i = 0; i < m_Header.m_SkinCount; ++i)
        m_pSkin[i].Read(pFile);

    // go to texture coordinates offset
    std::fseek(pFile, m_Header.m_TextureCoordOffset, SEEK_SET);

    // read texture coordinates
    for (QR_UInt32 i = 0; i < m_Header.m_TextureCoordCount; ++i)
        m_pTexCoord[i].Read(pFile);

    // go to polygons offset
    std::fseek(pFile, m_Header.m_PolygonOffset, SEEK_SET);

    // read polygons
    for (QR_UInt32 i = 0; i < m_Header.m_PolygonCount; ++i)
        m_pPolygon[i].Read(pFile);

    // read OpenGL commands
    std::fseek(pFile, m_Header.m_GlCmdsOffset, SEEK_SET);
    std::fread(m_pGlCmds, sizeof(QR_UInt32), m_Header.m_GlCmdsCount, pFile);

    // go to frames offset
    std::fseek(pFile, m_Header.m_FrameOffset, SEEK_SET);

    // read frames
    for (QR_UInt32 i = 0; i < m_Header.m_FrameCount; ++i)
        m_pFrame[i].Read(pFile, m_Header);

    // close file
    std::fclose(pFile);

    return true;
}
//------------------------------------------------------------------------------
