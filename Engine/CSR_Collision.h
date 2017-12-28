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

#ifndef CSR_CollisionH
#define CSR_CollisionH

// std
#include <stddef.h>

// compactStar engine
#include "CSR_Geometry.h"
#include "CSR_Vertex.h"

//---------------------------------------------------------------------------
// Structures
//---------------------------------------------------------------------------

// Aligned-axis bounding box tree node prototype (required to use itself inside the structure)
typedef struct CSR_AABBNode CSR_AABBNode;

/**
* Aligned-axis bounding box tree node
*/
struct CSR_AABBNode
{
    CSR_AABBNode*            m_pParent;
    CSR_AABBNode*            m_pLeft;
    CSR_AABBNode*            m_pRight;
    CSR_Box*                 m_pBox;
    CSR_IndexedPolygonTable* m_pPolygons;
};

#ifdef __cplusplus
    extern "C"
    {
#endif
        //-------------------------------------------------------------------
        // Aligned-Axis Bounding Box functions
        //-------------------------------------------------------------------

        /**
        * Extends a bounding box to include a polygon
        *@param pVB - vertex buffer containing the indexed polygon vertices
        *@param pP - polygon to add
        *@param pB - bounding box in which polygon should be included
        *@param[in, out] pEmpty - if 1, box is empty and still no contains any polygon
        */
        void csrBoxExtendToPolygon(const float*              pVB,
                                   const CSR_IndexedPolygon* pP,
                                         CSR_Box*            pB,
                                         int*                pEmpty);

        //-------------------------------------------------------------------
        // Aligned-Axis Bounding Box tree
        //-------------------------------------------------------------------

//        /**
//        * Cuts box on the longest axis
//        *@param pBox - box to cut
//        *@param[out] pLeftBox - resulting left box
//        *@param[out] pRightBox - resulting right box
//        */
//        void csrBoxCut(const CSR_Box* pBox, CSR_Box* pLeftBox, CSR_Box* pRightBox);
//
//        /**
//        * Populates AABB tree
//        *@param pNode - root or parent node to create from
//        *@param pPolygons - source polygon array
//        *@param polygonsCount - polygon array count
//        *@return 1 on success, otherwise 0
//        */
//        int csrAABBTreePopulate(      CSR_AABBNode* pNode,
//                                const CSR_Polygon*  pPolygons,
//                                      size_t        polygonsCount);
//
//        /**
//        * Resolves AABB tree
//        *@param pRay - ray against which tree boxes will be tested
//        *@param pNode - root or parent node to resolve
//        *@param[out] pPolygons - polygons belonging to boxes hit by ray
//        *@param[out] polygonsCount - polygon array count
//        *@return 1 on success, otherwise 0
//        */
//        int csrAABBTreeResolve(CSR_Ray*      pRay,
//                               CSR_AABBNode* pNode,
//                               CSR_Polygon** pPolygons,
//                               unsigned*     pPolygonsCount);
//
//        /**
//        * Releases node content
//        *@param pNode - node for which content should be released
//        *@note Only the node content is released, the node itself is not released
//        */
//        void csrAABBTreeReleaseNode(CSR_AABBNode* pNode);
//
//        /**
//        * Releases tree content
//        *@param pNode - root node from which content should be released
//        */
//        void csrAABBTreeRelease(CSR_AABBNode* pNode);
//
//        //-------------------------------------------------------------------
//        // Collision reaction
//        //-------------------------------------------------------------------
//
//        /**
//        * Gets the next position while sliding on the plane
//        *@param pSlidingPlane - sliding plane
//        *@param pPosition - current position
//        *@param pRadius - radius around the current position
//        *@param pR - resulting position
//        */
//        void csrCollisionSlidingPos(const CSR_Plane*   pSlidingPlane,
//                                    const CSR_Vector3* pPosition,
//                                          float        radius,
//                                          CSR_Vector3* pR);

#ifdef __cplusplus
    }
#endif

//---------------------------------------------------------------------------
// Compiler
//---------------------------------------------------------------------------

// needed in mobile c compiler to link the .h file with the .c
#if defined(_OS_IOS_) || defined(_OS_ANDROID_) || defined(_OS_WINDOWS_)
    #include "CSR_Collision.c"
#endif

#endif
