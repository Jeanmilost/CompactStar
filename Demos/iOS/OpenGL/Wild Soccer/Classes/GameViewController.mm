/*****************************************************************************
 * ==> Wild soccer game demo ------------------------------------------------*
 *****************************************************************************
 * Description : Wild soccer game demo view controller                       *
 * Developer   : Jean-Milost Reymond                                         *
 * Copyright   : 2015 - 2018, this file is part of the Minimal API. You are  *
 *               free to copy or redistribute this file, modify it, or use   *
 *               it for your own projects, commercial or not. This file is   *
 *               provided "as is", without ANY WARRANTY OF ANY KIND          *
 *****************************************************************************/

#import "GameViewController.h"

// std
#include <memory>

// openGL
#import <OpenGLES/ES2/glext.h>

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Geometry.h"
#include "CSR_Collision.h"
#include "CSR_Vertex.h"
#include "CSR_Model.h"
#include "CSR_Renderer.h"
#include "CSR_Renderer_OpenGL.h"
#include "CSR_Physics.h"
#include "CSR_Sound.h"

// classes
#import "CSR_ShaderHelper.h"
#import "CSR_ObjectiveCHelper.h"

// the compactStar engine should be compiled with an OpenGL renderer to run this demo
#ifndef CSR_USE_OPENGL
    #warning "OpenGL isn't the currently enabled library. Please enable the CSR_USE_OPENGL conditional define in CSR_Renderer.h, and use CSR_Renderer_OpenGL as renderer."
#endif

// energy factor for the shoot
#define M_ShootEnergyFactor 23.0f

//------------------------------------------------------------------------------
// Local structures
//------------------------------------------------------------------------------
typedef struct
{
    void*       m_pKey;
    CSR_Matrix4 m_Matrix;
    CSR_Sphere  m_Geometry;
    CSR_Body    m_Body;
} CSR_Ball;
//------------------------------------------------------------------------------
typedef struct
{
    void*       m_pKey;
    CSR_Matrix4 m_Matrix;
    CSR_Rect    m_Bounds;
} CSR_Goal;
//------------------------------------------------------------------------------
// CSR_CallbackController
//------------------------------------------------------------------------------
void* CSR_CallbackController::m_pOwner = NULL;
//------------------------------------------------------------------------------
CSR_CallbackController::CSR_CallbackController()
{}
//------------------------------------------------------------------------------
CSR_CallbackController::CSR_CallbackController(void* pOwner)
{
    // set the global static owner to share between all the functions
    m_pOwner = pOwner;

    // configure the scene context
    csrSceneContextInit(&m_SceneContext);
    m_SceneContext.m_fOnGetShader     = OnGetShader;
    m_SceneContext.m_fOnGetID         = OnGetID;
    m_SceneContext.m_fOnDeleteTexture = OnDeleteTexture;
}
//------------------------------------------------------------------------------
CSR_CallbackController::~CSR_CallbackController()
{}
//------------------------------------------------------------------------------
void CSR_CallbackController::ReleaseModel(CSR_Model* pModel) const
{
    csrModelRelease(pModel, OnDeleteTexture);
}
//------------------------------------------------------------------------------
void CSR_CallbackController::ReleaseScene(CSR_Scene* pScene) const
{
    csrSceneRelease(pScene, OnDeleteTexture);
}
//------------------------------------------------------------------------------
void CSR_CallbackController::DrawScene(CSR_Scene* pScene) const
{
    // draw the scene
    csrSceneDraw(pScene, &m_SceneContext);
}
//------------------------------------------------------------------------------
void* CSR_CallbackController::OnGetShader(const void* pModel, CSR_EModelType type)
{
    return [(__bridge id)m_pOwner OnGetShader :pModel :type];
}
//------------------------------------------------------------------------------
void* CSR_CallbackController::OnGetID(const void* pKey)
{
    return [(__bridge id)m_pOwner OnGetID :pKey];
}
//------------------------------------------------------------------------------
void CSR_CallbackController::OnDeleteTexture(const CSR_Texture* pTexture)
{
    [(__bridge id)m_pOwner OnDeleteTexture :pTexture];
}
//----------------------------------------------------------------------------
// GameViewController
//----------------------------------------------------------------------------
@interface GameViewController()
{
    CSR_CallbackController* m_pCallbackController;
    CSR_Scene*              m_pScene;
    CSR_OpenGLShader*       m_pShader;
    CSR_OpenGLShader*       m_pSkyboxShader;
    void*                   m_pLandscapeKey;
    float                   m_MapHeight;
    float                   m_MapScale;
    float                   m_Angle;
    float                   m_RollAngle;
    float                   m_BallDirAngle;
    float                   m_BallOffset;
    float                   m_StepTime;
    float                   m_StepInterval;
    float                   m_PosVelocity;
    float                   m_DirVelocity;
    float                   m_ControlRadius;
    CSR_Ball                m_Ball;
    CSR_Goal                m_Goal;
    CSR_Sphere              m_ViewSphere;
    CSR_Matrix4             m_LandscapeMatrix;
    CSR_Matrix4             m_YouWonMatrix;
    CSR_Vector2             m_TouchOrigin;
    CSR_Vector2             m_TouchPosition;
    CSR_Vector3             m_FrictionForce;
    CSR_Color               m_Color;
    ALCdevice*              m_pOpenALDevice;
    ALCcontext*             m_pOpenALContext;
    CSR_Sound*              m_pSound;
    CSR_OpenGLID            m_ID[5];
    CFTimeInterval          m_PreviousTime;
}

@property (strong, nonatomic) EAGLContext* pContext;

/**
* Enables OpenGL
*/
- (void) EnableOpenGL;

/**
* Disables OpenGL
*/
- (void) DisableOpenGL;

/**
* Loads a landscape from a bitmap file
*@param pFileName - bitmap file name
*@return true on success, optherwise false
*/
- (bool) LoadLandscapeFromBitmap :(const char*)pFileName;

/**
* Checks if the ball hit the goal
*@param pBall - ball to check
*@param pOldPos - previous ball position
*@param pDir - ball direction
*@return true if the ball hit the goal, otherwise false
*/
- (bool) CheckForGoal :(CSR_Ball*)pBall :(const CSR_Vector3*)pOldPos :(const CSR_Vector3*)pDir;

