/****************************************************************************
 * ==> DirectX model demo --------------------------------------------------*
 ****************************************************************************
 * Description : DirectX (.x) model with bone animation demo                *
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
#include "CSR_X.h"

// classes
#include "CSR_ShaderHelper.h"

#pragma package(smart_init)
#ifdef __llvm__
    #pragma link "glewSL.a"
#else
    #pragma link "glewSL.lib"
#endif
#pragma resource "*.dfm"

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
                  sceneDir = ::ExcludeTrailingPathDelimiter(sceneDir) + L"\\Common";
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
void TMainForm::OnGetXIndexCallback(const CSR_X* pX, size_t* pAnimSetIndex, size_t* pFrameIndex)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return;

    return pMainForm->OnGetXIndex(pX, pAnimSetIndex, pFrameIndex);
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
GLuint TMainForm::LoadTexture(const std::string& fileName) const
{
    try
    {
        // load texture in a picture
        std::auto_ptr<TPicture> pPicture(new TPicture());
        pPicture->LoadFromFile(fileName.c_str());

        // convert it to a bitmap
        std::auto_ptr<TBitmap> pTexture(new TBitmap());
        pTexture->Assign(pPicture->Graphic);

        int pixelSize;

        // search for bitmap pixel format
        switch (pTexture->PixelFormat)
        {
            case pf24bit: pixelSize = 3; break;
            case pf32bit: pixelSize = 4; break;
            default:                     return M_CSR_Error_Code;
        }

        GLuint           textureID    = M_CSR_Error_Code;
        CSR_PixelBuffer* pPixelBuffer = csrPixelBufferCreate();

        try
        {
            // configure the pixel buffer
            pPixelBuffer->m_PixelType    = CSR_PT_BGR;
            pPixelBuffer->m_ImageType    = CSR_IT_Raw;
            pPixelBuffer->m_Width        = pTexture->Width;
            pPixelBuffer->m_Height       = pTexture->Height;
            pPixelBuffer->m_BytePerPixel = pixelSize;
            pPixelBuffer->m_DataLength   = pTexture->Width * pTexture->Height * pixelSize;

            // reserve memory for the pixel array. NOTE use malloc and not new here to be conform
            // with the c standards (otherwise CodeGuard will not be happy)
            pPixelBuffer->m_pData = malloc(pPixelBuffer->m_DataLength);

            TRGBTriple* pLineRGB;
            TRGBQuad*   pLineRGBA;

            // iterate through lines to copy
            for (int y = 0; y < pTexture->Height; ++y)
            {
                // get the next pixel line from bitmap
                if (pixelSize == 3)
                    pLineRGB  = static_cast<TRGBTriple*>(pTexture->ScanLine[y]);
                else
                    pLineRGBA = static_cast<TRGBQuad*>(pTexture->ScanLine[y]);

                // calculate the start y position
                const int yPos = y * pTexture->Width * pixelSize;

                // iterate through pixels to copy
                for (int x = 0; x < pTexture->Width; ++x)
                {
                    // copy to pixel array and take the opportunity to swap the pixel RGB values
                    if (pixelSize == 3)
                    {
                        ((unsigned char*)pPixelBuffer->m_pData)[yPos + (x * 3)]     = pLineRGB[x].rgbtRed;
                        ((unsigned char*)pPixelBuffer->m_pData)[yPos + (x * 3) + 1] = pLineRGB[x].rgbtGreen;
                        ((unsigned char*)pPixelBuffer->m_pData)[yPos + (x * 3) + 2] = pLineRGB[x].rgbtBlue;
                    }
                    else
                    {
                        ((unsigned char*)pPixelBuffer->m_pData)[yPos + (x * 4)]     = pLineRGBA[x].rgbRed;
                        ((unsigned char*)pPixelBuffer->m_pData)[yPos + (x * 4) + 1] = pLineRGBA[x].rgbGreen;
                        ((unsigned char*)pPixelBuffer->m_pData)[yPos + (x * 4) + 2] = pLineRGBA[x].rgbBlue;
                        ((unsigned char*)pPixelBuffer->m_pData)[yPos + (x * 4) + 3] = pLineRGBA[x].rgbReserved;
                    }
                }
            }

            // load the texture on the GPU
            textureID = csrOpenGLTextureFromPixelBuffer(pPixelBuffer);
        }
        __finally
        {
            csrPixelBufferRelease(pPixelBuffer);
        }

        return textureID;
    }
    catch (...)
    {
        return M_CSR_Error_Code;
    }
}
//------------------------------------------------------------------------------
void TMainForm::InitScene(int w, int h)
{
    // create the default scene
    m_pScene = csrSceneCreate();

    // create the scene context
    csrSceneContextInit(&m_SceneContext);
    m_SceneContext.m_fOnSceneBegin = OnSceneBeginCallback;
    m_SceneContext.m_fOnSceneEnd   = OnSceneEndCallback;
    m_SceneContext.m_fOnGetShader  = OnGetShaderCallback;
    m_SceneContext.m_fOnGetXIndex  = OnGetXIndexCallback;
    m_SceneContext.m_fOnGetID      = OnGetIDCallback;

    // configure the scene color background
    m_pScene->m_Color.m_R = 0.08f;
    m_pScene->m_Color.m_G = 0.12f;
    m_pScene->m_Color.m_B = 0.17f;

    // initialize the matrices
    csrMat4Identity(&m_pScene->m_ViewMatrix);
    csrMat4Identity(&m_ModelMatrix);

    // get the shaders
    const std::string vsTextured = CSR_ShaderHelper::GetVertexShader(CSR_ShaderHelper::IE_ST_Texture);
    const std::string fsTextured = CSR_ShaderHelper::GetFragmentShader(CSR_ShaderHelper::IE_ST_Texture);

    // load the shader
    m_pShader = csrOpenGLShaderLoadFromStr(vsTextured.c_str(),
                                           vsTextured.length(),
                                           fsTextured.c_str(),
                                           fsTextured.length(),
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
    m_pShader->m_VertexSlot   = glGetAttribLocation (m_pShader->m_ProgramID, "csr_aVertices");
    m_pShader->m_ColorSlot    = glGetAttribLocation (m_pShader->m_ProgramID, "csr_aColor");
    m_pShader->m_TexCoordSlot = glGetAttribLocation (m_pShader->m_ProgramID, "csr_aTexCoord");
    m_pShader->m_TextureSlot  = glGetUniformLocation(m_pShader->m_ProgramID, "csr_sTexture");

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    CSR_VertexCulling vc;
    vc.m_Type = CSR_CT_Back;
    vc.m_Face = CSR_CF_CCW;

    // load the .x model
    CSR_X* pX = csrXOpen((m_SceneDir + "\\Models\\X\\tiny_4anim.x").c_str(),
                         &vf,
                         &vc,
                          0,
                          0,
                          0,
                          0,
                          OnLoadTextureCallback,
                          OnApplySkinCallback,
                          0);

    // succeeded?
    if (!pX)
    {
        Close();
        return;
    }

    // add it to the scene
    csrSceneAddX(m_pScene, pX, 0, 0);
    csrSceneAddModelMatrix(m_pScene, pX, &m_ModelMatrix);

    // create the rotation matrix
    CSR_Matrix4 rotMat;
    CSR_Vector3 axis;
    axis.m_X = 0.0f;
    axis.m_Y = 0.0f;
    axis.m_Z = 1.0f;
    csrMat4Rotate(M_PI, &axis, &rotMat);

    // create the scale matrix
    CSR_Matrix4 scaleMat;
    csrMat4Identity(&scaleMat);
    scaleMat.m_Table[0][0] = 0.075f;
    scaleMat.m_Table[1][1] = 0.075f;
    scaleMat.m_Table[2][2] = 0.075f;

    // place the model in the 3d world (update the matrix directly)
    csrMat4Multiply(&scaleMat, &rotMat, &m_ModelMatrix);
    m_ModelMatrix.m_Table[3][1] = -25.0f;

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
    // create a point of view from an arcball
    CSR_ArcBall arcball;
    arcball.m_AngleX = 0.0f;
    arcball.m_AngleY = m_Angle;
    arcball.m_Radius = 100.0f;
    csrSceneArcBallToMatrix(&arcball, &m_pScene->m_ViewMatrix);

    // rotate the view around the model
    m_Angle = std::fmod(m_Angle + elapsedTime, M_PI * 2);
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
    return 0;
}
//---------------------------------------------------------------------------
void TMainForm::OnApplySkin(size_t index, const CSR_Skin* pSkin, int* pCanRelease)
{
    if (!pSkin)
        return;

    CSR_OpenGLHelper::AddTexture(&pSkin->m_Texture,
                                  LoadTexture(m_SceneDir + "\\Models\\X\\" + pSkin->m_Texture.m_pFileName),
                                  m_OpenGLResources);
}
//---------------------------------------------------------------------------
void* TMainForm::OnGetShader(const void* pModel, CSR_EModelType type)
{
    return m_pShader;
}
//---------------------------------------------------------------------------
void* TMainForm::OnGetID(const void* pKey)
{
    return CSR_OpenGLHelper::GetTextureID(pKey, m_OpenGLResources);
}
//---------------------------------------------------------------------------
void TMainForm::OnGetXIndex(const CSR_X* pX, size_t* pAnimSetIndex, size_t* pFrameIndex)
{
    // get the animation set (1 = walking) and the frame (limited to 4800 conformly to the model specs)
    *pAnimSetIndex = 1;
    *pFrameIndex   = (m_PreviousTime * 5) % 4800;
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
    return CSR_OpenGLHelper::DeleteTexture(pTexture, m_OpenGLResources);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnIdle(TObject* pSender, bool& done)
{
    done = false;
    OnDrawScene(false);
}
//---------------------------------------------------------------------------
