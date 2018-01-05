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

//---------------------------------------------------------------------------
// Vertex format functions
//---------------------------------------------------------------------------
void csrVertexFormatCalculateStride(CSR_VertexFormat* pVertexFormat)
{
    // no vertex format?
    if (!pVertexFormat)
        return;

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
// Vertex buffer functions
//---------------------------------------------------------------------------
CSR_VertexBuffer* csrVertexBufferCreate(void)
{
    // create a new vertex buffer
    CSR_VertexBuffer* pVB = (CSR_VertexBuffer*)malloc(sizeof(CSR_VertexBuffer));

    // succeeded?
    if (!pVB)
        return 0;

    // initialize the vertex buffer content
    pVB->m_Format.m_Type        = CSR_VT_Triangles;
    pVB->m_Format.m_UseNormals  = 0;
    pVB->m_Format.m_UseTextures = 0;
    pVB->m_Format.m_UseColors   = 0;
    pVB->m_Format.m_Stride      = 0;
    pVB->m_pData                = 0;
    pVB->m_Count                = 0;

    return pVB;
}
//---------------------------------------------------------------------------
void csrVertexBufferRelease(CSR_VertexBuffer* pVB)
{
    // no vertex buffer to release?
    if (!pVB)
        return;

    // free the vertex buffer content
    if (pVB->m_pData)
        free(pVB->m_pData);

    // free the vertex buffer
    free(pVB);
}
//---------------------------------------------------------------------------
int csrVertexBufferAdd(CSR_Vector3*      pVertex,
                       CSR_Vector3*      pNormal,
                       CSR_Vector2*      pUV,
                       unsigned          color,
                       CSR_VertexBuffer* pVB)
{
    size_t offset;
    float* pNewData;

    // no vertex buffer to add to?
    if (!pVB)
        return 0;

    // allocate memory for the new vertex
    pNewData = (float*)csrMemoryAlloc(pVB->m_pData,
                                      sizeof(float),
                                      pVB->m_Count + pVB->m_Format.m_Stride);

    // succeeded?
    if (!pNewData)
        return 0;

    pVB->m_pData = pNewData;
    offset       = pVB->m_Count;

    // source vertex exists?
    if (!pVertex)
    {
        // cannot add a nonexistent vertex, fill with empty data in this case
        pVB->m_pData[offset]     = 0.0f;
        pVB->m_pData[offset + 1] = 0.0f;
        pVB->m_pData[offset + 2] = 0.0f;
    }
    else
    {
        // copy vertex from source
        pVB->m_pData[offset]     = pVertex->m_X;
        pVB->m_pData[offset + 1] = pVertex->m_Y;
        pVB->m_pData[offset + 2] = pVertex->m_Z;
    }

    offset += 3;

    // do include normals?
    if (pVB->m_Format.m_UseNormals)
    {
        // source normal exists?
        if (!pNormal)
        {
            // cannot add a nonexistent normal, fill with empty data in this case
            pVB->m_pData[offset]     = 0.0f;
            pVB->m_pData[offset + 1] = 0.0f;
            pVB->m_pData[offset + 2] = 0.0f;
        }
        else
        {
            // copy normal from source
            pVB->m_pData[offset]     = pNormal->m_X;
            pVB->m_pData[offset + 1] = pNormal->m_Y;
            pVB->m_pData[offset + 2] = pNormal->m_Z;
        }

        offset += 3;
    }

    // do include texture coordinates?
    if (pVB->m_Format.m_UseTextures)
    {
        // source texture coordinates exists?
        if (!pUV)
        {
            // cannot add nonexistent texture coordinates, fill with empty data in this case
            pVB->m_pData[offset]     = 0.0f;
            pVB->m_pData[offset + 1] = 0.0f;
        }
        else
        {
            // copy texture coordinates from source
            pVB->m_pData[offset]     = pUV->m_X;
            pVB->m_pData[offset + 1] = pUV->m_Y;
        }

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

    return 1;
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
    pMesh->m_Texture.m_TextureID = GL_INVALID_VALUE;
    pMesh->m_Texture.m_BumpMapID = GL_INVALID_VALUE;
    pMesh->m_pVB                 = 0;
    pMesh->m_Count               = 0;

    return pMesh;
}
//---------------------------------------------------------------------------
void csrMeshRelease(CSR_Mesh* pMesh)
{
    size_t i;

    // no mesh to release?
    if (!pMesh)
        return;

    // free the mesh content
    if (pMesh->m_pVB)
    {
        // free the mesh vertex buffer content
        for (i = 0; i < pMesh->m_Count; ++i)
            if (pMesh->m_pVB[i].m_pData)
                free(pMesh->m_pVB[i].m_pData);

        // free the mesh vertex buffer
        free(pMesh->m_pVB);
    }

    // free the mesh
    free(pMesh);
}
//---------------------------------------------------------------------------
// Indexed polygon functions
//---------------------------------------------------------------------------
int csrIndexedPolygonToPolygon(const CSR_IndexedPolygon* pIndexedPolygon,
                                     CSR_Polygon3*       pPolygon)
{
    size_t i;
    size_t index;
    int    success;

    // no polygon to extract to?
    if (!pPolygon)
        return 0;

    // no indexed polygon to extract from?
    if (!pIndexedPolygon || !pIndexedPolygon->m_pVB)
    {
        // clear the polygon
        for (i = 0; i < 3; ++i)
        {
            pPolygon->m_Vertex[i].m_X = 0.0f;
            pPolygon->m_Vertex[i].m_Y = 0.0f;
            pPolygon->m_Vertex[i].m_Z = 0.0f;
        }

        return 0;
    }

    success = 1;

    // iterate through vertices to extract
    for (i = 0; i < 3; ++i)
    {
        // get the vertex buffer index
        index = pIndexedPolygon->m_pIndex[i];

        // is index out of bounds?
        if (index >= pIndexedPolygon->m_pVB->m_Count)
        {
            pPolygon->m_Vertex[i].m_X = 0.0f;
            pPolygon->m_Vertex[i].m_Y = 0.0f;
            pPolygon->m_Vertex[i].m_Z = 0.0f;
            success                   = 0;
            continue;
        }

        // get the polygon vertex from vertex buffer
        pPolygon->m_Vertex[i].m_X = pIndexedPolygon->m_pVB->m_pData[index];
        pPolygon->m_Vertex[i].m_Y = pIndexedPolygon->m_pVB->m_pData[index + 1];
        pPolygon->m_Vertex[i].m_Z = pIndexedPolygon->m_pVB->m_pData[index + 2];
    }

    return success;
}
//---------------------------------------------------------------------------
// Indexed polygon buffer functions
//---------------------------------------------------------------------------
CSR_IndexedPolygonBuffer* csrIndexedPolygonBufferCreate(void)
{
    // create a new indexed polygon buffer
    CSR_IndexedPolygonBuffer* pIPB = (CSR_IndexedPolygonBuffer*)malloc(sizeof(CSR_IndexedPolygonBuffer));

    // succeeded?
    if (!pIPB)
        return 0;

    // initialize the indexed polygon buffer content
    pIPB->m_pIndexedPolygon = 0;
    pIPB->m_Count           = 0;

    return pIPB;
}
//---------------------------------------------------------------------------
void csrIndexedPolygonBufferRelease(CSR_IndexedPolygonBuffer* pIPB)
{
    size_t i;

    // no indexed polygon buffer to release?
    if (!pIPB)
        return;

    // free the indexed polygon buffer content
    if (pIPB->m_pIndexedPolygon)
        free(pIPB->m_pIndexedPolygon);

    // free the indexed polygon buffer
    free(pIPB);
}
//---------------------------------------------------------------------------
int csrIndexedPolygonBufferAdd(const CSR_IndexedPolygon*       pIndexedPolygon,
                                     CSR_IndexedPolygonBuffer* pIPB)
{
    size_t              offset;
    CSR_IndexedPolygon* pNewIndexedPolygon;

    // no indexed polygon to add?
    if (!pIndexedPolygon)
        return 0;

    // no indexed polygon buffer to add to?
    if (!pIPB)
        return 0;

    // allocate memory for the new indexed polygon
    pNewIndexedPolygon = (CSR_IndexedPolygon*)csrMemoryAlloc(pIPB->m_pIndexedPolygon,
                                                             sizeof(CSR_IndexedPolygon),
                                                             pIPB->m_Count + 1);

    // succeeded?
    if (!pNewIndexedPolygon)
        return 0;

    pIPB->m_pIndexedPolygon = pNewIndexedPolygon;
    offset                  = pIPB->m_Count;
    ++pIPB->m_Count;

    // copy the indexed polygon to the indexed polygon buffer
    pIPB->m_pIndexedPolygon[offset] = *pIndexedPolygon;

    return 1;
}
//---------------------------------------------------------------------------
CSR_IndexedPolygonBuffer* csrIndexedPolygonBufferFromMesh(const CSR_Mesh* pMesh)
{
    size_t                    i;
    size_t                    j;
    size_t                    index;
    CSR_IndexedPolygon        indexedPolygon;
    CSR_IndexedPolygonBuffer* pIPB;

    // is source mesh empty?
    if (!pMesh || !pMesh->m_pVB || !pMesh->m_Count)
        return 0;

    // create an indexed polygon buffer
    pIPB = csrIndexedPolygonBufferCreate();

    // succeeded?
    if (!pIPB)
        return 0;

    for (i = 0; i < pMesh->m_Count; ++i)
    {
        // assign the reference to the source vertex buffer
        indexedPolygon.m_pVB = &pMesh->m_pVB[i];

        // search for vertex type
        switch (pMesh->m_pVB[i].m_Format.m_Type)
        {
            case CSR_VT_Triangles:
            {
                // calculate iteration step
                const unsigned step = (pMesh->m_pVB[i].m_Format.m_Stride * 3);

                // iterate through source vertices
                for (j = 0; j < pMesh->m_pVB[i].m_Count; j += step)
                {
                    // extract polygon from source vertex buffer and add it to polygon buffer
                    indexedPolygon.m_pIndex[0] = j;
                    indexedPolygon.m_pIndex[1] = j +  pMesh->m_pVB[i].m_Format.m_Stride;
                    indexedPolygon.m_pIndex[2] = j + (pMesh->m_pVB[i].m_Format.m_Stride * 2);
                    csrIndexedPolygonBufferAdd(&indexedPolygon, pIPB);
                }

                continue;
            }

            case CSR_VT_TriangleStrip:
            {
                // calculate length to read in triangle strip buffer
                const unsigned stripLength =
                        (pMesh->m_pVB[i].m_Count -
                                (pMesh->m_pVB[i].m_Format.m_Stride * 2));

                index = 0;

                // iterate through source vertices
                for (j = 0; j < stripLength; j += pMesh->m_pVB[i].m_Format.m_Stride)
                {
                    // extract polygon from source buffer, revert odd polygons
                    if (!index || !(index % 2))
                    {
                        indexedPolygon.m_pIndex[0] = j;
                        indexedPolygon.m_pIndex[1] = j +  pMesh->m_pVB[i].m_Format.m_Stride;
                        indexedPolygon.m_pIndex[2] = j + (pMesh->m_pVB[i].m_Format.m_Stride * 2);
                    }
                    else
                    {
                        indexedPolygon.m_pIndex[0] = j +  pMesh->m_pVB[i].m_Format.m_Stride;
                        indexedPolygon.m_pIndex[1] = j;
                        indexedPolygon.m_pIndex[2] = j + (pMesh->m_pVB[i].m_Format.m_Stride * 2);
                    }

                    csrIndexedPolygonBufferAdd(&indexedPolygon, pIPB);
                    ++index;
                }

                continue;
            }

            case CSR_VT_TriangleFan:
            {
                // calculate length to read in triangle fan buffer
                const unsigned fanLength =
                        (pMesh->m_pVB[i].m_Count - pMesh->m_pVB[i].m_Format.m_Stride);

                // iterate through source vertices
                for (j  = pMesh->m_pVB[i].m_Format.m_Stride;
                     j  < fanLength;
                     j += pMesh->m_pVB[i].m_Format.m_Stride)
                {
                    // extract polygon from source buffer
                    indexedPolygon.m_pIndex[0] = 0;
                    indexedPolygon.m_pIndex[1] = j;
                    indexedPolygon.m_pIndex[2] = j + pMesh->m_pVB[i].m_Format.m_Stride;
                    csrIndexedPolygonBufferAdd(&indexedPolygon, pIPB);
                }

                continue;
            }

            case CSR_VT_Quads:
            {
                // calculate iteration step
                const unsigned step = (pMesh->m_pVB[i].m_Format.m_Stride * 4);

                // iterate through source vertices
                for (j = 0; j < pMesh->m_pVB[i].m_Count; j += step)
                {
                    // calculate vertices position
                    const unsigned v1 = j;
                    const unsigned v2 = j +  pMesh->m_pVB[i].m_Format.m_Stride;
                    const unsigned v3 = j + (pMesh->m_pVB[i].m_Format.m_Stride * 2);
                    const unsigned v4 = j + (pMesh->m_pVB[i].m_Format.m_Stride * 3);

                    // extract first polygon from source buffer
                    indexedPolygon.m_pIndex[0] = v1;
                    indexedPolygon.m_pIndex[1] = v2;
                    indexedPolygon.m_pIndex[2] = v3;
                    csrIndexedPolygonBufferAdd(&indexedPolygon, pIPB);

                    // extract second polygon from source buffer
                    indexedPolygon.m_pIndex[0] = v3;
                    indexedPolygon.m_pIndex[1] = v2;
                    indexedPolygon.m_pIndex[2] = v4;
                    csrIndexedPolygonBufferAdd(&indexedPolygon, pIPB);
                }

                continue;
            }

            case CSR_VT_QuadStrip:
            {
                // calculate iteration step
                const unsigned step = (pMesh->m_pVB[i].m_Format.m_Stride * 2);

                // calculate length to read in triangle strip buffer
                const unsigned stripLength =
                        (pMesh->m_pVB[i].m_Count -
                                (pMesh->m_pVB[i].m_Format.m_Stride * 2));

                // iterate through source vertices
                for (j = 0; j < stripLength; j += step)
                {
                    // calculate vertices position
                    const unsigned v1 = j;
                    const unsigned v2 = j +  pMesh->m_pVB[i].m_Format.m_Stride;
                    const unsigned v3 = j + (pMesh->m_pVB[i].m_Format.m_Stride * 2);
                    const unsigned v4 = j + (pMesh->m_pVB[i].m_Format.m_Stride * 3);

                    // extract first polygon from source buffer
                    indexedPolygon.m_pIndex[0] = v1;
                    indexedPolygon.m_pIndex[1] = v2;
                    indexedPolygon.m_pIndex[2] = v3;
                    csrIndexedPolygonBufferAdd(&indexedPolygon, pIPB);

                    // extract second polygon from source buffer
                    indexedPolygon.m_pIndex[0] = v3;
                    indexedPolygon.m_pIndex[1] = v2;
                    indexedPolygon.m_pIndex[2] = v4;
                    csrIndexedPolygonBufferAdd(&indexedPolygon, pIPB);
                }

                continue;
            }

            default:
                continue;
        }
    }

    return pIPB;
}
//---------------------------------------------------------------------------
