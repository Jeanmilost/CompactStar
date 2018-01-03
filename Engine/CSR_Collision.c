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

//---------------------------------------------------------------------------
// Aligned-Axis Bounding Box tree functions
//---------------------------------------------------------------------------
int csrAABBTreePopulate(const CSR_PolygonBuffer* pPolygons, CSR_AABBNode* pNode)
{
    size_t            i;
    size_t            j;
    size_t            index;
    CSR_Box           leftBox;
    CSR_Box           rightBox;
    CSR_Polygon3      polygon;
    CSR_PolygonIndex* pLeftPolygons      = 0;
    CSR_PolygonIndex* pRightPolygons     = 0;
    unsigned          leftPolygonsCount  = 0;
    unsigned          rightPolygonsCount = 0;
    int               boxEmpty           = 1;
    int               canResolveLeft     = 0;
    int               canResolveRight    = 0;
    int               result             = 0;
    float             tolerance          = M_CSR_Epsilon;

    // initialize node content
    pNode->m_pParent   = 0;
    pNode->m_pLeft     = 0;
    pNode->m_pRight    = 0;
    pNode->m_pBox      = 0;
    pNode->m_pPolygons = 0;

    // create a collision box
    pNode->m_pBox = (CSR_Box*)malloc(sizeof(CSR_Box));

    // iterate through polygons to divide
    for (i = 0; i < pPolygons->m_Count; ++i)
    {
        // extract the polygon
        for (j = 0; j < 3; ++j)
        {
            index                   = pPolygons->m_pPolygons[i].m_pIndex[j];
            polygon.m_Vertex[j].m_X = pPolygons->m_pPolygons[i].m_pVB->m_pData[index];
            polygon.m_Vertex[j].m_Y = pPolygons->m_pPolygons[i].m_pVB->m_pData[index + 1];
            polygon.m_Vertex[j].m_Z = pPolygons->m_pPolygons[i].m_pVB->m_pData[index + 2];
        }

        // calculate the bounding box around the remaining polygons
        csrBoxExtendToPolygon(&polygon, pNode->m_pBox, &boxEmpty);
    }

    // divide box in 2 sub-boxes
    csrBoxCut(pNode->m_pBox, &leftBox, &rightBox);

    // iterate again through polygons to divide
    for (i = 0; i < pPolygons->m_Count; ++i)
        for (j = 0; j < 3; ++j)
            // check if first polygon vertice belongs to left or right sub-box
            if (miniVectorIsBetween(&pPolygons[i].m_v[j], &leftBox.m_Min, &leftBox.m_Max, &tolerance))
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
            if (miniVectorIsBetween(&pPolygons[i].m_v[j], &rightBox.m_Min, &rightBox.m_Max, &tolerance))
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
