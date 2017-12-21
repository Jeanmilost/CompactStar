/****************************************************************************
 * ==> CSR_Geometry --------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides the geometric functions and structs   *
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

#ifndef CSR_GeometryH
#define CSR_GeometryH

// std
#include <stddef.h>

//---------------------------------------------------------------------------
// Structures
//---------------------------------------------------------------------------

/**
* 3D vector
*/
typedef struct
{
    float m_X;
    float m_Y;
    float m_Z;
} CSR_Vector3;

/**
* 4x4 matrix
*/
typedef struct
{
    float m_Table[4][4];
} CSR_Matrix;

/**
* Quaternion
*/
typedef struct
{
    float m_X;
    float m_Y;
    float m_Z;
    float m_W;
} CSR_Quaternion;

/**
* Plane
*/
typedef struct
{
    float m_A;
    float m_B;
    float m_C;
    float m_D;
} CSR_Plane;

/**
* Line
*/
typedef struct
{
    CSR_Vector3 m_Point;
    CSR_Vector3 m_Dir;
    CSR_Vector3 m_InvDir;
} CSR_Line3;

/**
* Ray
*/
typedef struct
{
    CSR_Vector3 m_Pos;
    CSR_Vector3 m_Dir;
    CSR_Vector3 m_InvDir;
} CSR_Ray3;

/**
* Line segment
*/
typedef struct
{
    CSR_Vector3 m_Start;
    CSR_Vector3 m_End;
} CSR_Segment3;

/**
* Sphere
*/
typedef struct
{
    CSR_Vector3 m_Center;
    float       m_Radius;
} CSR_Sphere;

/**
* Aligned-axis box
*/
typedef struct
{
    CSR_Vector3 m_Min;
    CSR_Vector3 m_Max;
} CSR_Box;

/**
* Polygon
*/
typedef struct
{
    CSR_Vector3 m_Vertex[3];
} CSR_Polygon;

