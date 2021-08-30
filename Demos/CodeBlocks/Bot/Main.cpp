/*****************************************************************************
 * ==> Bot demo -------------------------------------------------------------*
 *****************************************************************************
 * Description : An intelligent bot demo                                     *
 * Developer   : Jean-Milost Reymond                                         *
 * Copyright   : 2015 - 2021, this file is part of the Minimal API. You are  *
 *               free to copy or redistribute this file, modify it, or use   *
 *               it for your own projects, commercial or not. This file is   *
 *               provided "as is", without ANY WARRANTY OF ANY KIND          *
 *****************************************************************************/

// std
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Geometry.h"
#include "CSR_Collision.h"
#include "CSR_Texture.h"
#include "CSR_Vertex.h"
#include "CSR_Model.h"
#include "CSR_Scene.h"
#include "CSR_AI.h"
#include "CSR_Renderer_OpenGL.h"
#include "CSR_Sound.h"

// classes
#include "CSR_OpenGLHelper.h"
#include "CSR_ShaderHelper.h"

// windows
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// resources
#include "resources.rh"

// resource files to load
#define LANDSCAPE_TEXTURE_FILE     "\\grass.bmp"
#define LANDSCAPE_DATA_FILE        "\\bot_level.bmp"
#define FADER_TEXTURE_FILE         "\\blank.bmp"
#define MDL_FILE                   "\\wizard.mdl"
#define SKYBOX_LEFT                "\\left.bmp"
#define SKYBOX_TOP                 "\\top.bmp"
#define SKYBOX_RIGHT               "\\right.bmp"
#define SKYBOX_BOTTOM              "\\bottom.bmp"
#define SKYBOX_FRONT               "\\front.bmp"
#define SKYBOX_BACK                "\\back.bmp"
#define FOOT_STEP_LEFT_SOUND_FILE  "\\footstep_left.wav"
#define FOOT_STEP_RIGHT_SOUND_FILE "\\footstep_right.wav"
#define HIT_SOUND_FILE             "\\hit.wav"

//------------------------------------------------------------------------------
/**
* Artificial intelligence tasks enumeration
*/
typedef enum
{
    E_BT_Watching,
    E_BT_Searching,
    E_BT_Attacking,
    E_BT_Dying
} CSR_EBotTask;
//------------------------------------------------------------------------------
/**
* Bot dying sequence
*/
typedef enum
{
    E_DS_None,
    E_DS_Dying,
    E_DS_FadeOut,
    E_DS_FadeIn
} CSR_EDyingSequence;
//------------------------------------------------------------------------------
/**
* Bot
*/
typedef struct
{
    CSR_MDL*           m_pModel;
    CSR_Sphere         m_Geometry;
    CSR_Vector2        m_StartPosition;
    CSR_Vector2        m_EndPosition;
    CSR_Vector3        m_Dir;
    CSR_Matrix4        m_Matrix;
    CSR_EDyingSequence m_DyingSequence;
    float              m_Angle;
    float              m_Velocity;
    float              m_MovePos;
} CSR_Bot;
//------------------------------------------------------------------------------
/**
* Terrain limits
*/
typedef struct
{
    CSR_Vector2 m_Min;
    CSR_Vector2 m_Max;
} CSR_TerrainLimits;
//----------------------------------------------------------------------------
typedef std::vector<std::string> IFileNames;
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
//------------------------------------------------------------------------------
HDC                          g_hDC                 = 0;
HGLRC                        g_hRC                 = 0;
CSR_OpenGLHelper::IResources g_OpenGLResources;
ALCdevice*                   g_pOpenALDevice       = NULL;
ALCcontext*                  g_pOpenALContext      = NULL;
CSR_Scene*                   g_pScene              = NULL;
CSR_TaskManager*             g_pTaskManager        = NULL;
CSR_OpenGLShader*            g_pShader             = NULL;
CSR_OpenGLShader*            g_pSkyboxShader       = NULL;
CSR_Mesh*                    g_pFader              = NULL;
void*                        g_pLandscapeKey       = NULL;
float                        g_MapHeight           = 3.0f;
float                        g_MapScale            = 0.2f;
float                        g_Angle               = 0.0f;
float                        g_RotationSpeed       = 0.02f;
float                        g_StepTime            = 0.0f;
float                        g_StepInterval        = 300.0f;
float                        g_PosVelocity         = 0.0f;
float                        g_DirVelocity         = 0.0f;
float                        g_ControlRadius       = 40.0f;
float                        g_BotAlpha            = 1.0f;
float                        g_FaderAlpha          = 0.0f;
float                        g_Time                = 0.0f;
float                        g_Interval            = 0.0f;
double                       g_TextureLastTime     = 0.0;
double                       g_ModelLastTime       = 0.0;
double                       g_MeshLastTime        = 0.0;
double                       g_WatchingTime        = 0.0;
double                       g_ElapsedTime         = 0.0;
size_t                       g_AnimIndex           = 0;
size_t                       g_TextureIndex        = 0;
size_t                       g_ModelIndex          = 0;
size_t                       g_MeshIndex           = 0;
size_t                       g_LastKnownIndex      = 0;
size_t                       g_FrameCount          = 0;
int                          g_AlternateStep       = 0;
int                          g_BotShowPlayer       = 0;
int                          g_BotHitPlayer        = 0;
int                          g_BotDying            = 0;
int                          g_PlayerDying         = 0;
CSR_SceneContext             g_SceneContext;
CSR_TaskContext              g_TaskContext;
CSR_Bot                      g_Bot;
CSR_Sphere                   g_BoundingSphere;
CSR_Matrix4                  g_LandscapeMatrix;
CSR_Matrix4                  g_FaderMatrix;
CSR_Vector2                  g_TouchOrigin;
CSR_Vector2                  g_TouchPosition;
CSR_Color                    g_Color;
CSR_TerrainLimits            g_TerrainLimits;
GLuint                       g_GrayscaleSlot       = 0;
GLuint                       g_AlphaSlot           = 0;
CSR_Sound*                   g_pFootStepLeftSound  = NULL;
CSR_Sound*                   g_pFootStepRightSound = NULL;
CSR_Sound*                   g_pHitSound           = NULL;
std::string                  g_SceneDir;
double                       g_FPS                 = 0.0;
unsigned __int64             g_StartTime           = 0L;
unsigned __int64             g_PreviousTime        = 0L;
bool                         g_Initialized         = false;
//------------------------------------------------------------------------------
int CheckPlayerVisible();
int CheckBotHitPlayer();
void UpdateScene(float elapsedTime);
void DrawScene();
//------------------------------------------------------------------------------
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
            break;

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_LEFT:  g_DirVelocity = -1.0f; break;
                case VK_RIGHT: g_DirVelocity =  1.0f; break;
                case VK_UP:    g_PosVelocity = -1.0f; break;
                case VK_DOWN:  g_PosVelocity =  1.0f; break;
            }

            break;
        }

        case WM_KEYUP:
        {
            switch (wParam)
            {
                case VK_LEFT:
                case VK_RIGHT: g_DirVelocity = 0.0f; break;

                case VK_UP:
                case VK_DOWN:  g_PosVelocity = 0.0f; break;
            }

            break;
        }

        case WM_SIZE:
        {
            if (!g_Initialized)
                break;

            const int width  = ((int)(short)LOWORD(lParam));
            const int height = ((int)(short)HIWORD(lParam));

            // update the viewport
            CSR_OpenGLHelper::CreateViewport((float)width,
                                             (float)height,
                                             0.01f,
                                             100.0f,
                                             g_pShader,
                                             g_pScene->m_ProjectionMatrix);

            UpdateScene(0.0f);
            DrawScene();

            ::SwapBuffers(g_hDC);
            break;
        }

        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}