/**
* Calculates the point where a bounding sphere hits the ground
*@param pBoundingSphere - bounding sphere to calculate
*@param dir - ground dir
*@param[out] pMatrix - transformation matrix to apply to the sphere to lies on the ground
*@param[out] pGroundPlane - plane of the ground slope on which the sphere lies
*@return true if a ground collision was found, otherwise false
*/
- (bool) ApplyGroundCollision :(const CSR_Sphere*)pBoundingSphere
                              :(float)dir
                              :(CSR_Matrix4*)pMatrix
                              :(CSR_Plane*)pGroundPlane;

/**
* Applies the physical laws on the scene items
*@param elapsedTime - elapsed time since the last calculation, in milliseconds
*/
- (void) ApplyPhysics :(float)elapsedTime;

/**
* Shoots the ball
*/
- (void) Shoot;

/**
* Creates the viewport
*@param w - viewport width
*@param h - viewport height
*/
- (void) CreateViewport :(float)w  :(float)h;

/**
* Initializes the scene
*/
- (void) InitScene;

/**
* Deletes the scene
*/
- (void) DeleteScene;

/**
* Updates the scene
*@param elapsedTime - elapsed time since last update, in milliseconds
*/
- (void) UpdateScene :(float)elapsedTime;

/**
* Draws the scene
*/
- (void) DrawScene;

/**
* Called when screen is long pressed
*@param pSender - recognizer that raised the event
*/
- (void) OnLongPress :(UIGestureRecognizer*)pSender;

