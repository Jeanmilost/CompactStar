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

#ifndef MiniCollisionH
#define MiniCollisionH

// std
#include <stddef.h>

// mini API
//REM #include "CSR_Common.h"
#include "CSR_Geometry.h"

//---------------------------------------------------------------------------
// Structures
//---------------------------------------------------------------------------

// Aligned-axis bounding box tree node prototype
typedef struct CSR_AABBNode CSR_AABBNode;

/**
* Aligned-axis bounding box tree node
*/
struct CSR_AABBNode
{
    CSR_AABBNode* m_pParent;
    CSR_AABBNode* m_pLeft;
    CSR_AABBNode* m_pRight;
    CSR_Box*      m_pBox;
    CSR_Polygon*  m_pPolygons;
    size_t        m_PolygonsCount;
};

#ifdef __cplusplus
    extern "C"
    {
#endif

        //-------------------------------------------------------------------
        // Aligned-Axis Bounding Box tree
        //-------------------------------------------------------------------

        /**
        * Extends a bounding box to include a polygon
        *@param pP - polygon to add
        *@param pB - bounding box in which polygon should be included
        *@param empty - if true, box is empty an still no contains any polygon
        */
        void csrBoxExtendToPolygon(const CSR_Polygon* pP, CSR_Box* pB, int empty);

        /**
        * Extracts a polygon from a vertex buffer and adds it in an array
        *@param pVB - source vertex buffer
        *@param v1 - first polygon vertex index in vertex buffer
        *@param v2 - second polygon vertex index in vertex buffer
        *@param v3 - third polygon vertex index in vertex buffer
        *@param[out] pPolygons - polygon array that contains generated polygons
        *@param[out] pPolygonsCount - polygons count contained in array
        *@note Generated polygons should be deleted by calling miniReleasePolygons()
        *      when useless
        */
        void csrPolygonFromVB(const float*        pVB,
                                    unsigned      v1,
                                    unsigned      v2,
                                    unsigned      v3,
                                    CSR_Polygon** pPolygons,
                                    unsigned*     pPolygonsCount);

        /**
        * Extracts polygons from vertex buffer
        *@param pVB - source vertex buffer
        *@param length - vertex buffer length
        *@param type - polygons type as arranged in vertex buffer, where:
        *              0 = triangles
        *              1 = triangle strip
        *              2 = triangle fan
        *              3 = quads
        *              4 = quad strip
        *@param[out] pPolygons - polygon array that contains generated polygons
        *@param[out] pPolygonsCount - polygons count contained in array
        *@return 1 on success, otherwise 0
        *@note Generated polygons should be deleted by calling miniReleasePolygons()
        *      when useless
        */
        int csrPolygonsFromVB(const float*        pVB,
                                    unsigned      length,
                                    unsigned      type,
                                    unsigned      stride,
                                    CSR_Polygon** pPolygons,
                                    unsigned*     pPolygonsCount);

        /**
        * Releases polygons previously created by csrPolygonsFromVB() or csrPolygonFromVB()
        *@param pPolygons - polygon array to release
        */
        void csrPolygonsRelease(CSR_Polygon* pPolygons);

        /**
        * Cuts box on the longest axis
        *@param pBox - box to cut
        *@param[out] pLeftBox - resulting left box
        *@param[out] pRightBox - resulting right box
        */
        void csrBoxCut(const CSR_Box* pBox, CSR_Box* pLeftBox, CSR_Box* pRightBox);

        /**
        * Populates AABB tree
        *@param pNode - root or parent node to create from
        *@param pPolygons - source polygon array
        *@param polygonsCount - polygon array count
        *@return 1 on success, otherwise 0
        */
        int csrAABBTreePopulate(      CSR_AABBNode* pNode,
                                const CSR_Polygon*  pPolygons,
                                      size_t        polygonsCount);

        /**
        * Resolves AABB tree
        *@param pRay - ray against which tree boxes will be tested
        *@param pNode - root or parent node to resolve
        *@param[out] pPolygons - polygons belonging to boxes hit by ray
        *@param[out] polygonsCount - polygon array count
        *@return 1 on success, otherwise 0
        */
        int csrAABBTreeResolve(CSR_Ray*      pRay,
                               CSR_AABBNode* pNode,
                               CSR_Polygon** pPolygons,
                               unsigned*     pPolygonsCount);

        /**
        * Releases node content
        *@param pNode - node for which content should be released
        *@note Only the node content is released, the node itself is not released
        */
        void csrAABBTreeReleaseNode(CSR_AABBNode* pNode);

        /**
        * Releases tree content
        *@param pNode - root node from which content should be released
        */
        void csrAABBTreeRelease(CSR_AABBNode* pNode);

        //-------------------------------------------------------------------
        // Collision reaction
        //-------------------------------------------------------------------

        /**
        * Gets the next position while sliding on the plane
        *@param pSlidingPlane - sliding plane
        *@param pPosition - current position
        *@param pRadius - radius around the current position
        *@param pR - resulting position
        */
        void csrCollisionSlidingPos(const CSR_Plane*   pSlidingPlane,
                                    const CSR_Vector3* pPosition,
                                          float        radius,
                                          CSR_Vector3* pR);

#ifdef __cplusplus
    }
#endif

//---------------------------------------------------------------------------
// Compiler
//---------------------------------------------------------------------------

// needed in mobile c compiler to link the .h file with the .c
#if defined(_OS_IOS_) || defined(_OS_ANDROID_) || defined(_OS_WINDOWS_)
    #include "MiniCollision.c"
#endif

#endif
