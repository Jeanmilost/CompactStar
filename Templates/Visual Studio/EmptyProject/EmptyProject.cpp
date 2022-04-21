/****************************************************************************
 * ==> Empty project -------------------------------------------------------*
 ****************************************************************************
 * Description : An empty and ready to use project                          *
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
#include "CSR_WaveFront.h"
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

//------------------------------------------------------------------------------
HDC                          g_hDC             = 0;
HGLRC                        g_hRC             = 0;
CSR_Scene*                   g_pScene          = nullptr;
CSR_SceneContext             g_SceneContext;
CSR_OpenGLShader*            g_pShader         = nullptr;
CSR_OpenGLHelper::IResources g_OpenGLResources;
std::size_t                  g_FrameCount;
double                       g_FPS             = 0.0;
unsigned __int64             g_StartTime       = 0L;
unsigned __int64             g_PreviousTime    = 0L;
bool                         g_Initialized     = false;

// -- DEMO CODE -------------------------
std::string                  g_SceneDir        = "..\\..\\..\\Common";
CSR_Matrix4                  g_ModelMatrix;
float                        g_Angle           = 0.0f;
// -- DEMO CODE END ---------------------

// TODO add your values here

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
void* OnGetShader(const void* pModel, CSR_EModelType type)
{
    return g_pShader;
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

    // configure the scene color background
    g_pScene->m_Color.m_R = 0.08f;
    g_pScene->m_Color.m_G = 0.12f;
    g_pScene->m_Color.m_B = 0.17f;

    // initialize the matrices
    csrMat4Identity(&g_pScene->m_ViewMatrix);

    // -- DEMO CODE -------------------------
    csrMat4Identity(&g_ModelMatrix);

    // get the shaders
    const std::string vsColored = CSR_ShaderHelper::GetVertexShader  (CSR_ShaderHelper::IEShaderType::IE_ST_Color);
    const std::string fsColored = CSR_ShaderHelper::GetFragmentShader(CSR_ShaderHelper::IEShaderType::IE_ST_Color);

    // load the shader
    g_pShader = csrOpenGLShaderLoadFromStr(vsColored.c_str(),
                                           vsColored.length(),
                                           fsColored.c_str(),
                                           fsColored.length(),
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
    g_pShader->m_VertexSlot = glGetAttribLocation (g_pShader->m_ProgramID, "csr_aVertices");
    g_pShader->m_ColorSlot  = glGetAttribLocation (g_pShader->m_ProgramID, "csr_aColor");

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 0;
    vf.m_HasPerVertexColor = 0;

    CSR_VertexCulling vc;
    vc.m_Type = CSR_CT_None;
    vc.m_Face = CSR_CF_CCW;

    // load the "Hello World!" model
    CSR_Model* pModel = csrWaveFrontOpen((g_SceneDir + "\\Models\\WaveFront\\Hello World.obj").c_str(),
                                         &vf,
                                         &vc,
                                         0,
                                         0,
                                         0,
                                         0);

    // succeeded?
    if (!pModel)
        return false;

    // add it to the scene
    csrSceneAddModel(g_pScene, pModel, 0, 0);
    csrSceneAddModelMatrix(g_pScene, pModel, &g_ModelMatrix);

    // place the model in the 3d world (update the matrix directly)
    g_ModelMatrix.m_Table[3][0] = -0.01f;
    g_ModelMatrix.m_Table[3][1] = -0.025f;
    // -- DEMO CODE END ---------------------

    // TODO add your initialization code here

    g_Initialized = true;

    return true;
}
//------------------------------------------------------------------------------
void DeleteScene()
{
    g_Initialized = false;

    // release the scene
    csrSceneRelease(g_pScene, nullptr);

    // release the shader
    csrOpenGLShaderRelease(g_pShader);

    // TODO add your cleanup code here
}
//------------------------------------------------------------------------------
void UpdateScene(float elapsedTime)
{
    // -- DEMO CODE -------------------------
    // create a point of view from an arcball
    CSR_ArcBall arcball;
    arcball.m_AngleX = 0.0f;
    arcball.m_AngleY = g_Angle;
    arcball.m_Radius = 0.35f;
    csrSceneArcBallToMatrix(&arcball, &g_pScene->m_ViewMatrix);

    // rotate the view around the model
    g_Angle = fmod((float)(g_Angle + elapsedTime), (float)(M_PI * 2.0f));
    // -- DEMO CODE END ---------------------

    // TODO add your scene animation code here
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
    wcex.cbSize        = sizeof(WNDCLASSEX);
    wcex.style         = CS_OWNDC;
    wcex.lpfnWndProc   = WindowProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = hInstance;
    wcex.hIcon         = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON));
    wcex.hIconSm       = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));
    wcex.hCursor       = ::LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName  = NULL;
    wcex.lpszClassName = L"CSR_EmptyProject";

    if (!RegisterClassEx(&wcex))
        return 0;

    // create the main window
    hWnd = ::CreateWindowEx(0,
                           L"CSR_EmptyProject",
                           L"Hello World!",
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
                                   g_PreviousTime = now;

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
    ::DestroyWindow(hWnd);

    return (int)msg.wParam;
}
//------------------------------------------------------------------------------
