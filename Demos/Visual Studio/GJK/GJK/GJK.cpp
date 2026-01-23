/*****************************************************************************
 * ==> GJK demo -------------------------------------------------------------*
 *****************************************************************************
 * Description : Gilbert-Johnson-Keerthi (GJK) algorithm demo                *
 * Developer   : Jean-Milost Reymond                                         *
 * Copyright   : 2015 - 2022, this file is part of the Minimal API. You are  *
 *               free to copy or redistribute this file, modify it, or use   *
 *               it for your own projects, commercial or not. This file is   *
 *               provided "as is", without ANY WARRANTY OF ANY KIND          *
 *****************************************************************************/

 // std
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include <stdlib.h>

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Geometry.h"
#include "CSR_Collision.h"
#include "CSR_GJK.h"
#include "CSR_Texture.h"
#include "CSR_Vertex.h"
#include "CSR_Model.h"
#include "CSR_Scene.h"
#include "CSR_AI.h"
#include "CSR_Renderer_OpenGL.h"

// classes
#include "CSR_OpenGLHelper.h"
#include "CSR_ShaderHelper.h"

// windows
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// openGL
#include <gl/gl.h>
#define GLEW_STATIC
#include <GL/glew.h>

// resources
#include "Resource.h"

//------------------------------------------------------------------------------
#define BACKGROUND_TEXTURE_FILE "\\background.bmp"
//------------------------------------------------------------------------------
HDC                          g_hDC             = 0;
HGLRC                        g_hRC             = 0;
std::string                  g_SceneDir;
CSR_OpenGLHelper::IResources g_OpenGLResources;
CSR_Scene*                   g_pScene          = nullptr;
CSR_SceneContext             g_SceneContext;
CSR_OpenGLShader*            g_pColShader      = nullptr;
CSR_OpenGLShader*            g_pTexShader      = nullptr;
CSR_Mesh*                    g_pBackgroundMesh = nullptr;
CSR_ArcBall                  g_Arcball;
CSR_Collider*                g_pPlayerCollider;
CSR_Collider*                g_pModel1Collider;
CSR_Collider*                g_pModel2Collider;
CSR_Collider*                g_pModel3Collider;
CSR_Collider*                g_pModel4Collider;
CSR_Matrix4                  g_BackgroundMatrix;
CSR_Matrix4                  g_PlayerMatrix;
CSR_Matrix4                  g_Model1Matrix;
CSR_Matrix4                  g_Model2Matrix;
CSR_Matrix4                  g_Model3Matrix;
CSR_Matrix4                  g_Model4Matrix;
size_t                       g_FrameCount      = 0;
int                          g_LastMouseXPos   = 0;
int                          g_LastMouseYPos   = 0;
int                          g_xDelta          = 0;
int                          g_yDelta          = 0;
float                        g_xPos            = 0.0f;
float                        g_zPos            = 0.0f;
float                        g_Velocity        = 0.025f;
double                       g_FPS             = 0.0;
unsigned __int64             g_StartTime       = 0L;
unsigned __int64             g_PreviousTime    = 0L;
bool                         g_Initialized     = false;
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

        case WM_SIZE:
        {
            if (!g_Initialized)
                break;

            const int width  = ((int)(short)LOWORD(lParam));
            const int height = ((int)(short)HIWORD(lParam));

            // update the viewport in the color shader
            CSR_OpenGLHelper::CreateViewport((float)width,
                                             (float)height,
                                             0.01f,
                                             100.0f,
                                             g_pColShader,
                                             g_pScene->m_ProjectionMatrix);

            // update the viewport in the texture shader
            CSR_OpenGLHelper::CreateViewport((float)width,
                                             (float)height,
                                             0.01f,
                                             100.0f,
                                             g_pTexShader,
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
//------------------------------------------------------------------------------
void UpdatePos(CSR_ArcBall& arcball, double elapsedTime)
{
    POINT p;

    // get current mouse position
    ::GetCursorPos(&p);

    // calculate delta on x and y axis
    g_xDelta = g_LastMouseXPos - p.x;
    g_yDelta = g_LastMouseYPos - p.y;

    // update the last known position
    g_LastMouseXPos = p.x;
    g_LastMouseYPos = p.y;

    // calculate the new direction from last mouse move
    arcball.m_AngleY -= std::fmodf((float)g_xDelta * 0.01f, (float)M_PI * 2.0f);

    // reset the deltas (otherwise the player will turn forever)
    g_xDelta = 0;
    g_yDelta = 0;

    // get the pressed key, if any, and convert it to the matching player state
    if ((::GetKeyState(VK_UP) & 0x8000) || (::GetKeyState(87) & 0x8000) || (::GetKeyState(119) & 0x8000))
    {
        // move player forward
        g_xPos += g_Velocity * std::cosf(arcball.m_AngleY + (float)(M_PI * 0.5)) * (float)(elapsedTime * 0.025);
        g_zPos += g_Velocity * std::sinf(arcball.m_AngleY + (float)(M_PI * 0.5)) * (float)(elapsedTime * 0.025);
    }

    arcball.m_Position.m_X =  g_xPos;
    arcball.m_Position.m_Y = -0.5f;
    arcball.m_Position.m_Z =  2.0f + g_zPos;
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
void BuildMatrix(const CSR_Vector3& pos, float roll, float pitch, float yaw, float scale, CSR_Matrix4& matrix)
{
    CSR_Vector3 axis;

    // set rotation axis
    axis.m_X = 1.0f;
    axis.m_Y = 0.0f;
    axis.m_Z = 0.0f;

    CSR_Matrix4 rotateXMatrix;

    // create the rotation matrix on x axis
    csrMat4Rotate(roll, &axis, &rotateXMatrix);

    // set rotation axis
    axis.m_X = 0.0f;
    axis.m_Y = 1.0f;
    axis.m_Z = 0.0f;

    CSR_Matrix4 rotateYMatrix;

    // create the rotation matrix on y axis
    csrMat4Rotate(pitch, &axis, &rotateYMatrix);

    // set rotation axis
    axis.m_X = 0.0f;
    axis.m_Y = 0.0f;
    axis.m_Z = 1.0f;

    CSR_Matrix4 rotateZMatrix;

    // create the rotation matrix on z axis
    csrMat4Rotate(yaw, &axis, &rotateZMatrix);

    CSR_Vector3 factor;

    // set scale factor
    factor.m_X = scale;
    factor.m_Y = scale;
    factor.m_Z = scale;

    CSR_Matrix4 scaleMatrix;

    // create the scale matrix
    csrMat4Scale(&factor, &scaleMatrix);

    CSR_Matrix4 firstIntermediateMatrix;
    CSR_Matrix4 secondIntermediateMatrix;

    // build the model matrix
    csrMat4Multiply(&scaleMatrix,              &rotateXMatrix, &firstIntermediateMatrix);
    csrMat4Multiply(&firstIntermediateMatrix,  &rotateYMatrix, &secondIntermediateMatrix);
    csrMat4Multiply(&secondIntermediateMatrix, &rotateZMatrix, &matrix);

    // place it in the world
    matrix.m_Table[3][0] = pos.m_X;
    matrix.m_Table[3][1] = pos.m_Y;
    matrix.m_Table[3][2] = pos.m_Z;
}
//---------------------------------------------------------------------------
void* OnGetShader(const void* pModel, CSR_EModelType type)
{
    csrShaderEnable(g_pTexShader);

    // textured model?
    if (pModel == g_pBackgroundMesh)
        return g_pTexShader;

    return g_pColShader;
}
//---------------------------------------------------------------------------
void* OnGetID(const void* pKey)
{
    return CSR_OpenGLHelper::GetTextureID(pKey, g_OpenGLResources);
}
//------------------------------------------------------------------------------
bool InitScene(int w, int h)
{
    CSR_VertexFormat  vertexFormat  = {};
    CSR_VertexCulling vertexCulling = {};
    CSR_Material      material      = {};
    CSR_SceneItem*    pSceneItem    = nullptr;

    // initialize the scene
    g_pScene = csrSceneCreate();

    // configure the scene background color
    g_pScene->m_Color.m_R = 0.25f;
    g_pScene->m_Color.m_G = 0.4f;
    g_pScene->m_Color.m_B = 0.5f;
    g_pScene->m_Color.m_A = 1.0f;

    // configure the scene view matrix
    csrMat4Identity(&g_pScene->m_ViewMatrix);

    // configure the scene context
    csrSceneContextInit(&g_SceneContext);
    g_SceneContext.m_fOnGetShader = OnGetShader;
    g_SceneContext.m_fOnGetID     = OnGetID;

    const std::string vsColored = CSR_ShaderHelper::GetVertexShader  (CSR_ShaderHelper::IEShaderType::IE_ST_Color);
    const std::string fsColored = CSR_ShaderHelper::GetFragmentShader(CSR_ShaderHelper::IEShaderType::IE_ST_Color);

    // load the shader
    g_pColShader = csrOpenGLShaderLoadFromStr(vsColored.c_str(),
                                              vsColored.length(),
                                              fsColored.c_str(),
                                              fsColored.length(),
                                              0,
                                              0);

    // succeeded?
    if (!g_pColShader)
        return false;

    // update the viewport
    CSR_OpenGLHelper::CreateViewport((float)w,
                                     (float)h,
                                     0.01f,
                                     100.0f,
                                     g_pColShader,
                                     g_pScene->m_ProjectionMatrix);

    // get shader attributes
    g_pColShader->m_VertexSlot = glGetAttribLocation(g_pColShader->m_ProgramID, "csr_aVertices");
    g_pColShader->m_ColorSlot  = glGetAttribLocation(g_pColShader->m_ProgramID, "csr_aColor");

    const std::string vsTextured = CSR_ShaderHelper::GetVertexShader  (CSR_ShaderHelper::IEShaderType::IE_ST_Texture);
    const std::string fsTextured = CSR_ShaderHelper::GetFragmentShader(CSR_ShaderHelper::IEShaderType::IE_ST_Texture);

    // load the shader
    g_pTexShader = csrOpenGLShaderLoadFromStr(vsTextured.c_str(),
                                              vsTextured.length(),
                                              fsTextured.c_str(),
                                              fsTextured.length(),
                                              0,
                                              0);

    // succeeded?
    if (!g_pTexShader)
        return false;

    // update the viewport
    CSR_OpenGLHelper::CreateViewport((float)w,
                                     (float)h,
                                     0.01f,
                                     100.0f,
                                     g_pTexShader,
                                     g_pScene->m_ProjectionMatrix);

    // get shader attributes
    g_pTexShader->m_VertexSlot   = glGetAttribLocation (g_pTexShader->m_ProgramID, "csr_aVertices");
    g_pTexShader->m_ColorSlot    = glGetAttribLocation (g_pTexShader->m_ProgramID, "csr_aColor");
    g_pTexShader->m_TexCoordSlot = glGetAttribLocation (g_pTexShader->m_ProgramID, "csr_aTexCoord");
    g_pTexShader->m_TextureSlot  = glGetUniformLocation(g_pTexShader->m_ProgramID, "csr_sTexture");

    // configure OpenGL depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRangef(0.0f, 1.0f);

    CSR_Vector3 pos;

    // set background position
    pos.m_X = 0.0f;
    pos.m_Y = 0.0f;
    pos.m_Z = 0.0f;

    // place the background
    csrMat4Identity(&g_BackgroundMatrix);
    BuildMatrix(pos, (float)M_PI / 2.0f, 0.0f, 0.0f, 1.0f, g_BackgroundMatrix);

    // configure the vertex format
    vertexFormat.m_HasNormal         = 0;
    vertexFormat.m_HasTexCoords      = 1;
    vertexFormat.m_HasPerVertexColor = 1;

    vertexCulling.m_Face = CSR_CF_CW;
    vertexCulling.m_Type = CSR_CT_Front;

    // configure the material
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;
    material.m_uScale      = 1.0f;
    material.m_vScale      = 1.0f;

    // create the background
    g_pBackgroundMesh = csrShapeCreateSurface(20.0f, 20.0f, &vertexFormat, &vertexCulling, &material, 0);
    pSceneItem        = csrSceneAddMesh(g_pScene, g_pBackgroundMesh, 0, 0);
    csrSceneAddModelMatrix(g_pScene, g_pBackgroundMesh, &g_BackgroundMatrix);

    // load background texture
    CSR_OpenGLHelper::AddTexture(&g_pBackgroundMesh->m_Skin.m_Texture,
                                 LoadTexture((g_SceneDir + BACKGROUND_TEXTURE_FILE).c_str()),
                                 g_OpenGLResources);

    CSR_Matrix4 matrix;

    // create the player collider
    g_pPlayerCollider               = csrColliderCreate();
    g_pPlayerCollider->m_State      = CSR_CS_Dynamic;
    g_pPlayerCollider->m_Pos.m_X    = 0.0f;
    g_pPlayerCollider->m_Pos.m_Y    = 0.0f;
    g_pPlayerCollider->m_Pos.m_Z    = 0.0f;
    g_pPlayerCollider->m_Radius     = 0.17f;
    g_pPlayerCollider->m_TopY       = 0.85f;
    g_pPlayerCollider->m_BottomY    = 0.0f;
    g_pPlayerCollider->m_fOnSupport = csrGJKSupportCapsule;
    csrMat4Identity(&matrix);
    csrColliderSetRS(&matrix, g_pPlayerCollider);

    CSR_Mesh* pMesh;

    // set mesh format, material and culling
    vertexFormat.m_HasTexCoords = 0;
    material.m_Color            = 0x0000FFFF;
    vertexCulling.m_Type        = CSR_CT_Back;

    // build the player matrix
    csrMat4Identity(&g_PlayerMatrix);

    // create the player capsule
    pMesh                       = csrShapeCreateCapsule(0.85f, 0.17f, 16.0f, &vertexFormat, &vertexCulling, &material, 0);
    pSceneItem                  = csrSceneAddMesh(g_pScene, pMesh, 0, 0);
    pSceneItem->m_CollisionType = CSR_CO_GJK;
    pSceneItem->m_pCollider     = g_pPlayerCollider;
    csrSceneAddModelMatrix(g_pScene, pMesh, &g_PlayerMatrix);

    // create the first model collider
    g_pModel1Collider               =  csrColliderCreate();
    g_pModel1Collider->m_State      =  CSR_CS_Static;
    g_pModel1Collider->m_Pos.m_X    =  5.0f;
    g_pModel1Collider->m_Pos.m_Y    =  0.0f;
    g_pModel1Collider->m_Pos.m_Z    = -2.0f;
    g_pModel1Collider->m_Radius     =  0.17f;
    g_pModel1Collider->m_TopY       =  0.85f;
    g_pModel1Collider->m_BottomY    =  0.0f;
    g_pModel1Collider->m_fOnSupport =  csrGJKSupportCapsule;
    csrMat4Identity(&matrix);
    csrColliderSetRS(&matrix, g_pModel1Collider);

    // set mesh material
    material.m_Color = 0xFF00FFFF;

    // build the first model matrix
    csrMat4Identity(&g_Model1Matrix);
    g_Model1Matrix.m_Table[3][0] = g_pModel1Collider->m_Pos.m_X;
    g_Model1Matrix.m_Table[3][1] = g_pModel1Collider->m_Pos.m_Y;
    g_Model1Matrix.m_Table[3][2] = g_pModel1Collider->m_Pos.m_Z;

    // create the capsule
    pMesh                       = csrShapeCreateCapsule(0.85f, 0.17f, 16.0f, &vertexFormat, &vertexCulling, &material, 0);
    pSceneItem                  = csrSceneAddMesh(g_pScene, pMesh, 0, 0);
    pSceneItem->m_CollisionType = CSR_CO_GJK;
    pSceneItem->m_pCollider     = g_pModel1Collider;
    csrSceneAddModelMatrix(g_pScene, pMesh, &g_Model1Matrix);

    // set mesh culling
    vertexCulling.m_Type = CSR_CT_Front;

    // create the second model collider
    g_pModel2Collider               =  csrColliderCreate();
    g_pModel2Collider->m_State      =  CSR_CS_Static;
    g_pModel2Collider->m_Pos.m_X    = -5.0f;
    g_pModel2Collider->m_Pos.m_Y    =  0.0f;
    g_pModel2Collider->m_Pos.m_Z    =  3.5f;
    g_pModel2Collider->m_Min.m_X    = -0.4f;
    g_pModel2Collider->m_Min.m_Y    = -1.7f;
    g_pModel2Collider->m_Min.m_Z    = -1.3f;
    g_pModel2Collider->m_Max.m_X    =  0.4f;
    g_pModel2Collider->m_Max.m_Y    =  1.7f;
    g_pModel2Collider->m_Max.m_Z    =  1.3f;
    g_pModel2Collider->m_fOnSupport =  csrGJKSupportBox;
    csrMat4Identity(&matrix);
    BuildMatrix(pos, 0.0f, (float)(M_PI * 0.25), (float)(M_PI * 0.15), 1.0f, matrix);
    csrColliderSetRS(&matrix, g_pModel2Collider);

    // set mesh material
    material.m_Color = 0x00FF00FF;

    // build the second model matrix
    csrMat4Identity(&matrix);
    matrix.m_Table[3][0] = g_pModel2Collider->m_Pos.m_X;
    matrix.m_Table[3][1] = g_pModel2Collider->m_Pos.m_Y;
    matrix.m_Table[3][2] = g_pModel2Collider->m_Pos.m_Z;
    csrMat4Multiply(&g_pModel2Collider->m_MatRS, &matrix, &g_Model2Matrix);

    // create the box
    pMesh                       = csrShapeCreateBox(0.8f, 3.4f, 2.6f, 0, &vertexFormat, &vertexCulling, &material, 0);
    pSceneItem                  = csrSceneAddMesh(g_pScene, pMesh, 0, 0);
    pSceneItem->m_CollisionType = CSR_CO_GJK;
    pSceneItem->m_pCollider     = g_pModel2Collider;
    csrSceneAddModelMatrix(g_pScene, pMesh, &g_Model2Matrix);

    // create the third model collider
    g_pModel3Collider               =  csrColliderCreate();
    g_pModel3Collider->m_State      =  CSR_CS_Static;
    g_pModel3Collider->m_Pos.m_X    = -5.0f;
    g_pModel3Collider->m_Pos.m_Y    =  0.2f;
    g_pModel3Collider->m_Pos.m_Z    = -3.5f;
    g_pModel3Collider->m_Radius     =  1.2f;
    g_pModel3Collider->m_fOnSupport =  csrGJKSupportSphere;
    csrMat4Identity(&matrix);
    csrColliderSetRS(&matrix, g_pModel3Collider);

    // set mesh material
    material.m_Color = 0xFFFF00FF;

    // build the third model matrix
    csrMat4Identity(&g_Model3Matrix);
    g_Model3Matrix.m_Table[3][0] = g_pModel3Collider->m_Pos.m_X;
    g_Model3Matrix.m_Table[3][1] = g_pModel3Collider->m_Pos.m_Y;
    g_Model3Matrix.m_Table[3][2] = g_pModel3Collider->m_Pos.m_Z;

    // create the sphere
    pMesh                       = csrShapeCreateSphere(1.2f, 20, 20, &vertexFormat, &vertexCulling, &material, 0);
    pSceneItem                  = csrSceneAddMesh(g_pScene, pMesh, 0, 0);
    pSceneItem->m_CollisionType = CSR_CO_GJK;
    pSceneItem->m_pCollider     = g_pModel3Collider;
    csrSceneAddModelMatrix(g_pScene, pMesh, &g_Model3Matrix);

    // create the fourth model collider
    g_pModel4Collider               = csrColliderCreate();
    g_pModel4Collider->m_State      = CSR_CS_Static;
    g_pModel4Collider->m_Pos.m_X    = 5.0f;
    g_pModel4Collider->m_Pos.m_Y    = 0.2f;
    g_pModel4Collider->m_Pos.m_Z    = 4.1f;
    g_pModel4Collider->m_Radius     = 2.1f;
    g_pModel4Collider->m_TopY       = 1.5f;
    g_pModel4Collider->m_BottomY    = 0.0f;
    g_pModel4Collider->m_fOnSupport = csrGJKSupportCylinder;
    csrMat4Identity(&matrix);
    csrColliderSetRS(&matrix, g_pModel4Collider);

    // set mesh material
    material.m_Color = 0x00FFFFFF;

    // build the fourth model matrix
    csrMat4Identity(&g_Model4Matrix);
    g_Model4Matrix.m_Table[3][0] = g_pModel4Collider->m_Pos.m_X;
    g_Model4Matrix.m_Table[3][1] = g_pModel4Collider->m_Pos.m_Y;
    g_Model4Matrix.m_Table[3][2] = g_pModel4Collider->m_Pos.m_Z;

    // create the cylinder
    pMesh                       = csrShapeCreateCylinder(2.1f, 2.1f, 1.5f, 20, &vertexFormat, &vertexCulling, &material, 0);
    pSceneItem                  = csrSceneAddMesh(g_pScene, pMesh, 0, 0);
    pSceneItem->m_CollisionType = CSR_CO_GJK;
    pSceneItem->m_pCollider     = g_pModel4Collider;
    csrSceneAddModelMatrix(g_pScene, pMesh, &g_Model4Matrix);

    // initialize the arcball
    g_Arcball.m_Radius = 2.0f;
    g_Arcball.m_AngleX = 0.25f;

    g_Initialized = true;

    return true;
}
//------------------------------------------------------------------------------
void DeleteScene()
{
    g_Initialized = false;

    // delete scene content
    csrSceneRelease(g_pScene, nullptr);

    // delete scene shaders
    csrOpenGLShaderRelease(g_pColShader);
    csrOpenGLShaderRelease(g_pTexShader);
}
//------------------------------------------------------------------------------
void UpdateScene(float elapsedTime)
{
    // update the arcball position
    UpdatePos(g_Arcball, (double)elapsedTime * 1000.0);

    // update the collider position
    g_pPlayerCollider->m_Pos.m_X = -g_Arcball.m_Position.m_X;
    g_pPlayerCollider->m_Pos.m_Y =  0.0f;
    g_pPlayerCollider->m_Pos.m_Z = -g_Arcball.m_Position.m_Z;

    CSR_CollisionInput colInput;
    csrCollisionInputInit(&colInput);

    CSR_CollisionOutput colOutput;
    csrCollisionOutputInit(&colOutput);

    // detect the collisions in the scene
    csrSceneDetectCollision(g_pScene, &colInput, &colOutput, 0);

    // found a collision?
    if (colOutput.m_pColliders && colOutput.m_pColliders->m_Count)
    {
        // update the player position
        g_xPos -= colOutput.m_MinTransVec.m_X;
        g_zPos -= colOutput.m_MinTransVec.m_Z;

        // update the arcball position
        g_Arcball.m_Position.m_X += colOutput.m_MinTransVec.m_X;
        g_Arcball.m_Position.m_Z += colOutput.m_MinTransVec.m_Z;

        // update the collider position
        g_pPlayerCollider->m_Pos.m_X = -g_Arcball.m_Position.m_X;
        g_pPlayerCollider->m_Pos.m_Z = -g_Arcball.m_Position.m_Z;
    }

    // update the player matrix
    g_PlayerMatrix.m_Table[3][0] = g_pPlayerCollider->m_Pos.m_X;
    g_PlayerMatrix.m_Table[3][1] = g_pPlayerCollider->m_Pos.m_Y;
    g_PlayerMatrix.m_Table[3][2] = g_pPlayerCollider->m_Pos.m_Z;

    // get the view matrix matching with the camera
    csrSceneArcBallToMatrix(&g_Arcball, &g_pScene->m_ViewMatrix);
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
    wcex.hCursor       = ::LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName  = NULL;
    wcex.lpszClassName = L"CSR_GJK";

    if (!::RegisterClassEx(&wcex))
        return 0;

    // create the main window
    hWnd = ::CreateWindowEx(0,
                            L"CSR_GJK",
                            L"Gilbert-Johnson-Keerthi (GJK) algorithm",
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

    POINT p;

    // get current mouse position
    ::GetCursorPos(&p);

    // update the last known position
    g_LastMouseXPos = p.x;
    g_LastMouseYPos = p.y;

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
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // handle or dispatch messages
            if (msg.message == WM_QUIT)
                bQuit = TRUE;
            else
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }
        }
        else
        {
            // calculate time interval
            const unsigned __int64 now         = ::GetTickCount64();
            const double           elapsedTime = (now - g_PreviousTime) / 1000.0;
            g_PreviousTime                     = now;

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

    // destroy the window explicitly
    ::DestroyWindow(hWnd);

    return (int)msg.wParam;
}
//------------------------------------------------------------------------------
