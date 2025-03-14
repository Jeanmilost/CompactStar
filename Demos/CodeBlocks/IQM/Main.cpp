/*****************************************************************************
 * ==> Inter-Quake model demo -----------------------------------------------*
 *****************************************************************************
 * Description : Animated Inter-Quake Model demo                             *
 * Developer   : Jean-Milost Reymond                                         *
 * Copyright   : 2015 - 2022, this file is part of the Minimal API. You are  *
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
#include "CSR_DebugHelper.h"

// classes
#include "CSR_OpenGLHelper.h"
#include "CSR_ShaderHelper.h"

// windows
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// openGL
#include <gl/gl.h>
#define GLEW_STATIC
#include <GL/glew.h>

// resources
#include "Resources.rh"

#ifdef _DEBUG
    #define SHOW_SKELETON
#endif

// resource files to load
#define IQM_FILE  "mrfixit.iqm"

//------------------------------------------------------------------------------
HDC                          g_hDC             = 0;
HGLRC                        g_hRC             = 0;
CSR_Scene*                   g_pScene          = NULL;
CSR_OpenGLShader*            g_pShader         = NULL;
CSR_OpenGLShader*            g_pLineShader     = NULL;
CSR_SceneContext             g_SceneContext;
CSR_OpenGLHelper::IResources g_OpenGLResources;
std::vector<std::string>     g_TextureKeys;
CSR_IQM*                     g_pModel          = NULL;
CSR_Matrix4                  g_Matrix;
std::string                  g_SceneDir;
size_t                       g_FrameCount      = 0;
size_t                       g_FPS             = 20;
size_t                       g_AnimCount       = 0;
size_t                       g_MaxAnimFrame    = 101;
float                        g_Angle           = 0.0f;
double                       g_TextureLastTime = 0.0;
double                       g_ModelLastTime   = 0.0;
double                       g_MeshLastTime    = 0.0;
unsigned __int64             g_StartTime       = 0L;
unsigned __int64             g_PreviousTime    = 0L;
bool                         g_Initialized     = false;
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
                                             g_pShader,
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
CSR_PixelBuffer* OnLoadTexture(const char* pTextureName)
{
    const std::string resName = g_SceneDir + pTextureName;

    return csrPixelBufferFromTgaFile(resName.c_str());
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

    CSR_OpenGLHelper::AddTexture(key,
                                 csrOpenGLTextureFromPixelBuffer(pSkin->m_Texture.m_pBuffer),
                                 g_OpenGLResources);

    *pCanRelease = 1;
}
//---------------------------------------------------------------------------
void* OnGetShader(const void* pModel, CSR_EModelType type)
{
    return g_pShader;
}
//---------------------------------------------------------------------------
void* OnGetID(const void* pKey)
{
    const CSR_Texture* pTexture = static_cast<const CSR_Texture*>(pKey);

    if (!pTexture->m_pFileName)
        return NULL;

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
void BuildModelMatrix(CSR_Matrix4* pMatrix)
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
    csrMat4Rotate((float)(-M_PI / 4.0) + g_Angle, &axis, &rotateYMatrix);

    // set scale factor
    factor.m_X = 0.15f;
    factor.m_Y = 0.15f;
    factor.m_Z = 0.15f;

    // create the scale matrix
    csrMat4Scale(&factor, &scaleMatrix);

    // build the model matrix
    csrMat4Multiply(&scaleMatrix,        &rotateXMatrix, &intermediateMatrix);
    csrMat4Multiply(&intermediateMatrix, &rotateYMatrix, pMatrix);

    // place it in the world
    pMatrix->m_Table[3][0] =  0.0f;
    pMatrix->m_Table[3][1] = -0.55f;
    pMatrix->m_Table[3][2] = -2.0f;
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
        glUniformMatrix4fv(slot, 1, 0, &g_Matrix.m_Table[0][0]);

        csrDebugDrawSkeletonIQM(g_pModel,
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

    #ifdef SHOW_SKELETON
        // get the shaders
        const std::string vsLine = CSR_ShaderHelper::GetVertexShader  (CSR_ShaderHelper::IE_ST_Line);
        const std::string fsLine = CSR_ShaderHelper::GetFragmentShader(CSR_ShaderHelper::IE_ST_Line);

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
    const std::string vsTextured = CSR_ShaderHelper::GetVertexShader  (CSR_ShaderHelper::IE_ST_Texture);
    const std::string fsTextured = CSR_ShaderHelper::GetFragmentShader(CSR_ShaderHelper::IE_ST_Texture);

    // load the shader
    g_pShader = csrOpenGLShaderLoadFromStr(vsTextured.c_str(),
                                            vsTextured.length(),
                                            fsTextured.c_str(),
                                            fsTextured.length(),
                                            0,
                                            0);

    // succeeded?
    if (!g_pShader)
        return false;

    // enable the shader program
    csrShaderEnable(g_pShader);

    // get shader attributes
    g_pShader->m_VertexSlot   = glGetAttribLocation (g_pShader->m_ProgramID, "csr_aVertices");
    g_pShader->m_ColorSlot    = glGetAttribLocation (g_pShader->m_ProgramID, "csr_aColor");
    g_pShader->m_TexCoordSlot = glGetAttribLocation (g_pShader->m_ProgramID, "csr_aTexCoord");
    g_pShader->m_TextureSlot  = glGetUniformLocation(g_pShader->m_ProgramID, "csr_sTexture");

    // create the viewport
    CSR_OpenGLHelper::CreateViewport((float)w,
                                     (float)h,
                                     0.1f,
                                     100.0f,
                                     g_pShader,
                                     g_pScene->m_ProjectionMatrix);

    // configure the vertex format
    vertexFormat.m_HasNormal         = 0;
    vertexFormat.m_HasTexCoords      = 1;
    vertexFormat.m_HasPerVertexColor = 1;

    vertexCulling.m_Face = CSR_CF_CW;
    vertexCulling.m_Type = CSR_CT_Back;

    // configure the material
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    // load the IQM model
    g_pModel = csrIQMOpen((g_SceneDir + IQM_FILE).c_str(),
                         &vertexFormat,
                         &vertexCulling,
                         &material,
                          0,
                          0,
                          0,
                          OnLoadTexture,
                          OnApplySkin,
                          0);

    BuildModelMatrix(&g_Matrix);

    // add the model to the scene
    csrSceneAddIQM(g_pScene, g_pModel, 0, 0);
    csrSceneAddModelMatrix(g_pScene, g_pModel, &g_Matrix);

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

    // delete scene shader
    csrOpenGLShaderRelease(g_pShader);
    g_pShader = 0;

    // delete line shader
    csrOpenGLShaderRelease(g_pLineShader);
    g_pLineShader = 0;
}
//------------------------------------------------------------------------------
void UpdateScene(float elapsedTime)
{
    // rebuild the model matrix
    BuildModelMatrix(&g_Matrix);

    g_Angle = fmodf(g_Angle + 0.0025f, (float)M_PI * 2.0f);

    ++g_AnimCount;
}
//------------------------------------------------------------------------------
void DrawScene()
{
    // draw the scene
    csrSceneDraw(g_pScene, &g_SceneContext);
}
//------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR     lpCmdLine,
                   int       nCmdShow)
{
    WNDCLASSEX wcex  = {};
    HWND       hWnd  = 0;
    MSG        msg   = {};
    BOOL       bQuit = FALSE;

    // try to load application icon from resources
    HICON hIcon = (HICON)LoadImage(GetModuleHandle(NULL),
                                   MAKEINTRESOURCE(IDI_MAIN_ICON),
                                   IMAGE_ICON,
                                   16,
                                   16,
                                   0);

    // register the window class
    wcex.cbSize        = sizeof(WNDCLASSEX);
    wcex.style         = CS_OWNDC;
    wcex.lpfnWndProc   = WindowProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = hInstance;
    wcex.hIcon         = hIcon;
    wcex.hIconSm       = hIcon;
    wcex.hCursor       = ::LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName  = NULL;
    wcex.lpszClassName = "CSR_IQMDemo";

    if (!RegisterClassEx(&wcex))
        return 0;

    // create the main window
    hWnd = ::CreateWindowEx(0,
                            "CSR_IQMDemo",
                            "Animated Inter-Quake model demo",
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
    g_SceneDir = "..\\..\\..\\Common\\Models\\IQM\\MrFixit\\";

    // enable OpenGL
    CSR_OpenGLHelper::EnableOpenGL(hWnd, &g_hDC, &g_hRC);

    // stop GLEW crashing on OSX :-/
    glewExperimental = GL_TRUE;

    // initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        // shutdown OpenGL
        CSR_OpenGLHelper::DisableOpenGL(hWnd, g_hDC, g_hRC);

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

        // close the app
        return 0;
    }

    // initialize the timer
    g_StartTime    = ::GetTickCount();
    g_PreviousTime = ::GetTickCount();

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
            const unsigned __int64 now            = ::GetTickCount();
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
                g_StartTime                 = ::GetTickCount();
                g_FrameCount                = 0;
                g_FPS                       = (std::size_t)(((double)newFPS * smoothing) + ((double)g_FPS * (1.0 - smoothing)));
            }

            UpdateScene((float)elapsedTime);
            DrawScene();

            ::SwapBuffers(g_hDC);

            ::Sleep(1);
        }
    }

    // delete the scene completely
    DeleteScene();

    // delete the scene textures
    CSR_OpenGLHelper::ClearResources(g_OpenGLResources);

    // shutdown OpenGL
    CSR_OpenGLHelper::DisableOpenGL(hWnd, g_hDC, g_hRC);

    // destroy the window explicitly
    ::DestroyWindow(hWnd);

    return (int)msg.wParam;
}
//------------------------------------------------------------------------------
