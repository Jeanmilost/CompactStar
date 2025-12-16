/****************************************************************************
 * ==> Wild soccer ---------------------------------------------------------*
 ****************************************************************************
 * Description : Wild soccer game demo                                      *
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
#include "CSR_Wavefront.h"
#include "CSR_Scene.h"
#include "CSR_Physics.h"
#include "CSR_Renderer_OpenGL.h"
#include "CSR_Sound.h"

// classes
#include "CSR_OpenGLHelper.h"
#include "CSR_ShaderHelper.h"

// windows
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// resources
#include "resource.h"

// resource files to load
#define YOU_WON_TEXTURE_FILE       "\\you_won.bmp"
#define LANDSCAPE_TEXTURE_FILE     "\\soccer_grass.bmp"
#define BALL_TEXTURE_FILE          "\\soccer_ball.bmp"
#define GOAL_TEXTURE_FILE          "\\soccer_goal.bmp"
#define SOCCER_GOAL_MODEL          "\\soccer_goal.obj"
#define LANDSCAPE_DATA_FILE        "\\level.bmp"
#define SKYBOX_LEFT                "\\left.bmp"
#define SKYBOX_TOP                 "\\top.bmp"
#define SKYBOX_RIGHT               "\\right.bmp"
#define SKYBOX_BOTTOM              "\\bottom.bmp"
#define SKYBOX_FRONT               "\\front.bmp"
#define SKYBOX_BACK                "\\back.bmp"
#define FOOT_STEP_LEFT_SOUND_FILE  "\\footstep_left.wav"
#define FOOT_STEP_RIGHT_SOUND_FILE "\\footstep_right.wav"
#define BALL_KICK_SOUND_FILE       "\\soccer_ball_kick.wav"

//---------------------------------------------------------------------------
// Global defines
//---------------------------------------------------------------------------
#define M_ShootEnergyFactor 23.5f // energy factor for the shoot
//------------------------------------------------------------------------------
// Structures
//------------------------------------------------------------------------------
/**
* Structure representing a physical ball
*/
typedef struct
{
    void*       m_pKey;
    CSR_Matrix4 m_Matrix;
    CSR_Sphere  m_Geometry;
    CSR_Body    m_Body;
} CSR_Ball;
//------------------------------------------------------------------------------
/**
* Structure representing the goal
*/
typedef struct
{
    void*       m_pKey;
    CSR_Matrix4 m_Matrix;
    CSR_Rect    m_Bounds;
} CSR_Goal;
//----------------------------------------------------------------------------
typedef std::vector<std::string> IFileNames;
//------------------------------------------------------------------------------
HDC                          g_hDC                 = 0;
HGLRC                        g_hRC                 = 0;
ALCdevice*                   g_pOpenALDevice       = nullptr;
ALCcontext*                  g_pOpenALContext      = nullptr;
CSR_Scene*                   g_pScene              = nullptr;
CSR_SceneContext             g_SceneContext;
CSR_OpenGLShader*            g_pShader             = nullptr;
CSR_OpenGLShader*            g_pSkyboxShader       = nullptr;
CSR_OpenGLHelper::IResources g_OpenGLResources;
CSR_Sphere                   g_ViewSphere;
CSR_Ball                     g_Ball;
CSR_Goal                     g_Goal;
CSR_Matrix4                  g_LandscapeMatrix;
CSR_Matrix4                  g_YouWonMatrix;
CSR_Sound*                   g_pFootStepLeftSound  = nullptr;
CSR_Sound*                   g_pFootStepRightSound = nullptr;
CSR_Sound*                   g_pBallKickSound      = nullptr;
void*                        g_pLandscapeKey       = nullptr;
std::string                  g_SceneDir;
std::size_t                  g_FrameCount          = 0;
int                          g_PrevOrigin          = 0;
int                          g_AlternateStep       = 0;
float                        g_Angle               = (float)(M_PI / -4.0);
float                        g_RollAngle           = 0.0f;
float                        g_BallDirAngle        = 0.0f;
float                        g_BallOffset          = 0.0f;
float                        g_PosVelocity         = 0.0f;
float                        g_DirVelocity         = 0.0f;
float                        g_StepTime            = 0.0f;
float                        g_StepInterval        = 300.0f;
double                       g_FPS                 = 0.0;
unsigned __int64             g_StartTime           = 0L;
unsigned __int64             g_PreviousTime        = 0L;
bool                         g_Initialized         = false;
//------------------------------------------------------------------------------
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
                case VK_UP:    g_PosVelocity =  1.0f; break;
                case VK_DOWN:  g_PosVelocity = -1.0f; break;

                case VK_SPACE:
                {
                    // get the player position as a circle (i.e. ignore the y axis)
                    CSR_Circle playerCircle   = {};
                    playerCircle.m_Center.m_X = g_ViewSphere.m_Center.m_X;
                    playerCircle.m_Center.m_Y = g_ViewSphere.m_Center.m_Z;
                    playerCircle.m_Radius     = g_ViewSphere.m_Radius + 0.15f;

                    // get the ball position as a circle (i.e. ignore the y axis)
                    CSR_Circle ballCircle   = {};
                    ballCircle.m_Center.m_X = g_Ball.m_Geometry.m_Center.m_X;
                    ballCircle.m_Center.m_Y = g_Ball.m_Geometry.m_Center.m_Z;
                    ballCircle.m_Radius     = g_Ball.m_Geometry.m_Radius;

                    CSR_Figure2 f1 = {};
                    f1.m_Type      = CSR_F2_Circle;
                    f1.m_pFigure   = &playerCircle;

                    CSR_Figure2 f2 = {};
                    f2.m_Type      = CSR_F2_Circle;
                    f2.m_pFigure   = &ballCircle;

                    // check if the player is closer enough to the ball to shoot it
                    if (csrIntersect2(&f1, &f2, 0, 0))
                    {
                        // calculate the direction and intensity of the shoot
                        CSR_Vector2 distance;
                        csrVec2Sub(&ballCircle.m_Center, &playerCircle.m_Center, &distance);

                        // shoot the ball
                        g_Ball.m_Body.m_Velocity.m_X = M_ShootEnergyFactor * distance.m_X;
                        g_Ball.m_Body.m_Velocity.m_Y = 0.0f;
                        g_Ball.m_Body.m_Velocity.m_Z = M_ShootEnergyFactor * distance.m_Y;

                        // play the kick sound
                        csrSoundStop(g_pBallKickSound);
                        csrSoundPlay(g_pBallKickSound);
                    }

                    break;
                }

                case 'R':
                    // reset the simulation
                    csrMat4Identity(&g_Ball.m_Matrix);
                    g_Ball.m_Geometry.m_Center.m_X = 0.0f;
                    g_Ball.m_Geometry.m_Center.m_Y = 0.0f;
                    g_Ball.m_Geometry.m_Center.m_Z = 0.0f;
                    g_Ball.m_Body.m_Velocity.m_X   = 0.0f;
                    g_Ball.m_Body.m_Velocity.m_Y   = 0.0f;
                    g_Ball.m_Body.m_Velocity.m_Z   = 0.0f;
                    g_BallDirAngle                 = 0.0f;
                    g_RollAngle                    = 0.0f;
                    break;
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

    return g_pShader;
}
//---------------------------------------------------------------------------
void OnSceneBegin(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    csrDrawBegin(&pScene->m_Color);
}
//---------------------------------------------------------------------------
void OnSceneEnd(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    csrDrawEnd();
}
//---------------------------------------------------------------------------
void* OnGetID(const void* pKey)
{
    return CSR_OpenGLHelper::GetTextureID(pKey, g_OpenGLResources);
}
//---------------------------------------------------------------------------
void OnDeleteTexture(const CSR_Texture* pTexture)
{
    return CSR_OpenGLHelper::DeleteTexture(pTexture, g_OpenGLResources);
}
//---------------------------------------------------------------------------
int LoadLandscapeFromBitmap(const char* pFileName)
{
    if (!pFileName)
        return 0;

    CSR_Material      material = {};
    CSR_VertexCulling vc       = {};
    CSR_VertexFormat  vf       = {};
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
    pModel->m_pMesh = csrLandscapeCreate(pBitmap, 3.0f, 0.2f, &vf, &vc, &material, 0);
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
            GLuint         index = 0;

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

            if (!pPixels)
                return M_CSR_Error_Code;

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
int ApplyGroundCollision(const CSR_Sphere*  pBoundingSphere,
                               float        dir,
                               CSR_Matrix4* pMatrix,
                               CSR_Plane*   pGroundPlane)
{
    if (!g_pScene)
        return 0;

    // validate the inputs
    if (!pBoundingSphere || !pMatrix)
        return 0;

    CSR_CollisionInput collisionInput;
    csrCollisionInputInit(&collisionInput);
    collisionInput.m_BoundingSphere.m_Radius = pBoundingSphere->m_Radius;

    CSR_Camera camera = {};

    // calculate the camera position in the 3d world, without the ground value
    camera.m_Position.m_X = -pBoundingSphere->m_Center.m_X;
    camera.m_Position.m_Y =  0.0f;
    camera.m_Position.m_Z = -pBoundingSphere->m_Center.m_Z;
    camera.m_xAngle       =  0.0f;
    camera.m_yAngle       =  dir;
    camera.m_zAngle       =  0.0f;
    camera.m_Factor.m_X   =  1.0f;
    camera.m_Factor.m_Y   =  1.0f;
    camera.m_Factor.m_Z   =  1.0f;
    camera.m_MatCombType  =  IE_CT_Translate_Scale_Rotate;

    // get the view matrix matching with the camera
    csrSceneCameraToMatrix(&camera, pMatrix);

    CSR_Vector3 modelCenter = {};

    // get the model center
    modelCenter.m_X = 0.0f;
    modelCenter.m_Y = 0.0f;
    modelCenter.m_Z = 0.0f;

    CSR_Matrix4 invertMatrix;
    float       determinant;

    // calculate the current camera position above the landscape
    csrMat4Inverse(pMatrix, &invertMatrix, &determinant);
    csrMat4Transform(&invertMatrix, &modelCenter, &collisionInput.m_BoundingSphere.m_Center);
    collisionInput.m_CheckPos = collisionInput.m_BoundingSphere.m_Center;

    CSR_CollisionOutput collisionOutput;

    // calculate the collisions in the whole scene
    csrSceneDetectCollision(g_pScene, &collisionInput, &collisionOutput, 0);

    // update the ground position directly inside the matrix (this is where the final value is required)
    pMatrix->m_Table[3][1] = -collisionOutput.m_GroundPos;

    // get the resulting plane
    *pGroundPlane = collisionOutput.m_GroundPlane;

    if (collisionOutput.m_Collision & CSR_CO_Ground)
        return 1;

    return 0;
}
//---------------------------------------------------------------------------
bool CheckForGoal(CSR_Ball* pBall, const CSR_Vector3* pOldPos, const CSR_Vector3* pDir)
{
    if (!pBall || !pDir)
        return false;

    if (!pDir->m_X && !pDir->m_Y && !pDir->m_Z)
        return false;

    // is ball hitting the goal?
    if (pBall->m_Geometry.m_Center.m_X >= g_Goal.m_Bounds.m_Min.m_X &&
        pBall->m_Geometry.m_Center.m_X <= g_Goal.m_Bounds.m_Max.m_X &&
        pBall->m_Geometry.m_Center.m_Z >= g_Goal.m_Bounds.m_Min.m_Y &&
        pBall->m_Geometry.m_Center.m_Z <= g_Goal.m_Bounds.m_Max.m_Y)
    {
        // player hit the goal
        // a       b
        // |-------|
        // |       |
        // |       |
        // |       |
        // |       |
        // |       |
        // |       |
        // |       |
        // |-------|
        // d       c
        CSR_Segment3 ab = {};
        CSR_Segment3 bc = {};
        CSR_Segment3 cd = {};
        CSR_Segment3 da = {};

        // build the ab segment
        ab.m_Start.m_X = g_Goal.m_Bounds.m_Min.m_X;
        ab.m_Start.m_Y = 0.0f;
        ab.m_Start.m_Z = g_Goal.m_Bounds.m_Min.m_Y;
        ab.m_End.m_X   = g_Goal.m_Bounds.m_Max.m_X;
        ab.m_End.m_Y   = 0.0f;
        ab.m_End.m_Z   = g_Goal.m_Bounds.m_Min.m_Y;

        // build the bc segment
        bc.m_Start.m_X = g_Goal.m_Bounds.m_Max.m_X;
        bc.m_Start.m_Y = 0.0f;
        bc.m_Start.m_Z = g_Goal.m_Bounds.m_Min.m_Y;
        bc.m_End.m_X   = g_Goal.m_Bounds.m_Max.m_X;
        bc.m_End.m_Y   = 0.0f;
        bc.m_End.m_Z   = g_Goal.m_Bounds.m_Max.m_Y;

        // build the cd segment
        cd.m_Start.m_X = g_Goal.m_Bounds.m_Max.m_X;
        cd.m_Start.m_Y = 0.0f;
        cd.m_Start.m_Z = g_Goal.m_Bounds.m_Max.m_Y;
        cd.m_End.m_X   = g_Goal.m_Bounds.m_Min.m_X;
        cd.m_End.m_Y   = 0.0f;
        cd.m_End.m_Z   = g_Goal.m_Bounds.m_Max.m_Y;

        // build the da segment
        da.m_Start.m_X = g_Goal.m_Bounds.m_Min.m_X;
        da.m_Start.m_Y = 0.0f;
        da.m_Start.m_Z = g_Goal.m_Bounds.m_Max.m_Y;
        da.m_End.m_X   = g_Goal.m_Bounds.m_Min.m_X;
        da.m_End.m_Y   = 0.0f;
        da.m_End.m_Z   = g_Goal.m_Bounds.m_Min.m_Y;

        CSR_Vector3 ptAB;
        CSR_Vector3 ptBC;
        CSR_Vector3 ptCD;
        CSR_Vector3 ptDA;

        // calculate the closest point from previous position to each of the segments
        csrSeg3ClosestPoint(&ab, pOldPos, &ptAB);
        csrSeg3ClosestPoint(&bc, pOldPos, &ptBC);
        csrSeg3ClosestPoint(&cd, pOldPos, &ptCD);
        csrSeg3ClosestPoint(&da, pOldPos, &ptDA);

        CSR_Vector3 PPtAB;
        CSR_Vector3 PPtBC;
        CSR_Vector3 PPtCD;
        CSR_Vector3 PPtDA;

        // calculate each distances between the previous point and each points found on segments
        csrVec3Sub(&ptAB, pOldPos, &PPtAB);
        csrVec3Sub(&ptBC, pOldPos, &PPtBC);
        csrVec3Sub(&ptCD, pOldPos, &PPtCD);
        csrVec3Sub(&ptDA, pOldPos, &PPtDA);

        float lab;
        float lbc;
        float lcd;
        float lda;

        // calculate each lengths between the previous point and each points found on segments
        csrVec3Length(&PPtAB, &lab);
        csrVec3Length(&PPtBC, &lbc);
        csrVec3Length(&PPtCD, &lcd);
        csrVec3Length(&PPtDA, &lda);

        // find on which side the player is hitting the goal
        if (lab < lbc && lab < lcd && lab < lda)
            pBall->m_Body.m_Velocity.m_Z = -pBall->m_Body.m_Velocity.m_Z;
        else
        if (lbc < lab && lbc < lcd && lbc < lda)
            pBall->m_Body.m_Velocity.m_X = -pBall->m_Body.m_Velocity.m_X;
        else
        if (lcd < lab && lcd < lbc && lcd < lda)
        {
            pBall->m_Body.m_Velocity.m_Z = -pBall->m_Body.m_Velocity.m_Z;
            return true;
        }
        else
        if (lda < lab && lda < lbc && lda < lcd)
            pBall->m_Body.m_Velocity.m_X = -pBall->m_Body.m_Velocity.m_X;
        else
        {
            pBall->m_Body.m_Velocity.m_X = -pBall->m_Body.m_Velocity.m_X;
            pBall->m_Body.m_Velocity.m_Z = -pBall->m_Body.m_Velocity.m_Z;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
void ApplyPhysics(float elapsedTime)
{
    CSR_Plane   groundPlane  = {};
    CSR_Vector3 acceleration = {};
    CSR_Vector3 prevCenter   = {};
    CSR_Vector3 ballDir      = {};
    CSR_Vector3 ballDirN     = {};

    // apply the ground collision on the current position and get the ground polygon
    ApplyGroundCollision(&g_Ball.m_Geometry, 0.0f, &g_Ball.m_Matrix, &groundPlane);

    CSR_Vector3 planeNormal = {};

    // get the normal of the plane
    planeNormal.m_X = groundPlane.m_A;
    planeNormal.m_Y = groundPlane.m_B;
    planeNormal.m_Z = groundPlane.m_C;

    // calculate the next ball roll position
    csrPhysicsRoll(&planeNormal, g_Ball.m_Body.m_Mass, 0.0025f, elapsedTime, &g_Ball.m_Body.m_Velocity);

    // keep the previous ball position
    prevCenter = g_Ball.m_Geometry.m_Center;

    // calculate the new position (using the formula pos = pos + (v * dt))
    g_Ball.m_Geometry.m_Center.m_X += g_Ball.m_Body.m_Velocity.m_X * elapsedTime;
    g_Ball.m_Geometry.m_Center.m_Y += g_Ball.m_Body.m_Velocity.m_Y * elapsedTime;
    g_Ball.m_Geometry.m_Center.m_Z += g_Ball.m_Body.m_Velocity.m_Z * elapsedTime;

    // check if the new position is valid
    if (!ApplyGroundCollision(&g_Ball.m_Geometry, 0.0f, &g_Ball.m_Matrix, &groundPlane))
    {
        // do perform a rebound on the x or z axis?
        const bool xRebound = g_Ball.m_Geometry.m_Center.m_X <= -3.08f || g_Ball.m_Geometry.m_Center.m_X >= 3.08f;
        const bool zRebound = g_Ball.m_Geometry.m_Center.m_Z <= -3.08f || g_Ball.m_Geometry.m_Center.m_Z >= 3.08f;

        // reset the ball to the previous position
        g_Ball.m_Geometry.m_Center = prevCenter;

        // do perform a rebound on the x axis?
        if (xRebound)
            g_Ball.m_Body.m_Velocity.m_X = -g_Ball.m_Body.m_Velocity.m_X;

        // do perform a rebound on the z axis?
        if (zRebound)
            g_Ball.m_Body.m_Velocity.m_Z = -g_Ball.m_Body.m_Velocity.m_Z;
    }
    else
    {
        float       distance;
        CSR_Matrix4 rxMatrix;
        CSR_Matrix4 ryMatrix;
        CSR_Matrix4 ballMatrix;
        CSR_Vector3 rollDistance;
        CSR_Vector3 rollDir;

        // calculate the rolling angle (depends on the distance the ball moved)
        csrVec3Sub(&g_Ball.m_Geometry.m_Center, &prevCenter, &rollDistance);
        csrVec3Length(&rollDistance, &distance);
        g_RollAngle = fmodf(float(g_RollAngle + (distance * 10.0f)), float(M_PI * 2.0f));

        CSR_Vector3 axis = {};
        axis.m_X         = 1.0f;
        axis.m_Y         = 0.0f;
        axis.m_Z         = 0.0f;

        // the ball moved since the last frame?
        if (distance)
        {
            // calculate the new ball direction angle
            csrVec3Normalize(&rollDistance, &rollDir);
            csrVec3Dot(&rollDir, &axis, &g_BallDirAngle);
        }

        // calculate the rotation matrix on the x axis
        csrMat4Rotate(g_RollAngle, &axis, &rxMatrix);

        axis.m_X = 0.0f;
        axis.m_Y = 1.0f;
        axis.m_Z = 0.0f;

        // ball moved on the z axis since the last frame?
        if (rollDistance.m_Z)
            // calculate the offset to apply to the ball direction
            g_BallOffset = (rollDistance.m_Z > 0.0f ? 1.0f : -1.0f);

        // calculate the rotation matrix on the y axis
        csrMat4Rotate((float)(M_PI * 2.0) - ((acosf(g_BallDirAngle) * g_BallOffset) - (float)(M_PI / 2.0)),
                      &axis,
                      &ryMatrix);

        // build the final matrix
        csrMat4Multiply(&rxMatrix, &ryMatrix, &ballMatrix);

        // replace the ball in the model coordinate system (do that directly on the matrix)
        ballMatrix.m_Table[3][0] = -g_Ball.m_Matrix.m_Table[3][0];
        ballMatrix.m_Table[3][1] = -g_Ball.m_Matrix.m_Table[3][1];
        ballMatrix.m_Table[3][2] = -g_Ball.m_Matrix.m_Table[3][2];

        g_Ball.m_Matrix = ballMatrix;
    }

    // calculate the ball direction
    csrVec3Sub(&g_Ball.m_Geometry.m_Center, &prevCenter, &ballDir);
    csrVec3Normalize(&ballDir, &ballDirN);

    // check if the goal was hit
    if (CheckForGoal(&g_Ball, &prevCenter, &ballDirN))
        g_YouWonMatrix.m_Table[3][1] = 1.375f;
}
//------------------------------------------------------------------------------
bool InitScene(int w, int h)
{
    // create the default scene
    g_pScene = csrSceneCreate();

    // create the scene context
    csrSceneContextInit(&g_SceneContext);
    g_SceneContext.m_fOnSceneBegin = OnSceneBegin;
    g_SceneContext.m_fOnSceneEnd   = OnSceneEnd;
    g_SceneContext.m_fOnGetShader  = OnGetShader;
    g_SceneContext.m_fOnGetID      = OnGetID;

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
    g_ViewSphere.m_Center.m_X = 3.08f;
    g_ViewSphere.m_Center.m_Y = 0.0f;
    g_ViewSphere.m_Center.m_Z = 3.08f;
    g_ViewSphere.m_Radius     = 0.1f;

    // set the ball bounding sphere default position
    g_Ball.m_Geometry.m_Center.m_X = 0.0f;
    g_Ball.m_Geometry.m_Center.m_Y = 0.0f;
    g_Ball.m_Geometry.m_Center.m_Z = 0.0f;
    g_Ball.m_Geometry.m_Radius     = 0.025f;
    csrBodyInit(&g_Ball.m_Body);

    const std::string vsTextured = CSR_ShaderHelper::GetVertexShader  (CSR_ShaderHelper::IEShaderType::IE_ST_Texture);
    const std::string fsTextured = CSR_ShaderHelper::GetFragmentShader(CSR_ShaderHelper::IEShaderType::IE_ST_Texture);

    // load the shader
    g_pShader = csrOpenGLShaderLoadFromStr(vsTextured.c_str(),
                                           vsTextured.length(),
                                           fsTextured.c_str(),
                                           fsTextured.length(),
                                           0,
                                           0);

    // succeeded?
    if (!g_pShader)
        return false;

    // update the viewport
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
    g_pShader->m_TextureSlot  = glGetUniformLocation(g_pShader->m_ProgramID, "csr_sTexture");

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

    CSR_VertexFormat vertexFormat    = {};
    vertexFormat.m_HasNormal         = 0;
    vertexFormat.m_HasPerVertexColor = 1;
    vertexFormat.m_HasTexCoords      = 1;

    CSR_Material material  = {};
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    // create the ball
    CSR_Mesh* pMesh = csrShapeCreateSphere(g_Ball.m_Geometry.m_Radius,
                                           20,
                                           20,
                                          &vertexFormat,
                                           0,
                                          &material,
                                           0);

    // load ball texture
    CSR_OpenGLHelper::AddTexture(&pMesh->m_Skin.m_Texture,
                                  LoadTexture((g_SceneDir + BALL_TEXTURE_FILE).c_str()),
                                  g_OpenGLResources);

    // add the mesh to the scene
    CSR_SceneItem* pSceneItem = csrSceneAddMesh(g_pScene, pMesh, 0, 1);
    csrSceneAddModelMatrix(g_pScene, pMesh, &g_Ball.m_Matrix);

    // configure the ball particle
    g_Ball.m_pKey        = pSceneItem->m_pModel;
    g_Ball.m_Body.m_Mass = 0.3f;

    // create the goal
    CSR_Model* pModel = csrWaveFrontOpen((g_SceneDir + SOCCER_GOAL_MODEL).c_str(),
                                         &vertexFormat,
                                          0,
                                         &material,
                                          0,
                                          0,
                                          0);

    CSR_Vector3 translation = {};
    translation.m_X         =  0.0f;
    translation.m_Y         =  1.375f;
    translation.m_Z         = -1.75f;

    CSR_Matrix4 translationMatrix;

    // apply translation to goal
    csrMat4Translate(&translation, &translationMatrix);

    CSR_Vector3 axis = {};
    axis.m_X         = 0.0f;
    axis.m_Y         = 1.0f;
    axis.m_Z         = 0.0f;

    CSR_Matrix4 ryMatrix;

    // apply rotation on y axis to goal
    csrMat4Rotate((float)M_PI, &axis, &ryMatrix);

    CSR_Vector3 factor = {};
    factor.m_X         = 0.0025f;
    factor.m_Y         = 0.0025f;
    factor.m_Z         = 0.0025f;

    CSR_Matrix4 scaleMatrix;

    // apply scaling to goal
    csrMat4Scale(&factor, &scaleMatrix);

    CSR_Matrix4 buildMatrix;

    // build the goal model matrix
    csrMat4Multiply(&ryMatrix, &scaleMatrix, &buildMatrix);
    csrMat4Multiply(&buildMatrix, &translationMatrix, &g_Goal.m_Matrix);

    // add the model to the scene
    pSceneItem = csrSceneAddModel(g_pScene, pModel, 0, 1);
    csrSceneAddModelMatrix(g_pScene, pModel, &g_Goal.m_Matrix);

    // load goal texture
    CSR_OpenGLHelper::AddTexture(&pModel->m_pMesh->m_Skin.m_Texture,
                                  LoadTexture((g_SceneDir + GOAL_TEXTURE_FILE).c_str()),
                                  g_OpenGLResources);

    CSR_Box goalBox = {};

    // transform the goal bounding box in his local system coordinates
    csrMat4ApplyToVector(&g_Goal.m_Matrix, &pSceneItem->m_pAABBTree[0].m_pBox->m_Min, &goalBox.m_Min);
    csrMat4ApplyToVector(&g_Goal.m_Matrix, &pSceneItem->m_pAABBTree[0].m_pBox->m_Max, &goalBox.m_Max);

    // configure the goal
    g_Goal.m_pKey             = pSceneItem->m_pModel;
    g_Goal.m_Bounds.m_Min.m_X = min(goalBox.m_Min.m_X, goalBox.m_Max.m_X);
    g_Goal.m_Bounds.m_Min.m_Y = min(goalBox.m_Min.m_Z, goalBox.m_Max.m_Z);
    g_Goal.m_Bounds.m_Max.m_X = max(goalBox.m_Min.m_X, goalBox.m_Max.m_X);
    g_Goal.m_Bounds.m_Max.m_Y = max(goalBox.m_Min.m_Z, goalBox.m_Max.m_Z);

    // create the You Won surface
    pMesh = csrShapeCreateSurface(0.6f, 0.2f, &vertexFormat, 0, &material, 0);

    // load the You Won texture
    CSR_OpenGLHelper::AddTexture(&pMesh->m_Skin.m_Texture,
                                  LoadTexture((g_SceneDir + YOU_WON_TEXTURE_FILE).c_str()),
                                  g_OpenGLResources);

    // add the mesh to the scene
    pSceneItem = csrSceneAddMesh(g_pScene, pMesh, 0, 1);
    csrSceneAddModelMatrix(g_pScene, pMesh, &g_YouWonMatrix);

    // initialize the You Won matrix
    csrMat4Identity(&g_YouWonMatrix);
    g_YouWonMatrix.m_Table[3][0] =  0.0f;
    g_YouWonMatrix.m_Table[3][1] =  99999.0f;
    g_YouWonMatrix.m_Table[3][2] = -1.65f;

    const std::string vsSkybox = CSR_ShaderHelper::GetVertexShader  (CSR_ShaderHelper::IEShaderType::IE_ST_Skybox);
    const std::string fsSkybox = CSR_ShaderHelper::GetFragmentShader(CSR_ShaderHelper::IEShaderType::IE_ST_Skybox);

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
    g_pBallKickSound      = csrSoundOpenWavFile(g_pOpenALDevice, g_pOpenALContext, (g_SceneDir + BALL_KICK_SOUND_FILE).c_str());

    // change the volume
    csrSoundChangeVolume(g_pFootStepLeftSound,  0.2f);
    csrSoundChangeVolume(g_pFootStepRightSound, 0.2f);

    g_Initialized = true;

    return true;
}
//------------------------------------------------------------------------------
void DeleteScene()
{
    g_Initialized = false;

    // release the shader
    csrOpenGLShaderRelease(g_pShader);
    csrOpenGLShaderRelease(g_pSkyboxShader);

    // release the scene
    csrSceneRelease(g_pScene, OnDeleteTexture);

    // stop running step sound, if needed
    csrSoundStop(g_pFootStepLeftSound);
    csrSoundStop(g_pFootStepRightSound);
    csrSoundStop(g_pBallKickSound);

    // release OpenAL interface
    csrSoundRelease(g_pFootStepLeftSound);
    csrSoundRelease(g_pFootStepRightSound);
    csrSoundRelease(g_pBallKickSound);
}
//------------------------------------------------------------------------------
void UpdateScene(float elapsedTime)
{
    ApplyPhysics(elapsedTime);

    // is player rotating?
    if (g_DirVelocity)
    {
        // calculate the player direction
        g_Angle += g_DirVelocity * elapsedTime;

        // validate and apply it
        if (g_Angle > (float)(M_PI * 2.0))
            g_Angle -= (float)(M_PI * 2.0);
        else
        if (g_Angle < 0.0f)
            g_Angle += (float)(M_PI * 2.0);
    }

    // keep the old position to revert it in case of error
    const CSR_Vector3 oldPos = g_ViewSphere.m_Center;

    // is player moving?
    if (g_PosVelocity)
    {
        // calculate the next position
        g_ViewSphere.m_Center.m_X += g_PosVelocity * sinf(g_Angle) * elapsedTime;
        g_ViewSphere.m_Center.m_Z -= g_PosVelocity * cosf(g_Angle) * elapsedTime;

        // calculate next time where the step sound should be played
        g_StepTime += (elapsedTime * 1000.0f);

        // do play the sound?
        if (g_StepTime > g_StepInterval)
        {
            // do play the left or right footstep sound?
            if (!(g_AlternateStep % 2))
            {
                csrSoundStop(g_pFootStepLeftSound);
                csrSoundPlay(g_pFootStepLeftSound);
            }
            else
            {
                csrSoundStop(g_pFootStepRightSound);
                csrSoundPlay(g_pFootStepRightSound);
            }

            g_StepTime = 0.0f;

            // next time the other footstep sound will be played
            g_AlternateStep = (g_AlternateStep + 1) & 1;
        }
    }

    // enable the shader program
    csrShaderEnable(g_pShader);

    CSR_Plane groundPlane;

    // calculate the ground position and check if next position is valid
    if (!ApplyGroundCollision(&g_ViewSphere, g_Angle, &g_pScene->m_ViewMatrix, &groundPlane))
    {
        // invalid next position, get the scene item (just one for this scene)
        const CSR_SceneItem* pItem = csrSceneGetItem(g_pScene, g_pLandscapeKey);

        // found it?
        if (pItem)
        {
            // check if the x position is out of bounds, and correct it if yes
            if (g_ViewSphere.m_Center.m_X <= pItem->m_pAABBTree->m_pBox->m_Min.m_X ||
                g_ViewSphere.m_Center.m_X >= pItem->m_pAABBTree->m_pBox->m_Max.m_X)
                g_ViewSphere.m_Center.m_X  = oldPos.m_X;

            // do the same thing with the z position. Doing that separately for each axis will make
            // the point of view to slide against the landscape border (this is possible because the
            // landscape is axis-aligned)
            if (g_ViewSphere.m_Center.m_Z <= pItem->m_pAABBTree->m_pBox->m_Min.m_Z ||
                g_ViewSphere.m_Center.m_Z >= pItem->m_pAABBTree->m_pBox->m_Max.m_Z)
                g_ViewSphere.m_Center.m_Z  = oldPos.m_Z;
        }
        else
            // failed to get the scene item, just revert the position
            g_ViewSphere.m_Center = oldPos;

        // recalculate the ground value (this time the collision result isn't tested, because the
        // previous position is always considered as valid)
        ApplyGroundCollision(&g_ViewSphere, g_Angle, &g_pScene->m_ViewMatrix, &groundPlane);
    }
    else
    if (g_ViewSphere.m_Center.m_X >= g_Goal.m_Bounds.m_Min.m_X && g_ViewSphere.m_Center.m_X <= g_Goal.m_Bounds.m_Max.m_X &&
        g_ViewSphere.m_Center.m_Z >= g_Goal.m_Bounds.m_Min.m_Y && g_ViewSphere.m_Center.m_Z <= g_Goal.m_Bounds.m_Max.m_Y)
    {
        // player hit the goal
        // a       b
        // |-------|
        // |       |
        // |       |
        // |       |
        // |       |
        // |       |
        // |       |
        // |       |
        // |-------|
        // d       c
        CSR_Segment3 ab = {};
        CSR_Segment3 bc = {};
        CSR_Segment3 cd = {};
        CSR_Segment3 da = {};

        // build the ab segment
        ab.m_Start.m_X = g_Goal.m_Bounds.m_Min.m_X;
        ab.m_Start.m_Y = 0.0f;
        ab.m_Start.m_Z = g_Goal.m_Bounds.m_Min.m_Y;
        ab.m_End.m_X   = g_Goal.m_Bounds.m_Max.m_X;
        ab.m_End.m_Y   = 0.0f;
        ab.m_End.m_Z   = g_Goal.m_Bounds.m_Min.m_Y;

        // build the bc segment
        bc.m_Start.m_X = g_Goal.m_Bounds.m_Max.m_X;
        bc.m_Start.m_Y = 0.0f;
        bc.m_Start.m_Z = g_Goal.m_Bounds.m_Min.m_Y;
        bc.m_End.m_X   = g_Goal.m_Bounds.m_Max.m_X;
        bc.m_End.m_Y   = 0.0f;
        bc.m_End.m_Z   = g_Goal.m_Bounds.m_Max.m_Y;

        // build the cd segment
        cd.m_Start.m_X = g_Goal.m_Bounds.m_Max.m_X;
        cd.m_Start.m_Y = 0.0f;
        cd.m_Start.m_Z = g_Goal.m_Bounds.m_Max.m_Y;
        cd.m_End.m_X   = g_Goal.m_Bounds.m_Min.m_X;
        cd.m_End.m_Y   = 0.0f;
        cd.m_End.m_Z   = g_Goal.m_Bounds.m_Max.m_Y;

        // build the da segment
        da.m_Start.m_X = g_Goal.m_Bounds.m_Min.m_X;
        da.m_Start.m_Y = 0.0f;
        da.m_Start.m_Z = g_Goal.m_Bounds.m_Max.m_Y;
        da.m_End.m_X   = g_Goal.m_Bounds.m_Min.m_X;
        da.m_End.m_Y   = 0.0f;
        da.m_End.m_Z   = g_Goal.m_Bounds.m_Min.m_Y;

        CSR_Vector3 ptAB;
        CSR_Vector3 ptBC;
        CSR_Vector3 ptCD;
        CSR_Vector3 ptDA;

        // calculate the closest point from previous position to each of the segments
        csrSeg3ClosestPoint(&ab, &oldPos, &ptAB);
        csrSeg3ClosestPoint(&bc, &oldPos, &ptBC);
        csrSeg3ClosestPoint(&cd, &oldPos, &ptCD);
        csrSeg3ClosestPoint(&da, &oldPos, &ptDA);

        CSR_Vector3 PPtAB;
        CSR_Vector3 PPtBC;
        CSR_Vector3 PPtCD;
        CSR_Vector3 PPtDA;

        // calculate each distances between the previous point and each points found on segments
        csrVec3Sub(&ptAB, &oldPos, &PPtAB);
        csrVec3Sub(&ptBC, &oldPos, &PPtBC);
        csrVec3Sub(&ptCD, &oldPos, &PPtCD);
        csrVec3Sub(&ptDA, &oldPos, &PPtDA);

        float lab;
        float lbc;
        float lcd;
        float lda;

        // calculate each lengths between the previous point and each points found on segments
        csrVec3Length(&PPtAB, &lab);
        csrVec3Length(&PPtBC, &lbc);
        csrVec3Length(&PPtCD, &lcd);
        csrVec3Length(&PPtDA, &lda);

        // find on which side the player is hitting the goal
        if (lab < lbc && lab < lcd && lab < lda)
            g_ViewSphere.m_Center.m_Z = oldPos.m_Z;
        else
        if (lbc < lab && lbc < lcd && lbc < lda)
            g_ViewSphere.m_Center.m_X = oldPos.m_X;
        else
        if (lcd < lab && lcd < lbc && lcd < lda)
            g_ViewSphere.m_Center.m_Z = oldPos.m_Z;
        else
        if (lda < lab && lda < lbc && lda < lcd)
            g_ViewSphere.m_Center.m_X = oldPos.m_X;
        else
        {
            g_ViewSphere.m_Center.m_X = oldPos.m_X;
            g_ViewSphere.m_Center.m_Z = oldPos.m_Z;
        }

        // recalculate the ground value (this time the collision result isn't tested, because the
        // previous position is always considered as valid)
        ApplyGroundCollision(&g_ViewSphere, g_Angle, &g_pScene->m_ViewMatrix, &groundPlane);
    }
}
//------------------------------------------------------------------------------
void DrawScene()
{
    // draw the scene
    csrSceneDraw(g_pScene, &g_SceneContext);
}
//------------------------------------------------------------------------------
int APIENTRY wWinMain(_In_     HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_     LPWSTR    lpCmdLine,
                      _In_     int       nCmdShow)
{
    WNDCLASSEX wcex  = {};
    HWND       hWnd  = 0;
    MSG        msg   = {};
    BOOL       bQuit = FALSE;

    // register the window class
    wcex.cbSize        = sizeof(WNDCLASSEX);
    wcex.style         = CS_OWNDC;
    wcex.lpfnWndProc   = WindowProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = hInstance;
    wcex.hIcon         = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON));
    wcex.hIconSm       = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));
    wcex.hCursor       = ::LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName  = nullptr;
    wcex.lpszClassName = L"CSR_WildSoccer";

    if (!RegisterClassEx(&wcex))
        return 0;

    // create the main window
    hWnd = ::CreateWindowEx(0,
                            L"CSR_WildSoccer",
                            L"Wild soccer demo",
                            WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            800,
                            650,
                            nullptr,
                            nullptr,
                            hInstance,
                            nullptr);

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
    g_StartTime    = ::GetTickCount64();
    g_PreviousTime = ::GetTickCount64();

    // application main loop
    while (!bQuit)
    {
        // check for messages
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
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
            const unsigned __int64 now            = ::GetTickCount64();
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
                g_StartTime                 = ::GetTickCount64();
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
