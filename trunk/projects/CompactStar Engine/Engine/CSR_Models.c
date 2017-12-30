/****************************************************************************
 * ==> CSR_Models ----------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides the basic models functions and types  *
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

#include "CSR_Models.h"

// std
#include <stdlib.h>
#include <string.h>

//---------------------------------------------------------------------------
// Vertex functions
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
void csrVertexCalculateStride(CSR_VertexFormat* pVertexFormat)
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
void csrVertexAdd(CSR_Vector3*      pVertex,
                  CSR_Vector3*      pNormal,
                  CSR_Vector2*      pUV,
                  unsigned          color,
                  CSR_VertexBuffer* pVB)
{
    float* pNewData;
    size_t offset = pVB->m_Count;

    // allocate memory for a new vertex
    if (!pVB->m_Count)
        pVB->m_pData = (float*)malloc(pVB->m_Format.m_Stride * sizeof(float));
    else
    {
        pNewData = (float*)realloc(pVB->m_pData,
                                  (pVB->m_Count + pVB->m_Format.m_Stride) * sizeof(float));

        // certify that the new memory block was well allocated
        if (!pNewData)
            return;

        pVB->m_pData = pNewData;
    }

    // failed to allocate memory, do nothing
    if (!pVB->m_pData)
        return;

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
            // cannot add a nonexistent normal, fill with empty data in this case
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
    pMesh->m_pVB   = 0;
    pMesh->m_Count = 0;

    return pMesh;
}
//---------------------------------------------------------------------------
void csrMeshRelease(CSR_Mesh* pMesh)
{
    size_t i;

    // no mesh to free?
    if (!pMesh)
        return;

    // do free the mesh content?
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
// Face functions
//---------------------------------------------------------------------------
CSR_FaceBuffer* csrFaceBufferCreate(void)
{
    // create a new face buffer
    CSR_FaceBuffer* pFB = (CSR_FaceBuffer*)malloc(sizeof(CSR_FaceBuffer));

    // succeeded?
    if (!pFB)
        return 0;

    // initialize the face buffer content
    pFB->m_pData = 0;
    pFB->m_Count = 0;

    return pFB;
}
//---------------------------------------------------------------------------
void csrFaceBufferRelease(CSR_FaceBuffer* pFB)
{
    size_t i;

    // no face buffer to free?
    if (!pFB)
        return;

    // do free the face buffer content?
    if (pFB->m_pData)
    {
        // free the face buffer data content
        for (i = 0; i < pFB->m_Count; ++i)
            if (pFB->m_pData[i].m_pIndex)
                free(pFB->m_pData[i].m_pIndex);

        // free the face buffer data
        free(pFB->m_pData);
    }

    // free the face buffer
    free(pFB);
}
//---------------------------------------------------------------------------
void csrFaceAdd(const size_t* pIndexes, size_t indexCount, CSR_FaceBuffer* pFB)
{
    size_t    offset;
    CSR_Face* pNewFace;

    // no source to add from?
    if (!pIndexes || !indexCount)
        return;

    offset = pFB->m_Count;

    // allocate memory for a new face
    if (!pFB->m_Count)
        pFB->m_pData = (CSR_Face*)malloc(sizeof(CSR_Face));
    else
    {
        pNewFace = (CSR_Face*)realloc(pFB->m_pData, (pFB->m_Count + 1) * sizeof(CSR_Face));

        // certify that the new memory block was well allocated
        if (!pNewFace)
            return;

        pFB->m_pData = pNewFace;
    }

    // failed to allocate memory, do nothing
    if (!pFB->m_pData)
        return;

    ++pFB->m_Count;

    // allocate memory for the face data
    pFB->m_pData[offset].m_pIndex = (size_t*)malloc(indexCount * sizeof(size_t));

    // failed to allocate memory, do nothing
    if (!pFB->m_pData[offset].m_pIndex)
    {
        pFB->m_pData[offset].m_Count = 0;
        return;
    }

    // populate the face content
    memcpy(pFB->m_pData[offset].m_pIndex, pIndexes, indexCount);
    pFB->m_pData[offset].m_Count = indexCount;
}
//---------------------------------------------------------------------------
CSR_FaceBuffer* csrFaceBufferFromVertexBuffer(const CSR_VertexBuffer* pVB)
{
    unsigned i;
    unsigned index;
    size_t   indexes[3];

    // create a face buffer
    CSR_FaceBuffer* pFB = csrFaceBufferCreate();

    // succeeded?
    if (!pFB)
        return 0;

    // assign the reference to the source vertex buffer
    pFB->m_pVB = pVB;

    // no data to extract from?
    if (!pVB || !pVB->m_pData || !pVB->m_Count || !pVB->m_Format.m_Stride)
        return pFB;

    // search for vertex type
    switch (!pVB->m_Format.m_Type)
    {
        case CSR_VT_Triangles:
        {
            // calculate iteration step
            const unsigned step = (pVB->m_Format.m_Stride * 3);

            // iterate through source vertices
            for (i = 0; i < pVB->m_Count; i += step)
            {
                // extract polygon from source vertex buffer and add it to face buffer
                indexes[0] = i;
                indexes[1] = i +  pVB->m_Format.m_Stride;
                indexes[2] = i + (pVB->m_Format.m_Stride * 2);
                csrFaceAdd(indexes, 3, pFB);
            }

            return pFB;
        }

        case CSR_VT_TriangleStrip:
        {
            // calculate length to read in triangle strip buffer
            const unsigned stripLength = (pVB->m_Count - (pVB->m_Format.m_Stride * 2));
                           index       = 0;

            // iterate through source vertices
            for (i = 0; i < stripLength; i += pVB->m_Format.m_Stride)
            {
                // extract polygon from source buffer, revert odd polygons
                if (!index || !(index % 2))
                {
                    indexes[0] = i;
                    indexes[1] = i +  pVB->m_Format.m_Stride;
                    indexes[2] = i + (pVB->m_Format.m_Stride * 2);
                }
                else
                {
                    indexes[0] = i +  pVB->m_Format.m_Stride;
                    indexes[1] = i;
                    indexes[2] = i + (pVB->m_Format.m_Stride * 2);
                }

                csrFaceAdd(indexes, 3, pFB);
                ++index;
            }

            return pFB;
        }

        case CSR_VT_TriangleFan:
        {
            // calculate length to read in triangle fan buffer
            const unsigned fanLength = (pVB->m_Count - pVB->m_Format.m_Stride);

            // iterate through source vertices
            for (i = pVB->m_Format.m_Stride; i < fanLength; i += pVB->m_Format.m_Stride)
            {
                // extract polygon from source buffer
                indexes[0] = 0;
                indexes[1] = i;
                indexes[2] = i + pVB->m_Format.m_Stride;
                csrFaceAdd(indexes, 3, pFB);
            }

            return pFB;
        }

        case CSR_VT_Quads:
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

                // extract first polygon from source buffer
                indexes[0] = v1;
                indexes[1] = v2;
                indexes[2] = v3;
                csrFaceAdd(indexes, 3, pFB);

                // extract second polygon from source buffer
                indexes[0] = v3;
                indexes[1] = v2;
                indexes[2] = v4;
                csrFaceAdd(indexes, 3, pFB);
            }

            return pFB;
        }

        case CSR_VT_QuadStrip:
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

                // extract first polygon from source buffer
                indexes[0] = v1;
                indexes[1] = v2;
                indexes[2] = v3;
                csrFaceAdd(indexes, 3, pFB);

                // extract second polygon from source buffer
                indexes[0] = v3;
                indexes[1] = v2;
                indexes[2] = v4;
                csrFaceAdd(indexes, 3, pFB);
            }

            return pFB;
        }

        default:
            return pFB;
    }
}
//---------------------------------------------------------------------------
// Shape functions
//---------------------------------------------------------------------------
CSR_Mesh* csrShapeCreateSurface(const CSR_VertexFormat* pVertexFormat,
                                      float             width,
                                      float             height,
                                      unsigned          color)
{
    int         i;
    int         index;
    CSR_Mesh*   pMesh;
    CSR_Vector3 vertex;
    CSR_Vector3 normal;
    CSR_Vector2 uv;

    // create a buffer template: 0 for negative values, 1 for positive
    const int bufferTemplate[] =
    {
        0, 0,
        0, 1,
        1, 0,
        1, 1,
    };

    // no vertex format?
    if (!pVertexFormat)
        return 0;

    // create a mesh to contain the shape
    pMesh = csrMeshCreate();

    // succeeded?
    if (!pMesh)
        return 0;

    // create a vertex buffer
    pMesh->m_pVB = csrVertexBufferCreate();

    // succeeded?
    if (!pMesh->m_pVB)
    {
        csrMeshRelease(pMesh);
        return 0;
    }

    // configure the vertex format
    pMesh->m_pVB[0].m_Format = *pVertexFormat;
    csrVertexCalculateStride(&pMesh->m_pVB[0].m_Format);

    // iterate through vertex to create
    for (i = 0; i < 4; ++i)
    {
        // calculate template buffer index
        index = i * 2;

        // populate vertex buffer
        if (bufferTemplate[index])
            vertex.m_X =  width / 2.0f;
        else
            vertex.m_X = -width / 2.0f;

        if (bufferTemplate[index + 1])
            vertex.m_Y =  height / 2.0f;
        else
            vertex.m_Y = -height / 2.0f;

        vertex.m_Z = 0.0f;

        // do include normals?
        if (pMesh->m_pVB[0].m_Format.m_UseNormals)
        {
            normal.m_X =  0.0f;
            normal.m_Y =  0.0f;
            normal.m_Z = -1.0f;
        }

        // do include texture?
        if (pMesh->m_pVB[0].m_Format.m_UseTextures)
        {
            // calculate texture u coordinate
            if (bufferTemplate[index])
                uv.m_X = 1.0f;
            else
                uv.m_X = 0.0f;

            // calculate texture v coordinate
            if (bufferTemplate[index + 1])
                uv.m_Y = 1.0f;
            else
                uv.m_Y = 0.0f;
        }

        // add the vertex to the buffer
        csrVertexAdd(&vertex, &normal, &uv, color, pMesh->m_pVB);
    }

    return pMesh;
}
//---------------------------------------------------------------------------