//---------------------------------------------------------------------------
void* OnGetShader(const void* pModel, CSR_EModelType type)
{
    if (pModel == g_pScene->m_pSkybox)
        return g_pSkyboxShader;

    csrShaderEnable(g_pShader);

    // bot or fader?
    if (pModel == g_Bot.m_pModel)
        glUniform1f(g_AlphaSlot, g_BotAlpha);
    else
    if (pModel == g_pFader)
        glUniform1f(g_AlphaSlot, g_FaderAlpha);
    else
        glUniform1f(g_AlphaSlot, 1.0f);

    return g_pShader;
}
//---------------------------------------------------------------------------
void* OnGetID(const void* pKey)
{
    return CSR_OpenGLHelper::GetTextureID(pKey, g_OpenGLResources);
}
//---------------------------------------------------------------------------
void OnApplySkin(std::size_t index, const CSR_Skin* pSkin, int* pCanRelease)
{
    // load the texture from the received pixel buffer
    const GLuint textureID = csrOpenGLTextureFromPixelBuffer(pSkin->m_Texture.m_pBuffer);

    // succeeded?
    if (textureID == M_CSR_Error_Code)
        return;

    // add the texture to the OpenGL resources
    CSR_OpenGLHelper::AddTexture(&pSkin->m_Texture, textureID, g_OpenGLResources);

    // from now the texture resource on the model side may be released (will no longer be used)
    if (pCanRelease)
        *pCanRelease = 1;
}
//---------------------------------------------------------------------------
void OnGetMDLIndex(const CSR_MDL* pMDL,
                   std::size_t* pSkinIndex,
                   std::size_t* pModelIndex,
                   std::size_t* pMeshIndex)
{
    *pSkinIndex  = g_TextureIndex;
    *pModelIndex = g_ModelIndex;
    *pMeshIndex  = g_MeshIndex;
}
//---------------------------------------------------------------------------
void OnDeleteTexture(const CSR_Texture* pTexture)
{
    return CSR_OpenGLHelper::DeleteTexture(pTexture, g_OpenGLResources);
}
//------------------------------------------------------------------------------
void OnPrepareWatching()
{
    // calculate a new sleep time and reset the elapsed time
    g_WatchingTime  = (double)(rand() % 200) / 100.0;
    g_ElapsedTime   = 0.0;
    g_BotShowPlayer = 0;
}
//------------------------------------------------------------------------------
int OnWatching(double elapsedTime)
{
    if (g_BotAlpha < 1.0f)
        g_BotAlpha += 1.0f * (float)elapsedTime;

    if (g_BotAlpha >= 1.0f)
    {
        g_BotAlpha            = 1.0f;
        g_Bot.m_DyingSequence = E_DS_None;
    }

    // is player alive?
    if (!g_PlayerDying)
    {
        // player hit bot?
        if (g_BotHitPlayer)
        {
            g_BotDying = 1;

            // play hit sound
            csrSoundPlay(g_pHitSound);

            return 1;
        }

        // is bot showing player?
        if (CheckPlayerVisible())
        {
            g_BotShowPlayer = 1;
            return 1;
        }
    }

    // watching time elapsed?
    if (g_ElapsedTime >= g_WatchingTime)
        return 1;

    g_ElapsedTime += elapsedTime;
    return 0;
}
//------------------------------------------------------------------------------
void OnPrepareSearching()
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
    g_Bot.m_StartPosition.m_X = g_Bot.m_Geometry.m_Center.m_X;
    g_Bot.m_StartPosition.m_Y = g_Bot.m_Geometry.m_Center.m_Z;

    // get a new position to move to
    g_Bot.m_EndPosition.m_X = -3.0f + ((rand() % 600) / 100.0f);
    g_Bot.m_EndPosition.m_Y = -3.0f + ((rand() % 600) / 100.0f);

    // calculate the bot direction
    dir.m_X = g_Bot.m_EndPosition.m_X - g_Bot.m_StartPosition.m_X;
    dir.m_Y = 0.0f;
    dir.m_Z = g_Bot.m_EndPosition.m_Y - g_Bot.m_StartPosition.m_Y;
    csrVec3Normalize(&dir, &g_Bot.m_Dir);

    // calculate the bot direction angle
    csrVec3Dot(&refDir, &g_Bot.m_Dir, &angle);
    g_Bot.m_Angle = acosf(angle);

    // calculate the bot direction normal angle
    csrVec3Dot(&refNormal, &g_Bot.m_Dir, &angle);

    // calculate the final bot direction angle
    if (angle < 0.0f)
        g_Bot.m_Angle = fmodf(g_Bot.m_Angle, float(M_PI * 2.0));
    else
        g_Bot.m_Angle = float(M_PI * 2.0) - fmodf(g_Bot.m_Angle, float(M_PI * 2.0));

    g_Bot.m_MovePos = 0.0f;
    g_BotShowPlayer = 0;
}
//------------------------------------------------------------------------------
int OnSearching(double elapsedTime)
{
    if (g_BotAlpha < 1.0f)
        g_BotAlpha += 1.0f * (float)elapsedTime;

    if (g_BotAlpha >= 1.0f)
    {
        g_BotAlpha            = 1.0f;
        g_Bot.m_DyingSequence = E_DS_None;
    }

    // calculate the next position
    g_Bot.m_Geometry.m_Center.m_X = g_Bot.m_StartPosition.m_X + ((g_Bot.m_EndPosition.m_X - g_Bot.m_StartPosition.m_X) * g_Bot.m_MovePos);
    g_Bot.m_Geometry.m_Center.m_Z = g_Bot.m_StartPosition.m_Y + ((g_Bot.m_EndPosition.m_Y - g_Bot.m_StartPosition.m_Y) * g_Bot.m_MovePos);

    // calculate the next move
    g_Bot.m_MovePos += g_Bot.m_Velocity * (float)elapsedTime;

    // is player alive?
    if (!g_PlayerDying)
    {
        // player hit bot?
        if (g_BotHitPlayer)
        {
            g_BotDying = 1;

            // play hit sound
            csrSoundPlay(g_pHitSound);

            return 1;
        }

        // is bot showing player?
        if (CheckPlayerVisible())
        {
            g_BotShowPlayer = 1;
            return 1;
        }
    }

    // check if move end was reached
    return g_Bot.m_MovePos >= 1.0f;
}
//------------------------------------------------------------------------------
void OnPrepareAttacking()
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
    g_Bot.m_StartPosition.m_X = g_Bot.m_Geometry.m_Center.m_X;
    g_Bot.m_StartPosition.m_Y = g_Bot.m_Geometry.m_Center.m_Z;

    // get the player position as destination
    g_Bot.m_EndPosition.m_X = g_BoundingSphere.m_Center.m_X;
    g_Bot.m_EndPosition.m_Y = g_BoundingSphere.m_Center.m_Z;

    // calculate the bot direction
    dir.m_X = g_Bot.m_EndPosition.m_X - g_Bot.m_StartPosition.m_X;
    dir.m_Y = 0.0f;
    dir.m_Z = g_Bot.m_EndPosition.m_Y - g_Bot.m_StartPosition.m_Y;
    csrVec3Normalize(&dir, &g_Bot.m_Dir);

    // calculate the bot direction angle
    csrVec3Dot(&refDir, &g_Bot.m_Dir, &angle);
    g_Bot.m_Angle = acosf(angle);

    // calculate the bot direction normal angle
    csrVec3Dot(&refNormal, &g_Bot.m_Dir, &angle);

    // calculate the final bot direction angle
    if (angle < 0.0f)
        g_Bot.m_Angle = fmodf(g_Bot.m_Angle, float(M_PI * 2.0));
    else
        g_Bot.m_Angle = float(M_PI * 2.0) - fmodf(g_Bot.m_Angle, float(M_PI * 2.0));

    g_Bot.m_MovePos = 0.0f;
    g_BotShowPlayer = 0;
}
//------------------------------------------------------------------------------
int OnAttacking(double elapsedTime)
{
    if (g_BotAlpha < 1.0f)
        g_BotAlpha += 1.0f * (float)elapsedTime;

    if (g_BotAlpha >= 1.0f)
    {
        g_BotAlpha            = 1.0f;
        g_Bot.m_DyingSequence = E_DS_None;
    }

    // calculate the next position
    g_Bot.m_Geometry.m_Center.m_X = g_Bot.m_StartPosition.m_X + ((g_Bot.m_EndPosition.m_X - g_Bot.m_StartPosition.m_X) * g_Bot.m_MovePos);
    g_Bot.m_Geometry.m_Center.m_Z = g_Bot.m_StartPosition.m_Y + ((g_Bot.m_EndPosition.m_Y - g_Bot.m_StartPosition.m_Y) * g_Bot.m_MovePos);

    // calculate the next move
    g_Bot.m_MovePos += g_Bot.m_Velocity * (float)elapsedTime;

    // to keep the correct visual
    CheckPlayerVisible();

    // the bot hit the player?
    if (g_BotHitPlayer)
    {
        g_PlayerDying = 1;

        // play hit sound
        csrSoundPlay(g_pHitSound);

        return 1;
    }

    // check if move end was reached
    return (g_Bot.m_MovePos >= 1.0f);
}
//------------------------------------------------------------------------------
void OnPrepareDying()
{
    g_Bot.m_DyingSequence = E_DS_Dying;
    g_LastKnownIndex      = 0;
}
//------------------------------------------------------------------------------
int OnDying(double elapsedTime)
{
    // execute the dying sequence
    switch (g_Bot.m_DyingSequence)
    {
        case E_DS_Dying:
            // dying animation end reached?
            if (g_LastKnownIndex && g_ModelIndex < g_LastKnownIndex)
            {
                g_ModelIndex          = g_LastKnownIndex;
                g_Bot.m_DyingSequence = E_DS_FadeOut;
            }
            else
                g_LastKnownIndex = g_ModelIndex;

            break;

        case E_DS_FadeOut:
            // fade out the bot
            g_BotAlpha -= 1.0f * (float)elapsedTime;

            // fade out end reached?
            if (g_BotAlpha <= 0.0f)
            {
                g_BotAlpha            = 0.0f;
                g_Bot.m_DyingSequence = E_DS_FadeIn;

                // ...thus a new position will be calculated
                OnPrepareSearching();

                // apply it immediately
                g_Bot.m_Geometry.m_Center.m_X = g_Bot.m_EndPosition.m_X;
                g_Bot.m_Geometry.m_Center.m_Z = g_Bot.m_EndPosition.m_Y;
            }

            break;

        case E_DS_FadeIn:
            g_BotShowPlayer = 0;
            g_BotDying      = 0;
            return 1;
    }

    return 0;
}
//------------------------------------------------------------------------------
void OnTaskChange(CSR_Task* pTask, double elapsedTime)
{
    // is bot dying?
    if (g_BotDying)
    {
        OnPrepareDying();

        g_pTaskManager->m_pTask->m_Action = E_BT_Dying;
        g_AnimIndex                       = 4;
        return;
    }

    // do bot attack the player?
    if (g_BotShowPlayer && !g_PlayerDying && pTask->m_Action != E_BT_Dying)
    {
        OnPrepareAttacking();

        g_AnimIndex     = 1;
        pTask->m_Action = E_BT_Attacking;
        return;
    }

    // prepare the next task
    switch (pTask->m_Action)
    {
        case E_BT_Watching:
            OnPrepareSearching();

            g_AnimIndex     = 1;
            pTask->m_Action = E_BT_Searching;
            break;

        case E_BT_Searching:
        case E_BT_Attacking:
        case E_BT_Dying:
            OnPrepareWatching();

            g_AnimIndex     = 0;
            pTask->m_Action = E_BT_Watching;
            break;
    }
}
//------------------------------------------------------------------------------
int OnTaskRun(CSR_Task* pTask, double elapsedTime)
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
int LoadLandscapeFromBitmap(const char* pFileName)
{
    if (!pFileName)
        return 0;

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
    pBitmap = csrPixelBufferFromBitmapFile(pFileName);

    // succeeded?
    if (!pBitmap)
    {
        csrModelRelease(pModel, OnDeleteTexture);
        return 0;
    }

    // load the landscape mesh from the grayscale bitmap
    pModel->m_pMesh     = csrLandscapeCreate(pBitmap, 3.0f, 0.2f, &vf, &vc, &material, 0);
    pModel->m_MeshCount = 1;

    csrPixelBufferRelease(pBitmap);

    csrMat4Identity(&g_LandscapeMatrix);

    // add the model to the scene
    pSceneItem = csrSceneAddModel(g_pScene, pModel, 0, 1);
    csrSceneAddModelMatrix(g_pScene, pModel, &g_LandscapeMatrix);

    // succeeded?
    if (pSceneItem)
        pSceneItem->m_CollisionType = CSR_CO_Ground;

    // keep the key
    g_pLandscapeKey = pModel;

    return 1;
}
//---------------------------------------------------------------------------
GLuint LoadTexture(const char* pFileName)
{
    if (!pFileName)
        return M_CSR_Error_Code;

    try
    {
        // load the texture content from bitmap
        CSR_PixelBuffer* pPixelBuffer = csrPixelBufferFromBitmapFile(pFileName);

        // load the texture on the GPU
        GLuint textureID = csrOpenGLTextureFromPixelBuffer(pPixelBuffer);

        // clear the memory
        csrPixelBufferRelease(pPixelBuffer);

        return textureID;
    }
    catch (...)
    {
        return M_CSR_Error_Code;
    }
}
//---------------------------------------------------------------------------
GLuint LoadCubemap(const IFileNames fileNames)
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
            // load the texture content from bitmap
            CSR_PixelBuffer* pPixelBuffer = csrPixelBufferFromBitmapFile(fileNames[i].c_str());

            if (!pPixelBuffer || !pPixelBuffer->m_Width || !pPixelBuffer->m_Height)
                return M_CSR_Error_Code;

            unsigned char* pPixels = 0;
            unsigned       x;
            unsigned       y;
            unsigned char  c;
            GLint          pixelType;
            GLuint         index   = 0;

            // select the correct pixel type to use
            switch (pPixelBuffer->m_BytePerPixel)
            {
                case 3:  pixelType = GL_RGB;  break;
                case 4:  pixelType = GL_RGBA; break;
                default: return M_CSR_Error_Code;
            }

            // reorder the pixels
            pPixels = (unsigned char*)malloc(sizeof(unsigned char)  *
                                             pPixelBuffer->m_Width  *
                                             pPixelBuffer->m_Height *
                                             3);

            // get bitmap data into right format
            for (y = 0; y < pPixelBuffer->m_Height; ++y)
                for (x = 0; x < pPixelBuffer->m_Width; ++x)
                    for (c = 0; c < 3; ++c)
                        pPixels[3 * (pPixelBuffer->m_Width * y + x) + c] =
                                ((unsigned char*)pPixelBuffer->m_pData)
                                        [pPixelBuffer->m_Stride * y + 3 * (pPixelBuffer->m_Width - x - 1) + (2 - c)];

            // load the texture on the GPU
            glTexImage2D((GLenum)(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i),
                         0,
                         pixelType,
                         pPixelBuffer->m_Width,
                         pPixelBuffer->m_Height,
                         0,
                         pixelType,
                         GL_UNSIGNED_BYTE,
                         pPixels);

            free(pPixels);
            csrPixelBufferRelease(pPixelBuffer);
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
//---------------------------------------------------------------------------
void BuildBotMatrix()
{
    CSR_Vector3 axis;
    CSR_Vector3 factor;
    CSR_Matrix4 rotateXMatrix;
    CSR_Matrix4 rotateYMatrix;
    CSR_Matrix4 scaleMatrix;
    CSR_Matrix4 intermediateMatrix;

    csrMat4Identity(&g_Bot.m_Matrix);

    // set rotation axis
    axis.m_X = 1.0f;
    axis.m_Y = 0.0f;
    axis.m_Z = 0.0f;

    // create the rotation matrix
    csrMat4Rotate(float(-M_PI / 2.0), &axis, &rotateXMatrix);

    // set rotation axis
    axis.m_X = 0.0f;
    axis.m_Y = 1.0f;
    axis.m_Z = 0.0f;

    // create the rotation matrix
    csrMat4Rotate(g_Bot.m_Angle, &axis, &rotateYMatrix);

    // set scale factor
    factor.m_X = 0.005f;
    factor.m_Y = 0.005f;
    factor.m_Z = 0.005f;

    // create the scale matrix
    csrMat4Scale(&factor, &scaleMatrix);

    // build the model matrix
    csrMat4Multiply(&scaleMatrix,        &rotateXMatrix, &intermediateMatrix);
    csrMat4Multiply(&intermediateMatrix, &rotateYMatrix, &g_Bot.m_Matrix);
}
//---------------------------------------------------------------------------
void BuildFaderMatrix()
{
    CSR_Vector3 translation;
    CSR_Vector3 axis;
    CSR_Matrix4 translateMatrix;
    CSR_Matrix4 rotateYMatrix;
    float       angle;

    csrMat4Identity(&g_FaderMatrix);

    // calculate the fader angle
    angle = float(M_PI * 2.0) - g_Angle;

    // calculate the fader position
    translation.m_X = g_BoundingSphere.m_Center.m_X - (0.05f * sinf(angle));
    translation.m_Y = g_BoundingSphere.m_Center.m_Y;
    translation.m_Z = g_BoundingSphere.m_Center.m_Z - (0.05f * cosf(angle));

    // create the translate matrix
    csrMat4Translate(&translation, &translateMatrix);

    // set rotation axis
    axis.m_X = 0.0f;
    axis.m_Y = 1.0f;
    axis.m_Z = 0.0f;

    // create the rotation matrix
    csrMat4Rotate(angle, &axis, &rotateYMatrix);

    // build the final matrix
    csrMat4Multiply(&rotateYMatrix, &translateMatrix, &g_FaderMatrix);
}
//---------------------------------------------------------------------------
int ApplyGroundCollision(const CSR_Sphere* pBoundingSphere, CSR_Matrix4* pMatrix)
{
    if (!g_pScene)
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
    camera.m_yAngle       =  g_Angle;
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
    csrSceneDetectCollision(g_pScene, &collisionInput, &collisionOutput, 0);

    // update the ground position directly inside the matrix (this is where the final value is required)
    pMatrix->m_Table[3][1] = -collisionOutput.m_GroundPos;

    if (collisionOutput.m_Collision & CSR_CO_Ground)
        return 1;

    return 0;
}
//------------------------------------------------------------------------------
int CheckPlayerVisible()
{
    CSR_SceneItem*     pSceneItem;
    CSR_Vector3        botToPlayer;
    CSR_Vector3        botToPlayerDir;
    CSR_Polygon3Buffer polygons;
    float              angle;

    // calculate the bot to player vector
    botToPlayer.m_X = g_Bot.m_Geometry.m_Center.m_X - g_BoundingSphere.m_Center.m_X;
    botToPlayer.m_Y = g_Bot.m_Geometry.m_Center.m_Y - g_BoundingSphere.m_Center.m_Y;
    botToPlayer.m_Z = g_Bot.m_Geometry.m_Center.m_Z - g_BoundingSphere.m_Center.m_Z;

    // calculate the angle between the bot dir and the bot to player dir
    csrVec3Normalize(&botToPlayer, &botToPlayerDir);
    csrVec3Dot(&botToPlayerDir, &g_Bot.m_Dir, &angle);

    // reset the bot detection signal
    csrShaderEnable(g_pShader);
    glUniform1i(g_GrayscaleSlot, 0);

    // is bot showing the player?
    if (angle > -0.707f)
        return 0;

    // set the bot detection signal
    csrShaderEnable(g_pShader);
    glUniform1i(g_GrayscaleSlot, 1);

    return 1;
}
//------------------------------------------------------------------------------
int CheckBotHitPlayer()
{
    CSR_Figure3 firstSphere;
    CSR_Figure3 secondSphere;

    // get the player bounding object
    firstSphere.m_Type    = CSR_F3_Sphere;
    firstSphere.m_pFigure = &g_BoundingSphere;

    // get the bot bounding object
    secondSphere.m_Type    = CSR_F3_Sphere;
    secondSphere.m_pFigure = &g_Bot.m_Geometry;

    return csrIntersect3(&firstSphere, &secondSphere, 0, 0, 0);
}
//------------------------------------------------------------------------------
bool InitScene(int w, int h)
{
    CSR_VertexFormat vertexFormat;
    CSR_Material     material;
    CSR_Mesh*        pMesh;
    CSR_Model*       pModel;

    srand(0);

    // initialize the terrain limits
    g_TerrainLimits.m_Min.m_X = -3.0f;
    g_TerrainLimits.m_Min.m_Y = -3.0f;
    g_TerrainLimits.m_Max.m_X =  3.0f;
    g_TerrainLimits.m_Max.m_Y =  3.0f;

    // initialize the scene
    g_pScene = csrSceneCreate();

    // configure the scene background color
    g_pScene->m_Color.m_R = 0.45f;
    g_pScene->m_Color.m_G = 0.8f;
    g_pScene->m_Color.m_B = 1.0f;
    g_pScene->m_Color.m_A = 1.0f;

    // configure the scene ground direction
    g_pScene->m_GroundDir.m_X =  0.0f;
    g_pScene->m_GroundDir.m_Y = -1.0f;
    g_pScene->m_GroundDir.m_Z =  0.0f;

    // configure the scene view matrix
    csrMat4Identity(&g_pScene->m_ViewMatrix);

    // set the viewpoint bounding sphere default position
    g_BoundingSphere.m_Center.m_X = 0.0f;
    g_BoundingSphere.m_Center.m_Y = 0.0f;
    g_BoundingSphere.m_Center.m_Z = 0.0f;
    g_BoundingSphere.m_Radius     = 0.1f;

    // configure the scene context
    csrSceneContextInit(&g_SceneContext);
    g_SceneContext.m_fOnGetShader     = OnGetShader;
    g_SceneContext.m_fOnGetID         = OnGetID;
    g_SceneContext.m_fOnGetMDLIndex   = OnGetMDLIndex;
    g_SceneContext.m_fOnDeleteTexture = OnDeleteTexture;

    // compile, link and use shader
    g_pShader = csrOpenGLShaderLoadFromStr(&g_VSTextured[0],
                                            sizeof(g_VSTextured),
                                           &g_FSTextured[0],
                                            sizeof(g_FSTextured),
                                            0,
                                            0);

    // succeeded?
    if (!g_pShader)
        return false;

    csrShaderEnable(g_pShader);

    // create the viewport
    CSR_OpenGLHelper::CreateViewport((float)w,
                                     (float)h,
                                     0.01f,
                                     100.0f,
                                     g_pShader,
                                     g_pScene->m_ProjectionMatrix);

    // get shader attributes
    g_pShader->m_VertexSlot   = glGetAttribLocation (g_pShader->m_ProgramID, "csr_aVertices");
    g_pShader->m_ColorSlot    = glGetAttribLocation (g_pShader->m_ProgramID, "csr_aColor");
    g_pShader->m_TexCoordSlot = glGetAttribLocation (g_pShader->m_ProgramID, "csr_aTexCoord");
    g_pShader->m_TextureSlot  = glGetAttribLocation (g_pShader->m_ProgramID, "csr_sColorMap");
    g_GrayscaleSlot           = glGetUniformLocation(g_pShader->m_ProgramID, "csr_uGrayscale");
    g_AlphaSlot               = glGetUniformLocation(g_pShader->m_ProgramID, "csr_uAlpha");

    // configure OpenGL depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRangef(0.0f, 1.0f);

    // load the landscape
    if (!LoadLandscapeFromBitmap((g_SceneDir + LANDSCAPE_DATA_FILE).c_str()))
        return false;

    // get back the scene item containing the model
    CSR_SceneItem* pItem = csrSceneGetItem(g_pScene, g_pLandscapeKey);

    // found it?
    if (!pItem)
        return false;

    // load landscape texture
    CSR_OpenGLHelper::AddTexture(&((CSR_Model*)(pItem->m_pModel))->m_pMesh[0].m_Skin.m_Texture,
                                  LoadTexture((g_SceneDir + LANDSCAPE_TEXTURE_FILE).c_str()),
                                  g_OpenGLResources);

    g_Bot.m_Angle         = 0.0f;
    g_Bot.m_Velocity      = 0.5f;
    g_Bot.m_DyingSequence = E_DS_None;

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
    g_Bot.m_pModel = csrMDLOpen((g_SceneDir + MDL_FILE).c_str(),
                                0,
                                &vertexFormat,
                                0,
                                &material,
                                0,
                                OnApplySkin,
                                OnDeleteTexture);

    // add the model to the scene
    CSR_SceneItem* pSceneItem = csrSceneAddMDL(g_pScene, g_Bot.m_pModel, 1, 0);
    csrSceneAddModelMatrix(g_pScene, g_Bot.m_pModel, &g_Bot.m_Matrix);

    // succeeded?
    if (pSceneItem)
        pSceneItem->m_CollisionType = CSR_CO_Ground;

    // configure bot geometry
    g_Bot.m_Geometry.m_Center.m_X = 3.0f;
    g_Bot.m_Geometry.m_Center.m_Y = 0.0f;
    g_Bot.m_Geometry.m_Center.m_Z = 0.0f;
    g_Bot.m_Geometry.m_Radius     = 0.125f;

    // configure the vertex format
    vertexFormat.m_HasNormal         = 0;
    vertexFormat.m_HasTexCoords      = 1;
    vertexFormat.m_HasPerVertexColor = 1;

    // configure the material
    material.m_Color       = 0xFF0000FF;
    material.m_Transparent = 1;
    material.m_Wireframe   = 0;

    // create a surface for the fader
    g_pFader = csrShapeCreateSurface(100.0f, 100.0f, &vertexFormat, 0, &material, 0);

    // add the mesh to the scene
    pSceneItem = csrSceneAddMesh(g_pScene, g_pFader, 1, 0);
    csrSceneAddModelMatrix(g_pScene, g_pFader, &g_FaderMatrix);

    // initialize the fader matrix
    BuildFaderMatrix();

    const std::string vsSkybox = CSR_ShaderHelper::GetVertexShader  (CSR_ShaderHelper::IE_ST_Skybox);
    const std::string fsSkybox = CSR_ShaderHelper::GetFragmentShader(CSR_ShaderHelper::IE_ST_Skybox);

    // load the skybox shader
    g_pSkyboxShader = csrOpenGLShaderLoadFromStr(vsSkybox.c_str(),
                                                 vsSkybox.length(),
                                                 fsSkybox.c_str(),
                                                 fsSkybox.length(),
                                                 0,
                                                 0);

    // succeeded?
    if (!g_pSkyboxShader)
        return false;

    // enable the shader program
    csrShaderEnable(g_pSkyboxShader);

    // get shader attributes
    g_pSkyboxShader->m_VertexSlot  = glGetAttribLocation (g_pSkyboxShader->m_ProgramID, "csr_aVertices");
    g_pSkyboxShader->m_CubemapSlot = glGetUniformLocation(g_pSkyboxShader->m_ProgramID, "csr_sCubemap");

    // create the skybox
    g_pScene->m_pSkybox = csrSkyboxCreate(1.0f, 1.0f, 1.0f);

    // succeeded?
    if (!g_pScene->m_pSkybox)
        return false;

    IFileNames cubemapFileNames;
    cubemapFileNames.push_back(g_SceneDir + SKYBOX_RIGHT);
    cubemapFileNames.push_back(g_SceneDir + SKYBOX_LEFT);
    cubemapFileNames.push_back(g_SceneDir + SKYBOX_TOP);
    cubemapFileNames.push_back(g_SceneDir + SKYBOX_BOTTOM);
    cubemapFileNames.push_back(g_SceneDir + SKYBOX_FRONT);
    cubemapFileNames.push_back(g_SceneDir + SKYBOX_BACK);

    // load the cubemap texture
    CSR_OpenGLHelper::AddTexture(&g_pScene->m_pSkybox->m_Skin.m_CubeMap,
                                  LoadCubemap(cubemapFileNames),
                                  g_OpenGLResources);

    // load the sound files
    g_pFootStepLeftSound  = csrSoundOpenWavFile(g_pOpenALDevice, g_pOpenALContext, (g_SceneDir + FOOT_STEP_LEFT_SOUND_FILE).c_str());
    g_pFootStepRightSound = csrSoundOpenWavFile(g_pOpenALDevice, g_pOpenALContext, (g_SceneDir + FOOT_STEP_RIGHT_SOUND_FILE).c_str());
    g_pHitSound           = csrSoundOpenWavFile(g_pOpenALDevice, g_pOpenALContext, (g_SceneDir + HIT_SOUND_FILE).c_str());

    // change the volume
    csrSoundChangeVolume(g_pFootStepLeftSound,  0.2f);
    csrSoundChangeVolume(g_pFootStepRightSound, 0.2f);

    // initialize the IA task manager
    g_pTaskManager = csrTaskManagerCreate();
    csrTaskManagerInit(g_pTaskManager);
    g_pTaskManager->m_pTask = csrTaskCreate();
    g_pTaskManager->m_Count = 1;

    // initialize the bot task
    csrTaskInit(g_pTaskManager->m_pTask);
    g_pTaskManager->m_pTask->m_Action = E_BT_Watching;

    // initialize the IA task manager context
    g_TaskContext.m_fOnTaskChange = OnTaskChange;
    g_TaskContext.m_fOnTaskRun    = OnTaskRun;

    // prepare the first bot task
    OnPrepareWatching();

    g_Initialized = true;

    return true;
}
//------------------------------------------------------------------------------
void DeleteScene()
{
    g_Initialized = false;

    // delete the task manager
    csrTaskManagerRelease(g_pTaskManager);

    // delete the scene
    csrSceneRelease(g_pScene, OnDeleteTexture);

    // delete scene shader
    csrOpenGLShaderRelease(g_pShader);

    // delete skybox shader
    csrOpenGLShaderRelease(g_pSkyboxShader);

    // stop running step sound, if needed
    csrSoundStop(g_pFootStepLeftSound);
    csrSoundStop(g_pFootStepRightSound);
    csrSoundStop(g_pHitSound);

    // release OpenAL interface
    csrSoundRelease(g_pFootStepLeftSound);
    csrSoundRelease(g_pFootStepRightSound);
    csrSoundRelease(g_pHitSound);
}
//------------------------------------------------------------------------------
void UpdateScene(float elapsedTime)
{
    float          posY;
    float          angle;
    CSR_Sphere     prevSphere;
    CSR_SceneItem* pSceneItem;

    // check if the bot hit the player
    g_BotHitPlayer = CheckBotHitPlayer();

    // run the IA
    csrTaskManagerExecute(g_pTaskManager, &g_TaskContext, elapsedTime);

    // rebuild the bot matrix
    BuildBotMatrix();

    pSceneItem = csrSceneGetItem(g_pScene, g_pLandscapeKey);
    posY       = M_CSR_NoGround;

    // calculate the y position where to place the bot
    csrGroundPosY(&g_Bot.m_Geometry, pSceneItem->m_pAABBTree, &g_pScene->m_GroundDir, 0, &posY);

    // set the bot y position directly inside the matrix
    g_Bot.m_Matrix.m_Table[3][0] = g_Bot.m_Geometry.m_Center.m_X;
    g_Bot.m_Matrix.m_Table[3][1] = posY;
    g_Bot.m_Matrix.m_Table[3][2] = g_Bot.m_Geometry.m_Center.m_Z;

    // calculate next model indexes to show
    if (g_Bot.m_DyingSequence != E_DS_FadeOut)
        csrMDLUpdateIndex(g_Bot.m_pModel,
                          (std::size_t)g_FPS,
                          g_AnimIndex,
                         &g_TextureIndex,
                         &g_ModelIndex,
                         &g_MeshIndex,
                         &g_TextureLastTime,
                         &g_ModelLastTime,
                         &g_MeshLastTime,
                          elapsedTime * 0.5f);

    // is player dying?
    if (g_PlayerDying)
    {
        // reset the bot data
        g_pTaskManager->m_pTask->m_Action = E_BT_Watching;
        g_BotShowPlayer                   = 0;
        glUniform1i(g_GrayscaleSlot, 0);

        // fade to red
        g_FaderAlpha += 1.0f * elapsedTime;

        // fader max value reached?
        if (g_FaderAlpha >= 1.0f)
        {
            g_FaderAlpha = 1.0f;

            // get a new player position
            g_BoundingSphere.m_Center.m_X = -3.0f + ((rand() % 600) / 100.0f);
            g_BoundingSphere.m_Center.m_Z = -3.0f + ((rand() % 600) / 100.0f);

            g_PlayerDying = 0;
        }

        return;
    }

    // fade out
    if (g_FaderAlpha > 0.0f)
        g_FaderAlpha -= 1.0f * elapsedTime;
    else
        g_FaderAlpha = 0.0f;

    // is player rotating?
    if (g_DirVelocity)
    {
        // calculate the player direction
        g_Angle += g_DirVelocity * elapsedTime;

        // validate and apply it
        if (g_Angle > float(M_PI * 2.0))
            g_Angle -= float(M_PI * 2.0);
        else
        if (g_Angle < 0.0f)
            g_Angle += float(M_PI * 2.0);
    }

    prevSphere = g_BoundingSphere;

    // is player moving?
    if (g_PosVelocity)
    {
        // calculate the next player position
        g_BoundingSphere.m_Center.m_X += g_PosVelocity * cosf(g_Angle + float(M_PI * 0.5)) * elapsedTime;
        g_BoundingSphere.m_Center.m_Z += g_PosVelocity * sinf(g_Angle + float(M_PI * 0.5)) * elapsedTime;

        // calculate the ground position and check if next position is valid
        if (!ApplyGroundCollision(&g_BoundingSphere, &g_pScene->m_ViewMatrix))
        {
            // invalid next position, get the scene item (just one for this scene)
            const CSR_SceneItem* pItem = csrSceneGetItem(g_pScene, g_pLandscapeKey);

            // found it?
            if (pItem)
            {
                // check if the x position is out of bounds, and correct it if yes
                if (g_BoundingSphere.m_Center.m_X <= pItem->m_pAABBTree->m_pBox->m_Min.m_X ||
                    g_BoundingSphere.m_Center.m_X >= pItem->m_pAABBTree->m_pBox->m_Max.m_X)
                    g_BoundingSphere.m_Center.m_X  = prevSphere.m_Center.m_X;

                // do the same thing with the z position. Doing that separately for each axis will make
                // the point of view to slide against the landscape border (this is possible because the
                // landscape is axis-aligned)
                if (g_BoundingSphere.m_Center.m_Z <= pItem->m_pAABBTree->m_pBox->m_Min.m_Z ||
                    g_BoundingSphere.m_Center.m_Z >= pItem->m_pAABBTree->m_pBox->m_Max.m_Z)
                    g_BoundingSphere.m_Center.m_Z  = prevSphere.m_Center.m_Z;
            }
            else
                // failed to get the scene item, just revert the position
                g_BoundingSphere.m_Center = prevSphere.m_Center;

            // recalculate the ground value (this time the collision result isn't tested, because the
            // previous position is always considered as valid)
            ApplyGroundCollision(&g_BoundingSphere, &g_pScene->m_ViewMatrix);
        }

        // calculate next time where the step sound should be played
        g_StepTime += (elapsedTime * 1000.0f);

        // count frames
        while (g_StepTime > g_StepInterval)
        {
            // do play the left or right footstep sound?
            if (!(g_AlternateStep % 2))
                csrSoundPlay(g_pFootStepLeftSound);
            else
                csrSoundPlay(g_pFootStepRightSound);

            g_StepTime = 0.0f;

            // next time the other footstep sound will be played
            g_AlternateStep = (g_AlternateStep + 1) & 1;
        }
    }

    // rebuild the fader matrix
    BuildFaderMatrix();
}
//------------------------------------------------------------------------------
void DrawScene()
{
    // finalize the view matrix
    ApplyGroundCollision(&g_BoundingSphere, &g_pScene->m_ViewMatrix);

    // draw the scene
    csrSceneDraw(g_pScene, &g_SceneContext);
}
//------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR     lpCmdLine,
                   int       nCmdShow)
{
    WNDCLASSEX wcex;
    HWND       hWnd;
    MSG        msg;
    BOOL       bQuit = FALSE;

    // try to load application icon from resources
    HICON hIcon = (HICON)LoadImage(GetModuleHandle(NULL),
                                   MAKEINTRESOURCE(IDI_MAIN_ICON),
                                   IMAGE_ICON,
                                   16,
                                   16,
                                   0);

    // register the window class
    wcex.cbSize        = sizeof(WNDCLASSEX);
    wcex.style         = CS_OWNDC;
    wcex.lpfnWndProc   = WindowProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = hInstance;
    wcex.hIcon         = hIcon;
    wcex.hIconSm       = hIcon;
    wcex.hCursor       = ::LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName  = NULL;
    wcex.lpszClassName = "CSR_BotDemo";

    if (!RegisterClassEx(&wcex))
        return 0;

    // create the main window
    hWnd = ::CreateWindowEx(0,
                            "CSR_BotDemo",
                            "Bot demo",
                            WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            800,
                            650,
                            NULL,
                            NULL,
                            hInstance,
                            NULL);

    ::ShowWindow(hWnd, nCmdShow);

    g_SceneDir = "Resources";

    // initialize OpenAL
    csrSoundInitializeOpenAL(&g_pOpenALDevice, &g_pOpenALContext);

    // enable OpenGL
    CSR_OpenGLHelper::EnableOpenGL(hWnd, &g_hDC, &g_hRC);

    // stop GLEW crashing on OSX :-/
    glewExperimental = GL_TRUE;

    // initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        // shutdown OpenGL
        CSR_OpenGLHelper::DisableOpenGL(hWnd, g_hDC, g_hRC);

        // close the app
        return 0;
    }

    RECT clientRect;
    ::GetClientRect(hWnd, &clientRect);

    // initialize the scene
    if (!InitScene(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top))
    {
        // shutdown OpenGL
        CSR_OpenGLHelper::DisableOpenGL(hWnd, g_hDC, g_hRC);

        // close the app
        return 0;
    }

    // initialize the timer
    g_StartTime    = ::GetTickCount();
    g_PreviousTime = ::GetTickCount();

    // application main loop
    while (!bQuit)
    {
        // check for messages
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // handle or dispatch messages
            if (msg.message == WM_QUIT)
                bQuit = TRUE;
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            // calculate time interval
            const unsigned __int64 now            =  ::GetTickCount();
            const double           elapsedTime    = (now - g_PreviousTime) / 1000.0;
                                   g_PreviousTime =  now;

            if (!g_Initialized)
                continue;

            ++g_FrameCount;

            // calculate the FPS
            if (g_FrameCount >= 100)
            {
                const double      smoothing = 0.1;
                const std::size_t fpsTime   = (std::size_t)(now > g_StartTime ? now - g_StartTime : 1L);
                const std::size_t newFPS    = (g_FrameCount * 100) / fpsTime;
                g_StartTime                 = ::GetTickCount();
                g_FrameCount                = 0;
                g_FPS                       = (newFPS * smoothing) + (g_FPS * (1.0 - smoothing));
            }

            UpdateScene((float)elapsedTime);
            DrawScene();

            ::SwapBuffers(g_hDC);

            ::Sleep(1);
        }
    }

    // delete the scene completely
    DeleteScene();

    // delete the scene textures
    CSR_OpenGLHelper::ClearResources(g_OpenGLResources);

    // shutdown OpenGL
    CSR_OpenGLHelper::DisableOpenGL(hWnd, g_hDC, g_hRC);

    // release OpenAL interface
    csrSoundReleaseOpenAL(g_pOpenALDevice, g_pOpenALContext);

    // destroy the window explicitly
    DestroyWindow(hWnd);

    return (int)msg.wParam;
}
//------------------------------------------------------------------------------
