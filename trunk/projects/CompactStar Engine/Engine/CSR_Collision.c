/*****************************************************************************
 * ==> MiniCollisions -------------------------------------------------------*
 *****************************************************************************
 * Description : This module provides the functions required to detect the   *
 *               collisions inside a 2d or 3d world.                         *
 * Developer   : Jean-Milost Reymond                                         *
 * Copyright   : 2015 - 2017, this file is part of the Minimal API. You are  *
 *               free to copy or redistribute this file, modify it, or use   *
 *               it for your own projects, commercial or not. This file is   *
 *               provided "as is", without ANY WARRANTY OF ANY KIND          *
 *****************************************************************************/

#include "CSR_Collision.h"

// std
#include <stdlib.h>
#include <math.h>

//----------------------------------------------------------------------------
// Box calculation
//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
// Collision detection
//----------------------------------------------------------------------------
int miniPolygonsIntersect(MINI_Vector3* pP1V1, MINI_Vector3* pP1V2, MINI_Vector3* pP1V3,
                          MINI_Vector3* pP2V1, MINI_Vector3* pP2V2, MINI_Vector3* pP2V3)
{
    MINI_Plane   plane1;
    MINI_Plane   plane2;
    MINI_Vector3 v1v2LineS;
    MINI_Vector3 v1v2LineE;
    MINI_Vector3 v2v3LineS;
    MINI_Vector3 v2v3LineE;
    MINI_Vector3 v3v1LineS;
    MINI_Vector3 v3v1LineE;
    MINI_Vector3 ov1ov2LineS;
    MINI_Vector3 ov1ov2LineE;
    MINI_Vector3 ov2ov3LineS;
    MINI_Vector3 ov2ov3LineE;
    MINI_Vector3 ov3ov1LineS;
    MINI_Vector3 ov3ov1LineE;
    MINI_Vector3 normal1;
    MINI_Vector3 normal2;
    MINI_Vector3 p1pt1;
    MINI_Vector3 p1pt2;
    MINI_Vector3 p1pt3;
    MINI_Vector3 p2pt1;
    MINI_Vector3 p2pt2;
    MINI_Vector3 p2pt3;
    MINI_Vector3 p;
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
    float        epsylon = 1.0E-3;

    // get planes from polygons
    miniPlaneFromPoints(pP1V1, pP1V2, pP1V3, &plane1);
    miniPlaneFromPoints(pP2V1, pP2V2, pP2V3, &plane2);

    // are planes merged?
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
        if (miniInside(pP1V1, pP2V1, pP2V2, pP2V3) ||
            miniInside(pP1V2, pP2V1, pP2V2, pP2V3) ||
            miniInside(pP1V3, pP2V1, pP2V2, pP2V3) ||
            miniInside(pP2V1, pP1V1, pP1V2, pP1V3) ||
            miniInside(pP2V2, pP1V1, pP1V2, pP1V3) ||
            miniInside(pP2V3, pP1V1, pP1V2, pP1V3))
            return 1;

        // create polygon lines
        miniCopy(pP1V1, &v1v2LineS);
        miniCopy(pP1V2, &v1v2LineE);
        miniCopy(pP1V2, &v2v3LineS);
        miniCopy(pP1V3, &v2v3LineE);
        miniCopy(pP1V3, &v3v1LineS);
        miniCopy(pP1V1, &v3v1LineE);
        miniCopy(pP2V1, &ov1ov2LineS);
        miniCopy(pP2V2, &ov1ov2LineE);
        miniCopy(pP2V2, &ov2ov3LineS);
        miniCopy(pP2V3, &ov2ov3LineE);
        miniCopy(pP2V3, &ov3ov1LineS);
        miniCopy(pP2V1, &ov3ov1LineE);

        miniGetShortestDistance(&v1v2LineS,   &v1v2LineE,
                                &ov1ov2LineS, &ov1ov2LineE, &epsylon, &result);

        // is shortest distance between lines equal to 0?
        if (result < epsylon)
            return 1;

        miniGetShortestDistance(&v2v3LineS,   &v2v3LineE,
                                &ov1ov2LineS, &ov1ov2LineE, &epsylon, &result);

        // is shortest distance between lines equal to 0?
        if (result < epsylon)
            return 1;

        miniGetShortestDistance(&v3v1LineS,   &v3v1LineE,
                                &ov1ov2LineS, &ov1ov2LineE, &epsylon, &result);

        // is shortest distance between lines equal to 0?
        if (result < epsylon)
            return 1;

        miniGetShortestDistance(&v1v2LineS,   &v1v2LineE,
                                &ov2ov3LineS, &ov2ov3LineE, &epsylon, &result);

        // is shortest distance between lines equal to 0?
        if (result < epsylon)
            return 1;

        miniGetShortestDistance(&v2v3LineS,   &v2v3LineE,
                                &ov2ov3LineS, &ov2ov3LineE, &epsylon, &result);

        // is shortest distance between lines equal to 0?
        if (result < epsylon)
            return 1;

        miniGetShortestDistance(&v3v1LineS,   &v3v1LineE,
                                &ov2ov3LineS, &ov2ov3LineE, &epsylon, &result);

        // is shortest distance between lines equal to 0?
        if (result < epsylon)
            return 1;

        miniGetShortestDistance(&v1v2LineS,   &v1v2LineE,
                                &ov3ov1LineS, &ov3ov1LineE, &epsylon, &result);

        // is shortest distance between lines equal to 0?
        if (result < epsylon)
            return 1;

        miniGetShortestDistance(&v2v3LineS,   &v2v3LineE,
                                &ov3ov1LineS, &ov3ov1LineE, &epsylon, &result);

        // is shortest distance between lines equal to 0?
        if (result < epsylon)
            return 1;

        miniGetShortestDistance(&v3v1LineS,   &v3v1LineE,
                                &ov3ov1LineS, &ov3ov1LineE, &epsylon, &result);

        // is shortest distance between lines equal to 0?
        if (result < epsylon)
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
    miniDot(&normal1, &normal2, &planesDot);

    // are plane parallels?
    if (planesDot == 1.0f || planesDot == -1.0f)
        // planes are parallels but not merged, no collision is possible
        return 0;

    // calculate distance from each first polygon vertex to second polygon plane
    miniDistanceToPlane(pP1V1, &plane2, &dist1v1);
    miniDistanceToPlane(pP1V2, &plane2, &dist1v2);
    miniDistanceToPlane(pP1V3, &plane2, &dist1v3);

    // prepare list containing first polygon intersection points
    p1ptsCount = 0;

    // is first polygon V1 to V2 line segment intersects second polygon plane?
    if ((dist1v1 >= 0.0f && dist1v2 < 0.0f) || (dist1v1 < 0.0f && dist1v2 >= 0.0f))
        // calculate intersection point and add to list on success
        if (miniLinePlaneIntersect(&plane2, pP1V1, pP1V2, &p))
        {
            miniCopy(&p, &p1pt1);
            ++p1ptsCount;
        }

    // is first polygon V2 to V3 line segment intersects second polygon plane?
    if ((dist1v2 >= 0.0f && dist1v3 < 0.0f) || (dist1v2 < 0.0f && dist1v3 >= 0.0f))
        // calculate intersection point and add to list on success
        if (miniLinePlaneIntersect(&plane2, pP1V2, pP1V3, &p))
        {
            miniCopy(&p, &p1pt2);
            ++p1ptsCount;
        }

    // is first polygon V3 to V1 line segment intersects second polygon plane?
    if ((dist1v3 >= 0.0f && dist1v1 < 0.0f) || (dist1v3 < 0.0f && dist1v1 >= 0.0f))
        // calculate intersection point and add to list on success
        if (miniLinePlaneIntersect(&plane2, pP1V3, pP1V1, &p))
        {
            miniCopy(&p, &p1pt3);
            ++p1ptsCount;
        }

    // were the first polygon 2 intersection point found?
    if (p1ptsCount != 2)
        return 0;

    // calculate distance from each second polygon vertex to first polygon plane
    miniDistanceToPlane(pP2V1, &plane1, &dist2v1);
    miniDistanceToPlane(pP2V2, &plane1, &dist2v2);
    miniDistanceToPlane(pP2V3, &plane1, &dist2v3);

    // prepare list containing second polygon intersection points
    p2ptsCount = 0;

    // is second polygon V1 to V2 line segment intersects first polygon plane?
    if ((dist2v1 >= 0.0f && dist2v2 < 0.0f) || (dist2v1 < 0.0f && dist2v2 >= 0.0f))
        // calculate intersection point and add to list on success
        if (miniLinePlaneIntersect(&plane1, pP2V1, pP2V2, &p))
        {
            miniCopy(&p, &p2pt1);
            ++p2ptsCount;
        }

    // is second polygon V2 to V3 line segment intersects first polygon plane?
    if ((dist2v2 >= 0.0f && dist2v3 < 0.0f) || (dist2v2 < 0.0f && dist2v3 >= 0.0f))
        // calculate intersection point and add to list on success
        if (miniLinePlaneIntersect(&plane1, pP2V2, pP2V3, &p))
        {
            miniCopy(&p, &p2pt2);
            ++p2ptsCount;
        }

    // is second polygon V3 to V1 line segment intersects first polygon plane?
    if ((dist2v3 >= 0.0f && dist2v1 < 0.0f) || (dist2v3 < 0.0f && dist2v1 >= 0.0f))
        // calculate intersection point and add to list on success
        if (miniLinePlaneIntersect(&plane1, pP2V3, pP2V1, &p))
        {
            miniCopy(&p, &p2pt3);
            ++p2ptsCount;
        }

    // were the second polygon 2 intersection point found?
    if (p2ptsCount != 2)
        return 0;

    // first and second polygon intersection points are on the same line, so
    // check if calculated first and second polygon segments intersect
    if (miniVectorIsBetween(&p1pt1, &p2pt1, &p2pt2, &epsylon))
        return 1;

    if (miniVectorIsBetween(&p1pt2, &p2pt1, &p2pt2, &epsylon))
        return 1;

    if (miniVectorIsBetween(&p2pt1, &p1pt1, &p1pt2, &epsylon))
        return 1;

    if (miniVectorIsBetween(&p2pt2, &p1pt1, &p1pt2, &epsylon))
        return 1;

    return 0;
}
//----------------------------------------------------------------------------
int miniRayPolygonIntersect(const MINI_Ray* pRay, const MINI_Polygon* pPolygon)
{
    MINI_Plane   polygonPlane;
    MINI_Vector3 pointOnPlane;

    // create a plane using the 3 vertices of the polygon
    miniPlaneFromPoints(&pPolygon->m_v[0], &pPolygon->m_v[1], &pPolygon->m_v[2], &polygonPlane);

    // calculate the intersection point
    if (!miniRayPlaneIntersect(&polygonPlane, &pRay->m_Pos, &pRay->m_Dir, &pointOnPlane))
        return 0;

    // check if calculated point is inside the polygon
    return miniInside(&pointOnPlane, &pPolygon->m_v[0], &pPolygon->m_v[1], &pPolygon->m_v[2]);
}
//----------------------------------------------------------------------------
int miniLinePolygonIntersect(const MINI_Vector3* pP1,
                             const MINI_Vector3* pP2,
                             const MINI_Polygon* pPolygon)
{
    MINI_Plane   polygonPlane;
    MINI_Vector3 pointOnPlane;

    // create a plane using the 3 vertices of the polygon
    miniPlaneFromPoints(&pPolygon->m_v[0], &pPolygon->m_v[1], &pPolygon->m_v[2], &polygonPlane);

    // calculate the intersection point
    if (!miniLinePlaneIntersect(&polygonPlane, pP1, pP2, &pointOnPlane))
        return 0;

    // check if calculated point is inside the polygon
    return miniInside(&pointOnPlane, &pPolygon->m_v[0], &pPolygon->m_v[1], &pPolygon->m_v[2]);
}
//----------------------------------------------------------------------------
int miniRayBoxIntersect(const MINI_Ray* pRay, const MINI_Box* pBox)
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
    if (pRay->m_InvDir.m_X != inf)
        tx1 = ((pBox->m_Min.m_X - pRay->m_Pos.m_X) * pRay->m_InvDir.m_X);
    else
    if ((pBox->m_Min.m_X - pRay->m_Pos.m_X) < 0.0f)
        tx1 = -inf;
    else
        tx1 = inf;

    // calculate farthest point where ray intersects box on x coordinate
    if (pRay->m_InvDir.m_X != inf)
        tx2 = ((pBox->m_Max.m_X - pRay->m_Pos.m_X) * pRay->m_InvDir.m_X);
    else
    if ((pBox->m_Max.m_X - pRay->m_Pos.m_X) < 0.0f)
        tx2 = -inf;
    else
        tx2 = inf;

    // calculate nearest point where ray intersects box on y coordinate
    if (pRay->m_InvDir.m_Y != inf)
        ty1 = ((pBox->m_Min.m_Y - pRay->m_Pos.m_Y) * pRay->m_InvDir.m_Y);
    else
    if ((pBox->m_Min.m_Y - pRay->m_Pos.m_Y) < 0.0f)
        ty1 = -inf;
    else
        ty1 = inf;

    // calculate farthest point where ray intersects box on y coordinate
    if (pRay->m_InvDir.m_Y != inf)
        ty2 = ((pBox->m_Max.m_Y - pRay->m_Pos.m_Y) * pRay->m_InvDir.m_Y);
    else
    if ((pBox->m_Max.m_Y - pRay->m_Pos.m_Y) < 0.0f)
        ty2 = -inf;
    else
        ty2 = inf;

    // calculate nearest point where ray intersects box on z coordinate
    if (pRay->m_InvDir.m_Z != inf)
        tz1 = ((pBox->m_Min.m_Z - pRay->m_Pos.m_Z) * pRay->m_InvDir.m_Z);
    else
    if ((pBox->m_Min.m_Z - pRay->m_Pos.m_Z) < 0.0f)
        tz1 = -inf;
    else
        tz1 = inf;

    // calculate farthest point where ray intersects box on z coordinate
    if (pRay->m_InvDir.m_Z != inf)
        tz2 = ((pBox->m_Max.m_Z - pRay->m_Pos.m_Z) * pRay->m_InvDir.m_Z);
    else
    if ((pBox->m_Max.m_Z - pRay->m_Pos.m_Z) < 0.0f)
        tz2 = -inf;
    else
        tz2 = inf;

    // calculate near/far intersection on each axis
    miniMin(&tx1, &tx2, &txn);
    miniMax(&tx1, &tx2, &txf);
    miniMin(&ty1, &ty2, &tyn);
    miniMax(&ty1, &ty2, &tyf);
    miniMin(&tz1, &tz2, &tzn);
    miniMax(&tz1, &tz2, &tzf);

    // calculate final near/far intersection point
    miniMax(&tyn, &tzn,   &tnear);
    miniMax(&txn, &tnear, &tnear);
    miniMin(&tyf, &tzf,  &tfar);
    miniMin(&txf, &tfar, &tfar);

    // check if ray intersects box
    return (tfar >= tnear);
}
//----------------------------------------------------------------------------
int miniSpherePolygonIntersectAndSlide(const MINI_Sphere*  pSphere,
                                       const MINI_Polygon* pPolygon,
                                             MINI_Plane*   pSlidingPlane)
{
    float        testPoint1;
    float        testPoint2;
    MINI_Vector3 sphereNormal;
    MINI_Vector3 pointOnSphere;
    MINI_Vector3 pointOnPlane;
    MINI_Vector3 pointOnTriangle;
    MINI_Plane   polygonPlane;

    // create a plane using the 3 vertices of the polygon
    miniPlaneFromPoints(&pPolygon->m_v[0], &pPolygon->m_v[1], &pPolygon->m_v[2], &polygonPlane);

    // calculate the distance between the center of the sphere and the plane
    miniDistanceToPlane(&pSphere->m_Pos, &polygonPlane, &testPoint1);

    // calculate the normal of the distance sphere-plane using the positive or
    // negative value of the calculated distance between plane and position
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

    // calculate the point who the segment from center of sphere in the
    // direction of the plane will cross the border of the sphere
    pointOnSphere.m_X = pSphere->m_Pos.m_X + (pSphere->m_Radius * sphereNormal.m_X);
    pointOnSphere.m_Y = pSphere->m_Pos.m_Y + (pSphere->m_Radius * sphereNormal.m_Y),
    pointOnSphere.m_Z = pSphere->m_Pos.m_Z + (pSphere->m_Radius * sphereNormal.m_Z);

    // calculate the distance between the border of the sphere and the plane
    miniDistanceToPlane(&pointOnSphere, &polygonPlane, &testPoint2);

    // if the test points are on each side of the plane, then the sphere cross
    // the plane. We assume that the segment from the center of the sphere to
    // the direction of the plane can never be co-planar
    if ((testPoint1 <= 0.0f && testPoint2 >= 0.0) ||
        (testPoint2 <= 0.0f && testPoint1 >= 0.0))
    {
        // calculate who the segment cross the plane
        if (testPoint1 == 0.0)
            // if testPoint1 is equal to 0, the center of the sphere cross the
            // plane
            pointOnPlane = pSphere->m_Pos;
        else
        if (testPoint2 == 0.0)
            // if testPoint2 is equal to 0, the border of the sphere cross the
            // plane
            pointOnPlane = pointOnSphere;
        else
            // calculate the intersection point
            miniLinePlaneIntersect(&polygonPlane,
                                   &pSphere->m_Pos,
                                   &pointOnSphere,
                                   &pointOnPlane);

        // check if calculated point is inside the polygon
        if (miniInside(&pointOnPlane, &pPolygon->m_v[0], &pPolygon->m_v[1], &pPolygon->m_v[2]))
        {
            // if yes, the sphere collide the polygon. In this case, we copy
            // the plane and we returns true
            *pSlidingPlane = polygonPlane;
            return 1;
        }
        else
        {
            // otherwise check if the sphere collide the border of the polygon.
            // First we calculate the test point on the border of the polygon
            miniClosestPointOnTriangle(&pointOnPlane,
                                       &pPolygon->m_v[0],
                                       &pPolygon->m_v[1],
                                       &pPolygon->m_v[2],
                                       &pointOnTriangle);

            // check if calculated point is inside the sphere
            if (miniPointInSphere(&pointOnTriangle, pSphere))
            {
                // if yes, the sphere collide the polygon. In this case, we
                // copy the plane and we returns true
                *pSlidingPlane = polygonPlane;
                return 1;
            }
        }
    }

    // no collision was found
    return 0;
}
//----------------------------------------------------------------------------
// Collision reaction
//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
// Polygons extraction
//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
void miniReleasePolygons(MINI_Polygon* pPolygons)
{
    if (pPolygons)
        free(pPolygons);
}
//----------------------------------------------------------------------------
// Aligned-Axis Bounding Box tree
//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
