/****************************************************************************
 * ==> CSR_Model -----------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides the basic model functions and types   *
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

#include "CSR_Model.h"

// std
#include <stdlib.h>
#include <math.h>

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
    csrVertexFormatCalculateStride(&pMesh->m_pVB[0].m_Format);

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
        csrVertexBufferAdd(&vertex, &normal, &uv, color, pMesh->m_pVB);
    }

    return pMesh;
}
//---------------------------------------------------------------------------
CSR_Mesh* csrShapeCreateSphere(const CSR_VertexFormat* pVertexFormat,
                                     float             radius,
                                     int               slices,
                                     int               stacks,
                                     unsigned          color)
{
    int               i;
    int               j;
    float             majorStep;
    float             minorStep;
    float             a;
    float             b;
    float             r0;
    float             r1;
    float             z0;
    float             z1;
    float             c;
    float             x;
    float             y;
    size_t            index;
    size_t            vbLength;
    CSR_Mesh*         pMesh;
    CSR_VertexBuffer* pVB;
    CSR_Vector3       vertex;
    CSR_Vector3       normal;
    CSR_Vector2       uv;

    // no vertex format?
    if (!pVertexFormat)
        return 0;

    // create a mesh to contain the shape
    pMesh = csrMeshCreate();

    // succeeded?
    if (!pMesh)
        return 0;

    // initialize global values
    majorStep    = (M_PI          / slices);
    minorStep    = ((2.0f * M_PI) / stacks);

    // iterate through vertex slices
    for (i = 0; i < slices; ++i)
    {
        // create a new vertex buffer to contain the next slice
        pVB = (CSR_VertexBuffer*)csrMemoryAlloc(pMesh->m_pVB,
                                                sizeof(CSR_VertexBuffer),
                                                pMesh->m_Count + 1);

        // succeeded?
        if (!pVB)
        {
            csrMeshRelease(pMesh);
            return 0;
        }

        // get the vertex buffer index and update the mesh
        index        = pMesh->m_Count;
        pMesh->m_pVB = pVB;
        ++pMesh->m_Count;

        // initialiye the newlz created vertex buffer
        pMesh->m_pVB[index].m_Format        = *pVertexFormat;
        pMesh->m_pVB[index].m_Format.m_Type = CSR_VT_TriangleStrip;
        pMesh->m_pVB[index].m_pData         = 0;
        pMesh->m_pVB[index].m_Count         = 0;

        // calculate the stride
        csrVertexFormatCalculateStride(&pMesh->m_pVB[index].m_Format);

        // calculate next slice values
        a  = i      * majorStep;
        b  = a      + majorStep;
        r0 = radius * sinf(a);
        r1 = radius * sinf(b);
        z0 = radius * cosf(a);
        z1 = radius * cosf(b);

        // iterate through vertex stacks
        for (j = 0; j <= stacks; ++j)
        {
            c = j * minorStep;
            x = cosf(c);
            y = sinf(c);

            // calculate vertex
            vertex.m_X = x * r0;
            vertex.m_Y = y * r0;
            vertex.m_Z =     z0;

            // do include normals?
            if (pMesh->m_pVB[0].m_Format.m_UseNormals)
            {
                normal.m_X = (x * r0) / radius;
                normal.m_Y = (y * r0) / radius;
                normal.m_Z =      z0  / radius;
            }

            // do include texture?
            if (pMesh->m_pVB[0].m_Format.m_UseTextures)
            {
                uv.m_X = ((float)j / (float)stacks);
                uv.m_Y = ((float)i / (float)slices);
            }

            // add the vertex to the buffer
            csrVertexBufferAdd(&vertex, &normal, &uv, color, &pMesh->m_pVB[index]);

            // calculate vertex
            vertex.m_X = x * r1;
            vertex.m_Y = y * r1;
            vertex.m_Z =     z1;

            // do include normals?
            if (pMesh->m_pVB[0].m_Format.m_UseNormals)
            {
                normal.m_X = (x * r1) / radius;
                normal.m_Y = (y * r1) / radius;
                normal.m_Z =      z1  / radius;
            }

            // do include texture?
            if (pMesh->m_pVB[0].m_Format.m_UseTextures)
            {
                uv.m_X = ( (float)j         / (float)stacks);
                uv.m_Y = (((float)i + 1.0f) / (float)slices);
            }

            // add the vertex to the buffer
            csrVertexBufferAdd(&vertex, &normal, &uv, color, &pMesh->m_pVB[index]);
        }
    }

    return pMesh;
}
//---------------------------------------------------------------------------
