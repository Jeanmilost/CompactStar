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
int csrAABBTreeFromPolygons(const CSR_PolygonBuffer* pPolygons, CSR_AABBNode* pNode)
{
    size_t             i;
    size_t             j;
    CSR_Box            leftBox;
    CSR_Box            rightBox;
    CSR_Polygon3       polygon;
    CSR_PolygonIndex*  pNewPolygons    = 0;
    CSR_PolygonBuffer* pLeftPolygons   = 0;
    CSR_PolygonBuffer* pRightPolygons  = 0;
    int                boxEmpty        = 1;
    int                canResolveLeft  = 0;
    int                canResolveRight = 0;
    int                result          = 0;
    float              tolerance       = M_CSR_Epsilon;

    // no node?
    if (!pNode)
        return 0;

    // initialize node content
    pNode->m_pParent        = 0;
    pNode->m_pLeft          = 0;
    pNode->m_pRight         = 0;
    pNode->m_pBox           = (CSR_Box*)malloc(sizeof(CSR_Box));
    pNode->m_pPolygonBuffer = csrPolygonBufferCreate();

    // succeeded?
    if (!pNode->m_pBox || !pNode->m_pPolygonBuffer)
    {
        csrAABBTreeNodeRelease(pNode);
        return 0;
    }

    // create the polygon buffers that will contain the divided polygons
    pLeftPolygons  = csrPolygonBufferCreate();
    pRightPolygons = csrPolygonBufferCreate();

    // succeeded?
    if (!pLeftPolygons || !pRightPolygons)
    {
        csrPolygonBufferRelease(pLeftPolygons);
        csrPolygonBufferRelease(pRightPolygons);
        csrAABBTreeNodeRelease(pNode);
        return 0;
    }

    // iterate through polygons to divide
    for (i = 0; i < pPolygons->m_Count; ++i)
    {
        // using his index, extract the polygon from his vertex buffer
        csrPolygonFromIndex(&pPolygons->m_pPolygons[i], &polygon);

        // extend the bounding box to include the polygon
        csrBoxExtendToPolygon(&polygon, pNode->m_pBox, &boxEmpty);
    }

    // divide the bounding box in 2 sub-boxes
    csrBoxCut(pNode->m_pBox, &leftBox, &rightBox);

    // iterate again through polygons to divide
    for (i = 0; i < pPolygons->m_Count; ++i)
    {
        // using his index, extract the polygon from his vertex buffer
        csrPolygonFromIndex(&pPolygons->m_pPolygons[i], &polygon);

        // iterate through the polygon vertices
        for (j = 0; j < 3; ++j)
            // check at which sub-box the polygon vertex belongs (and thus to which buffer it should
            // be added)
            if (csrVec3BetweenRange(&polygon.m_Vertex[j], &leftBox.m_Min, &leftBox.m_Max, tolerance))
            {
                // left polygon buffer already contains polygons?
                if (!pLeftPolygons->m_Count)
                {
                    // no, create memory for the first polygon in the polygon buffer
                    pLeftPolygons->m_pPolygons = (CSR_PolygonIndex*)malloc(sizeof(CSR_PolygonIndex));
                    pLeftPolygons->m_Count     = 1;
                }
                else
                {
                    // yes, increase the memory to contain a new polygon in the polygon buffer
                    ++pLeftPolygons->m_Count;
                    pNewPolygons =
                            (CSR_PolygonIndex*)realloc(pLeftPolygons->m_pPolygons,
                                                       pLeftPolygons->m_Count * sizeof(CSR_PolygonIndex));

                    // certify that the new memory block was well allocated
                    if (!pNewPolygons)
                        free(pLeftPolygons->m_pPolygons);

                    pLeftPolygons->m_pPolygons = pNewPolygons;
                }

                // failed to create memory for the new polygon in the polygon buffer?
                if (!pLeftPolygons->m_pPolygons)
                {
                    // serious error, probably not enough memory, clear all and break the process
                    csrPolygonBufferRelease(pLeftPolygons);
                    csrPolygonBufferRelease(pRightPolygons);
                    csrAABBTreeNodeRelease(pNode);
                    return 0;
                }

                // copy the polygon index content inside his buffer
                pLeftPolygons->m_pPolygons[pLeftPolygons->m_Count - 1] = pPolygons->m_pPolygons[i];
                break;
            }
            else
            if (csrVec3BetweenRange(&polygon.m_Vertex[j], &rightBox.m_Min, &rightBox.m_Max, tolerance))
            {
                // right polygon buffer already contains polygons?
                if (!pRightPolygons->m_Count)
                {
                    // no, create memory for the first polygon in the polygon buffer
                    pRightPolygons->m_pPolygons = (CSR_PolygonIndex*)malloc(sizeof(CSR_PolygonIndex));
                    pRightPolygons->m_Count     = 1;
                }
                else
                {
                    // yes, increase the memory to contain a new polygon in the polygon buffer
                    ++pRightPolygons->m_Count;
                    pNewPolygons =
                            (CSR_PolygonIndex*)realloc(pRightPolygons->m_pPolygons,
                                                       pRightPolygons->m_Count * sizeof(CSR_PolygonIndex));

                    // certify that the new memory block was well allocated
                    if (!pNewPolygons)
                        free(pRightPolygons->m_pPolygons);

                    pRightPolygons->m_pPolygons = pNewPolygons;
                }

                // failed to create memory for the new polygon in the polygon buffer?
                if (!pRightPolygons->m_pPolygons)
                {
                    // serious error, probably not enough memory, clear all and break the process
                    csrPolygonBufferRelease(pLeftPolygons);
                    csrPolygonBufferRelease(pRightPolygons);
                    csrAABBTreeNodeRelease(pNode);
                    return 0;
                }

                // copy the polygon content inside his buffer
                pRightPolygons->m_pPolygons[pRightPolygons->m_Count - 1] = pPolygons->m_pPolygons[i];
                break;
            }
    }

    canResolveLeft  = (pLeftPolygons->m_Count  && pLeftPolygons->m_Count  < pPolygons->m_Count);
    canResolveRight = (pRightPolygons->m_Count && pRightPolygons->m_Count < pPolygons->m_Count);

    // leaf reached?
    if (!canResolveLeft && !canResolveRight)
    {
        // copy the left polygons in the node polygon buffer
        for (i = 0; i < pLeftPolygons->m_Count; ++i)
        {
            // node polygon buffer already contains polygons?
            if (!pNode->m_pPolygonBuffer->m_Count)
            {
                // no, create memory for the first polygon in the polygon buffer
                pNode->m_pPolygonBuffer->m_pPolygons = (CSR_PolygonIndex*)malloc(sizeof(CSR_PolygonIndex));
                pNode->m_pPolygonBuffer->m_Count     = 1;
            }
            else
            {
                // yes, increase the memory to contain a new polygon in the polygon buffer
                ++pNode->m_pPolygonBuffer->m_Count;
                pNewPolygons =
                        (CSR_PolygonIndex*)realloc(pNode->m_pPolygonBuffer,
                                                   pNode->m_pPolygonBuffer->m_Count *
                                                           sizeof(CSR_PolygonIndex));

                // certify that the new memory block was well allocated
                if (!pNewPolygons)
                    free(pNode->m_pPolygonBuffer);

                pNode->m_pPolygonBuffer->m_pPolygons = pNewPolygons;
            }

            // failed to create memory for the new polygon in the polygon buffer?
            if (!pNode->m_pPolygonBuffer->m_pPolygons)
            {
                // serious error, probably not enough memory, stop all the process
                csrPolygonBufferRelease(pLeftPolygons);
                csrPolygonBufferRelease(pRightPolygons);
                csrAABBTreeNodeRelease(pNode);
                return 0;
            }

            // copy the polygon content inside his buffer
            pNode->m_pPolygonBuffer->m_pPolygons[pNode->m_pPolygonBuffer->m_Count - 1] =
                    pLeftPolygons->m_pPolygons[i];
        }

        // copy right polygons in polygon list
        for (i = 0; i < pRightPolygons->m_Count; ++i)
        {
            // node polygon buffer already contains polygons?
            if (!pNode->m_pPolygonBuffer->m_Count)
            {
                // no, create memory for the first polygon in the polygon buffer
                pNode->m_pPolygonBuffer->m_pPolygons = (CSR_PolygonIndex*)malloc(sizeof(CSR_PolygonIndex));
                pNode->m_pPolygonBuffer->m_Count     = 1;
            }
            else
            {
                // yes, increase the memory to contain a new polygon in the polygon buffer
                ++pNode->m_pPolygonBuffer->m_Count;
                pNewPolygons =
                        (CSR_PolygonIndex*)realloc(pNode->m_pPolygonBuffer,
                                                   pNode->m_pPolygonBuffer->m_Count *
                                                           sizeof(CSR_PolygonIndex));

                // certify that the new memory block was well allocated
                if (!pNewPolygons)
                    free(pNode->m_pPolygonBuffer);

                pNode->m_pPolygonBuffer->m_pPolygons = pNewPolygons;
            }

            // failed to create memory for the new polygon in the polygon buffer?
            if (!pNode->m_pPolygonBuffer->m_pPolygons)
            {
                // serious error, probably not enough memory, stop all the process
                csrPolygonBufferRelease(pLeftPolygons);
                csrPolygonBufferRelease(pRightPolygons);
                csrAABBTreeNodeRelease(pNode);
                return 0;
            }

            // copy the polygon content inside his buffer
            pNode->m_pPolygonBuffer->m_pPolygons[pNode->m_pPolygonBuffer->m_Count - 1] =
                    pRightPolygons->m_pPolygons[i];
        }

        // delete left and right polygon buffers, as they will no longer be used
        csrPolygonBufferRelease(pLeftPolygons);
        csrPolygonBufferRelease(pRightPolygons);

        return 1;
    }

    // do create left node?
    if (canResolveLeft)
    {
        // create and populate left node
        pNode->m_pLeft            = (CSR_AABBNode*)malloc(sizeof(CSR_AABBNode));
        pNode->m_pLeft->m_pParent = pNode;

        result |= csrAABBTreeFromPolygons(pLeftPolygons, pNode->m_pLeft);

        // delete left polygon buffer, as it will no longer be used
        csrPolygonBufferRelease(pLeftPolygons);
    }

    // do create right node?
    if (canResolveRight)
    {
        // create and populate right node
        pNode->m_pRight            = (CSR_AABBNode*)malloc(sizeof(CSR_AABBNode));
        pNode->m_pRight->m_pParent = pNode;

        result |= csrAABBTreeFromPolygons(pRightPolygons, pNode->m_pRight);

        // delete right polygon buffer, as it will no longer be used
        csrPolygonBufferRelease(pRightPolygons);
    }

    return result;
}
//---------------------------------------------------------------------------
CSR_AABBNode* csrAABBTreeFromMesh(const CSR_Mesh* pMesh)
{
    CSR_AABBNode* pRoot;

    // get polygon buffer from mesh
    CSR_PolygonBuffer* pPolygonBuffer = csrPolygonBufferFromMesh(pMesh);

    if (!pPolygonBuffer)
        return 0;

    // create the root node
    pRoot = (CSR_AABBNode*)malloc(sizeof(CSR_AABBNode));

    // succeeded?
    if (!pRoot)
    {
        csrPolygonBufferRelease(pPolygonBuffer);
        return 0;
    }

    // populate the tree
    if (!csrAABBTreeFromPolygons(pPolygonBuffer, pRoot))
    {
        csrAABBTreeRelease(pRoot);
        csrPolygonBufferRelease(pPolygonBuffer);
        return 0;
    }

    // release the polygon buffer
    csrPolygonBufferRelease(pPolygonBuffer);

    return pRoot;
}
//---------------------------------------------------------------------------
void csrAABBTreeNodeRelease(CSR_AABBNode* pNode)
{
    // release the bounding box
    if (pNode->m_pBox)
    {
        free(pNode->m_pBox);
        pNode->m_pBox = 0;
    }

    // release the polygon buffer
    if (pNode->m_pPolygonBuffer)
    {
        // release the polygon buffer content
        if (pNode->m_pPolygonBuffer->m_pPolygons)
            free(pNode->m_pPolygonBuffer->m_pPolygons);

        free(pNode->m_pPolygonBuffer);
        pNode->m_pPolygonBuffer = 0;
    }
}
//---------------------------------------------------------------------------
void csrAABBTreeRelease(CSR_AABBNode* pNode)
{
    if (!pNode)
        return;

    // release all children on left side
    if (pNode->m_pLeft)
        csrAABBTreeRelease(pNode->m_pLeft);

    // release all children on right side
    if (pNode->m_pRight)
        csrAABBTreeRelease(pNode->m_pRight);

    // delete node content
    csrAABBTreeNodeRelease(pNode);

    // delete node
    free(pNode);
}
//---------------------------------------------------------------------------
