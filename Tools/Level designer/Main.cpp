#include <vcl.h>
#pragma hdrstop
#include "Main.h"

// std
#include <memory>
#include <string>

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Geometry.h"
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
    m_pScene(NULL),
    m_pCurrentShader(NULL),
    m_pLoadingModel(NULL),
    m_pLoadingTexture(NULL),
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
CSR_Shader* TMainForm::OnGetShaderCallback(const void* pModel, CSR_EModelType type)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return 0;

    return pMainForm->OnGetShader(pModel, type);
}
//------------------------------------------------------------------------------
void TMainForm::OnTextureReadCallback(      std::size_t      index,
                                      const CSR_PixelBuffer* pPixelBuffer,
                                            int*             pNoGPU)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return;

    pMainForm->OnTextureRead(index, pPixelBuffer, pNoGPU);
}
//------------------------------------------------------------------------------
int TMainForm::OnGetTextureIndexCallback(const void* pModel, size_t index, int bumpMap)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return 0;

    return pMainForm->OnGetTextureIndex(pModel, index, bumpMap);
}
//------------------------------------------------------------------------------
int TMainForm::OnGetShaderIndexCallback(const void* pModel)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return 0;

    return pMainForm->OnGetShaderIndex(pModel);
}
//------------------------------------------------------------------------------
void TMainForm::InitScene()
{
    // create a scene
    m_pScene = csrSceneCreate();

    // initialize the scene context
    m_SceneContext.m_Handle             = 0;
    m_SceneContext.m_fOnSceneBegin      = 0;
    m_SceneContext.m_fOnSceneEnd        = 0;
    m_SceneContext.m_fOnGetShader       = OnGetShaderCallback;
    m_SceneContext.m_fOnGetModelIndex   = 0;
    m_SceneContext.m_fOnGetMDLIndex     = 0;
    m_SceneContext.m_fOnDetectCollision = 0;

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

    CSR_Mesh* pSphere = csrShapeCreateSphere(0.5f, 20, 20, &vf, NULL, &sm, NULL);
    CSR_Mesh* pBox    = csrShapeCreateBox(1.0f, 1.0f, 1.0f, 0, &vf, NULL, &bm, NULL);

    csrSceneAddMesh(m_pScene, pSphere, 0, 1);
    csrSceneAddMesh(m_pScene, pBox, 0, 1);

    CSR_WriteContext writeContext;
    writeContext.m_fOnGetTextureIndex = OnGetTextureIndexCallback;
    writeContext.m_fOnGetShaderIndex  = OnGetShaderIndexCallback;

    CSR_Buffer* pBuffer = csrBufferCreate();
    csrSerializerWriteScene(&writeContext, m_pScene, pBuffer);
    csrFileSave("Scene1.bin", pBuffer);
    csrBufferRelease(pBuffer);

    csrSceneDeleteFrom(m_pScene, pSphere);

    CSR_MDL* pMDL = csrMDLOpen("N:\\Jeanmilost\\Devel\\Projects\\CompactStar Engine\\Common\\Models\\player.mdl",
                               0,
                               0,
                               0,
                               0,
                               0,
                               OnTextureReadCallback);

    m_pLoadingModel = pMDL;

    try
    {
        CSR_Buffer* pBuffer = csrBufferCreate();
        csrSerializerWriteMDL(&writeContext, pMDL, pBuffer);
        csrFileSave("Test.bin", pBuffer);
        csrBufferRelease(pBuffer);
    }
    __finally
    {
        m_pLoadingModel = NULL;

        if (m_pLoadingTexture)
        {
            csrBufferRelease(m_pLoadingTexture);
            m_pLoadingTexture = NULL;
        }
    }

    csrMDLRelease(pMDL);

    CSR_ReadContext readContext;
    CSR_Buffer*     pSceneBuffer = csrFileOpen("Scene1.bin");
    size_t          offset       = 0;
    CSR_Scene*      pScene       = csrSceneCreate();

    csrSceneInit(pScene);

    CSR_SceneFileHeader* pHeader = (CSR_SceneFileHeader*)malloc(sizeof(CSR_SceneFileHeader));
    csrSerializerReadHeader(&readContext, pSceneBuffer, &offset, pHeader);

    csrSerializerReadScene(&readContext,
                            pSceneBuffer,
                           &offset,
                            pHeader->m_ChunkSize - pHeader->m_HeaderSize,
                            pScene);

    free(pHeader);
    csrBufferRelease(pSceneBuffer);

    pBuffer = csrBufferCreate();
    csrSerializerWriteScene(&writeContext, pScene, pBuffer);
    csrFileSave("Scene2.bin", pBuffer);
    csrBufferRelease(pBuffer);

    csrSceneRelease(pScene);

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

    // release the shader
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
        // link the model matrix to the scene, if still not done
        if (!m_pScene->m_pItem[i].m_pMatrixList)
            csrSceneAddModelMatrix(m_pScene, m_pScene->m_pItem[i].m_pModel, &m_ModelMatrix);

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
        csrMat4Multiply(&rotateMatrix,  &translateMatrix, &m_ModelMatrix);
    }

    for (std::size_t i = 0; i < m_pScene->m_TransparentItemCount; ++i)
    {
        // link the model matrix to the scene, if still not done
        if (!m_pScene->m_pTransparentItem[i].m_pMatrixList)
            csrSceneAddModelMatrix(m_pScene, m_pScene->m_pTransparentItem[i].m_pModel, &m_ModelMatrix);

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
        csrMat4Multiply(&rotateMatrix,  &translateMatrix, &m_ModelMatrix);
    }
}
//------------------------------------------------------------------------------
void TMainForm::DrawScene()
{
    csrSceneDraw(m_pScene, &m_SceneContext);
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
        m_SceneContext.m_Handle = std::size_t(it.Second().m_hRC);

        // update and draw the scene
        UpdateScene(resize ? 0.0f : elapsedTime);
        DrawScene();

        ::SwapBuffers(it.Second().m_hDC);
    }
}
//---------------------------------------------------------------------------
CSR_Shader* TMainForm::OnGetShader(const void* pModel, CSR_EModelType type)
{
    return m_pCurrentShader;
}
//---------------------------------------------------------------------------
void TMainForm::OnTextureRead(std::size_t index, const CSR_PixelBuffer* pPixelBuffer, int* pNoGPU)
{
    // FIXME
    if (m_pLoadingTexture)
        throw "Too many textures loaded in this demo!";

    // FIXME
    m_pLoadingTexture = csrBufferCreate();
    m_pLoadingTexture->m_pData = malloc(pPixelBuffer->m_DataLength);
    std::memcpy(m_pLoadingTexture->m_pData, pPixelBuffer->m_pData, pPixelBuffer->m_DataLength);
    m_pLoadingTexture->m_Length = pPixelBuffer->m_DataLength;
}
//---------------------------------------------------------------------------
int TMainForm::OnGetTextureIndex(const void* pModel, size_t index, int bumpMap)
{
    if (bumpMap)
        return -1;

    if (pModel != m_pLoadingModel)
        return -1;

    return 254;
}
//---------------------------------------------------------------------------
int TMainForm::OnGetShaderIndex(const void* pModel)
{
    if (pModel != m_pLoadingModel)
        return -1;

    return 254;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnIdle(TObject* pSender, bool& done)
{
    done = false;

    OnDrawScene(false);
}
//---------------------------------------------------------------------------
