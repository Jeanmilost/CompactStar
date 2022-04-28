/****************************************************************************
 * ==> Collada model demo --------------------------------------------------*
 ****************************************************************************
 * Description : Collada (.dae) rigged and textured model demo              *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2022, this file is part of the CompactStar Engine.  *
 *               You are free to copy or redistribute this file, modify it, *
 *               or use it for your own projects, commercial or not. This   *
 *               file is provided "as is", WITHOUT ANY WARRANTY OF ANY      *
 *               KIND. THE DEVELOPER IS NOT RESPONSIBLE FOR ANY DAMAGE OF   *
 *               ANY KIND, ANY LOSS OF DATA, OR ANY LOSS OF PRODUCTIVITY    *
 *               TIME THAT MAY RESULT FROM THE USAGE OF THIS SOURCE CODE,   *
 *               DIRECTLY OR NOT.                                           *
 ****************************************************************************/

// std
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Geometry.h"
#include "CSR_Model.h"
#include "CSR_Collada.h"
#include "CSR_Renderer_OpenGL.h"
#include "CSR_Scene.h"

#ifdef _DEBUG
    #include "CSR_DebugHelper.h"
#endif

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
#include "Resource.h"

#ifdef _DEBUG
    //#define SHOW_SKELETON
#endif

// resource files to load
#define COLLADA_FILE "Resources\\Juliet.dae"

//----------------------------------------------------------------------------
typedef std::vector<std::string> IFileNames;
//------------------------------------------------------------------------------
HDC                          g_hDC             = 0;
HGLRC                        g_hRC             = 0;
CSR_Scene*                   g_pScene          = nullptr;
CSR_Collada*                 g_pCollada        = nullptr;
CSR_SceneContext             g_SceneContext;
CSR_OpenGLShader*            g_pShader         = nullptr;
#ifdef SHOW_SKELETON
    CSR_OpenGLShader*        g_pLineShader     = nullptr;
