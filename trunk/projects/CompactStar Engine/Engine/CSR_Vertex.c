/****************************************************************************
 * ==> CSR_Vertex ----------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides the basic vertex functions and types  *
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

#include "CSR_Vertex.h"

// std
#include <stdlib.h>

//---------------------------------------------------------------------------
// Vertex functions
//---------------------------------------------------------------------------
void csrVertexCalculateStride(CSR_VertexFormat* pVertexFormat)
{
    pVertexFormat->m_Stride = 3;

    // do use normals?
    if (pVertexFormat->m_UseNormals)
        pVertexFormat->m_Stride += 3;

    // do use textures?
    if (pVertexFormat->m_UseTextures)
        pVertexFormat->m_Stride += 2;

    // do use colors?
    if (pVertexFormat->m_UseColors)
        pVertexFormat->m_Stride += 4;
}
//---------------------------------------------------------------------------
void csrVertexAdd(CSR_Vector3*      pVertex,
                  CSR_Vector3*      pNormal,
                  CSR_Vector2*      pUV,
                  unsigned          color,
                  CSR_VertexBuffer* pVB)
{
    unsigned offset = pVB->m_Count;

    // allocate memory for new vertex
    if (!pVB->m_Count)
        pVB->m_pData = (float*)malloc(pVB->m_Format.m_Stride * sizeof(float));
    else
        pVB->m_pData = (float*)realloc(pVB->m_pData,
                                      (pVB->m_Count + pVB->m_Format.m_Stride) * sizeof(float));

    // copy vertex from source
    pVB->m_pData[offset]     = pVertex->m_X;
    pVB->m_pData[offset + 1] = pVertex->m_Y;
    pVB->m_pData[offset + 2] = pVertex->m_Z;

    offset += 3;

    // do include normals?
    if (pVB->m_Format.m_UseNormals)
    {
        // copy normal from source
        pVB->m_pData[offset]     = pNormal->m_X;
        pVB->m_pData[offset + 1] = pNormal->m_Y;
        pVB->m_pData[offset + 2] = pNormal->m_Z;

        offset += 3;
    }

    // do include texture coordinates?
    if (pVB->m_Format.m_UseTextures)
    {
        // copy texture coordinates from source
        pVB->m_pData[offset]     = pUV->m_X;
        pVB->m_pData[offset + 1] = pUV->m_Y;

        offset += 2;
    }

    // do include colors?
    if (pVB->m_Format.m_UseColors)
    {
        // set color data
        pVB->m_pData[offset]     = (float)((color >> 24) & 0xFF) / 255.0f;
        pVB->m_pData[offset + 1] = (float)((color >> 16) & 0xFF) / 255.0f;
        pVB->m_pData[offset + 2] = (float)((color >> 8)  & 0xFF) / 255.0f;
        pVB->m_pData[offset + 3] = (float) (color        & 0xFF) / 255.0f;
    }

    // update vertex count
    pVB->m_Count += pVB->m_Format.m_Stride;
}
//---------------------------------------------------------------------------
// Vertex buffer functions
//---------------------------------------------------------------------------
CSR_VertexBuffer* csrVertexBufferCreate(void)
{
    // create a new vertex buffer
    CSR_VertexBuffer* pBuffer = (CSR_VertexBuffer*)malloc(sizeof(CSR_VertexBuffer));

    // succeeded?
    if (!pBuffer)
        return 0;

    // initialize the vertex buffer content
    pBuffer->m_Format.m_Type        = CSR_E_Triangles;
    pBuffer->m_Format.m_UseNormals  = 0;
    pBuffer->m_Format.m_UseTextures = 0;
    pBuffer->m_Format.m_UseColors   = 0;
    pBuffer->m_Format.m_Stride      = 0;
    pBuffer->m_pData                = 0;
    pBuffer->m_Count                = 0;

    return pBuffer;
}
//---------------------------------------------------------------------------
void csrVertexBufferRelease(CSR_VertexBuffer* pVB)
{
    // no vertex buffer to free?
    if (!pVB)
        return;

    // free the vertex buffer content
    if (pVB->m_pData)
        free(pVB->m_pData);

    // free the vertex buffer
    free(pVB);
}
//---------------------------------------------------------------------------
// Mesh functions
//---------------------------------------------------------------------------
CSR_Mesh* csrMeshCreate(void)
{
    // create a new mesh
    CSR_Mesh* pMesh = (CSR_Mesh*)malloc(sizeof(CSR_Mesh));

    // succeeded?
    if (!pMesh)
        return 0;

    // initialize the buffer content
    pMesh->m_pVertices = 0;
    pMesh->m_Count     = 0;

    return pMesh;
}
//---------------------------------------------------------------------------
void csrMeshRelease(CSR_Mesh* pMesh)
{
    size_t i;

    // release the mesh content
    for (i = 0; i < pMesh->m_Count; ++i)
        if (pMesh->m_pVertices[i].m_pData)
            free(pMesh->m_pVertices[i].m_pData);

    // free the mesh
    free(pMesh);
}
//---------------------------------------------------------------------------
// Indexed polygon functions
//---------------------------------------------------------------------------
int csrIndexedToPolygon(const CSR_VertexBuffer* pVB, const CSR_IndexedPolygon* pIP, CSR_Polygon3* pP)
{
    unsigned i;

    // iterate through the polygon vertices
    for (i = 0; i < 3; ++i)
    {
        // is index out of bounds? (NOTE + 2 to include the 3 vertex values, which are always
        // consecutive inside the vertex buffer)
        if (pIP->m_Index[i] + 2 >= pVB->m_Count)
            return 0;

        // get polygon vertex from vertex buffer
        pP->m_Vertex[i].m_X = pVB->m_pData[pIP->m_Index[i]];
        pP->m_Vertex[i].m_Y = pVB->m_pData[pIP->m_Index[i] + 1];
        pP->m_Vertex[i].m_Z = pVB->m_pData[pIP->m_Index[i] + 2];
    }

    return 1;
}
//---------------------------------------------------------------------------
void csrIndexedAdd(size_t i1, size_t i2, size_t i3, CSR_IndexedPolygonTable* pIT)
{
    size_t index;

    // polygon array already contains polygons?
    if (!pIT->m_Count)
    {
        // no, add new first polygon in array
        pIT->m_pData = (CSR_IndexedPolygon*)malloc(sizeof(CSR_IndexedPolygon));
        pIT->m_Count = 1;
    }
    else
    {
        // yes, increase the polygons count and add new polygon inside array
        ++pIT->m_Count;
        pIT->m_pData = (CSR_IndexedPolygon*)realloc(pIT->m_pData,
                                                    pIT->m_Count * sizeof(CSR_IndexedPolygon));
    }

    // populate the newly created indexed polygon
    index                          = pIT->m_Count - 1;
    pIT->m_pData[index].m_Index[0] = i1;
    pIT->m_pData[index].m_Index[1] = i2;
    pIT->m_pData[index].m_Index[2] = i3;
}
//---------------------------------------------------------------------------
// Indexed polygon table functions
//---------------------------------------------------------------------------
CSR_IndexedPolygonTable* csrIndexedPolygonTableCreate(void)
{
    // create a new indexed polygon table
    CSR_IndexedPolygonTable* pTable = (CSR_IndexedPolygonTable*)malloc(sizeof(CSR_IndexedPolygonTable));

    // succeeded?
    if (!pTable)
        return 0;

    // initialize the buffer content
    pTable->m_pData = 0;
    pTable->m_Count = 0;

    return pTable;
}
//---------------------------------------------------------------------------
CSR_IndexedPolygonTable* csrIndexedPolygonTableFromVB(const CSR_VertexBuffer* pVB)
{
    unsigned                 i;
    unsigned                 index;
    CSR_IndexedPolygonTable* pTable;

    // create a new indexed polygon table
    pTable = csrIndexedPolygonTableCreate();

    // succeeded?
    if (!pTable)
        return 0;

    // no data to extract from?
    if (!pVB)
        return pTable;

    // search for vertex type
    switch (pVB->m_Format.m_Type)
    {
        case CSR_E_Triangles:
        {
            // calculate iteration step
            const unsigned step = (pVB->m_Format.m_Stride * 3);

            // iterate through source vertices
            for (i = 0; i < pVB->m_Count; i += step)
                // extract polygon from source buffer
                csrIndexedAdd(i,
                              i +  pVB->m_Format.m_Stride,
                              i + (pVB->m_Format.m_Stride * 2),
                              pTable);

            return pTable;
        }

        case CSR_E_TriangleStrip:
        {
            // calculate length to read in triangle strip buffer
            const unsigned stripLength = (pVB->m_Count - (pVB->m_Format.m_Stride * 2));
                           index       = 0;

            // iterate through source vertices
            for (i = 0; i < stripLength; i += pVB->m_Format.m_Stride)
            {
                // extract polygon from source buffer, revert odd polygons
                if (!index || !(index % 2))
                    csrIndexedAdd(i,
                                  i +  pVB->m_Format.m_Stride,
                                  i + (pVB->m_Format.m_Stride * 2),
                                  pTable);
                else
                    csrIndexedAdd(i +  pVB->m_Format.m_Stride,
                                  i,
                                  i + (pVB->m_Format.m_Stride * 2),
                                  pTable);

                ++index;
            }

            return pTable;
        }

        case CSR_E_TriangleFan:
        {
            // calculate length to read in triangle fan buffer
            const unsigned fanLength = (pVB->m_Count - pVB->m_Format.m_Stride);

            // iterate through source vertices
            for (i = pVB->m_Format.m_Stride; i < fanLength; i += pVB->m_Format.m_Stride)
                // extract polygon from source buffer
                csrIndexedAdd(0, i, i + pVB->m_Format.m_Stride, pTable);

            return pTable;
        }

        case CSR_E_Quads:
        {
            // calculate iteration step
            const unsigned step = (pVB->m_Format.m_Stride * 4);

            // iterate through source vertices
            for (i = 0; i < pVB->m_Count; i += step)
            {
                // calculate vertices position
                const unsigned v1 = i;
                const unsigned v2 = i +  pVB->m_Format.m_Stride;
                const unsigned v3 = i + (pVB->m_Format.m_Stride * 2);
                const unsigned v4 = i + (pVB->m_Format.m_Stride * 3);

                // extract polygons from source buffer
                csrIndexedAdd(v1, v2, v3, pTable);
                csrIndexedAdd(v3, v2, v4, pTable);
            }

            return pTable;
        }

        case CSR_E_QuadStrip:
        {
            // calculate iteration step
            const unsigned step = (pVB->m_Format.m_Stride * 2);

            // calculate length to read in triangle strip buffer
            const unsigned stripLength = (pVB->m_Count - (pVB->m_Format.m_Stride * 2));

            // iterate through source vertices
            for (i = 0; i < stripLength; i += step)
            {
                // calculate vertices position
                const unsigned v1 = i;
                const unsigned v2 = i +  pVB->m_Format.m_Stride;
                const unsigned v3 = i + (pVB->m_Format.m_Stride * 2);
                const unsigned v4 = i + (pVB->m_Format.m_Stride * 3);

                // extract polygons from source buffer
                csrIndexedAdd(v1, v2, v3, pTable);
                csrIndexedAdd(v3, v2, v4, pTable);
            }

            return pTable;
        }

        default:
            csrIndexedPolygonTableRelease(pTable);
            return 0;
    }
}
//---------------------------------------------------------------------------
void csrIndexedPolygonTableRelease(CSR_IndexedPolygonTable* pIT)
{
    // no indexed polygon table to free?
    if (!pIT)
        return;

    // free the indexed polygon table content
    if (pIT->m_pData)
        free(pIT->m_pData);

    // free the polygon index table
    free(pIT);
}
//---------------------------------------------------------------------------
