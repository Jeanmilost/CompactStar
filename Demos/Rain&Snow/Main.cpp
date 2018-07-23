/****************************************************************************
 * ==> Main ----------------------------------------------------------------*
 ****************************************************************************
 * Description : Rain and snow demo                                         *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2018, this file is part of the CompactStar Engine.  *
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
    m_pRain(NULL),
    m_pSnow(NULL),
    m_pRainSound(NULL),
    m_pWindSound(NULL),
    m_pShader(NULL),
    m_pEffect(NULL),
    m_pMSAA(NULL),
    m_FrameCount(0),
    m_PrevOrigin(0),
    m_Angle(0.0f),
    m_PosVelocity(0.0f),
    m_DirVelocity(0.0f),
    m_FPS(0.0),
    m_StartTime(0),
    m_PreviousTime(0),
    m_UseOilPainting(true),
    m_UseMSAA(false),
    m_Initialized(false)
{
    // build the model dir from the application exe
    UnicodeString sceneDir = ::ExtractFilePath(Application->ExeName);
                  sceneDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(sceneDir));
                  sceneDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(sceneDir));
                  sceneDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(sceneDir));
                  sceneDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(sceneDir));
                  sceneDir = ::ExcludeTrailingPathDelimiter(sceneDir) + L"\\Common\\Sounds";
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

    // release the oil painting post processing effect
    if (m_pEffect)
        delete m_pEffect;

    // release the multisampling antialiasing
    csrMSAARelease(m_pMSAA);

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
                                     0.001f,
                                     1000.0f,
                                     m_pShader,
                                     m_pScene->m_ProjectionMatrix);

    // multisampling antialiasing was already created?
    if (!m_pMSAA)
        // create the multisampling antialiasing
        m_pMSAA = csrMSAACreate(ClientWidth, ClientHeight, 4);
    else
        // change his size
        csrMSAAChangeSize(ClientWidth, ClientHeight, m_pMSAA);

    // oil painting post processing effect was already created?
    if (!m_pEffect)
        // create the oil painting post processing effect
        m_pEffect = new CSR_PostProcessingEffect_OilPainting(ClientWidth, ClientHeight, 4);
    else
        // change the effect viewport size
        m_pEffect->ChangeSize(ClientWidth, ClientHeight);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::aeEventsMessage(tagMSG& msg, bool& handled)
{
    m_PosVelocity = 0.0f;
    m_DirVelocity = 0.0f;

    switch (msg.message)
    {
        case WM_KEYDOWN:
        {
            // no scene?
            if (!m_pScene)
                break;

            // search for the pressed key
            switch (msg.wParam)
            {
                case VK_LEFT:
                    m_DirVelocity = -1.0f;
                    handled       =  true;
                    break;

                case VK_RIGHT:
                    m_DirVelocity = 1.0f;
                    handled       = true;
                    break;

                case '1':
                    // create a rain effect
                    CreateWeather(true, false);
                    break;

                case '2':
                    // create a snow effect
                    CreateWeather(false, true);
                    break;

                case '3':
                    // create a combined rain and snow effect
                    CreateWeather(true, true);
                    break;
            }

            return;
        }
    }
}
//------------------------------------------------------------------------------
CSR_Shader* TMainForm::OnGetShaderCallback(const void* pModel, CSR_EModelType type)
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
void TMainForm::OnCalculateMotionCallback(const CSR_Particles* pParticles,
                                                CSR_Particle*  pParticle,
                                                float          elapsedTime)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return;

    if (pMainForm->m_pRain && pParticles == pMainForm->m_pRain->GetKey())
        pMainForm->m_pRain->OnCalculateMotion(pParticles, pParticle, elapsedTime);

    if (pMainForm->m_pSnow && pParticles == pMainForm->m_pSnow->GetKey())
        pMainForm->m_pSnow->OnCalculateMotion(pParticles, pParticle, elapsedTime);
}
//---------------------------------------------------------------------------
void TMainForm::CreateWeather(bool rain, bool snow)
{
    // stop the currently polaying sounds
    csrSoundStop(m_pRainSound);
    csrSoundStop(m_pWindSound);

    // delete the snow
    if (m_pSnow)
    {
        delete m_pSnow;
        m_pSnow = NULL;
    }

    // delete the rain
    if (m_pRain)
    {
        delete m_pRain;
        m_pRain = NULL;
    }

    // previous scene exists?
    if (m_pScene)
        // recreate it from scratch
        csrSceneRelease(m_pScene);

    // create a new scene
    m_pScene = csrSceneCreate();

    // create the scene context
    csrSceneContextInit(&m_SceneContext);
    m_SceneContext.m_fOnSceneBegin = OnSceneBeginCallback;
    m_SceneContext.m_fOnSceneEnd   = OnSceneEndCallback;
    m_SceneContext.m_fOnGetShader  = OnGetShaderCallback;

    // configure the scene color background
    m_pScene->m_Color.m_R = 0.08f;
    m_pScene->m_Color.m_G = 0.12f;
    m_pScene->m_Color.m_B = 0.17f;

    // create the scene viewport
    CSR_OpenGLHelper::CreateViewport(ClientWidth,
                                     ClientHeight,
                                     0.001f,
                                     1000.0f,
                                     m_pShader,
                                     m_pScene->m_ProjectionMatrix);

    // create the box in which the weather effects will be rendered
    CSR_Box weatherBox;
    weatherBox.m_Min.m_X = -3.0f;
    weatherBox.m_Min.m_Y =  3.0f;
    weatherBox.m_Min.m_Z =  2.0f;
    weatherBox.m_Max.m_X =  3.0f;
    weatherBox.m_Max.m_Y = -1.1f;
    weatherBox.m_Max.m_Z = -2.0f;

    // configure the additional wind force for the rain
    CSR_Vector3 rainWindForce;
    rainWindForce.m_X = 0.0f;
    rainWindForce.m_Y = 1.0f;
    rainWindForce.m_Z = 0.0f;

    // configure the additional wind force for the snow
    CSR_Vector3 snowWindForce;
    snowWindForce.m_X = 1.0f;
    snowWindForce.m_Y = 1.0f;
    snowWindForce.m_Z = 0.5f;

    // create the rain effect
    if (rain)
    {
        m_pRain = new CSR_Weather_Rain(m_pScene, weatherBox, 2.0f, rainWindForce, 2000, 0.1f);
        m_pRain->Set_OnCalculateMotion_Callback(OnCalculateMotionCallback);

        // load the sound file
        m_pRainSound = csrSoundOpenWavFile(m_pOpenALDevice, m_pOpenALContext, (m_SceneDir + "\\Rain\\Rain.wav").c_str());

        // set the sound to loop
        csrSoundLoop(m_pRainSound, 1);

        // play the sound
        csrSoundPlay(m_pRainSound);
    }

    // create the snow effect
    if (snow)
    {
        m_pSnow = new CSR_Weather_Snow(m_pScene, weatherBox, 2.0f, snowWindForce, 2000, true);
        m_pSnow->Set_OnCalculateMotion_Callback(OnCalculateMotionCallback);

        // load the sound file
        m_pWindSound = csrSoundOpenWavFile(m_pOpenALDevice, m_pOpenALContext, (m_SceneDir + "\\Wind\\Wind.wav").c_str());

        // set the sound to loop
        csrSoundLoop(m_pWindSound, 1);

        // play the sound
        csrSoundPlay(m_pWindSound);
    }
}
//------------------------------------------------------------------------------
void TMainForm::InitScene(int w, int h)
{
    const std::string vsColored = CSR_ShaderHelper::GetVertexShader(CSR_ShaderHelper::IE_ST_Color);
    const std::string fsColored = CSR_ShaderHelper::GetFragmentShader(CSR_ShaderHelper::IE_ST_Color);

    // load the shader to use to draw the colored cord
    m_pShader = csrShaderLoadFromStr(vsColored.c_str(),
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
    m_pShader->m_VertexSlot = glGetAttribLocation (m_pShader->m_ProgramID, "csr_aVertices");
    m_pShader->m_ColorSlot  = glGetAttribLocation (m_pShader->m_ProgramID, "csr_aColor");

    // by default create a combined rain and snow effect
    CreateWeather(true, true);

    m_Initialized = true;
}
//------------------------------------------------------------------------------
void TMainForm::DeleteScene()
{
    m_Initialized = false;

    // delete the snow
    if (m_pSnow)
        delete m_pSnow;

    // delete the rain
    if (m_pRain)
        delete m_pRain;

    // release the scene
    csrSceneRelease(m_pScene);

    // release the shader
    csrShaderRelease(m_pShader);

    // release the sounds
    csrSoundRelease(m_pRainSound);
    csrSoundRelease(m_pWindSound);
}
//------------------------------------------------------------------------------
void TMainForm::UpdateScene(float elapsedTime)
{
    if (m_pRain)
        m_pRain->Animate(elapsedTime);

    if (m_pSnow)
        m_pSnow->Animate(elapsedTime);

    // is player rotating?
    if (m_DirVelocity)
    {
        // calculate the player direction
        m_Angle += m_DirVelocity * elapsedTime;

        // validate and apply it
        if (m_Angle > M_PI * 2.0f)
            m_Angle -= M_PI * 2.0f;
        else
        if (m_Angle < 0.0f)
            m_Angle += M_PI * 2.0f;
    }

    CSR_ArcBall arcball;
    arcball.m_AngleX = 0.0f;
    arcball.m_AngleY = m_Angle;
    arcball.m_Radius = 0.25f;

    csrSceneArcBallToMatrix(&arcball, &m_pScene->m_ViewMatrix);
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
CSR_Shader* TMainForm::OnGetShader(const void* pModel, CSR_EModelType type)
{
    return m_pShader;
}
//---------------------------------------------------------------------------
void TMainForm::OnSceneBegin(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    if (m_UseOilPainting)
        m_pEffect->DrawBegin(&pScene->m_Color);
    else
    if (m_UseMSAA)
        csrMSAADrawBegin(&pScene->m_Color, m_pMSAA);
    else
        csrDrawBegin(&pScene->m_Color);
}
//---------------------------------------------------------------------------
void TMainForm::OnSceneEnd(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    if (m_UseOilPainting)
        m_pEffect->DrawEnd();
    else
    if (m_UseMSAA)
        csrMSAADrawEnd(m_pMSAA);
    else
        csrDrawEnd();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnIdle(TObject* pSender, bool& done)
{
    done = false;
    OnDrawScene(false);
}
//---------------------------------------------------------------------------
