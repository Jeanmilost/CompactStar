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

#import <UIKit/UIKit.h>

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Geometry.h"
#include "CSR_Physics.h"
#include "CSR_Scene.h"
#include "CSR_Sound.h"

//---------------------------------------------------------------------------
// Game structures
//---------------------------------------------------------------------------
typedef enum
{
    IE_E_None,
    IE_E_Left,
    IE_E_Top,
    IE_E_Right,
    IE_E_Bottom
} CSR_EEdge;
//---------------------------------------------------------------------------
typedef struct
{
    void* _Nullable m_pKey;
    CSR_Matrix4     m_Matrix;
    CSR_Sphere      m_Geometry;
    CSR_Body        m_Body;
} CSR_Ball;
//---------------------------------------------------------------------------
typedef struct
{
    void* _Nullable m_pKey;
    CSR_Matrix4     m_Matrix;
    CSR_Rect        m_Bounds;
} CSR_Goal;
//---------------------------------------------------------------------------

/**
* Wild soccer game logic
*@author Jean-Milost Reymond
*/
@interface CSR_GameLogic : NSObject
{
    float        m_RollAngle;
    float        m_BallDirAngle;
    float        m_BallOffset;
    CSR_Vector2* m_pTouchOrigin;
    CSR_Vector2* m_pTouchPosition;
    CSR_Ball*    m_pBall;
    CSR_Goal*    m_pGoal;
    CSR_Sphere*  m_pViewSphere;
    CSR_Matrix4* m_pYouWonMatrix;
    ALCdevice*   m_pOpenALDevice;
    ALCcontext*  m_pOpenALContext;
    CSR_Sound*   m_pFootStepLeftSound;
    CSR_Sound*   m_pFootStepRightSound;
    CSR_Sound*   m_pBallKickSound;
}

@property (nonatomic, assign) CSR_Ball*    _Nullable m_pBall;
@property (nonatomic, assign) CSR_Goal*    _Nullable m_pGoal;
@property (nonatomic, assign) CSR_Sphere*  _Nullable m_pViewSphere;
@property (nonatomic, assign) CSR_Matrix4* _Nullable m_pYouWonMatrix;
@property (nonatomic, assign) CSR_Vector2* _Nullable m_pTouchOrigin;
@property (nonatomic, assign) CSR_Vector2* _Nullable m_pTouchPosition;
@property (nonatomic, assign) CSR_Sound*   _Nullable m_pFootStepLeftSound;
@property (nonatomic, assign) CSR_Sound*   _Nullable m_pFootStepRightSound;
@property (nonatomic, assign) CSR_Sound*   _Nullable m_pBallKickSound;

/**
* Initializes the class
*/
- (instancetype _Nullable)init;

/**
* Releases the class
*/
- (void) dealloc;

/**
* Calculates the point where a bounding sphere hits the ground
*@param pScene - scene containing the items to check
*@param pBoundingSphere - bounding sphere to calculate
*@param dir - ground dir
*@param[out] pMatrix - transformation matrix to apply to the sphere to lies on the ground
*@param[out] pGroundPlane - plane of the ground slope on which the sphere lies
*@return true if a ground collision was found, otherwise false
*/
- (bool) ApplyGroundCollision :(CSR_Scene* _Nullable)pScene
                              :(const CSR_Sphere* _Nullable)pBoundingSphere
                              :(float)dir
                              :(CSR_Matrix4* _Nullable)pMatrix
                              :(CSR_Plane* _Nullable)pGroundPlane;

/**
* Applies the physical laws on the scene items
*@param pScene - scene containing the items on which physical laws should be applied
*@param elapsedTime - elapsed time since the last calculation, in milliseconds
*/
- (void) ApplyPhysics :(CSR_Scene* _Nullable)pScene :(float)elapsedTime;

/**
* Shoots the ball
*/
- (void) Shoot;

/**
* Checks if the ball hit the goal
*@param pOldPos - previous ball position
*@param pDir - ball direction
*@return true if the ball hit the goal, otherwise false
*/
- (bool) CheckForGoal :(const CSR_Vector3* _Nullable)pOldPos
                      :(const CSR_Vector3* _Nullable)pDir;

/**
* Gets the goal hit edge
*@param pOldPos - the previous known position of the object to test against the goal
*@return the goal hit edge
*/
- (CSR_EEdge) GetGoalHitEdge :(const CSR_Vector3* _Nullable)pOldPos;

@end
