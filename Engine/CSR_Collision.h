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

/**
* Indexed polygon, keep the index of each vertex composing the polygon data inside a vertex buffer
*/
typedef struct
{
    size_t m_VertexIndex[3];
} CSR_IndexedPolygon;

/**
* Indexed polygon array
*/
typedef struct
{
    CSR_IndexedPolygon* m_pBuffer;
    size_t              m_Count;
} CSR_IndexedPolygons;

// Aligned-axis bounding box tree node prototype (required to use itself inside the structure)
typedef struct CSR_AABBNode CSR_AABBNode;

/**
* Aligned-axis bounding box tree node
*/
struct CSR_AABBNode
{
    CSR_AABBNode*        m_pParent;
    CSR_AABBNode*        m_pLeft;
    CSR_AABBNode*        m_pRight;
    CSR_Box*             m_pBox;
    CSR_IndexedPolygons* m_pPolygons;
    //REM? float*               m_pVertexBuffer;
};

#ifdef __cplusplus
    extern "C"
    {
#endif

        //-------------------------------------------------------------------
        // Indexed polygons
        //-------------------------------------------------------------------

        /**
        * Converts an indexed polygon to a polygon
        *@param pVB - vertex buffer containing the vertices referred by the indexed polygon
        *@param pP - indexed polygon
        *@param[out] pR - polygon
        */
        void csrIndexedToPolygon(const float* pVB, const CSR_IndexedPolygon* pP, CSR_Polygon3* pR);

        /**
        * Adds a new indexed polygon, generated from his 3 indices, inside an indexed polygon buffer
        *@param v1 - polygon first vertex index
        *@param v2 - polygon second vertex index
        *@param v3 - polygon third vertex index
        *@param[out] pPolygons - indexed polygon buffer in which the new polygon should be added
        *@note The buffer should be deleted by calling csrIndexedBufferRelease() when useless
        */
        void csrIndexedBufferAdd(size_t v1, size_t v2, size_t v3, CSR_IndexedPolygons* pPolygons);

        /**
        * Releases polygons previously created by csrPolygonsFromVB() or csrPolygonFromVB()
        *@param pPolygons - polygon array to release
        */
        void csrIndexedBufferRelease(CSR_IndexedPolygons* pPolygons);

        //-------------------------------------------------------------------
        // Aligned-Axis Bounding Box tree
        //-------------------------------------------------------------------
//
//        /**
//        * Extends a bounding box to include a polygon
//        *@param pVB - vertex buffer containing the indexed polygon vertices
//        *@param pP - polygon to add
//        *@param pB - bounding box in which polygon should be included
//        *@param[in, out] pEmpty - if 1, box is empty and still no contains any polygon
//        */
//        void csrBoxExtendToPolygon(const float*              pVB,
//                                   const CSR_IndexedPolygon* pP,
//                                         CSR_Box*            pB,
//                                         int*                pEmpty);
//
//        /**
//        * Extracts polygons from vertex buffer
//        *@param pVB - source vertex buffer
//        *@param length - vertex buffer length
//        *@param type - polygons type as arranged in vertex buffer, where:
//        *              0 = triangles
//        *              1 = triangle strip
//        *              2 = triangle fan
//        *              3 = quads
//        *              4 = quad strip
//        *@param[out] pPolygons - polygon array that contains generated polygons
//        *@param[out] pPolygonsCount - polygons count contained in array
//        *@return 1 on success, otherwise 0
//        *@note Generated polygons should be deleted by calling miniReleasePolygons()
//        *      when useless
//        */
//        int csrPolygonsFromVB(const float*        pVB,
//                                    unsigned      length,
//                                    unsigned      type,
//                                    unsigned      stride,
//                                    CSR_Polygon** pPolygons,
//                                    unsigned*     pPolygonsCount);
//
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
    #include "MiniCollision.c"
#endif

#endif
