/****************************************************************************
 * ==> Main ----------------------------------------------------------------*
 ****************************************************************************
 * Description : Ground collision tool main form                            *
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

// interface
#include "TLandscapeSelection.h"

#pragma package(smart_init)
#ifdef __llvm__
    #pragma link "glewSL.a"
#else
    #pragma link "glewSL.lib"
#endif
#pragma link "OpenAL32E.lib"
#pragma resource "*.dfm"

//----------------------------------------------------------------------------
/**
* Vertex shader program to show a textured mesh
*@note The view matrix is implicitly set to identity and combined to the model one
*/
const char g_VertexProgram_TexturedMesh[] =
    "precision mediump float;"
    "attribute vec3 csr_aVertex;"
    "attribute vec4 csr_aColor;"
    "attribute vec2 csr_aTexCoord;"
    "uniform   mat4 csr_uProjection;"
    "uniform   mat4 csr_uView;"
    "uniform   mat4 csr_uModel;"
    "varying   vec4 csr_vColor;"
    "varying   vec2 csr_vTexCoord;"
    "void main(void)"
    "{"
    "    csr_vColor    = csr_aColor;"
    "    csr_vTexCoord = csr_aTexCoord;"
    "    gl_Position   = csr_uProjection * csr_uView * csr_uModel * vec4(csr_aVertex, 1.0);"
    "}";
//----------------------------------------------------------------------------
/**
* Fragment shader program to show a textured mesh
*/
const char g_FragmentProgram_TexturedMesh[] =
    "precision mediump float;"
    "uniform sampler2D csr_sTexture;"
    "varying lowp vec4 csr_vColor;"
    "varying      vec2 csr_vTexCoord;"
    "void main(void)"
    "{"
    "    gl_FragColor = csr_vColor * texture2D(csr_sTexture, csr_vTexCoord);"
    "}";
