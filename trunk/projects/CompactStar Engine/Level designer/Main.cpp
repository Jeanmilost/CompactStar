#include <vcl.h>
#pragma hdrstop
#include "Main.h"

#include <string>

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Geometry.h"
#include "CSR_Renderer.h"
#include "CSR_Sound.h"

#pragma package(smart_init)
#ifdef __llvm__
    #pragma link "glewSL.a"
#else
    #pragma link "glewSL.lib"
#endif
#pragma link "OpenAL32E.lib"
#pragma resource "*.dfm"

//----------------------------------------------------------------------------
//FIXME
const char* miniGetVSColored()
{
    return "precision mediump float;"
           "attribute vec4 qr_vPosition;"
           "attribute vec4 qr_vColor;"
           "uniform   mat4 qr_uProjection;"
           "uniform   mat4 qr_uModelview;"
           "varying   vec4 qr_fColor;"
           "void main(void)"
           "{"
           "    qr_fColor    = qr_vColor;"
           "    gl_Position  = qr_uProjection * qr_uModelview * qr_vPosition;"
           "}";
}
//----------------------------------------------------------------------------
//FIXME
const char* miniGetFSColored()
{
    return "precision mediump float;"
           "varying lowp vec4 qr_fColor;"
           "void main(void)"
           "{"
           "    gl_FragColor = qr_fColor;"
           "}";
}
//---------------------------------------------------------------------------
TMainForm* MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* pOwner) :
    TForm(pOwner),
    m_hDC(NULL),
    m_hRC(NULL),
    m_pShader(NULL),
    m_pSphere(NULL),
    m_PreviousTime(0)
{
    // enable OpenGL
    EnableOpenGL(paView->Handle, &m_hDC, &m_hRC);

    // stop GLEW crashing on OSX :-/
    glewExperimental = GL_TRUE;

    // initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        // shutdown OpenGL
        DisableOpenGL(Handle, m_hDC, m_hRC);

        // close the app
        Application->Terminate();
    }
}
//---------------------------------------------------------------------------
__fastcall TMainForm::~TMainForm()
{
    DeleteScene();
    DisableOpenGL(paView->Handle, m_hDC, m_hRC);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormShow(TObject* pSender)
{
    // initialize the scene
    InitScene(ClientWidth, ClientHeight);

    // initialize the timer
    m_PreviousTime = ::GetTickCount();

    // listen the application idle
    Application->OnIdle = OnIdle;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormResize(TObject *Sender)
{
    // update the viewport
    CreateViewport(ClientWidth, ClientHeight);
}
//---------------------------------------------------------------------------
void TMainForm::EnableOpenGL(HWND hWnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    // get the device context
    *hDC = ::GetDC(hWnd);

    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize      = sizeof(pfd);
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 32;
    pfd.iLayerType = PFD_MAIN_PLANE;

    // set the pixel format for the device context
    iFormat = ChoosePixelFormat(*hDC, &pfd);
    SetPixelFormat(*hDC, iFormat, &pfd);

    // create and enable the OpenGL render context
    *hRC = wglCreateContext(*hDC);
    wglMakeCurrent(*hDC, *hRC);
}
//------------------------------------------------------------------------------
void TMainForm::DisableOpenGL(HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}
//------------------------------------------------------------------------------
void TMainForm::CreateViewport(float w, float h)
{
    if (!m_pShader)
        return;

    // calculate matrix items
    const float zNear  = 1.0f;
    const float zFar   = 1000.0f;
    const float fov    = 45.0f;
    const float aspect = w / h;

    // create the OpenGL viewport
    glViewport(0, 0, w, h);

    CSR_Matrix4 matrix;
    csrMat4Perspective(fov, aspect, zNear, zFar, &matrix);

    // connect projection matrix to shader
    GLint projectionUniform = glGetUniformLocation(m_pShader->m_ProgramID, "qr_uProjection");
    glUniformMatrix4fv(projectionUniform, 1, 0, &matrix.m_Table[0][0]);
}
//------------------------------------------------------------------------------
void TMainForm::InitScene(int w, int h)
{
    CSR_Buffer* pVS = csrBufferCreate();
    CSR_Buffer* pFS = csrBufferCreate();

    std::string data = miniGetVSColored();

    pVS->m_Length = data.length();
    pVS->m_pData  = new unsigned char[pVS->m_Length + 1];
    std::memcpy(pVS->m_pData, data.c_str(), pVS->m_Length);
    pVS->m_pData[pVS->m_Length] = 0x0;

    data = miniGetFSColored();

    pFS->m_Length = data.length();
    pFS->m_pData  = new unsigned char[pFS->m_Length + 1];
    std::memcpy(pFS->m_pData, data.c_str(), pFS->m_Length);
    pFS->m_pData[pFS->m_Length] = 0x0;

    m_pShader = csrShaderLoadFromBuffer(pVS, pFS);

    csrBufferRelease(pVS);
    csrBufferRelease(pFS);

    csrShaderEnable(m_pShader);

    // get shader attributes
    m_pShader->m_VertexSlot = glGetAttribLocation(m_pShader->m_ProgramID, "qr_vPosition");
    m_pShader->m_ColorSlot  = glGetAttribLocation(m_pShader->m_ProgramID, "qr_vColor");

    CSR_VertexFormat vf;
    vf.m_Type = CSR_VT_TriangleStrip;
    vf.m_UseNormals  = 0;
    vf.m_UseTextures = 0;
    vf.m_UseColors   = 1;

    m_pSphere   = csrShapeCreateSphere(&vf, 0.5f, 20, 20, 0xFFFF);
    m_pBox      = csrShapeCreateBox(&vf, 1.0f, 1.0f, 1.0f, 0xFF0000FF, 0);
    m_pAABBTree = csrAABBTreeFromMesh(m_pSphere);

    // configure OpenGL depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRangef(0.0f, 1.0f);

    // enable culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}
//------------------------------------------------------------------------------
void TMainForm::DeleteScene()
{
    csrAABBTreeRelease(m_pAABBTree);
    csrMeshRelease(m_pBox);
    csrMeshRelease(m_pSphere);
    csrShaderRelease(m_pShader);
}
//------------------------------------------------------------------------------
void TMainForm::UpdateScene(float elapsedTime)
{
}
//------------------------------------------------------------------------------
void TMainForm::DrawScene()
{
    float       xAngle;
    float       yAngle;
    CSR_Vector3 t;
    CSR_Vector3 r;
    CSR_Matrix4 translateMatrix;
    CSR_Matrix4 xRotateMatrix;
    CSR_Matrix4 yRotateMatrix;
    CSR_Matrix4 rotateMatrix;
    CSR_Matrix4 modelMatrix;

    csrSceneBegin(0.0f, 0.0f, 0.0f, 1.0f);

    // set translation
    t.m_X =  0.0f;
    t.m_Y =  0.0f;
    t.m_Z = -2.0f;

    csrMat4Translate(&t, &translateMatrix);

    // set rotation on X axis
    r.m_X = 1.0f;
    r.m_Y = 0.0f;
    r.m_Z = 0.0f;

    // rotate 90 degrees
    xAngle = 1.57075f;

    csrMat4Rotate(&xAngle, &r, &xRotateMatrix);

    // set rotation on Y axis
    r.m_X = 0.0f;
    r.m_Y = 1.0f;
    r.m_Z = 0.0f;

    yAngle = 0.0f;

    csrMat4Rotate(&yAngle, &r, &yRotateMatrix);

    // build model view matrix
    csrMat4Multiply(&xRotateMatrix, &yRotateMatrix,   &rotateMatrix);
    csrMat4Multiply(&rotateMatrix,  &translateMatrix, &modelMatrix);

    // connect model view matrix to shader
    GLint modelUniform = glGetUniformLocation(m_pShader->m_ProgramID, "qr_uModelview");
    glUniformMatrix4fv(modelUniform, 1, 0, &modelMatrix.m_Table[0][0]);

    csrSceneDrawMesh(m_pSphere, m_pShader);
    csrSceneDrawMesh(m_pBox, m_pShader);

    csrSceneEnd();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnIdle(TObject* pSender, bool& done)
{
    // calculate time interval
    const unsigned __int64 now            = ::GetTickCount();
    const double           elapsedTime    = (now - m_PreviousTime) / 1000.0;
                           m_PreviousTime =  now;

    // update and draw the scene
    UpdateScene(elapsedTime);
    DrawScene();

    ::SwapBuffers(m_hDC);

    done = false;
}
//---------------------------------------------------------------------------
