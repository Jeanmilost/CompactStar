/*****************************************************************************
 * ==> Capsules demo --------------------------------------------------------*
 *****************************************************************************
 * Description : Capsules and collision demo                                 *
 * Developer   : Jean-Milost Reymond                                         *
 * Copyright   : 2015 - 2021, this file is part of the Minimal API. You are  *
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
#include "CSR_Scene.h"
#include "CSR_AI.h"
#include "CSR_Renderer_OpenGL.h"
#include "CSR_Sound.h"

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
const char g_VSColored[] = "precision mediump float;"
                           "attribute vec3  csr_aVertices;"
                           "attribute vec4  csr_aColor;"
                           "uniform   float csr_uAlpha;"
                           "uniform   float csr_uCollision;"
                           "uniform   mat4  csr_uProjection;"
                           "uniform   mat4  csr_uView;"
                           "uniform   mat4  csr_uModel;"
                           "varying   vec4  csr_vColor;"
                           "varying   float csr_fAlpha;"
                           "varying   float csr_fCollision;"
                           "void main(void)"
                           "{"
                           "    csr_vColor     = csr_aColor;"
                           "    csr_fAlpha     = csr_uAlpha;"
                           "    csr_fCollision = csr_uCollision;"
                           "    gl_Position    = csr_uProjection * csr_uView * csr_uModel * vec4(csr_aVertices, 1.0);"
                           "}";
//----------------------------------------------------------------------------
const char g_FSColored[] = "precision mediump float;"
                           "varying lowp vec4  csr_vColor;"
                           "varying      float csr_fAlpha;"
                           "varying      float csr_fCollision;"
                           "void main(void)"
                           "{"
                           "    if (csr_fCollision > 0.5)"
                           "        gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);"
                           "    else"
                           "        gl_FragColor = vec4(csr_vColor.x, csr_vColor.y, csr_vColor.z, csr_fAlpha);"
                           "}";
//------------------------------------------------------------------------------
HDC                          g_hDC             = 0;
HGLRC                        g_hRC             = 0;
CSR_OpenGLHelper::IResources g_OpenGLResources;
ALCdevice*                   g_pOpenALDevice   = nullptr;
ALCcontext*                  g_pOpenALContext  = nullptr;
CSR_Scene*                   g_pScene          = nullptr;
CSR_OpenGLShader*            g_pShader         = nullptr;
CSR_Mesh*                    g_pCapsule1Mesh   = nullptr;
CSR_Mesh*                    g_pCapsule2Mesh   = nullptr;
CSR_SceneContext             g_SceneContext;
CSR_Capsule                  g_Capsule1;
CSR_Capsule                  g_Capsule2;
CSR_Matrix4                  g_Capsule1Matrix;
CSR_Matrix4                  g_Capsule2Matrix;
GLuint                       g_AlphaSlot       = 0;
GLuint                       g_CollisionSlot   = 0;
size_t                       g_FrameCount      = 0;
int                          g_Collision       = 0;
float                        g_DeltaX          = 0.5f;
float                        g_Angle           = 0.0f;
double                       g_FPS             = 0.0;
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

        case WM_KEYDOWN:
            switch (wParam)
            {
                case VK_ESCAPE:
                    ::PostQuitMessage(0);
                    break;

                case VK_LEFT:
                    g_DeltaX -= 0.005f;
                    break;

                case VK_RIGHT:
                    g_DeltaX += 0.005f;
                    break;

                case VK_UP:
                    g_Angle += 0.005f;
                    break;

                case VK_DOWN:
                    g_Angle -= 0.005f;
                    break;
            }

            break;

        case WM_SIZE:
        {
            if (!g_Initialized)
                break;

            const int width  = ((int)(short)LOWORD(lParam));
            const int height = ((int)(short)HIWORD(lParam));

            // update the viewport
            CSR_OpenGLHelper::CreateViewport((float)width,
                                             (float)height,
                                             0.01f,
                                             100.0f,
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
    csrShaderEnable(g_pShader);

    // bot or fader?
    if (g_Collision)
        glUniform1f(g_CollisionSlot, 1.0f);
    else
        glUniform1f(g_CollisionSlot, 0.0f);

    glUniform1f(g_AlphaSlot, 1.0f);

    return g_pShader;
}
//---------------------------------------------------------------------------
void OnApplySkin(std::size_t index, const CSR_Skin* pSkin, int* pCanRelease)
{
    // load the texture from the received pixel buffer
    const GLuint textureID = csrOpenGLTextureFromPixelBuffer(pSkin->m_Texture.m_pBuffer);

    // succeeded?
    if (textureID == M_CSR_Error_Code)
        return;

    // add the texture to the OpenGL resources
    CSR_OpenGLHelper::AddTexture(&pSkin->m_Texture, textureID, g_OpenGLResources);

    // from now the texture resource on the model side may be released (will no longer be used)
    if (pCanRelease)
        *pCanRelease = 1;
}
//------------------------------------------------------------------------------
bool InitScene(int w, int h)
{
    CSR_VertexFormat  vertexFormat  = {};
    CSR_VertexCulling vertexCulling = {};
    CSR_Material      material      = {};
    CSR_SceneItem*    pSceneItem    = nullptr;

    // initialize the scene
    g_pScene = csrSceneCreate();

    // configure the scene background color
    g_pScene->m_Color.m_R = 0.25f;
    g_pScene->m_Color.m_G = 0.4f;
    g_pScene->m_Color.m_B = 0.5f;
    g_pScene->m_Color.m_A = 1.0f;

    // configure the scene view matrix
    csrMat4Identity(&g_pScene->m_ViewMatrix);

    // configure the scene context
    csrSceneContextInit(&g_SceneContext);
    g_SceneContext.m_fOnGetShader = OnGetShader;

    // compile, link and use shader
    g_pShader = csrOpenGLShaderLoadFromStr(&g_VSColored[0],
                                            sizeof(g_VSColored),
                                           &g_FSColored[0],
                                            sizeof(g_FSColored),
                                            0,
                                            0);

    // succeeded?
    if (!g_pShader)
        return false;

    csrShaderEnable(g_pShader);

    // create the viewport
    CSR_OpenGLHelper::CreateViewport((float)w,
                                     (float)h,
                                     0.1f,
                                     100.0f,
                                     g_pShader,
                                     g_pScene->m_ProjectionMatrix);

    // get shader attributes
    g_pShader->m_VertexSlot = glGetAttribLocation (g_pShader->m_ProgramID, "csr_aVertices");
    g_pShader->m_ColorSlot  = glGetAttribLocation (g_pShader->m_ProgramID, "csr_aColor");
    g_AlphaSlot             = glGetUniformLocation(g_pShader->m_ProgramID, "csr_uAlpha");
    g_CollisionSlot         = glGetUniformLocation(g_pShader->m_ProgramID, "csr_uCollision");

    // configure OpenGL depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRangef(0.0f, 1.0f);

    // initialize the first capsule
    g_Capsule1.m_Top.m_X    =  0.0f;
    g_Capsule1.m_Top.m_Y    =  1.0f;
    g_Capsule1.m_Top.m_Z    = -2.0f;
    g_Capsule1.m_Bottom.m_X =  0.0f;
    g_Capsule1.m_Bottom.m_Y =  0.0f;
    g_Capsule1.m_Bottom.m_Z = -2.0f;
    g_Capsule1.m_Radius     =  0.2f;

    // configure the vertex format
    vertexFormat.m_HasNormal         = 0;
    vertexFormat.m_HasTexCoords      = 0;
    vertexFormat.m_HasPerVertexColor = 1;

    vertexCulling.m_Face = CSR_CF_CCW;
    vertexCulling.m_Type = CSR_CT_Back;

    // configure the material
    material.m_Color       = 0xFF0000FF;
    material.m_Transparent = 1;
    material.m_Wireframe   = 0;

    // initialize first capsule matrix
    csrMat4Identity(&g_Capsule1Matrix);
    g_Capsule1Matrix.m_Table[3][0] = g_Capsule1.m_Bottom.m_X + g_DeltaX;
    g_Capsule1Matrix.m_Table[3][1] = g_Capsule1.m_Bottom.m_Y;
    g_Capsule1Matrix.m_Table[3][2] = g_Capsule1.m_Bottom.m_Z;

    // create a capsule
    g_pCapsule1Mesh = csrShapeCreateCapsule(1.0f, 0.2f, 10.0f, &vertexFormat, 0, &material, 0);
    pSceneItem      = csrSceneAddMesh(g_pScene, g_pCapsule1Mesh, 1, 0);
    csrSceneAddModelMatrix(g_pScene, g_pCapsule1Mesh, &g_Capsule1Matrix);

    // initialize the second capsule
    g_Capsule2.m_Top.m_X    =  0.0f;
    g_Capsule2.m_Top.m_Y    =  1.0f;
    g_Capsule2.m_Top.m_Z    = -2.0f;
    g_Capsule2.m_Bottom.m_X =  0.0f;
    g_Capsule2.m_Bottom.m_Y =  0.0f;
    g_Capsule2.m_Bottom.m_Z = -2.0f;
    g_Capsule2.m_Radius     =  0.2f;

    // configure the material
    material.m_Color = 0x0000FFFF;

    // initialize second capsule matrix
    csrMat4Identity(&g_Capsule2Matrix);
    g_Capsule2Matrix.m_Table[3][0] = g_Capsule2.m_Bottom.m_X - g_DeltaX;
    g_Capsule2Matrix.m_Table[3][1] = g_Capsule2.m_Bottom.m_Y;
    g_Capsule2Matrix.m_Table[3][2] = g_Capsule2.m_Bottom.m_Z;

    // create a capsule
    g_pCapsule2Mesh = csrShapeCreateCapsule(1.0f, 0.2f, 10.0f, &vertexFormat, 0, &material, 0);
    pSceneItem      = csrSceneAddMesh(g_pScene, g_pCapsule2Mesh, 1, 0);
    csrSceneAddModelMatrix(g_pScene, g_pCapsule2Mesh, &g_Capsule2Matrix);

    g_Initialized = true;

    return true;
}
//------------------------------------------------------------------------------
void DeleteScene()
{
    g_Initialized = false;

    // delete scene shader
    csrOpenGLShaderRelease(g_pShader);
}
//------------------------------------------------------------------------------
void UpdateScene(float elapsedTime)
{
    // create the X rotation matrix
    CSR_Matrix4 rotMatX;
    CSR_Vector3 axis = {};
    axis.m_X = 1.0f;
    axis.m_Y = 0.0f;
    axis.m_Z = 0.0f;
    csrMat4Rotate(0.0f, &axis, &rotMatX);

    // create the Y rotation matrix
    CSR_Matrix4 rotMatY;
    axis.m_X = 0.0f;
    axis.m_Y = 1.0f;
    axis.m_Z = 0.0f;
    csrMat4Rotate(0.0f, &axis, &rotMatY);

    // create the Y rotation matrix
    CSR_Matrix4 rotMatZ;
    axis.m_X = 0.0f;
    axis.m_Y = 0.0f;
    axis.m_Z = 1.0f;
    csrMat4Rotate(g_Angle, &axis, &rotMatZ);

    CSR_Matrix4 buildMat;
    CSR_Matrix4 matrix;

    // combine the rotation matrices
    csrMat4Multiply(&rotMatX,  &rotMatY, &buildMat);
    csrMat4Multiply(&buildMat, &rotMatZ, &matrix);

    // update first capsule matrix
    g_Capsule1Matrix               =  matrix;
    g_Capsule1Matrix.m_Table[3][0] =  g_Capsule1.m_Bottom.m_X + g_DeltaX;
    g_Capsule1Matrix.m_Table[3][1] = -0.5f;
    g_Capsule1Matrix.m_Table[3][2] =  g_Capsule1.m_Bottom.m_Z;

    // combine the rotation matrices
    csrMat4Rotate  (-g_Angle,   &axis,    &rotMatZ);
    csrMat4Multiply( &buildMat, &rotMatZ, &matrix);

    // update second capsule matrix
    g_Capsule2Matrix               =  matrix;
    g_Capsule2Matrix.m_Table[3][0] =  g_Capsule2.m_Bottom.m_X - g_DeltaX;
    g_Capsule2Matrix.m_Table[3][1] = -0.5f;
    g_Capsule2Matrix.m_Table[3][2] =  g_Capsule2.m_Bottom.m_Z;

    CSR_Capsule capsule1 = {};
    CSR_Capsule capsule2 = {};

    CSR_Vector3 cap1Top = {};
    cap1Top.m_X = 0.0f;
    cap1Top.m_Y = g_Capsule1.m_Top.m_Y;
    cap1Top.m_Z = 0.0f;

    CSR_Vector3 cap1Bottom = {};
    cap1Bottom.m_X = 0.0f;
    cap1Bottom.m_Y = g_Capsule1.m_Bottom.m_Y;
    cap1Bottom.m_Z = 0.0f;

    CSR_Vector3 cap2Top = {};
    cap2Top.m_X = 0.0f;
    cap2Top.m_Y = g_Capsule2.m_Top.m_Y;
    cap2Top.m_Z = 0.0f;

    CSR_Vector3 cap2Bottom = {};
    cap2Bottom.m_X = 0.0f;
    cap2Bottom.m_Y = g_Capsule2.m_Bottom.m_Y;
    cap2Bottom.m_Z = 0.0f;

    csrMat4Transform(&g_Capsule1Matrix, &cap1Top,    &capsule1.m_Top);
    csrMat4Transform(&g_Capsule1Matrix, &cap1Bottom, &capsule1.m_Bottom);

    csrMat4Transform(&g_Capsule2Matrix, &cap2Top,    &capsule2.m_Top);
    csrMat4Transform(&g_Capsule2Matrix, &cap2Bottom, &capsule2.m_Bottom);

    capsule1.m_Radius = g_Capsule1.m_Radius;
    capsule2.m_Radius = g_Capsule2.m_Radius;

    CSR_Figure3 figure1 = {};
    figure1.m_pFigure   = &capsule1;
    figure1.m_Type      = CSR_F3_Capsule;

    CSR_Figure3 figure2 = {};
    figure2.m_pFigure   = &capsule2;
    figure2.m_Type      = CSR_F3_Capsule;

    // test collision between capsules
    g_Collision = csrIntersect3(&figure1, &figure2, 0, 0, 0);
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
    wcex.hCursor       =  ::LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground =  (HBRUSH)::GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName  =  NULL;
    wcex.lpszClassName = L"CSR_Capsules";

    if (!RegisterClassEx(&wcex))
        return 0;

    // create the main window
    hWnd = ::CreateWindowEx( 0,
                            L"CSR_Capsules",
                            L"Capsules",
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

    // release OpenAL interface
    csrSoundReleaseOpenAL(g_pOpenALDevice, g_pOpenALContext);

    // destroy the window explicitly
    DestroyWindow(hWnd);

    return (int)msg.wParam;
}
//------------------------------------------------------------------------------
