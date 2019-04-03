/****************************************************************************
 * ==> Spaceship game demo -------------------------------------------------*
 ****************************************************************************
 * Description : A spaceship game demo                                      *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2019, this file is part of the CompactStar Engine.  *
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
#include "CSR_Scene.h"

// classes
#include "CSR_OpenGLHelper.h"
#include "CSR_ShaderHelper.h"

#pragma package(smart_init)
#ifdef __llvm__
    #pragma link "glewSL.a"
#else
    #pragma link "glewSL.lib"
#endif
#pragma link "openAL32E.lib"
#pragma resource "*.dfm"

// texture and model files
#define BACKGROUND_TEXTURE "\\background.bmp"
#define STAR_TEXTURE       "\\star.bmp"
#define SPACESHIP_MODEL    "\\spaceship.mdl"
#define METEORE_MODEL      "\\meteore.mdl"
#define FIRE_SOUND         "\\fire.wav"
#define EXPLOSION_SOUND    "\\explosion.wav"

//----------------------------------------------------------------------------
// Shaders
//----------------------------------------------------------------------------
const char g_VSColorAlpha[] =
    "precision mediump float;"
    "attribute vec3  csr_aVertices;"
    "attribute vec4  csr_aColor;"
    "uniform   float csr_uAlpha;"
    "uniform   mat4  csr_uProjection;"
    "uniform   mat4  csr_uView;"
    "uniform   mat4  csr_uModel;"
    "varying   vec4  csr_vColor;"
    "varying   float csr_fAlpha;"
    "void main(void)"
    "{"
    "    csr_vColor  = csr_aColor;"
    "    csr_fAlpha  = csr_uAlpha;"
    "    gl_Position = csr_uProjection * csr_uView * csr_uModel * vec4(csr_aVertices, 1.0);"
    "}";
//----------------------------------------------------------------------------
const char g_FSColorAlpha[] =
    "precision mediump float;"
    "varying lowp vec4  csr_vColor;"
    "varying      float csr_fAlpha;"
    "void main(void)"
    "{"
    "    gl_FragColor = vec4(csr_vColor.x, csr_vColor.y, csr_vColor.z, csr_fAlpha);"
    "}";
//----------------------------------------------------------------------------
const char g_VSTextured[] =
    "precision mediump float;"
    "attribute vec3  csr_aVertices;"
    "attribute vec4  csr_aColor;"
    "attribute vec2  csr_aTexCoord;"
    "uniform   mat4  csr_uProjection;"
    "uniform   mat4  csr_uView;"
    "uniform   mat4  csr_uModel;"
    "uniform   float csr_uAlpha;"
    "varying   vec4  csr_vColor;"
    "varying   vec2  csr_vTexCoord;"
    "varying   float csr_fAlpha;"
    "void main(void)"
    "{"
    "    csr_vColor    = csr_aColor;"
    "    csr_fAlpha    = csr_uAlpha;"
    "    csr_vTexCoord = csr_aTexCoord;"
    "    gl_Position   = csr_uProjection * csr_uView * csr_uModel * vec4(csr_aVertices, 1.0);"
    "}";
//----------------------------------------------------------------------------
const char g_FSTextured[] =
    "precision mediump float;"
    "uniform sampler2D  csr_sTexture;"
    "varying lowp vec4  csr_vColor;"
    "varying      vec2  csr_vTexCoord;"
    "varying      float csr_fAlpha;"
    "void main(void)"
    "{"
    "    vec4 color   = vec4(csr_vColor.x, csr_vColor.y, csr_vColor.z, csr_fAlpha);"
    "    gl_FragColor = color * texture2D(csr_sTexture, csr_vTexCoord);"
    "}";
//---------------------------------------------------------------------------
// TMainForm
//---------------------------------------------------------------------------
TMainForm* MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* pOwner) :
    TForm(pOwner),
    m_hDC(NULL),
    m_hRC(NULL),
    m_pOpenALDevice(NULL),
    m_pOpenALContext(NULL),
    m_pScene(NULL),
    m_pShader(NULL),
    m_pFlameShader(0),
    m_Angle(0.0f),
    m_Alpha(0.3f),
    m_MinAlpha(0.25f),
    m_MaxAlpha(0.35f),
    m_AlphaOffset(1.0f),
    m_ShipAlpha(1.0f),
    m_LaserTime(0.0f),
    m_LaserInterval(100.0f),
    m_RotateVelocity(0.1f),
    m_Aspect(1.0f),
    m_LaserVisible(false),
    m_ShipDying(false),
    m_pMeteore(0),
    m_pFlame(0),
    m_pLaser(0),
    m_pStar(0),
    m_pStars(0),
    m_pMeteores(0),
    m_AlphaSlot(0),
    m_TexAlphaSlot(0),
    m_pFireSound(0),
    m_pExplosionSound(0),
    m_FrameCount(0),
    m_FPS(0.0),
    m_StartTime(0),
    m_PreviousTime(0),
    m_Initialized(false)
{
    // build the model dir from the application exe
    UnicodeString sceneDir = ::ExtractFilePath(Application->ExeName);
                  sceneDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(sceneDir));
                  sceneDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(sceneDir));
                  sceneDir = ::ExcludeTrailingPathDelimiter(sceneDir) + L"\\Resources";
                m_SceneDir =   AnsiString(sceneDir).c_str();

    // initialize OpenAL
    csrSoundInitializeOpenAL(&m_pOpenALDevice, &m_pOpenALContext);

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

    // release OpenAL interface
    csrSoundReleaseOpenAL(m_pOpenALDevice, m_pOpenALContext);
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
                                     0.01f,
                                     100.0f,
                                     m_pShader,
                                     m_pScene->m_ProjectionMatrix);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::aeEventsMessage(tagMSG& msg, bool& handled)
{
    if (m_ShipDying)
        return;

    switch (msg.message)
    {
        case WM_KEYDOWN:
            switch (msg.wParam)
            {
                case VK_LEFT:  m_Angle        = std::fmod(m_Angle + m_RotateVelocity, M_PI * 2.0f); return;
                case VK_RIGHT: m_Angle        = std::fmod(m_Angle - m_RotateVelocity, M_PI * 2.0f); return;

                case VK_SPACE:
                {
                    CSR_Vector2 laserPos;
                    CSR_Vector2 laserDir;

                    laserPos.m_X =  0.0f;
                    laserPos.m_X =  0.0f;
                    laserDir.m_X = -sinf(m_Angle);
                    laserDir.m_Y =  cosf(m_Angle);

                    // compute laser ray
                    csrRay2FromPointDir(&laserPos, &laserDir, &m_LaserRay);

                    // play laser sound
                    if (m_pFireSound)
                        csrSoundPlay(m_pFireSound);

                    // show laser
                    m_LaserVisible = true;
                }
            }
    }
}
//---------------------------------------------------------------------------
void TMainForm::OnCalculateStarMotionCallback(const CSR_Particles* pParticles,
                                                    CSR_Particle*  pParticle,
                                                    float          elapsedTime)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return;

    pMainForm->OnCalculateStarMotion(pParticles, pParticle, elapsedTime);
}
//---------------------------------------------------------------------------
void TMainForm::OnCalculateMeteoreMotionCallback(const CSR_Particles* pParticles,
                                                       CSR_Particle*  pParticle,
                                                       float          elapsedTime)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return;

    pMainForm->OnCalculateMeteoreMotion(pParticles, pParticle, elapsedTime);
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
void TMainForm::OnApplySkinCallback(size_t index, const CSR_Skin* pSkin, int* pCanRelease)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return;

    return pMainForm->OnApplySkin(index, pSkin, pCanRelease);
}
//---------------------------------------------------------------------------
void* TMainForm::OnGetIDCallback(const void* pKey)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return NULL;

    return pMainForm->OnGetID(pKey);
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
//---------------------------------------------------------------------------
bool TMainForm::CheckLaserCollision(const CSR_Circle* pMeteoreBoundingCircle, const CSR_Ray2* pLaserRay) const
{
    CSR_Figure2 figure1;
    CSR_Figure2 figure2;

    figure1.m_Type    = CSR_F2_Ray;
    figure1.m_pFigure = pLaserRay;

    figure2.m_Type    = CSR_F2_Circle;
    figure2.m_pFigure = pMeteoreBoundingCircle;

    return csrIntersect2(&figure1, &figure2, 0, 0);
}
//---------------------------------------------------------------------------
bool TMainForm::CheckSpaceshipCollision(const CSR_Circle* pMeteoreBoundingCircle,
                                        const CSR_Circle* pSpaceshipBoundingCircle) const
{
    CSR_Figure2 figure1;
    CSR_Figure2 figure2;
    CSR_Vector2 p1;
    CSR_Vector2 p2;

    figure1.m_Type    = CSR_F2_Circle;
    figure1.m_pFigure = pMeteoreBoundingCircle;

    figure2.m_Type    = CSR_F2_Circle;
    figure2.m_pFigure = pSpaceshipBoundingCircle;

    return csrIntersect2(&figure1, &figure2, &p1, &p2);
}
//---------------------------------------------------------------------------
void TMainForm::BuildSpaceshipMatrix(float angle)
{
    CSR_Vector3 translation;
    translation.m_X =  0.0f;
    translation.m_Y =  0.0f;
    translation.m_Z = -1.75f;

    CSR_Matrix4 translationMatrix;

    // apply translation to spaceship matrix
    csrMat4Translate(&translation, &translationMatrix);

    CSR_Vector3 axis;
    axis.m_X = 0.0f;
    axis.m_Y = 0.0f;
    axis.m_Z = 1.0f;

    CSR_Matrix4 rzMatrix;

    // apply rotation on z axis to spaceship matrix
    csrMat4Rotate(angle, &axis, &rzMatrix);

    CSR_Vector3 factor;
    factor.m_X = 0.05f;
    factor.m_Y = 0.05f;
    factor.m_Z = 0.05f;

    CSR_Matrix4 scaleMatrix;

    // apply scaling to spaceship matrix
    csrMat4Scale(&factor, &scaleMatrix);

    CSR_Matrix4 buildMatrix;

    // build the spaceship model matrix
    csrMat4Multiply(&scaleMatrix, &rzMatrix, &buildMatrix);
    csrMat4Multiply(&buildMatrix, &translationMatrix, &m_Spaceship.m_Matrix);
}
//---------------------------------------------------------------------------
void TMainForm::BuildFlameMatrix(float angle)
{
    CSR_Vector3 translation;
    translation.m_X =  0.0f;
    translation.m_Y = -0.2f;
    translation.m_Z = -1.75f;

    CSR_Matrix4 translationMatrix;

    // apply translation to spaceship rear flame matrix
    csrMat4Translate(&translation, &translationMatrix);

    CSR_Vector3 axis;
    axis.m_X = 0.0f;
    axis.m_Y = 0.0f;
    axis.m_Z = 1.0f;

    CSR_Matrix4 rzMatrix;

    // apply rotation on z axis to spaceship rear flame matrix
    csrMat4Rotate(angle, &axis, &rzMatrix);

    // build the spaceship rear flame matrix
    csrMat4Multiply(&translationMatrix, &rzMatrix, &m_FlameMatrix);
}
//---------------------------------------------------------------------------
void TMainForm::BuildMeteoreMatrix(const CSR_Vector3* pTranslation,
                                   const CSR_Vector3* pRotation,
                                   const CSR_Vector3* pScaling,
                                         CSR_Matrix4* pMeteoreMatrix) const
{
    CSR_Matrix4 translationMatrix;

    // apply translation to meteore matrix
    csrMat4Translate(pTranslation, &translationMatrix);

    CSR_Vector3 axis;
    axis.m_X = 1.0f;
    axis.m_Y = 0.0f;
    axis.m_Z = 0.0f;

    CSR_Matrix4 rxMatrix;

    // apply rotation on x axis to meteore matrix
    csrMat4Rotate(pRotation->m_X, &axis, &rxMatrix);

    axis.m_X = 0.0f;
    axis.m_Y = 1.0f;
    axis.m_Z = 0.0f;

    CSR_Matrix4 ryMatrix;

    // apply rotation on y axis to meteore matrix
    csrMat4Rotate(pRotation->m_Y, &axis, &ryMatrix);

    axis.m_X = 0.0f;
    axis.m_Y = 0.0f;
    axis.m_Z = 1.0f;

    CSR_Matrix4 rzMatrix;

    // apply rotation on z axis to meteore matrix
    csrMat4Rotate(pRotation->m_Z, &axis, &rzMatrix);

    CSR_Matrix4 scaleMatrix;

    // apply scaling to meteore matrix
    csrMat4Scale(pScaling, &scaleMatrix);

    CSR_Matrix4 intermediateMatrix1;
    CSR_Matrix4 intermediateMatrix2;
    CSR_Matrix4 intermediateMatrix3;

    // build the meteore matrix
    csrMat4Multiply(&scaleMatrix,         &rxMatrix,          &intermediateMatrix1);
    csrMat4Multiply(&intermediateMatrix1, &ryMatrix,          &intermediateMatrix2);
    csrMat4Multiply(&intermediateMatrix2, &rzMatrix,          &intermediateMatrix3);
    csrMat4Multiply(&intermediateMatrix3, &translationMatrix,  pMeteoreMatrix);
}
//---------------------------------------------------------------------------
void TMainForm::BuildLaserMatrix(float angle)
{
    float posOffset;

    if (m_LaserVisible)
        posOffset = 5.15f;
    else
        posOffset = 99999.0f;

    CSR_Vector3 translation;
    translation.m_X =  posOffset * -sinf(m_Angle);
    translation.m_Y =  posOffset *  cosf(m_Angle);
    translation.m_Z = -1.75f;

    CSR_Matrix4 translationMatrix;

    // apply translation to laser matrix
    csrMat4Translate(&translation, &translationMatrix);

    CSR_Vector3 axis;
    axis.m_X = 0.0f;
    axis.m_Y = 0.0f;
    axis.m_Z = 1.0f;

    CSR_Matrix4 rzMatrix;

    // apply rotation on z axis to laser matrix
    csrMat4Rotate(angle, &axis, &rzMatrix);

    // build the laser model matrix
    csrMat4Multiply(&rzMatrix, &translationMatrix, &m_LaserMatrix);
}
//---------------------------------------------------------------------------
void TMainForm::OnCalculateStarMotion(const CSR_Particles* pParticles,
                                            CSR_Particle*  pParticle,
                                            float          elapsedTime) const
{
    pParticle->m_pBody->m_Velocity.m_X =  0.5f * sinf(m_Angle);
    pParticle->m_pBody->m_Velocity.m_Y = -0.5f * cosf(m_Angle);

    // calculate next star position
    pParticle->m_pMatrix->m_Table[3][0] += pParticle->m_pBody->m_Velocity.m_X * elapsedTime;
    pParticle->m_pMatrix->m_Table[3][1] += pParticle->m_pBody->m_Velocity.m_Y * elapsedTime;
    pParticle->m_pMatrix->m_Table[3][2] += pParticle->m_pBody->m_Velocity.m_Z * elapsedTime;

    // limit x pos inside the star box
    if (pParticle->m_pMatrix->m_Table[3][0] <= m_StarBox.m_Min.m_X)
        pParticle->m_pMatrix->m_Table[3][0] += (m_StarBox.m_Max.m_X - m_StarBox.m_Min.m_X);
    else
    if (pParticle->m_pMatrix->m_Table[3][0] >= m_StarBox.m_Max.m_X)
        pParticle->m_pMatrix->m_Table[3][0] -= (m_StarBox.m_Max.m_X - m_StarBox.m_Min.m_X);

    // limit y pos inside the star box
    if (pParticle->m_pMatrix->m_Table[3][1] <= m_StarBox.m_Min.m_Y)
        pParticle->m_pMatrix->m_Table[3][1] += (m_StarBox.m_Max.m_Y - m_StarBox.m_Min.m_Y);
    else
    if (pParticle->m_pMatrix->m_Table[3][1] >= m_StarBox.m_Max.m_Y)
        pParticle->m_pMatrix->m_Table[3][1] -= (m_StarBox.m_Max.m_Y - m_StarBox.m_Min.m_Y);

    // limit z pos inside the star box
    if (pParticle->m_pMatrix->m_Table[3][2] <= m_StarBox.m_Min.m_Z)
        pParticle->m_pMatrix->m_Table[3][2] += (m_StarBox.m_Max.m_Z - m_StarBox.m_Min.m_Z);
    else
    if (pParticle->m_pMatrix->m_Table[3][2] >= m_StarBox.m_Max.m_Z)
        pParticle->m_pMatrix->m_Table[3][2] -= (m_StarBox.m_Max.m_Z - m_StarBox.m_Min.m_Z);
}
//---------------------------------------------------------------------------
void TMainForm::OnCalculateMeteoreMotion(const CSR_Particles* pParticles,
                                               CSR_Particle*  pParticle,
                                               float          elapsedTime)
{
    CSR_Vector3 position;
    size_t      index;

    // get particle index (mass was used for that)
    index = (size_t)pParticle->m_pBody->m_Mass;

    // calculate velocity on x and y axis
    pParticle->m_pBody->m_Velocity.m_X =  0.5f * sinf(m_Angle);
    pParticle->m_pBody->m_Velocity.m_Y = -0.5f * cosf(m_Angle);

    // calculate next meteore position
    position.m_X = pParticle->m_pMatrix->m_Table[3][0] + pParticle->m_pBody->m_Velocity.m_X * elapsedTime;
    position.m_Y = pParticle->m_pMatrix->m_Table[3][1] + pParticle->m_pBody->m_Velocity.m_Y * elapsedTime;
    position.m_Z = pParticle->m_pMatrix->m_Table[3][2] + pParticle->m_pBody->m_Velocity.m_Z * elapsedTime;

    // calculate next meteore rotation
    m_Meteores[index].m_Rotation.m_X += m_Meteores[index].m_RotationVelocity.m_X * elapsedTime;
    m_Meteores[index].m_Rotation.m_Y += m_Meteores[index].m_RotationVelocity.m_Y * elapsedTime;
    m_Meteores[index].m_Rotation.m_Z += m_Meteores[index].m_RotationVelocity.m_Z * elapsedTime;

    // build meteore matrix
    BuildMeteoreMatrix(&position,
                       &m_Meteores[index].m_Rotation,
                       &m_Meteores[index].m_Scaling,
                        pParticle->m_pMatrix);

    // put the meteore bounding circle to the location to test
    m_MeteoreBoundingCircle.m_Center.m_X = pParticle->m_pMatrix->m_Table[3][0];
    m_MeteoreBoundingCircle.m_Center.m_Y = pParticle->m_pMatrix->m_Table[3][1];

    // hit by a laser?
    if (m_LaserVisible && CheckLaserCollision(&m_MeteoreBoundingCircle, &m_LaserRay))
    {
        // sort new positions
        float newPosX = (((float)(rand() % (int)200.0f)) * 0.01f);
        float newPosY = (((float)(rand() % (int)200.0f)) * 0.01f);

        // recalculate a new position out of the screen, on the left or on the right
        if (newPosX >= 1.0f)
            pParticle->m_pMatrix->m_Table[3][0] = m_StarBox.m_Max.m_X - (newPosX - 1.0f);
        else
            pParticle->m_pMatrix->m_Table[3][0] = m_StarBox.m_Min.m_X + newPosX;

        // recalculate a new position out of the screen, on the top or on the bottom
        if (newPosY >= 1.0f)
            pParticle->m_pMatrix->m_Table[3][1] = m_StarBox.m_Max.m_Y - (newPosY - 1.0f);
        else
            pParticle->m_pMatrix->m_Table[3][1] = m_StarBox.m_Min.m_Y + newPosY;

        pParticle->m_pMatrix->m_Table[3][2] = m_Meteores[index].m_StartPos.m_Z;
    }

    // hitting the spaceship?
    if (!m_ShipDying && CheckSpaceshipCollision(&m_MeteoreBoundingCircle, &m_SpaceshipBoundingCircle))
    {
        m_ShipDying = true;
        csrSoundPlay(m_pExplosionSound);
    }

    // limit x pos inside the star box
    if (pParticle->m_pMatrix->m_Table[3][0] <= m_StarBox.m_Min.m_X)
        pParticle->m_pMatrix->m_Table[3][0] += (m_StarBox.m_Max.m_X - m_StarBox.m_Min.m_X);
    else
    if (pParticle->m_pMatrix->m_Table[3][0] >= m_StarBox.m_Max.m_X)
        pParticle->m_pMatrix->m_Table[3][0] -= (m_StarBox.m_Max.m_X - m_StarBox.m_Min.m_X);

    // limit y pos inside the star box
    if (pParticle->m_pMatrix->m_Table[3][1] <= m_StarBox.m_Min.m_Y)
        pParticle->m_pMatrix->m_Table[3][1] += (m_StarBox.m_Max.m_Y - m_StarBox.m_Min.m_Y);
    else
    if (pParticle->m_pMatrix->m_Table[3][1] >= m_StarBox.m_Max.m_Y)
        pParticle->m_pMatrix->m_Table[3][1] -= (m_StarBox.m_Max.m_Y - m_StarBox.m_Min.m_Y);

    // limit z pos inside the star box
    if (pParticle->m_pMatrix->m_Table[3][2] <= m_StarBox.m_Min.m_Z)
        pParticle->m_pMatrix->m_Table[3][2] += (m_StarBox.m_Max.m_Z - m_StarBox.m_Min.m_Z);
    else
    if (pParticle->m_pMatrix->m_Table[3][2] >= m_StarBox.m_Max.m_Z)
        pParticle->m_pMatrix->m_Table[3][2] -= (m_StarBox.m_Max.m_Z - m_StarBox.m_Min.m_Z);
}
//------------------------------------------------------------------------------
void TMainForm::InitScene(int w, int h)
{
    // create the scene
    m_pScene = csrSceneCreate();

    // configure the scene background color
    m_pScene->m_Color.m_R = 0.0f;
    m_pScene->m_Color.m_G = 0.0f;
    m_pScene->m_Color.m_B = 0.0f;
    m_pScene->m_Color.m_A = 1.0f;

    // configure the scene ground direction
    m_pScene->m_GroundDir.m_X =  0.0f;
    m_pScene->m_GroundDir.m_Y = -1.0f;
    m_pScene->m_GroundDir.m_Z =  0.0f;

    // configure the scene view matrix
    csrMat4Identity(&m_pScene->m_ViewMatrix);

    // configure the scene context
    csrSceneContextInit(&m_SceneContext);
    m_SceneContext.m_fOnSceneBegin    = OnSceneBeginCallback;
    m_SceneContext.m_fOnSceneEnd      = OnSceneEndCallback;
    m_SceneContext.m_fOnGetShader     = OnGetShaderCallback;
    m_SceneContext.m_fOnGetID         = OnGetIDCallback;
    m_SceneContext.m_fOnDeleteTexture = OnDeleteTextureCallback;

    // compile, link and use shader
    m_pShader = csrOpenGLShaderLoadFromStr(&g_VSTextured[0],
                                            sizeof(g_VSTextured),
                                           &g_FSTextured[0],
                                            sizeof(g_FSTextured),
                                            0,
                                            0);

    // succeeded?
    if (!m_pShader)
        // show the error message to the user
        throw new std::runtime_error("Failed to load the scene shader.\n");

    csrShaderEnable(m_pShader);

    // get shader attributes
    m_pShader->m_VertexSlot   = glGetAttribLocation(m_pShader->m_ProgramID,  "csr_aVertices");
    m_pShader->m_ColorSlot    = glGetAttribLocation(m_pShader->m_ProgramID,  "csr_aColor");
    m_pShader->m_TexCoordSlot = glGetAttribLocation(m_pShader->m_ProgramID,  "csr_aTexCoord");
    m_pShader->m_TextureSlot  = glGetAttribLocation(m_pShader->m_ProgramID,  "csr_sTexture");
    m_TexAlphaSlot            = glGetUniformLocation(m_pShader->m_ProgramID, "csr_uAlpha");

    // compile, link and use flame shader
    m_pFlameShader = csrOpenGLShaderLoadFromStr(&g_VSColorAlpha[0],
                                                 sizeof(g_VSColorAlpha),
                                                &g_FSColorAlpha[0],
                                                 sizeof(g_FSColorAlpha),
                                                 0,
                                                 0);

    // succeeded?
    if (!m_pFlameShader)
        // show the error message to the user
        throw new std::runtime_error("Failed to load the flame shader.\n");

    csrShaderEnable(m_pFlameShader);

    // get flame shader attributes
    m_pFlameShader->m_VertexSlot = glGetAttribLocation(m_pFlameShader->m_ProgramID,  "csr_aVertices");
    m_pFlameShader->m_ColorSlot  = glGetAttribLocation(m_pFlameShader->m_ProgramID,  "csr_aColor");
    m_AlphaSlot                  = glGetUniformLocation(m_pFlameShader->m_ProgramID, "csr_uAlpha");

    csrShaderEnable(m_pShader);

    // configure OpenGL depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRangef(0.0f, 1.0f);

    CSR_VertexFormat vertexFormat;
    vertexFormat.m_HasNormal         = 0;
    vertexFormat.m_HasPerVertexColor = 1;
    vertexFormat.m_HasTexCoords      = 1;

    CSR_Material material;
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 1;
    material.m_Wireframe   = 0;

    // create the spaceship
    CSR_MDL* pMDL = csrMDLOpen((m_SceneDir + SPACESHIP_MODEL).c_str(),
                               0,
                              &vertexFormat,
                               0,
                              &material,
                               0,
                               OnApplySkinCallback,
                               OnDeleteTextureCallback);

    m_Spaceship.m_pKey = pMDL;

    // build the spaceship matrix
    BuildSpaceshipMatrix(0.0f);

    // add the model to the scene
    CSR_SceneItem* pSceneItem = csrSceneAddMDL(m_pScene, pMDL, 1, 0);
    csrSceneAddModelMatrix(m_pScene, pMDL, &m_Spaceship.m_Matrix);

    // configure spaceship bounding sphere
    m_SpaceshipBoundingCircle.m_Center.m_X = 0.0f;
    m_SpaceshipBoundingCircle.m_Center.m_Y = 0.0f;
    m_SpaceshipBoundingCircle.m_Radius     = 0.15f;

    material.m_Color       = 0x00B4FFFF;
    material.m_Transparent = 1;

    // build the rear flame model
    m_pFlame = csrShapeCreateCylinder(0.001f,
                                      0.025f,
                                      0.125f,
                                      10,
                                     &vertexFormat,
                                      0,
                                     &material,
                                      0);

    // initialize the rear flame model matrix
    BuildFlameMatrix(0.0f);

    // add the model to the scene
    pSceneItem = csrSceneAddMesh(m_pScene, m_pFlame, 1, 0);
    csrSceneAddModelMatrix(m_pScene, m_pFlame, &m_FlameMatrix);

    material.m_Color = 0xFF1122FF;

    // build the laser model
    m_pLaser = csrShapeCreateCylinder(0.01f,
                                      0.01f,
                                      10.0f,
                                      10,
                                     &vertexFormat,
                                      0,
                                     &material,
                                      0);

    // initialize the laser model matrix
    BuildLaserMatrix(0.0f);

    // add the model to the scene
    pSceneItem = csrSceneAddMesh(m_pScene, m_pLaser, 1, 0);
    csrSceneAddModelMatrix(m_pScene, m_pLaser, &m_LaserMatrix);

    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;

    // create the background surface
    CSR_Mesh* pMesh = csrShapeCreateSurface(10.0f,
                                            10.0f,
                                           &vertexFormat,
                                            0,
                                           &material,
                                            0);

    // initialize background matrix
    csrMat4Identity(&m_Background);
    m_Background.m_Table[3][2] = -5.0f;

    // add the mesh to the scene
    pSceneItem = csrSceneAddMesh(m_pScene, pMesh, 0, 0);
    csrSceneAddModelMatrix(m_pScene, pMesh, &m_Background);

    // load background texture
    CSR_OpenGLHelper::AddTexture(&pMesh->m_Skin,
                                  LoadTexture(m_SceneDir + BACKGROUND_TEXTURE),
                                  m_OpenGLResources);

    // create the mesh to use for particles
    m_pStar = csrShapeCreateDisk(0.0f,
                                 0.0f,
                                 0.01f,
                                 6,
                                &vertexFormat,
                                 0,
                                &material,
                                 0);

    // add the mesh to the scene
    pSceneItem = csrSceneAddMesh(m_pScene, m_pStar, 0, 0);

    m_Aspect = float(w) / float(h);

    // configure the star box
    m_StarBox.m_Min.m_X = -0.5f * 4.0f * m_Aspect;
    m_StarBox.m_Min.m_Y = -0.5f * 4.0f;
    m_StarBox.m_Min.m_Z = -4.0f;
    m_StarBox.m_Max.m_X =  0.5f * 4.0f * m_Aspect;
    m_StarBox.m_Max.m_Y =  0.5f * 4.0f;
    m_StarBox.m_Max.m_Z = -2.0f;

    m_pStars = csrParticlesCreate();
    m_pStars->m_fOnCalculateMotion = OnCalculateStarMotionCallback;

    // iterate through the particles to create
    for (std::size_t i = 0; i < STAR_COUNT; ++i)
    {
        // add a new particle, ignore the mass
        CSR_Particle* pParticle    = csrParticlesAdd(m_pStars);
        pParticle->m_pBody->m_Mass = 0.0f;

        // calculate the particle start position
        const float x = m_StarBox.m_Min.m_X + (((float)(rand() % (int)400.0f)) * 0.01f * m_Aspect);
        const float y = m_StarBox.m_Min.m_Y + (((float)(rand() % (int)400.0f)) * 0.01f);
        const float z = m_StarBox.m_Min.m_Z + (((float)(rand() % (int)200.0f)) * 0.01f);

        // calculate the particle initial force
        pParticle->m_pBody->m_Velocity.m_X =  0.0f;
        pParticle->m_pBody->m_Velocity.m_Y = -0.5f;
        pParticle->m_pBody->m_Velocity.m_Z =  0.0f;

        // configure the particle matrix (was set to identity while the particle was created)
        pParticle->m_pMatrix->m_Table[3][0] = x;
        pParticle->m_pMatrix->m_Table[3][1] = y;
        pParticle->m_pMatrix->m_Table[3][2] = z;

        // add it to the scene
        pSceneItem = csrSceneAddModelMatrix(m_pScene, m_pStar, pParticle->m_pMatrix);

        // keep the particle model key
        pParticle->m_pKey = pSceneItem->m_pModel;
    }

    // load star texture
    CSR_OpenGLHelper::AddTexture(&m_pStar->m_Skin,
                                  LoadTexture(m_SceneDir + STAR_TEXTURE),
                                  m_OpenGLResources);

    // scene should contain meteores?
    #pragma warn -8008
        if (METEORE_COUNT)
        {
            material.m_Transparent = 0;

            // create the meteore
            m_pMeteore = csrMDLOpen((m_SceneDir + METEORE_MODEL).c_str(),
                                    0,
                                   &vertexFormat,
                                    0,
                                   &material,
                                    0,
                                    OnApplySkinCallback,
                                    OnDeleteTextureCallback);

            // add the model to the scene
            pSceneItem = csrSceneAddMDL(m_pScene, m_pMeteore, 0, 0);

            m_pMeteores = csrParticlesCreate();
            m_pMeteores->m_fOnCalculateMotion = OnCalculateMeteoreMotionCallback;

            // iterate through the particles to create
            for (std::size_t i = 0; i < METEORE_COUNT; ++i)
            {
                float rotationAngle;

                // add a new particle, ignore the mass
                CSR_Particle* pParticle    = csrParticlesAdd(m_pMeteores);
                pParticle->m_pBody->m_Mass = i; // NOTE here use the mass as particle index

                // calculate the particle start position
                const float x =  m_StarBox.m_Min.m_X + (((float)(rand() % (int)400.0f)) * 0.01f * m_Aspect);
                const float y =  m_StarBox.m_Min.m_Y + (((float)(rand() % (int)400.0f)) * 0.01f);
                const float z = -0.1f;

                // calculate the particle initial force
                pParticle->m_pBody->m_Velocity.m_X =  0.0f;
                pParticle->m_pBody->m_Velocity.m_Y = -0.5f;
                pParticle->m_pBody->m_Velocity.m_Z =  0.0f;

                // configure the particle matrix (was set to identity while the particle was created)
                pParticle->m_pMatrix->m_Table[3][0] = x;
                pParticle->m_pMatrix->m_Table[3][1] = y;
                pParticle->m_pMatrix->m_Table[3][2] = z;

                // keep the meteore start position
                m_Meteores[i].m_StartPos.m_X = x;
                m_Meteores[i].m_StartPos.m_Y = m_StarBox.m_Min.m_Y;
                m_Meteores[i].m_StartPos.m_Z = z;

                // set meteore rotation and scaling
                rotationAngle                        = ((float)(rand() % (int)628.0f)) * 0.01f;
                m_Meteores[i].m_Rotation.m_X         = sinf(rotationAngle);
                m_Meteores[i].m_Rotation.m_Y         = cosf(rotationAngle);
                m_Meteores[i].m_Rotation.m_Z         = cosf(rotationAngle);
                m_Meteores[i].m_RotationVelocity.m_X = ((float)(rand() % (int)100.0f)) * 0.01f;
                m_Meteores[i].m_RotationVelocity.m_Y = ((float)(rand() % (int)100.0f)) * 0.01f;
                m_Meteores[i].m_RotationVelocity.m_Z = ((float)(rand() % (int)100.0f)) * 0.01f;
                m_Meteores[i].m_Scaling.m_X          = 0.1f;
                m_Meteores[i].m_Scaling.m_Y          = 0.1f;
                m_Meteores[i].m_Scaling.m_Z          = 0.1f;

                // add it to the scene
                pSceneItem = csrSceneAddModelMatrix(m_pScene, m_pMeteore, pParticle->m_pMatrix);

                // keep the particle model key
                pParticle->m_pKey = pSceneItem->m_pModel;
            }

            m_MeteoreBoundingCircle.m_Radius = 0.125f;
        }
    #pragma warn .8008

    // load the sound files
    m_pFireSound      = csrSoundOpenWavFile(m_pOpenALDevice, m_pOpenALContext, (m_SceneDir + FIRE_SOUND).c_str());
    m_pExplosionSound = csrSoundOpenWavFile(m_pOpenALDevice, m_pOpenALContext, (m_SceneDir + EXPLOSION_SOUND).c_str());

    m_Initialized = true;
}
//------------------------------------------------------------------------------
void TMainForm::DeleteScene()
{
    m_Initialized = false;

    // delete the particle systems
    csrParticlesRelease(m_pStars);
    csrParticlesRelease(m_pMeteores);

    // release the scene
    csrSceneRelease(m_pScene, OnDeleteTextureCallback);

    // release the shader
    csrOpenGLShaderRelease(m_pShader);
    csrOpenGLShaderRelease(m_pFlameShader);

    // stop running sounds, if needed
    csrSoundStop(m_pFireSound);
    csrSoundStop(m_pExplosionSound);

    // release OpenAL interface
    csrSoundRelease(m_pFireSound);
    csrSoundRelease(m_pExplosionSound);
}
//------------------------------------------------------------------------------
void TMainForm::UpdateScene(float elapsedTime)
{
    // animate the star particles
    csrParticlesAnimate(m_pStars, elapsedTime);

    // animate the meteore particles
    csrParticlesAnimate(m_pMeteores, elapsedTime);

    // rebuild the spaceship matrix
    BuildSpaceshipMatrix(m_Angle);

    // rebuild the flame matrix
    BuildFlameMatrix(m_Angle);

    // initialize the laser model matrix
    BuildLaserMatrix(m_Angle);

    // calculate next alpha value
    m_Alpha += m_AlphaOffset * elapsedTime;

    // is alpha out of bounds?
    if (m_Alpha < m_MinAlpha)
    {
        m_Alpha       =  m_MinAlpha;
        m_AlphaOffset = -m_AlphaOffset;
    }
    else
    if (m_Alpha > m_MaxAlpha)
    {
        m_Alpha       =  m_MaxAlpha;
        m_AlphaOffset = -m_AlphaOffset;
    }

    if (m_LaserVisible)
    {
        // calculate laser show elapsed time
        m_LaserTime += (elapsedTime * 1000.0f);

        // count frames
        if (m_LaserTime > m_LaserInterval)
        {
            m_LaserTime    = 0.0f;
            m_LaserVisible = false;
        }
    }

    if (m_ShipDying)
    {
        m_ShipAlpha -= elapsedTime * 0.5f;

        if (m_ShipAlpha <= 0.0f)
        {
            m_ShipAlpha = 1.0f;
            m_ShipDying = false;
        }
    }
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
void* TMainForm::OnGetShader(const void* pModel, CSR_EModelType type)
{
    // flame or laser?
    if (pModel == m_pFlame || pModel == m_pLaser)
    {
        csrShaderEnable(m_pFlameShader);

        // set alpha transparency level
        if (pModel == m_pFlame)
            glUniform1f(m_AlphaSlot, m_Alpha * m_ShipAlpha);
        else
            glUniform1f(m_AlphaSlot, m_Alpha + 0.5f);

        return m_pFlameShader;
    }

    csrShaderEnable(m_pShader);

    // spaceship?
    if (pModel == m_Spaceship.m_pKey)
        glUniform1f(m_TexAlphaSlot, m_ShipAlpha);
    else
        glUniform1f(m_TexAlphaSlot, 1.0f);

    return m_pShader;
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
void TMainForm::OnApplySkin(size_t index, const CSR_Skin* pSkin, int* pCanRelease)
{
    // load the texture from the received pixel buffer
    const GLuint textureID = csrOpenGLTextureFromPixelBuffer(pSkin->m_Texture.m_pBuffer);

    // suceeded?
    if (textureID == M_CSR_Error_Code)
        return;

    // add the texture to the OpenGL resources
    CSR_OpenGLHelper::AddTexture(&pSkin->m_Texture, textureID, m_OpenGLResources);

    // from now the texture resource on the model side may be released (will no longer be used)
    if (pCanRelease)
        *pCanRelease = 1;
}
//---------------------------------------------------------------------------
void* TMainForm::OnGetID(const void* pKey)
{
    return CSR_OpenGLHelper::GetTextureID(pKey, m_OpenGLResources);
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
