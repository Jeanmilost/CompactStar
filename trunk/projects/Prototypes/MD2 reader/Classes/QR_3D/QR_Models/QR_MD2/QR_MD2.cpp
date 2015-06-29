/******************************************************************************
 * ==> QR_MD2 ----------------------------------------------------------------*
 ******************************************************************************
 * Description : MD2 model                                                    *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#include "QR_MD2.h"

// std
#include <cstdio>
#include <cmath>

// qr engine
#include "QR_Exception.h"
#include "QR_StdFileBuffer.h"
#include "QR_MD2Common.h"

//------------------------------------------------------------------------------
// Global defines
//------------------------------------------------------------------------------
#define M_MD2_Normals_Table_File_Version 1.0f
//------------------------------------------------------------------------------
// QR_MD2::ILight - c++ cross-platform
//------------------------------------------------------------------------------
QR_MD2::ILight::ILight() :
    m_Ambient(QR_Color(255, 255, 255, 255)),
    m_Light(QR_Color(255, 255, 255, 255))
{}
//------------------------------------------------------------------------------
QR_MD2::ILight::ILight(const QR_Color&     ambient,
                       const QR_Color&     light,
                       const QR_Vector3DP& direction) :
    m_Ambient(ambient),
    m_Light(light),
    m_Direction(direction)
{}
//------------------------------------------------------------------------------
QR_MD2::ILight::~ILight()
{}
//------------------------------------------------------------------------------
// QR_MD2 - c++ cross-platform
//------------------------------------------------------------------------------
QR_MD2::QR_MD2() :
    QR_Model(),
    m_Color(QR_Color(255, 255, 255, 255)),
    m_LightningMode(IE_LM_None),
    m_DoConvertRHLH(false)
{
    #ifdef _DEBUG
        // verify vertex buffer alignment
        if (!QR_Vertex::VerifyAlignment())
            M_THROW_EXCEPTION("Misaligned vertex buffer data type - must be 4 bytes aligned");
    #endif
}
//------------------------------------------------------------------------------
QR_MD2::~QR_MD2()
{}
//------------------------------------------------------------------------------
QR_Int32 QR_MD2::Load(const std::string& fileName)
{
    return m_Mesh.Load(fileName);
}
//------------------------------------------------------------------------------
QR_Int32 QR_MD2::Load(const std::wstring& fileName)
{
    return m_Mesh.Load(fileName);
}
//------------------------------------------------------------------------------
QR_Int32 QR_MD2::Load(QR_Buffer& buffer, const QR_Buffer::ISizeType& length)
{
    return m_Mesh.Load(buffer, length);
}
//---------------------------------------------------------------------------
QR_Int32 QR_MD2::LoadNormals(const std::string& fileName)
{
    // create a file buffer and open it for read
    QR_StdFileBuffer buffer;
    buffer.Open(fileName, QR_FileBuffer::IE_M_Read);
    buffer.Seek(0, 0);

    // read WaveFront content
    return LoadNormals(buffer, buffer.GetSize());
}
//---------------------------------------------------------------------------
QR_Int32 QR_MD2::LoadNormals(const std::wstring& fileName)
{
    // create a file buffer and open it for read
    QR_StdFileBuffer buffer;
    buffer.Open(fileName, QR_FileBuffer::IE_M_Read);
    buffer.Seek(0, 0);

    // read WaveFront content
    return LoadNormals(buffer, buffer.GetSize());
}
//------------------------------------------------------------------------------
QR_Int32 QR_MD2::LoadNormals(QR_Buffer& buffer, const QR_Buffer::ISizeType& length)
{
    // clear previous table, if needed
    m_Normals.clear();

    // is buffer empty?
    if (buffer.Empty())
        return QR_MD2Common::IE_C_EmptyBuffer;

    QR_Float fileVersion;

    // read version
    buffer.Read(&fileVersion, sizeof(QR_Float));

    // is version correct?
    if (fileVersion != M_MD2_Normals_Table_File_Version)
        return QR_MD2Common::IE_C_InvalidFileVersion;

    QR_UInt32 dataLength;

    // read file length
    buffer.Read(&dataLength, sizeof(QR_UInt32));

    // reserve memory for normals table
    m_Normals.reserve(dataLength);

    // iterate through normals
    for (QR_UInt32 i = 0; i < dataLength; ++i)
    {
        QR_Vector3DP normal;

        // read normal from file
        buffer.Read(&normal, sizeof(QR_Vector3DP));

        // set normal in table
        m_Normals.push_back(normal);
    }

    return QR_MD2Common::IE_C_Success;
}
//------------------------------------------------------------------------------
bool QR_MD2::IsRHLHConversionEnabled() const
{
    return m_DoConvertRHLH;
}
//------------------------------------------------------------------------------
void QR_MD2::EnableRHLHConversion(bool value)
{
    m_DoConvertRHLH = value;
}
//------------------------------------------------------------------------------
QR_MD2::IELightningMode QR_MD2::GetLightningMode() const
{
    return m_LightningMode;
}
//------------------------------------------------------------------------------
void QR_MD2::SetLightningMode(IELightningMode mode)
{
    m_LightningMode = mode;

    // do pre-calculate light?
    if (mode == IE_LM_Precalculated)
        // ensure that at least color and normals are enabled
        m_VertexFormat = (QR_Vertex::IEFormat)(m_VertexFormat           |
                                               QR_Vertex::IE_VF_Normals |
                                               QR_Vertex::IE_VF_Colors);
}
//------------------------------------------------------------------------------
void QR_MD2::SetLight(const ILight& light)
{
    m_Light = light;
}
//------------------------------------------------------------------------------
QR_Int32 QR_MD2::GetVertices(QR_SizeT index, QR_Vertices& vertices) const
{
    // is frame index out of bounds?
    if (index >= m_Mesh.m_Header.m_FrameCount)
        return QR_MD2Common::IE_C_IndexOutOfBounds;

    // do use normals and pre-calculated normals table wasn't populated?
    if ((m_VertexFormat & QR_Vertex::IE_VF_Normals) && !m_Normals.size())
        return QR_MD2Common::IE_C_NormalsTableEmpty;

    // get source frame from which vertices should be extracted
    QR_MD2Parser::IFrame* pSrcFrame = &m_Mesh.m_pFrame[index];

    // found it?
    if (!pSrcFrame)
        return QR_MD2Common::IE_C_FrameNotFound;

    // basically stride is the coordinates values size
    QR_SizeT stride = 3;

    // do include normals?
    if (m_VertexFormat & QR_Vertex::IE_VF_Normals)
        stride += 3;

    // do include texture coordinates?
    if (m_VertexFormat & QR_Vertex::IE_VF_TexCoords)
        stride += 2;

    // do include colors?
    if (m_VertexFormat & QR_Vertex::IE_VF_Colors)
        stride += 4;

    QR_Int32* pGLCmds = m_Mesh.m_pGlCmds;

    // iterate through OpenGL commands (negative value is for triangle fan,
    // positive value is for triangle strip, 0 means list end)
    while (QR_Int32 i = *pGLCmds)
    {
        // the first command is the number of vertexes to process, read above,
        // so skip it
        ++pGLCmds;

        // create and populate new vertex for the current command
        std::auto_ptr<QR_Vertex> pVertex(new QR_Vertex());
        pVertex->m_Format = m_VertexFormat;
        pVertex->m_Stride = stride;

        // search for OpenGL command type
        if (i < 0)
        {
            pVertex->m_Type = QR_Vertex::IE_VT_TriangleFan;
            i = -i;
        }
        else
            pVertex->m_Type = QR_Vertex::IE_VT_TriangleStrip;

        // iterate through OpenGL commands to process
        for (; i > 0; --i, pGLCmds += 3)
        {
            // get source vertex
            QR_MD2Parser::IVertex* pSrcVertex = &pSrcFrame->m_pVertex[pGLCmds[2]];

            // found it?
            if (!pSrcVertex)
                return QR_MD2Common::IE_C_VertexNotFound;

            // uncompress vertex
            QR_Vector3DP vertex = UncompressVertex(pSrcFrame, pSrcVertex);

            // do convert right hand <-> left hand coordinate system?
            if (m_DoConvertRHLH)
                // apply conversion
                vertex.m_X = -vertex.m_X;

            // populate vertex buffer
            pVertex->m_Buffer.push_back(vertex.m_X);
            pVertex->m_Buffer.push_back(vertex.m_Y);
            pVertex->m_Buffer.push_back(vertex.m_Z);

            QR_Vector3DP normal;

            // do include normals?
            if (m_VertexFormat & QR_Vertex::IE_VF_Normals)
            {
                // get vertex normal
                QR_Vector3DP normal = m_Normals[pSrcVertex->m_NormalIndex];

                // do convert right hand <-> left hand coordinate system?
                if (m_DoConvertRHLH)
                    // apply conversion
                    normal.m_X = -normal.m_X;

                pVertex->m_Buffer.push_back(normal.m_X);
                pVertex->m_Buffer.push_back(normal.m_Y);
                pVertex->m_Buffer.push_back(normal.m_Z);
            }

            // do include texture coordinates?
            if (m_VertexFormat & QR_Vertex::IE_VF_TexCoords)
            {
                // get vertex texture coordinates
                M_Precision tu = ((M_Precision*)pGLCmds)[0];
                M_Precision tv = ((M_Precision*)pGLCmds)[1];

                pVertex->m_Buffer.push_back(tu);
                pVertex->m_Buffer.push_back(tv);
            }

            // do include colors?
            if (m_VertexFormat & QR_Vertex::IE_VF_Colors)
            {
                QR_Color color;

                // get final color to apply to vertex
                switch (m_LightningMode)
                {
                    case IE_LM_Precalculated:
                        // calculate lightning
                        color  = GetPreCalculatedLight(normal, m_Light);
                        break;

                    case IE_LM_None:
                    default:
                        // use default color
                        color = m_Color;
                        break;
                }

                pVertex->m_Buffer.push_back(color.GetRedF());
                pVertex->m_Buffer.push_back(color.GetGreenF());
                pVertex->m_Buffer.push_back(color.GetBlueF());
                pVertex->m_Buffer.push_back(color.GetAlphaF());
            }
        }

        // add vertices to output
        vertices.push_back(pVertex.release());
    }

    return QR_MD2Common::IE_C_Success;
}
//------------------------------------------------------------------------------
QR_SizeT QR_MD2::GetVerticesCount() const
{
    return m_Mesh.m_Header.m_FrameCount;
}
//------------------------------------------------------------------------------
QR_Int32 QR_MD2::Interpolate(const QR_Float& position, const QR_Vertices& vertices1,
        const QR_Vertices& vertices2, QR_Vertices& result)
{
    // get vertice count
    const QR_SizeT count = vertices1.size();

    // are vertices compatible?
    if (count != vertices2.size())
        return QR_MD2Common::IE_C_IncompatibleVertices;

    // iterate through vertices to interpolate
    for (QR_SizeT i = 0; i < count; ++i)
    {
        // are frame compatibles?
        if (!vertices1[i]->CompareFormat(*vertices2[i]))
            return QR_MD2Common::IE_C_IncompatibleFrames;

        // create and populate new interpolation vertex
        std::auto_ptr<QR_Vertex> pVertex(new QR_Vertex());
        pVertex->m_Offset    = vertices1[i]->m_Offset;
        pVertex->m_Length    = vertices1[i]->m_Length;
        pVertex->m_Stride    = vertices1[i]->m_Stride;
        pVertex->m_Type      = vertices1[i]->m_Type;
        pVertex->m_Format    = vertices1[i]->m_Format;
        pVertex->m_CoordType = vertices1[i]->m_CoordType;

        // get vertex buffer data count
        const QR_SizeT bufferCount = vertices1[i]->m_Buffer.size();

        // not a 3D coordinate?
        if (vertices1[i]->m_CoordType != QR_Vertex::IE_VC_XYZ)
            return QR_MD2Common::IE_C_Not3DCoords;

        // iterate through vertex buffer content
        for (QR_SizeT j = 0; j < bufferCount; j += vertices1[i]->m_Stride)
        {
            QR_UInt32 index = 3;

            // get positions
            QR_Vector3DP srcVec(vertices1[i]->m_Buffer[j],
                                vertices1[i]->m_Buffer[j + 1],
                                vertices1[i]->m_Buffer[j + 2]);
            QR_Vector3DP dstVec(vertices2[i]->m_Buffer[j],
                                vertices2[i]->m_Buffer[j + 1],
                                vertices2[i]->m_Buffer[j + 2]);

            // interpolate positions
            QR_Vector3DP vec = srcVec.Interpolate(dstVec, position);

            // set interpolated positions in destination buffer
            pVertex->m_Buffer.push_back(vec.m_X);
            pVertex->m_Buffer.push_back(vec.m_Y);
            pVertex->m_Buffer.push_back(vec.m_Z);

            // do include normals?
            if (vertices1[i]->m_Format & QR_Vertex::IE_VF_Normals)
            {
                // get normals
                QR_Vector3DP srcNormal(vertices1[i]->m_Buffer[j + index],
                                       vertices1[i]->m_Buffer[j + index + 1],
                                       vertices1[i]->m_Buffer[j + index + 2]);
                QR_Vector3DP dstNormal(vertices2[i]->m_Buffer[j + index],
                                       vertices2[i]->m_Buffer[j + index + 1],
                                       vertices2[i]->m_Buffer[j + index + 2]);

                // interpolate normals
                QR_Vector3DP normal = srcNormal.Interpolate(dstNormal, position);

                // set interpolated normals in destination buffer
                pVertex->m_Buffer.push_back(normal.m_X);
                pVertex->m_Buffer.push_back(normal.m_Y);
                pVertex->m_Buffer.push_back(normal.m_Z);

                index += 3;
            }

            // do include texture coordinates?
            if (vertices1[i]->m_Format & QR_Vertex::IE_VF_TexCoords)
            {
                // copy texture coordinates from source
                pVertex->m_Buffer.push_back(vertices1[i]->m_Buffer[j + index]);
                pVertex->m_Buffer.push_back(vertices1[i]->m_Buffer[j + index + 1]);

                index += 2;
            }

            // do include colors?
            if (vertices1[i]->m_Format & QR_Vertex::IE_VF_Colors)
            {
                // copy color from source
                pVertex->m_Buffer.push_back(vertices1[i]->m_Buffer[j + index]);
                pVertex->m_Buffer.push_back(vertices1[i]->m_Buffer[j + index + 1]);
                pVertex->m_Buffer.push_back(vertices1[i]->m_Buffer[j + index + 2]);
                pVertex->m_Buffer.push_back(vertices1[i]->m_Buffer[j + index + 3]);
            }
        }

        // add interpolated vertice to output list
        result.push_back(pVertex.get());
        pVertex.release();
    }

    return QR_MD2Common::IE_C_Success;
}
//------------------------------------------------------------------------------
QR_Vector3DP QR_MD2::UncompressVertex(const QR_MD2Parser::IFrame*  pFrame,
                                      const QR_MD2Parser::IVertex* pVertex) const
{
    M_ASSERT(pFrame);
    M_ASSERT(pVertex);

    M_Precision vertex[3];

    // iterate through vertex coordinates
    for (QR_UInt8 i = 0; i < 3; ++i)
        // uncompress vertex using frame scale and translate values
        vertex[i] = (pFrame->m_Scale[i] * pVertex->m_Vertex[i]) + pFrame->m_Translate[i];

    return QR_Vector3DP(vertex[0], vertex[1], vertex[2]);
}
//------------------------------------------------------------------------------
QR_Color QR_MD2::GetPreCalculatedLight(const QR_Vector3DP& normal,
                                       const ILight&       light) const
{
    // calculate light angle
    M_Precision lightAngle = normal.Dot(light.m_Direction);

    // is light angle out of bounds?
    if (lightAngle < 0.0f)
        lightAngle = 0.0f;

    // calculate light color
    QR_UInt32 r = (QR_UInt32)(light.m_Light.GetRed()   * lightAngle) + light.m_Ambient.GetRed();
    QR_UInt32 g = (QR_UInt32)(light.m_Light.GetGreen() * lightAngle) + light.m_Ambient.GetGreen();
    QR_UInt32 b = (QR_UInt32)(light.m_Light.GetBlue()  * lightAngle) + light.m_Ambient.GetBlue();
    QR_UInt32 a = (QR_UInt32)(light.m_Light.GetAlpha() * lightAngle) + light.m_Ambient.GetAlpha();

    // is color red component out of bounds?
    if (r > 255)
        r = 255;

    // is color green component out of bounds?
    if (g > 255)
        g = 255;

    // is color blue component out of bounds?
    if (b > 255)
        b = 255;

    // is color alpha component out of bounds?
    if (a > 255)
        a = 255;

    return QR_Color(r, g, b, a);
}
//------------------------------------------------------------------------------

