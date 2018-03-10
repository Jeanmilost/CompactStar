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
           "attribute vec4 csr_aVertices;"
           "attribute vec4 csr_aColor;"
           "uniform   mat4 csr_uProjection;"
           "uniform   mat4 csr_uView;"
           "uniform   mat4 csr_uModel;"
           "varying   vec4 csr_vColor;"
           "void main(void)"
           "{"
           "    csr_vColor   = csr_aColor;"
           "    gl_Position  = csr_uProjection * csr_uView * csr_uModel * csr_aVertices;"
           "}";
}
//----------------------------------------------------------------------------
//FIXME
const char* miniGetFSColored()
{
    return "precision mediump float;"
           "varying lowp vec4 csr_vColor;"
           "void main(void)"
           "{"
           "    gl_FragColor = csr_vColor;"
           "}";
}
//---------------------------------------------------------------------------
// TMainForm
//---------------------------------------------------------------------------
TMainForm* MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* pOwner) :
    TForm(pOwner),
    m_pCurrentShader(NULL),
    m_pScene(NULL),
    m_pSphere(NULL),
    m_PreviousTime(0),
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
    InitScene();

    // initialize the timer
    m_PreviousTime = ::GetTickCount();

    // listen the application idle
    Application->OnIdle = OnIdle;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormResize(TObject* pSender)
{
    if (!m_pScene)
        return;

    m_OpenGLHelper.ResizeViews(m_pCurrentShader);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnSplitterMoved(TObject* pSender)
{
    if (!m_pScene)
        return;

    m_OpenGLHelper.ResizeViews(m_pCurrentShader);
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
            if (!m_pScene)
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
            if (!m_pScene)
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
void TMainForm::InitScene()
{
    // create a scene
    m_pScene = csrSceneCreate();

    // configure the scene color
    csrRGBAToColor(0xFF, &m_pScene->m_Color);

    const std::string vsColored = miniGetVSColored();
    const std::string fsColored = miniGetFSColored();

    // iterate through views to initialize
    for (CSR_OpenGLHelper::IContextIterator it = m_OpenGLHelper.Begin(); it != m_OpenGLHelper.End(); ++it)
    {
        // enable view context
        wglMakeCurrent(it.Second().m_hDC, it.Second().m_hRC);

        CSR_Shader* pShader = NULL;

        try
        {
            CSR_Shader* pShader = csrShaderLoadFromStr(vsColored.c_str(),
                                                       vsColored.length(),
                                                       fsColored.c_str(),
                                                       fsColored.length(),
                                                       NULL,
                                                       NULL);

            csrShaderEnable(pShader);

            // get shader attributes
            pShader->m_VertexSlot = glGetAttribLocation(pShader->m_ProgramID, "csr_aVertices");
            pShader->m_ColorSlot  = glGetAttribLocation(pShader->m_ProgramID, "csr_aColor");

            m_OpenGLHelper.CreateViewport(it.First()->ClientWidth, it.First()->ClientHeight, pShader);

            m_Shaders.push_back(pShader);
            pShader = NULL;
        }
        __finally
        {
            csrShaderRelease(pShader);
        }
    }

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

    csrSceneAddMesh(m_pScene, m_pSphere, NULL, 0);
    //csrSceneAddMesh(m_pScene, m_pBox, NULL, 0);

    // configure OpenGL depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRangef(0.0f, 1.0f);
}
//------------------------------------------------------------------------------
void TMainForm::DeleteScene()
{
    // release the scene
    csrSceneRelease(m_pScene);
    m_pScene = NULL;

    // release the scene objects
    csrAABBTreeRelease(m_pAABBTree);
    csrMeshRelease(m_pBox);
    csrMeshRelease(m_pSphere);

    for (std::size_t i = 0; i < m_Shaders.size(); ++i)
        csrShaderRelease(m_Shaders[i]);
}
//------------------------------------------------------------------------------
void TMainForm::UpdateScene(float elapsedTime)
{
    CSR_Vector3 t;
    CSR_Vector3 r;
    CSR_Matrix4 translateMatrix;
    CSR_Matrix4 xRotateMatrix;
    CSR_Matrix4 yRotateMatrix;
    CSR_Matrix4 rotateMatrix;

    for (std::size_t i = 0; i < m_pScene->m_ItemCount; ++i)
    {
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
        const float xAngle = 1.57075f;

        csrMat4Rotate(xAngle, &r, &xRotateMatrix);

        // set rotation on Y axis
        r.m_X = 0.0f;
        r.m_Y = 1.0f;
        r.m_Z = 0.0f;

        const float yAngle = 0.0f;

        csrMat4Rotate(yAngle, &r, &yRotateMatrix);

        // build model view matrix
        csrMat4Multiply(&xRotateMatrix, &yRotateMatrix,   &rotateMatrix);
        csrMat4Multiply(&rotateMatrix,  &translateMatrix, &m_pScene->m_pItem[i].m_Matrix);

        m_pScene->m_pItem[i].m_pShader = m_pCurrentShader;
    }

    for (std::size_t i = 0; i < m_pScene->m_TransparentItemCount; ++i)
    {
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
        const float xAngle = 1.57075f;

        csrMat4Rotate(xAngle, &r, &xRotateMatrix);

        // set rotation on Y axis
        r.m_X = 0.0f;
        r.m_Y = 1.0f;
        r.m_Z = 0.0f;

        const float yAngle = 0.0f;

        csrMat4Rotate(yAngle, &r, &yRotateMatrix);

        // build model view matrix
        csrMat4Multiply(&xRotateMatrix, &yRotateMatrix,   &rotateMatrix);
        csrMat4Multiply(&rotateMatrix,  &translateMatrix, &m_pScene->m_pTransparentItem[i].m_Matrix);

        m_pScene->m_pTransparentItem[i].m_pShader = m_pCurrentShader;
    }
}
//------------------------------------------------------------------------------
void TMainForm::DrawScene()
{
    csrSceneDraw(m_pScene);
}
//---------------------------------------------------------------------------
void TMainForm::OnDrawScene(bool resize)
{
    if (!m_pScene)
        return;

    // calculate time interval
    const unsigned __int64 now            = ::GetTickCount();
    const double           elapsedTime    = (now - m_PreviousTime) / 1000.0;
                           m_PreviousTime =  now;

    std::size_t count = 0;

    // iterate through views to redraw
    for (CSR_OpenGLHelper::IContextIterator it = m_OpenGLHelper.Begin(); it != m_OpenGLHelper.End(); ++it)
    {
        m_pCurrentShader = m_Shaders[count];
        ++count;

        // is view control visible?
        if (!CSR_VCLHelper::IsVisible(it.First()))
            continue;

        // enable view context
        wglMakeCurrent(it.Second().m_hDC, it.Second().m_hRC);

        // update and draw the scene
        UpdateScene(resize ? 0.0f : elapsedTime);
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
