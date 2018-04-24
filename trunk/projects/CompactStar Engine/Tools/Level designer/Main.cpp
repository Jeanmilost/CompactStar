/*****************************************************************************
 * ==> Main -----------------------------------------------------------------*
 *****************************************************************************
 * Description : This module contains the level designer main form           *
 * Developer   : Jean-Milost Reymond                                         *
 * Copyright   : 2015 - 2018, this file is part of the Minimal API. You are  *
 *               free to copy or redistribute this file, modify it, or use   *
 *               it for your own projects, commercial or not. This file is   *
 *               provided "as is", without ANY WARRANTY OF ANY KIND          *
 *****************************************************************************/

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
#include "CSR_DesignerHelper.h"
#include "CSR_DesignerViewHelper.h"

// dialogs
#include "TAddItemDialog.h"

#pragma package(smart_init)
#ifdef __llvm__
    #pragma link "glewSL.a"
#else
    #pragma link "glewSL.lib"
#endif
#pragma link "OpenAL32E.lib"
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
// TMainForm::IArcBall
//---------------------------------------------------------------------------
TMainForm::IArcBall::IArcBall() :
    m_AngleX(0.0f),
    m_AngleY(0.0f),
    m_Radius(2.0f)
{}
//---------------------------------------------------------------------------
TMainForm::IArcBall::~IArcBall()
{}
//---------------------------------------------------------------------------
// TMainForm
//---------------------------------------------------------------------------
TMainForm* MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* pOwner) :
    TForm(pOwner),
    m_pCurrentView(NULL),
    m_pScene(NULL),
    m_pCollisionInfo(NULL),
    m_pCurrentShader(NULL),
    m_pCurrentMatrix(NULL),
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

    if (!m_pCurrentMatrix)
        return;

    m_OpenGLHelper.ResizeViews(m_pCurrentShader, *m_pCurrentMatrix);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::aeEventsMessage(tagMSG& msg, bool& handled)
{
    switch (msg.message)
    {
        case WM_KEYDOWN:
            switch (msg.wParam)
            {
                case VK_LEFT:     m_ArcBall.m_AngleY = std::fmod(m_ArcBall.m_AngleY + 0.05f, M_PI * 2.0f); handled = true; break;
                case VK_RIGHT:    m_ArcBall.m_AngleY = std::fmod(m_ArcBall.m_AngleY - 0.05f, M_PI * 2.0f); handled = true; break;
                case VK_UP:       m_ArcBall.m_AngleX = std::fmod(m_ArcBall.m_AngleX + 0.05f, M_PI * 2.0f); handled = true; break;
                case VK_DOWN:     m_ArcBall.m_AngleX = std::fmod(m_ArcBall.m_AngleX - 0.05f, M_PI * 2.0f); handled = true; break;
                case VK_ADD:      m_ArcBall.m_Radius =           m_ArcBall.m_Radius + 0.05f;               handled = true; break;
                case VK_SUBTRACT: m_ArcBall.m_Radius =           m_ArcBall.m_Radius - 0.05f;               handled = true; break;
            }

            return;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnSplitterMoved(TObject* pSender)
{
    if (!m_pScene)
        return;

    if (!m_pCurrentMatrix)
        return;

    m_OpenGLHelper.ResizeViews(m_pCurrentShader, *m_pCurrentMatrix);
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
//---------------------------------------------------------------------------
void TMainForm::CalculateMouseRay(TPanel* pView)
{
    // get the mouse position in screen coordinates
    TPoint mousePos = Mouse->CursorPos;

    // convert to view coordinates
    if (!::ScreenToClient(pView->Handle, &mousePos))
        return;

    CSR_Vector2 clientPos;
    clientPos.m_X = mousePos.X;
    clientPos.m_Y = mousePos.Y;

    CSR_Rect clientRect;
    clientRect.m_Min.m_X = 0.0f;
    clientRect.m_Min.m_Y = 0.0f;
    clientRect.m_Max.m_X = pView->ClientWidth;
    clientRect.m_Max.m_Y = pView->ClientHeight;

    CSR_Rect viewRect;

    // get the viewport rectangle
    viewRect.m_Min.m_X = -1.0f;
    viewRect.m_Min.m_Y =  1.0f;
    viewRect.m_Max.m_X =  1.0f;
    viewRect.m_Max.m_Y = -1.0f;

    // calculate the mouse ray in the viewport coordinate system
    csrSceneGetTouchRay(&clientPos, &clientRect, m_pCurrentMatrix, &m_pScene->m_Matrix, &m_Ray);
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

    // configure the scene color
    csrRGBAToColor(0xFF, &m_pScene->m_Color);

    const std::string vsColored = CSR_DesignerHelper::GetVertexShader(CSR_DesignerHelper::IE_ST_Color);
    const std::string fsColored = CSR_DesignerHelper::GetFragmentShader(CSR_DesignerHelper::IE_ST_Color);

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

            std::unique_ptr<CSR_Matrix4> pMatrix(new CSR_Matrix4());

            m_OpenGLHelper.CreateViewport(it.First()->ClientWidth,
                                          it.First()->ClientHeight,
                                          pShader,
                                         *pMatrix.get());

            m_Matrices.push_back(pMatrix.get());
            m_Shaders.push_back(pShader);

            pMatrix.release();
            pShader = NULL;
        }
        __finally
        {
            csrShaderRelease(pShader);
        }
    }

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 0;

    CSR_Material sm;
    sm.m_Color       = 0xFFFF;
    sm.m_Transparent = 0;
    sm.m_Wireframe   = 0;

    CSR_Material bm;
    bm.m_Color       = 0xFF0000FF;
    bm.m_Transparent = 0;
    bm.m_Wireframe   = 0;

    //CSR_Mesh*  pSphere = csrShapeCreateSphere(0.5f, 20, 20, &vf, NULL, &sm, NULL);
    //CSR_Mesh*  pBox    = csrShapeCreateBox(1.0f, 1.0f, 1.0f, 0, &vf, NULL, &bm, NULL);
    //CSR_Model* pModel  = csrWaveFrontOpen("N:\\Jeanmilost\\Devel\\Projects\\CompactStar Engine\\Common\\Models\\WaveFront\\Ahsoka_Tano.obj", &vf, NULL, &sm, NULL, NULL);
    //CSR_Model* pModel  = csrWaveFrontOpen("N:\\Jeanmilost\\Devel\\Projects\\CompactStar Engine\\Common\\Models\\WaveFront\\Balloon.obj", &vf, NULL, &sm, NULL, NULL);
    CSR_Model* pModel  = csrWaveFrontOpen("N:\\Jeanmilost\\Devel\\Projects\\CompactStar Engine\\Common\\Models\\WaveFront\\Model.obj", &vf, NULL, &sm, NULL, NULL);

    //csrSceneAddMesh(m_pScene, pSphere, 0, 1);
    //csrSceneAddMesh(m_pScene, pBox, 0, 1);
    csrSceneAddModel(m_pScene, pModel, 0, 1);

    /*
    CSR_WriteContext writeContext;
    writeContext.m_fOnGetTextureIndex = OnGetTextureIndexCallback;
    writeContext.m_fOnGetShaderIndex  = OnGetShaderIndexCallback;

    CSR_Buffer* pBuffer = csrBufferCreate();
    csrSerializerWriteScene(&writeContext, m_pScene, pBuffer);
    csrFileSave("Scene1.bin", pBuffer);
    csrBufferRelease(pBuffer);

    csrSceneDeleteFrom(m_pScene, pBox);

    CSR_MDL* pMDL = csrMDLOpen("N:\\Jeanmilost\\Devel\\Projects\\CompactStar Engine\\Common\\Models\\MDL\\player.mdl",
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
    */

    // configure OpenGL depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRangef(0.0f, 1.0f);
}
//------------------------------------------------------------------------------
void TMainForm::DeleteScene()
{
    // release the last collision result
    csrCollisionInfoRelease(m_pCollisionInfo);

    // release the scene
    csrSceneRelease(m_pScene);
    m_pScene = NULL;

    // release the matrices
    for (std::size_t i = 0; i < m_Matrices.size(); ++i)
        delete m_Matrices[i];

    // release the shader
    for (std::size_t i = 0; i < m_Shaders.size(); ++i)
        csrShaderRelease(m_Shaders[i]);
}
//------------------------------------------------------------------------------
void TMainForm::UpdateScene(float elapsedTime)
{
    CSR_Vector3 t;
    CSR_Vector3 r;
    CSR_Vector3 factor;
    CSR_Matrix4 translateMatrix;
    CSR_Matrix4 xRotateMatrix;
    CSR_Matrix4 yRotateMatrix;
    CSR_Matrix4 scaleMatrix;
    CSR_Matrix4 combinedMatrixLevel1;
    CSR_Matrix4 combinedMatrixLevel2;

    // build the scene view matrix
    csrSceneArcBallToMatrix(m_ArcBall.m_Radius,
                            m_ArcBall.m_AngleX,
                            m_ArcBall.m_AngleY,
                           &m_pScene->m_Matrix);

    for (std::size_t i = 0; i < m_pScene->m_ItemCount; ++i)
    {
        // link the model matrix to the scene, if still not done
        if (!m_pScene->m_pItem[i].m_pMatrixArray)
            csrSceneAddModelMatrix(m_pScene, m_pScene->m_pItem[i].m_pModel, &m_ModelMatrix);

        // set translation
        t.m_X =  0.0f;
        t.m_Y =  0.0f;
        //t.m_Y = -2.0f;//0.0f;
        t.m_Z = -1.0f;
        //t.m_Z = -5.0f;

        csrMat4Translate(&t, &translateMatrix);

        // set rotation on X axis
        r.m_X = 1.0f;
        r.m_Y = 0.0f;
        r.m_Z = 0.0f;

        // rotate 90 degrees
        //const float xAngle = 1.57075f;
        const float xAngle = 0.0f;//1.57075f;

        csrMat4Rotate(xAngle, &r, &xRotateMatrix);

        // set rotation on Y axis
        r.m_X = 0.0f;
        r.m_Y = 1.0f;
        r.m_Z = 0.0f;

        const float yAngle = 0.0f;

        csrMat4Rotate(yAngle, &r, &yRotateMatrix);

        // set scale factor
        //factor.m_X = 0.05f;//0.25f;//25.0f;//0.25f;
        //factor.m_Y = 0.05f;//0.25f;//25.0f;//0.25f;
        //factor.m_Z = 0.05f;//0.25f;//25.0f;//0.25f;
        factor.m_X = 5.0f;
        factor.m_Y = 5.0f;
        factor.m_Z = 5.0f;
        //factor.m_X = 0.05f;//25.0f;//0.25f;
        //factor.m_Y = 0.05f;//25.0f;//0.25f;
        //factor.m_Z = 0.05f;//25.0f;//0.25f;

        csrMat4Scale(&factor, &scaleMatrix);

        // build model view matrix
        csrMat4Multiply(&scaleMatrix,          &xRotateMatrix,   &combinedMatrixLevel1);
        csrMat4Multiply(&combinedMatrixLevel1, &yRotateMatrix,   &combinedMatrixLevel2);
        csrMat4Multiply(&combinedMatrixLevel1, &translateMatrix, &m_ModelMatrix);
    }

    for (std::size_t i = 0; i < m_pScene->m_TransparentItemCount; ++i)
    {
        // link the model matrix to the scene, if still not done
        if (!m_pScene->m_pTransparentItem[i].m_pMatrixArray)
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

        // set scale factor
        factor.m_X = 5.0f;
        factor.m_Y = 5.0f;
        factor.m_Z = 5.0f;

        csrMat4Scale(&factor, &scaleMatrix);

        // build model view matrix
        csrMat4Multiply(&scaleMatrix,          &xRotateMatrix,   &combinedMatrixLevel1);
        csrMat4Multiply(&combinedMatrixLevel1, &yRotateMatrix,   &combinedMatrixLevel2);
        csrMat4Multiply(&combinedMatrixLevel1, &translateMatrix, &m_ModelMatrix);
    }

    // has a current view?
    if (m_pCurrentView)
    {
        // get the mouse position and transform it in the current view coordinate system
        TPoint mousePos = Mouse->CursorPos;
        ::ScreenToClient(m_pCurrentView->Handle, &mousePos);

        // do calculate the collisions?
        if (m_pCurrentView->ClientRect.Contains(mousePos))
        {
            // calculate the ray from current mouse position
            CalculateMouseRay(m_pCurrentView);

            // release the previous collision result (a new one will be generated)
            csrCollisionInfoRelease(m_pCollisionInfo);

            // create a new collision result
            m_pCollisionInfo = csrCollisionInfoCreate();

            // check the collisions happening in the scene, against the mouse ray
            csrSceneDetectCollision(m_pScene, &m_Ray, m_pCollisionInfo);

            /*REM FIXME
            if (m_pCollisionInfo->m_pModels)
            for (std::size_t i = 0; i < m_pCollisionInfo->m_pModels->m_Count; ++i)
            {
                CSR_CollisionModelInfo* pModelInfo =
                        static_cast<pModelInfo*>(m_pCollisionInfo->m_pModels[i].m_pItem->m_pData);

                if (!pModelInfo)
                    continue;

                switch (pModelInfo->m_Type)
                {
                    case CSR_MT_Mesh:
                    {
                        CSR_Mesh* pMesh = static_cast<CSR_Mesh*>(pModelInfo->m_pModel);

                        if (!pMesh)
                            continue;

                        pMesh->m_pVB[0].m_Material.m_Color = 0xFF0000FF;
                    }
                }
            }
            */
        }
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
        m_pCurrentMatrix = m_Matrices[count];
        ++count;

        // is view control visible?
        if (!CSR_VCLHelper::IsVisible(it.First()))
            continue;

        try
        {
            m_pCurrentView = dynamic_cast<TPanel*>(it.First());

            // enable view context
            wglMakeCurrent(it.Second().m_hDC, it.Second().m_hRC);
            m_SceneContext.m_Handle = std::size_t(it.Second().m_hRC);

            // update and draw the scene
            UpdateScene(resize ? 0.0f : elapsedTime);
            DrawScene();

            ::SwapBuffers(it.Second().m_hDC);
        }
        __finally
        {
            m_pCurrentView = NULL;
        }
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
void __fastcall TMainForm::btAddItemClick(TObject* pSender)
{
    std::unique_ptr<TAddItemDialog> pDialog(new TAddItemDialog(this));

    // show the add item wizard dialog
    if (pDialog->ShowModal() != mrOk)
        return;
}
//---------------------------------------------------------------------------
