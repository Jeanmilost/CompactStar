/****************************************************************************
 * ==> GameLogic -----------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides the Wild Soccer game logic            *
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

#import "GameLogic.h"

// std
#include <math.h>

// classes
#import "CSR_ObjectiveCHelper.h"

// energy factor for the shoot
#ifdef CSR_USE_METAL
    #define M_ShootEnergyFactor 25.0f
#else
    #define M_ShootEnergyFactor 23.0f
#endif

//---------------------------------------------------------------------------
// CSR_GameLogic
//---------------------------------------------------------------------------
@implementation CSR_GameLogic
{}
//----------------------------------------------------------------------------
@synthesize m_pBall;
@synthesize m_pGoal;
@synthesize m_pViewSphere;
@synthesize m_pYouWonMatrix;
@synthesize m_pTouchOrigin;
@synthesize m_pTouchPosition;
@synthesize m_pFootStepLeftSound;
@synthesize m_pFootStepRightSound;
@synthesize m_pBallKickSound;
//----------------------------------------------------------------------------
- (instancetype)init
{
    if (self = [super init])
    {
        m_RollAngle           = 0.0f;
        m_BallDirAngle        = 0.0f;
        m_BallOffset          = 0.0f;
        m_pBall               = (CSR_Ball*)malloc(sizeof(CSR_Ball));
        m_pGoal               = (CSR_Goal*)malloc(sizeof(CSR_Goal));
        m_pViewSphere         = (CSR_Sphere*)malloc(sizeof(CSR_Sphere));
        m_pYouWonMatrix       = (CSR_Matrix4*)malloc(sizeof(CSR_Matrix4));
        m_pOpenALDevice       = nil;
        m_pOpenALContext      = nil;
        m_pTouchOrigin        = (CSR_Vector2*)malloc(sizeof(CSR_Vector2));
        m_pTouchPosition      = (CSR_Vector2*)malloc(sizeof(CSR_Vector2));
        m_pFootStepLeftSound  = nil;
        m_pFootStepRightSound = nil;
        m_pBallKickSound      = nil;

        // set the viewpoint bounding sphere default position
        m_pViewSphere->m_Center.m_X = 3.08f;
        m_pViewSphere->m_Center.m_Y = 0.0f;
        m_pViewSphere->m_Center.m_Z = 3.08f;
        m_pViewSphere->m_Radius     = 0.1f;
        
        // set the ball bounding sphere default position
        m_pBall->m_Geometry.m_Center.m_X = 0.0f;
        m_pBall->m_Geometry.m_Center.m_Y = 0.0f;
        m_pBall->m_Geometry.m_Center.m_Z = 0.0f;
        m_pBall->m_Geometry.m_Radius     = 0.025f;
        csrBodyInit(&m_pBall->m_Body);

        // initialize the You Won matrix
        csrMat4Identity(m_pYouWonMatrix);
        m_pYouWonMatrix->m_Table[3][0] =  0.0f;
        m_pYouWonMatrix->m_Table[3][1] =  99999.0f;
        m_pYouWonMatrix->m_Table[3][2] = -1.65f;

        csrSoundInitializeOpenAL(&m_pOpenALDevice, &m_pOpenALContext);
        
        char* pFileName = 0;

        // get the left footstep sound resource file path
        [CSR_ObjectiveCHelper ResourceToFileName :@"footstep_left" :@"wav" :&pFileName];
        
        // load left footstep sound file
        m_pFootStepLeftSound = csrSoundOpenWavFile(m_pOpenALDevice, m_pOpenALContext, pFileName);

        // get the right footstep sound resource file path
        [CSR_ObjectiveCHelper ResourceToFileName :@"footstep_right" :@"wav" :&pFileName];
        
        // load right footstep sound file
        m_pFootStepRightSound = csrSoundOpenWavFile(m_pOpenALDevice, m_pOpenALContext, pFileName);

        // get the ball kick sound resource file path
        [CSR_ObjectiveCHelper ResourceToFileName :@"soccer_ball_kick" :@"wav" :&pFileName];
        
        // load ball kick sound file
        m_pBallKickSound = csrSoundOpenWavFile(m_pOpenALDevice, m_pOpenALContext, pFileName);

        free(pFileName);

        // configure the footstep sounds volume
        csrSoundChangeVolume(m_pFootStepLeftSound,  0.2f);
        csrSoundChangeVolume(m_pFootStepRightSound, 0.2f);
    }

    return self;
}
//----------------------------------------------------------------------------
- (void) dealloc
{
    if (m_pBall)
        free(m_pBall);
    
    if (m_pGoal)
        free(m_pGoal);

    if (m_pViewSphere)
        free(m_pViewSphere);

    if (m_pYouWonMatrix)
        free(m_pYouWonMatrix);

    if(m_pTouchOrigin)
        free(m_pTouchOrigin);
    
    if(m_pTouchPosition)
        free(m_pTouchPosition);

    // stop running sound, if needed
    csrSoundStop(m_pFootStepLeftSound);
    csrSoundStop(m_pFootStepRightSound);
    csrSoundStop(m_pBallKickSound);

    // release OpenAL interface
    csrSoundRelease(m_pFootStepLeftSound);
    csrSoundRelease(m_pFootStepRightSound);
    csrSoundRelease(m_pBallKickSound);
    csrSoundReleaseOpenAL(m_pOpenALDevice, m_pOpenALContext);

    // useless because using ARC
    //[super dealloc];
}
//----------------------------------------------------------------------------
- (bool) ApplyGroundCollision :(CSR_Scene* _Nullable)pScene
                              :(const CSR_Sphere*)pBoundingSphere
                              :(float)dir
                              :(CSR_Matrix4*)pMatrix
                              :(CSR_Plane*)pGroundPlane;
{
    if (!pScene)
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
    csrSceneDetectCollision(pScene, &collisionInput, &collisionOutput, 0);
    
    // update the ground position directly inside the matrix (this is where the final value is required)
    pMatrix->m_Table[3][1] = -collisionOutput.m_GroundPos;
    
    // get the resulting plane
    *pGroundPlane = collisionOutput.m_GroundPlane;
    
    return (collisionOutput.m_Collision & CSR_CO_Ground);
}
//----------------------------------------------------------------------------
- (void) ApplyPhysics :(CSR_Scene* _Nullable)pScene :(float)elapsedTime
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
    [self ApplyGroundCollision :pScene :&m_pBall->m_Geometry :0.0f :&m_pBall->m_Matrix :&groundPlane];
    
    // get the normal of the plane
    planeNormal.m_X = groundPlane.m_A;
    planeNormal.m_Y = groundPlane.m_B;
    planeNormal.m_Z = groundPlane.m_C;
    
    // calculate the next ball roll position
    csrPhysicsRoll(&planeNormal,
                    m_pBall->m_Body.m_Mass,
                    #ifdef CSR_USE_METAL
                        0.0035f,
                    #elif TARGET_IPHONE_SIMULATOR
                        0.0075f,
                    #else
                        0.005f,
                    #endif
                    elapsedTime,
                   &m_pBall->m_Body.m_Velocity);
    
    // keep the previous ball position
    prevCenter = m_pBall->m_Geometry.m_Center;
    
    // calculate the new position ( using the formula pos = pos + (v * dt))
    m_pBall->m_Geometry.m_Center.m_X += m_pBall->m_Body.m_Velocity.m_X * elapsedTime;
    m_pBall->m_Geometry.m_Center.m_Y += m_pBall->m_Body.m_Velocity.m_Y * elapsedTime;
    m_pBall->m_Geometry.m_Center.m_Z += m_pBall->m_Body.m_Velocity.m_Z * elapsedTime;
    
    // check if the new position is valid
    if (![self ApplyGroundCollision :pScene :&m_pBall->m_Geometry :0.0f :&m_pBall->m_Matrix :&groundPlane])
    {
        // do perform a rebound on the x or z axis?
        const int xRebound = m_pBall->m_Geometry.m_Center.m_X <= -3.08f || m_pBall->m_Geometry.m_Center.m_X >= 3.08f;
        const int zRebound = m_pBall->m_Geometry.m_Center.m_Z <= -3.08f || m_pBall->m_Geometry.m_Center.m_Z >= 3.08f;
        
        // reset the ball to the previous position
        m_pBall->m_Geometry.m_Center = prevCenter;
        
        // do perform a rebound on the x axis?
        if (xRebound)
            m_pBall->m_Body.m_Velocity.m_X = -m_pBall->m_Body.m_Velocity.m_X;
        
        // do perform a rebound on the z axis?
        if (zRebound)
            m_pBall->m_Body.m_Velocity.m_Z = -m_pBall->m_Body.m_Velocity.m_Z;
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
        csrVec3Sub(&m_pBall->m_Geometry.m_Center, &prevCenter, &rollDistance);
        csrVec3Length(&rollDistance, &distance);
        m_RollAngle = fmod(m_RollAngle + (distance * 10.0f), M_PI * 2.0f);
        
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
        csrMat4Rotate((M_PI * 2.0f) - ((acos(m_BallDirAngle) * m_BallOffset) - (M_PI / 2.0f)),
                      &axis,
                      &ryMatrix);
        
        // build the final matrix
        csrMat4Multiply(&rxMatrix, &ryMatrix, &ballMatrix);
        
        // replace the ball in the model coordinate system (do that directly on the matrix)
        ballMatrix.m_Table[3][0] = -m_pBall->m_Matrix.m_Table[3][0];
        ballMatrix.m_Table[3][1] = -m_pBall->m_Matrix.m_Table[3][1];
        ballMatrix.m_Table[3][2] = -m_pBall->m_Matrix.m_Table[3][2];
        
        m_pBall->m_Matrix = ballMatrix;
    }
    
    // calculate the ball direction
    csrVec3Sub(&m_pBall->m_Geometry.m_Center, &prevCenter, &ballDir);
    csrVec3Normalize(&ballDir, &ballDirN);
    
    // check if the goal was hit
    if ([self CheckForGoal :&prevCenter :&ballDirN])
        m_pYouWonMatrix->m_Table[3][1] = 1.375f;
}
//----------------------------------------------------------------------------
- (void) Shoot
{
    CSR_Circle  playerCircle;
    CSR_Circle  ballCircle;
    CSR_Figure2 f1;
    CSR_Figure2 f2;
    
    // get the player position as a circle (i.e. ignore the y axis)
    playerCircle.m_Center.m_X = m_pViewSphere->m_Center.m_X;
    playerCircle.m_Center.m_Y = m_pViewSphere->m_Center.m_Z;
    playerCircle.m_Radius     = m_pViewSphere->m_Radius + 0.15f;
    
    // get the ball position as a circle (i.e. ignore the y axis)
    ballCircle.m_Center.m_X = m_pBall->m_Geometry.m_Center.m_X;
    ballCircle.m_Center.m_Y = m_pBall->m_Geometry.m_Center.m_Z;
    ballCircle.m_Radius     = m_pBall->m_Geometry.m_Radius;
    
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
        m_pBall->m_Body.m_Velocity.m_X = M_ShootEnergyFactor * distance.m_X;
        m_pBall->m_Body.m_Velocity.m_Y = 0.0f;
        m_pBall->m_Body.m_Velocity.m_Z = M_ShootEnergyFactor * distance.m_Y;

        // play the kick sound
        csrSoundStop(m_pBallKickSound);
        csrSoundPlay(m_pBallKickSound);
    }
}
//---------------------------------------------------------------------------
- (bool) CheckForGoal :(const CSR_Vector3* _Nullable)pOldPos
                      :(const CSR_Vector3* _Nullable)pDir
{
    if (!m_pBall || !pDir)
        return false;
    
    if (!pDir->m_X && !pDir->m_Y && !pDir->m_Z)
        return false;
    
    // is ball hitting the goal?
    if (m_pBall->m_Geometry.m_Center.m_X >= m_pGoal->m_Bounds.m_Min.m_X &&
        m_pBall->m_Geometry.m_Center.m_X <= m_pGoal->m_Bounds.m_Max.m_X &&
        m_pBall->m_Geometry.m_Center.m_Z >= m_pGoal->m_Bounds.m_Min.m_Y &&
        m_pBall->m_Geometry.m_Center.m_Z <= m_pGoal->m_Bounds.m_Max.m_Y)
        switch ([self GetGoalHitEdge :pOldPos])
        {
            case IE_E_Top:
                m_pBall->m_Body.m_Velocity.m_Z = -m_pBall->m_Body.m_Velocity.m_Z;
                break;
                
            case IE_E_Bottom:
                m_pBall->m_Body.m_Velocity.m_Z = -m_pBall->m_Body.m_Velocity.m_Z;
                return true;
                
            case IE_E_Left:
            case IE_E_Right:
                m_pBall->m_Body.m_Velocity.m_X = -m_pBall->m_Body.m_Velocity.m_X;
                break;
                
            default:
                m_pBall->m_Body.m_Velocity.m_X = -m_pBall->m_Body.m_Velocity.m_X;
                m_pBall->m_Body.m_Velocity.m_Z = -m_pBall->m_Body.m_Velocity.m_Z;
                break;
        }
    
    return false;
}
//---------------------------------------------------------------------------
- (CSR_EEdge) GetGoalHitEdge :(const CSR_Vector3* _Nullable)pOldPos
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
    ab.m_Start.m_X = m_pGoal->m_Bounds.m_Min.m_X;
    ab.m_Start.m_Y = 0.0f;
    ab.m_Start.m_Z = m_pGoal->m_Bounds.m_Min.m_Y;
    ab.m_End.m_X   = m_pGoal->m_Bounds.m_Max.m_X;
    ab.m_End.m_Y   = 0.0f;
    ab.m_End.m_Z   = m_pGoal->m_Bounds.m_Min.m_Y;
    
    // build the bc segment
    bc.m_Start.m_X = m_pGoal->m_Bounds.m_Max.m_X;
    bc.m_Start.m_Y = 0.0f;
    bc.m_Start.m_Z = m_pGoal->m_Bounds.m_Min.m_Y;
    bc.m_End.m_X   = m_pGoal->m_Bounds.m_Max.m_X;
    bc.m_End.m_Y   = 0.0f;
    bc.m_End.m_Z   = m_pGoal->m_Bounds.m_Max.m_Y;
    
    // build the cd segment
    cd.m_Start.m_X = m_pGoal->m_Bounds.m_Max.m_X;
    cd.m_Start.m_Y = 0.0f;
    cd.m_Start.m_Z = m_pGoal->m_Bounds.m_Max.m_Y;
    cd.m_End.m_X   = m_pGoal->m_Bounds.m_Min.m_X;
    cd.m_End.m_Y   = 0.0f;
    cd.m_End.m_Z   = m_pGoal->m_Bounds.m_Max.m_Y;
    
    // build the da segment
    da.m_Start.m_X = m_pGoal->m_Bounds.m_Min.m_X;
    da.m_Start.m_Y = 0.0f;
    da.m_Start.m_Z = m_pGoal->m_Bounds.m_Max.m_Y;
    da.m_End.m_X   = m_pGoal->m_Bounds.m_Min.m_X;
    da.m_End.m_Y   = 0.0f;
    da.m_End.m_Z   = m_pGoal->m_Bounds.m_Min.m_Y;
    
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
    
    // find on which edge the player is hitting the goal
    if (lab < lbc && lab < lcd && lab < lda)
        return IE_E_Top;
    else
    if (lbc < lab && lbc < lcd && lbc < lda)
        return IE_E_Left;
    else
    if (lcd < lab && lcd < lbc && lcd < lda)
        return IE_E_Bottom;
    else
    if (lda < lab && lda < lbc && lda < lcd)
        return IE_E_Right;

    return IE_E_None;
}
//---------------------------------------------------------------------------
@end
//---------------------------------------------------------------------------