#endif
CSR_OpenGLHelper::IResources g_OpenGLResources;
CSR_Matrix4                  g_ModelMatrix;
std::string                  g_SceneDir;
std::size_t                  g_FrameCount;
int                          g_PrevOrigin      = 0;
float                        g_Angle           = (float)(M_PI / 2);
double                       g_FPS             = 0.0;
unsigned __int64             g_StartTime       = 0L;
unsigned __int64             g_PreviousTime    = 0L;
bool                         g_Initialized     = false;
std::vector<std::string>     g_TextureKeys;
//------------------------------------------------------------------------------
GLuint LoadTexture(const char* pFileName);
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
                                             1000.0f,
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
    return 0;
}
//---------------------------------------------------------------------------
void OnApplySkin(size_t index, const CSR_Skin* pSkin, int* pCanRelease)
{
    if (!pSkin)
        return;

    if (!pSkin->m_Texture.m_pFileName)
        return;

    // NOTE don't respect the global g_SceneDir folder, because the contained
    // texture format isn't compatible with the below LoadTexture() function
    const std::string resPath = "Resources\\";
    const std::string urlName = pSkin->m_Texture.m_pFileName;
          std::string fileName;

    if (urlName == "file:///MI_CH_Main_Juliet_Hair_Cine_Cos22")
        fileName = resPath + "TX_CH_Main_Juliet_Hair_D_Cos22.bmp";
    else
    if (urlName == "file:///MI_CH_Main_Juliet_Skin_Cine_Cos22")
        fileName = resPath + "TX_CH_Main_Juliet_Skin_D_Cos22.bmp";
    else
    if (urlName == "file:///MI_CH_Main_Juliet_Eyelash_Cine_Cos22")
        fileName = resPath + "TX_CH_Main_Juliet_Skin_D_Cos22.bmp";
    else
    if (urlName == "file:///MI_CH_Main_Juliet_Cloth_Cine_Cos22")
        fileName = resPath + "TX_CH_Main_Juliet_Cloth_D_Cos22.bmp";
    else
        return;

    const std::string key = pSkin->m_Texture.m_pFileName;

    CSR_OpenGLHelper::AddTexture(key,
                                 LoadTexture(fileName.c_str()),
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
        return nullptr;

    const std::string key = pTexture->m_pFileName;

    return CSR_OpenGLHelper::GetTextureID(key, g_OpenGLResources);
}
//---------------------------------------------------------------------------
void OnGetColladaIndex(const CSR_Collada* pX, size_t* pAnimSetIndex, size_t* pFrameIndex)
{
    *pAnimSetIndex = 0;
    *pFrameIndex   = 0;
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
        glUniformMatrix4fv(slot, 1, 0, &g_ModelMatrix.m_Table[0][0]);

        csrDebugDrawSkeletonCollada(g_pCollada,
                                    g_pLineShader,
                                    0,
                                    0);
    #endif

    csrDrawEnd();
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

        return textureID;
    }
    catch (...)
    {
        return M_CSR_Error_Code;
    }
}
//------------------------------------------------------------------------------
bool InitScene(int w, int h)
{
    // create the default scene
    g_pScene = csrSceneCreate();

    // create the scene context
    csrSceneContextInit(&g_SceneContext);
    g_SceneContext.m_fOnSceneBegin      = OnSceneBegin;
    g_SceneContext.m_fOnSceneEnd        = OnSceneEnd;
    g_SceneContext.m_fOnGetShader       = OnGetShader;
    g_SceneContext.m_fOnGetColladaIndex = OnGetColladaIndex;
    g_SceneContext.m_fOnGetID           = OnGetID;

    // configure the scene color background
    g_pScene->m_Color.m_R = 0.08f;
    g_pScene->m_Color.m_G = 0.12f;
    g_pScene->m_Color.m_B = 0.17f;

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

        // succeeded?
        if (!g_pLineShader)
            return false;

        // enable the shader program
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

    // initialize the matrices
    csrMat4Identity(&g_pScene->m_ViewMatrix);
    csrMat4Identity(&g_ModelMatrix);

    // get the shaders
    const std::string vsTextured = CSR_ShaderHelper::GetVertexShader  (CSR_ShaderHelper::IEShaderType::IE_ST_Texture);
    const std::string fsTextured = CSR_ShaderHelper::GetFragmentShader(CSR_ShaderHelper::IEShaderType::IE_ST_Texture);

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

    // create the viewport
    CSR_OpenGLHelper::CreateViewport((float)w,
                                     (float)h,
                                     0.1f,
                                     1000.0f,
                                     g_pShader,
                                     g_pScene->m_ProjectionMatrix);

    // get shader attributes
    g_pShader->m_VertexSlot   = glGetAttribLocation (g_pShader->m_ProgramID, "csr_aVertices");
    g_pShader->m_ColorSlot    = glGetAttribLocation (g_pShader->m_ProgramID, "csr_aColor");
    g_pShader->m_TexCoordSlot = glGetAttribLocation (g_pShader->m_ProgramID, "csr_aTexCoord");
    g_pShader->m_TextureSlot  = glGetUniformLocation(g_pShader->m_ProgramID, "csr_sTexture");

    g_TextureKeys.reserve(4);

    CSR_VertexFormat vf    = {};
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    CSR_VertexCulling vc = {};
    vc.m_Type            = CSR_CT_Back;
    vc.m_Face            = CSR_CF_CCW;

    CSR_Material material  = {};
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    // load the .dae model
    g_pCollada = csrColladaOpen(COLLADA_FILE,
                               &vf,
                               &vc,
                               &material,
                                0,
                                1,
                                0,
                                OnLoadTexture,
                                OnApplySkin,
                                0);

    // succeeded?
    if (!g_pCollada)
        return false;

    // add it to the scene
    csrSceneAddCollada(g_pScene, g_pCollada, 0, 0);
    csrSceneAddModelMatrix(g_pScene, g_pCollada, &g_ModelMatrix);

    // create the rotation matrix
    CSR_Matrix4 rotMat;
    CSR_Vector3 axis = {};
    axis.m_X         = 0.0f;
    axis.m_Y         = 0.0f;
    axis.m_Z         = 1.0f;
    csrMat4Rotate(0.0f, &axis, &rotMat);

    // create the scale matrix
    CSR_Matrix4 scaleMat;
    csrMat4Identity(&scaleMat);
    scaleMat.m_Table[0][0] = 0.4f;
    scaleMat.m_Table[1][1] = 0.4f;
    scaleMat.m_Table[2][2] = 0.4f;

    // place the model in the 3d world (update the matrix directly)
    csrMat4Multiply(&scaleMat, &rotMat, &g_ModelMatrix);
    g_ModelMatrix.m_Table[3][1] = -35.0f;

    g_Initialized = true;

    return true;
}
//------------------------------------------------------------------------------
void DeleteScene()
{
    g_Initialized = false;

    // release the scene
    csrSceneRelease(g_pScene, OnDeleteTexture);

    // release the shader
    csrOpenGLShaderRelease(g_pShader);
}
//------------------------------------------------------------------------------
void UpdateScene(float elapsedTime)
{
    // create a point of view from an arcball
    CSR_ArcBall arcball = {};
    arcball.m_AngleX    = 0.0f;
    arcball.m_AngleY    = g_Angle;
    arcball.m_Radius    = 100.0f;
    csrSceneArcBallToMatrix(&arcball, &g_pScene->m_ViewMatrix);

    // rotate the view around the model
    g_Angle = fmodf(g_Angle + elapsedTime, (float)(M_PI * 2));
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
    wcex.hCursor       =  ::LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground =  (HBRUSH)::GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName  =  nullptr;
    wcex.lpszClassName = L"CSR_JulietDemo";

    if (!RegisterClassEx(&wcex))
        return 0;

    // create the main window
    hWnd = ::CreateWindowEx(0,
                           L"CSR_JulietDemo",
                           L"Juliet - Textured and rigged Collada model",
                            WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            800,
                            650,
                            nullptr,
                            nullptr,
                            hInstance,
                            nullptr);

    ::ShowWindow(hWnd, nCmdShow);

    g_SceneDir = "..\\..\\..\\..\\Common";

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
    g_StartTime    = ::GetTickCount64();
    g_PreviousTime = ::GetTickCount64();

    // application main loop
    while (!bQuit)
    {
        // check for messages
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // handle or dispatch messages
            if (msg.message == WM_QUIT)
                bQuit = TRUE;
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
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
                g_FPS                       = (newFPS * smoothing) + (g_FPS * (1.0 - smoothing));
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
    DestroyWindow(hWnd);

    return (int)msg.wParam;
}
//------------------------------------------------------------------------------
