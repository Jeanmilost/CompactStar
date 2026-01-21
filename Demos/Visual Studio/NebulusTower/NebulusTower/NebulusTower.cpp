/*****************************************************************************
 * ==> Nebulus tower demo ---------------------------------------------------*
 *****************************************************************************
 * Description : Nebulus tower demo                                          *
 * Developer   : Jean-Milost Reymond                                         *
 * Copyright   : 2015 - 2026, this file is part of the Minimal API. You are  *
 *               free to copy or redistribute this file, modify it, or use   *
 *               it for your own projects, commercial or not. This file is   *
 *               provided "as is", without ANY WARRANTY OF ANY KIND          *
 *****************************************************************************/

// std
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Geometry.h"
#include "CSR_Collision.h"
#include "CSR_Texture.h"
#include "CSR_Vertex.h"
#include "CSR_Model.h"
#include "CSR_Iqm.h"
#include "CSR_Scene.h"
#include "CSR_AI.h"
#include "CSR_Renderer_OpenGL.h"
#include "CSR_Sound.h"
#include "CSR_DebugHelper.h"

// classes
#include "CSR_OpenGLHelper.h"
#include "CSR_ShaderHelper.h"

// windows
#include <objbase.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// openGL
#include <gl/gl.h>
#define GLEW_STATIC
#include <GL/glew.h>

#ifdef _DEBUG
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif

// resources
#include "Resource.h"

#ifdef _DEBUG
    #define SHOW_SKELETON
#endif

// resource files to load
#define SKYBOX_LEFT          "left.bmp"
#define SKYBOX_TOP           "top.bmp"
#define SKYBOX_RIGHT         "right.bmp"
#define SKYBOX_BOTTOM        "bottom.bmp"
#define SKYBOX_FRONT         "front.bmp"
#define SKYBOX_BACK          "back.bmp"
#define IQM_FILE             "Robot.iqm"
#define TOWER_TEXTURE_FILE   "NebulusTowerBlue.bmp"
#define WATER_SOUND_FILE     "water.wav"
#define FOOTSTEPS_SOUND_FILE "footsteps_metal.wav"

//----------------------------------------------------------------------------
typedef std::vector<std::string> IFileNames;
//------------------------------------------------------------------------------
HDC                          g_hDC                  = 0;
HGLRC                        g_hRC                  = 0;
CSR_Scene*                   g_pScene               = nullptr;
CSR_OpenGLShader*            g_pSkyboxShader        = nullptr;
CSR_OpenGLShader*            g_pTextureShader       = nullptr;
CSR_OpenGLShader*            g_pColorShader         = nullptr;
CSR_OpenGLShader*            g_pLineShader          = nullptr;
CSR_OpenGLShader*            g_pWaterShader         = nullptr;
CSR_SceneContext             g_SceneContext;
CSR_ArcBall                  g_Arcball;
CSR_OpenGLHelper::IResources g_OpenGLResources;
std::vector<std::string>     g_TextureKeys;
ALCdevice*                   g_pOpenALDevice        = nullptr;
ALCcontext*                  g_pOpenALContext       = nullptr;
CSR_IQM*                     g_pPlayerModel         = nullptr;
CSR_Mesh*                    g_pTowerMesh           = nullptr;
CSR_Mesh*                    g_pWaterMesh           = nullptr;
CSR_Collider*                g_pPlayerCollider      = nullptr;
CSR_Sound*                   g_pFootstepsSound      = nullptr;
CSR_Sound*                   g_pWaterSound          = nullptr;
CSR_Matrix4                  g_PlayerMatrix;
CSR_Matrix4                  g_PlayerColliderMatrix;
CSR_Matrix4                  g_TowerMatrix;
CSR_Matrix4                  g_WaterMatrix;
std::string                  g_SceneDir;
std::size_t                  g_FrameCount           = 0;
std::size_t                  g_FPS                  = 20;
std::size_t                  g_AnimCount            = 0;
std::size_t                  g_MaxAnimFrame         = 60;
const std::size_t            g_PlatformCount        = 22;
float                        g_xPos                 = 0.0f;
float                        g_yPos                 = 0.0f;
float                        g_zPos                 = 0.0f;
float                        g_Distance             = 1.4f;
float                        g_PlayerShift          = 0.25f;
float                        g_WalkOffset           = 1.0f;
float                        g_Angle                = 0.0f;
float                        g_Velocity             = 0.75f;
float                        g_Gravity              = 0.5f;
float                        g_Time                 = 0.0f;
double                       g_TextureLastTime      = 0.0;
double                       g_ModelLastTime        = 0.0;
double                       g_MeshLastTime         = 0.0;
unsigned __int64             g_StartTime            = 0L;
unsigned __int64             g_PreviousTime         = 0L;
bool                         g_Walking              = false;
bool                         g_Initialized          = false;
CSR_Matrix4                  g_PlatformMatrix[g_PlatformCount];
CSR_Matrix4                  g_PlatformMatrixTop[g_PlatformCount];
CSR_Matrix4                  g_PlatformMatrixBottom[g_PlatformCount];
CSR_Collider*                g_PlatformCollider[g_PlatformCount];
CSR_Mesh*                    g_pPlatformBodyMesh[g_PlatformCount];
CSR_Mesh*                    g_pPlatformTopMesh[g_PlatformCount];
CSR_Mesh*                    g_pPlatformBottomMesh[g_PlatformCount];
//------------------------------------------------------------------------------
void UpdateScene(float elapsedTime);
void DrawScene();
//------------------------------------------------------------------------------
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
            break;

        case WM_DESTROY:
            return 0;

        case WM_SIZE:
        {
            if (!g_Initialized)
                break;

            const int width  = ((int)(short)LOWORD(lParam));
            const int height = ((int)(short)HIWORD(lParam));

            // update the viewport
            CSR_OpenGLHelper::CreateViewport((float)width,
                                             (float)height,
                                             0.1f,
                                             100.0f,
                                             g_pTextureShader,
                                             g_pScene->m_ProjectionMatrix);

            #ifdef SHOW_SKELETON
                // create the line shader viewport
                CSR_OpenGLHelper::CreateViewport((float)width,
                                                 (float)height,
                                                 0.1f,
                                                 1000.0f,
                                                 g_pLineShader,
                                                 g_pScene->m_ProjectionMatrix);
            #endif

            UpdateScene(0.0f);
            DrawScene();

            ::SwapBuffers(g_hDC);
            break;
        }

        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}
