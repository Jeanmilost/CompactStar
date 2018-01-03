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

// compactStart engine
#include "CSR_Common.h"
#include "CSR_Geometry.h"
#include "CSR_Models.h"

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
    CSR_AABBNode*      m_pParent;
    CSR_AABBNode*      m_pLeft;
    CSR_AABBNode*      m_pRight;
    CSR_Box*           m_pBox;
    CSR_PolygonBuffer* m_pPolygons;
};

#ifdef __cplusplus
    extern "C"
    {
#endif
        //-------------------------------------------------------------------
        // Aligned-Axis Bounding Box tree functions
        //-------------------------------------------------------------------

        /**
        * Populates an AABB tree from a polygon buffer
        *@param pPolygons - buffer containing the polygons from which the tree will be populated
        *@param[in, out] pNode - root or parent node to create from, populated node on function ends
        *@return 1 on success, otherwise 0
        */
        int csrAABBTreePopulate(const CSR_PolygonBuffer* pPolygons, CSR_AABBNode* pNode);

//        /**
//        * Resolves AABB tree
//        *@param pRay - ray against which tree boxes will be tested
//        *@param pNode - root or parent node to resolve
//        *@param[out] pPolygons - polygons belonging to boxes hit by ray
//        *@param[out] polygonsCount - polygon array count
//        *@return 1 on success, otherwise 0
//        */
//        int csrAABBTreeResolve(MINI_Ray*      pRay,
//                               MINI_AABBNode* pNode,
//                               MINI_Polygon** pPolygons,
//                               unsigned*      pPolygonsCount);
//
//        /**
//        * Releases node content
//        *@param pNode - node for which content should be released
//        *@note Only the node content is released, the node itself is not released
//        */
//        void csrAABBTreeNodeRelease(MINI_AABBNode* pNode);
//
//        /**
//        * Releases tree content
//        *@param pNode - root node from which content should be released
//        */
//        void csrAABBTreeRelease(MINI_AABBNode* pNode);

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
