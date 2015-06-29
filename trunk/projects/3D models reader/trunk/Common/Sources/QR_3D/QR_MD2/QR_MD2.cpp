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

//------------------------------------------------------------------------------
// QR_MD2::IVertexBuffer - c++ cross-platform
//------------------------------------------------------------------------------
QR_MD2::IVertexBuffer::IVertexBuffer()
{}
//------------------------------------------------------------------------------
QR_MD2::IVertexBuffer::~IVertexBuffer()
{}
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
// QR_MD2::IAnimationInfo - c++ cross-platform
//------------------------------------------------------------------------------
QR_MD2::IAnimationInfo::IAnimationInfo()
{}
//------------------------------------------------------------------------------
QR_MD2::IAnimationInfo::IAnimationInfo(QR_SizeT           start,
                                       QR_SizeT           end,
                                       const std::string& name) :
    m_Start(start),
    m_End(end),
    m_Name(name)
{}
//------------------------------------------------------------------------------
QR_MD2::IAnimationInfo::~IAnimationInfo()
{}
//------------------------------------------------------------------------------
// QR_MD2 - c++ cross-platform
//------------------------------------------------------------------------------
QR_MD2::QR_MD2() : QR_Model()
{
    #ifdef _DEBUG
        #if defined(__BORLANDC__) || defined(__CODEGEARC__)
            #pragma warn -8008
            #pragma warn -8066
        #endif

        // check if vertex buffer data type is 4 bytes aligned (to avoid slow draw)
        if (sizeof(IDataType) % 4)
            M_THROW_EXCEPTION("Misaligned vertex buffer data type - must be 4 bytes aligned");

        #if defined(__BORLANDC__) || defined(__CODEGEARC__)
            #pragma warn .8066
            #pragma warn .8008
        #endif
    #endif
}
//------------------------------------------------------------------------------
QR_MD2::~QR_MD2()
{}
//------------------------------------------------------------------------------
bool QR_MD2::LoadMesh(const std::string& fileName, const QR_Float& version)
{
    return m_Mesh.Load(fileName, version);
}
//------------------------------------------------------------------------------
bool QR_MD2::LoadNormalsTable(const std::string& fileName, const QR_Float& version)
{
    // clear previous table, if needed
    m_Normals.clear();

    // is file name empty?
    if (fileName.empty())
        return false;

    // try to open file
    std::FILE* pFile = std::fopen(fileName.c_str(), "rb");

    // succeeded?
    if (!pFile)
        return false;

    QR_Float fileVersion;

    // read version
    std::fread(&fileVersion, sizeof(QR_Float), 1, pFile);

    // is version correct?
    if (fileVersion != version)
    {
        std::fclose(pFile);
        return false;
    }

    QR_UInt32 length;

    // read file length
    std::fread(&length, sizeof(QR_UInt32), 1, pFile);

    // reserve memory for normals table
    m_Normals.reserve(length);

    // iterate through normals
    for (QR_UInt32 i = 0; i < length; ++i)
    {
        QR_Vector3DP normal;

        // read normal from file
        std::fread(&normal, sizeof(QR_Vector3DP), 1, pFile);

        // set normal in table
        m_Normals.push_back(normal);
    }

    std::fclose(pFile);
    return true;
}
//------------------------------------------------------------------------------
bool QR_MD2::SetAnimation(IAnimationInfo animationInfo)
{
    // is animation info indexes out of bounds?
    if (animationInfo.m_Start > m_Mesh.m_Header.m_FrameCount ||
        animationInfo.m_End > m_Mesh.m_Header.m_FrameCount)
        return false;

    // configure animation to run
    m_AnimationInfo    = animationInfo;
    m_CurFrameID       = animationInfo.m_Start;
    m_NextFrameID      = animationInfo.m_Start + 1;
    m_InterpolationPos = 0.0f;
    return true;
}
//------------------------------------------------------------------------------
bool QR_MD2::Animate(const QR_Double& elapsedTime, QR_SizeT fps)
{
    // are animation info correct?
    if ((m_AnimationInfo.m_Start > m_Mesh.m_Header.m_FrameCount - 1) ||
        (m_AnimationInfo.m_End   > m_Mesh.m_Header.m_FrameCount - 1))
        return false;

    // calculate time interval between each frames
    const QR_Double timeInterval = (1000.0f / (QR_Double)fps);
    // calculate next interpolation position
    m_InterpolationPos += (elapsedTime / timeInterval);

    // max interpolation position reached?    if (m_InterpolationPos >= 1.0f)
    {
        // calculate number of frames to skip
        const QR_SizeT framesToSkip = std::floor(m_InterpolationPos);

        // update interpolation position (should always be between 0 and 1)
        m_InterpolationPos -= framesToSkip;
        // go to next frame
        m_CurFrameID  += framesToSkip;
        m_NextFrameID  = m_CurFrameID + 1;
    }

    // calculate animation length
    const QR_SizeT animationLength =
            (m_AnimationInfo.m_End - m_AnimationInfo.m_Start) + 1;

    // current frame end reached?
    if (m_CurFrameID > m_AnimationInfo.m_End)
        m_CurFrameID = m_AnimationInfo.m_Start +
                ((m_CurFrameID - m_AnimationInfo.m_Start) % animationLength);

    // next frame end reached?
    if (m_NextFrameID > m_AnimationInfo.m_End)
        m_NextFrameID = m_AnimationInfo.m_Start +
                ((m_NextFrameID - m_AnimationInfo.m_Start) % animationLength);

    return true;
}
//------------------------------------------------------------------------------
#ifdef QRENGINE_USE_OBSOLETE_FUNCTIONS
    bool QR_MD2::GetVertexBuffer(IBuffer&            vb,
                                 IELightningMode     lightningMode,
                                 const ILight&       preCalculatedLight,
                                 QR_Vertex::IEFormat vertexFormat,
                                 bool                convertRHLH) const
    {
        // are current and next frame indexes out of bounds?
        if ((m_CurFrameID  > m_Mesh.m_Header.m_FrameCount - 1) ||
            (m_NextFrameID > m_Mesh.m_Header.m_FrameCount - 1))
            return false;

        // pre-calculated normals table was populated?
        if (!m_Normals.size())
            return false;

        // get current and next frames from mesh
        QR_MD2Parser::IFrame* pCurFrame  = &m_Mesh.m_pFrame[m_CurFrameID];
        QR_MD2Parser::IFrame* pNextFrame = &m_Mesh.m_pFrame[m_NextFrameID];

        // found them?
        if (!pCurFrame || !pNextFrame)
            return false;

        // iterate through frame polygons
        for (QR_UInt32 i = 0; i < m_Mesh.m_Header.m_PolygonCount; ++i)
            // iterate through polygon vertexes
            for (QR_UInt8 j = 0; j < 3; ++j)
            {
                // get current and next vertexes
                QR_MD2Parser::IVertex* pCurVertex  =
                        &pCurFrame->m_pVertex[m_Mesh.m_pPolygon[i].m_VertexIndex[j]];
                QR_MD2Parser::IVertex* pNextVertex =
                        &pNextFrame->m_pVertex[m_Mesh.m_pPolygon[i].m_VertexIndex[j]];

                // found them?
                if (!pCurVertex || !pNextVertex)
                    return false;

                // get vertexes to interpolate
                QR_Vector3DP curVertex  = UncompressVertex(pCurFrame,  pCurVertex);
                QR_Vector3DP nextVertex = UncompressVertex(pNextFrame, pNextVertex);

                // interpolate current and next vertex
                QR_Vector3DP vertex = Interpolate(curVertex,
                                                  nextVertex,
                                                  m_InterpolationPos);

                // do convert right hand <-> left hand coordinate system?
                if (convertRHLH)
                    // apply conversion
                    vertex.m_X = -vertex.m_X;

                // populate vertex buffer
                vb.push_back(vertex.m_X);
                vb.push_back(vertex.m_Y);
                vb.push_back(vertex.m_Z);

                QR_Vector3DP normal;

                // do include normals?
                if (vertexFormat & QR_Vertex::IE_VF_Normals)
                {
                    // get vertex normals to interpolate
                    QR_Vector3DP curNormal  = m_Normals[pCurVertex->m_NormalIndex];
                    QR_Vector3DP nextNormal = m_Normals[pNextVertex->m_NormalIndex];

                    // interpolate current and next vertex normals
                    normal = Interpolate(curNormal,
                                         nextNormal,
                                         m_InterpolationPos);

                    // do convert right hand <-> left hand coordinate system?
                    if (convertRHLH)
                        // apply conversion
                        normal.m_X = -normal.m_X;

                    vb.push_back(normal.m_X);
                    vb.push_back(normal.m_Y);
                    vb.push_back(normal.m_Z);
                }

                // do include texture coordinates?
                if (vertexFormat & QR_Vertex::IE_VF_TexCoords)
                {
                    // get vertex texture coordinates
                    M_Precision tu =
                            (M_Precision)(m_Mesh.m_pTexCoord[m_Mesh.m_pPolygon[i].
                                                m_TextureCoordIndex[j]].m_U) /
                                          m_Mesh.m_Header.m_SkinWidth;
                    M_Precision tv =
                            (M_Precision)(m_Mesh.m_pTexCoord[m_Mesh.m_pPolygon[i].
                                                m_TextureCoordIndex[j]].m_V) /
                                          m_Mesh.m_Header.m_SkinHeight;

                    vb.push_back(tu);
                    vb.push_back(tv);
                }

                // do include colors?
                if (vertexFormat & QR_Vertex::IE_VF_Colors)
                {
                    // get vertex color
                    QR_Color color = GetVertexColor(normal,
                                                    lightningMode,
                                                    preCalculatedLight);

                    vb.push_back(color.GetRedF());
                    vb.push_back(color.GetGreenF());
                    vb.push_back(color.GetBlueF());
                    vb.push_back(color.GetAlphaF());
                }
            }

        return true;
    }