//---------------------------------------------------------------------------
// TMainForm::IStats
//---------------------------------------------------------------------------
TMainForm::IStats::IStats() :
    m_Altitude(0.0f),
    m_FPS(0),
    m_RefreshCounter(0)
{}
//---------------------------------------------------------------------------
TMainForm::IStats::~IStats()
{}
//---------------------------------------------------------------------------
void TMainForm::IStats::Clear()
{
    m_Altitude = 0.0f;
}
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
    m_pSound(NULL),
    m_pShader(NULL),
    m_pScene(NULL),
    m_pLandscapeKey(NULL),
    m_pEffect(NULL),
    m_pMSAA(NULL),
    m_FrameCount(0),
    m_Angle(0.0f),
    m_PosVelocity(0.0f),
    m_DirVelocity(0.0f),
    m_StepTime(0.0f),
    m_StepInterval(350.0f),
    m_DriftOffsetX(0.0f),
    m_DriftOffsetZ(0.0f),
    m_StartTime(0),
    m_PreviousTime(0),
    m_Initialized(false),
    m_fViewWndProc_Backup(NULL)
{
    // build the model dir from the application exe
    UnicodeString sceneDir = ::ExtractFilePath(Application->ExeName);
                  sceneDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(sceneDir));
                  sceneDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(sceneDir));
                  sceneDir = ::ExcludeTrailingPathDelimiter(sceneDir) + L"\\Scenes";
                m_SceneDir =   AnsiString(sceneDir).c_str();

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
    // restore the normal view procedure
    if (m_fViewWndProc_Backup)
        paView->WindowProc = m_fViewWndProc_Backup;

    DeleteScene();
    DisableOpenGL(paView->Handle, m_hDC, m_hRC);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormCreate(TObject* pSender)
{
    // hook the panel procedure
    m_fViewWndProc_Backup = paView->WindowProc;
    paView->WindowProc    = ViewWndProc;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormShow(TObject* pSender)
{
    // initialize the scene
    InitScene(paView->ClientWidth, paView->ClientHeight);

    // initialize the timer
    m_StartTime    = ::GetTickCount();
    m_PreviousTime = ::GetTickCount();

    // listen the application idle
    Application->OnIdle = OnIdle;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormResize(TObject* pSender)
{
    // update the viewport
    CreateViewport(paView->ClientWidth, paView->ClientHeight);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::spMainViewMoved(TObject* pSender)
{
    // update the viewport
    CreateViewport(paView->ClientWidth, paView->ClientHeight);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::btResetViewportClick(TObject* pSender)
{
    // reset the viewpoint bounding sphere to his default position
    m_ViewSphere.m_Center.m_X = 0.0f;
    m_ViewSphere.m_Center.m_Y = 0.0f;
    m_ViewSphere.m_Center.m_Z = 0.0f;
    m_ViewSphere.m_Radius     = 0.1f;

    // also reset the model bounding sphere to his default position
    m_ModelSphere.m_Center.m_X =  0.0f;
    m_ModelSphere.m_Center.m_Y =  0.0f;
    m_ModelSphere.m_Center.m_Z = -0.5f;
    m_ModelSphere.m_Radius     =  0.05f;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::btLoadModelClick(TObject* pSender)
{
    // create a landscape selection dialog box
    std::auto_ptr<TLandscapeSelection> pLandscapeSelection
            (new TLandscapeSelection(this, UnicodeString(AnsiString(m_SceneDir.c_str())).c_str()));

    // execute the dialog and check if user canceled it
    if (pLandscapeSelection->ShowModal() != mrOk)
        return;

    // do change the landscape model?
    if (pLandscapeSelection->rbSourceBitmap->Checked)
    {
        // load the landscape from a grayscale image
        if (!LoadModelFromBitmap(AnsiString(pLandscapeSelection->edBitmapFileName->Text).c_str()))
        {
            // show the error message to the user
            ::MessageDlg(L"Failed to load the landscape from the grayscale image.",
                         mtError,
                         TMsgDlgButtons() << mbOK,
                         0);
            return;
        }
    }
    else
    if (pLandscapeSelection->rbSourceModel->Checked)
    {
        // load the landscape from a model file
        if (!LoadModel(AnsiString(pLandscapeSelection->edModelFileName->Text).c_str()))
        {
            // show the error message to the user
            ::MessageDlg(L"Failed to load the landscape.",
                         mtError,
                         TMsgDlgButtons() << mbOK,
                         0);
            return;
        }
    }

    // do change the texture?
    if (!pLandscapeSelection->edTextureFileName->Text.IsEmpty())
    {
        // invalid next position, get the scene item (just one for this scene)
        const CSR_SceneItem* pItem = csrSceneGetItem(m_pScene, m_pLandscapeKey);

        // found it?
        if (!pItem)
            return;

        const CSR_Model* pModel = (CSR_Model*)pItem->m_pModel;

        if (!pModel || !pModel->m_MeshCount)
            return;

        // release the previous texture
        glDeleteTextures(1, &pModel->m_pMesh[0].m_Shader.m_TextureID);

        // load the new one
        pModel->m_pMesh[0].m_Shader.m_TextureID =
                LoadTexture(pLandscapeSelection->edTextureFileName->Text.c_str());

        // failed?
        if (pModel->m_pMesh[0].m_Shader.m_TextureID == M_CSR_Error_Code)
        {
            // show the error message to the user
            ::MessageDlg(L"Failed to load the texture.",
                         mtError,
                         TMsgDlgButtons() << mbOK,
                         0);
            return;
        }
    }

    // do change the walk sound?
    if (!pLandscapeSelection->edSoundFileName->Text.IsEmpty())
    {
        // stop the sound and delete it
        csrSoundStop(m_pSound);
        csrSoundRelease(m_pSound);

        // get the new sound file to load
        const std::string soundFile = AnsiString(pLandscapeSelection->edSoundFileName->Text).c_str();

        // load the sound file
        m_pSound = csrSoundOpen(m_pOpenALDevice, m_pOpenALContext, soundFile.c_str(), 44100);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::aeEventsMessage(tagMSG& msg, bool& handled)
{
    m_PosVelocity = 0.0f;
    m_DirVelocity = 0.0f;

    switch (msg.message)
    {
        case WM_KEYDOWN:
            switch (msg.wParam)
            {
                case VK_LEFT:  m_DirVelocity = -1.0f; handled = true; break;
                case VK_RIGHT: m_DirVelocity =  1.0f; handled = true; break;
                case VK_UP:    m_PosVelocity =  1.0f; handled = true; break;
                case VK_DOWN:  m_PosVelocity = -1.0f; handled = true; break;
            }

            return;
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
int TMainForm::OnCustomDetectCollisionCallback(const CSR_Scene*           pScene,
                                               const CSR_SceneItem*       pSceneItem,
                                                     size_t               index,
                                               const CSR_Matrix4*         pInvertedModelMatrix,
                                               const CSR_CollisionInput*  pCollisionInput,
                                                     CSR_CollisionOutput* pCollisionOutput)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return 0;

    return pMainForm->OnCustomDetectCollision(pScene,
                                              pSceneItem,
                                              index,
                                              pInvertedModelMatrix,
                                              pCollisionInput,
                                              pCollisionOutput);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ViewWndProc(TMessage& message)
{
    switch (message.Msg)
    {
        case WM_PAINT:
        {
            // is scene initialized?
            if (!m_Initialized)
                break;

            HDC           hDC = NULL;
            ::PAINTSTRUCT ps;

            try
            {
                // get and lock the view device context
                hDC = ::BeginPaint(paView->Handle, &ps);

                // on success, draw the scene
                if (hDC)
                    OnDrawScene(true);
            }
            __finally
            {
                // unlock and release the device context
                ::EndPaint(paView->Handle, &ps);
            }

            return;
        }
    }

    if (m_fViewWndProc_Backup)
        m_fViewWndProc_Backup(message);
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
    const float zNear  = 0.001f;
    const float zFar   = 1000.0f;
    const float fov    = 45.0f;
    const float aspect = w / h;

    // create the OpenGL viewport
    glViewport(0, 0, w, h);

    // create a perspective projection matrix
    csrMat4Perspective(fov, aspect, zNear, zFar, &m_ProjectionMatrix);

    // enable the shader program
    csrShaderEnable(m_pShader);

    // connect projection matrix to shader
    GLint projectionSlot = glGetUniformLocation(m_pShader->m_ProgramID, "csr_uProjection");
    glUniformMatrix4fv(projectionSlot, 1, 0, &m_ProjectionMatrix.m_Table[0][0]);

    // multisampling antialiasing was already created?
    if (!m_pMSAA)
        // create the multisampling antialiasing
        m_pMSAA = csrMSAACreate(w, h, 4);
    else
        // change his size
        csrMSAAChangeSize(w, h, m_pMSAA);

    // oil painting post processing effect was already created?
    if (!m_pEffect)
        // create the oil painting post processing effect
        m_pEffect = new CSR_PostProcessingEffect_OilPainting(w, h, 4);
    else
        // change the effect viewport size
        m_pEffect->ChangeSize(w, h);
}
//------------------------------------------------------------------------------
void TMainForm::InitScene(int w, int h)
{
    // initialize the scene
    m_pScene = csrSceneCreate();

    // configure the scene background color
    m_pScene->m_Color.m_R = 0.45f;
    m_pScene->m_Color.m_G = 0.8f;
    m_pScene->m_Color.m_B = 1.0f;
    m_pScene->m_Color.m_A = 1.0f;

    // configure the scene ground direction
    m_pScene->m_GroundDir.m_X =  0.0f;
    m_pScene->m_GroundDir.m_Y = -1.0f;
    m_pScene->m_GroundDir.m_Z =  0.0f;

    // configure the scene view matrix
    csrMat4Identity(&m_pScene->m_Matrix);

    // set the viewpoint bounding sphere default position
    m_ViewSphere.m_Center.m_X = 0.0f;
    m_ViewSphere.m_Center.m_Y = 0.0f;
    m_ViewSphere.m_Center.m_Z = 0.0f;
    m_ViewSphere.m_Radius     = 0.1f;

    // set the model bounding sphere default position
    m_ModelSphere.m_Center.m_X =  0.0f;
    m_ModelSphere.m_Center.m_Y =  0.0f;
    m_ModelSphere.m_Center.m_Z = -0.5f;
    m_ModelSphere.m_Radius     =  0.05f;

    // configure the scene context
    csrSceneContextInit(&m_SceneContext);
    m_SceneContext.m_fOnGetShader  = OnGetShaderCallback;
    m_SceneContext.m_fOnSceneBegin = OnSceneBeginCallback;
    m_SceneContext.m_fOnSceneEnd   = OnSceneEndCallback;

    // load the shader
    m_pShader  = csrShaderLoadFromStr(g_VertexProgram_TexturedMesh,
                                      sizeof(g_VertexProgram_TexturedMesh),
                                      g_FragmentProgram_TexturedMesh,
                                      sizeof(g_FragmentProgram_TexturedMesh),
                                      0,
                                      0);

    // succeeded?
    if (!m_pShader)
    {
        // show the error message to the user
        ::MessageDlg(L"Failed to load the shader.\r\n\r\nThe application will quit.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);

        Application->Terminate();
        return;
    }

    // enable the shader program
    csrShaderEnable(m_pShader);

    // get shader attributes
    m_pShader->m_VertexSlot   = glGetAttribLocation (m_pShader->m_ProgramID, "csr_aVertex");
    m_pShader->m_ColorSlot    = glGetAttribLocation (m_pShader->m_ProgramID, "csr_aColor");
    m_pShader->m_TexCoordSlot = glGetAttribLocation (m_pShader->m_ProgramID, "csr_aTexCoord");
    m_pShader->m_TextureSlot  = glGetUniformLocation(m_pShader->m_ProgramID, "csr_sTexture");

    // load the landscape model from a grayscale bitmap file
    if (!LoadModelFromBitmap(m_SceneDir + "\\Bitmaps\\the_face.bmp"))
    {
        // show the error message to the user
        ::MessageDlg(L"An error occurred while the default landscape model was created.\r\n\r\nThe application will quit.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);

        Application->Terminate();
        return;
    }

    // get a default texture file name
    const UnicodeString textureFile =
            UnicodeString(AnsiString(m_SceneDir.c_str())) + L"\\Textures\\snow.jpg";

    // get back the scene item containing the model
    CSR_SceneItem* pItem = csrSceneGetItem(m_pScene, m_pLandscapeKey);

    // found it?
    if (!pItem)
    {
        // show the error message to the user
        ::MessageDlg(L"The landscape was not found in the scene.\r\n\r\nThe application will quit.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);

        Application->Terminate();
        return;
    }

    // load the texture
    ((CSR_Model*)pItem->m_pModel)->m_pMesh[0].m_Shader.m_TextureID = LoadTexture(textureFile.c_str());

    // failed?
    if (((CSR_Model*)pItem->m_pModel)->m_pMesh[0].m_Shader.m_TextureID == M_CSR_Error_Code)
    {
        // show the error message to the user
        ::MessageDlg(L"Unknown texture format.\r\n\r\nThe application will quit.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);

        Application->Terminate();
        return;
    }

    // initialize OpenAL
    csrSoundInitializeOpenAL(&m_pOpenALDevice, &m_pOpenALContext);

    // get the default sound file
    const std::string soundFile = m_SceneDir + "\\Sounds\\human_walk_grass_step.wav";

    // load the sound file
    m_pSound = csrSoundOpen(m_pOpenALDevice, m_pOpenALContext, soundFile.c_str(), 44100);

    m_Initialized = true;
}
//------------------------------------------------------------------------------
void TMainForm::DeleteScene()
{
    // release the scene
    csrSceneRelease(m_pScene);

    // release the shader
    csrShaderRelease(m_pShader);

    // release the oil painting post processing effect
    if (m_pEffect)
        delete m_pEffect;

    // release the multisampling antialiasing
    csrMSAARelease(m_pMSAA);

    // release OpenAL interface
    csrSoundRelease(m_pSound);
    csrSoundReleaseOpenAL(m_pOpenALDevice, m_pOpenALContext);
}
//------------------------------------------------------------------------------
void TMainForm::UpdateScene(float elapsedTime)
{
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

    // keep the old position to revert it in case of error
    const CSR_Vector3 oldPos = m_ViewSphere.m_Center;

    // is player moving?
    if (m_PosVelocity)
    {
        // calculate the next position
        m_ViewSphere.m_Center.m_X += m_PosVelocity * sinf(m_Angle) * elapsedTime;
        m_ViewSphere.m_Center.m_Z -= m_PosVelocity * cosf(m_Angle) * elapsedTime;

        // is the sound enabled?
        if (!ckDisableSound->Checked)
        {
            // calculate next time where the step sound should be played
            m_StepTime += (elapsedTime * 1000.0f);

            // do play the sound?
            if (m_StepTime > m_StepInterval)
            {
                csrSoundStop(m_pSound);
                csrSoundPlay(m_pSound);
                m_StepTime = 0.0f;
            }
        }
    }
    else
    {
        m_ViewSphere.m_Center.m_X += m_DriftOffsetX;
        m_ViewSphere.m_Center.m_Z += m_DriftOffsetZ;
    }

    // enable the shader program
    csrShaderEnable(m_pShader);

    // calculate the ground position and check if next position is valid
    if (!ApplyGroundCollision(&m_ViewSphere, &m_pScene->m_Matrix))
    {
        // invalid next position, get the scene item (just one for this scene)
        const CSR_SceneItem* pItem = csrSceneGetItem(m_pScene, m_pLandscapeKey);

        // found it?
        if (pItem)
        {
            // check if the x position is out of bounds, and correct it if yes
            if (m_ViewSphere.m_Center.m_X <= pItem->m_pAABBTree->m_pBox->m_Min.m_X ||
                m_ViewSphere.m_Center.m_X >= pItem->m_pAABBTree->m_pBox->m_Max.m_X)
                m_ViewSphere.m_Center.m_X = oldPos.m_X;

            // do the same thing with the z position. Doing that separately for each axis will make
            // the point of view to slide against the landscape border (this is possible because the
            // landscape is axis-aligned)
            if (m_ViewSphere.m_Center.m_Z <= pItem->m_pAABBTree->m_pBox->m_Min.m_Z ||
                m_ViewSphere.m_Center.m_Z >= pItem->m_pAABBTree->m_pBox->m_Max.m_Z)
                m_ViewSphere.m_Center.m_Z = oldPos.m_Z;
        }
        else
            // failed to get the scene item, just revert the position
            m_ViewSphere.m_Center = oldPos;

        // recalculate the ground value (this time the collision result isn't tested, because the
        // previous position is always considered as valid)
        ApplyGroundCollision(&m_ViewSphere, &m_pScene->m_Matrix);
    }

    // update the stats
    m_Stats.m_Altitude = -m_pScene->m_Matrix.m_Table[3][1];
}
//------------------------------------------------------------------------------
void TMainForm::DrawScene()
{
    // draw the scene
    csrSceneDraw(m_pScene, &m_SceneContext);
}
//---------------------------------------------------------------------------
bool TMainForm::AddSphere()
{
    // release the previous model, if exists
    csrSceneDeleteFrom(m_pScene, m_pSphereKey);

    // set the model bounding sphere to his default position
    m_ModelSphere.m_Center.m_X =  0.0f;
    m_ModelSphere.m_Center.m_Y =  0.0f;
    m_ModelSphere.m_Center.m_Z = -0.5f;
    m_ModelSphere.m_Radius     =  0.05f;

    CSR_Material material;
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    // load the model
    CSR_Mesh* pMesh = csrShapeCreateSphere(m_ModelSphere.m_Radius, 20, 20, &vf, 0, &material, 0);

    // succeeded?
    if (!pMesh)
        return false;

    // load a texture for the ball
    pMesh->m_Shader.m_TextureID =
            LoadTexture(UnicodeString(AnsiString(m_SceneDir.c_str()) + L"\\Textures\\ball.png").c_str());

    // initialize the sphere model matrix
    csrMat4Translate(&m_ModelSphere.m_Center, &m_SphereMatrix);

    // add the model to the scene
    csrSceneAddMesh(m_pScene, pMesh, 0, 0);
    csrSceneAddModelMatrix(m_pScene, pMesh, &m_SphereMatrix);

    // keep the key
    m_pSphereKey = pMesh;

    return true;
}
//---------------------------------------------------------------------------
bool TMainForm::LoadModel(const std::string& fileName)
{
    // release the previous model, if exists
    csrSceneDeleteFrom(m_pScene, m_pLandscapeKey);

    CSR_Material material;
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    CSR_VertexCulling vc;
    vc.m_Type = CSR_CT_Front;
    vc.m_Face = CSR_CF_CW;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    // load the model
    CSR_Model* pModel = csrWaveFrontOpen(fileName.c_str(), &vf, &vc, &material, 0, 0);

    // succeeded?
    if (!pModel)
        return false;

    // initialize the landscape model matrix
    csrMat4Identity(&m_LandscapeMatrix);

    // add the model to the scene
    csrSceneAddModel(m_pScene, pModel, 0, 1);
    csrSceneAddModelMatrix(m_pScene, pModel, &m_LandscapeMatrix);

    // get back the scene item containing the model
    CSR_SceneItem* pItem = csrSceneGetItem(m_pScene, pModel);

    // found it?
    if (pItem)
        pItem->m_CollisionType = CSR_ECollisionType(CSR_CO_Ground | CSR_CO_Custom);

    // keep the key
    m_pLandscapeKey = pModel;

    // reset the viewpoint bounding sphere to his default position
    m_ViewSphere.m_Center.m_X = 0.0f;
    m_ViewSphere.m_Center.m_Y = 0.0f;
    m_ViewSphere.m_Center.m_Z = 0.0f;
    m_ViewSphere.m_Radius     = 0.1f;

    if (!AddSphere())
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool TMainForm::LoadModelFromBitmap(const std::string& fileName)
{
    // release the previous model, if exists
    csrSceneDeleteFrom(m_pScene, m_pLandscapeKey);

    CSR_Model*       pModel  = NULL;
    CSR_PixelBuffer* pBitmap = NULL;

    try
    {
        CSR_Material material;
        material.m_Color       = 0xFFFFFFFF;
        material.m_Transparent = 0;
        material.m_Wireframe   = 0;

        CSR_VertexCulling vc;
        vc.m_Type = CSR_CT_None;
        vc.m_Face = CSR_CF_CW;

        CSR_VertexFormat vf;
        vf.m_HasNormal         = 0;
        vf.m_HasTexCoords      = 1;
        vf.m_HasPerVertexColor = 1;

        // load a default grayscale bitmap from which a landscape will be generated
        pBitmap = csrPixelBufferFromBitmap(fileName.c_str());

        // create a landscape model from the grayscale bitmap
        pModel              = csrModelCreate();
        pModel->m_pMesh     = csrLandscapeCreate(pBitmap, 3.0f, 0.2f, &vf, &vc, &material, 0);
        pModel->m_MeshCount = 1;
    }
    __finally
    {
        csrPixelBufferRelease(pBitmap);
    }

    // initialize the landscape model matrix
    csrMat4Identity(&m_LandscapeMatrix);

    // add the model to the scene
    csrSceneAddModel(m_pScene, pModel, 0, 1);
    csrSceneAddModelMatrix(m_pScene, pModel, &m_LandscapeMatrix);

    // get back the scene item containing the model
    CSR_SceneItem* pItem = csrSceneGetItem(m_pScene, pModel);

    // found it?
    if (pItem)
        pItem->m_CollisionType = CSR_ECollisionType(CSR_CO_Ground | CSR_CO_Custom);

    // keep the key
    m_pLandscapeKey = pModel;

    // reset the viewpoint bounding sphere to his default position
    m_ViewSphere.m_Center.m_X = 0.0f;
    m_ViewSphere.m_Center.m_Y = 0.0f;
    m_ViewSphere.m_Center.m_Z = 0.0f;
    m_ViewSphere.m_Radius     = 0.1f;

    if (!AddSphere())
        return false;

    return true;
}
//---------------------------------------------------------------------------
GLuint TMainForm::LoadTexture(const std::wstring& fileName)
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
        default:      return M_CSR_Error_Code;
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

        // reserve memory for the pixel array
        pPixelBuffer->m_pData = new unsigned char[pPixelBuffer->m_DataLength];

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
        textureID = csrTextureFromPixelBuffer(pPixelBuffer);
    }
    __finally
    {
        csrPixelBufferRelease(pPixelBuffer);
    }

    return textureID;
}
//---------------------------------------------------------------------------
bool TMainForm::ApplyGroundCollision(const CSR_Sphere* pBoundingSphere, CSR_Matrix4* pMatrix)
{
    if (!m_pScene)
        return false;

    // validate the inputs
    if (!pBoundingSphere || !pMatrix)
        return false;

    CSR_CollisionInput collisionInput;
    csrCollisionInputInit(&collisionInput);
    collisionInput.m_BoundingSphere.m_Radius = pBoundingSphere->m_Radius;

    CSR_Camera camera;

    // calculate the camera position in the 3d world, without the ground value
    camera.m_Position.m_X = -pBoundingSphere->m_Center.m_X;
    camera.m_Position.m_Y =  0.0f;
    camera.m_Position.m_Z = -pBoundingSphere->m_Center.m_Z;
    camera.m_xAngle       =  0.0f;
    camera.m_yAngle       =  m_Angle;
    camera.m_zAngle       =  0.0f;
    camera.m_Factor.m_X   =  1.0f;
    camera.m_Factor.m_Y   =  1.0f;
    camera.m_Factor.m_Z   =  1.0f;
    camera.m_MatCombType  =  IE_CT_Translate_Scale_Rotate;

    // get the view matrix matching with the camera
    csrSceneCameraToMatrix(&camera, pMatrix);

    CSR_Vector3 modelCenter;

    // get the model center
    modelCenter.m_X = 0.0f;
    modelCenter.m_Y = 0.0f;
    modelCenter.m_Z = 0.0f;

    CSR_Matrix4 invertView;
    float       determinant;

    // calculate the current camera position above the landscape
    csrMat4Inverse(pMatrix, &invertView, &determinant);
    csrMat4Transform(&invertView, &modelCenter, &collisionInput.m_BoundingSphere.m_Center);
    collisionInput.m_CheckPos = collisionInput.m_BoundingSphere.m_Center;

    CSR_CollisionOutput collisionOutput;

    // calculate the collisions in the whole scene
    csrSceneDetectCollision(m_pScene, &collisionInput, &collisionOutput, OnCustomDetectCollisionCallback);

    // update the ground position directly inside the matrix (this is where the final value is required)
    pMatrix->m_Table[3][1] = -collisionOutput.m_GroundPos;

    m_DriftOffsetX = 0.0f;
    m_DriftOffsetZ = 0.0f;

    // found a ground collision?
    if (collisionOutput.m_Collision & CSR_CO_Ground)
    {
        // do slip against the slopes?
        if (ckSlipAgainstSlopes->Checked)
        {
            // get the ground plane direction
            CSR_Vector3 planeDir;
            planeDir.m_X = collisionOutput.m_GroundPlane.m_A;
            planeDir.m_Y = collisionOutput.m_GroundPlane.m_B;
            planeDir.m_Z = collisionOutput.m_GroundPlane.m_C;

            CSR_Vector3 xDir;
            xDir.m_X = 1.0f;
            xDir.m_Y = 0.0f;
            xDir.m_Z = 0.0f;

            float dirAngleX;

            // calculate the angle to drift against the x axis
            csrVec3Dot(&xDir, &planeDir, &dirAngleX);

            CSR_Vector3 zDir;
            zDir.m_X = 0.0f;
            zDir.m_Y = 0.0f;
            zDir.m_Z = 1.0f;

            float dirAngleZ;

            // calculate the angle to drift against the z axis
            csrVec3Dot(&zDir, &planeDir, &dirAngleZ);

            CSR_Vector3 up;
            up.m_X = 0.0f;
            up.m_Y = 1.0f;
            up.m_Z = 0.0f;

            float velocityAngle;

            // calculate the drift velocity (the steeper is the slope, the higher is the fall speed)
            csrVec3Dot(&up, &planeDir, &velocityAngle);

            // calculate the drift offsets to apply on the next frame
            m_DriftOffsetX = 0.001f * dirAngleX * velocityAngle;
            m_DriftOffsetZ = 0.001f * dirAngleZ * velocityAngle;
        }

        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void TMainForm::ShowStats() const
{
    unsigned    vertexCount = 0;
    unsigned    stride;
    std::size_t polyCount;

    // invalid next position, get the scene item (just one for this scene)
    const CSR_SceneItem* pItem = csrSceneGetItem(m_pScene, m_pLandscapeKey);

    // found it?
    if (!pItem)
        return;

    const CSR_Model* pModel = (CSR_Model*)pItem->m_pModel;

    if (pModel && pModel->m_MeshCount)
    {
        // get the mesh stride
        stride = pModel->m_pMesh[0].m_pVB ? pModel->m_pMesh[0].m_pVB->m_Format.m_Stride : 0;

        // count all vertices contained in the mesh
        for (std::size_t i = 0; i < pModel->m_pMesh[0].m_Count; ++i)
            vertexCount += pModel->m_pMesh[0].m_pVB[i].m_Count;

        // calculate the polygons count
        if (!pModel->m_pMesh[0].m_pVB)
            polyCount = 0;
        else
            switch (pModel->m_pMesh[0].m_pVB->m_Format.m_Type)
            {
                case CSR_VT_Triangles:
                    polyCount = stride ? ((vertexCount / stride) / 3) : 0;
                    break;

                case CSR_VT_TriangleStrip:
                case CSR_VT_TriangleFan:
                    polyCount = stride ? ((vertexCount / stride) - (2 * pModel->m_pMesh[0].m_Count)) : 0;
                    break;

                default:
                    polyCount = 0;
                    break;
            }
    }

    // show the stats
    laPolygonCount->Caption = L"Polygons Count: " + ::IntToStr(int(polyCount));
    laX->Caption            = L"X: "              + ::FloatToStr(m_ViewSphere.m_Center.m_X);
    laZ->Caption            = L"Z: "              + ::FloatToStr(m_ViewSphere.m_Center.m_Z);
    laAltitude->Caption     = L"Altitude: "       + ::FloatToStr(m_Stats.m_Altitude);
    laFPS->Caption          = L"FPS:"             + ::IntToStr(int(m_Stats.m_FPS));
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

    // clear the tree stats
    m_Stats.Clear();

    ++m_FrameCount;

    // calculate the FPS
    if (m_FrameCount >= 100)
    {
        const double      smoothing = 0.1;
        const std::size_t fpsTime   = now > m_StartTime ? now - m_StartTime : 1;
        const std::size_t newFPS    = (m_FrameCount * 100) / fpsTime;
        m_StartTime                 = ::GetTickCount();
        m_FrameCount                = 0;
        m_Stats.m_FPS               = (newFPS * smoothing) + (m_Stats.m_FPS * (1.0 - smoothing));
    }

    // update and draw the scene
    UpdateScene(elapsedTime);
    DrawScene();

    ::SwapBuffers(m_hDC);

    // limit the stats refreshing (otherwise the demo may run slower)
    if (m_Stats.m_RefreshCounter > 1000)
    {
        // show the stats
        ShowStats();
        m_Stats.m_RefreshCounter = 0;
    }

    ++m_Stats.m_RefreshCounter;
}
//---------------------------------------------------------------------------
CSR_Shader* TMainForm::OnGetShader(const void* pModel, CSR_EModelType type)
{
    return m_pShader;
}
//---------------------------------------------------------------------------
void TMainForm::OnSceneBegin(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    if (ckOilPainting->Checked)
        m_pEffect->DrawBegin(&pScene->m_Color);
    else
    if (ckAntialiasing->Checked)
        csrMSAADrawBegin(&pScene->m_Color, m_pMSAA);
    else
        csrDrawBegin(&pScene->m_Color);
}
//---------------------------------------------------------------------------
void TMainForm::OnSceneEnd(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    if (ckOilPainting->Checked)
        m_pEffect->DrawEnd();
    else
    if (ckAntialiasing->Checked)
        csrMSAADrawEnd(m_pMSAA);
    else
        csrDrawEnd();
}
//---------------------------------------------------------------------------
int TMainForm::OnCustomDetectCollision(const CSR_Scene*           pScene,
                                       const CSR_SceneItem*       pSceneItem,
                                             size_t               index,
                                       const CSR_Matrix4*         pInvertedModelMatrix,
                                       const CSR_CollisionInput*  pCollisionInput,
                                             CSR_CollisionOutput* pCollisionOutput)
{
    // found the correct scene item to check?
    if (pSceneItem->m_pModel != m_pLandscapeKey)
        return 0;

    // do show the ball?
    if (!ckShowBall->Checked)
    {
        CSR_Vector3 defPos;
        defPos.m_X =  0.0f;
        defPos.m_Y = -99999.0f;
        defPos.m_Z =  0.0f;

        // modify the model matrix
        csrMat4Translate(&defPos, &m_SphereMatrix);
        return 0;
    }

    CSR_Vector3 nextPos;

    // calculate the next sphere position
    nextPos.m_X = m_ViewSphere.m_Center.m_X - (0.5f * sinf((M_PI * 2.0f) - m_Angle));
    nextPos.m_Y = 0.0f;
    nextPos.m_Z = m_ViewSphere.m_Center.m_Z - (0.5f * cosf((M_PI * 2.0f) - m_Angle));

    CSR_Sphere nextPosSphere;
    nextPosSphere.m_Radius = m_ModelSphere.m_Radius;

    // put the pos to check into the model coordinate system (at the location where the collision
    // should be checked)
    csrMat4Transform(pInvertedModelMatrix, &nextPos, &nextPosSphere.m_Center);

    float posY;

    // calculate the y position where to place the point of view
    if (csrGroundPosY(&nextPosSphere,
                      &pSceneItem->m_pAABBTree[pSceneItem->m_AABBTreeIndex],
                      &pScene->m_GroundDir,
                       0,
                      &posY))
    {
        // found a valid position, update it
        m_ModelSphere.m_Center     = nextPosSphere.m_Center;
        m_ModelSphere.m_Center.m_Y = posY;
    }
    else
        m_ModelSphere.m_Center.m_Y = -99999.0f;

    // modify the model matrix
    csrMat4Translate(&m_ModelSphere.m_Center, &m_SphereMatrix);

    return 0;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnIdle(TObject* pSender, bool& done)
{
    done = false;
    OnDrawScene(false);
}
//---------------------------------------------------------------------------
