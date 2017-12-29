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

    // release the mesh content
    for (i = 0; i < pMesh->m_Count; ++i)
        if (pMesh->m_pVB[i].m_pData)
            free(pMesh->m_pVB[i].m_pData);

    // free the mesh
    free(pMesh);
}
//---------------------------------------------------------------------------