#endif
//------------------------------------------------------------------------------
bool QR_MD2::GetVertexBuffer(IVertexBufferList&  vbs,
                             IELightningMode     lightningMode,
                             const ILight&       preCalculatedLight,
                             QR_Vertex::IEFormat vertexFormat,
                             bool                convertRHLH) const
{
    // are current and next frame indexes out of bounds?
    if ((m_CurFrameID  > m_Mesh.m_Header.m_FrameCount - 1) ||
        (m_NextFrameID > m_Mesh.m_Header.m_FrameCount - 1))
        return false;

    // pre-calculated normals table was populated?
    if (!m_Normals.size())
        return false;

    // get current and next frames from mesh
    QR_MD2Parser::IFrame* pCurFrame  = &m_Mesh.m_pFrame[m_CurFrameID];
    QR_MD2Parser::IFrame* pNextFrame = &m_Mesh.m_pFrame[m_NextFrameID];

    // found them?
    if (!pCurFrame || !pNextFrame)
        return false;

    QR_Int32* pGLCmds = m_Mesh.m_pGlCmds;

    // iterate through OpenGL commands (negative value is for triangle fan,
    // positive value is for triangle strip, 0 means list end)
    while (QR_Int32 i = *pGLCmds)
    {
        // the first command is the number of vertexes to process, read above,
        // so skip it
        ++pGLCmds;

        // create new vertex buffer for the current command
        std::auto_ptr<IVertexBuffer> pVertexBuffer(new IVertexBuffer());

        // search for OpenGL command type
        if (i < 0)
        {
            pVertexBuffer->m_Type = QR_Vertex::IE_VT_TriangleFan;
            i = -i;
        }
        else
            pVertexBuffer->m_Type = QR_Vertex::IE_VT_TriangleStrip;

        // iterate through OpenGL commands to process
        for (; i > 0; --i, pGLCmds += 3)
        {
            // get current and next vertexes
            QR_MD2Parser::IVertex* pCurVertex  = &pCurFrame->m_pVertex[pGLCmds[2]];
            QR_MD2Parser::IVertex* pNextVertex = &pNextFrame->m_pVertex[pGLCmds[2]];

            // found them?
            if (!pCurVertex || !pNextVertex)
                return false;

            // get vertexes to interpolate
            QR_Vector3DP curVertex  = UncompressVertex(pCurFrame,  pCurVertex);
            QR_Vector3DP nextVertex = UncompressVertex(pNextFrame, pNextVertex);

            // interpolate current and next vertex
            QR_Vector3DP vertex = Interpolate(curVertex,
                                              nextVertex,
                                              m_InterpolationPos);

            // do convert right hand <-> left hand coordinate system?
            if (convertRHLH)
                // apply conversion
                vertex.m_X = -vertex.m_X;

            // populate vertex buffer
            pVertexBuffer->m_Buffer.push_back(vertex.m_X);
            pVertexBuffer->m_Buffer.push_back(vertex.m_Y);
            pVertexBuffer->m_Buffer.push_back(vertex.m_Z);

            QR_Vector3DP normal;

            // do include normals?
            if (vertexFormat & QR_Vertex::IE_VF_Normals)
            {
                // get vertex normals to interpolate
                QR_Vector3DP curNormal  = m_Normals[pCurVertex->m_NormalIndex];
                QR_Vector3DP nextNormal = m_Normals[pNextVertex->m_NormalIndex];

                // interpolate current and next vertex normals
                normal = Interpolate(curNormal,
                                     nextNormal,
                                     m_InterpolationPos);

                // do convert right hand <-> left hand coordinate system?
                if (convertRHLH)
                    // apply conversion
                    normal.m_X = -normal.m_X;

                pVertexBuffer->m_Buffer.push_back(normal.m_X);
                pVertexBuffer->m_Buffer.push_back(normal.m_Y);
                pVertexBuffer->m_Buffer.push_back(normal.m_Z);
            }

            // do include texture coordinates?
            if (vertexFormat & QR_Vertex::IE_VF_TexCoords)
            {
                // get vertex texture coordinates
                M_Precision tu = ((M_Precision*)pGLCmds)[0];
                M_Precision tv = ((M_Precision*)pGLCmds)[1];

                pVertexBuffer->m_Buffer.push_back(tu);
                pVertexBuffer->m_Buffer.push_back(tv);
            }

            // do include colors?
            if (vertexFormat & QR_Vertex::IE_VF_Colors)
            {
                // get vertex color
                QR_Color color = GetVertexColor(normal,
                                                lightningMode,
                                                preCalculatedLight);

                pVertexBuffer->m_Buffer.push_back(color.GetRedF());
                pVertexBuffer->m_Buffer.push_back(color.GetGreenF());
                pVertexBuffer->m_Buffer.push_back(color.GetBlueF());
                pVertexBuffer->m_Buffer.push_back(color.GetAlphaF());
            }
        }

        // add vertex buffer to list
        vbs.push_back(pVertexBuffer.release());
    }

    return true;
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
QR_Color QR_MD2::GetVertexColor(const QR_Vector3DP& normal,
                                IELightningMode     lightningMode,
                                const ILight&       preCalculatedLight) const
{
    // search for lightning mode
    switch (lightningMode)
    {
        case IE_L_Precalculated:
            return GetPreCalculatedLight(normal, preCalculatedLight);

        case IE_L_Unknown:
        case IE_L_None:
        case IE_L_Native:
        default:
            // white color
            return QR_Color(255, 255, 255, 255);
    }
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
QR_Vector3DP QR_MD2::Interpolate(const QR_Vector3DP& v1,
                                 const QR_Vector3DP& v2,
                                 const M_Precision&  position) const
{
    QR_Vector3DP interpolation;

    // calculate interpolation
    interpolation.m_X = v1.m_X + position * (v2.m_X - v1.m_X);
    interpolation.m_Y = v1.m_Y + position * (v2.m_Y - v1.m_Y);
    interpolation.m_Z = v1.m_Z + position * (v2.m_Z - v1.m_Z);

    return interpolation;
}
//------------------------------------------------------------------------------