@end
//----------------------------------------------------------------------------
@implementation GameViewController
//----------------------------------------------------------------------------
- (void) viewDidLoad
{
    [super viewDidLoad];

    m_pCallbackController =  new CSR_CallbackController((__bridge void*)self);
    m_pScene              =  nil;
    m_pShader             =  nil;
    m_pSkyboxShader       =  nil;
    m_pLandscapeKey       =  nil;
    m_MapHeight           =  3.0f;
    m_MapScale            =  0.2f;
    m_Angle               =  M_PI / -4.0f;
    m_RollAngle           =  0.0f;
    m_BallDirAngle        =  0.0f;
    m_BallOffset          =  0.0f;
    m_StepTime            =  0.0f;
    m_StepInterval        =  300.0f;
    m_PosVelocity         = -10.0f;
    m_DirVelocity         =  15.0f;
    m_ControlRadius       =  40.0f;
    m_pOpenALDevice       =  nil;
    m_pOpenALContext      =  nil;
    m_pSound              =  nil;
    m_PreviousTime        =  CACurrentMediaTime();

    // add a long press gesture to the view
    UILongPressGestureRecognizer* pGestureRecognizer =
    [[UILongPressGestureRecognizer alloc]initWithTarget:self
                                                 action:@selector(OnLongPress:)];
    
    pGestureRecognizer.minimumPressDuration = 0;
    
    // add gesture recognizer to view
    [self.view addGestureRecognizer: pGestureRecognizer];

    [self EnableOpenGL];
    [self InitScene];
}
//----------------------------------------------------------------------------
- (void) dealloc
{
    if (m_pCallbackController)
        delete m_pCallbackController;

    [self DeleteScene];
    [self DisableOpenGL];
    
    if ([EAGLContext currentContext] == self.pContext)
        [EAGLContext setCurrentContext:nil];
}
//----------------------------------------------------------------------------
- (void) didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    
    if ([self isViewLoaded] && ([[self view]window] == nil))
    {
        self.view = nil;
        
        [self DeleteScene];
        [self DisableOpenGL];
        
        if ([EAGLContext currentContext] == self.pContext)
            [EAGLContext setCurrentContext:nil];
        
        self.pContext = nil;
    }
    
    // todo: dispose here any resource that can be recreated
}
//----------------------------------------------------------------------------
- (BOOL) prefersStatusBarHidden
{
    return YES;
}
//----------------------------------------------------------------------------
- (void)glkView :(GLKView*)view drawInRect:(CGRect)rect
{
    // calculate time interval
    const CFTimeInterval now            =  CACurrentMediaTime();
    const double         elapsedTime    = (now - m_PreviousTime);
                         m_PreviousTime =  now;
    
    [self UpdateScene :elapsedTime];
    [self DrawScene];
}
//----------------------------------------------------------------------------
- (void) EnableOpenGL
{
    self.pContext = [[EAGLContext alloc]initWithAPI:kEAGLRenderingAPIOpenGLES2];
    
    if (!self.pContext)
        NSLog(@"Failed to create ES context");
    
    GLKView* pView            = (GLKView*)self.view;
    pView.context             = self.pContext;
    pView.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    
    [EAGLContext setCurrentContext:self.pContext];
}
//----------------------------------------------------------------------------
- (void) DisableOpenGL
{
    [EAGLContext setCurrentContext:self.pContext];
}
//---------------------------------------------------------------------------
- (void*) OnGetShader :(const void*)pModel :(CSR_EModelType)type
{
    if (pModel == m_pScene->m_pSkybox)
        return m_pSkyboxShader;
    
    return m_pShader;
}
//---------------------------------------------------------------------------
- (void*) OnGetID :(const void*)pKey
{
    // iterate through resource ids
    for (std::size_t i = 0; i < 5; ++i)
        // found the texture to get?
        if (pKey == m_ID[i].m_pKey)
            return &m_ID[i];

    return 0;
}
//---------------------------------------------------------------------------
- (void) OnDeleteTexture :(const CSR_Texture*)pTexture
{
    // iterate through resource ids
    for (std::size_t i = 0; i < 3; ++i)
        // found the texture to delete?
        if (pTexture == m_ID[i].m_pKey)
        {
            // unuse the texture
            if (m_ID[i].m_UseCount)
                --m_ID[i].m_UseCount;
            
            // is texture no longer used?
            if (m_ID[i].m_UseCount)
                return;
            
            // delete the texture from the GPU
            if (m_ID[i].m_ID != M_CSR_Error_Code)
            {
                glDeleteTextures(1, (GLuint*)(&m_ID[i].m_ID));
                m_ID[i].m_ID = M_CSR_Error_Code;
            }
            
            return;
        }
}
//---------------------------------------------------------------------------
- (bool) LoadLandscapeFromBitmap :(const char*)pFileName
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
        return false;
    
    // load a default grayscale bitmap from which a landscape will be generated
    pBitmap = csrPixelBufferFromBitmapFile(pFileName);
    
    // succeeded?
    if (!pBitmap)
    {
        m_pCallbackController->ReleaseModel(pModel);
        return false;
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
    
    return true;
}
//---------------------------------------------------------------------------
- (bool) CheckForGoal :(CSR_Ball*)pBall :(const CSR_Vector3*)pOldPos :(const CSR_Vector3*)pDir
{
    if (!pBall || !pDir)
        return false;
    
    if (!pDir->m_X && !pDir->m_Y && !pDir->m_Z)
        return false;
    
    // is ball hitting the goal?
    if (pBall->m_Geometry.m_Center.m_X >= m_Goal.m_Bounds.m_Min.m_X &&
        pBall->m_Geometry.m_Center.m_X <= m_Goal.m_Bounds.m_Max.m_X &&
        pBall->m_Geometry.m_Center.m_Z >= m_Goal.m_Bounds.m_Min.m_Y &&
        pBall->m_Geometry.m_Center.m_Z <= m_Goal.m_Bounds.m_Max.m_Y)
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
        CSR_Segment3 ab;
        CSR_Segment3 bc;
        CSR_Segment3 cd;
        CSR_Segment3 da;
        
        // build the ab segment
        ab.m_Start.m_X = m_Goal.m_Bounds.m_Min.m_X;
        ab.m_Start.m_Y = 0.0f;
        ab.m_Start.m_Z = m_Goal.m_Bounds.m_Min.m_Y;
        ab.m_End.m_X   = m_Goal.m_Bounds.m_Max.m_X;
        ab.m_End.m_Y   = 0.0f;
        ab.m_End.m_Z   = m_Goal.m_Bounds.m_Min.m_Y;
        
        // build the bc segment
        bc.m_Start.m_X = m_Goal.m_Bounds.m_Max.m_X;
        bc.m_Start.m_Y = 0.0f;
        bc.m_Start.m_Z = m_Goal.m_Bounds.m_Min.m_Y;
        bc.m_End.m_X   = m_Goal.m_Bounds.m_Max.m_X;
        bc.m_End.m_Y   = 0.0f;
        bc.m_End.m_Z   = m_Goal.m_Bounds.m_Max.m_Y;
        
        // build the cd segment
        cd.m_Start.m_X = m_Goal.m_Bounds.m_Max.m_X;
        cd.m_Start.m_Y = 0.0f;
        cd.m_Start.m_Z = m_Goal.m_Bounds.m_Max.m_Y;
        cd.m_End.m_X   = m_Goal.m_Bounds.m_Min.m_X;
        cd.m_End.m_Y   = 0.0f;
        cd.m_End.m_Z   = m_Goal.m_Bounds.m_Max.m_Y;
        
        // build the da segment
        da.m_Start.m_X = m_Goal.m_Bounds.m_Min.m_X;
        da.m_Start.m_Y = 0.0f;
        da.m_Start.m_Z = m_Goal.m_Bounds.m_Max.m_Y;
        da.m_End.m_X   = m_Goal.m_Bounds.m_Min.m_X;
        da.m_End.m_Y   = 0.0f;
        da.m_End.m_Z   = m_Goal.m_Bounds.m_Min.m_Y;
        
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
- (bool) ApplyGroundCollision :(const CSR_Sphere*)pBoundingSphere
                              :(float)dir
                              :(CSR_Matrix4*)pMatrix
                              :(CSR_Plane*)pGroundPlane;
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
    camera.m_yAngle       =  dir;
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
    
    CSR_Matrix4 invertMatrix;
    float       determinant;
    
    // calculate the current camera position above the landscape
    csrMat4Inverse(pMatrix, &invertMatrix, &determinant);
    csrMat4Transform(&invertMatrix, &modelCenter, &collisionInput.m_BoundingSphere.m_Center);
    collisionInput.m_CheckPos = collisionInput.m_BoundingSphere.m_Center;
    
    CSR_CollisionOutput collisionOutput;
    
    // calculate the collisions in the whole scene
    csrSceneDetectCollision(m_pScene, &collisionInput, &collisionOutput, 0);
    
    // update the ground position directly inside the matrix (this is where the final value is required)
    pMatrix->m_Table[3][1] = -collisionOutput.m_GroundPos;
    
    // get the resulting plane
    *pGroundPlane = collisionOutput.m_GroundPlane;
    
    return (collisionOutput.m_Collision & CSR_CO_Ground);
}
//---------------------------------------------------------------------------
- (void) ApplyPhysics :(float)elapsedTime
{
    CSR_Plane   groundPlane;
    CSR_Vector3 planeNormal;
    CSR_Vector3 prevCenter;
    CSR_Vector3 ballDir;
    CSR_Vector3 ballDirN;
 
    // limit the time to 100ms otherwise the ball may move too quickly
    if (elapsedTime > 0.1f)
        elapsedTime = 0.1f;

    // apply the ground collision on the current position and get the ground polygon
    [self ApplyGroundCollision :&m_Ball.m_Geometry :0.0f :&m_Ball.m_Matrix :&groundPlane];
    
    // get the normal of the plane
    planeNormal.m_X = groundPlane.m_A;
    planeNormal.m_Y = groundPlane.m_B;
    planeNormal.m_Z = groundPlane.m_C;
    
    // calculate the next ball roll position
    csrPhysicsRoll(&planeNormal,
                    m_Ball.m_Body.m_Mass,
                    #if TARGET_IPHONE_SIMULATOR
                        0.0075f,
                    #else
                        0.005f,
                    #endif
                    elapsedTime,
                   &m_Ball.m_Body.m_Velocity);

    // keep the previous ball position
    prevCenter = m_Ball.m_Geometry.m_Center;
    
    // calculate the new position ( using the formula pos = pos + (v * dt))
    m_Ball.m_Geometry.m_Center.m_X += m_Ball.m_Body.m_Velocity.m_X * elapsedTime;
    m_Ball.m_Geometry.m_Center.m_Y += m_Ball.m_Body.m_Velocity.m_Y * elapsedTime;
    m_Ball.m_Geometry.m_Center.m_Z += m_Ball.m_Body.m_Velocity.m_Z * elapsedTime;
    
    // check if the new position is valid
    if (![self ApplyGroundCollision :&m_Ball.m_Geometry :0.0f :&m_Ball.m_Matrix :&groundPlane])
    {
        // do perform a rebound on the x or z axis?
        const int xRebound = m_Ball.m_Geometry.m_Center.m_X <= -3.08f || m_Ball.m_Geometry.m_Center.m_X >= 3.08f;
        const int zRebound = m_Ball.m_Geometry.m_Center.m_Z <= -3.08f || m_Ball.m_Geometry.m_Center.m_Z >= 3.08f;

        // reset the ball to the previous position
        m_Ball.m_Geometry.m_Center = prevCenter;
        
        // do perform a rebound on the x axis?
        if (xRebound)
            m_Ball.m_Body.m_Velocity.m_X = -m_Ball.m_Body.m_Velocity.m_X;
        
        // do perform a rebound on the z axis?
        if (zRebound)
            m_Ball.m_Body.m_Velocity.m_Z = -m_Ball.m_Body.m_Velocity.m_Z;
    }
    else
    {
        float       distance;
        CSR_Matrix4 rxMatrix;
        CSR_Matrix4 ryMatrix;
        CSR_Matrix4 ballMatrix;
        CSR_Vector3 rollDistance;
        CSR_Vector3 rollDir;
        CSR_Vector3 axis;
        
        // calculate the rolling angle (depends on the distance the ball moved)
        csrVec3Sub(&m_Ball.m_Geometry.m_Center, &prevCenter, &rollDistance);
        csrVec3Length(&rollDistance, &distance);
        m_RollAngle = std::fmod(m_RollAngle + (distance * 10.0f), M_PI * 2.0f);
        
        axis.m_X = 1.0f;
        axis.m_Y = 0.0f;
        axis.m_Z = 0.0f;
        
        // the ball moved since the last frame?
        if (distance)
        {
            // calculate the new ball direction angle
            csrVec3Normalize(&rollDistance, &rollDir);
            csrVec3Dot(&rollDir, &axis, &m_BallDirAngle);
        }
        
        // calculate the rotation matrix on the x axis
        csrMat4Rotate(m_RollAngle, &axis, &rxMatrix);
        
        axis.m_X = 0.0f;
        axis.m_Y = 1.0f;
        axis.m_Z = 0.0f;
        
        // ball moved on the z axis since the last frame?
        if (rollDistance.m_Z)
            // calculate the offset to apply to the ball direction
            m_BallOffset = (rollDistance.m_Z > 0.0f ? 1.0f : -1.0f);
        
        // calculate the rotation matrix on the y axis
        csrMat4Rotate((M_PI * 2.0f) - ((std::acos(m_BallDirAngle) * m_BallOffset) - (M_PI / 2.0f)),
                      &axis,
                      &ryMatrix);
        
        // build the final matrix
        csrMat4Multiply(&rxMatrix, &ryMatrix, &ballMatrix);
        
        // replace the ball in the model coordinate system (do that directly on the matrix)
        ballMatrix.m_Table[3][0] = -m_Ball.m_Matrix.m_Table[3][0];
        ballMatrix.m_Table[3][1] = -m_Ball.m_Matrix.m_Table[3][1];
        ballMatrix.m_Table[3][2] = -m_Ball.m_Matrix.m_Table[3][2];
        
        m_Ball.m_Matrix = ballMatrix;
    }
    
    // calculate the ball direction
    csrVec3Sub(&m_Ball.m_Geometry.m_Center, &prevCenter, &ballDir);
    csrVec3Normalize(&ballDir, &ballDirN);
    
    // check if the goal was hit
    if ([self CheckForGoal :&m_Ball :&prevCenter :&ballDirN])
        m_YouWonMatrix.m_Table[3][1] = 1.375f;
}
//---------------------------------------------------------------------------
- (void) Shoot
{
    CSR_Circle  playerCircle;
    CSR_Circle  ballCircle;
    CSR_Figure2 f1;
    CSR_Figure2 f2;
    
    // get the player position as a circle (i.e. ignore the y axis)
    playerCircle.m_Center.m_X = m_ViewSphere.m_Center.m_X;
    playerCircle.m_Center.m_Y = m_ViewSphere.m_Center.m_Z;
    playerCircle.m_Radius     = m_ViewSphere.m_Radius + 0.15f;
    
    // get the ball position as a circle (i.e. ignore the y axis)
    ballCircle.m_Center.m_X = m_Ball.m_Geometry.m_Center.m_X;
    ballCircle.m_Center.m_Y = m_Ball.m_Geometry.m_Center.m_Z;
    ballCircle.m_Radius     = m_Ball.m_Geometry.m_Radius;
    
    f1.m_Type    = CSR_F2_Circle;
    f1.m_pFigure = &playerCircle;
    
    f2.m_Type    = CSR_F2_Circle;
    f2.m_pFigure = &ballCircle;
    
    // check if the player is closer enough to the ball to shoot it
    if (csrIntersect2(&f1, &f2, 0, 0))
    {
        // calculate the direction and intensity of the shoot
        CSR_Vector2 distance;
        csrVec2Sub(&ballCircle.m_Center, &playerCircle.m_Center, &distance);
        
        // shoot the ball
        m_Ball.m_Body.m_Velocity.m_X = M_ShootEnergyFactor * distance.m_X;
        m_Ball.m_Body.m_Velocity.m_Y = 0.0f;
        m_Ball.m_Body.m_Velocity.m_Z = M_ShootEnergyFactor * distance.m_Y;
    }
}
//----------------------------------------------------------------------------
- (void) CreateViewport :(float)w :(float)h
{
    // calculate matrix items
    const float zNear  = 0.01f;
    const float zFar   = 100.0f;
    const float fov    = 45.0f;
    const float aspect = w / h;
    
    csrMat4Perspective(fov, aspect, zNear, zFar, &m_pScene->m_ProjectionMatrix);
    
    csrShaderEnable(m_pShader);
    
    // connect projection matrix to shader
    GLint projectionUniform = glGetUniformLocation(m_pShader->m_ProgramID, "csr_uProjection");
    glUniformMatrix4fv(projectionUniform, 1, 0, &m_pScene->m_ProjectionMatrix.m_Table[0][0]);
}
//----------------------------------------------------------------------------
- (void) InitScene
{
    CSR_VertexFormat vertexFormat;
    CSR_Material     material;
    CSR_PixelBuffer* pPixelBuffer = 0;
    CSR_Mesh*        pMesh;
    CSR_SceneItem*   pSceneItem;
    
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
    m_ViewSphere.m_Center.m_X = 3.08f;
    m_ViewSphere.m_Center.m_Y = 0.0f;
    m_ViewSphere.m_Center.m_Z = 3.08f;
    m_ViewSphere.m_Radius     = 0.1f;
    
    // set the ball bounding sphere default position
    m_Ball.m_Geometry.m_Center.m_X = 0.0f;
    m_Ball.m_Geometry.m_Center.m_Y = 0.0f;
    m_Ball.m_Geometry.m_Center.m_Z = 0.0f;
    m_Ball.m_Geometry.m_Radius     = 0.025f;
    csrBodyInit(&m_Ball.m_Body);
    
    // configure the friction force
    m_FrictionForce.m_X = 0.1f;
    m_FrictionForce.m_Y = 0.1f;
    m_FrictionForce.m_Z = 0.1f;
    
    std::string vertexShader   = CSR_ShaderHelper::GetVertexShader(CSR_ShaderHelper::IE_ST_Texture);
    std::string fragmentShader = CSR_ShaderHelper::GetFragmentShader(CSR_ShaderHelper::IE_ST_Texture);
    
    // compile, link and use shader
    m_pShader = csrOpenGLShaderLoadFromStr(vertexShader.c_str(),
                                           vertexShader.length(),
                                           fragmentShader.c_str(),
                                           fragmentShader.length(),
                                           0,
                                           0);

    // succeeded?
    if (!m_pShader)
        @throw @"Failed to load the scene shader";
    
    csrShaderEnable(m_pShader);
    
    // get shader attributes
    m_pShader->m_VertexSlot   = glGetAttribLocation(m_pShader->m_ProgramID, "csr_aVertices");
    m_pShader->m_ColorSlot    = glGetAttribLocation(m_pShader->m_ProgramID, "csr_aColor");
    m_pShader->m_TexCoordSlot = glGetAttribLocation(m_pShader->m_ProgramID, "csr_aTexCoord");
    m_pShader->m_TextureSlot  = glGetAttribLocation(m_pShader->m_ProgramID, "csr_sColorMap");
    
    // get the screen rect
    CGRect screenRect = [[UIScreen mainScreen]bounds];
    
    // create the viewport
    [self CreateViewport :screenRect.size.width :screenRect.size.height];

    // configure OpenGL depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRangef(0.0f, 1.0f);
    
    char* pFileName = 0;

    // get the resource file path
    [CSR_ObjectiveCHelper ResourceToFileName :@"level" :@"bmp" :&pFileName];

    // load the landscape
    if (![self LoadLandscapeFromBitmap :pFileName])
        @throw @"The landscape could not be loaded";
    
    free(pFileName);

    // get back the scene item containing the model
    pSceneItem = csrSceneGetItem(m_pScene, m_pLandscapeKey);
    
    // found it?
    if (!pSceneItem)
        @throw @"The landscape was not found in the scene";
    
    // get the resource file path
    [CSR_ObjectiveCHelper ResourceToFileName :@"soccer_grass" :@"bmp" :&pFileName];

    // load landscape texture
    pPixelBuffer       = csrPixelBufferFromBitmapFile(pFileName);
    m_ID[0].m_pKey     = &((CSR_Model*)(pSceneItem->m_pModel))->m_pMesh[0].m_Skin.m_Texture;
    m_ID[0].m_ID       = csrOpenGLTextureFromPixelBuffer(pPixelBuffer);
    m_ID[0].m_UseCount = 1;
    
    free(pFileName);

    // landscape texture will no longer be used
    csrPixelBufferRelease(pPixelBuffer);
    
    vertexFormat.m_HasNormal         = 0;
    vertexFormat.m_HasPerVertexColor = 1;
    vertexFormat.m_HasTexCoords      = 1;
    
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;
    
    // create the ball
    pMesh = csrShapeCreateSphere(m_Ball.m_Geometry.m_Radius,
                                 20,
                                 20,
                                 &vertexFormat,
                                 0,
                                 &material,
                                 0);
    
    // get the resource file path
    [CSR_ObjectiveCHelper ResourceToFileName :@"soccer_ball" :@"bmp" :&pFileName];

    // load ball texture
    pPixelBuffer       = csrPixelBufferFromBitmapFile(pFileName);
    m_ID[1].m_pKey     = &pMesh->m_Skin.m_Texture;
    m_ID[1].m_ID       = csrOpenGLTextureFromPixelBuffer(pPixelBuffer);
    m_ID[1].m_UseCount = 1;
    
    free(pFileName);

    // ball texture will no longer be used
    csrPixelBufferRelease(pPixelBuffer);
    
    // add the mesh to the scene
    pSceneItem = csrSceneAddMesh(m_pScene, pMesh, 0, 1);
    csrSceneAddModelMatrix(m_pScene, pMesh, &m_Ball.m_Matrix);
    
    // configure the ball particle
    m_Ball.m_pKey        = pSceneItem->m_pModel;
    m_Ball.m_Body.m_Mass = 0.3f;
    
    vertexFormat.m_HasNormal         = 0;
    vertexFormat.m_HasPerVertexColor = 1;
    vertexFormat.m_HasTexCoords      = 1;
    
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;
    
    // get the resource file path
    [CSR_ObjectiveCHelper ResourceToFileName :@"soccer_goal" :@"obj" :&pFileName];

    // create the goal
    CSR_Model* pModel = csrWaveFrontOpen(pFileName,
                                         &vertexFormat,
                                         0,
                                         &material,
                                         0,
                                         0,
                                         0);
    
    free(pFileName);

    CSR_Vector3 translation;
    translation.m_X =  0.0f;
    translation.m_Y =  1.375f;
    translation.m_Z = -1.75f;
    
    CSR_Matrix4 translationMatrix;
    
    // apply translation to goal
    csrMat4Translate(&translation, &translationMatrix);
    
    CSR_Vector3 axis;
    axis.m_X = 0.0f;
    axis.m_Y = 1.0f;
    axis.m_Z = 0.0f;
    
    CSR_Matrix4 ryMatrix;
    
    // apply rotation on y axis to goal
    csrMat4Rotate(M_PI, &axis, &ryMatrix);
    
    CSR_Vector3 factor;
    factor.m_X = 0.0025f;
    factor.m_Y = 0.0025f;
    factor.m_Z = 0.0025f;
    
    CSR_Matrix4 scaleMatrix;
    
    // apply scaling to goal
    csrMat4Scale(&factor, &scaleMatrix);
    
    CSR_Matrix4 buildMatrix;
    
    // build the goal model matrix
    csrMat4Multiply(&ryMatrix, &scaleMatrix, &buildMatrix);
    csrMat4Multiply(&buildMatrix, &translationMatrix, &m_Goal.m_Matrix);
    
    // add the model to the scene
    pSceneItem = csrSceneAddModel(m_pScene, pModel, 0, 1);
    csrSceneAddModelMatrix(m_pScene, pModel, &m_Goal.m_Matrix);
    
    // get the resource file path
    [CSR_ObjectiveCHelper ResourceToFileName :@"soccer_goal" :@"bmp" :&pFileName];

    // load goal texture
    pPixelBuffer       = csrPixelBufferFromBitmapFile(pFileName);
    m_ID[2].m_pKey     = &pMesh->m_Skin.m_Texture;
    m_ID[2].m_ID       = csrOpenGLTextureFromPixelBuffer(pPixelBuffer);
    m_ID[2].m_UseCount = 1;
    
    free(pFileName);

    // goal texture will no longer be used
    csrPixelBufferRelease(pPixelBuffer);
    
    CSR_Box goalBox;
    
    // transform the goal bounding box in his local system coordinates
    csrMat4ApplyToVector(&m_Goal.m_Matrix, &pSceneItem->m_pAABBTree[0].m_pBox->m_Min, &goalBox.m_Min);
    csrMat4ApplyToVector(&m_Goal.m_Matrix, &pSceneItem->m_pAABBTree[0].m_pBox->m_Max, &goalBox.m_Max);
    
    // configure the goal
    m_Goal.m_pKey = pSceneItem->m_pModel;
    csrMathMin(goalBox.m_Min.m_X, goalBox.m_Max.m_X, &m_Goal.m_Bounds.m_Min.m_X);
    csrMathMin(goalBox.m_Min.m_Z, goalBox.m_Max.m_Z, &m_Goal.m_Bounds.m_Min.m_Y);
    csrMathMax(goalBox.m_Min.m_X, goalBox.m_Max.m_X, &m_Goal.m_Bounds.m_Max.m_X);
    csrMathMax(goalBox.m_Min.m_Z, goalBox.m_Max.m_Z, &m_Goal.m_Bounds.m_Max.m_Y );
    
    vertexFormat.m_HasNormal         = 0;
    vertexFormat.m_HasPerVertexColor = 1;
    vertexFormat.m_HasTexCoords      = 1;
    
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;
    
    // create the You Won surface
    pMesh = csrShapeCreateSurface(0.6f, 0.2f, &vertexFormat, 0, &material, 0);
    
    // get the resource file path
    [CSR_ObjectiveCHelper ResourceToFileName :@"you_won" :@"bmp" :&pFileName];

    // load the You Won texture
    pPixelBuffer       = csrPixelBufferFromBitmapFile(pFileName);
    m_ID[3].m_pKey     = &pMesh->m_Skin.m_Texture;
    m_ID[3].m_ID       = csrOpenGLTextureFromPixelBuffer(pPixelBuffer);
    m_ID[3].m_UseCount = 1;
    
    free(pFileName);

    // goal texture will no longer be used
    csrPixelBufferRelease(pPixelBuffer);
    
    // initialize the You Won matrix
    csrMat4Identity(&m_YouWonMatrix);
    m_YouWonMatrix.m_Table[3][0] =  0.0f;
    m_YouWonMatrix.m_Table[3][1] =  99999.0f;
    m_YouWonMatrix.m_Table[3][2] = -1.65f;
    
    // add the mesh to the scene
    pSceneItem = csrSceneAddMesh(m_pScene, pMesh, 0, 1);
    csrSceneAddModelMatrix(m_pScene, pMesh, &m_YouWonMatrix);
    
    vertexShader   = CSR_ShaderHelper::GetVertexShader(CSR_ShaderHelper::IE_ST_Skybox);
    fragmentShader = CSR_ShaderHelper::GetFragmentShader(CSR_ShaderHelper::IE_ST_Skybox);
    
    // load the skybox shader
    m_pSkyboxShader = csrOpenGLShaderLoadFromStr(vertexShader.c_str(),
                                                 vertexShader.length(),
                                                 fragmentShader.c_str(),
                                                 fragmentShader.length(),
                                                 0,
                                                 0);

    // succeeded?
    if (!m_pSkyboxShader)
        @throw @"Failed to load the skybox shader";
    
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
        // show the error message to the user
        printf("Failed to create the skybox.\n");
        return;
    }
    
    char* pCubemapFileNames[6];

    // get the resource file path
    [CSR_ObjectiveCHelper ResourceToFileName :@"skybox_right_small"  :@"bmp" :&pCubemapFileNames[0]];
    [CSR_ObjectiveCHelper ResourceToFileName :@"skybox_left_small"   :@"bmp" :&pCubemapFileNames[1]];
    [CSR_ObjectiveCHelper ResourceToFileName :@"skybox_top_small"    :@"bmp" :&pCubemapFileNames[2]];
    [CSR_ObjectiveCHelper ResourceToFileName :@"skybox_bottom_small" :@"bmp" :&pCubemapFileNames[3]];
    [CSR_ObjectiveCHelper ResourceToFileName :@"skybox_front_small"  :@"bmp" :&pCubemapFileNames[4]];
    [CSR_ObjectiveCHelper ResourceToFileName :@"skybox_back_small"   :@"bmp" :&pCubemapFileNames[5]];

    // load the cubemap texture
    m_ID[4].m_pKey     = &m_pScene->m_pSkybox->m_Skin.m_CubeMap;
    m_ID[4].m_ID       = csrOpenGLCubemapLoad((const char**)pCubemapFileNames);
    m_ID[4].m_UseCount = 1;
    
    free(pCubemapFileNames[0]);
    free(pCubemapFileNames[1]);
    free(pCubemapFileNames[2]);
    free(pCubemapFileNames[3]);
    free(pCubemapFileNames[4]);
    free(pCubemapFileNames[5]);

    csrSoundInitializeOpenAL(&m_pOpenALDevice, &m_pOpenALContext);
    
    // get the resource file path
    [CSR_ObjectiveCHelper ResourceToFileName :@"human_walk_grass_step" :@"wav" :&pFileName];

    // load step sound file
    m_pSound = csrSoundOpenWavFile(m_pOpenALDevice, m_pOpenALContext, pFileName);

    free(pFileName);
}
//----------------------------------------------------------------------------
- (void) DeleteScene
{
    // delete the scene
    m_pCallbackController->ReleaseScene(m_pScene);
    m_pScene = nil;
    
    // delete scene shader
    csrOpenGLShaderRelease(m_pShader);
    m_pShader = nil;
    
    // delete skybox shader
    csrOpenGLShaderRelease(m_pSkyboxShader);
    m_pSkyboxShader = nil;
    
    // stop running step sound, if needed
    csrSoundStop(m_pSound);
    
    // release OpenAL interface
    csrSoundRelease(m_pSound);
    csrSoundReleaseOpenAL(m_pOpenALDevice, m_pOpenALContext);
}
//----------------------------------------------------------------------------
- (void) UpdateScene :(float)elapsedTime
{
    float      angle;
    CSR_Sphere prevSphere;
    CSR_Plane  groundPlane;
    
    [self ApplyPhysics :elapsedTime];
    
    // if screen isn't touched, do nothing
    if (!m_TouchOrigin.m_X || !m_TouchOrigin.m_Y)
        return;
    
    prevSphere = m_ViewSphere;
    
    // calculate the angle formed by the touch gesture x and y distances
    if ((m_TouchPosition.m_X < m_TouchOrigin.m_X || m_TouchPosition.m_Y < m_TouchOrigin.m_Y) &&
        !(m_TouchPosition.m_X < m_TouchOrigin.m_X && m_TouchPosition.m_Y < m_TouchOrigin.m_Y))
        angle = -atanf((m_TouchPosition.m_Y - m_TouchOrigin.m_Y) / (m_TouchPosition.m_X - m_TouchOrigin.m_X));
    else
        angle =  atanf((m_TouchPosition.m_Y - m_TouchOrigin.m_Y) / (m_TouchPosition.m_X - m_TouchOrigin.m_X));
    
    // calculate the possible min and max values for each axis
    float minX = m_TouchOrigin.m_X - (cosf(angle) * m_ControlRadius);
    float maxX = m_TouchOrigin.m_X + (cosf(angle) * m_ControlRadius);
    float minY = m_TouchOrigin.m_Y - (sinf(angle) * m_ControlRadius);
    float maxY = m_TouchOrigin.m_Y + (sinf(angle) * m_ControlRadius);
    
    // limit the touch gesture in a radius distance
    if (m_TouchPosition.m_X > maxX)
        m_TouchPosition.m_X = maxX;
    else
    if (m_TouchPosition.m_X < minX)
        m_TouchPosition.m_X = minX;

    if (m_TouchPosition.m_Y > maxY)
        m_TouchPosition.m_Y = maxY;
    else
    if (m_TouchPosition.m_Y < minY)
        m_TouchPosition.m_Y = minY;

    // calculate the final pos and dir velocity
    const float posVelocity = (m_PosVelocity * ((m_TouchPosition.m_Y - m_TouchOrigin.m_Y) / m_TouchOrigin.m_Y));
    const float dirVelocity = (m_DirVelocity * ((m_TouchPosition.m_X - m_TouchOrigin.m_X) / m_TouchOrigin.m_X));
    
    // calculate the next player direction
    m_Angle += dirVelocity * elapsedTime;
    
    // validate it
    if (m_Angle > M_PI * 2.0f)
        m_Angle -= M_PI * 2.0f;
    else
    if (m_Angle < 0.0f)
        m_Angle += M_PI * 2.0f;
    
    // calculate the next player position
    m_ViewSphere.m_Center.m_X += posVelocity * cosf(m_Angle + (M_PI * 0.5f)) * elapsedTime;
    m_ViewSphere.m_Center.m_Z += posVelocity * sinf(m_Angle + (M_PI * 0.5f)) * elapsedTime;
    
    // calculate the ground position and check if next position is valid
    if (![self ApplyGroundCollision :&m_ViewSphere :m_Angle :&m_pScene->m_ViewMatrix :&groundPlane])
    {
        // invalid next position, get the scene item (just one for this scene)
        const CSR_SceneItem* pItem = csrSceneGetItem(m_pScene, m_pLandscapeKey);
        
        // found it?
        if (pItem)
        {
            // check if the x position is out of bounds, and correct it if yes
            if (m_ViewSphere.m_Center.m_X <= pItem->m_pAABBTree->m_pBox->m_Min.m_X ||
                m_ViewSphere.m_Center.m_X >= pItem->m_pAABBTree->m_pBox->m_Max.m_X)
                m_ViewSphere.m_Center.m_X = prevSphere.m_Center.m_X;
            
            // do the same thing with the z position. Doing that separately for each axis will make
            // the point of view to slide against the landscape border (this is possible because the
            // landscape is axis-aligned)
            if (m_ViewSphere.m_Center.m_Z <= pItem->m_pAABBTree->m_pBox->m_Min.m_Z ||
                m_ViewSphere.m_Center.m_Z >= pItem->m_pAABBTree->m_pBox->m_Max.m_Z)
                m_ViewSphere.m_Center.m_Z = prevSphere.m_Center.m_Z;
        }
        else
            // failed to get the scene item, just revert the position
            m_ViewSphere.m_Center = prevSphere.m_Center;
        
        // recalculate the ground value (this time the collision result isn't tested, because the
        // previous position is always considered as valid)
        [self ApplyGroundCollision :&m_ViewSphere :m_Angle :&m_pScene->m_ViewMatrix :&groundPlane];
    }
    else
    {
        float       groundAngle;
        CSR_Vector3 slopeDir;
        
        // get the slope direction
        slopeDir.m_X = groundPlane.m_A;
        slopeDir.m_Y = groundPlane.m_B;
        slopeDir.m_Z = groundPlane.m_C;
        
        // calculate the slope angle
        csrVec3Dot(&m_pScene->m_GroundDir, &slopeDir, &groundAngle);
        
        // is the slope too inclined to allow the player to walk on it?
        if (fabs(groundAngle) < 0.5f)
            // revert the position
            m_ViewSphere.m_Center = prevSphere.m_Center;
        else
        if (m_ViewSphere.m_Center.m_X >= m_Goal.m_Bounds.m_Min.m_X && m_ViewSphere.m_Center.m_X <= m_Goal.m_Bounds.m_Max.m_X &&
            m_ViewSphere.m_Center.m_Z >= m_Goal.m_Bounds.m_Min.m_Y && m_ViewSphere.m_Center.m_Z <= m_Goal.m_Bounds.m_Max.m_Y)
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
            CSR_Segment3 ab;
            CSR_Segment3 bc;
            CSR_Segment3 cd;
            CSR_Segment3 da;
            
            // build the ab segment
            ab.m_Start.m_X = m_Goal.m_Bounds.m_Min.m_X;
            ab.m_Start.m_Y = 0.0f;
            ab.m_Start.m_Z = m_Goal.m_Bounds.m_Min.m_Y;
            ab.m_End.m_X   = m_Goal.m_Bounds.m_Max.m_X;
            ab.m_End.m_Y   = 0.0f;
            ab.m_End.m_Z   = m_Goal.m_Bounds.m_Min.m_Y;
            
            // build the bc segment
            bc.m_Start.m_X = m_Goal.m_Bounds.m_Max.m_X;
            bc.m_Start.m_Y = 0.0f;
            bc.m_Start.m_Z = m_Goal.m_Bounds.m_Min.m_Y;
            bc.m_End.m_X   = m_Goal.m_Bounds.m_Max.m_X;
            bc.m_End.m_Y   = 0.0f;
            bc.m_End.m_Z   = m_Goal.m_Bounds.m_Max.m_Y;
            
            // build the cd segment
            cd.m_Start.m_X = m_Goal.m_Bounds.m_Max.m_X;
            cd.m_Start.m_Y = 0.0f;
            cd.m_Start.m_Z = m_Goal.m_Bounds.m_Max.m_Y;
            cd.m_End.m_X   = m_Goal.m_Bounds.m_Min.m_X;
            cd.m_End.m_Y   = 0.0f;
            cd.m_End.m_Z   = m_Goal.m_Bounds.m_Max.m_Y;
            
            // build the da segment
            da.m_Start.m_X = m_Goal.m_Bounds.m_Min.m_X;
            da.m_Start.m_Y = 0.0f;
            da.m_Start.m_Z = m_Goal.m_Bounds.m_Max.m_Y;
            da.m_End.m_X   = m_Goal.m_Bounds.m_Min.m_X;
            da.m_End.m_Y   = 0.0f;
            da.m_End.m_Z   = m_Goal.m_Bounds.m_Min.m_Y;
            
            CSR_Vector3 ptAB;
            CSR_Vector3 ptBC;
            CSR_Vector3 ptCD;
            CSR_Vector3 ptDA;
            
            // calculate the closest point from previous position to each of the segments
            csrSeg3ClosestPoint(&ab, &prevSphere.m_Center, &ptAB);
            csrSeg3ClosestPoint(&bc, &prevSphere.m_Center, &ptBC);
            csrSeg3ClosestPoint(&cd, &prevSphere.m_Center, &ptCD);
            csrSeg3ClosestPoint(&da, &prevSphere.m_Center, &ptDA);
            
            CSR_Vector3 PPtAB;
            CSR_Vector3 PPtBC;
            CSR_Vector3 PPtCD;
            CSR_Vector3 PPtDA;
            
            // calculate each distances between the previous point and each points found on segments
            csrVec3Sub(&ptAB, &prevSphere.m_Center, &PPtAB);
            csrVec3Sub(&ptBC, &prevSphere.m_Center, &PPtBC);
            csrVec3Sub(&ptCD, &prevSphere.m_Center, &PPtCD);
            csrVec3Sub(&ptDA, &prevSphere.m_Center, &PPtDA);
            
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
                m_ViewSphere.m_Center.m_Z = prevSphere.m_Center.m_Z;
            else
            if (lbc < lab && lbc < lcd && lbc < lda)
                m_ViewSphere.m_Center.m_X = prevSphere.m_Center.m_X;
            else
            if (lcd < lab && lcd < lbc && lcd < lda)
                m_ViewSphere.m_Center.m_Z = prevSphere.m_Center.m_Z;
            else
            if (lda < lab && lda < lbc && lda < lcd)
                m_ViewSphere.m_Center.m_X = prevSphere.m_Center.m_X;
            else
            {
                m_ViewSphere.m_Center.m_X = prevSphere.m_Center.m_X;
                m_ViewSphere.m_Center.m_Z = prevSphere.m_Center.m_Z;
            }
            
            // recalculate the ground value (this time the collision result isn't tested, because
            // the previous position is always considered as valid)
            [self ApplyGroundCollision :&m_ViewSphere :m_Angle :&m_pScene->m_ViewMatrix :&groundPlane];
        }
    }
    
    // calculate next time where the step sound should be played
    m_StepTime += (elapsedTime * 1000.0f);
    
    // count frames
    while (m_StepTime > m_StepInterval)
    {
        csrSoundStop(m_pSound);
        csrSoundPlay(m_pSound);
        m_StepTime = 0.0f;
    }
}
//----------------------------------------------------------------------------
- (void) DrawScene;
{
    CSR_Plane groundPlane;
    
    // finalize the view matrix
    [self ApplyGroundCollision :&m_ViewSphere :m_Angle :&m_pScene->m_ViewMatrix :&groundPlane];
    
    // draw the scene
    m_pCallbackController->DrawScene(m_pScene);
}
//----------------------------------------------------------------------------
- (void)OnLongPress :(UIGestureRecognizer*)pSender
{
    const CGPoint touchPos = [pSender locationInView :nil];
    
    switch (pSender.state)
    {
        case UIGestureRecognizerStateBegan:
            // initialize the position. NOTE inverted because the screen is in landscape mode
            m_TouchOrigin.m_X   = touchPos.y;
            m_TouchOrigin.m_Y   = touchPos.x;
            m_TouchPosition.m_X = touchPos.y;
            m_TouchPosition.m_Y = touchPos.x;
            break;
            
        case UIGestureRecognizerStateChanged:
            // get the next position. NOTE inverted because the screen is in landscape mode
            m_TouchPosition.m_X = touchPos.y;
            m_TouchPosition.m_Y = touchPos.x;
            break;
            
        case UIGestureRecognizerStateEnded:
            if (m_TouchPosition.m_X == m_TouchOrigin.m_X && m_TouchPosition.m_Y == m_TouchOrigin.m_Y)
                [self Shoot];

            // reset the position
            m_TouchOrigin.m_X   = 0;
            m_TouchOrigin.m_Y   = 0;
            m_TouchPosition.m_X = 0;
            m_TouchPosition.m_Y = 0;
            break;
            
        default:
            break;
    }
}
//----------------------------------------------------------------------------
@end
//----------------------------------------------------------------------------