//---------------------------------------------------------------------------
GLuint LoadCubemap(const IFileNames fileNames)
{
    try
    {
        GLuint textureID = M_CSR_Error_Code;

        // create new OpenGL texture
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        const std::size_t fileNameCount = fileNames.size();

        // iterate through the cubemap texture files to load
        for (std::size_t i = 0; i < fileNameCount; i++)
        {
            // load the texture content from bitmap
            CSR_PixelBuffer* pPixelBuffer = csrPixelBufferFromBitmapFile(fileNames[i].c_str());

            if (!pPixelBuffer || !pPixelBuffer->m_Width || !pPixelBuffer->m_Height)
                return M_CSR_Error_Code;

            unsigned char* pPixels = 0;
            unsigned       x;
            unsigned       y;
            unsigned char  c;
            GLint          pixelType;
            GLuint         index = 0;

            // select the correct pixel type to use
            switch (pPixelBuffer->m_BytePerPixel)
            {
                case 3:  pixelType = GL_RGB;  break;
                case 4:  pixelType = GL_RGBA; break;
                default:                      return M_CSR_Error_Code;
            }

            // reorder the pixels
            pPixels = (unsigned char*)malloc(sizeof(unsigned char)  *
                                             pPixelBuffer->m_Width  *
                                             pPixelBuffer->m_Height *
                                             pPixelBuffer->m_BytePerPixel);

            if (!pPixels)
                return M_CSR_Error_Code;

            // get bitmap data into right format
            for (y = 0; y < pPixelBuffer->m_Height; ++y)
                for (x = 0; x < pPixelBuffer->m_Width; ++x)
                    for (c = 0; c < pPixelBuffer->m_BytePerPixel; ++c)
                        pPixels[pPixelBuffer->m_BytePerPixel * (pPixelBuffer->m_Width * y + x) + c] =
                                ((unsigned char*)pPixelBuffer->m_pData)
                                        [pPixelBuffer->m_Stride * y + pPixelBuffer->m_BytePerPixel * (pPixelBuffer->m_Width - x - 1) + (pPixelBuffer->m_BytePerPixel - 1 - c)];

            // load the texture on the GPU
            glTexImage2D((GLenum)(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i),
                         0,
                         pixelType,
                         pPixelBuffer->m_Width,
                         pPixelBuffer->m_Height,
                         0,
                         pixelType,
                         GL_UNSIGNED_BYTE,
                         pPixels);

            free(pPixels);
            csrPixelBufferRelease(pPixelBuffer);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,     GL_CLAMP_TO_EDGE);

        return textureID;
    }
    catch (...)
    {
        return M_CSR_Error_Code;
    }
}
//---------------------------------------------------------------------------
GLuint LoadTexture(const char* pFileName)
{
    if (!pFileName)
        return M_CSR_Error_Code;

    try
    {
        // load the texture content from bitmap
        CSR_PixelBuffer* pPixelBuffer = csrPixelBufferFromBitmapFile(pFileName);

        // load the texture on the GPU
        GLuint textureID = csrOpenGLTextureFromPixelBuffer(pPixelBuffer);

        // clear the memory
        csrPixelBufferRelease(pPixelBuffer);

        glBindTexture(GL_TEXTURE_2D, textureID);

        // set texture wrapping mode
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        return textureID;
    }
    catch (...)
    {
        return M_CSR_Error_Code;
    }
}
//---------------------------------------------------------------------------
CSR_PixelBuffer* OnLoadTexture(const char* pTextureName)
{
    const std::string resName = g_SceneDir + pTextureName;

    return csrPixelBufferFromTgaFile((resName.substr(0, resName.length() - 3) + "tga").c_str());
}
//---------------------------------------------------------------------------
void OnApplySkin(size_t index, const CSR_Skin* pSkin, int* pCanRelease)
{
    if (!pSkin)
        return;

    if (!pSkin->m_Texture.m_pFileName)
        return;

    if (!pSkin->m_Texture.m_pBuffer)
        return;

    const std::string key = pSkin->m_Texture.m_pFileName;

    CSR_OpenGLID* pID = static_cast<CSR_OpenGLID*>(CSR_OpenGLHelper::GetTextureID(key, g_OpenGLResources));

    if (!pID)
        CSR_OpenGLHelper::AddTexture(key,
                                     csrOpenGLTextureFromPixelBuffer(pSkin->m_Texture.m_pBuffer),
                                     g_OpenGLResources);

    *pCanRelease = 1;
}
//---------------------------------------------------------------------------
void* OnGetShader(const void* pModel, CSR_EModelType type)
{
    if (pModel == g_pPlayerModel || pModel == g_pTowerMesh)
        return g_pTextureShader;
    else
    if (pModel == g_pScene->m_pSkybox)
        return g_pSkyboxShader;

    for (std::size_t i = 0; i < g_PlatformCount; ++i)
        if (pModel == g_pPlatformBodyMesh[i] || pModel == g_pPlatformTopMesh[i] || pModel == g_pPlatformBottomMesh[i] )
            return g_pColorShader;

    return g_pWaterShader;
}
//---------------------------------------------------------------------------
void* OnGetID(const void* pKey)
{
    const CSR_Texture* pTexture = static_cast<const CSR_Texture*>(pKey);

    if (!pTexture->m_pFileName)
        return nullptr;

    const std::string key = pTexture->m_pFileName;

    return CSR_OpenGLHelper::GetTextureID(key, g_OpenGLResources);
}
//---------------------------------------------------------------------------
void OnGetIQMIndex(const CSR_IQM* pIQM, size_t* pAnimSetIndex, size_t* pFrameIndex)
{
    *pAnimSetIndex = 0;
    *pFrameIndex   = (g_AnimCount / 10) % g_MaxAnimFrame;
}
//---------------------------------------------------------------------------
void OnDeleteTexture(const CSR_Texture* pTexture)
{
    if (!pTexture)
        return;

    if (!pTexture->m_pFileName)
        return;

    const std::string key = pTexture->m_pFileName;

    CSR_OpenGLHelper::DeleteTexture(key, g_OpenGLResources);
}
//---------------------------------------------------------------------------
void BuildModelMatrix(CSR_Matrix4* pMatrix, bool isCollider)
{
    CSR_Vector3 axis   = {};
    CSR_Vector3 factor = {};
    CSR_Matrix4 rotateXMatrix;
    CSR_Matrix4 rotateYMatrix;
    CSR_Matrix4 scaleMatrix;
    CSR_Matrix4 intermediateMatrix;

    csrMat4Identity(pMatrix);

    // set rotation axis
    axis.m_X = 1.0f;
    axis.m_Y = 0.0f;
    axis.m_Z = 0.0f;

    // create the rotation matrix
    csrMat4Rotate(-((float)M_PI / 2.0), &axis, &rotateXMatrix);

    // set rotation axis
    axis.m_X = 0.0f;
    axis.m_Y = 1.0f;
    axis.m_Z = 0.0f;

    // create the rotation matrix
    csrMat4Rotate(-g_Angle - ((float)M_PI / 2.0f) * g_WalkOffset, &axis, &rotateYMatrix);

    // set scale factor
    factor.m_X = isCollider ? 1.0f : 0.1f;
    factor.m_Y = isCollider ? 1.0f : 0.1f;
    factor.m_Z = isCollider ? 1.0f : 0.1f;

    // create the scale matrix
    csrMat4Scale(&factor, &scaleMatrix);

    // build the model matrix
    csrMat4Multiply(&scaleMatrix,        &rotateXMatrix, &intermediateMatrix);
    csrMat4Multiply(&intermediateMatrix, &rotateYMatrix, pMatrix);

    // place it in the world
    pMatrix->m_Table[3][0] = g_xPos - (g_Distance * sinf(g_Angle));
    pMatrix->m_Table[3][1] = g_yPos -  g_PlayerShift;
    pMatrix->m_Table[3][2] = g_zPos + (g_Distance * cosf(g_Angle));
}
//---------------------------------------------------------------------------
void OnSceneBegin(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    csrDrawBegin(&pScene->m_Color);
}
//---------------------------------------------------------------------------
void OnSceneEnd(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    #ifdef SHOW_SKELETON
        csrShaderEnable(g_pLineShader);

        csrOpenGLShaderConnectViewMatrix(g_pLineShader,
                                         &g_pScene->m_ViewMatrix);

        // connect model matrix to shader
        GLint slot = glGetUniformLocation(g_pLineShader->m_ProgramID, "csr_uModel");
        glUniformMatrix4fv(slot, 1, 0, &g_PlayerMatrix.m_Table[0][0]);

        csrDebugDrawSkeletonIQM(g_pPlayerModel,
                                g_pLineShader,
                                0,
                                (g_AnimCount / 10) % g_MaxAnimFrame);
    #endif

    csrDrawEnd();
}
//------------------------------------------------------------------------------
bool InitScene(int w, int h)
{
    CSR_VertexFormat  vertexFormat  = {};
    CSR_VertexCulling vertexCulling = {};
    CSR_Material      material      = {};

    // initialize the scene
    g_pScene = csrSceneCreate();

    // configure the scene color background
    g_pScene->m_Color.m_R = 0.08f;
    g_pScene->m_Color.m_G = 0.12f;
    g_pScene->m_Color.m_B = 0.17f;
    g_pScene->m_Color.m_A = 1.0f;

    // configure the scene view matrix
    csrMat4Identity(&g_pScene->m_ViewMatrix);

    // configure the scene context
    csrSceneContextInit(&g_SceneContext);
    g_SceneContext.m_fOnGetShader     = OnGetShader;
    g_SceneContext.m_fOnGetID         = OnGetID;
    g_SceneContext.m_fOnGetIQMIndex   = OnGetIQMIndex;
    g_SceneContext.m_fOnDeleteTexture = OnDeleteTexture;
    g_SceneContext.m_fOnSceneBegin    = OnSceneBegin;
    g_SceneContext.m_fOnSceneEnd      = OnSceneEnd;

    // initialize the arcball
    g_Arcball.m_Position.m_X = g_xPos;
    g_Arcball.m_Position.m_Y = g_yPos;
    g_Arcball.m_Position.m_Z = g_zPos;
    g_Arcball.m_AngleX       = 0.0f;
    g_Arcball.m_AngleY       = g_Angle;
    g_Arcball.m_Radius       = 4.0f;

    const std::string vsSkybox = CSR_ShaderHelper::GetVertexShader  (CSR_ShaderHelper::IEShaderType::IE_ST_Skybox);
    const std::string fsSkybox = CSR_ShaderHelper::GetFragmentShader(CSR_ShaderHelper::IEShaderType::IE_ST_Skybox);

    // load the skybox shader
    g_pSkyboxShader = csrOpenGLShaderLoadFromStr(vsSkybox.c_str(),
                                                 vsSkybox.length(),
                                                 fsSkybox.c_str(),
                                                 fsSkybox.length(),
                                                 0,
                                                 0);

    // succeeded?
    if (!g_pSkyboxShader)
        return false;

    // enable the shader program
    csrShaderEnable(g_pSkyboxShader);

    // create the viewport and connect projection matrix to shader
    CSR_OpenGLHelper::CreateViewport((float)w,
                                     (float)h,
                                     0.1f,
                                     100.0f,
                                     g_pSkyboxShader,
                                     g_pScene->m_ProjectionMatrix);

    // connect view matrix to shader
    const GLint skyboxViewUniform = glGetUniformLocation(g_pSkyboxShader->m_ProgramID, "csr_uView");
    glUniformMatrix4fv(skyboxViewUniform, 1, 0, &g_pScene->m_ViewMatrix.m_Table[0][0]);

    // get shader attributes
    g_pSkyboxShader->m_VertexSlot  = glGetAttribLocation (g_pSkyboxShader->m_ProgramID, "csr_aVertices");
    g_pSkyboxShader->m_CubemapSlot = glGetUniformLocation(g_pSkyboxShader->m_ProgramID, "csr_sCubemap");

    // create the skybox
    g_pScene->m_pSkybox = csrSkyboxCreate(1.0f, 1.0f, 1.0f);

    // succeeded?
    if (!g_pScene->m_pSkybox)
        return false;

    IFileNames cubemapFileNames;
    cubemapFileNames.push_back(g_SceneDir + SKYBOX_RIGHT);
    cubemapFileNames.push_back(g_SceneDir + SKYBOX_LEFT);
    cubemapFileNames.push_back(g_SceneDir + SKYBOX_TOP);
    cubemapFileNames.push_back(g_SceneDir + SKYBOX_BOTTOM);
    cubemapFileNames.push_back(g_SceneDir + SKYBOX_FRONT);
    cubemapFileNames.push_back(g_SceneDir + SKYBOX_BACK);

    // load the cubemap texture
    CSR_OpenGLHelper::AddTexture(&g_pScene->m_pSkybox->m_Skin.m_CubeMap,
                                 LoadCubemap(cubemapFileNames),
                                 g_OpenGLResources);

    #ifdef SHOW_SKELETON
        // get the shaders
        const std::string vsLine = CSR_ShaderHelper::GetVertexShader  (CSR_ShaderHelper::IEShaderType::IE_ST_Line);
        const std::string fsLine = CSR_ShaderHelper::GetFragmentShader(CSR_ShaderHelper::IEShaderType::IE_ST_Line);

        // load the line shader
        g_pLineShader = csrOpenGLShaderLoadFromStr(vsLine.c_str(),
                                                   vsLine.length(),
                                                   fsLine.c_str(),
                                                   fsLine.length(),
                                                   0,
                                                   0);

        if (!g_pLineShader)
            return false;

        csrShaderEnable(g_pLineShader);

        // create the viewport
        CSR_OpenGLHelper::CreateViewport((float)w,
                                         (float)h,
                                         0.1f,
                                         1000.0f,
                                         g_pLineShader,
                                         g_pScene->m_ProjectionMatrix);

        // get shader attributes
        g_pLineShader->m_VertexSlot = glGetAttribLocation(g_pLineShader->m_ProgramID, "csr_aVertices");
        g_pLineShader->m_ColorSlot  = glGetAttribLocation(g_pLineShader->m_ProgramID, "csr_aColor");
    #endif

    // get the shaders
    const std::string vsTextured = CSR_ShaderHelper::GetVertexShader  (CSR_ShaderHelper::IEShaderType::IE_ST_Texture);
    const std::string fsTextured = CSR_ShaderHelper::GetFragmentShader(CSR_ShaderHelper::IEShaderType::IE_ST_Texture);

    // load the shader
    g_pTextureShader = csrOpenGLShaderLoadFromStr(vsTextured.c_str(),
                                                  vsTextured.length(),
                                                  fsTextured.c_str(),
                                                  fsTextured.length(),
                                                  0,
                                                  0);

    // succeeded?
    if (!g_pTextureShader)
        return false;

    // enable the shader program
    csrShaderEnable(g_pTextureShader);

    // connect projection matrix to shader
    const GLint textureProjectionUniform = glGetUniformLocation(g_pTextureShader->m_ProgramID, "csr_uProjection");
    glUniformMatrix4fv(textureProjectionUniform, 1, 0, &g_pScene->m_ProjectionMatrix.m_Table[0][0]);

    // connect view matrix to shader
    const GLint textureViewUniform = glGetUniformLocation(g_pTextureShader->m_ProgramID, "csr_uView");
    glUniformMatrix4fv(textureViewUniform, 1, 0, &g_pScene->m_ViewMatrix.m_Table[0][0]);

    // get shader attributes
    g_pTextureShader->m_VertexSlot   = glGetAttribLocation (g_pTextureShader->m_ProgramID, "csr_aVertices");
    g_pTextureShader->m_ColorSlot    = glGetAttribLocation (g_pTextureShader->m_ProgramID, "csr_aColor");
    g_pTextureShader->m_TexCoordSlot = glGetAttribLocation (g_pTextureShader->m_ProgramID, "csr_aTexCoord");
    g_pTextureShader->m_TextureSlot  = glGetUniformLocation(g_pTextureShader->m_ProgramID, "csr_sTexture");

    // configure the vertex format
    vertexFormat.m_HasNormal         = 0;
    vertexFormat.m_HasTexCoords      = 1;
    vertexFormat.m_HasPerVertexColor = 1;

    // set vertex culling
    vertexCulling.m_Face = CSR_CF_CW;
    vertexCulling.m_Type = CSR_CT_Back;

    // configure the material
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;
    material.m_uScale      = 1.0f;
    material.m_vScale      = 1.0f;

    // load the IQM model
    g_pPlayerModel = csrIQMOpen((g_SceneDir + IQM_FILE).c_str(),
                                &vertexFormat,
                                &vertexCulling,
                                &material,
                                 0,
                                 0,
                                 0,
                                 OnLoadTexture,
                                 OnApplySkin,
                                 0);

    // build the player model matrix
    BuildModelMatrix(&g_PlayerMatrix,         false);
    BuildModelMatrix(&g_PlayerColliderMatrix, true);

    // create the player collider
    g_pPlayerCollider               = csrColliderCreate();
    g_pPlayerCollider->m_State      = CSR_CS_Dynamic;
    g_pPlayerCollider->m_Pos.m_X    = g_xPos;
    g_pPlayerCollider->m_Pos.m_Y    = g_yPos;
    g_pPlayerCollider->m_Pos.m_Z    = g_zPos;
    g_pPlayerCollider->m_Radius     = 0.15f;
    g_pPlayerCollider->m_TopY       = 0.15f;
    g_pPlayerCollider->m_BottomY    = 0.0f;
    g_pPlayerCollider->m_fOnSupport = csrGJKSupportCapsule;
    csrColliderSetRS(&g_PlayerColliderMatrix, g_pPlayerCollider);

    // add the model to the scene
    CSR_SceneItem* pSceneItem   = csrSceneAddIQM(g_pScene, g_pPlayerModel, 0, 0);
    pSceneItem->m_CollisionType = CSR_CO_GJK;
    pSceneItem->m_pCollider     = g_pPlayerCollider;
    csrSceneAddModelMatrix(g_pScene, g_pPlayerModel, &g_PlayerMatrix);

    // load the player footsteps sound file
    g_pFootstepsSound = csrSoundOpenWavFile(g_pOpenALDevice, g_pOpenALContext, (g_SceneDir + FOOTSTEPS_SOUND_FILE).c_str());

    // loop the sound
    csrSoundLoop(g_pFootstepsSound, 1);

    // change the volume
    csrSoundChangeVolume(g_pFootstepsSound, 1.0f);

    // stop the sound by default
    csrSoundStop(g_pFootstepsSound);

    // set vertex format for textured models
    vertexFormat.m_Type              = CSR_EVertexType::CSR_VT_Triangles;
    vertexFormat.m_HasNormal         = 0;
    vertexFormat.m_HasTexCoords      = 1;
    vertexFormat.m_HasPerVertexColor = 1;

    // set vertex culling
    vertexCulling.m_Face = CSR_CF_CCW;

    // create material
    material.m_Color  = 0xFFFFFFFF;
    material.m_uScale = 10.0f;
    material.m_vScale = 10.0f;

    // create the tower
    g_pTowerMesh = csrShapeCreateCylinder(1.2f, 1.2f, 10.0f, 50, &vertexFormat, &vertexCulling, &material, NULL);

    const std::size_t fileNameLen = strlen(TOWER_TEXTURE_FILE);

    // allocate memory for file name
    g_pTowerMesh->m_Skin.m_Texture.m_pFileName = (char*)malloc((fileNameLen + 1) * sizeof(char));

    if (!g_pTowerMesh->m_Skin.m_Texture.m_pFileName)
        return false;

    // copy the texture file name
    memcpy(g_pTowerMesh->m_Skin.m_Texture.m_pFileName, TOWER_TEXTURE_FILE, fileNameLen);
    g_pTowerMesh->m_Skin.m_Texture.m_pFileName[fileNameLen] = 0x0;

    // load background texture
    CSR_OpenGLHelper::AddTexture(std::string(g_pTowerMesh->m_Skin.m_Texture.m_pFileName),
                                 LoadTexture((g_SceneDir + TOWER_TEXTURE_FILE).c_str()),
                                 g_OpenGLResources);

    // configure the tower matrix
    csrMat4Identity(&g_TowerMatrix);
    g_TowerMatrix.m_Table[3][1] = 4.0f;

    // add the tower item to the scene
    csrSceneAddMesh(g_pScene, g_pTowerMesh, false, false);
    csrSceneAddModelMatrix(g_pScene, g_pTowerMesh, &g_TowerMatrix);

    // get the shaders
    const std::string vsColored = CSR_ShaderHelper::GetVertexShader  (CSR_ShaderHelper::IEShaderType::IE_ST_Color);
    const std::string fsColored = CSR_ShaderHelper::GetFragmentShader(CSR_ShaderHelper::IEShaderType::IE_ST_Color);

    // load the shader
    g_pColorShader = csrOpenGLShaderLoadFromStr(vsColored.c_str(),
                                                vsColored.length(),
                                                fsColored.c_str(),
                                                fsColored.length(),
                                                0,
                                                0);

    // succeeded?
    if (!g_pColorShader)
        return false;

    // enable the shader program
    csrShaderEnable(g_pColorShader);

    // connect projection matrix to shader
    const GLint colorProjectionUniform = glGetUniformLocation(g_pColorShader->m_ProgramID, "csr_uProjection");
    glUniformMatrix4fv(colorProjectionUniform, 1, 0, &g_pScene->m_ProjectionMatrix.m_Table[0][0]);

    // connect view matrix to shader
    const GLint colorViewUniform = glGetUniformLocation(g_pColorShader->m_ProgramID, "csr_uView");
    glUniformMatrix4fv(colorViewUniform, 1, 0, &g_pScene->m_ViewMatrix.m_Table[0][0]);

    // get shader attributes
    g_pColorShader->m_VertexSlot = glGetAttribLocation(g_pColorShader->m_ProgramID, "csr_aVertices");
    g_pColorShader->m_ColorSlot  = glGetAttribLocation(g_pColorShader->m_ProgramID, "csr_aColor");

    // set vertex format for colored models
    vertexFormat.m_Type              = CSR_EVertexType::CSR_VT_Triangles;
    vertexFormat.m_HasNormal         = 0;
    vertexFormat.m_HasTexCoords      = 0;
    vertexFormat.m_HasPerVertexColor = 1;

    const float step       = ((float)M_PI * 2.0f) / (float)g_PlatformCount;
    const float heightStep = 0.05f;

    // create first platform row
    for (std::size_t i = 0; i < g_PlatformCount; ++i)
    {
        // calculate the platform position
        csrMat4Identity(&g_PlatformMatrix[i]);
        g_PlatformMatrix[i].m_Table[3][0] =  g_Distance * sinf(i * step);
        g_PlatformMatrix[i].m_Table[3][1] = -0.44f + (i >= (g_PlatformCount >> 1) ? (g_PlatformCount - i) * heightStep : i * heightStep);
        g_PlatformMatrix[i].m_Table[3][2] =  g_Distance * cosf(i * step);

        // create material
        material.m_Color = 0x69615eff;

        // create the tower platform model
        g_pPlatformBodyMesh[i] = csrShapeCreateCylinder(0.2f, 0.2f, 0.1f, 15, &vertexFormat, &vertexCulling, &material, NULL);

        // create the platform collider
        g_PlatformCollider[i]               =  csrColliderCreate();
        g_PlatformCollider[i]->m_State      =  CSR_CS_Static;
        g_PlatformCollider[i]->m_Pos.m_X    =  g_PlatformMatrix[i].m_Table[3][0];
        g_PlatformCollider[i]->m_Pos.m_Y    =  g_PlatformMatrix[i].m_Table[3][1];
        g_PlatformCollider[i]->m_Pos.m_Z    =  g_PlatformMatrix[i].m_Table[3][2];
        g_PlatformCollider[i]->m_Radius     =  0.2f;
        g_PlatformCollider[i]->m_TopY       =  0.05f;
        g_PlatformCollider[i]->m_BottomY    = -0.05f;
        g_PlatformCollider[i]->m_fOnSupport =  csrGJKSupportCylinder;
        csrColliderSetRS(&g_PlatformMatrix[i], g_PlatformCollider[i]);

        // add the platform body item to the scene
        CSR_SceneItem* pSceneItem   = csrSceneAddMesh(g_pScene, g_pPlatformBodyMesh[i], false, false);
        pSceneItem->m_CollisionType = CSR_CO_GJK;
        pSceneItem->m_pCollider     = g_PlatformCollider[i];
        csrSceneAddModelMatrix(g_pScene, g_pPlatformBodyMesh[i], &g_PlatformMatrix[i]);

        CSR_Vector3 axis;

        // set rotation axis
        axis.m_X = 1.0f;
        axis.m_Y = 0.0f;
        axis.m_Z = 0.0f;

        // initialize the platform top matrix
        csrMat4Rotate(-((float)M_PI / 2.0), &axis, &g_PlatformMatrixTop[i]);

        // place the platform top closure in the world
        g_PlatformMatrixTop[i].m_Table[3][0] = g_PlatformMatrix[i].m_Table[3][0];
        g_PlatformMatrixTop[i].m_Table[3][1] = g_PlatformMatrix[i].m_Table[3][1] + 0.05f;
        g_PlatformMatrixTop[i].m_Table[3][2] = g_PlatformMatrix[i].m_Table[3][2];

        // create material
        material.m_Color = 0xcfc7c4ff;

        // create the tower platform model
        g_pPlatformTopMesh[i] = csrShapeCreateDisk(0.0f, 0.0f, 0.2f, 15, &vertexFormat, &vertexCulling, &material, NULL);

        // add the platform closure item to the scene
        csrSceneAddMesh(g_pScene, g_pPlatformTopMesh[i], false, false);
        csrSceneAddModelMatrix(g_pScene, g_pPlatformTopMesh[i], &g_PlatformMatrixTop[i]);

        // initialize the platform bottom matrix
        csrMat4Rotate(((float)M_PI / 2.0), &axis, &g_PlatformMatrixBottom[i]);

        // place the platform bottom closure in the world
        g_PlatformMatrixBottom[i].m_Table[3][0] = g_PlatformMatrix[i].m_Table[3][0];
        g_PlatformMatrixBottom[i].m_Table[3][1] = g_PlatformMatrix[i].m_Table[3][1] - 0.05f;
        g_PlatformMatrixBottom[i].m_Table[3][2] = g_PlatformMatrix[i].m_Table[3][2];

        // create the tower platform model
        g_pPlatformBottomMesh[i] = csrShapeCreateDisk(0.0f, 0.0f, 0.2f, 15, &vertexFormat, &vertexCulling, &material, NULL);

        // add the platform closure item to the scene
        csrSceneAddMesh(g_pScene, g_pPlatformBottomMesh[i], false, false);
        csrSceneAddModelMatrix(g_pScene, g_pPlatformBottomMesh[i], &g_PlatformMatrixBottom[i]);
    }

    // set vertex format for water model
    vertexFormat.m_HasNormal         = 0;
    vertexFormat.m_HasTexCoords      = 1;
    vertexFormat.m_HasPerVertexColor = 0;

    // create material
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;
    material.m_uScale      = 1.0f;
    material.m_vScale      = 1.0f;

    // create the water plan
    g_pWaterMesh = csrShapeCreateWaterSurface(100.0f, 100.0f, 50, &vertexFormat, &vertexCulling, &material, NULL);

    // configure the water matrix
    csrMat4Identity(&g_WaterMatrix);
    g_WaterMatrix.m_Table[3][1] = -1.0f;

    // add the water item to the scene
    csrSceneAddMesh(g_pScene, g_pWaterMesh, false, false);
    csrSceneAddModelMatrix(g_pScene, g_pWaterMesh, &g_WaterMatrix);

    // get the shaders
    const std::string vsWater = CSR_ShaderHelper::GetVertexShader  (CSR_ShaderHelper::IEShaderType::IE_ST_Water);
    const std::string fsWater = CSR_ShaderHelper::GetFragmentShader(CSR_ShaderHelper::IEShaderType::IE_ST_Water);

    // load the water shader
    g_pWaterShader = csrOpenGLShaderLoadFromStr(vsWater.c_str(),
                                                vsWater.length(),
                                                fsWater.c_str(),
                                                fsWater.length(),
                                                0,
                                                0);

    csrShaderEnable(g_pWaterShader);

    // connect projection matrix to shader
    const GLint waterProjectionUniform = glGetUniformLocation(g_pWaterShader->m_ProgramID, "csr_uProjection");
    glUniformMatrix4fv(waterProjectionUniform, 1, 0, &g_pScene->m_ProjectionMatrix.m_Table[0][0]);

    // connect view matrix to shader
    const GLint waterViewUniform = glGetUniformLocation(g_pWaterShader->m_ProgramID, "csr_uView");
    glUniformMatrix4fv(waterViewUniform, 1, 0, &g_pScene->m_ViewMatrix.m_Table[0][0]);

    // connect model matrix to shader
    const GLint waterModelUniform = glGetUniformLocation(g_pWaterShader->m_ProgramID, "csr_uModel");
    glUniformMatrix4fv(waterModelUniform, 1, 0, &g_WaterMatrix.m_Table[0][0]);

    // get shader attributes
    g_pWaterShader->m_VertexSlot   = glGetAttribLocation(g_pWaterShader->m_ProgramID,  "csr_aVertices");
    g_pWaterShader->m_TexCoordSlot = glGetAttribLocation(g_pWaterShader->m_ProgramID,  "csr_aTexCoord");

    // configure water
    glUniform1f(glGetUniformLocation((GLuint)g_pWaterShader->m_ProgramID, "csr_uWaveStrength"),  0.1f);
    glUniform3f(glGetUniformLocation((GLuint)g_pWaterShader->m_ProgramID, "csr_LightDir"),      -0.3f, -1.0f, -0.3f);
    glUniform3f(glGetUniformLocation((GLuint)g_pWaterShader->m_ProgramID, "csr_WaterColor"),     0.1f,  0.3f,  0.4f);
    glUniform3f(glGetUniformLocation((GLuint)g_pWaterShader->m_ProgramID, "csr_DeepWaterColor"), 0.0f,  0.1f,  0.2f);
    glUniform1f(glGetUniformLocation((GLuint)g_pWaterShader->m_ProgramID, "csr_WaterClearness"), 0.15f);

    // load the water sound file
    g_pWaterSound = csrSoundOpenWavFile(g_pOpenALDevice, g_pOpenALContext, (g_SceneDir + WATER_SOUND_FILE).c_str());

    // loop the sound
    csrSoundLoop(g_pWaterSound, 1);

    // change the volume
    csrSoundChangeVolume(g_pWaterSound, 0.5f);

    // play the sound
    csrSoundPlay(g_pWaterSound);

    g_Initialized = true;
    return true;
}
//------------------------------------------------------------------------------
void DeleteScene()
{
    g_Initialized = false;

    // delete the scene
    csrSceneRelease(g_pScene, OnDeleteTexture);
    g_pScene = 0;

    #ifdef SHOW_SKELETON
        // delete line shader
        csrOpenGLShaderRelease(g_pLineShader);
        g_pLineShader = 0;
    #endif

    // delete water shader
    csrOpenGLShaderRelease(g_pWaterShader);
    g_pWaterShader = 0;

    // delete color shader
    csrOpenGLShaderRelease(g_pColorShader);
    g_pColorShader = 0;

    // delete texture shader
    csrOpenGLShaderRelease(g_pTextureShader);
    g_pTextureShader = 0;

    // delete skybox shader
    csrOpenGLShaderRelease(g_pSkyboxShader);
    g_pSkyboxShader = 0;

    // stop running sounds, if needed
    csrSoundStop(g_pWaterSound);
    csrSoundStop(g_pFootstepsSound);

    // release OpenAL interface
    csrSoundRelease(g_pWaterSound);
    csrSoundRelease(g_pFootstepsSound);
}
//------------------------------------------------------------------------------
void UpdateScene(float elapsedTime)
{
    // left (or "A") or right (or "D") key pressed?
    if ((::GetKeyState(VK_LEFT) & 0x8000) || (::GetKeyState(65) & 0x8000))
    {
        g_Walking    = true;
        g_WalkOffset = 1.0f;
    }
    else
    if ((::GetKeyState(VK_RIGHT) & 0x8000) || (::GetKeyState(68) & 0x8000))
    {
        g_Walking    =  true;
        g_WalkOffset = -1.0f;
    }
    else
        g_Walking = false;

    // apply the gravity to the player
    g_yPos -= g_Gravity * (float)elapsedTime;

    // is player walking?
    if (g_Walking)
    {
        // calculate the next position
        g_Angle += g_Velocity * (float)g_WalkOffset * (float)elapsedTime;

        // play the walk animation
        g_AnimCount += 4;

        // play the walk sound, if not playing
        if (!csrSoundIsPlaying(g_pFootstepsSound))
            csrSoundPlay(g_pFootstepsSound);
    }
    else
    {
        // turn the animation idle
        g_AnimCount = 0;

        // stop the walk sound
        if (csrSoundIsPlaying(g_pFootstepsSound))
            csrSoundStop(g_pFootstepsSound);
    }

    // rebuild the model matrix
    BuildModelMatrix(&g_PlayerMatrix,         false);
    BuildModelMatrix(&g_PlayerColliderMatrix, true);

    // update player collider position
    g_pPlayerCollider->m_Pos.m_X = g_xPos - (g_Distance * sinf(g_Angle));
    g_pPlayerCollider->m_Pos.m_Y = g_yPos -  g_PlayerShift;
    g_pPlayerCollider->m_Pos.m_Z = g_zPos + (g_Distance * cosf(g_Angle));

    CSR_CollisionInput colInput;
    csrCollisionInputInit(&colInput);

    CSR_CollisionOutput colOutput;
    csrCollisionOutputInit(&colOutput);

    // detect the collisions in the scene
    csrSceneDetectCollision(g_pScene, &colInput, &colOutput, 0);

    // found a collision?
    if (colOutput.m_pColliders && colOutput.m_pColliders->m_Count)
    {
        // update the player position
        g_yPos += colOutput.m_MinTransVec.m_Y;

        // update player position
        g_PlayerMatrix.m_Table[3][0] = g_xPos - (g_Distance * sinf(g_Angle));
        g_PlayerMatrix.m_Table[3][1] = g_yPos -  g_PlayerShift;
        g_PlayerMatrix.m_Table[3][2] = g_zPos + (g_Distance * cosf(g_Angle));

        // update player collider position
        g_pPlayerCollider->m_Pos.m_X = g_xPos - (g_Distance * sinf(g_Angle));
        g_pPlayerCollider->m_Pos.m_Y = g_yPos -  g_PlayerShift;
        g_pPlayerCollider->m_Pos.m_Z = g_zPos + (g_Distance * cosf(g_Angle));
    }

    // update the arcball
    g_Arcball.m_Position.m_X =  g_xPos;
    g_Arcball.m_Position.m_Y = -g_yPos;
    g_Arcball.m_Position.m_Z =  g_zPos;
    g_Arcball.m_AngleY       =  g_Angle;

    // get the view matrix matching with the arcball
    csrSceneArcBallToMatrix(&g_Arcball, &g_pScene->m_ViewMatrix);

    csrShaderEnable(g_pWaterShader);

    // update time
    g_Time += (float)elapsedTime;
    glUniform1f(glGetUniformLocation((GLuint)g_pWaterShader->m_ProgramID, "csr_uTime"), g_Time);

    // calculate camera position from arcball parameters
    float camX = g_Arcball.m_Position.m_X + g_Arcball.m_Radius * sinf(g_Arcball.m_AngleY) * cosf(g_Arcball.m_AngleX);
    float camY = g_Arcball.m_Position.m_Y + g_Arcball.m_Radius * sinf(g_Arcball.m_AngleX);
    float camZ = g_Arcball.m_Position.m_Z + g_Arcball.m_Radius * cosf(g_Arcball.m_AngleY) * cosf(g_Arcball.m_AngleX);

    // set water camera position
    glUniform3f(glGetUniformLocation((GLuint)g_pWaterShader->m_ProgramID, "csr_CameraPos"), camX, camY, camZ);
}
//------------------------------------------------------------------------------
void DrawScene()
{
    // draw the scene
    csrSceneDraw(g_pScene, &g_SceneContext);
}
//------------------------------------------------------------------------------
int APIENTRY wWinMain(_In_     HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_     LPWSTR    lpCmdLine,
                      _In_     int       nCmdShow)
{
    if (!SUCCEEDED(::CoInitialize(nullptr)))
        return -1;

    // initialize memory leaks detection structures
    #ifdef _DEBUG
        _CrtMemState sOld;
        _CrtMemState sNew;
        _CrtMemState sDiff;

        // take a memory snapshot before execution begins
        ::_CrtMemCheckpoint(&sOld);
    #endif

    WNDCLASSEX wcex  = {};
    HWND       hWnd  = 0;
    MSG        msg   = {};
    BOOL       bQuit = FALSE;

    // register the window class
    wcex.cbSize        =  sizeof(WNDCLASSEX);
    wcex.style         =  CS_OWNDC;
    wcex.lpfnWndProc   =  WindowProc;
    wcex.cbClsExtra    =  0;
    wcex.cbWndExtra    =  0;
    wcex.hInstance     =  hInstance;
    wcex.hIcon         =  ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON));
    wcex.hIconSm       =  ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));
    wcex.hCursor       =  ::LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground =  (HBRUSH)::GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName  =  NULL;
    wcex.lpszClassName = L"CSR_NebulusTowerDemo";

    if (!RegisterClassEx(&wcex))
        return 0;

    // create the main window
    hWnd = ::CreateWindowEx(0,
                           L"CSR_NebulusTowerDemo",
                           L"Nebulus tower demo",
                            WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            800,
                            650,
                            NULL,
                            NULL,
                            hInstance,
                            NULL);

    ::ShowWindow(hWnd, nCmdShow);

    // get the global scene directory
    g_SceneDir = "Resources\\";

    // initialize OpenAL
    csrSoundInitializeOpenAL(&g_pOpenALDevice, &g_pOpenALContext);

    // enable OpenGL
    CSR_OpenGLHelper::EnableOpenGL(hWnd, &g_hDC, &g_hRC);

    // stop GLEW crashing on OSX :-/
    glewExperimental = GL_TRUE;

    // initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        // shutdown OpenGL
        CSR_OpenGLHelper::DisableOpenGL(hWnd, g_hDC, g_hRC);

        ::CoUninitialize();

        // close the app
        return 0;
    }

    RECT clientRect;
    ::GetClientRect(hWnd, &clientRect);

    // initialize the scene
    if (!InitScene(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top))
    {
        // shutdown OpenGL
        CSR_OpenGLHelper::DisableOpenGL(hWnd, g_hDC, g_hRC);

        ::CoUninitialize();

        // close the app
        return 0;
    }

    // initialize the timer
    g_StartTime    = ::GetTickCount64();
    g_PreviousTime = ::GetTickCount64();

    // application main loop
    while (!bQuit)
    {
        // check for messages
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // handle or dispatch messages
            if (msg.message == WM_QUIT)
                bQuit = TRUE;
            else
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }
        }
        else
        {
            // calculate time interval
            const unsigned __int64 now            = ::GetTickCount64();
            const double           elapsedTime    = (now - g_PreviousTime) / 1000.0;
                                   g_PreviousTime =  now;

            if (!g_Initialized)
                continue;

            ++g_FrameCount;

            // calculate the FPS
            if (g_FrameCount >= 100)
            {
                const double      smoothing = 0.1;
                const std::size_t fpsTime   = (std::size_t)(now > g_StartTime ? now - g_StartTime : 1L);
                const std::size_t newFPS    = (g_FrameCount * 100) / fpsTime;
                g_StartTime                 = ::GetTickCount64();
                g_FrameCount                = 0;
                g_FPS                       = (std::size_t)(((double)newFPS * smoothing) + ((double)g_FPS * (1.0 - smoothing)));
            }

            UpdateScene((float)elapsedTime);
            DrawScene();

            ::SwapBuffers(g_hDC);
        }
    }

    // delete the scene completely
    DeleteScene();

    // delete the scene textures
    CSR_OpenGLHelper::ClearResources(g_OpenGLResources);

    // shutdown OpenGL
    CSR_OpenGLHelper::DisableOpenGL(hWnd, g_hDC, g_hRC);

    // release OpenAL interface
    csrSoundReleaseOpenAL(g_pOpenALDevice, g_pOpenALContext);

    // destroy the window explicitly
    ::DestroyWindow(hWnd);

    // detect memory leaks, log them if found
    #ifdef _DEBUG
        // take a memory snapshot after execution ends
        ::_CrtMemCheckpoint(&sNew);

        // found a difference between memories?
        if (::_CrtMemDifference(&sDiff, &sOld, &sNew))
        {
            ::OutputDebugString(L"-----------_CrtMemDumpStatistics ---------\n");
            ::_CrtMemDumpStatistics(&sDiff);
            ::OutputDebugString(L"-----------_CrtMemDumpAllObjectsSince ---------\n");
            ::_CrtMemDumpAllObjectsSince(&sOld);
            ::OutputDebugString(L"-----------_CrtDumpMemoryLeaks ---------\n");
            ::_CrtDumpMemoryLeaks();
        }
    #endif

    ::CoUninitialize();

    return (int)msg.wParam;
}
//------------------------------------------------------------------------------
