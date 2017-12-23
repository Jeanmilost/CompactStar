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

#ifndef MiniCollisionH
#define MiniCollisionH

// mini API
#include "MiniCommon.h"
#include "MiniGeometry.h"

//----------------------------------------------------------------------------
// Global defines
//----------------------------------------------------------------------------

#define M_MINI_Epsilon 1.0E-3 // epsilon value used for tolerance

//----------------------------------------------------------------------------
// Structures
//----------------------------------------------------------------------------

/**
* Polygon
*/
typedef struct
{
    MINI_Vector3 m_v[3];
} MINI_Polygon;

// Aligned-axis bounding box tree node prototype
typedef struct MINI_AABBNode MINI_AABBNode;

/**
* Aligned-axis bounding box tree node
*/
struct MINI_AABBNode
{
    MINI_AABBNode* m_pParent;
    MINI_AABBNode* m_pLeft;
    MINI_AABBNode* m_pRight;
    MINI_Box*      m_pBox;
    MINI_Polygon*  m_pPolygons;
    unsigned       m_PolygonsCount;
};

#ifdef __cplusplus
    extern "C"
    {
#endif

        //----------------------------------------------------------------------------
        // Box calculation
        //----------------------------------------------------------------------------

        /**
        * Adds a polygon inside an existing bounding box
        *@param pPolygon - polygon to add
        *@param pBox - bounding box in which polygon should be added
        *@param empty - if true, box is empty an still no contains any polygon
        */
        void miniAddPolygonToBoundingBox(const MINI_Polygon* pPolygon,
                                               MINI_Box*     pBox,
                                               int*          pEmpty);

        //----------------------------------------------------------------------------
        // Collision reaction
        //----------------------------------------------------------------------------

        /**
        * Gets the next position while sliding on the plane
        *@param pSlidingPlane - sliding plane
        *@param pPosition - current position
        *@param pRadius - radius around the current position
        *@param pR - resulting position
        */
        void miniGetSlidingPoint(const MINI_Plane*   pSlidingPlane,
                                 const MINI_Vector3* pPosition,
                                 const float*        pRadius,
                                       MINI_Vector3* pR);

        //----------------------------------------------------------------------------
        // Polygons extraction
        //----------------------------------------------------------------------------

        /**
        * Adds polygon to array
        *@param pVB - source vertex buffer
        *@param v1 - first polygon vertex index in vertex buffer
        *@param v2 - second polygon vertex index in vertex buffer
        *@param v3 - third polygon vertex index in vertex buffer
        *@param[out] pPolygons - polygon array that contains generated polygons
        *@param[out] pPolygonsCount - polygons count contained in array
        *@note Generated polygons should be deleted by calling miniReleasePolygons()
        *      when useless
        */
        void miniAddPolygon(const float*         pVB,
                                  unsigned       v1,
                                  unsigned       v2,
                                  unsigned       v3,
                                  MINI_Polygon** pPolygons,
                                  unsigned*      pPolygonsCount);

        /**
        * Gets polygons from vertex buffer
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
        int miniGetPolygonsFromVB(const float*         pVB,
                                        unsigned       length,
                                        unsigned       type,
                                        unsigned       stride,
                                        MINI_Polygon** pPolygons,
                                        unsigned*      pPolygonsCount);

        /**
        * Release polygons previously created by GetPolygonsFromVB() or AddPolygon()
        *@param pPolygons - polygon array to release
        */
        void miniReleasePolygons(MINI_Polygon* pPolygons);

        //----------------------------------------------------------------------------
        // Aligned-Axis Bounding Box tree
        //----------------------------------------------------------------------------

        /**
        * Cuts box on the longest axis
        *@param pBox - box to cut
        *@param[out] pLeftBox - resulting left box
        *@param[out] pRightBox - resulting right box
        */
        void miniCutBox(const MINI_Box* pBox, MINI_Box* pLeftBox, MINI_Box* pRightBox);

        /**
        * Populates AABB tree
        *@param pNode - root or parent node to create from
        *@param pPolygons - source polygon array
        *@param polygonsCount - polygon array count
        *@return 1 on success, otherwise 0
        */
        int miniPopulateTree(      MINI_AABBNode* pNode,
                             const MINI_Polygon*  pPolygons,
                                   unsigned       polygonsCount);

        /**
        * Resolves AABB tree
        *@param pRay - ray against which tree boxes will be tested
        *@param pNode - root or parent node to resolve
        *@param[out] pPolygons - polygons belonging to boxes hit by ray
        *@param[out] polygonsCount - polygon array count
        *@return 1 on success, otherwise 0
        */
        int miniResolveTree(MINI_Ray*      pRay,
                            MINI_AABBNode* pNode,
                            MINI_Polygon** pPolygons,
                            unsigned*      pPolygonsCount);

        /**
        * Releases node content
        *@param pNode - node for which content should be released
        *@note Only the node content is released, the node itself is not released
        */
        void miniReleaseNode(MINI_AABBNode* pNode);

        /**
        * Releases tree content
        *@param pNode - root node from which content should be released
        */
        void miniReleaseTree(MINI_AABBNode* pNode);

#ifdef __cplusplus
    }
#endif

//----------------------------------------------------------------------------
// Compiler
//----------------------------------------------------------------------------

// needed in mobile c compiler to link the .h file with the .c
#if defined(_OS_IOS_) || defined(_OS_ANDROID_) || defined(_OS_WINDOWS_)
    #include "MiniCollision.c"
#endif

#endif