#ifdef __cplusplus
    extern "C"
    {
#endif
        //-------------------------------------------------------------------
        // Vector functions
        //-------------------------------------------------------------------

        /**
        * Adds vector contents
        *@param pV1 - vector that contains values to be added
        *@param pV2 - vector to add to
        *@param[out] pR - resulting vector
        */
        void csrVec3Add(const CSR_Vector3* pV1, const CSR_Vector3* pV2, CSR_Vector3* pR);

        /**
        * Subtracts vector contents
        *@param pV1 - vector that contains values to be subtracted
        *@param pV2 - vector to subtract by
        *@param[out] pR - resulting vector
        */
        void csrVec3Sub(const CSR_Vector3* pV1, const CSR_Vector3* pV2, CSR_Vector3* pR);

        /**
        * Calculates vector length
        *@param pV - vector to calculate length
        *@param[out] pR - vector length
        */
        void csrVec3Length(const CSR_Vector3* pV, float* pR);

        /**
        * Normalizes the vector
        *@param pV - vector to normalize
        *@param[out] pR - normailzed vector
        */
        void csrVec3Normalize(const CSR_Vector3* pV, CSR_Vector3* pR);

        /**
        * Calculates cross product between 2 vectors
        *@param pV1 - first vector
        *@param pV2 - second vector
        *@param[out] pR - resulting vector
        */
        void csrVec3Cross(const CSR_Vector3* pV1, const CSR_Vector3* pV2, CSR_Vector3* pR);

        /**
        * Calculates dot product between 2 vectors
        *@param pV1 - first vector
        *@param pV2 - second vector
        *@param[out] pR - resulting angle
        */
        void csrVec3Dot(const CSR_Vector3* pV1, const CSR_Vector3* pV2, float* pR);

        //-------------------------------------------------------------------
        // Matrix functions
        //-------------------------------------------------------------------

        /**
        * Gets matrix identity
        *@param[out] pR - resulting identity matrix
        */
        void csrMatIdentity(CSR_Matrix* pR);

        /**
        * Gets orthogonal matrix
        *@param left - left view rect edge
        *@param right - right view rect edge
        *@param bottom - bottom view rect edge
        *@param top - top view rect edge
        *@param zNear - z near clipping value
        *@param zFar - z far clipping value
        *@param[out] pR - resulting orthogonal matrix
        */
        void csrMatOrtho(float       left,
                         float       right,
                         float       bottom,
                         float       top,
                         float       zNear,
                         float       zFar,
                         CSR_Matrix* pR);

        /**
        * Gets frustrum matrix
        *@param left - left view rect edge
        *@param right - right view rect edge
        *@param bottom - bottom view rect edge
        *@param top - top view rect edge
        *@param zNear - z near clipping value
        *@param zFar - z far clipping value
        *@param[out] pR - resulting frustrum matrix
        */
        void csrMatFrustum(float       left,
                           float       right,
                           float       bottom,
                           float       top,
                           float       zNear,
                           float       zFar,
                           CSR_Matrix* pR);

        /**
        * Gets perspective matrix
        *@param fovyDeg - field of view angle in radian
        *@param aspect - aspect ratio
        *@param zNear - z near clipping value
        *@param zFar - z far clipping value
        *@param[out] pR - resulting perspective matrix
        */
        void csrMatPerspective(float       fovyDeg,
                               float       aspect,
                               float       zNear,
                               float       zFar,
                               CSR_Matrix* pR);

        /**
        * Gets translation matrix
        *@param pT - translation vector
        *@param[out] pR - resulting translation matrix
        */
        void csrMatTranslate(const CSR_Vector3* pT, CSR_Matrix* pR);

        /**
        * Gets rotation matrix
        *@param pAngle - rotation angle, in radian
        *@param pAxis - rotation axis
        *@param[out] pR - resulting orthogonal matrix
        */
        void csrMatRotate(const float* pAngle, const CSR_Vector3* pAxis, CSR_Matrix* pR);

        /**
        * Gets scale matrix
        *@param pFactor - scale factor
        *@param[out] pR - resulting scale matrix
        */
        void csrMatScale(const CSR_Vector3* pFactor, CSR_Matrix* pR);

        /**
        * Multiplies matrix by another matrix
        *@param pM1 - first matrix to multiply
        *@param pM2 - second matrix to multiply with
        *@param[out] pR - resulting orthogonal matrix
        */
        void csrMatMultiply(const CSR_Matrix* pM1, const CSR_Matrix* pM2, CSR_Matrix* pR);

        /**
        * Inverses a matrix
        *@param pM - matrix to inverse
        *@param[out] pR - inversed matrix
        *@param[out] pDeterminant - matrix determinant
        */
        void csrMatInverse(const CSR_Matrix* pM, CSR_Matrix* pR, float* pDeterminant);

        /**
        * Applies matrix to a vector
        *@param pM - matrix to apply
        *@param pV - vector on which matrix should be applied
        *@param[out] pR - resulting vector
        */
        void csrMatApplyToVector(const CSR_Matrix* pM, const CSR_Vector3* pV, CSR_Vector3* pR);

        /**
        * Applies matrix to a normal
        *@param pM - matrix to apply
        *@param pN - normal on which matrix should be applied
        *@param[out] pR - resulting normal
        */
        void csrMatApplyToNormal(const CSR_Matrix* pM, const CSR_Vector3* pN, CSR_Vector3* pR);

        /**
        * Unprojects a ray (i.e. transforms it in viewport coordinates)
        *@param pProj - projection matrix
        *@param pView - view matrix
        *@param[in, out] pRayPos - ray position, unprojected ray position on function ends
        *@param[in, out] pRayDir - ray direction, unprojected ray direction on function ends
        */
        void csrMatUnproject(const CSR_Matrix*  pProj,
                             const CSR_Matrix*  pView,
                                   CSR_Vector3* pRayPos,
                                   CSR_Vector3* pRayDir);

        //-------------------------------------------------------------------
        // Quaternion functions
        //-------------------------------------------------------------------

        /**
        * Gets a quaternion from an angle and a vector representing a rotation axis
        *@param pAngle - rotation angle
        *@param pAxis- rotation axis
        *@param[out] pR - resulting quaternion
        */
        void csrQuatFromAxis(float angle, const CSR_Vector3* pAxis, CSR_Quaternion* pR);

        /**
        * Gets a quaternion from Euler angles
        *@param pAngleX - rotation angle on x axis
        *@param pAngleY - rotation angle on y axis
        *@param pAngleZ - rotation angle on z axis
        *@param[out] pR - resulting quaternion
        */
        void csrQuatFromEuler(float angleX, float angleY, float angleZ, CSR_Quaternion* pR);

        /**
        * Gets a quaternion from a pitch, yaw and roll angles
        *@param pitch - the pitch angle in radians, pitch is comparable to a head up/down movement
        *@param yaw - the yaw angle in radians, yaw is comparable to a head left/right movement
        *@param roll - the roll angle in radians, roll is comparable to an aircraft rolling movement
        *@param[out] pR - resulting quaternion
        */
        void csrQuatFromPYR(float pitch, float yaw, float roll, CSR_Quaternion* pR);

        /**
        * Gets the squared length of a quaternion
        *@param pQ - quaternion for which the norm should be get
        *@param[out] pR - the resulting norm
        */
        void csrQuatLengthSquared(const CSR_Quaternion* pQ, float* pR);

        /**
        * Gets the quaternion length
        *@param pQ - quaternion for which the length should be get
        *@param[out] pR - the resulting length
        */
        void csrQuatLength(const CSR_Quaternion* pQ, float* pR);

        /**
        * Normalizes the quaternion
        *@param pV - vector to normalize
        *@param[out] pR - normailzed vector
        */
        void csrQuatNormalize(const CSR_Quaternion* pQ, CSR_Quaternion* pR);

        /**
        * Calculates dot product between 2 quaternions
        *@param pQ1 - first quaternion
        *@param pQ2 - second quaternion
        *@param[out] pR - resulting angle
        */
        void csrQuatDot(const CSR_Quaternion* pQ1, const CSR_Quaternion* pQ2, float* pR);

        /**
        * Scales a quaternion
        *@param pQ - quaternion to scale
        *@param s - scale factor
        *@param[out] pR - scaled quaternion
        */
        void csrQuatScale(const CSR_Quaternion* pQ, float s, CSR_Quaternion* pR);

        /**
        * Conjugates the quaternion
        *@param pQ - quaternion to conjugate
        *@param[out] pR - conjugated quaternion
        */
        void csrQuatConjugate(const CSR_Quaternion* pQ, CSR_Quaternion* pR);

        /**
        * Multiplies a quaternion by another
        *@param pQ1 - first quaternion to multiply
        *@param pQ2 - second quaternion to multiply with
        *@param[out] pR - multiplied quaternion
        */
        void csrQuatMultiply(const CSR_Quaternion* pQ1,
                             const CSR_Quaternion* pQ2,
                                   CSR_Quaternion* pR);

        /**
        * Inverse the quaternion
        *@param pQ - quaternion to inverse
        *@param[out] pR - inverted quaternion
        */
        void csrQuatInverse(const CSR_Quaternion* pQ, CSR_Quaternion* pR);

        /**
        * Rotates a vector by a quaternion
        *@param pQ - quaternion by which the vector will be rotated
        *@param pV  vector to rotate
        *@param[out] pR - rotated vector
        */
        void csrQuatRotate(const CSR_Quaternion* pQ, const CSR_Vector3* pV, CSR_Vector3* pR);

        /**
        * Gets the spherical linear interpolated quaternion between 2 quaternions
        *@param pQ1 - quaternion to interpolate
        *@param pQ2 - quaternion to interpolate with
        *@param p - interpolation position, in percent (between 0.0f and 1.0f)
        *@param[out] pR - the resulting spherical linear interpolated quaternion
        */
        void csrQuatSlerp(const CSR_Quaternion* pQ1,
                          const CSR_Quaternion* pQ2,
                                float           p,
                                CSR_Quaternion* pR);

        /**
        * Get a quaternion from a matrix
        *@param pM - rotation matrix
        *@param[out] pR - quaternion
        */
        void csrQuatFromMatrix(const CSR_Matrix* pM, CSR_Quaternion* pR);

        /**
        * Gets a rotation matrix from a quaternion
        *@param pQ - quaternion from which the matrix should be get
        *@param[out] pR - rotation matrix
        */
        void csrQuatToMatrix(const CSR_Quaternion* pQ, CSR_Matrix* pR);

        //-------------------------------------------------------------------
        // Plane functions
        //-------------------------------------------------------------------

        /**
        * Creates a plane from a point and a normal
        *@param pP - point
        *@param pN - normal
        *@param[out] pR - resulting plane
        */
        void csrPlaneFromPointNormal(const CSR_Vector3* pP, const CSR_Vector3* pN, CSR_Plane* pR);

        /**
        * Creates a plane from 3 vertex
        *@param pV1 - first vector
        *@param pV2 - second vector
        *@param pV3 - third vector
        *@param[out] pR - resulting plane
        */
        void csrPlaneFromPoints(const CSR_Vector3* pV1,
                                const CSR_Vector3* pV2,
                                const CSR_Vector3* pV3,
                                      CSR_Plane*   pR);

        /**
        * Calculates a distance between a point and a plane
        *@param pP - point
        *@param pPl - plane
        *@param[out] - resulting distance
        */
        void csrPlaneDistanceTo(const CSR_Vector3* pP, const CSR_Plane* pPl, float* pR);

        //-------------------------------------------------------------------
        // Inside checks
        //-------------------------------------------------------------------

        /**
        * Checks if a point is inside a polygon
        *@param pP - point to check
        *@param pPo - polygon to check against
        *@return 1 if point is inside the polygon, otherwise 0
        */
        int csrInsidePolygon(const CSR_Vector3* pP, const CSR_Polygon* pPo);

        /**
        * Checks if a point is inside a box
        *@param pP - point to check
        *@param pB - box to check against
        *@return 1 if point is inside the box, otherwise 0
        */
        int csrInsideBox(const CSR_Vector3* pP, const CSR_Box* pB);

        /**
        * Checks if a point is inside a sphere
        *@param pP - point to check
        *@param pS - sphere to check against
        *@return 1 if point is inside the sphere, otherwise 0
        */
        int csrInsideSphere(const CSR_Vector3* pP, const CSR_Sphere* pS);

        //-------------------------------------------------------------------
        // Intersection checks
        //-------------------------------------------------------------------

#ifdef __cplusplus
    }
#endif

//---------------------------------------------------------------------------
// Compiler
//---------------------------------------------------------------------------

// needed in mobile c compiler to link the .h file with the .c
#if defined(_OS_IOS_) || defined(_OS_ANDROID_) || defined(_OS_WINDOWS_)
    #include "CSR_Common.c"
#endif

#endif
