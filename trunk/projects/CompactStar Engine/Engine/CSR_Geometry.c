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

// compactStar engine
#include "CSR_Common.h"

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
int csrVec3BetweenRange(const CSR_Vector3* pV,
                        const CSR_Vector3* pRS,
                        const CSR_Vector3* pRE,
                              float        tolerance)
{
    // check if each vector component is between start and end limits
    if (!csrMathBetween(pV->m_X, pRS->m_X, pRE->m_X, tolerance))
        return 0;

    if (!csrMathBetween(pV->m_Y, pRS->m_Y, pRE->m_Y, tolerance))
        return 0;

    if (!csrMathBetween(pV->m_Z, pRS->m_Z, pRE->m_Z, tolerance))
        return 0;

    return 1;
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
void csrMatOrtho(float left, float right, float bottom, float top, float zNear, float zFar, CSR_Matrix* pR)
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
void csrMatFrustum(float left, float right, float bottom, float top, float zNear, float zFar, CSR_Matrix* pR)
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
void csrMatPerspective(float fovyDeg, float aspect, float zNear, float zFar, CSR_Matrix* pR)
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
    *pR = ((pQ1->m_X * pQ2->m_X) + (pQ1->m_Y * pQ2->m_Y) + (pQ1->m_Z * pQ2->m_Z) + (pQ1->m_W * pQ2->m_W));
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
    qm.m_W = pQ->m_W * qv.m_W * qi.m_W;

    pR->m_X = qm.m_X;
    pR->m_Y = qm.m_Y;
    pR->m_Z = qm.m_Z;
}
//---------------------------------------------------------------------------
int csrQuatSlerp(const CSR_Quaternion* pQ1, const CSR_Quaternion* pQ2, float p, CSR_Quaternion* pR)
{
    CSR_Quaternion interpolateWith;
    float          scale0;
    float          scale1;
    float          theta;
    float          sinTheta;
    float          result;

    // are quaternions identical?
    if (pQ1->m_X == pQ2->m_X && pQ1->m_Y == pQ2->m_Y && pQ1->m_Z == pQ2->m_Z && pQ1->m_W == pQ2->m_W)
    {
        *pR = *pQ1;
        return 1;
    }

    // calculate dot product between q1 and q2
    csrQuatDot(pQ1, pQ2, &result);

    // check if angle is higher than 90� (this happen if dot product is less than 0)
    if (result < 0.0f)
    {
        // negate the second quaternion and the dot product result
        interpolateWith.m_X = -pQ2->m_X;
        interpolateWith.m_Y = -pQ2->m_Y;
        interpolateWith.m_Z = -pQ2->m_Z;
        interpolateWith.m_W = -pQ2->m_W;
        result              = -result;
    }
    else
        interpolateWith = *pQ2;

    // calculate the first and second scaling factor to apply to the interpolation
    scale0 = 1.0f - p;
    scale1 = p;

    // is angle large enough to apply the calculation?
    if ((1.0f - result) > 0.1f)
    {
        // calculate the angle between the 2 quaternions and get the sinus of that angle
        theta    = acos(result);
        sinTheta = sinf(theta);

        // is resulting sinus equal to 0? (just to verify, should not happen)
        if (!sinTheta)
        {
            pR->m_X = 0.0f;
            pR->m_Y = 0.0f;
            pR->m_Z = 0.0f;
            pR->m_W = 0.0f;

            return 0;
        }

        // calculate the scaling for q1 and q2, according to the angle and it's sine value
        scale0 = sinf((1.0f - p) * theta) / sinTheta;
        scale1 = sinf (p         * theta) / sinTheta;
    }

    // calculate the resulting quaternion by using a special form of linear interpolation
    pR->m_X = (scale0 * pQ1->m_X) + (scale1 * interpolateWith.m_X);
    pR->m_Y = (scale0 * pQ1->m_Y) + (scale1 * interpolateWith.m_Y);
    pR->m_Z = (scale0 * pQ1->m_Z) + (scale1 * interpolateWith.m_Z);
    pR->m_W = (scale0 * pQ1->m_W) + (scale1 * interpolateWith.m_W);

    return 1;
}
//---------------------------------------------------------------------------
int csrQuatFromMatrix(const CSR_Matrix* pM, CSR_Quaternion* pR)
{
    float diagonal;
    float scale;

    // calculate the matrix diagonal by adding up it's diagonal indices (also known as "trace")
    diagonal = pM->m_Table[0][0] + pM->m_Table[1][1] + pM->m_Table[2][2] + pM->m_Table[3][3];

    // is diagonal greater than zero?
    if (diagonal > 0.00000001f)
    {
        // calculate the diagonal scale
        scale = sqrt(diagonal) * 2.0f;

        // calculate the quaternion values using the respective equation
        pR->m_X = (pM->m_Table[1][2] - pM->m_Table[2][1]) / scale;
        pR->m_Y = (pM->m_Table[2][0] - pM->m_Table[0][2]) / scale;
        pR->m_Z = (pM->m_Table[0][1] - pM->m_Table[1][0]) / scale;
        pR->m_W =  0.25f * scale;

        return 1;
    }

    // search for highest value in the matrix diagonal
    if (pM->m_Table[0][0] > pM->m_Table[1][1] && pM->m_Table[0][0] > pM->m_Table[2][2])
    {
        // calculate scale using the first diagonal element and double that value
        scale = sqrt(1.0f + pM->m_Table[0][0] - pM->m_Table[1][1] - pM->m_Table[2][2]) * 2.0f;

        // should not happen, but better to verify
        if (!scale)
        {
            pR->m_X = 0.0f;
            pR->m_Y = 0.0f;
            pR->m_Z = 0.0f;
            pR->m_W = 0.0f;

            return 0;
        }

        // calculate the quaternion values using the respective equation
        pR->m_X =  0.25f * scale;
        pR->m_Y = (pM->m_Table[0][1] + pM->m_Table[1][0]) / scale;
        pR->m_Z = (pM->m_Table[2][0] + pM->m_Table[0][2]) / scale;
        pR->m_W = (pM->m_Table[1][2] - pM->m_Table[2][1]) / scale;

        return 1;
    }

    if (pM->m_Table[1][1] > pM->m_Table[2][2])
    {
        // calculate scale using the second diagonal element and double that value
        scale = sqrt(1.0f + pM->m_Table[1][1] - pM->m_Table[0][0] - pM->m_Table[2][2]) * 2.0f;

        // should not happen, but better to verify
        if (!scale)
        {
            pR->m_X = 0.0f;
            pR->m_Y = 0.0f;
            pR->m_Z = 0.0f;
            pR->m_W = 0.0f;

            return 0;
        }

        // calculate the quaternion values using the respective equation
        pR->m_X = (pM->m_Table[0][1] + pM->m_Table[1][0]) / scale;
        pR->m_Y =  0.25f * scale;
        pR->m_Z = (pM->m_Table[1][2] + pM->m_Table[2][1]) / scale;
        pR->m_W = (pM->m_Table[2][0] - pM->m_Table[0][2]) / scale;

        return 1;
    }

    // calculate scale using the third diagonal element and double that value
    scale = sqrt(1.0f + pM->m_Table[2][2] - pM->m_Table[0][0] - pM->m_Table[1][1]) * 2.0f;

    // should not happen, but better to verify
    if (!scale)
    {
        pR->m_X = 0.0f;
        pR->m_Y = 0.0f;
        pR->m_Z = 0.0f;
        pR->m_W = 0.0f;

        return 0;
    }

    // calculate the quaternion values using the respective equation
    pR->m_X = (pM->m_Table[2][0] + pM->m_Table[0][2]) / scale;
    pR->m_Y = (pM->m_Table[1][2] + pM->m_Table[2][1]) / scale;
    pR->m_Z =  0.25f * scale;
    pR->m_W = (pM->m_Table[0][1] - pM->m_Table[1][0]) / scale;

    return 1;
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
// Segment functions
//---------------------------------------------------------------------------
void csrSeg3ShortestDistance(const CSR_Segment3* pS1,
                             const CSR_Segment3* pS2,
                                   float         tolerance,
                                   float*        pR)
{
    CSR_Vector3 delta21;
    CSR_Vector3 delta43;
    CSR_Vector3 delta13;
    CSR_Vector3 dP;
    float       a;
    float       b;
    float       c;
    float       d;
    float       e;
    float       D;
    float       sc;
    float       sN;
    float       sD;
    float       tc;
    float       tN;
    float       tD;
    float       dotdP;

    // the parametric formulas for the 2 segments are:
    // p = s1 + s(s2 - s1), where s1 = pS1->m_Start and s2 = pS1->m_End
    // p = s3 + t(s4 - s3), where s3 = pS2->m_Start and s4 = pS2->m_End
    // so, calculate the basic values to use for the calculation
    csrVec3Sub(&pS1->m_End,   &pS1->m_Start, &delta21);
    csrVec3Sub(&pS2->m_End,   &pS2->m_Start, &delta43);
    csrVec3Sub(&pS1->m_Start, &pS2->m_Start, &delta13);

    // calculate the distance (represented by D) between the 2 line segments
    csrVec3Dot(&delta21, &delta21, &a);
    csrVec3Dot(&delta21, &delta43, &b);
    csrVec3Dot(&delta43, &delta43, &c);
    csrVec3Dot(&delta21, &delta13, &d);
    csrVec3Dot(&delta43, &delta13, &e);
    D = ((a * c) - (b * b));

    // sc = sN / sD, default sD = D >= 0
    sD = D;

    // tc = tN / tD, default tD = D >= 0
    tD = D;

    // compute the line parameters of the two closest points
    if (fabs(D) < tolerance)
    {
        // the lines are almost parallel, force using point P0 on segment S1
        // to prevent possible division by 0 later
        sN = 0.0f;
        sD = 1.0f;
        tN = e;
        tD = c;
    }
    else
    {
        // get the closest points on the infinite lines
        sN = ((b * e) - (c * d));
        tN = ((a * e) - (b * d));

        // sc < 0 => the s=0 edge is visible
        if (sN < 0.0f)
        {
            sN = 0.0f;
            tN = e;
            tD = c;
        }
        else
        // sc > 1 => the s=1 edge is visible
        if (sN > sD)
        {
            sN = sD;
            tN = e + b;
            tD = c;
        }
    }

    // tc < 0 => the t=0 edge is visible
    if (tN < 0.0f)
    {
        tN = 0.0f;

        // recompute sc for this edge
        if (-d < 0.0f)
            sN = 0.0f;
        else
        if (-d > a)
            sN = sD;
        else
        {
            sN = -d;
            sD =  a;
        }
    }
    else
    // tc > 1 => the t=1 edge is visible
    if (tN > tD)
    {
        tN = tD;

        // recompute sc for this edge
        if ((-d + b) < 0.0f)
            sN = 0.0f;
        else
        if ((-d + b) > a)
            sN = sD;
        else
        {
            sN = (-d + b);
            sD = a;
        }
    }

    // finally do the division to get sc and tc
    if (fabs(sN) < tolerance)
        sc = 0.0f;
    else
        sc = sN / sD;

    if (fabs(tN) < tolerance)
        tc = 0.0f;
    else
        tc = tN / tD;

    // get the difference of the two closest points
    dP.m_X = delta13.m_X + (sc * delta21.m_X) - (tc * delta43.m_X);
    dP.m_Y = delta13.m_Y + (sc * delta21.m_Y) - (tc * delta43.m_Y);
    dP.m_Z = delta13.m_Z + (sc * delta21.m_Z) - (tc * delta43.m_Z);

    // return the closest distance
    csrVec3Dot(&dP, &dP, &dotdP);
    *pR = sqrt(dotdP);
}
//---------------------------------------------------------------------------
void csrSeg3ClosestPoint(const CSR_Segment3* pS, const CSR_Vector3* pP, CSR_Vector3* pR)
{
    float       segLength;
    float       angle;
    CSR_Vector3 PToStart;
    CSR_Vector3 length;
    CSR_Vector3 normalizedLength;
    CSR_Vector3 p;

    // calculate the distance between the test point and the segment
    csrVec3Sub( pP,        &pS->m_Start, &PToStart);
    csrVec3Sub(&pS->m_End, &pS->m_Start, &length);
    csrVec3Length(&length, &segLength);

    // calculate the direction of the segment
    csrVec3Normalize(&length, &normalizedLength);

    // calculate the projection of the point on the segment
    csrVec3Dot(&normalizedLength, &PToStart, &angle);

    // check if projection is before the segment
    if (angle < 0.0)
        *pR = pS->m_Start;
    else
    // check if projection is after the segment
    if (angle > segLength)
        *pR = pS->m_End;
    else
    {
        // calculate the position of the projection on the segment
        p.m_X = normalizedLength.m_X * angle;
        p.m_Y = normalizedLength.m_Y * angle;
        p.m_Z = normalizedLength.m_Z * angle;

        // calculate and returns the point coordinate on the segment
        csrVec3Add(&pS->m_Start, &p, pR);
    }
}
//---------------------------------------------------------------------------
// Polygon functions
//---------------------------------------------------------------------------
void csrPolygonClosestPoint(const CSR_Vector3* pP, const CSR_Polygon* pPo, CSR_Vector3* pR)
{
    float        dAB;
    float        dBC;
    float        dCA;
    float        min;
    CSR_Vector3  rab;
    CSR_Vector3  rbc;
    CSR_Vector3  rca;
    CSR_Vector3  vAB;
    CSR_Vector3  vBC;
    CSR_Vector3  vCA;
    CSR_Segment3 sab;
    CSR_Segment3 sbc;
    CSR_Segment3 sca;

    // get line segments from each polygon edge
    sab.m_Start = pPo->m_Vertex[0];
    sab.m_End   = pPo->m_Vertex[1];
    sbc.m_Start = pPo->m_Vertex[1];
    sbc.m_End   = pPo->m_Vertex[2];
    sca.m_Start = pPo->m_Vertex[2];
    sca.m_End   = pPo->m_Vertex[0];

    // calculate the projections points on each edge of the triangle
    csrSeg3ClosestPoint(&sab, pP, &rab);
    csrSeg3ClosestPoint(&sbc, pP, &rbc);
    csrSeg3ClosestPoint(&sca, pP, &rca);

    // calculate the distances between points below and test point
    csrVec3Sub(pP, &rab, &vAB);
    csrVec3Sub(pP, &rbc, &vBC);
    csrVec3Sub(pP, &rca, &vCA);

    // calculate the length of each segments
    csrVec3Length(&vAB, &dAB);
    csrVec3Length(&vBC, &dBC);
    csrVec3Length(&vCA, &dCA);

    // calculate the shortest distance
    min = dAB;
    *pR = rab;

    // check if dBC is shortest
    if (dBC < min)
    {
        min = dBC;
        *pR = rbc;
    }

    // check if dCA is shortest
    if (dCA < min)
        *pR = rca;
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
{
    return (pP->m_X >= pB->m_Min.m_X && pP->m_Y >= pB->m_Min.m_Y && pP->m_Z >= pB->m_Min.m_Z &&
            pP->m_X <= pB->m_Max.m_X && pP->m_Y <= pB->m_Max.m_Y && pP->m_Z <= pB->m_Max.m_Z);
}
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
// Intersection checks
//---------------------------------------------------------------------------
int csrIntersectRayPlane(const CSR_Ray3* pRa, const CSR_Plane* pPl, CSR_Vector3* pR)
{
    CSR_Vector3 n;
    float       dot;
    float       nDot;
    float       temp;

    // get the normal of the plane
    n.m_X = pPl->m_A;
    n.m_Y = pPl->m_B;
    n.m_Z = pPl->m_C;

    // calculate the angle between the line and the normal to the plane
    csrVec3Dot(&n, &pRa->m_Dir, &dot);

    // if normal to the plane is perpendicular to the line, then the line is either parallel to the
    // plane and there are no solutions or the line is on the plane in which case there are an
    // infinite number of solutions
    if (!dot)
        return 0;

    csrVec3Dot(&n, &pRa->m_Pos, &nDot);

    temp = ((pPl->m_D + nDot) / dot);

    // calculates the intersection point
    pR->m_X = (pRa->m_Pos.m_X - (temp * pRa->m_Dir.m_X));
    pR->m_Y = (pRa->m_Pos.m_Y - (temp * pRa->m_Dir.m_Y));
    pR->m_Z = (pRa->m_Pos.m_Z - (temp * pRa->m_Dir.m_Z));

    return 1;
}
//---------------------------------------------------------------------------
int csrIntersectSegPlane(const CSR_Segment3* pS, const CSR_Plane* pPl, CSR_Vector3* pR)
{
    CSR_Vector3 dir;
    CSR_Ray3    ray;

    // get the ray start position
    ray.m_Pos = pS->m_Start;

    // calculate the ray direction. NOTE the inverted direction can be omitted here because this
    // value will not be used by the csrIntersectRayPlane() function
    csrVec3Sub(&pS->m_End, &pS->m_Start, &dir);
    csrVec3Normalize(&dir, &ray.m_Dir);

    if (csrIntersectRayPlane(&ray, pPl, pR))
        // the segment will only intersect the plane if the intersection point is inside the segment
        // limits
        return csrVec3BetweenRange(pR, &pS->m_Start, &pS->m_End, M_CSR_Epsilon);

    return 0;
}
//---------------------------------------------------------------------------
int csrIntersectRayPolygon(const CSR_Ray3* pR, const CSR_Polygon* pP)
{
    CSR_Plane   polygonPlane;
    CSR_Vector3 pointOnPlane;

    // create a plane using the 3 vertices of the polygon
    csrPlaneFromPoints(&pP->m_Vertex[0], &pP->m_Vertex[1], &pP->m_Vertex[2], &polygonPlane);

    // calculate the intersection point
    if (!csrIntersectRayPlane(pR, &polygonPlane, &pointOnPlane))
        return 0;

    // check if calculated point is inside the polygon
    return csrInsidePolygon(&pointOnPlane, pP);
}
//---------------------------------------------------------------------------
int csrIntersectSegPolygon(const CSR_Segment3* pS, const CSR_Polygon* pP)
{
    CSR_Plane   polygonPlane;
    CSR_Vector3 pointOnPlane;

    // create a plane using the 3 vertices of the polygon
    csrPlaneFromPoints(&pP->m_Vertex[0], &pP->m_Vertex[1], &pP->m_Vertex[2], &polygonPlane);

    // calculate the intersection point
    if (!csrIntersectSegPlane(pS, &polygonPlane, &pointOnPlane))
        return 0;

    // check if calculated point is inside the polygon
    return csrInsidePolygon(&pointOnPlane, pP);
}
//---------------------------------------------------------------------------
int csrIntersectPolygons(const CSR_Polygon* pP1, const CSR_Polygon* pP2)
{
    CSR_Plane    plane1;
    CSR_Plane    plane2;
    CSR_Segment3 v1v2Seg;
    CSR_Segment3 v2v3Seg;
    CSR_Segment3 v3v1Seg;
    CSR_Segment3 ov1ov2Seg;
    CSR_Segment3 ov2ov3Seg;
    CSR_Segment3 ov3ov1Seg;
    CSR_Vector3  normal1;
    CSR_Vector3  normal2;
    CSR_Vector3  p1pt1;
    CSR_Vector3  p1pt2;
    CSR_Vector3  p1pt3;
    CSR_Vector3  p2pt1;
    CSR_Vector3  p2pt2;
    CSR_Vector3  p2pt3;
    CSR_Vector3  p;
    int          p1ptsCount;
    int          p2ptsCount;
    float        planesDot;
    float        dist1v1;
    float        dist1v2;
    float        dist1v3;
    float        dist2v1;
    float        dist2v2;
    float        dist2v3;
    float        result;

    // todo -cCheck -oJean: I think something is wrong in this algorithm. It should be revised

    // get planes from polygons
    csrPlaneFromPoints(&pP1->m_Vertex[0], &pP1->m_Vertex[1], &pP1->m_Vertex[2], &plane1);
    csrPlaneFromPoints(&pP2->m_Vertex[0], &pP2->m_Vertex[1], &pP2->m_Vertex[2], &plane2);

    // coplanar?
    if (((plane1.m_A ==  plane2.m_A)  &&
         (plane1.m_B ==  plane2.m_B)  &&
         (plane1.m_C ==  plane2.m_C)  &&
         (plane1.m_D ==  plane2.m_D)) ||
        ((plane1.m_A == -plane2.m_A)  &&
         (plane1.m_B == -plane2.m_B)  &&
         (plane1.m_C == -plane2.m_C)  &&
         (plane1.m_D == -plane2.m_D)))
    {
        // is any vertex inside other polygon?
        if (csrInsidePolygon(&pP1->m_Vertex[0], pP2) ||
            csrInsidePolygon(&pP1->m_Vertex[1], pP2) ||
            csrInsidePolygon(&pP1->m_Vertex[2], pP2) ||
            csrInsidePolygon(&pP2->m_Vertex[0], pP1) ||
            csrInsidePolygon(&pP2->m_Vertex[1], pP1) ||
            csrInsidePolygon(&pP2->m_Vertex[2], pP1))
            return 1;

        // create polygon lines
        v1v2Seg.m_Start   = pP1->m_Vertex[0];
        v1v2Seg.m_End     = pP1->m_Vertex[1];
        v2v3Seg.m_Start   = pP1->m_Vertex[1];
        v2v3Seg.m_End     = pP1->m_Vertex[2];
        v3v1Seg.m_Start   = pP1->m_Vertex[2];
        v3v1Seg.m_End     = pP1->m_Vertex[0];
        ov1ov2Seg.m_Start = pP2->m_Vertex[0];
        ov1ov2Seg.m_End   = pP2->m_Vertex[1];
        ov2ov3Seg.m_Start = pP2->m_Vertex[1];
        ov2ov3Seg.m_End   = pP2->m_Vertex[2];
        ov3ov1Seg.m_Start = pP2->m_Vertex[2];
        ov3ov1Seg.m_End   = pP2->m_Vertex[0];

        csrSeg3ShortestDistance(&v1v2Seg, &ov1ov2Seg, M_CSR_Epsilon, &result);

        // is shortest distance between lines equal to 0?
        if (result < M_CSR_Epsilon)
            return 1;

        csrSeg3ShortestDistance(&v2v3Seg, &ov1ov2Seg, M_CSR_Epsilon, &result);

        // is shortest distance between lines equal to 0?
        if (result < M_CSR_Epsilon)
            return 1;

        csrSeg3ShortestDistance(&v3v1Seg, &ov1ov2Seg, M_CSR_Epsilon, &result);

        // is shortest distance between lines equal to 0?
        if (result < M_CSR_Epsilon)
            return 1;

        csrSeg3ShortestDistance(&v1v2Seg, &ov2ov3Seg, M_CSR_Epsilon, &result);

        // is shortest distance between lines equal to 0?
        if (result < M_CSR_Epsilon)
            return 1;

        csrSeg3ShortestDistance(&v2v3Seg, &ov2ov3Seg, M_CSR_Epsilon, &result);

        // is shortest distance between lines equal to 0?
        if (result < M_CSR_Epsilon)
            return 1;

        csrSeg3ShortestDistance(&v3v1Seg, &ov2ov3Seg, M_CSR_Epsilon, &result);

        // is shortest distance between lines equal to 0?
        if (result < M_CSR_Epsilon)
            return 1;

        csrSeg3ShortestDistance(&v1v2Seg, &ov3ov1Seg, M_CSR_Epsilon, &result);

        // is shortest distance between lines equal to 0?
        if (result < M_CSR_Epsilon)
            return 1;

        csrSeg3ShortestDistance(&v2v3Seg, &ov3ov1Seg, M_CSR_Epsilon, &result);

        // is shortest distance between lines equal to 0?
        if (result < M_CSR_Epsilon)
            return 1;

        csrSeg3ShortestDistance(&v3v1Seg, &ov3ov1Seg, M_CSR_Epsilon, &result);

        // is shortest distance between lines equal to 0?
        if (result < M_CSR_Epsilon)
            return 1;

        return 0;
    }

    // get plane normals
    normal1.m_X = plane1.m_A;
    normal1.m_Y = plane1.m_B;
    normal1.m_Z = plane1.m_C;

    normal2.m_X = plane2.m_A;
    normal2.m_Y = plane2.m_B;
    normal2.m_Z = plane2.m_C;

    // calculate angle between planes
    csrVec3Dot(&normal1, &normal2, &planesDot);

    // are plane parallels?
    if (planesDot == 1.0f || planesDot == -1.0f)
        // planes are parallels but not coplanars, no collision is possible
        return 0;

    // calculate distance from each first polygon vertex to second polygon plane
    csrPlaneDistanceTo(&pP1->m_Vertex[0], &plane2, &dist1v1);
    csrPlaneDistanceTo(&pP1->m_Vertex[1], &plane2, &dist1v2);
    csrPlaneDistanceTo(&pP1->m_Vertex[2], &plane2, &dist1v3);

    // prepare list containing first polygon intersection points
    p1ptsCount = 0;

    // is first polygon V1 to V2 line segment intersects second polygon plane?
    if ((dist1v1 >= 0.0f && dist1v2 < 0.0f) || (dist1v1 < 0.0f && dist1v2 >= 0.0f))
    {
        v1v2Seg.m_Start   = pP1->m_Vertex[0];
        v1v2Seg.m_End     = pP1->m_Vertex[1];

        // calculate intersection point and add to list on success
        if (csrIntersectSegPlane(&v1v2Seg, &plane2, &p))
        {
            p1pt1 = p;
            ++p1ptsCount;
        }
    }

    // is first polygon V2 to V3 line segment intersects second polygon plane?
    if ((dist1v2 >= 0.0f && dist1v3 < 0.0f) || (dist1v2 < 0.0f && dist1v3 >= 0.0f))
    {
        v2v3Seg.m_Start   = pP1->m_Vertex[1];
        v2v3Seg.m_End     = pP1->m_Vertex[2];

        // calculate intersection point and add to list on success
        if (csrIntersectSegPlane(&v2v3Seg, &plane2, &p))
        {
            p1pt2 = p;
            ++p1ptsCount;
        }
    }

    // is first polygon V3 to V1 line segment intersects second polygon plane?
    if ((dist1v3 >= 0.0f && dist1v1 < 0.0f) || (dist1v3 < 0.0f && dist1v1 >= 0.0f))
    {
        v3v1Seg.m_Start   = pP1->m_Vertex[2];
        v3v1Seg.m_End     = pP1->m_Vertex[0];

        // calculate intersection point and add to list on success
        if (csrIntersectSegPlane(&v3v1Seg, &plane2, &p))
        {
            p1pt3 = p;
            ++p1ptsCount;
        }
    }

    // 2 intersection point were found on the first polygon?
    if (p1ptsCount != 2)
        return 0;

    // calculate distance from each second polygon vertex to first polygon plane
    csrPlaneDistanceTo(&pP2->m_Vertex[0], &plane1, &dist2v1);
    csrPlaneDistanceTo(&pP2->m_Vertex[1], &plane1, &dist2v2);
    csrPlaneDistanceTo(&pP2->m_Vertex[2], &plane1, &dist2v3);

    // prepare list containing second polygon intersection points
    p2ptsCount = 0;

    // is second polygon V1 to V2 line segment intersects first polygon plane?
    if ((dist2v1 >= 0.0f && dist2v2 < 0.0f) || (dist2v1 < 0.0f && dist2v2 >= 0.0f))
    {
        v1v2Seg.m_Start   = pP2->m_Vertex[0];
        v1v2Seg.m_End     = pP2->m_Vertex[1];

        // calculate intersection point and add to list on success
        if (csrIntersectSegPlane(&v1v2Seg, &plane1, &p))
        {
            p2pt1 = p;
            ++p2ptsCount;
        }
    }

    // is second polygon V2 to V3 line segment intersects first polygon plane?
    if ((dist2v2 >= 0.0f && dist2v3 < 0.0f) || (dist2v2 < 0.0f && dist2v3 >= 0.0f))
    {
        v2v3Seg.m_Start   = pP2->m_Vertex[1];
        v2v3Seg.m_End     = pP2->m_Vertex[2];

        // calculate intersection point and add to list on success
        if (csrIntersectSegPlane(&v2v3Seg, &plane1, &p))
        {
            p2pt2 = p;
            ++p2ptsCount;
        }
    }

    // is second polygon V3 to V1 line segment intersects first polygon plane?
    if ((dist2v3 >= 0.0f && dist2v1 < 0.0f) || (dist2v3 < 0.0f && dist2v1 >= 0.0f))
    {
        v3v1Seg.m_Start   = pP2->m_Vertex[2];
        v3v1Seg.m_End     = pP2->m_Vertex[0];

        // calculate intersection point and add to list on success
        if (csrIntersectSegPlane(&v3v1Seg, &plane1, &p))
        {
            p2pt3 = p;
            ++p2ptsCount;
        }
    }

    // 2 intersection point were found on the second polygon?
    if (p2ptsCount != 2)
        return 0;

    // todo -cCheck -oJean: Not sure if this part is required and/or works well. To be tested

    // first and second polygon intersection points are on the same line, so check if calculated
    // first and second polygon segments intersect
    if (csrVec3BetweenRange(&p1pt1, &p2pt1, &p2pt2, M_CSR_Epsilon))
        return 1;

    if (csrVec3BetweenRange(&p1pt2, &p2pt1, &p2pt2, M_CSR_Epsilon))
        return 1;

    if (csrVec3BetweenRange(&p1pt3, &p2pt1, &p2pt2, M_CSR_Epsilon))
        return 1;

    if (csrVec3BetweenRange(&p1pt1, &p2pt2, &p2pt3, M_CSR_Epsilon))
        return 1;

    if (csrVec3BetweenRange(&p1pt2, &p2pt2, &p2pt3, M_CSR_Epsilon))
        return 1;

    if (csrVec3BetweenRange(&p1pt3, &p2pt2, &p2pt3, M_CSR_Epsilon))
        return 1;

    if (csrVec3BetweenRange(&p1pt1, &p2pt3, &p2pt1, M_CSR_Epsilon))
        return 1;

    if (csrVec3BetweenRange(&p1pt2, &p2pt3, &p2pt1, M_CSR_Epsilon))
        return 1;

    if (csrVec3BetweenRange(&p1pt3, &p2pt3, &p2pt1, M_CSR_Epsilon))
        return 1;

    if (csrVec3BetweenRange(&p2pt1, &p1pt1, &p1pt2, M_CSR_Epsilon))
        return 1;

    if (csrVec3BetweenRange(&p2pt2, &p1pt1, &p1pt2, M_CSR_Epsilon))
        return 1;

    if (csrVec3BetweenRange(&p2pt3, &p1pt1, &p1pt2, M_CSR_Epsilon))
        return 1;

    if (csrVec3BetweenRange(&p2pt1, &p1pt2, &p1pt3, M_CSR_Epsilon))
        return 1;

    if (csrVec3BetweenRange(&p2pt2, &p1pt2, &p1pt3, M_CSR_Epsilon))
        return 1;

    if (csrVec3BetweenRange(&p2pt3, &p1pt2, &p1pt3, M_CSR_Epsilon))
        return 1;

    if (csrVec3BetweenRange(&p2pt1, &p1pt3, &p1pt1, M_CSR_Epsilon))
        return 1;

    if (csrVec3BetweenRange(&p2pt2, &p1pt3, &p1pt1, M_CSR_Epsilon))
        return 1;

    if (csrVec3BetweenRange(&p2pt3, &p1pt3, &p1pt1, M_CSR_Epsilon))
        return 1;

    return 0;
}
//---------------------------------------------------------------------------
int miniIntersectRayBox(const CSR_Ray3* pR, const CSR_Box* pB)
{
    float tx1;
    float tx2;
    float ty1;
    float ty2;
    float tz1;
    float tz2;
    float txn;
    float txf;
    float tyn;
    float tyf;
    float tzn;
    float tzf;
    float tnear;
    float tfar;

    // get infinite value
    const float inf = 1.0f / 0.0f;

    // calculate nearest point where ray intersects box on x coordinate
    if (pR->m_InvDir.m_X != inf)
        tx1 = ((pB->m_Min.m_X - pR->m_Pos.m_X) * pR->m_InvDir.m_X);
    else
    if ((pB->m_Min.m_X - pR->m_Pos.m_X) < 0.0f)
        tx1 = -inf;
    else
        tx1 = inf;

    // calculate farthest point where ray intersects box on x coordinate
    if (pR->m_InvDir.m_X != inf)
        tx2 = ((pB->m_Max.m_X - pR->m_Pos.m_X) * pR->m_InvDir.m_X);
    else
    if ((pB->m_Max.m_X - pR->m_Pos.m_X) < 0.0f)
        tx2 = -inf;
    else
        tx2 = inf;

    // calculate nearest point where ray intersects box on y coordinate
    if (pR->m_InvDir.m_Y != inf)
        ty1 = ((pB->m_Min.m_Y - pR->m_Pos.m_Y) * pR->m_InvDir.m_Y);
    else
    if ((pB->m_Min.m_Y - pR->m_Pos.m_Y) < 0.0f)
        ty1 = -inf;
    else
        ty1 = inf;

    // calculate farthest point where ray intersects box on y coordinate
    if (pR->m_InvDir.m_Y != inf)
        ty2 = ((pB->m_Max.m_Y - pR->m_Pos.m_Y) * pR->m_InvDir.m_Y);
    else
    if ((pB->m_Max.m_Y - pR->m_Pos.m_Y) < 0.0f)
        ty2 = -inf;
    else
        ty2 = inf;

    // calculate nearest point where ray intersects box on z coordinate
    if (pR->m_InvDir.m_Z != inf)
        tz1 = ((pB->m_Min.m_Z - pR->m_Pos.m_Z) * pR->m_InvDir.m_Z);
    else
    if ((pB->m_Min.m_Z - pR->m_Pos.m_Z) < 0.0f)
        tz1 = -inf;
    else
        tz1 = inf;

    // calculate farthest point where ray intersects box on z coordinate
    if (pR->m_InvDir.m_Z != inf)
        tz2 = ((pB->m_Max.m_Z - pR->m_Pos.m_Z) * pR->m_InvDir.m_Z);
    else
    if ((pB->m_Max.m_Z - pR->m_Pos.m_Z) < 0.0f)
        tz2 = -inf;
    else
        tz2 = inf;

    // calculate near/far intersection on each axis
    csrMathMin(tx1, tx2, &txn);
    csrMathMax(tx1, tx2, &txf);
    csrMathMin(ty1, ty2, &tyn);
    csrMathMax(ty1, ty2, &tyf);
    csrMathMin(tz1, tz2, &tzn);
    csrMathMax(tz1, tz2, &tzf);

    // calculate final near/far intersection point
    csrMathMax(tyn, tzn,   &tnear);
    csrMathMax(txn, tnear, &tnear);
    csrMathMin(tyf, tzf,  &tfar);
    csrMathMin(txf, tfar, &tfar);

    // check if ray intersects box
    return (tfar >= tnear);
}
//---------------------------------------------------------------------------
int csrIntersectBoxes(const CSR_Box* pB1, const CSR_Box* pB2)
{
    // FIXME implement that

    /*FIXME
    return !(pFirstRect->m_Pos.m_X                               <= pSecondRect->m_Pos.m_X + pSecondRect->m_Size.m_Width  &&
             pFirstRect->m_Pos.m_X + pFirstRect->m_Size.m_Width  >= pSecondRect->m_Pos.m_X                                &&
             pFirstRect->m_Pos.m_Y                               <= pSecondRect->m_Pos.m_Y + pSecondRect->m_Size.m_Height &&
             pFirstRect->m_Pos.m_Y + pFirstRect->m_Size.m_Height >= pSecondRect->m_Pos.m_Y);
    */
}
//---------------------------------------------------------------------------
int csrIntersectSpherePolygon(const CSR_Sphere* pS, const CSR_Polygon* pP, CSR_Plane* pR)
{
    float        testPoint1;
    float        testPoint2;
    CSR_Vector3  sphereNormal;
    CSR_Vector3  pointOnSphere;
    CSR_Vector3  pointOnPlane;
    CSR_Vector3  pointOnTriangle;
    CSR_Plane    polygonPlane;
    CSR_Segment3 seg;

    // create a plane using the 3 vertices of the polygon
    csrPlaneFromPoints(&pP->m_Vertex[0], &pP->m_Vertex[1], &pP->m_Vertex[2], &polygonPlane);

    // calculate the distance between the center of the sphere and the plane
    csrPlaneDistanceTo(&pS->m_Center, &polygonPlane, &testPoint1);

    // calculate the normal of the distance sphere-plane using the positive or negative value of the
    // calculated distance between plane and position
    if (testPoint1 < 0.0f)
    {
        sphereNormal.m_X = polygonPlane.m_A;
        sphereNormal.m_Y = polygonPlane.m_B;
        sphereNormal.m_Z = polygonPlane.m_C;
    }
    else
    {
        sphereNormal.m_X = -polygonPlane.m_A;
        sphereNormal.m_Y = -polygonPlane.m_B;
        sphereNormal.m_Z = -polygonPlane.m_C;
    }

    // calculate the point who the segment from center of sphere in the direction of the plane will
    // cross the border of the sphere
    pointOnSphere.m_X = pS->m_Center.m_X + (pS->m_Radius * sphereNormal.m_X);
    pointOnSphere.m_Y = pS->m_Center.m_Y + (pS->m_Radius * sphereNormal.m_Y),
    pointOnSphere.m_Z = pS->m_Center.m_Z + (pS->m_Radius * sphereNormal.m_Z);

    // calculate the distance between the border of the sphere and the plane
    csrPlaneDistanceTo(&pointOnSphere, &polygonPlane, &testPoint2);

    // if the test points are on each side of the plane, then the sphere cross the plane. We assume
    // that the segment from the center of the sphere to the direction of the plane can never be
    // coplanar
    if ((testPoint1 <= 0.0f && testPoint2 >= 0.0f) || (testPoint2 <= 0.0f && testPoint1 >= 0.0f))
    {
        // calculate who the segment cross the plane
        if (!testPoint1)
            // if testPoint1 is equal to 0, the center of the sphere cross the plane
            pointOnPlane = pS->m_Center;
        else
        if (!testPoint2)
            // if testPoint2 is equal to 0, the border of the sphere cross the plane
            pointOnPlane = pointOnSphere;
        else
        {
            seg.m_Start = pS->m_Center;
            seg.m_End   = pointOnSphere;

            // calculate the intersection point
            csrIntersectSegPlane(&seg, &polygonPlane, &pointOnPlane);
        }

        // check if calculated point is inside the polygon
        if (csrInsidePolygon(&pointOnPlane, pP))
        {
            // if yes, the sphere collide the polygon. In this case, we copy the plane and we
            // returns true
            *pR = polygonPlane;
            return 1;
        }
        else
        {
            // otherwise check if the sphere collide the border of the polygon. First we calculate
            // the test point on the border of the polygon
            csrPolygonClosestPoint(&pointOnPlane, pP, &pointOnTriangle);

            // check if calculated point is inside the sphere
            if (csrInsideSphere(&pointOnTriangle, pS))
            {
                // if yes, the sphere collide the polygon. In this case, we copy the plane and we
                // returns true
                *pR = polygonPlane;
                return 1;
            }
        }
    }

    // no collision was found
    return 0;
}
//---------------------------------------------------------------------------
int csrIntersectSphereBox(const CSR_Sphere* pS, const CSR_Box* pB)
{
    // FIXME implement that
}
//---------------------------------------------------------------------------
int csrIntersectSpheres(const CSR_Sphere* pS1, const CSR_Sphere* pS2)
{
    CSR_Vector3 dist;
    float        length;

    // calculate the distance between the both sphere centers
    dist.m_X = fabs(pS1->m_Center.m_X - pS2->m_Center.m_X);
    dist.m_Y = fabs(pS1->m_Center.m_Y - pS2->m_Center.m_Y);
    dist.m_Z = fabs(pS1->m_Center.m_Z - pS2->m_Center.m_Z);

    // calculate the length between the both sphere centers
    csrVec3Length(&dist, &length);

    // the spheres are on collision if the length between the centers is lower than or equal to the
    // sum of the both sphere radius
    return (length <= (pS1->m_Radius + pS2->m_Radius));
}
//---------------------------------------------------------------------------