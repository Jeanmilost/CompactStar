#include <vcl.h>
#pragma hdrstop
#include "Main.h"

// std
#include <memory>
#include <string>

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Geometry.h"
#include "CSR_Renderer.h"
#include "CSR_Sound.h"

// classes
#include "CSR_DesignerViewHelper.h"

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
// TMainForm
//---------------------------------------------------------------------------
TMainForm* MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* pOwner) :
    TForm(pOwner),
    m_pShader(NULL),
    m_pSphere(NULL),
    m_PreviousTime(0),
    m_Initialized(false),
    m_fViewWndProc_Backup(NULL)
{
    // create an OpenGL context for the 3d views
    m_OpenGLHelper.AddContext(paDesigner3DView);
    m_OpenGLHelper.AddContext(pa3DView);

    // stop GLEW crashing on OSX :-/
    glewExperimental = GL_TRUE;

    // initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        // release the OpenGL contexts on the views
        m_OpenGLHelper.ClearContexts();

        // close the app
        Application->Terminate();
    }
}
//---------------------------------------------------------------------------
__fastcall TMainForm::~TMainForm()
{
    DeleteScene();

    // release the OpenGL contexts on the views
    m_OpenGLHelper.ClearContexts();

    // clear the hooks
    m_pDesignerViewXHook.reset();
    m_pDesignerViewYHook.reset();
    m_pDesignerViewZHook.reset();
    m_pDesigner3DViewHook.reset();
    m_p3DViewHook.reset();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormShow(TObject* pSender)
{
    // hook the designer views
    m_pDesignerViewXHook.reset (new CSR_VCLControlHook(paDesignerXView,  OnDesignerViewMessage));
    m_pDesignerViewYHook.reset (new CSR_VCLControlHook(paDesignerYView,  OnDesignerViewMessage));
    m_pDesignerViewZHook.reset (new CSR_VCLControlHook(paDesignerZView,  OnDesignerViewMessage));
    m_pDesigner3DViewHook.reset(new CSR_VCLControlHook(paDesigner3DView, On3DViewMessage));

    // hook the 3d view
    m_p3DViewHook.reset(new CSR_VCLControlHook(pa3DView, On3DViewMessage));

    // initialize the scene
    InitScene(ClientWidth, ClientHeight);

    // initialize the timer
    m_PreviousTime = ::GetTickCount();

    // listen the application idle
    Application->OnIdle = OnIdle;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormResize(TObject* pSender)
{
    if (!m_Initialized)
        return;

    m_OpenGLHelper.ResizeViews(m_pShader);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnSplitterMoved(TObject* pSender)
{
    if (!m_Initialized)
        return;

    m_OpenGLHelper.ResizeViews(m_pShader);
}
//---------------------------------------------------------------------------
bool TMainForm::OnDesignerViewMessage(TControl* pControl, TMessage& message, TWndMethod fCtrlOriginalProc)
{
    switch (message.Msg)
    {
        case WM_ERASEBKGND:
            // do nothing, the background will be fully repainted later
            message.Result = 1L;
            return true;

        case WM_PAINT:
        {
            TPanel* pPanel = dynamic_cast<TPanel*>(pControl);

            if (!pPanel)
                return false;

            ::PAINTSTRUCT ps;
            HDC           hDC;

            try
            {
                // begin paint
                hDC = ::BeginPaint(pPanel->Handle, &ps);

                // succeeded?
                if (!hDC)
                    return false;

                // draw the background grid
                CSR_DesignerViewHelper::DrawGrid(pControl->ClientRect,
                                                 CSR_DesignerViewHelper::IGridOptions(),
                                                 hDC);
            }
            __finally
            {
                // end paint
                if (hDC)
                    ::EndPaint(pPanel->Handle, &ps);
            }

            // validate entire client rect (it has just been completely redrawn)
            ::ValidateRect(pPanel->Handle, NULL);

            // notify main Windows procedure that the view was repainted
            message.Result = 0L;
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool TMainForm::On3DViewMessage(TControl* pControl, TMessage& message, TWndMethod fCtrlOriginalProc)
{
    switch (message.Msg)
    {
        case WM_ERASEBKGND:
            // do nothing, the background will be fully repainted later
            message.Result = 1L;
            return true;

        case WM_WINDOWPOSCHANGED:
        {
            if (!m_Initialized)
                break;

            TPanel* pPanel = dynamic_cast<TPanel*>(pControl);

            if (!pPanel)
                return false;

            if (fCtrlOriginalProc)
                fCtrlOriginalProc(message);

            // redraw here, thus the view will be redrawn to the correct size in real time while the
            // size changes
            OnDrawScene(true);

            return true;
        }

        case WM_PAINT:
        {
            // is scene initialized?
            if (!m_Initialized)
                return false;

            TPanel* pPanel = dynamic_cast<TPanel*>(pControl);

            if (!pPanel)
                return false;

            // draw the scene
            OnDrawScene(true);

            // validate entire client rect (it has just been completely redrawn)
            ::ValidateRect(pPanel->Handle, NULL);

            // notify main Windows procedure that the view was repainted
            message.Result = 0L;
            return true;
        }
    }

    return false;
}
//------------------------------------------------------------------------------
void TMainForm::InitScene(int w, int h)
{
    // configure the scene color
    m_Scene.m_Color.m_R = 0.0f;
    m_Scene.m_Color.m_G = 0.0f;
    m_Scene.m_Color.m_B = 0.0f;
    m_Scene.m_Color.m_A = 1.0f;

    // iterate through views to initialize
    for (CSR_OpenGLHelper::IContextIterator it = m_OpenGLHelper.Begin(); it != m_OpenGLHelper.End(); ++it)
    {
        // enable view context
        wglMakeCurrent(it.Second().m_hDC, it.Second().m_hRC);

        std::string vsColored = miniGetVSColored();
        std::string fsColored = miniGetFSColored();

        m_pShader = csrShaderLoadFromStr(vsColored.c_str(),
                                         vsColored.length(),
                                         fsColored.c_str(),
                                         fsColored.length(),
                                         NULL,
                                         NULL);

        csrShaderEnable(m_pShader);

        // get shader attributes
        m_pShader->m_VertexSlot = glGetAttribLocation(m_pShader->m_ProgramID, "qr_vPosition");
        m_pShader->m_ColorSlot  = glGetAttribLocation(m_pShader->m_ProgramID, "qr_vColor");

        m_OpenGLHelper.CreateViewport(w, h, m_pShader);

        CSR_VertexFormat vf;
        vf.m_HasNormal         = 0;
        vf.m_HasTexCoords      = 0;
        vf.m_HasPerVertexColor = 1;

        CSR_Material sm;
        sm.m_Color       = 0xFFFF;
        sm.m_Transparent = 0;
        sm.m_Wireframe   = 0;

        CSR_Material bm;
        bm.m_Color       = 0xFF0000FF;
        bm.m_Transparent = 0;
        bm.m_Wireframe   = 0;

        m_pSphere   = csrShapeCreateSphere(0.5f, 20, 20, &vf, NULL, &sm, NULL);
        m_pBox      = csrShapeCreateBox(1.0f, 1.0f, 1.0f, 0, &vf, NULL, &bm, NULL);
        m_pAABBTree = csrAABBTreeFromMesh(m_pSphere);

        // configure OpenGL depth testing
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LEQUAL);
        glDepthRangef(0.0f, 1.0f);
    }

    m_Initialized = true;
}
//------------------------------------------------------------------------------
void TMainForm::DeleteScene()
{
    m_Initialized = false;

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

    try
    {
        csrSceneBegin(&m_Scene.m_Color);

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

        csrMat4Rotate(xAngle, &r, &xRotateMatrix);

        // set rotation on Y axis
        r.m_X = 0.0f;
        r.m_Y = 1.0f;
        r.m_Z = 0.0f;

        yAngle = 0.0f;

        csrMat4Rotate(yAngle, &r, &yRotateMatrix);

        // build model view matrix
        csrMat4Multiply(&xRotateMatrix, &yRotateMatrix,   &rotateMatrix);
        csrMat4Multiply(&rotateMatrix,  &translateMatrix, &modelMatrix);

        // connect model view matrix to shader
        GLint modelUniform = glGetUniformLocation(m_pShader->m_ProgramID, "qr_uModelview");
        glUniformMatrix4fv(modelUniform, 1, 0, &modelMatrix.m_Table[0][0]);

        csrSceneDrawMesh(m_pSphere, m_pShader);
        //csrSceneDrawMesh(m_pBox, m_pShader);
    }
    __finally
    {
        csrSceneEnd();
    }
}
//---------------------------------------------------------------------------
void TMainForm::OnDrawScene(bool resize)
{
    // iterate through views to redraw
    for (CSR_OpenGLHelper::IContextIterator it = m_OpenGLHelper.Begin(); it != m_OpenGLHelper.End(); ++it)
    {
        // is view control visible?
        if (!CSR_VCLHelper::IsVisible(it.First()))
            continue;

        // enable view context
        wglMakeCurrent(it.Second().m_hDC, it.Second().m_hRC);

        // do draw the scene for a resize?
        if (resize)
        {
            if (!m_Initialized)
                continue;

            // just process a minimal draw
            UpdateScene(0.0);
            DrawScene();

            ::SwapBuffers(it.Second().m_hDC);
            continue;
        }

        // calculate time interval
        const unsigned __int64 now            = ::GetTickCount();
        const double           elapsedTime    = (now - m_PreviousTime) / 1000.0;
                               m_PreviousTime =  now;

        if (!m_Initialized)
            continue;

        // update and draw the scene
        UpdateScene(elapsedTime);
        DrawScene();

        ::SwapBuffers(it.Second().m_hDC);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnIdle(TObject* pSender, bool& done)
{
    done = false;

    OnDrawScene(false);
}
//---------------------------------------------------------------------------
