/****************************************************************************
 * ==> Collada model demo --------------------------------------------------*
 ****************************************************************************
 * Description : Collada (.dae) model with bone animation demo              *
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

#include <vcl.h>
#pragma hdrstop
#include "Main.h"

// vcl
#include <Vcl.Imaging.jpeg.hpp>
#include <Vcl.Imaging.pngimage.hpp>

// std
#include <memory>
#include <sstream>

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Geometry.h"
#include "CSR_Model.h"

// classes
#include "CSR_ShaderHelper.h"

#pragma package(smart_init)
#ifdef __llvm__
    #pragma link "glewSL.a"
#else
    #pragma link "glewSL.lib"
#endif
#pragma resource "*.dfm"

// resource files to load
#define COLLADA_FILE "cat.dae"

//---------------------------------------------------------------------------
// TMainForm
//---------------------------------------------------------------------------
TMainForm* MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* pOwner) :
    TForm(pOwner),
    m_hDC(NULL),
    m_hRC(NULL),
    m_pScene(NULL),
    m_pShader(NULL),
    m_FrameCount(0),
    m_AnimCount(0),
    m_MaxAnimFrame(36),
    m_PrevOrigin(0),
    m_Angle(M_PI / 2),
    m_FPS(0.0),
    m_StartTime(0),
    m_PreviousTime(0),
    m_Initialized(false)
{
    // build the model dir from the application exe
    UnicodeString sceneDir = ::ExtractFilePath(Application->ExeName);
                  sceneDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(sceneDir));
                  sceneDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(sceneDir));
                  sceneDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(sceneDir));
                  sceneDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(sceneDir));
                  sceneDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(sceneDir));
                  sceneDir = ::ExcludeTrailingPathDelimiter(sceneDir) + L"\\Common\\Models\\Collada\\Cat\\";
                m_SceneDir =   AnsiString(sceneDir).c_str();

    // enable OpenGL
    CSR_OpenGLHelper::EnableOpenGL(Handle, &m_hDC, &m_hRC);

    // stop GLEW crashing on OSX :-/
    glewExperimental = GL_TRUE;

    // initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        // shutdown OpenGL
        CSR_OpenGLHelper::DisableOpenGL(Handle, m_hDC, m_hRC);

        // close the app
        Application->Terminate();
    }
}
//---------------------------------------------------------------------------
__fastcall TMainForm::~TMainForm()
{
    // delete the scene completely
    DeleteScene();

    // shutdown OpenGL
    CSR_OpenGLHelper::DisableOpenGL(Handle, m_hDC, m_hRC);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormShow(TObject* pSender)
{
    // initialize the scene
    InitScene(ClientWidth, ClientHeight);

    // initialize the timer
    m_StartTime    = ::GetTickCount();
    m_PreviousTime = ::GetTickCount();

    // listen the application idle
    Application->OnIdle = OnIdle;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormResize(TObject* pSender)
{
    if (!m_Initialized)
        return;

    // update the viewport
    CSR_OpenGLHelper::CreateViewport(ClientWidth,
                                     ClientHeight,
                                     0.1f,
                                     1000.0f,
                                     m_pShader,
                                     m_pScene->m_ProjectionMatrix);
}
//---------------------------------------------------------------------------
CSR_PixelBuffer* TMainForm::OnLoadTextureCallback(const char* pTextureName)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return 0;

    return pMainForm->OnLoadTexture(pTextureName);
}
//------------------------------------------------------------------------------
void TMainForm::OnApplySkinCallback(size_t index, const CSR_Skin* pSkin, int* pCanRelease)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return;

    return pMainForm->OnApplySkin(index, pSkin, pCanRelease);
}
//------------------------------------------------------------------------------
void* TMainForm::OnGetShaderCallback(const void* pModel, CSR_EModelType type)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return 0;

    return pMainForm->OnGetShader(pModel, type);
}
//---------------------------------------------------------------------------
void* TMainForm::OnGetIDCallback(const void* pKey)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return 0;

    return pMainForm->OnGetID(pKey);
}
//---------------------------------------------------------------------------
void TMainForm::OnGetColladaIndexCallback(const CSR_Collada* pCollada, size_t* pAnimSetIndex, size_t* pFrameIndex)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return;

    return pMainForm->OnGetColladaIndex(pCollada, pAnimSetIndex, pFrameIndex);
}
//---------------------------------------------------------------------------
void TMainForm::OnSceneBeginCallback(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return;

    pMainForm->OnSceneBegin(pScene, pContext);
}
//---------------------------------------------------------------------------
void TMainForm::OnSceneEndCallback(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return;

    pMainForm->OnSceneEnd(pScene, pContext);
}
//---------------------------------------------------------------------------
void TMainForm::OnDeleteTextureCallback(const CSR_Texture* pTexture)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return;

    pMainForm->OnDeleteTexture(pTexture);
}
//---------------------------------------------------------------------------
void TMainForm::BuildModelMatrix(CSR_Matrix4* pMatrix)
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
    csrMat4Rotate((float)(-M_PI / 4.0) + m_Angle, &axis, &rotateYMatrix);

    // set scale factor
    factor.m_X = 2.0f;
    factor.m_Y = 2.0f;
    factor.m_Z = 2.0f;

    // create the scale matrix
    csrMat4Scale(&factor, &scaleMatrix);

    // build the model matrix
    csrMat4Multiply(&scaleMatrix,        &rotateXMatrix, &intermediateMatrix);
    csrMat4Multiply(&intermediateMatrix, &rotateYMatrix, pMatrix);

    // place it in the world
    pMatrix->m_Table[3][0] =  0.0f;
    pMatrix->m_Table[3][1] = -0.2f;
    pMatrix->m_Table[3][2] = -2.0f;
}
//------------------------------------------------------------------------------
void TMainForm::InitScene(int w, int h)
{
    CSR_VertexFormat  vertexFormat;
    CSR_VertexCulling vertexCulling;
    CSR_Material      material;

    // create the default scene
    m_pScene = csrSceneCreate();

    // create the scene context
    csrSceneContextInit(&m_SceneContext);
    m_SceneContext.m_fOnSceneBegin      = OnSceneBeginCallback;
    m_SceneContext.m_fOnSceneEnd        = OnSceneEndCallback;
    m_SceneContext.m_fOnGetShader       = OnGetShaderCallback;
    m_SceneContext.m_fOnGetColladaIndex = OnGetColladaIndexCallback;
    m_SceneContext.m_fOnGetID           = OnGetIDCallback;

    // configure the scene color background
    m_pScene->m_Color.m_R = 0.08f;
    m_pScene->m_Color.m_G = 0.12f;
    m_pScene->m_Color.m_B = 0.17f;

    // initialize the matrices
    csrMat4Identity(&m_pScene->m_ViewMatrix);
    csrMat4Identity(&m_ModelMatrix);

    // get the shaders
    const std::string vsColored = CSR_ShaderHelper::GetVertexShader  (CSR_ShaderHelper::IE_ST_Color);
    const std::string fsColored = CSR_ShaderHelper::GetFragmentShader(CSR_ShaderHelper::IE_ST_Color);

    // load the shader
    m_pShader = csrOpenGLShaderLoadFromStr(vsColored.c_str(),
                                           vsColored.length(),
                                           fsColored.c_str(),
                                           fsColored.length(),
                                           0,
                                           0);

    // succeeded?
    if (!m_pShader)
    {
        Close();
        return;
    }

    // enable the shader program
    csrShaderEnable(m_pShader);

    // get shader attributes
    m_pShader->m_VertexSlot = glGetAttribLocation(m_pShader->m_ProgramID, "csr_aVertices");
    m_pShader->m_NormalSlot = glGetAttribLocation(m_pShader->m_ProgramID, "csr_aNormal");
    m_pShader->m_ColorSlot  = glGetAttribLocation(m_pShader->m_ProgramID, "csr_aColor");

    // configure the vertex format
    vertexFormat.m_HasNormal         = 0;
    vertexFormat.m_HasTexCoords      = 0;
    vertexFormat.m_HasPerVertexColor = 1;

    vertexCulling.m_Face = CSR_CF_CCW;
    vertexCulling.m_Type = CSR_CT_Back;

    // configure the material
    material.m_Color       = 0x808080FF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    // load the Collada model
    CSR_Collada* pCollada = csrColladaOpen((m_SceneDir + COLLADA_FILE).c_str(),
                                           &vertexFormat,
                                           &vertexCulling,
                                           &material,
                                            0,
                                            0,
                                            0,
                                            OnLoadTextureCallback,
                                            OnApplySkinCallback,
                                            OnDeleteTextureCallback);

    // succeeded?
    if (!pCollada)
    {
        Close();
        return;
    }

    BuildModelMatrix(&m_ModelMatrix);

    // add it to the scene
    csrSceneAddCollada(m_pScene, pCollada, 0, 0);
    csrSceneAddModelMatrix(m_pScene, pCollada, &m_ModelMatrix);

    m_Initialized = true;
}
//------------------------------------------------------------------------------
void TMainForm::DeleteScene()
{
    m_Initialized = false;

    // release the scene
    csrSceneRelease(m_pScene, OnDeleteTextureCallback);

    // release the shader
    csrOpenGLShaderRelease(m_pShader);
}
//------------------------------------------------------------------------------
void TMainForm::UpdateScene(float elapsedTime)
{
    // rebuild the model matrix
    BuildModelMatrix(&m_ModelMatrix);

    // rotate the view around the model
    m_Angle = std::fmod(m_Angle + elapsedTime, M_PI * 2);

    ++m_AnimCount;
}
//------------------------------------------------------------------------------
void TMainForm::DrawScene()
{
    // draw the scene
    csrSceneDraw(m_pScene, &m_SceneContext);
}
//---------------------------------------------------------------------------
void TMainForm::OnDrawScene(bool resize)
{
    // do draw the scene for a resize?
    if (resize)
    {
        if (!m_Initialized)
            return;

        // just process a minimal draw
        UpdateScene(0.0);
        DrawScene();

        ::SwapBuffers(m_hDC);
        return;
    }

    // calculate time interval
    const unsigned __int64 now            = ::GetTickCount();
    const double           elapsedTime    = (now - m_PreviousTime) / 1000.0;
                           m_PreviousTime =  now;

    if (!m_Initialized)
        return;

    ++m_FrameCount;

    // calculate the FPS
    if (m_FrameCount >= 100)
    {
        const double      smoothing = 0.1;
        const std::size_t fpsTime   = now > m_StartTime ? now - m_StartTime : 1;
        const std::size_t newFPS    = (m_FrameCount * 100) / fpsTime;
        m_StartTime                 = ::GetTickCount();
        m_FrameCount                = 0;
        m_FPS                       = (newFPS * smoothing) + (m_FPS * (1.0 - smoothing));
    }

    // update and draw the scene
    UpdateScene(elapsedTime);
    DrawScene();

    ::SwapBuffers(m_hDC);
}
//---------------------------------------------------------------------------
CSR_PixelBuffer* TMainForm::OnLoadTexture(const char* pTextureName)
{
    const std::string resName = m_SceneDir + pTextureName;

    return csrPixelBufferFromTgaFile(resName.c_str());
}
//---------------------------------------------------------------------------
void TMainForm::OnApplySkin(size_t index, const CSR_Skin* pSkin, int* pCanRelease)
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
                                 m_OpenGLResources);

    *pCanRelease = 1;
}
//---------------------------------------------------------------------------
void* TMainForm::OnGetShader(const void* pModel, CSR_EModelType type)
{
    return m_pShader;
}
//---------------------------------------------------------------------------
void* TMainForm::OnGetID(const void* pKey)
{
    const CSR_Texture* pTexture = static_cast<const CSR_Texture*>(pKey);

    if (!pTexture->m_pFileName)
        return NULL;

    const std::string key = pTexture->m_pFileName;

    return CSR_OpenGLHelper::GetTextureID(key, m_OpenGLResources);
}
//---------------------------------------------------------------------------
void TMainForm::OnGetColladaIndex(const CSR_Collada* pCollada, size_t* pAnimSetIndex, size_t* pFrameIndex)
{
    *pAnimSetIndex = 0;
    *pFrameIndex   = (m_AnimCount / 10) % m_MaxAnimFrame;
}
//---------------------------------------------------------------------------
void TMainForm::OnSceneBegin(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    csrDrawBegin(&pScene->m_Color);
}
//---------------------------------------------------------------------------
void TMainForm::OnSceneEnd(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    csrDrawEnd();
}
//---------------------------------------------------------------------------
void TMainForm::OnDeleteTexture(const CSR_Texture* pTexture)
{
    if (!pTexture)
        return;

    if (!pTexture->m_pFileName)
        return;

    const std::string key = pTexture->m_pFileName;

    CSR_OpenGLHelper::DeleteTexture(key, m_OpenGLResources);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnIdle(TObject* pSender, bool& done)
{
    done = false;
    OnDrawScene(false);
}
//---------------------------------------------------------------------------

