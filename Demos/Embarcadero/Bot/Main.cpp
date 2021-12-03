/****************************************************************************
 * ==> Bot demo ------------------------------------------------------------*
 ****************************************************************************
 * Description : Simple bot demo (with 4 tasks: wait, search, attack, die)  *
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
#include <math.h>

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Geometry.h"
#include "CSR_Model.h"
#include "CSR_Mdl.h"
#include "CSR_Texture.h"

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

// resource files to load
#define LANDSCAPE_TEXTURE_FILE     "\\grass.png"
#define LANDSCAPE_DATA_FILE        "\\bot_level.bmp"
#define FADER_TEXTURE_FILE         "\\blank.bmp"
#define MDL_FILE                   "\\wizard.mdl"
#define SKYBOX_LEFT                "\\left.png"
#define SKYBOX_TOP                 "\\top.png"
#define SKYBOX_RIGHT               "\\right.png"
#define SKYBOX_BOTTOM              "\\bottom.png"
#define SKYBOX_FRONT               "\\front.png"
#define SKYBOX_BACK                "\\back.png"
#define FOOT_STEP_LEFT_SOUND_FILE  "\\footstep_left.wav"
#define FOOT_STEP_RIGHT_SOUND_FILE "\\footstep_right.wav"
#define HIT_SOUND_FILE             "\\hit.wav"

//----------------------------------------------------------------------------
// Global constants
//----------------------------------------------------------------------------
const char g_VSTextured[] =
    "precision mediump float;"
    "attribute vec3  csr_aVertices;"
    "attribute vec4  csr_aColor;"
    "attribute vec2  csr_aTexCoord;"
    "uniform   float csr_uAlpha;"
    "uniform   int   csr_uGrayscale;"
    "uniform   mat4  csr_uProjection;"
    "uniform   mat4  csr_uView;"
    "uniform   mat4  csr_uModel;"
    "varying   vec4  csr_vColor;"
    "varying   vec2  csr_vTexCoord;"
    "varying   float csr_fGrayscale;"
    "varying   float csr_fAlpha;"
    "void main(void)"
    "{"
    "    csr_vColor    = csr_aColor;"
    "    csr_vTexCoord = csr_aTexCoord;"
    "    gl_Position   = csr_uProjection * csr_uView * csr_uModel * vec4(csr_aVertices, 1.0);"
    ""
    "    if (csr_uGrayscale == 1)"
    "        csr_fGrayscale = 1.0;"
    "    else"
    "        csr_fGrayscale = 0.0;"
    ""
    "    csr_fAlpha = csr_uAlpha;"
    "}";
//----------------------------------------------------------------------------
const char g_FSTextured[] =
    "precision mediump float;"
    "uniform sampler2D  csr_sColorMap;"
    "varying lowp vec4  csr_vColor;"
    "varying      vec2  csr_vTexCoord;"
    "varying      float csr_fGrayscale;"
    "varying      float csr_fAlpha;"
    "void main(void)"
    "{"
    "    float csr_fFadeFactor = 1.0;"
    "    vec4 color            = csr_vColor * texture2D(csr_sColorMap, csr_vTexCoord);"
    ""
    "    if (csr_fGrayscale > 0.5)"
    "    {"
    "        float grayscaleVal = (color.x * 0.3 + color.y * 0.59 + color.z * 0.11);"
    "        gl_FragColor       = vec4(grayscaleVal, grayscaleVal, grayscaleVal, csr_fAlpha);"
    "    }"
    "    else"
    "        gl_FragColor = vec4(color.x * csr_fFadeFactor, color.y * csr_fFadeFactor, color.z * csr_fFadeFactor, csr_fAlpha);"
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
    m_pTaskManager(NULL),
    m_pShader(NULL),
    m_pSkyboxShader(NULL),
    m_pFader(NULL),
    m_pLandscapeKey(NULL),
    m_MapHeight(3.0f),
    m_MapScale(0.2f),
    m_Angle(0.0f),
    m_RotationSpeed(0.02f),
    m_StepTime(0.0f),
    m_StepInterval(300.0f),
    m_PosVelocity(0.0f),
    m_DirVelocity(0.0f),
    m_ControlRadius(40.0f),
    m_BotAlpha(1.0f),
    m_FaderAlpha(0.0f),
    m_Time(0.0f),
    m_Interval(0.0f),
    m_TextureLastTime(0.0),
    m_ModelLastTime(0.0),
    m_MeshLastTime(0.0),
    m_WatchingTime(0.0),
    m_ElapsedTime(0.0),
    m_AnimIndex(0),
    m_TextureIndex(0),
    m_ModelIndex(0),
    m_MeshIndex(0),
    m_LastKnownIndex(0),
    m_FrameCount(0),
    m_AlternateStep(0),
    m_BotShowPlayer(0),
    m_BotHitPlayer(0),
    m_BotDying(0),
    m_PlayerDying(0),
    m_GrayscaleSlot(0),
    m_AlphaSlot(0),
    m_pFootStepLeftSound (NULL),
    m_pFootStepRightSound(NULL),
    m_pHitSound(NULL),
    m_FPS(0.0),
    m_StartTime(0L),
    m_PreviousTime(0L),
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

    // delete the scene textures
    CSR_OpenGLHelper::ClearResources(m_OpenGLResources);

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
void TMainForm::OnTaskChangeCallback(CSR_Task* pTask, double elapsedTime)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return;

    pMainForm->OnTaskChange(pTask, elapsedTime);
}
//------------------------------------------------------------------------------
int TMainForm::OnTaskRunCallback(CSR_Task* pTask, double elapsedTime)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return 0;

    return pMainForm->OnTaskRun(pTask, elapsedTime);
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
void TMainForm::OnApplySkinCallback(std::size_t index, const CSR_Skin* pSkin, int* pCanRelease)
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
void TMainForm::OnGetMDLIndexCallback(const CSR_MDL*     pMDL,
                                            std::size_t* pSkinIndex,
                                            std::size_t* pModelIndex,
                                            std::size_t* pMeshIndex)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return;

    pMainForm->OnGetMDLIndex(pMDL, pSkinIndex, pModelIndex, pMeshIndex);
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
GLuint TMainForm::LoadCubemap(const IFileNames fileNames) const
{
    try
    {
        GLuint textureID = M_CSR_Error_Code;

        // create new OpenGL texture
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        const std::size_t fileNameCount = fileNames.size();

        // iterate through the cubemap texture files to load
        for (std::size_t i = 0; i < fileNameCount; i++)
        {
            // load texture in a picture
            std::auto_ptr<TPicture> pPicture(new TPicture());
            pPicture->LoadFromFile(fileNames[i].c_str());

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
                        // copy to pixel array
                        if (pixelSize == 3)
                        {
                            const std::size_t offset = yPos + (((pTexture->Width - 1) - x) * 3);

                            ((unsigned char*)pPixelBuffer->m_pData)[offset]     = pLineRGB[x].rgbtRed;
                            ((unsigned char*)pPixelBuffer->m_pData)[offset + 1] = pLineRGB[x].rgbtGreen;
                            ((unsigned char*)pPixelBuffer->m_pData)[offset + 2] = pLineRGB[x].rgbtBlue;
                        }
                        else
                        {
                            const std::size_t offset = yPos + (((pTexture->Width - 1) - x) * 4);

                            ((unsigned char*)pPixelBuffer->m_pData)[offset]     = pLineRGBA[x].rgbRed;
                            ((unsigned char*)pPixelBuffer->m_pData)[offset + 1] = pLineRGBA[x].rgbGreen;
                            ((unsigned char*)pPixelBuffer->m_pData)[offset + 2] = pLineRGBA[x].rgbBlue;
                            ((unsigned char*)pPixelBuffer->m_pData)[offset + 3] = pLineRGBA[x].rgbReserved;
                        }
                    }
                }

                // load the texture on the GPU
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0,
                             GL_RGB,
                             pPixelBuffer->m_Width,
                             pPixelBuffer->m_Height,
                             0,
                             GL_RGB,
                             GL_UNSIGNED_BYTE,
                             pPixelBuffer->m_pData);
            }
            __finally
            {
                csrPixelBufferRelease(pPixelBuffer);
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,     GL_CLAMP_TO_EDGE);

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
    CSR_VertexFormat vertexFormat;
    CSR_Material     material;
    CSR_Mesh*        pMesh;
    CSR_Model*       pModel;

    std::srand(0);

    // initialize the terrain limits
    m_TerrainLimits.m_Min.m_X = -3.0f;
    m_TerrainLimits.m_Min.m_Y = -3.0f;
    m_TerrainLimits.m_Max.m_X =  3.0f;
    m_TerrainLimits.m_Max.m_Y =  3.0f;

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
    csrMat4Identity(&m_pScene->m_ViewMatrix);

    // set the viewpoint bounding sphere default position
    m_BoundingSphere.m_Center.m_X = 0.0f;
    m_BoundingSphere.m_Center.m_Y = 0.0f;
    m_BoundingSphere.m_Center.m_Z = 0.0f;
    m_BoundingSphere.m_Radius     = 0.1f;

    // configure the scene context
    csrSceneContextInit(&m_SceneContext);
    m_SceneContext.m_fOnGetShader     = OnGetShaderCallback;
    m_SceneContext.m_fOnGetID         = OnGetIDCallback;
    m_SceneContext.m_fOnGetMDLIndex   = OnGetMDLIndexCallback;
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
    {
        Close();
        return;
    }

    csrShaderEnable(m_pShader);

    // get shader attributes
    m_pShader->m_VertexSlot   = glGetAttribLocation (m_pShader->m_ProgramID, "csr_aVertices");
    m_pShader->m_ColorSlot    = glGetAttribLocation (m_pShader->m_ProgramID, "csr_aColor");
    m_pShader->m_TexCoordSlot = glGetAttribLocation (m_pShader->m_ProgramID, "csr_aTexCoord");
    m_pShader->m_TextureSlot  = glGetAttribLocation (m_pShader->m_ProgramID, "csr_sColorMap");
    m_GrayscaleSlot           = glGetUniformLocation(m_pShader->m_ProgramID, "csr_uGrayscale");
    m_AlphaSlot               = glGetUniformLocation(m_pShader->m_ProgramID, "csr_uAlpha");

    // configure OpenGL depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRangef(0.0f, 1.0f);

    // load the landscape
    if (!LoadLandscapeFromBitmap((m_SceneDir + LANDSCAPE_DATA_FILE).c_str()))
    {
        Close();
        return;
    }

    // get back the scene item containing the model
    CSR_SceneItem* pItem = csrSceneGetItem(m_pScene, m_pLandscapeKey);

    // found it?
    if (!pItem)
    {
        Close();
        return;
    }

    // load landscape texture
    CSR_OpenGLHelper::AddTexture(&((CSR_Model*)(pItem->m_pModel))->m_pMesh[0].m_Skin.m_Texture,
                                  LoadTexture(m_SceneDir + LANDSCAPE_TEXTURE_FILE),
                                  m_OpenGLResources);

    m_Bot.m_Angle         = 0.0f;
    m_Bot.m_Velocity      = 0.5f;
    m_Bot.m_DyingSequence = E_DS_None;

    BuildBotMatrix();

    // configure the vertex format
    vertexFormat.m_HasNormal         = 0;
    vertexFormat.m_HasTexCoords      = 1;
    vertexFormat.m_HasPerVertexColor = 1;

    // configure the material
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 1;
    material.m_Wireframe   = 0;

    // load the MDL model
    m_Bot.m_pModel = csrMDLOpen((m_SceneDir + MDL_FILE).c_str(),
                                0,
                                &vertexFormat,
                                0,
                                &material,
                                0,
                                OnApplySkinCallback,
                                OnDeleteTextureCallback);

    // add the model to the scene
    CSR_SceneItem* pSceneItem = csrSceneAddMDL(m_pScene, m_Bot.m_pModel, 1, 0);
    csrSceneAddModelMatrix(m_pScene, m_Bot.m_pModel, &m_Bot.m_Matrix);

    // succeeded?
    if (pSceneItem)
        pSceneItem->m_CollisionType = CSR_CO_Ground;

    // configure bot geometry
    m_Bot.m_Geometry.m_Center.m_X = 3.0f;
    m_Bot.m_Geometry.m_Center.m_Y = 0.0f;
    m_Bot.m_Geometry.m_Center.m_Z = 0.0f;
    m_Bot.m_Geometry.m_Radius     = 0.125f;

    // configure the vertex format
    vertexFormat.m_HasNormal         = 0;
    vertexFormat.m_HasTexCoords      = 1;
    vertexFormat.m_HasPerVertexColor = 1;

    // configure the material
    material.m_Color       = 0xFF0000FF;
    material.m_Transparent = 1;
    material.m_Wireframe   = 0;

    // create a surface for the fader
    m_pFader = csrShapeCreateSurface(100.0f, 100.0f, &vertexFormat, 0, &material, 0);

    // add the mesh to the scene
    pSceneItem = csrSceneAddMesh(m_pScene, m_pFader, 1, 0);
    csrSceneAddModelMatrix(m_pScene, m_pFader, &m_FaderMatrix);

    // initialize the fader matrix
    BuildFaderMatrix();

    const std::string vsSkybox = CSR_ShaderHelper::GetVertexShader(CSR_ShaderHelper::IE_ST_Skybox);
    const std::string fsSkybox = CSR_ShaderHelper::GetFragmentShader(CSR_ShaderHelper::IE_ST_Skybox);

    // load the skybox shader
    m_pSkyboxShader = csrOpenGLShaderLoadFromStr(vsSkybox.c_str(),
                                                 vsSkybox.length(),
                                                 fsSkybox.c_str(),
                                                 fsSkybox.length(),
                                                 0,
                                                 0);

    // succeeded?
    if (!m_pSkyboxShader)
    {
        Close();
        return;
    }

    // enable the shader program
    csrShaderEnable(m_pSkyboxShader);

    // get shader attributes
    m_pSkyboxShader->m_VertexSlot  = glGetAttribLocation (m_pSkyboxShader->m_ProgramID, "csr_aVertices");
    m_pSkyboxShader->m_CubemapSlot = glGetUniformLocation(m_pSkyboxShader->m_ProgramID, "csr_sCubemap");

    // create the skybox
    m_pScene->m_pSkybox = csrSkyboxCreate(1.0f, 1.0f, 1.0f);

    // succeeded?
    if (!m_pScene->m_pSkybox)
    {
        Close();
        return;
    }

    IFileNames cubemapFileNames;
    cubemapFileNames.push_back(m_SceneDir + SKYBOX_RIGHT);
    cubemapFileNames.push_back(m_SceneDir + SKYBOX_LEFT);
    cubemapFileNames.push_back(m_SceneDir + SKYBOX_TOP);
    cubemapFileNames.push_back(m_SceneDir + SKYBOX_BOTTOM);
    cubemapFileNames.push_back(m_SceneDir + SKYBOX_FRONT);
    cubemapFileNames.push_back(m_SceneDir + SKYBOX_BACK);

    // load the cubemap texture
    CSR_OpenGLHelper::AddTexture(&m_pScene->m_pSkybox->m_Skin.m_CubeMap,
                                  LoadCubemap(cubemapFileNames),
                                  m_OpenGLResources);

    // load the sound files
    m_pFootStepLeftSound  = csrSoundOpenWavFile(m_pOpenALDevice, m_pOpenALContext, (m_SceneDir + FOOT_STEP_LEFT_SOUND_FILE).c_str());
    m_pFootStepRightSound = csrSoundOpenWavFile(m_pOpenALDevice, m_pOpenALContext, (m_SceneDir + FOOT_STEP_RIGHT_SOUND_FILE).c_str());
    m_pHitSound           = csrSoundOpenWavFile(m_pOpenALDevice, m_pOpenALContext, (m_SceneDir + HIT_SOUND_FILE).c_str());

    // change the volume
    csrSoundChangeVolume(m_pFootStepLeftSound,  0.2f);
    csrSoundChangeVolume(m_pFootStepRightSound, 0.2f);

    // initialize the IA task manager
    m_pTaskManager = csrTaskManagerCreate();
    csrTaskManagerInit(m_pTaskManager);
    m_pTaskManager->m_pTask = csrTaskCreate();
    m_pTaskManager->m_Count = 1;

    // initialize the bot task
    csrTaskInit(m_pTaskManager->m_pTask);
    m_pTaskManager->m_pTask->m_Action = E_BT_Watching;

    // initialize the IA task manager context
    m_TaskContext.m_fOnTaskChange = OnTaskChangeCallback;
    m_TaskContext.m_fOnTaskRun    = OnTaskRunCallback;

    // prepare the first bot task
    OnPrepareWatching();

    m_Initialized = true;
}
//------------------------------------------------------------------------------
void TMainForm::DeleteScene()
{
    m_Initialized = false;

    // delete the task manager
    csrTaskManagerRelease(m_pTaskManager);

    // delete the scene
    csrSceneRelease(m_pScene, OnDeleteTextureCallback);

    // delete scene shader
    csrOpenGLShaderRelease(m_pShader);

    // delete skybox shader
    csrOpenGLShaderRelease(m_pSkyboxShader);

    // stop running step sound, if needed
    csrSoundStop(m_pFootStepLeftSound);
    csrSoundStop(m_pFootStepRightSound);
    csrSoundStop(m_pHitSound);

    // release OpenAL interface
    csrSoundRelease(m_pFootStepLeftSound);
    csrSoundRelease(m_pFootStepRightSound);
    csrSoundRelease(m_pHitSound);
}
//------------------------------------------------------------------------------
void TMainForm::UpdateScene(float elapsedTime)
{
    float          posY;
    float          angle = 0.0f;
    CSR_Sphere     prevSphere;
    CSR_SceneItem* pSceneItem;

    // check if the bot hit the player
    m_BotHitPlayer = CheckBotHitPlayer();

    // run the IA
    csrTaskManagerExecute(m_pTaskManager, &m_TaskContext, elapsedTime);

    // rebuild the bot matrix
    BuildBotMatrix();

    pSceneItem = csrSceneGetItem(m_pScene, m_pLandscapeKey);
    posY       = M_CSR_NoGround;

    // calculate the y position where to place the bot
    csrGroundPosY(&m_Bot.m_Geometry, pSceneItem->m_pAABBTree, &m_pScene->m_GroundDir, 0, &posY);

    // set the bot y position directly inside the matrix
    m_Bot.m_Matrix.m_Table[3][0] = m_Bot.m_Geometry.m_Center.m_X;
    m_Bot.m_Matrix.m_Table[3][1] = posY;
    m_Bot.m_Matrix.m_Table[3][2] = m_Bot.m_Geometry.m_Center.m_Z;

    // calculate next model indexes to show
    if (m_Bot.m_DyingSequence != E_DS_FadeOut)
        csrMDLUpdateIndex(m_Bot.m_pModel,
                          m_FPS,
                          m_AnimIndex,
                         &m_TextureIndex,
                         &m_ModelIndex,
                         &m_MeshIndex,
                         &m_TextureLastTime,
                         &m_ModelLastTime,
                         &m_MeshLastTime,
                          elapsedTime * 0.1f);

    // is player dying?
    if (m_PlayerDying)
    {
        // reset the bot data
        m_pTaskManager->m_pTask->m_Action = E_BT_Watching;
        m_BotShowPlayer                   = 0;
        glUniform1i(m_GrayscaleSlot, 0);

        // fade to red
        m_FaderAlpha += 1.0f * elapsedTime;

        // fader max value reached?
        if (m_FaderAlpha >= 1.0f)
        {
            m_FaderAlpha = 1.0f;

            // get a new player position
            m_BoundingSphere.m_Center.m_X = -3.0f + ((rand() % 600) / 100.0f);
            m_BoundingSphere.m_Center.m_Z = -3.0f + ((rand() % 600) / 100.0f);

            m_PlayerDying = 0;
        }

        return;
    }

    // fade out
    if (m_FaderAlpha > 0.0f)
        m_FaderAlpha -= 1.0f * elapsedTime;
    else
        m_FaderAlpha = 0.0f;

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

    prevSphere = m_BoundingSphere;

    // is player moving?
    if (m_PosVelocity)
    {
        // calculate the next player position
        m_BoundingSphere.m_Center.m_X += m_PosVelocity * cosf(m_Angle + (M_PI * 0.5f)) * elapsedTime;
        m_BoundingSphere.m_Center.m_Z += m_PosVelocity * sinf(m_Angle + (M_PI * 0.5f)) * elapsedTime;

        // calculate the ground position and check if next position is valid
        if (!ApplyGroundCollision(&m_BoundingSphere, &m_pScene->m_ViewMatrix))
        {
            // invalid next position, get the scene item (just one for this scene)
            const CSR_SceneItem* pItem = csrSceneGetItem(m_pScene, m_pLandscapeKey);

            // found it?
            if (pItem)
            {
                // check if the x position is out of bounds, and correct it if yes
                if (m_BoundingSphere.m_Center.m_X <= pItem->m_pAABBTree->m_pBox->m_Min.m_X ||
                    m_BoundingSphere.m_Center.m_X >= pItem->m_pAABBTree->m_pBox->m_Max.m_X)
                    m_BoundingSphere.m_Center.m_X  = prevSphere.m_Center.m_X;

                // do the same thing with the z position. Doing that separately for each axis will make
                // the point of view to slide against the landscape border (this is possible because the
                // landscape is axis-aligned)
                if (m_BoundingSphere.m_Center.m_Z <= pItem->m_pAABBTree->m_pBox->m_Min.m_Z ||
                    m_BoundingSphere.m_Center.m_Z >= pItem->m_pAABBTree->m_pBox->m_Max.m_Z)
                    m_BoundingSphere.m_Center.m_Z  = prevSphere.m_Center.m_Z;
            }
            else
                // failed to get the scene item, just revert the position
                m_BoundingSphere.m_Center = prevSphere.m_Center;

            // recalculate the ground value (this time the collision result isn't tested, because the
            // previous position is always considered as valid)
            ApplyGroundCollision(&m_BoundingSphere, &m_pScene->m_ViewMatrix);
        }

        BuildFaderMatrix();

        // calculate next time where the step sound should be played
        m_StepTime += (elapsedTime * 1000.0f);

        // count frames
        while (m_StepTime > m_StepInterval)
        {
            // do play the left or right footstep sound?
            if (!(m_AlternateStep % 2))
                csrSoundPlay(m_pFootStepLeftSound);
            else
                csrSoundPlay(m_pFootStepRightSound);

            m_StepTime = 0.0f;

            // next time the other footstep sound will be played
            m_AlternateStep = (m_AlternateStep + 1) & 1;
        }
    }

    // rebuild the fader matrix
    BuildFaderMatrix();
}
//------------------------------------------------------------------------------
void TMainForm::DrawScene()
{
    // finalize the view matrix
    ApplyGroundCollision(&m_BoundingSphere, &m_pScene->m_ViewMatrix);

    // draw the scene
    csrSceneDraw(m_pScene, &m_SceneContext);
}
//---------------------------------------------------------------------------
int TMainForm::LoadLandscapeFromBitmap(const char* fileName)
{
    CSR_Material      material;
    CSR_VertexCulling vc;
    CSR_VertexFormat  vf;
    CSR_Model*        pModel;
    CSR_PixelBuffer*  pBitmap;
    CSR_SceneItem*    pSceneItem;

    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    vc.m_Type = CSR_CT_None;
    vc.m_Face = CSR_CF_CW;

    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    // create a model to contain the landscape
    pModel = csrModelCreate();

    // succeeded?
    if (!pModel)
        return 0;

    // load a default grayscale bitmap from which a landscape will be generated
    pBitmap = csrPixelBufferFromBitmapFile(fileName);

    // succeeded?
    if (!pBitmap)
    {
        csrModelRelease(pModel, OnDeleteTextureCallback);
        return 0;
    }

    // load the landscape mesh from the grayscale bitmap
    pModel->m_pMesh     = csrLandscapeCreate(pBitmap, 3.0f, 0.2f, &vf, &vc, &material, 0);
    pModel->m_MeshCount = 1;

    csrPixelBufferRelease(pBitmap);

    csrMat4Identity(&m_LandscapeMatrix);

    // add the model to the scene
    pSceneItem = csrSceneAddModel(m_pScene, pModel, 0, 1);
    csrSceneAddModelMatrix(m_pScene, pModel, &m_LandscapeMatrix);

    // succeeded?
    if (pSceneItem)
        pSceneItem->m_CollisionType = CSR_CO_Ground;

    // keep the key
    m_pLandscapeKey = pModel;

    return 1;
}
//---------------------------------------------------------------------------
void TMainForm::BuildBotMatrix()
{
    CSR_Vector3 axis;
    CSR_Vector3 factor;
    CSR_Matrix4 rotateXMatrix;
    CSR_Matrix4 rotateYMatrix;
    CSR_Matrix4 scaleMatrix;
    CSR_Matrix4 intermediateMatrix;

    csrMat4Identity(&m_Bot.m_Matrix);

    // set rotation axis
    axis.m_X = 1.0f;
    axis.m_Y = 0.0f;
    axis.m_Z = 0.0f;

    // create the rotation matrix
    csrMat4Rotate(-M_PI / 2.0f, &axis, &rotateXMatrix);

    // set rotation axis
    axis.m_X = 0.0f;
    axis.m_Y = 1.0f;
    axis.m_Z = 0.0f;

    // create the rotation matrix
    csrMat4Rotate(m_Bot.m_Angle, &axis, &rotateYMatrix);

    // set scale factor
    factor.m_X = 0.005f;
    factor.m_Y = 0.005f;
    factor.m_Z = 0.005f;

    // create the scale matrix
    csrMat4Scale(&factor, &scaleMatrix);

    // build the model matrix
    csrMat4Multiply(&scaleMatrix,        &rotateXMatrix, &intermediateMatrix);
    csrMat4Multiply(&intermediateMatrix, &rotateYMatrix, &m_Bot.m_Matrix);
}
//---------------------------------------------------------------------------
void TMainForm::BuildFaderMatrix()
{
    CSR_Vector3 translation;
    CSR_Vector3 axis;
    CSR_Matrix4 translateMatrix;
    CSR_Matrix4 rotateYMatrix;
    float       angle;

    csrMat4Identity(&m_FaderMatrix);

    // calculate the fader angle
    angle = (M_PI * 2.0f) - m_Angle;

    // calculate the fader position
    translation.m_X = m_BoundingSphere.m_Center.m_X - (0.05 * sin(angle));
    translation.m_Y = m_BoundingSphere.m_Center.m_Y;
    translation.m_Z = m_BoundingSphere.m_Center.m_Z - (0.05 * cos(angle));

    // create the translate matrix
    csrMat4Translate(&translation, &translateMatrix);

    // set rotation axis
    axis.m_X = 0.0f;
    axis.m_Y = 1.0f;
    axis.m_Z = 0.0f;

    // create the rotation matrix
    csrMat4Rotate(angle, &axis, &rotateYMatrix);

    // build the final matrix
    csrMat4Multiply(&rotateYMatrix, &translateMatrix, &m_FaderMatrix);
}
//---------------------------------------------------------------------------
int TMainForm::ApplyGroundCollision(const CSR_Sphere* pBoundingSphere, CSR_Matrix4* pMatrix) const
{
    if (!m_pScene)
        return 0;

    // validate the inputs
    if (!pBoundingSphere || !pMatrix)
        return 0;

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
    csrSceneDetectCollision(m_pScene, &collisionInput, &collisionOutput, 0);

    // update the ground position directly inside the matrix (this is where the final value is required)
    pMatrix->m_Table[3][1] = -collisionOutput.m_GroundPos;

    if (collisionOutput.m_Collision & CSR_CO_Ground)
        return 1;

    return 0;
}
//------------------------------------------------------------------------------
int TMainForm::CheckPlayerVisible()
{
    CSR_SceneItem*     pSceneItem;
    CSR_Vector3        botToPlayer;
    CSR_Vector3        botToPlayerDir;
    CSR_Polygon3Buffer polygons;
    float              angle;

    // calculate the bot to player vector
    botToPlayer.m_X = m_Bot.m_Geometry.m_Center.m_X - m_BoundingSphere.m_Center.m_X;
    botToPlayer.m_Y = m_Bot.m_Geometry.m_Center.m_Y - m_BoundingSphere.m_Center.m_Y;
    botToPlayer.m_Z = m_Bot.m_Geometry.m_Center.m_Z - m_BoundingSphere.m_Center.m_Z;

    // calculate the angle between the bot dir and the bot to player dir
    csrVec3Normalize(&botToPlayer, &botToPlayerDir);
    csrVec3Dot(&botToPlayerDir, &m_Bot.m_Dir, &angle);

    // reset the bot detection signal
    csrShaderEnable(m_pShader);
    glUniform1i(m_GrayscaleSlot, 0);

    // is bot showing the player?
    if (angle > -0.707f)
        return 0;

    // set the bot detection signal
    csrShaderEnable(m_pShader);
    glUniform1i(m_GrayscaleSlot, 1);

    return 1;
}
//------------------------------------------------------------------------------
int TMainForm::CheckBotHitPlayer()
{
    CSR_Figure3 firstSphere;
    CSR_Figure3 secondSphere;

    // get the player bounding object
    firstSphere.m_Type    = CSR_F3_Sphere;
    firstSphere.m_pFigure = &m_BoundingSphere;

    // get the bot bounding object
    secondSphere.m_Type    = CSR_F3_Sphere;
    secondSphere.m_pFigure = &m_Bot.m_Geometry;

    return csrIntersect3(&firstSphere, &secondSphere, 0, 0, 0);
}
//------------------------------------------------------------------------------
void TMainForm::OnPrepareWatching()
{
    // calculate a new sleep time and reset the elapsed time
    m_WatchingTime  = (double)(rand() % 200) / 100.0;
    m_ElapsedTime   = 0.0;
    m_BotShowPlayer = 0;
}
//------------------------------------------------------------------------------
int TMainForm::OnWatching(double elapsedTime)
{
    if (m_BotAlpha < 1.0f)
        m_BotAlpha += 1.0f * elapsedTime;

    if (m_BotAlpha >= 1.0f)
    {
        m_BotAlpha            = 1.0f;
        m_Bot.m_DyingSequence = E_DS_None;
    }

    // is player alive?
    if (!m_PlayerDying)
    {
        // player hit bot?
        if (m_BotHitPlayer)
        {
            m_BotDying = 1;

            // play hit sound
            csrSoundPlay(m_pHitSound);

            return 1;
        }

        // is bot showing player?
        if (CheckPlayerVisible())
        {
            m_BotShowPlayer = 1;
            return 1;
        }
    }

    // watching time elapsed?
    if (m_ElapsedTime >= m_WatchingTime)
        return 1;

    m_ElapsedTime += elapsedTime;
    return 0;
}
//------------------------------------------------------------------------------
void TMainForm::OnPrepareSearching()
{
    CSR_Vector3 dir;
    CSR_Vector3 refDir;
    CSR_Vector3 refNormal;
    float       angle;

    // build the reference dir
    refDir.m_X = 1.0f;
    refDir.m_Y = 0.0f;
    refDir.m_Z = 0.0f;

    // build the reference dir
    refNormal.m_X = 0.0f;
    refNormal.m_Y = 0.0f;
    refNormal.m_Z = 1.0f;

    // keep the current position as the start one
    m_Bot.m_StartPosition.m_X = m_Bot.m_Geometry.m_Center.m_X;
    m_Bot.m_StartPosition.m_Y = m_Bot.m_Geometry.m_Center.m_Z;

    // get a new position to move to
    m_Bot.m_EndPosition.m_X = -3.0f + ((rand() % 600) / 100.0f);
    m_Bot.m_EndPosition.m_Y = -3.0f + ((rand() % 600) / 100.0f);

    // calculate the bot direction
    dir.m_X = m_Bot.m_EndPosition.m_X - m_Bot.m_StartPosition.m_X;
    dir.m_Y = 0.0f;
    dir.m_Z = m_Bot.m_EndPosition.m_Y - m_Bot.m_StartPosition.m_Y;
    csrVec3Normalize(&dir, &m_Bot.m_Dir);

    // calculate the bot direction angle
    csrVec3Dot(&refDir, &m_Bot.m_Dir, &angle);
    m_Bot.m_Angle = std::acos(angle);

    // calculate the bot direction normal angle
    csrVec3Dot(&refNormal, &m_Bot.m_Dir, &angle);

    // calculate the final bot direction angle
    if (angle < 0.0f)
        m_Bot.m_Angle = std::fmod(m_Bot.m_Angle, float(M_PI * 2.0));
    else
        m_Bot.m_Angle = (M_PI * 2.0f) - std::fmod(m_Bot.m_Angle, float(M_PI * 2.0));

    m_Bot.m_MovePos = 0.0f;
    m_BotShowPlayer = 0;
}
//------------------------------------------------------------------------------
int TMainForm::OnSearching(double elapsedTime)
{
    if (m_BotAlpha < 1.0f)
        m_BotAlpha += 1.0f * elapsedTime;

    if (m_BotAlpha >= 1.0f)
    {
        m_BotAlpha            = 1.0f;
        m_Bot.m_DyingSequence = E_DS_None;
    }

    // calculate the next position
    m_Bot.m_Geometry.m_Center.m_X = m_Bot.m_StartPosition.m_X + ((m_Bot.m_EndPosition.m_X - m_Bot.m_StartPosition.m_X) * m_Bot.m_MovePos);
    m_Bot.m_Geometry.m_Center.m_Z = m_Bot.m_StartPosition.m_Y + ((m_Bot.m_EndPosition.m_Y - m_Bot.m_StartPosition.m_Y) * m_Bot.m_MovePos);

    // calculate the next move
    m_Bot.m_MovePos += m_Bot.m_Velocity * elapsedTime;

    // is player alive?
    if (!m_PlayerDying)
    {
        // player hit bot?
        if (m_BotHitPlayer)
        {
            m_BotDying = 1;

            // play hit sound
            csrSoundPlay(m_pHitSound);

            return 1;
        }

        // is bot showing player?
        if (CheckPlayerVisible())
        {
            m_BotShowPlayer = 1;
            return 1;
        }
    }

    // check if move end was reached
    return m_Bot.m_MovePos >= 1.0f;
}
//------------------------------------------------------------------------------
void TMainForm::OnPrepareAttacking()
{
    CSR_Vector3 dir;
    CSR_Vector3 refDir;
    CSR_Vector3 refNormal;
    float       angle;

    // build the reference dir
    refDir.m_X = 1.0f;
    refDir.m_Y = 0.0f;
    refDir.m_Z = 0.0f;

    // build the reference dir
    refNormal.m_X = 0.0f;
    refNormal.m_Y = 0.0f;
    refNormal.m_Z = 1.0f;

    // keep the current position as the start one
    m_Bot.m_StartPosition.m_X = m_Bot.m_Geometry.m_Center.m_X;
    m_Bot.m_StartPosition.m_Y = m_Bot.m_Geometry.m_Center.m_Z;

    // get the player position as destination
    m_Bot.m_EndPosition.m_X = m_BoundingSphere.m_Center.m_X;
    m_Bot.m_EndPosition.m_Y = m_BoundingSphere.m_Center.m_Z;

    // calculate the bot direction
    dir.m_X = m_Bot.m_EndPosition.m_X - m_Bot.m_StartPosition.m_X;
    dir.m_Y = 0.0f;
    dir.m_Z = m_Bot.m_EndPosition.m_Y - m_Bot.m_StartPosition.m_Y;
    csrVec3Normalize(&dir, &m_Bot.m_Dir);

    // calculate the bot direction angle
    csrVec3Dot(&refDir, &m_Bot.m_Dir, &angle);
    m_Bot.m_Angle = std::acos(angle);

    // calculate the bot direction normal angle
    csrVec3Dot(&refNormal, &m_Bot.m_Dir, &angle);

    // calculate the final bot direction angle
    if (angle < 0.0f)
        m_Bot.m_Angle = std::fmod(m_Bot.m_Angle, float(M_PI * 2.0));
    else
        m_Bot.m_Angle = (M_PI * 2.0f) - std::fmod(m_Bot.m_Angle, float(M_PI * 2.0));

    m_Bot.m_MovePos = 0.0f;
    m_BotShowPlayer = 0;
}
//------------------------------------------------------------------------------
int TMainForm::OnAttacking(double elapsedTime)
{
    if (m_BotAlpha < 1.0f)
        m_BotAlpha += 1.0f * elapsedTime;

    if (m_BotAlpha >= 1.0f)
    {
        m_BotAlpha            = 1.0f;
        m_Bot.m_DyingSequence = E_DS_None;
    }

    // calculate the next position
    m_Bot.m_Geometry.m_Center.m_X = m_Bot.m_StartPosition.m_X + ((m_Bot.m_EndPosition.m_X - m_Bot.m_StartPosition.m_X) * m_Bot.m_MovePos);
    m_Bot.m_Geometry.m_Center.m_Z = m_Bot.m_StartPosition.m_Y + ((m_Bot.m_EndPosition.m_Y - m_Bot.m_StartPosition.m_Y) * m_Bot.m_MovePos);

    // calculate the next move
    m_Bot.m_MovePos += m_Bot.m_Velocity * elapsedTime;

    // to keep the correct visual
    CheckPlayerVisible();

    // the bot hit the player?
    if (m_BotHitPlayer)
    {
        m_PlayerDying = 1;

        // play hit sound
        csrSoundPlay(m_pHitSound);

        return 1;
    }

    // check if move end was reached
    return (m_Bot.m_MovePos >= 1.0f);
}
//------------------------------------------------------------------------------
void TMainForm::OnPrepareDying()
{
    m_Bot.m_DyingSequence = E_DS_Dying;
    m_LastKnownIndex      = 0;
}
//------------------------------------------------------------------------------
int TMainForm::OnDying(double elapsedTime)
{
    // execute the dying sequence
    switch (m_Bot.m_DyingSequence)
    {
        case E_DS_Dying:
            // dying animation end reached?
            if (m_LastKnownIndex && m_ModelIndex < m_LastKnownIndex)
            {
                m_ModelIndex          = m_LastKnownIndex;
                m_Bot.m_DyingSequence = E_DS_FadeOut;
            }
            else
                m_LastKnownIndex = m_ModelIndex;

            break;

        case E_DS_FadeOut:
            // fade out the bot
            m_BotAlpha -= 1.0f * elapsedTime;

            // fade out end reached?
            if (m_BotAlpha <= 0.0f)
            {
                m_BotAlpha            = 0.0f;
                m_Bot.m_DyingSequence = E_DS_FadeIn;

                // ...thus a new position will be calculated
                OnPrepareSearching();

                // apply it immediately
                m_Bot.m_Geometry.m_Center.m_X = m_Bot.m_EndPosition.m_X;
                m_Bot.m_Geometry.m_Center.m_Z = m_Bot.m_EndPosition.m_Y;
            }

            break;

        case E_DS_FadeIn:
            m_BotShowPlayer = 0;
            m_BotDying      = 0;
            return 1;
    }

    return 0;
}
//------------------------------------------------------------------------------
void TMainForm::OnTaskChange(CSR_Task* pTask, double elapsedTime)
{
    // is bot dying?
    if (m_BotDying)
    {
        OnPrepareDying();

        m_pTaskManager->m_pTask->m_Action = E_BT_Dying;
        m_AnimIndex                       = 4;
        return;
    }

    // do bot attack the player?
    if (m_BotShowPlayer && !m_PlayerDying && pTask->m_Action != E_BT_Dying)
    {
        OnPrepareAttacking();

        m_AnimIndex     = 1;
        pTask->m_Action = E_BT_Attacking;
        return;
    }

    // prepare the next task
    switch (pTask->m_Action)
    {
        case E_BT_Watching:
            OnPrepareSearching();

            m_AnimIndex     = 1;
            pTask->m_Action = E_BT_Searching;
            break;

        case E_BT_Searching:
        case E_BT_Attacking:
        case E_BT_Dying:
            OnPrepareWatching();

            m_AnimIndex     = 0;
            pTask->m_Action = E_BT_Watching;
            break;
    }
}
//------------------------------------------------------------------------------
int TMainForm::OnTaskRun(CSR_Task* pTask, double elapsedTime)
{
    switch (pTask->m_Action)
    {
        case E_BT_Watching:  return OnWatching(elapsedTime);
        case E_BT_Searching: return OnSearching(elapsedTime);
        case E_BT_Attacking: return OnAttacking(elapsedTime);
        case E_BT_Dying:     return OnDying(elapsedTime);
    }

    return 0;
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
    if (pModel == m_pScene->m_pSkybox)
        return m_pSkyboxShader;

    csrShaderEnable(m_pShader);

    // bot or fader?
    if (pModel == m_Bot.m_pModel)
        glUniform1f(m_AlphaSlot, m_BotAlpha);
    else
    if (pModel == m_pFader)
        glUniform1f(m_AlphaSlot, m_FaderAlpha);
    else
        glUniform1f(m_AlphaSlot, 1.0f);

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
void TMainForm::OnApplySkin(std::size_t index, const CSR_Skin* pSkin, int* pCanRelease)
{
    // load the texture from the received pixel buffer
    const GLuint textureID = csrOpenGLTextureFromPixelBuffer(pSkin->m_Texture.m_pBuffer);

    // succeeded?
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
void TMainForm::OnGetMDLIndex(const CSR_MDL*     pMDL,
                                    std::size_t* pSkinIndex,
                                    std::size_t* pModelIndex,
                                    std::size_t* pMeshIndex)
{
    *pSkinIndex  = m_TextureIndex;
    *pModelIndex = m_ModelIndex;
    *pMeshIndex  = m_MeshIndex;
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

    // detect the keyboard directly (unfortunately the Windows message loop detects keys one by
    // one, as well as DirectInput if not in exclusive mode, which isn't possible if the window isn't
    // in full screen)
    m_PosVelocity = 0.0f;
    m_DirVelocity = 0.0f;

    // a key or left arrow?
    if (::GetAsyncKeyState(0x41) || ::GetAsyncKeyState(VK_LEFT))
        m_DirVelocity = -1.5f;

    // d key or right arrow?
    if (::GetAsyncKeyState(0x44) || ::GetAsyncKeyState(VK_RIGHT))
        m_DirVelocity = 1.5f;

    // w key or up arrow?
    if (::GetAsyncKeyState(0x57) || ::GetAsyncKeyState(VK_UP))
        m_PosVelocity = -1.0f;

    // s key or down arrow?
    if (::GetAsyncKeyState(0x53) || ::GetAsyncKeyState(VK_DOWN))
        m_PosVelocity = 1.0f;

    OnDrawScene(false);
}
//---------------------------------------------------------------------------
