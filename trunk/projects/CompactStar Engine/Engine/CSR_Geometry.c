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

#include "CSR_Geometry.h"

// std
#include <math.h>

//---------------------------------------------------------------------------
// Vector functions
//---------------------------------------------------------------------------
void csrVec3Add(const CSR_Vector3* pV1, const CSR_Vector3* pV2, CSR_Vector3* pR)
{
    pR->m_X = pV1->m_X + pV2->m_X;
    pR->m_Y = pV1->m_Y + pV2->m_Y;
    pR->m_Z = pV1->m_Z + pV2->m_Z;
}
//---------------------------------------------------------------------------
void csrVec3Sub(const CSR_Vector3* pV1, const CSR_Vector3* pV2, CSR_Vector3* pR)
{
    pR->m_X = pV1->m_X - pV2->m_X;
    pR->m_Y = pV1->m_Y - pV2->m_Y;
    pR->m_Z = pV1->m_Z - pV2->m_Z;
}
//---------------------------------------------------------------------------
void csrVec3Length(const CSR_Vector3* pV, float* pR)
{
    *pR = sqrt((pV->m_X * pV->m_X) + (pV->m_Y * pV->m_Y) + (pV->m_Z * pV->m_Z));
}
//---------------------------------------------------------------------------
void csrVec3Normalize(const CSR_Vector3* pV, CSR_Vector3* pR)
{
    float len;

    // calculate vector length
    csrVec3Length(pV, &len);

    // no vector length?
    if (len == 0.0f)
    {
        pR->m_X = 0.0f;
        pR->m_Y = 0.0f;
        pR->m_Z = 0.0f;
        return;
    }

    // normalize vector (thus values will always be between 0.0f, and 1.0f)
    pR->m_X = (pV->m_X / len);
    pR->m_Y = (pV->m_Y / len);
    pR->m_Z = (pV->m_Z / len);
}
//---------------------------------------------------------------------------
void csrVec3Cross(const CSR_Vector3* pV1, const CSR_Vector3* pV2, CSR_Vector3* pR)
{
    pR->m_X = (pV1->m_Y * pV2->m_Z) - (pV2->m_Y * pV1->m_Z);
    pR->m_Y = (pV1->m_Z * pV2->m_X) - (pV2->m_Z * pV1->m_X);
    pR->m_Z = (pV1->m_X * pV2->m_Y) - (pV2->m_X * pV1->m_Y);
}
//---------------------------------------------------------------------------
void csrVec3Dot(const CSR_Vector3* pV1, const CSR_Vector3* pV2, float* pR)
{
    *pR = ((pV1->m_X * pV2->m_X) + (pV1->m_Y * pV2->m_Y) + (pV1->m_Z * pV2->m_Z));
}
//---------------------------------------------------------------------------
// Matrix functions
//---------------------------------------------------------------------------
void csrMatIdentity(CSR_Matrix* pR)
{
    pR->m_Table[0][0] = 1.0f; pR->m_Table[1][0] = 0.0f; pR->m_Table[2][0] = 0.0f; pR->m_Table[3][0] = 0.0f;
    pR->m_Table[0][1] = 0.0f; pR->m_Table[1][1] = 1.0f; pR->m_Table[2][1] = 0.0f; pR->m_Table[3][1] = 0.0f;
    pR->m_Table[0][2] = 0.0f; pR->m_Table[1][2] = 0.0f; pR->m_Table[2][2] = 1.0f; pR->m_Table[3][2] = 0.0f;
    pR->m_Table[0][3] = 0.0f; pR->m_Table[1][3] = 0.0f; pR->m_Table[2][3] = 0.0f; pR->m_Table[3][3] = 1.0f;
}
//---------------------------------------------------------------------------
void csrMatOrtho(float       left,
                 float       right,
                 float       bottom,
                 float       top,
                 float       zNear,
                 float       zFar,
                 CSR_Matrix* pR)
{
    // OpenGL specifications                                 can be rewritten as
    // |  2/(r-l)  0         0        -(r+l)/(r-l)  |        |  2/(r-l)  0        0        (r+l)/(l-r)  |
    // |  0        2/(t-b)   0        -(t+b)/(t-b)  |   =>   |  0        2/(t-b)  0        (t+b)/(b-t)  |
    // |  0        0        -2/(f-n)  -(f+n)/(f-n)  |        |  0        0        2/(n-f)  (f+n)/(n-f)  |
    // |  0        0         0         1            |        |  0        0        0        1            |

    // calculate matrix component values
    const float pfn =  zFar  + zNear;
    const float mnf =  zNear - zFar;
    const float prl =  right + left;
    const float mrl =  right - left;
    const float ptb =  top   + bottom;
    const float mtb =  top   - bottom;
    const float mlr = -mrl;
    const float mbt = -mtb;

    // build matrix
    pR->m_Table[0][0] = 2.0f / mrl; pR->m_Table[1][0] = 0.0f;       pR->m_Table[2][0] = 0.0f;       pR->m_Table[3][0] = prl / mlr;
    pR->m_Table[0][1] = 0.0f;       pR->m_Table[1][1] = 2.0f / mtb; pR->m_Table[2][1] = 0.0f;       pR->m_Table[3][1] = ptb / mbt;
    pR->m_Table[0][2] = 0.0f;       pR->m_Table[1][2] = 0.0f;       pR->m_Table[2][2] = 2.0f / mnf; pR->m_Table[3][2] = pfn / mnf;
    pR->m_Table[0][3] = 0.0f;       pR->m_Table[1][3] = 0.0f;       pR->m_Table[2][3] = 0.0f;       pR->m_Table[3][3] = 1.0f;
}
//---------------------------------------------------------------------------
void csrMatFrustum(float       left,
                   float       right,
                   float       bottom,
                   float       top,
                   float       zNear,
                   float       zFar,
                   CSR_Matrix* pR)
{
    // OpenGL specifications                                          can be rewritten as
    // |  2n/(r-l)     0             0             0         |        |  2n/(r-l)     0             0            0         |
    // |  0            2n/(t-b)      0             0         |   =>   |  0            2n/(t-b)      0            0         |
    // |  (r+l)/(r-l)  (t+b)/(t-b)  -(f+n)/(f-n)  -2fn/(f-n) |        |  (r+l)/(r-l)  (t+b)/(t-b)   (f+n)/(n-f)  2fn/(n-f) |
    // |  0            0            -1             0         |        |  0            0            -1            0         |
    // invalid for n <= 0, f <= 0, l = r, b = t, or n = f

    // calculate matrix component values
    const float x2n  =  2.0f * zNear;
    const float x2nf =  x2n  * zFar;
    const float pfn  = zFar  + zNear;
    const float mnf  = zNear - zFar;
    const float prl  = right + left;
    const float mrl  = right - left;
    const float ptb  = top   + bottom;
    const float mtb  = top   - bottom;

    // build matrix
    pR->m_Table[0][0] = x2n / mrl; pR->m_Table[1][0] = 0.0f;      pR->m_Table[2][0] =  0.0f;      pR->m_Table[3][0] = 0.0f;
    pR->m_Table[0][1] = 0.0f;      pR->m_Table[1][1] = x2n / mtb; pR->m_Table[2][1] =  0.0f;      pR->m_Table[3][1] = 0.0f;
    pR->m_Table[0][2] = prl / mrl; pR->m_Table[1][2] = ptb / mtb; pR->m_Table[2][2] =  pfn / mnf; pR->m_Table[3][2] = x2nf / mnf;
    pR->m_Table[0][3] = 0.0f;      pR->m_Table[1][3] = 0.0f;      pR->m_Table[2][3] = -1.0f;      pR->m_Table[3][3] = 0.0f;
}
//---------------------------------------------------------------------------
void csrMatPerspective(float       fovyDeg,
                       float       aspect,
                       float       zNear,
                       float       zFar,
                       CSR_Matrix* pR)
{
    const float maxY    =  zNear *  tanf(fovyDeg * M_PI / 360.0f);
    const float maxX    =  maxY   * aspect;
    const float negMaxY = -maxY;
    const float negMaxX = -maxX;

    csrMatFrustum(negMaxX, maxX, negMaxY, maxY, zNear, zFar, pR);
}
//---------------------------------------------------------------------------
void csrMatTranslate(const CSR_Vector3* pT, CSR_Matrix* pR)
{
    csrMatIdentity(pR);
    pR->m_Table[3][0] += pR->m_Table[0][0] * pT->m_X + pR->m_Table[1][0] * pT->m_Y + pR->m_Table[2][0] * pT->m_Z;
    pR->m_Table[3][1] += pR->m_Table[0][1] * pT->m_X + pR->m_Table[1][1] * pT->m_Y + pR->m_Table[2][1] * pT->m_Z;
    pR->m_Table[3][2] += pR->m_Table[0][2] * pT->m_X + pR->m_Table[1][2] * pT->m_Y + pR->m_Table[2][2] * pT->m_Z;
    pR->m_Table[3][3] += pR->m_Table[0][3] * pT->m_X + pR->m_Table[1][3] * pT->m_Y + pR->m_Table[2][3] * pT->m_Z;
}
//---------------------------------------------------------------------------
void csrMatRotate(const float* pAngle, const CSR_Vector3* pAxis, CSR_Matrix* pR)
{
    // calculate sinus, cosinus and inverted cosinus values
    const float c  = cosf(*pAngle);
    const float s  = sinf(*pAngle);
    const float ic = (1.0f - c);

    // create rotation matrix
    csrMatIdentity(pR);
    pR->m_Table[0][0] = (ic * pAxis->m_X * pAxis->m_X) +  c;
    pR->m_Table[1][0] = (ic * pAxis->m_X * pAxis->m_Y) - (s * pAxis->m_Z);
    pR->m_Table[2][0] = (ic * pAxis->m_X * pAxis->m_Z) + (s * pAxis->m_Y);
    pR->m_Table[0][1] = (ic * pAxis->m_Y * pAxis->m_X) + (s * pAxis->m_Z);
    pR->m_Table[1][1] = (ic * pAxis->m_Y * pAxis->m_Y) +  c;
    pR->m_Table[2][1] = (ic * pAxis->m_Y * pAxis->m_Z) - (s * pAxis->m_X);
    pR->m_Table[0][2] = (ic * pAxis->m_Z * pAxis->m_X) - (s * pAxis->m_Y);
    pR->m_Table[1][2] = (ic * pAxis->m_Z * pAxis->m_Y) + (s * pAxis->m_X);
    pR->m_Table[2][2] = (ic * pAxis->m_Z * pAxis->m_Z) +  c;
}
//---------------------------------------------------------------------------
void csrMatScale(const CSR_Vector3* pFactor, CSR_Matrix* pR)
{
    csrMatIdentity(pR);
    pR->m_Table[0][0] *= pFactor->m_X; pR->m_Table[1][0] *= pFactor->m_Y; pR->m_Table[2][0] *= pFactor->m_Z;
    pR->m_Table[0][1] *= pFactor->m_X; pR->m_Table[1][1] *= pFactor->m_Y; pR->m_Table[2][1] *= pFactor->m_Z;
    pR->m_Table[0][2] *= pFactor->m_X; pR->m_Table[1][2] *= pFactor->m_Y; pR->m_Table[2][2] *= pFactor->m_Z;
    pR->m_Table[0][3] *= pFactor->m_X; pR->m_Table[1][3] *= pFactor->m_Y; pR->m_Table[2][3] *= pFactor->m_Z;
}
//---------------------------------------------------------------------------
void csrMatMultiply(const CSR_Matrix* pM1, const CSR_Matrix* pM2, CSR_Matrix* pR)
{
    int i;
    int j;

    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j)
            pR->m_Table[i][j] = pM1->m_Table[i][0] * pM2->m_Table[0][j] +
                                pM1->m_Table[i][1] * pM2->m_Table[1][j] +
                                pM1->m_Table[i][2] * pM2->m_Table[2][j] +
                                pM1->m_Table[i][3] * pM2->m_Table[3][j];
}
//---------------------------------------------------------------------------
void csrMatInverse(const CSR_Matrix* pM, CSR_Matrix* pR, float* pDeterminant)
{
    float invDet;
    float t[3];
    float v[16];
    int   i;
    int   j;

    t[0] = pM->m_Table[2][2] * pM->m_Table[3][3] - pM->m_Table[2][3] * pM->m_Table[3][2];
    t[1] = pM->m_Table[1][2] * pM->m_Table[3][3] - pM->m_Table[1][3] * pM->m_Table[3][2];
    t[2] = pM->m_Table[1][2] * pM->m_Table[2][3] - pM->m_Table[1][3] * pM->m_Table[2][2];

    v[0] =  pM->m_Table[1][1] * t[0] - pM->m_Table[2][1] * t[1] + pM->m_Table[3][1] * t[2];
    v[4] = -pM->m_Table[1][0] * t[0] + pM->m_Table[2][0] * t[1] - pM->m_Table[3][0] * t[2];

    t[0] =  pM->m_Table[1][0] * pM->m_Table[2][1] - pM->m_Table[2][0] * pM->m_Table[1][1];
    t[1] =  pM->m_Table[1][0] * pM->m_Table[3][1] - pM->m_Table[3][0] * pM->m_Table[1][1];
    t[2] =  pM->m_Table[2][0] * pM->m_Table[3][1] - pM->m_Table[3][0] * pM->m_Table[2][1];

    v[8]  =  pM->m_Table[3][3] * t[0] - pM->m_Table[2][3] * t[1] + pM->m_Table[1][3] * t[2];
    v[12] = -pM->m_Table[3][2] * t[0] + pM->m_Table[2][2] * t[1] - pM->m_Table[1][2] * t[2];

    *pDeterminant = pM->m_Table[0][0] * v[0] +
                    pM->m_Table[0][1] * v[4] +
                    pM->m_Table[0][2] * v[8] +
                    pM->m_Table[0][3] * v[12];

    if (*pDeterminant == 0.0)
        return;

    t[0] = pM->m_Table[2][2] * pM->m_Table[3][3] - pM->m_Table[2][3] * pM->m_Table[3][2];
    t[1] = pM->m_Table[0][2] * pM->m_Table[3][3] - pM->m_Table[0][3] * pM->m_Table[3][2];
    t[2] = pM->m_Table[0][2] * pM->m_Table[2][3] - pM->m_Table[0][3] * pM->m_Table[2][2];

    v[1] = -pM->m_Table[0][1] * t[0] + pM->m_Table[2][1] * t[1] - pM->m_Table[3][1] * t[2];
    v[5] =  pM->m_Table[0][0] * t[0] - pM->m_Table[2][0] * t[1] + pM->m_Table[3][0] * t[2];

    t[0] = pM->m_Table[0][0] * pM->m_Table[2][1] - pM->m_Table[2][0] * pM->m_Table[0][1];
    t[1] = pM->m_Table[3][0] * pM->m_Table[0][1] - pM->m_Table[0][0] * pM->m_Table[3][1];
    t[2] = pM->m_Table[2][0] * pM->m_Table[3][1] - pM->m_Table[3][0] * pM->m_Table[2][1];

    v[9]  = -pM->m_Table[3][3] * t[0] - pM->m_Table[2][3] * t[1] - pM->m_Table[0][3] * t[2];
    v[13] =  pM->m_Table[3][2] * t[0] + pM->m_Table[2][2] * t[1] + pM->m_Table[0][2] * t[2];

    t[0] = pM->m_Table[1][2] * pM->m_Table[3][3] - pM->m_Table[1][3] * pM->m_Table[3][2];
    t[1] = pM->m_Table[0][2] * pM->m_Table[3][3] - pM->m_Table[0][3] * pM->m_Table[3][2];
    t[2] = pM->m_Table[0][2] * pM->m_Table[1][3] - pM->m_Table[0][3] * pM->m_Table[1][2];

    v[2] =  pM->m_Table[0][1] * t[0] - pM->m_Table[1][1] * t[1] + pM->m_Table[3][1] * t[2];
    v[6] = -pM->m_Table[0][0] * t[0] + pM->m_Table[1][0] * t[1] - pM->m_Table[3][0] * t[2];

    t[0] = pM->m_Table[0][0] * pM->m_Table[1][1] - pM->m_Table[1][0] * pM->m_Table[0][1];
    t[1] = pM->m_Table[3][0] * pM->m_Table[0][1] - pM->m_Table[0][0] * pM->m_Table[3][1];
    t[2] = pM->m_Table[1][0] * pM->m_Table[3][1] - pM->m_Table[3][0] * pM->m_Table[1][1];

    v[10] =  pM->m_Table[3][3] * t[0] + pM->m_Table[1][3] * t[1] + pM->m_Table[0][3] * t[2];
    v[14] = -pM->m_Table[3][2] * t[0] - pM->m_Table[1][2] * t[1] - pM->m_Table[0][2] * t[2];

    t[0] = pM->m_Table[1][2] * pM->m_Table[2][3] - pM->m_Table[1][3] * pM->m_Table[2][2];
    t[1] = pM->m_Table[0][2] * pM->m_Table[2][3] - pM->m_Table[0][3] * pM->m_Table[2][2];
    t[2] = pM->m_Table[0][2] * pM->m_Table[1][3] - pM->m_Table[0][3] * pM->m_Table[1][2];

    v[3] = -pM->m_Table[0][1] * t[0] + pM->m_Table[1][1] * t[1] - pM->m_Table[2][1] * t[2];
    v[7] =  pM->m_Table[0][0] * t[0] - pM->m_Table[1][0] * t[1] + pM->m_Table[2][0] * t[2];

    v[11] = -pM->m_Table[0][0] * (pM->m_Table[1][1] * pM->m_Table[2][3] - pM->m_Table[1][3] * pM->m_Table[2][1]) +
             pM->m_Table[1][0] * (pM->m_Table[0][1] * pM->m_Table[2][3] - pM->m_Table[0][3] * pM->m_Table[2][1]) -
             pM->m_Table[2][0] * (pM->m_Table[0][1] * pM->m_Table[1][3] - pM->m_Table[0][3] * pM->m_Table[1][1]);

    v[15] = pM->m_Table[0][0] * (pM->m_Table[1][1] * pM->m_Table[2][2] - pM->m_Table[1][2] * pM->m_Table[2][1]) -
            pM->m_Table[1][0] * (pM->m_Table[0][1] * pM->m_Table[2][2] - pM->m_Table[0][2] * pM->m_Table[2][1]) +
            pM->m_Table[2][0] * (pM->m_Table[0][1] * pM->m_Table[1][2] - pM->m_Table[0][2] * pM->m_Table[1][1]);

    invDet = 1.0 / *pDeterminant;

    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j)
            pR->m_Table[i][j] = v[4 * i + j] * invDet;
}
//---------------------------------------------------------------------------
void csrMatApplyToVector(const CSR_Matrix* pM, const CSR_Vector3* pV, CSR_Vector3* pR)
{
    pR->m_X = (pV->m_X * pM->m_Table[0][0] + pV->m_Y * pM->m_Table[1][0] + pV->m_Z * pM->m_Table[2][0] + pM->m_Table[3][0]);
    pR->m_Y = (pV->m_X * pM->m_Table[0][1] + pV->m_Y * pM->m_Table[1][1] + pV->m_Z * pM->m_Table[2][1] + pM->m_Table[3][1]);
    pR->m_Z = (pV->m_X * pM->m_Table[0][2] + pV->m_Y * pM->m_Table[1][2] + pV->m_Z * pM->m_Table[2][2] + pM->m_Table[3][2]);
}
//---------------------------------------------------------------------------
void csrMatApplyToNormal(const CSR_Matrix* pM, const CSR_Vector3* pN, CSR_Vector3* pR)
{
    pR->m_X = (pN->m_X * pM->m_Table[0][0] + pN->m_Y * pM->m_Table[1][0] + pN->m_Z * pM->m_Table[2][0]);
    pR->m_Y = (pN->m_X * pM->m_Table[0][1] + pN->m_Y * pM->m_Table[1][1] + pN->m_Z * pM->m_Table[2][1]);
    pR->m_Z = (pN->m_X * pM->m_Table[0][2] + pN->m_Y * pM->m_Table[1][2] + pN->m_Z * pM->m_Table[2][2]);
}
//---------------------------------------------------------------------------
void csrMatUnproject(const CSR_Matrix*  pProj,
                     const CSR_Matrix*  pView,
                           CSR_Vector3* pRayPos,
                           CSR_Vector3* pRayDir)
{
    float        determinant;
    CSR_Matrix  invertProj;
    CSR_Matrix  invertView;
    CSR_Matrix  unprojectMat;
    CSR_Vector3 unprojRayPos;
    CSR_Vector3 unprojRayDir;

    // unproject the ray to make it in the viewport coordinates
    csrMatInverse(pProj, &invertProj, &determinant);
    csrMatInverse(pView, &invertView, &determinant);
    csrMatMultiply(&invertProj, &invertView, &unprojectMat);
    csrMatApplyToVector(&unprojectMat, pRayPos, &unprojRayPos);
    csrMatApplyToVector(&unprojectMat, pRayDir, &unprojRayDir);

    // copy resulting ray position
    pRayPos->m_X = unprojRayPos.m_X;
    pRayPos->m_Y = unprojRayPos.m_Y;
    pRayPos->m_Z = unprojRayPos.m_Z;

    // normalize and copy resulting ray direction
    csrVec3Normalize(&unprojRayDir, pRayDir);
}
//---------------------------------------------------------------------------
// Quaternion functions
//---------------------------------------------------------------------------
void csrQuatFromAxis(float angle, const CSR_Vector3* pAxis, CSR_Quaternion* pR)
{
    const float s = sinf(angle * 0.5f);

    pR->m_X = (pAxis->m_X * s);
    pR->m_Y = (pAxis->m_Y * s);
    pR->m_Z = (pAxis->m_Z * s);
    pR->m_W =  cosf(angle * 0.5f);
}
//---------------------------------------------------------------------------
void csrQuatFromEuler(float angleX, float angleY, float angleZ, CSR_Quaternion* pR)
{
    const float cos_x_2 = cosf(0.5f * angleX);
    const float cos_y_2 = cosf(0.5f * angleY);
    const float cos_z_2 = cosf(0.5f * angleZ);

    const float sin_x_2 = sinf(0.5f * angleX);
    const float sin_y_2 = sinf(0.5f * angleY);
    const float sin_z_2 = sinf(0.5f * angleZ);

    // create the quaternion
    pR->m_X = cos_z_2 * cos_y_2 * sin_x_2 - sin_z_2 * sin_y_2 * cos_x_2;
    pR->m_Y = cos_z_2 * sin_y_2 * cos_x_2 + sin_z_2 * cos_y_2 * sin_x_2;
    pR->m_Z = sin_z_2 * cos_y_2 * cos_x_2 - cos_z_2 * sin_y_2 * sin_x_2;
    pR->m_W = cos_z_2 * cos_y_2 * cos_x_2 + sin_z_2 * sin_y_2 * sin_x_2;
}
//---------------------------------------------------------------------------
void csrQuatFromPYR(float pitch, float yaw, float roll, CSR_Quaternion* pR)
{
    // calculate the sinus and cosinus of each angles
    const float c1 = cos(yaw   / 2.0f);
    const float c2 = cos(pitch / 2.0f);
    const float c3 = cos(roll  / 2.0f);
    const float s1 = sin(yaw   / 2.0f);
    const float s2 = sin(pitch / 2.0f);
    const float s3 = sin(roll  / 2.0f);

    // calculate the quaternion values
    pR->m_X = (s1 * s2 * c3) + (c1 * c2 * s3);
    pR->m_Y = (s1 * c2 * c3) + (c1 * s2 * s3);
    pR->m_Z = (c1 * s2 * c3) - (s1 * c2 * s3);
    pR->m_W = (c1 * c2 * c3) - (s1 * s2 * s3);
}
//---------------------------------------------------------------------------
void csrQuatLengthSquared(const CSR_Quaternion* pQ, float* pR)
{
    *pR = ((pQ->m_X * pQ->m_X) + (pQ->m_Y * pQ->m_Y) + (pQ->m_Z * pQ->m_Z) + (pQ->m_W * pQ->m_W));
}
//---------------------------------------------------------------------------
void csrQuatLength(const CSR_Quaternion* pQ, float* pR)
{
    float norm;
    csrQuatLengthSquared(pQ, &norm);

    *pR = sqrt(norm);
}
//---------------------------------------------------------------------------
void csrQuatNormalize(const CSR_Quaternion* pQ, CSR_Quaternion* pR)
{
    float len;
    csrQuatLength(pQ, &len);

    if (!len)
    {
        pR->m_X = 0.0f;
        pR->m_Y = 0.0f;
        pR->m_Z = 0.0f;
        pR->m_W = 0.0f;
        return;
    }

    pR->m_X = (pR->m_X / len);
    pR->m_Y = (pR->m_Y / len);
    pR->m_Z = (pR->m_Z / len);
    pR->m_W = (pR->m_W / len);
}
//---------------------------------------------------------------------------
void csrQuatDot(const CSR_Quaternion* pQ1, const CSR_Quaternion* pQ2, float* pR)
{
    *pR = ((pQ1->m_X * pQ2->m_X) +
           (pQ1->m_Y * pQ2->m_Y) +
           (pQ1->m_Z * pQ2->m_Z) +
           (pQ1->m_W * pQ2->m_W));
}
//---------------------------------------------------------------------------
void csrQuatScale(const CSR_Quaternion* pQ, float s, CSR_Quaternion* pR)
{
    pR->m_X = pQ->m_X * s;
    pR->m_Y = pQ->m_Y * s;
    pR->m_Z = pQ->m_Z * s;
    pR->m_W = pQ->m_W * s;
}
//---------------------------------------------------------------------------
void csrQuatConjugate(const CSR_Quaternion* pQ, CSR_Quaternion* pR)
{
    pR->m_X = -pQ->m_X;
    pR->m_Y = -pQ->m_Y;
    pR->m_Z = -pQ->m_Z;
    pR->m_W =  pQ->m_W;
}
//---------------------------------------------------------------------------
void csrQuatMultiply(const CSR_Quaternion* pQ1, const CSR_Quaternion* pQ2, CSR_Quaternion* pR)
{
    pR->m_X = pQ1->m_W * pQ2->m_X + pQ1->m_X * pQ2->m_W + pQ1->m_Y * pQ2->m_Z - pQ1->m_Z * pQ2->m_Y;
    pR->m_Y = pQ1->m_W * pQ2->m_Y - pQ1->m_X * pQ2->m_Z + pQ1->m_Y * pQ2->m_W + pQ1->m_Z * pQ2->m_X;
    pR->m_Z = pQ1->m_W * pQ2->m_Z + pQ1->m_X * pQ2->m_Y - pQ1->m_Y * pQ2->m_X + pQ1->m_Z * pQ2->m_W;
    pR->m_W = pQ1->m_W * pQ2->m_W - pQ1->m_X * pQ2->m_X - pQ1->m_Y * pQ2->m_Y - pQ1->m_Z * pQ2->m_Z;
}
//---------------------------------------------------------------------------
void csrQuatInverse(const CSR_Quaternion* pQ, CSR_Quaternion* pR)
{
    float          lengthSq;
    CSR_Quaternion conjugedQuat;

    // calculate the squared length of the quaternion
    csrQuatLengthSquared(pQ, &lengthSq);

    // empty quaternion?
    if (!lengthSq)
    {
        pR->m_X = 0.0f;
        pR->m_Y = 0.0f;
        pR->m_Z = 0.0f;
        pR->m_W = 0.0f;
        return;
    }

    csrQuatConjugate(pQ, &conjugedQuat);
    csrQuatScale(&conjugedQuat, 1.0f / lengthSq, pR);
}
//---------------------------------------------------------------------------
void csrQuatRotate(const CSR_Quaternion* pQ, const CSR_Vector3* pV, CSR_Vector3* pR)
{
    CSR_Quaternion qv;
    CSR_Quaternion qi;
    CSR_Quaternion qm;

    qv.m_X = pV->m_X;
    qv.m_Y = pV->m_Y;
    qv.m_Z = pV->m_Z;
    qv.m_W = 0.0f;

    csrQuatInverse(pQ, &qi);

    // rotate vector
    qm.m_X = pQ->m_X * qv.m_X * qi.m_X;
    qm.m_Y = pQ->m_Y * qv.m_Y * qi.m_Y;
    qm.m_Z = pQ->m_Z * qv.m_Z * qi.m_Z;
    qm.m_W = pQ->m_Y * qv.m_W * qi.m_W;

    pR->m_X = qm.m_X;
    pR->m_X = qm.m_Y;
    pR->m_Z = qm.m_Z;
}
//---------------------------------------------------------------------------
void csrQuatSlerp(const CSR_Quaternion* pQ1, const CSR_Quaternion* pQ2, float p, CSR_Quaternion* pR)
{
    // are quaternions identical?
    if ((*this) == other)
        return (*this);

    // calculate dot product between q1 and q2
    T result = Dot(other);

    DWF_Quaternion interpolateWith;

    // check if angle is higher than 90° (this happen if dot product is less than 0)
    if (result < T(0.0))
    {
        // negate the second quaternion and the dot product result
        interpolateWith = -other;
        result          = -result;
    }
    else
        interpolateWith = other;

    // calculate the interpolation first and second scale
    T scale0 = T(1.0) - p;
    T scale1 = p;

    // is angle large enough to apply the calculation
    if ((T(1.0) - result) > T(0.1))
    {
        // calculate the angle between the 2 quaternions and get the sinus of that angle
        T theta    = std::acos(result);
        T sinTheta = std::sinf(theta);

        // is resulting sinus equal to 0? (just to verify, should not happen)
        if (!sinTheta)
            M_THROW_EXCEPTION("Invalid value");

        // calculate the scaling for q1 and q2, according to the angle and it's sine value
        scale0 = std::sinf((T(1.0) - p) * theta)  / sinTheta;
        scale1 = std::sinf((p           * theta)) / sinTheta;
    }

    // calculate the resulting quaternion by using a special form of linear interpolation
    return DWF_Quaternion<T>((scale0 * m_X) + (scale1 * interpolateWith.m_X);
                             (scale0 * m_Y) + (scale1 * interpolateWith.m_Y);
                             (scale0 * m_Z) + (scale1 * interpolateWith.m_Z);
                             (scale0 * m_W) + (scale1 * interpolateWith.m_W));
}
//---------------------------------------------------------------------------
void csrQuatFromMatrix(const CSR_Matrix* pM, CSR_Quaternion* pR)
{
    // calculate the matrix diagonal by adding up it's diagonal indices (also known as "trace")
    const T diagonal = matrix.m_Table[0][0] +
                       matrix.m_Table[1][1] +
                       matrix.m_Table[2][2] +
                       matrix.m_Table[3][3];

    T scale = T(0.0);

    // is diagonal greater than zero?
    if (diagonal > T(0.00000001))
    {
        // calculate the diagonal scale
        scale = sqrt(diagonal) * T(2.0);

        // calculate the quaternion values using the respective equation
        m_X = (matrix.m_Table[1][2] - matrix.m_Table[2][1]) / scale;
        m_Y = (matrix.m_Table[2][0] - matrix.m_Table[0][2]) / scale;
        m_Z = (matrix.m_Table[0][1] - matrix.m_Table[1][0]) / scale;
        m_W = T(0.25) * scale;

        return;
    }

    // search for highest value in the matrix diagonal
    if (matrix.m_Table[0][0] > matrix.m_Table[1][1] && matrix.m_Table[0][0] > matrix.m_Table[2][2])
    {
        // calculate scale using the first diagonal element and double that value
        scale = sqrt(T(1.0) + matrix.m_Table[0][0] - matrix.m_Table[1][1] - matrix.m_Table[2][2]) * T(2.0);

        // calculate the quaternion values using the respective equation
        m_X = T(0.25) * scale;
        m_Y = (matrix.m_Table[0][1] + matrix.m_Table[1][0]) / scale;
        m_Z = (matrix.m_Table[2][0] + matrix.m_Table[0][2]) / scale;
        m_W = (matrix.m_Table[1][2] - matrix.m_Table[2][1]) / scale;
    }
    else
    if (matrix.m_Table[1][1] > matrix.m_Table[2][2])
    {
        // calculate scale using the second diagonal element and double that value
        scale = sqrt(T(1.0) + matrix.m_Table[1][1] - matrix.m_Table[0][0] - matrix.m_Table[2][2]) * T(2.0);

        // calculate the quaternion values using the respective equation
        m_X = (matrix.m_Table[0][1] + matrix.m_Table[1][0]) / scale;
        m_Y = T(0.25) * scale;
        m_Z = (matrix.m_Table[1][2] + matrix.m_Table[2][1]) / scale;
        m_W = (matrix.m_Table[2][0] - matrix.m_Table[0][2]) / scale;
    }
    else
    {
        // calculate scale using the third diagonal element and double that value
        scale = sqrt(T(1.0) + matrix.m_Table[2][2] - matrix.m_Table[0][0] - matrix.m_Table[1][1]) * T(2.0);

        // calculate the quaternion values using the respective equation
        m_X = (matrix.m_Table[2][0] + matrix.m_Table[0][2]) / scale;
        m_Y = (matrix.m_Table[1][2] + matrix.m_Table[2][1]) / scale;
        m_Z = T(0.25) * scale;
        m_W = (matrix.m_Table[0][1] - matrix.m_Table[1][0]) / scale;
    }
}
//---------------------------------------------------------------------------
void csrQuatToMatrix(const CSR_Quaternion* pQ, CSR_Matrix* pR)
{
    pR->m_Table[0][0] = 1.0f - 2.0f * (pQ->m_Y * pQ->m_Y + pQ->m_Z * pQ->m_Z); pR->m_Table[0][1] =        2.0f * (pQ->m_X * pQ->m_Y - pQ->m_Z * pQ->m_W); pR->m_Table[0][2] =        2.0f * (pQ->m_X * pQ->m_Z + pQ->m_Y * pQ->m_W); pR->m_Table[0][3] = 0.0f;
    pR->m_Table[1][0] =        2.0f * (pQ->m_X * pQ->m_Y + pQ->m_Z * pQ->m_W); pR->m_Table[1][1] = 1.0f - 2.0f * (pQ->m_X * pQ->m_X + pQ->m_Z * pQ->m_Z); pR->m_Table[1][2] =        2.0f * (pQ->m_Y * pQ->m_Z - pQ->m_X * pQ->m_W); pR->m_Table[1][3] = 0.0f;
    pR->m_Table[2][0] =        2.0f * (pQ->m_X * pQ->m_Z - pQ->m_Y * pQ->m_W); pR->m_Table[2][1] =        2.0f * (pQ->m_Y * pQ->m_Z + pQ->m_X * pQ->m_W); pR->m_Table[2][2] = 1.0f - 2.0f * (pQ->m_X * pQ->m_X + pQ->m_Y * pQ->m_Y); pR->m_Table[2][3] = 0.0f;
    pR->m_Table[3][0] = 0.0f;                                                  pR->m_Table[3][1] = 0.0f;                                                  pR->m_Table[3][2] = 0.0f;                                                  pR->m_Table[3][3] = 1.0f;
}
//---------------------------------------------------------------------------
// Plane functions
//---------------------------------------------------------------------------
void csrPlaneFromPointNormal(const CSR_Vector3* pP, const CSR_Vector3* pN, CSR_Plane* pR)
{
    float d;

    // the a, b, and c components are only the normal of the plane
    pR->m_A = pN->m_X;
    pR->m_B = pN->m_Y;
    pR->m_C = pN->m_Z;

    // calculate plane d component using the aX + bY + cZ + d = 0 formula
    csrVec3Dot(pN, pP, &d);
    pR->m_D = -d;
}
//---------------------------------------------------------------------------
void csrPlaneFromPoints(const CSR_Vector3* pV1,
                        const CSR_Vector3* pV2,
                        const CSR_Vector3* pV3,
                              CSR_Plane*   pR)
{
    CSR_Vector3 e1;
    CSR_Vector3 e2;
    CSR_Vector3 normal;

    // calculate edge vectors
    csrVec3Sub(pV2, pV1, &e1);
    csrVec3Sub(pV3, pV1, &e2);

    // calculate the normal of the plane
    csrVec3Cross(&e1, &e2, &normal);
    csrVec3Normalize(&normal, &normal);

    // calculate and return the plane
    csrPlaneFromPointNormal(pV1, &normal, pR);
}
//---------------------------------------------------------------------------
void csrPlaneDistanceTo(const CSR_Vector3* pP, const CSR_Plane* pPl, float* pR)
{
    CSR_Vector3 n;
    float       dist;

    // get the normal of the plane
    n.m_X = pPl->m_A;
    n.m_Y = pPl->m_B;
    n.m_Z = pPl->m_C;

    // calculate the distance between the plane and the point
    csrVec3Dot(&n, pP, &dist);
    *pR = dist + pPl->m_D;
}
//---------------------------------------------------------------------------
// Inside checks
//---------------------------------------------------------------------------
int csrInsidePolygon(const CSR_Vector3* pP, const CSR_Polygon* pPo)
{
    CSR_Vector3 nPToV1;
    CSR_Vector3 nPToV2;
    CSR_Vector3 nPToV3;
    float       a1;
    float       a2;
    float       a3;
    float       angleResult;

    /*
    * check if the point p is inside the polygon in the following manner:
    *
    *                  V1                         V1
    *                  /\                         /\
    *                 /  \                       /  \
    *                / *p \                  *P /    \
    *               /      \                   /      \
    *            V2 -------- V3             V2 -------- V3
    *
    * calculate the vectors between the point p and each polygon vertex, then
    * calculate the angle formed by each of these vectors. If the sum of the
    * angles is equal to a complete circle, i.e. 2 * pi in radians, then the
    * point p is inside the polygon limits, otherwise the point is outside. It
    * is assumed that the point to check belongs to the polygon's plane
    */
    csrVec3Sub(&pPo->m_Vertex[0], pP, &nPToV1);
    csrVec3Sub(&pPo->m_Vertex[1], pP, &nPToV2);
    csrVec3Sub(&pPo->m_Vertex[2], pP, &nPToV3);
    csrVec3Normalize(&nPToV1, &nPToV1);
    csrVec3Normalize(&nPToV2, &nPToV2);
    csrVec3Normalize(&nPToV3, &nPToV3);

    // calculate the angles using the scalar product of each vectors. We use
    // the following algorithms:
    // A1 = NPToV1.x * NPToV2.x + NPToV1.y * NPToV2.y + NPToV1.z * NPToV2.z
    // A2 = NPToV2.x * NPToV3.x + NPToV2.y * NPToV3.y + NPToV2.z * NPToV3.z
    // A3 = NPToV3.x * NPToV1.x + NPToV3.y * NPToV1.y + NPToV3.z * NPToV1.z
    csrVec3Dot(&nPToV1, &nPToV2, &a1);
    csrVec3Dot(&nPToV2, &nPToV3, &a2);
    csrVec3Dot(&nPToV3, &nPToV1, &a3);

    // limit a1 to avoid rounding errors
    if (a1 > 1.0f)
        a1 = 1.0f;
    else
    if (a1 < -1.0f)
        a1 = -1.0f;

    // limit a2 to avoid rounding errors
    if (a2 > 1.0f)
        a2 = 1.0f;
    else
    if (a2 < -1.0f)
        a2 = -1.0f;

    // limit a3 to avoid rounding errors
    if (a3 > 1.0f)
        a3 = 1.0f;
    else
    if (a3 < -1.0f)
        a3 = -1.0f;

    // calculate the sum of all angles
    angleResult = acos(a1) + acos(a2) + acos(a3);

    // if sum is equal or higher to 6.28 radians then point P is inside polygon
    if (angleResult >= 6.28f)
        return 1;

    return 0;
}
//---------------------------------------------------------------------------
int csrInsideBox(const CSR_Vector3* pP, const CSR_Box* pB)
{}
//---------------------------------------------------------------------------
int csrInsideSphere(const CSR_Vector3* pP, const CSR_Sphere* pS)
{
    float       distance;
    CSR_Vector3 length;

    // calculate the distance between test point and the center of the sphere
    csrVec3Sub(pP, &pS->m_Center, &length);
    csrVec3Length(&length, &distance);

    // check if distance is shorter than the radius of the sphere and return result
    return (distance <= pS->m_Radius);
}
//---------------------------------------------------------------------------
