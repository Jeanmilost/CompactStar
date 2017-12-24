/****************************************************************************
 * ==> CSR_Collision -------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides the colision detection functions      *
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

#include "CSR_Collision.h"

// std
//REM #include <stdlib.h>
//REM #include <math.h>

//---------------------------------------------------------------------------
// Aligned-Axis Bounding Box tree
//---------------------------------------------------------------------------
void miniAddPolygonToBoundingBox(const MINI_Polygon* pPolygon,
                                       MINI_Box*     pBox,
                                       int*          pEmpty)
{
    float    r;
    unsigned i;

    // iterate through polygon vertices
    for (i = 0; i < 3; ++i)
    {
        // is box empty?
        if (*pEmpty)
        {
            // initialize bounding box with first vertex
            miniCopy(&pPolygon->m_v[i], &pBox->m_Min);
            miniCopy(&pPolygon->m_v[i], &pBox->m_Max);
            *pEmpty = 0;
            continue;
        }

        // search for box min edge
        miniMin(&pBox->m_Min.m_X, &pPolygon->m_v[i].m_X, &r);
        pBox->m_Min.m_X = r;
        miniMin(&pBox->m_Min.m_Y, &pPolygon->m_v[i].m_Y, &r);
        pBox->m_Min.m_Y = r;
        miniMin(&pBox->m_Min.m_Z, &pPolygon->m_v[i].m_Z, &r);
        pBox->m_Min.m_Z = r;

        // search for box max edge
        miniMax(&pBox->m_Max.m_X, &pPolygon->m_v[i].m_X, &r);
        pBox->m_Max.m_X = r;
        miniMax(&pBox->m_Max.m_Y, &pPolygon->m_v[i].m_Y, &r);
        pBox->m_Max.m_Y = r;
        miniMax(&pBox->m_Max.m_Z, &pPolygon->m_v[i].m_Z, &r);
        pBox->m_Max.m_Z = r;
    }
}
//---------------------------------------------------------------------------
void miniAddPolygon(const float*         pVB,
                          unsigned       v1,
                          unsigned       v2,
                          unsigned       v3,
                          MINI_Polygon** pPolygons,
                          unsigned*      pPolygonsCount)
{
    // polygon array already contains polygons?
    if (!(*pPolygonsCount))
    {
        // no, add new first polygon in array
        *pPolygons     = (MINI_Polygon*)malloc(sizeof(MINI_Polygon));
        *pPolygonsCount = 1;
    }
    else
    {
        // yes, increase the polygons count and add new polygon inside array
        ++(*pPolygonsCount);
        *pPolygons = (MINI_Polygon*)realloc(*pPolygons,
                                            *pPolygonsCount * sizeof(MINI_Polygon));
    }

    // copy polygon
    (*pPolygons)[*pPolygonsCount - 1].m_v[0].m_X = pVB[v1];
    (*pPolygons)[*pPolygonsCount - 1].m_v[0].m_Y = pVB[v1 + 1];
    (*pPolygons)[*pPolygonsCount - 1].m_v[0].m_Z = pVB[v1 + 2];
    (*pPolygons)[*pPolygonsCount - 1].m_v[1].m_X = pVB[v2];
    (*pPolygons)[*pPolygonsCount - 1].m_v[1].m_Y = pVB[v2 + 1];
    (*pPolygons)[*pPolygonsCount - 1].m_v[1].m_Z = pVB[v2 + 2];
    (*pPolygons)[*pPolygonsCount - 1].m_v[2].m_X = pVB[v3];
    (*pPolygons)[*pPolygonsCount - 1].m_v[2].m_Y = pVB[v3 + 1];
    (*pPolygons)[*pPolygonsCount - 1].m_v[2].m_Z = pVB[v3 + 2];
}
//---------------------------------------------------------------------------
int miniGetPolygonsFromVB(const float*         pVB,
                                unsigned       length,
                                unsigned       type,
                                unsigned       stride,
                                MINI_Polygon** pPolygons,
                                unsigned*      pPolygonsCount)
{
    unsigned i;
    unsigned index;

    // no data to extract from?
    if (!length)
        return 1;

    // search for vertex type
    switch (type)
    {
        // triangles
        case 0:
        {
            // calculate iteration step
            const unsigned step = (stride * 3);

            // iterate through source vertices
            for (i = 0; i < length; i += step)
                // extract polygon from source buffer
                miniAddPolygon(pVB,
                               i,
                               i +  stride,
                               i + (stride * 2),
                               pPolygons,
                               pPolygonsCount);

            return 1;
        }

        // triangle strip
        case 1:
        {
            // calculate length to read in triangle strip buffer
            const unsigned stripLength = (length - (stride * 2));
                           index       = 0;

            // iterate through source vertices
            for (i = 0; i < stripLength; i += stride)
            {
                // extract polygon from source buffer, revert odd polygons
                if (!index || !(index % 2))
                    miniAddPolygon(pVB,
                                   i,
                                   i +  stride,
                                   i + (stride * 2),
                                   pPolygons,
                                   pPolygonsCount);
                else
                    miniAddPolygon(pVB,
                                   i +  stride,
                                   i,
                                   i + (stride * 2),
                                   pPolygons,
                                   pPolygonsCount);

                ++index;
            }

            return 1;
        }

        // triangle fan
        case 2:
        {
            // calculate length to read in triangle fan buffer
            const unsigned fanLength = (length - stride);

            // iterate through source vertices
            for (i = stride; i < fanLength; i += stride)
                // extract polygon from source buffer
                miniAddPolygon(pVB,
                               0,
                               i,
                               i + stride,
                               pPolygons,
                               pPolygonsCount);

            return 1;
        }

        // quads
        case 3:
        {
            // calculate iteration step
            const unsigned step = (stride * 4);

            // iterate through source vertices
            for (i = 0; i < length; i += step)
            {
                // calculate vertices position
                const unsigned v1 = i;
                const unsigned v2 = i +  stride;
                const unsigned v3 = i + (stride * 2);
                const unsigned v4 = i + (stride * 3);

                // extract polygons from source buffer
                miniAddPolygon(pVB, v1, v2, v3, pPolygons, pPolygonsCount);
                miniAddPolygon(pVB, v3, v2, v4, pPolygons, pPolygonsCount);
            }

            return 1;
        }

        // quad strip
        case 4:
        {
            // calculate iteration step
            const unsigned step = (stride * 2);

            // calculate length to read in triangle strip buffer
            const unsigned stripLength = (length - (stride * 2));

            // iterate through source vertices
            for (i = 0; i < stripLength; i += step)
            {
                // calculate vertices position
                const unsigned v1 = i;
                const unsigned v2 = i +  stride;
                const unsigned v3 = i + (stride * 2);
                const unsigned v4 = i + (stride * 3);

                // extract polygons from source buffer
                miniAddPolygon(pVB, v1, v2, v3, pPolygons, pPolygonsCount);
                miniAddPolygon(pVB, v3, v2, v4, pPolygons, pPolygonsCount);
            }

            return 1;
        }

        default:
            return 0;
    }
}
//---------------------------------------------------------------------------
void miniReleasePolygons(MINI_Polygon* pPolygons)
{
    if (pPolygons)
        free(pPolygons);
}
//---------------------------------------------------------------------------
void miniCutBox(const MINI_Box* pBox, MINI_Box* pLeftBox, MINI_Box* pRightBox)
{
    float    x;
    float    y;
    float    z;
    unsigned longestAxis;

    // calculate each edge length
    x = fabs(pBox->m_Max.m_X - pBox->m_Min.m_X);
    y = fabs(pBox->m_Max.m_Y - pBox->m_Min.m_Y);
    z = fabs(pBox->m_Max.m_Z - pBox->m_Min.m_Z);

    // search for longest axis
    if (x >= y && x >= z)
        longestAxis = 0;
    else
    if (y >= x && y >= z)
        longestAxis = 1;
    else
        longestAxis = 2;

    // cut box
    switch (longestAxis)
    {
        // cut on x axis
        case 0:
            miniCopy(&pBox->m_Min, &pLeftBox->m_Min);
            miniCopy(&pBox->m_Max, &pLeftBox->m_Max);
            pLeftBox->m_Max.m_X = pBox->m_Min.m_X + (x / 2.0f);

            miniCopy(&pBox->m_Min, &pRightBox->m_Min);
            miniCopy(&pBox->m_Max, &pRightBox->m_Max);
            pRightBox->m_Min.m_X = pBox->m_Min.m_X + (x / 2.0f);
            break;

        // cut on y axis
        case 1:
            miniCopy(&pBox->m_Min, &pLeftBox->m_Min);
            miniCopy(&pBox->m_Max, &pLeftBox->m_Max);
            pLeftBox->m_Max.m_Y = pBox->m_Min.m_Y + (y / 2.0f);

            miniCopy(&pBox->m_Min, &pRightBox->m_Min);
            miniCopy(&pBox->m_Max, &pRightBox->m_Max);
            pRightBox->m_Min.m_Y = pBox->m_Min.m_Y + (y / 2.0f);
            break;

        // cut on z axis
        case 2:
            miniCopy(&pBox->m_Min, &pLeftBox->m_Min);
            miniCopy(&pBox->m_Max, &pLeftBox->m_Max);
            pLeftBox->m_Max.m_Z = pBox->m_Min.m_Z + (z / 2.0f);

            miniCopy(&pBox->m_Min, &pRightBox->m_Min);
            miniCopy(&pBox->m_Max, &pRightBox->m_Max);
            pRightBox->m_Min.m_Z = pBox->m_Min.m_Z + (z / 2.0f);
            break;
    }
}
//---------------------------------------------------------------------------
int miniPopulateTree(      MINI_AABBNode* pNode,
                     const MINI_Polygon*  pPolygons,
                           unsigned       polygonsCount)
{
    unsigned      i;
    unsigned      j;
    MINI_Box      leftBox;
    MINI_Box      rightBox;
    MINI_Polygon* pLeftPolygons      = 0;
    MINI_Polygon* pRightPolygons     = 0;
    unsigned      leftPolygonsCount  = 0;
    unsigned      rightPolygonsCount = 0;
    int           boxEmpty           = 1;
    int           canResolveLeft     = 0;
    int           canResolveRight    = 0;
    int           result             = 0;
    float         epsylon            = M_MINI_Epsilon;

    // initialize node content
    pNode->m_pParent       = 0;
    pNode->m_pLeft         = 0;
    pNode->m_pRight        = 0;
    pNode->m_pBox          = 0;
    pNode->m_pPolygons     = 0;
    pNode->m_PolygonsCount = 0;

    // create a collision box
    pNode->m_pBox = (MINI_Box*)malloc(sizeof(MINI_Box));

    // iterate through polygons to divide
    for (i = 0; i < polygonsCount; ++i)
        // calculate bounding box
        miniAddPolygonToBoundingBox(&pPolygons[i], pNode->m_pBox, &boxEmpty);

    // divide box in 2 sub-boxes
    miniCutBox(pNode->m_pBox, &leftBox, &rightBox);

    // iterate again through polygons to divide
    for (i = 0; i < polygonsCount; ++i)
        for (j = 0; j < 3; ++i)
            // check if first polygon vertice belongs to left or right sub-box
            if (miniVectorIsBetween(&pPolygons[i].m_v[j], &leftBox.m_Min, &leftBox.m_Max, &epsylon))
            {
                // left node polygon array already contains polygons?
                if (!leftPolygonsCount)
                {
                    // no, add new first polygon in array
                    pLeftPolygons     = (MINI_Polygon*)malloc(sizeof(MINI_Polygon));
                    leftPolygonsCount = 1;
                }
                else
                {
                    // yes, increase the polygons count and add new polygon inside array
                    ++leftPolygonsCount;
                    pLeftPolygons = (MINI_Polygon*)realloc(pLeftPolygons,
                                                           leftPolygonsCount * sizeof(MINI_Polygon));
                }

                // copy polygon
                miniCopy(&pPolygons[i].m_v[0], &pLeftPolygons[leftPolygonsCount - 1].m_v[0]);
                miniCopy(&pPolygons[i].m_v[1], &pLeftPolygons[leftPolygonsCount - 1].m_v[1]);
                miniCopy(&pPolygons[i].m_v[2], &pLeftPolygons[leftPolygonsCount - 1].m_v[2]);
                break;
            }
            else
            if (miniVectorIsBetween(&pPolygons[i].m_v[j], &rightBox.m_Min, &rightBox.m_Max, &epsylon))
            {
                // right node polygon array already contains polygons?
                if (!rightPolygonsCount)
                {
                    // no, add new first polygon in array
                    pRightPolygons    = (MINI_Polygon*)malloc(sizeof(MINI_Polygon));
                    rightPolygonsCount = 1;
                }
                else
                {
                    // yes, increase the polygons count and add new polygon inside array
                    ++rightPolygonsCount;
                    pRightPolygons = (MINI_Polygon*)realloc(pRightPolygons,
                                                            rightPolygonsCount * sizeof(MINI_Polygon));
                }

                // copy polygon
                miniCopy(&pPolygons[i].m_v[0], &pRightPolygons[rightPolygonsCount - 1].m_v[0]);
                miniCopy(&pPolygons[i].m_v[1], &pRightPolygons[rightPolygonsCount - 1].m_v[1]);
                miniCopy(&pPolygons[i].m_v[2], &pRightPolygons[rightPolygonsCount - 1].m_v[2]);
                break;
            }

    canResolveLeft  = (leftPolygonsCount  && leftPolygonsCount  < polygonsCount);
    canResolveRight = (rightPolygonsCount && rightPolygonsCount < polygonsCount);

    // leaf reached?
    if (!canResolveLeft && !canResolveRight)
    {
        // copy left polygons in polygon list
        for (i = 0; i < leftPolygonsCount; ++i)
        {
            // node polygon array already contains polygons?
            if (!pNode->m_PolygonsCount)
            {
                // no, add new first polygon in array
                pNode->m_pPolygons     = (MINI_Polygon*)malloc(sizeof(MINI_Polygon));
                pNode->m_PolygonsCount = 1;
            }
            else
            {
                // yes, increase the polygons count and add new polygon inside array
                ++pNode->m_PolygonsCount;
                pNode->m_pPolygons = (MINI_Polygon*)realloc(pNode->m_pPolygons,
                                                            pNode->m_PolygonsCount * sizeof(MINI_Polygon));
            }

            // copy polygon
            for (j = 0; j < 3 ; ++j)
                miniCopy(&pLeftPolygons[i].m_v[j], &pNode->m_pPolygons[pNode->m_PolygonsCount - 1].m_v[j]);
        }

        // copy right polygons in polygon list
        for (i = 0; i < rightPolygonsCount; ++i)
        {
            // node polygon array already contains polygons?
            if (!pNode->m_PolygonsCount)
            {
                // no, add new first polygon in array
                pNode->m_pPolygons     = (MINI_Polygon*)malloc(sizeof(MINI_Polygon));
                pNode->m_PolygonsCount = 1;
            }
            else
            {
                // yes, increase the polygons count and add new polygon inside array
                ++pNode->m_PolygonsCount;
                pNode->m_pPolygons = (MINI_Polygon*)realloc(pNode->m_pPolygons,
                                                            pNode->m_PolygonsCount * sizeof(MINI_Polygon));
            }

            // copy polygon
            for (j = 0; j < 3 ; ++j)
                miniCopy(&pRightPolygons[i].m_v[j], &pNode->m_pPolygons[pNode->m_PolygonsCount - 1].m_v[j]);
        }

        // delete left list, as it will no more be used
        if (pLeftPolygons)
            free(pLeftPolygons);

        // delete right list, as it will no more be used
        if (pRightPolygons)
            free(pRightPolygons);

        return 1;
    }

    // do create left node?
    if (canResolveLeft)
    {
        // create and populate left node
        pNode->m_pLeft            = (MINI_AABBNode*)malloc(sizeof(MINI_AABBNode));
        pNode->m_pLeft->m_pParent = pNode;

        result |= miniPopulateTree(pNode->m_pLeft, pLeftPolygons, leftPolygonsCount);

        // delete current list, as it will no more be used
        if (pLeftPolygons)
            free(pLeftPolygons);
    }

    // do create right node?
    if (canResolveRight)
    {
        // create and populate right node
        pNode->m_pRight            = (MINI_AABBNode*)malloc(sizeof(MINI_AABBNode));
        pNode->m_pRight->m_pParent = pNode;

        result |= miniPopulateTree(pNode->m_pRight, pRightPolygons, rightPolygonsCount);

        // delete current list, as it will no more be used
        if (pRightPolygons)
            free(pRightPolygons);
    }

    return result;
}
//---------------------------------------------------------------------------
int miniResolveTree(MINI_Ray*      pRay,
                    MINI_AABBNode* pNode,
                    MINI_Polygon** pPolygons,
                    unsigned*      pPolygonsCount)
{
    unsigned i;
    unsigned j;
    int      leftResolved  = 0;
    int      rightResolved = 0;

    // no node to resolve? (this should never happen, but...)
    if (!pNode)
        return 0;

    // is leaf?
    if (!pNode->m_pLeft && !pNode->m_pRight)
    {
        for (i = 0; i < pNode->m_PolygonsCount; ++i)
        {
            // polygon array already contains polygons?
            if (!(*pPolygonsCount))
            {
                // no, add new first polygon in array
                *pPolygons     = (MINI_Polygon*)malloc(sizeof(MINI_Polygon));
                *pPolygonsCount = 1;
            }
            else
            {
                // yes, increase the polygons count and add new polygon inside array
                ++(*pPolygonsCount);
                *pPolygons = (MINI_Polygon*)realloc(*pPolygons,
                                                    *pPolygonsCount * sizeof(MINI_Polygon));
            }

            // copy polygon
            for (j = 0; j < 3; ++j)
                miniCopy(&pNode->m_pPolygons[i].m_v[j], &(*pPolygons)[*pPolygonsCount - 1].m_v[j]);
        }

        return 1;
    }

    // check if ray intersects the left box
    if (pNode->m_pLeft && miniRayBoxIntersect(pRay, pNode->m_pLeft->m_pBox))
        // resolve left node
        leftResolved = miniResolveTree(pRay, pNode->m_pLeft, pPolygons, pPolygonsCount);

    // check if ray intersects the right box
    if (pNode->m_pRight && miniRayBoxIntersect(pRay, pNode->m_pRight->m_pBox))
        // resolve right node
        rightResolved = miniResolveTree(pRay, pNode->m_pRight, pPolygons, pPolygonsCount);

    return (leftResolved || rightResolved);
}
//---------------------------------------------------------------------------
void miniReleaseNode(MINI_AABBNode* pNode)
{
    // delete bounding box, if exists
    if (pNode->m_pBox)
        free(pNode->m_pBox);

    // delete polygon array, if exists
    if (pNode->m_pPolygons)
        free(pNode->m_pPolygons);

    pNode->m_PolygonsCount = 0;
}
//---------------------------------------------------------------------------
void miniReleaseTree(MINI_AABBNode* pNode)
{
    if (!pNode)
        return;

    // release all children on left side
    if (pNode->m_pLeft)
        miniReleaseTree(pNode->m_pLeft);

    // release all children on right side
    if (pNode->m_pRight)
        miniReleaseTree(pNode->m_pRight);

    // delete node content
    miniReleaseNode(pNode);

    // delete node
    free(pNode);
}
//---------------------------------------------------------------------------
// Collision reaction
//---------------------------------------------------------------------------
void miniGetSlidingPoint(const MINI_Plane*   pSlidingPlane,
                         const MINI_Vector3* pPosition,
                         const float*        pRadius,
                               MINI_Vector3* pR)
{
    float        distanceToPlane;
    MINI_Plane   plane;
    MINI_Vector3 planeRatio;
    MINI_Vector3 pointBeyondPlane;
    MINI_Vector3 pointOnPlane;

    plane = *pSlidingPlane;

    // calculate the distance between the center of the sphere and the plane
    miniDistanceToPlane(pPosition, &plane, &distanceToPlane);

    // check if value is negative
    if (distanceToPlane < 0.0f)
    {
        // invert the plane
        plane.m_A = -plane.m_A;
        plane.m_B = -plane.m_B;
        plane.m_C = -plane.m_C;
        plane.m_D = -plane.m_D;
    }

    // calculate the direction of the segment position - plane
    planeRatio.m_X = *pRadius * plane.m_A;
    planeRatio.m_Y = *pRadius * plane.m_B;
    planeRatio.m_Z = *pRadius * plane.m_C;

    // calculate who the segment perpendicular to the plane, from the center
    // of the sphere, cross the collision sphere. Normally this point is beyond
    // the plane
    pointBeyondPlane.m_X = pPosition->m_X - planeRatio.m_X;
    pointBeyondPlane.m_Y = pPosition->m_Y - planeRatio.m_Y;
    pointBeyondPlane.m_Z = pPosition->m_Z - planeRatio.m_Z;

    // calculate the point where the segment "center of the sphere - point beyond
    // the plane" cross the collision plane
    miniLinePlaneIntersect(pSlidingPlane,
                           pPosition,
                           &pointBeyondPlane,
                           &pointOnPlane);

    // from point calculated below, we add the radius of the sphere, and we
    // returns the value
    pR->m_X = pointOnPlane.m_X + planeRatio.m_X;
    pR->m_Y = pointOnPlane.m_Y + planeRatio.m_Y;
    pR->m_Z = pointOnPlane.m_Z + planeRatio.m_Z;
}
//---------------------------------------------------------------------------
