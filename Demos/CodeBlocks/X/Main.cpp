/****************************************************************************
 * ==> DirectX model demo --------------------------------------------------*
 ****************************************************************************
 * Description : DirectX (.x) model with bone animation demo                *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2021, this file is part of the CompactStar Engine.  *
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
#include "CSR_X.h"
#include "CSR_Renderer_OpenGL.h"
#include "CSR_Scene.h"

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

// resource files to load
#define LANDSCAPE_TEXTURE_FILE     "\\grass.bmp"
#define LANDSCAPE_DATA_FILE        "\\bot_level.bmp"
#define FADER_TEXTURE_FILE         "\\blank.bmp"
#define MDL_FILE                   "\\wizard.mdl"
#define SKYBOX_LEFT                "\\left.bmp"
#define SKYBOX_TOP                 "\\top.bmp"
#define SKYBOX_RIGHT               "\\right.bmp"
#define SKYBOX_BOTTOM              "\\bottom.bmp"
#define SKYBOX_FRONT               "\\front.bmp"
#define SKYBOX_BACK                "\\back.bmp"
#define FOOT_STEP_LEFT_SOUND_FILE  "\\footstep_left.wav"
#define FOOT_STEP_RIGHT_SOUND_FILE "\\footstep_right.wav"
#define HIT_SOUND_FILE             "\\hit.wav"

//----------------------------------------------------------------------------
typedef std::vector<std::string> IFileNames;
//------------------------------------------------------------------------------
HDC                          g_hDC             = 0;
HGLRC                        g_hRC             = 0;
CSR_Scene*                   g_pScene          = NULL;
CSR_SceneContext             g_SceneContext;
CSR_OpenGLShader*            g_pShader         = NULL;
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

    // NOTE don't respect the global g_SceneDir folder, because the contained
    // texture format isn't compatible with the below LoadTexture() function
    const std::string resPath = "Resources\\";

    CSR_OpenGLHelper::AddTexture(&pSkin->m_Texture,
                                 LoadTexture((resPath + pSkin->m_Texture.m_pFileName).c_str()),
                                 g_OpenGLResources);
}
//---------------------------------------------------------------------------
void* OnGetShader(const void* pModel, CSR_EModelType type)
{
    return g_pShader;
}
//---------------------------------------------------------------------------
void* OnGetID(const void* pKey)
{
    return CSR_OpenGLHelper::GetTextureID(pKey, g_OpenGLResources);
}
//---------------------------------------------------------------------------
void OnGetXIndex(const CSR_X* pX, size_t* pAnimSetIndex, size_t* pFrameIndex)
{
    // get the animation set (1 = walking) and the frame (limited to 4800 conform to the model specs)
    *pAnimSetIndex = 1;
    *pFrameIndex   = (g_PreviousTime * 5) % 4800;
}
//---------------------------------------------------------------------------
void OnSceneBegin(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    csrDrawBegin(&pScene->m_Color);
}
//---------------------------------------------------------------------------
void OnSceneEnd(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    csrDrawEnd();
}
//---------------------------------------------------------------------------
void OnDeleteTexture(const CSR_Texture* pTexture)
{
    return CSR_OpenGLHelper::DeleteTexture(pTexture, g_OpenGLResources);
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
    g_SceneContext.m_fOnSceneBegin = OnSceneBegin;
    g_SceneContext.m_fOnSceneEnd   = OnSceneEnd;
    g_SceneContext.m_fOnGetShader  = OnGetShader;
    g_SceneContext.m_fOnGetXIndex  = OnGetXIndex;
    g_SceneContext.m_fOnGetID      = OnGetID;

    // configure the scene color background
    g_pScene->m_Color.m_R = 0.08f;
    g_pScene->m_Color.m_G = 0.12f;
    g_pScene->m_Color.m_B = 0.17f;

    // initialize the matrices
    csrMat4Identity(&g_pScene->m_ViewMatrix);
    csrMat4Identity(&g_ModelMatrix);

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

    CSR_VertexFormat vf    = {};
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    CSR_VertexCulling vc = {};
    vc.m_Type            = CSR_CT_Back;
    vc.m_Face            = CSR_CF_CCW;

    // load the .x model
    CSR_X* pX = csrXOpen((g_SceneDir + "\\Models\\X\\tiny_4anim.x").c_str(),
                        &vf,
                        &vc,
                         0,
                         0,
                         0,
                         0,
                         OnLoadTexture,
                         OnApplySkin,
                         0);

    // succeeded?
    if (!pX)
        return false;

    // add it to the scene
    csrSceneAddX(g_pScene, pX, 0, 0);
    csrSceneAddModelMatrix(g_pScene, pX, &g_ModelMatrix);

    // create the rotation matrix
    CSR_Matrix4 rotMat;
    CSR_Vector3 axis = {};
    axis.m_X         = 0.0f;
    axis.m_Y         = 0.0f;
    axis.m_Z         = 1.0f;
    csrMat4Rotate(M_PI, &axis, &rotMat);

    // create the scale matrix
    CSR_Matrix4 scaleMat;
    csrMat4Identity(&scaleMat);
    scaleMat.m_Table[0][0] = 0.075f;
    scaleMat.m_Table[1][1] = 0.075f;
    scaleMat.m_Table[2][2] = 0.075f;

    // place the model in the 3d world (update the matrix directly)
    csrMat4Multiply(&scaleMat, &rotMat, &g_ModelMatrix);
    g_ModelMatrix.m_Table[3][1] = -25.0f;

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
    wcex.lpszClassName = "CSR_XDemo";

    if (!RegisterClassEx(&wcex))
        return 0;

    // create the main window
    hWnd = ::CreateWindowEx(0,
                            "CSR_XDemo",
                            "Animated DirectX model",
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

    g_SceneDir = "..\\..\\..\\Common";

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
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
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
