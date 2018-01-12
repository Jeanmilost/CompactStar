/****************************************************************************
 * ==> CSR_Model -----------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides the model functions and types         *
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
// Tables
//---------------------------------------------------------------------------
float g_NormalTable[] =
{
    -0.525731f,  0.000000f,  0.850651f,
    -0.442863f,  0.238856f,  0.864188f,
    -0.295242f,  0.000000f,  0.955423f,
    -0.309017f,  0.500000f,  0.809017f,
    -0.162460f,  0.262866f,  0.951056f,
     0.000000f,  0.000000f,  1.000000f,
     0.000000f,  0.850651f,  0.525731f,
    -0.147621f,  0.716567f,  0.681718f,
     0.147621f,  0.716567f,  0.681718f,
     0.000000f,  0.525731f,  0.850651f,
     0.309017f,  0.500000f,  0.809017f,
     0.525731f,  0.000000f,  0.850651f,
     0.295242f,  0.000000f,  0.955423f,
     0.442863f,  0.238856f,  0.864188f,
     0.162460f,  0.262866f,  0.951056f,
    -0.681718f,  0.147621f,  0.716567f,
    -0.809017f,  0.309017f,  0.500000f,
    -0.587785f,  0.425325f,  0.688191f,
    -0.850651f,  0.525731f,  0.000000f,
    -0.864188f,  0.442863f,  0.238856f,
    -0.716567f,  0.681718f,  0.147621f,
    -0.688191f,  0.587785f,  0.425325f,
    -0.500000f,  0.809017f,  0.309017f,
    -0.238856f,  0.864188f,  0.442863f,
    -0.425325f,  0.688191f,  0.587785f,
    -0.716567f,  0.681718f, -0.147621f,
    -0.500000f,  0.809017f, -0.309017f,
    -0.525731f,  0.850651f,  0.000000f,
     0.000000f,  0.850651f, -0.525731f,
    -0.238856f,  0.864188f, -0.442863f,
     0.000000f,  0.955423f, -0.295242f,
    -0.262866f,  0.951056f, -0.162460f,
     0.000000f,  1.000000f,  0.000000f,
     0.000000f,  0.955423f,  0.295242f,
    -0.262866f,  0.951056f,  0.162460f,
     0.238856f,  0.864188f,  0.442863f,
     0.262866f,  0.951056f,  0.162460f,
     0.500000f,  0.809017f,  0.309017f,
     0.238856f,  0.864188f, -0.442863f,
     0.262866f,  0.951056f, -0.162460f,
     0.500000f,  0.809017f, -0.309017f,
     0.850651f,  0.525731f,  0.000000f,
     0.716567f,  0.681718f,  0.147621f,
     0.716567f,  0.681718f, -0.147621f,
     0.525731f,  0.850651f,  0.000000f,
     0.425325f,  0.688191f,  0.587785f,
     0.864188f,  0.442863f,  0.238856f,
     0.688191f,  0.587785f,  0.425325f,
     0.809017f,  0.309017f,  0.500000f,
     0.681718f,  0.147621f,  0.716567f,
     0.587785f,  0.425325f,  0.688191f,
     0.955423f,  0.295242f,  0.000000f,
     1.000000f,  0.000000f,  0.000000f,
     0.951056f,  0.162460f,  0.262866f,
     0.850651f, -0.525731f,  0.000000f,
     0.955423f, -0.295242f,  0.000000f,
     0.864188f, -0.442863f,  0.238856f,
     0.951056f, -0.162460f,  0.262866f,
     0.809017f, -0.309017f,  0.500000f,
     0.681718f, -0.147621f,  0.716567f,
     0.850651f,  0.000000f,  0.525731f,
     0.864188f,  0.442863f, -0.238856f,
     0.809017f,  0.309017f, -0.500000f,
     0.951056f,  0.162460f, -0.262866f,
     0.525731f,  0.000000f, -0.850651f,
     0.681718f,  0.147621f, -0.716567f,
     0.681718f, -0.147621f, -0.716567f,
     0.850651f,  0.000000f, -0.525731f,
     0.809017f, -0.309017f, -0.500000f,
     0.864188f, -0.442863f, -0.238856f,
     0.951056f, -0.162460f, -0.262866f,
     0.147621f,  0.716567f, -0.681718f,
     0.309017f,  0.500000f, -0.809017f,
     0.425325f,  0.688191f, -0.587785f,
     0.442863f,  0.238856f, -0.864188f,
     0.587785f,  0.425325f, -0.688191f,
     0.688191f,  0.587785f, -0.425325f,
    -0.147621f,  0.716567f, -0.681718f,
    -0.309017f,  0.500000f, -0.809017f,
     0.000000f,  0.525731f, -0.850651f,
    -0.525731f,  0.000000f, -0.850651f,
    -0.442863f,  0.238856f, -0.864188f,
    -0.295242f,  0.000000f, -0.955423f,
    -0.162460f,  0.262866f, -0.951056f,
     0.000000f,  0.000000f, -1.000000f,
     0.295242f,  0.000000f, -0.955423f,
     0.162460f,  0.262866f, -0.951056f,
    -0.442863f, -0.238856f, -0.864188f,
    -0.309017f, -0.500000f, -0.809017f,
    -0.162460f, -0.262866f, -0.951056f,
     0.000000f, -0.850651f, -0.525731f,
    -0.147621f, -0.716567f, -0.681718f,
     0.147621f, -0.716567f, -0.681718f,
     0.000000f, -0.525731f, -0.850651f,
     0.309017f, -0.500000f, -0.809017f,
     0.442863f, -0.238856f, -0.864188f,
     0.162460f, -0.262866f, -0.951056f,
     0.238856f, -0.864188f, -0.442863f,
     0.500000f, -0.809017f, -0.309017f,
     0.425325f, -0.688191f, -0.587785f,
     0.716567f, -0.681718f, -0.147621f,
     0.688191f, -0.587785f, -0.425325f,
     0.587785f, -0.425325f, -0.688191f,
     0.000000f, -0.955423f, -0.295242f,
     0.000000f, -1.000000f,  0.000000f,
     0.262866f, -0.951056f, -0.162460f,
     0.000000f, -0.850651f,  0.525731f,
     0.000000f, -0.955423f,  0.295242f,
     0.238856f, -0.864188f,  0.442863f,
     0.262866f, -0.951056f,  0.162460f,
     0.500000f, -0.809017f,  0.309017f,
     0.716567f, -0.681718f,  0.147621f,
     0.525731f, -0.850651f,  0.000000f,
    -0.238856f, -0.864188f, -0.442863f,
    -0.500000f, -0.809017f, -0.309017f,
    -0.262866f, -0.951056f, -0.162460f,
    -0.850651f, -0.525731f,  0.000000f,
    -0.716567f, -0.681718f, -0.147621f,
    -0.716567f, -0.681718f,  0.147621f,
    -0.525731f, -0.850651f,  0.000000f,
    -0.500000f, -0.809017f,  0.309017f,
    -0.238856f, -0.864188f,  0.442863f,
    -0.262866f, -0.951056f,  0.162460f,
    -0.864188f, -0.442863f,  0.238856f,
    -0.809017f, -0.309017f,  0.500000f,
    -0.688191f, -0.587785f,  0.425325f,
    -0.681718f, -0.147621f,  0.716567f,
    -0.442863f, -0.238856f,  0.864188f,
    -0.587785f, -0.425325f,  0.688191f,
    -0.309017f, -0.500000f,  0.809017f,
    -0.147621f, -0.716567f,  0.681718f,
    -0.425325f, -0.688191f,  0.587785f,
    -0.162460f, -0.262866f,  0.951056f,
     0.442863f, -0.238856f,  0.864188f,
     0.162460f, -0.262866f,  0.951056f,
     0.309017f, -0.500000f,  0.809017f,
     0.147621f, -0.716567f,  0.681718f,
     0.000000f, -0.525731f,  0.850651f,
     0.425325f, -0.688191f,  0.587785f,
     0.587785f, -0.425325f,  0.688191f,
     0.688191f, -0.587785f,  0.425325f,
    -0.955423f,  0.295242f,  0.000000f,
    -0.951056f,  0.162460f,  0.262866f,
    -1.000000f,  0.000000f,  0.000000f,
    -0.850651f,  0.000000f,  0.525731f,
    -0.955423f, -0.295242f,  0.000000f,
    -0.951056f, -0.162460f,  0.262866f,
    -0.864188f,  0.442863f, -0.238856f,
    -0.951056f,  0.162460f, -0.262866f,
    -0.809017f,  0.309017f, -0.500000f,
    -0.864188f, -0.442863f, -0.238856f,
    -0.951056f, -0.162460f, -0.262866f,
    -0.809017f, -0.309017f, -0.500000f,
    -0.681718f,  0.147621f, -0.716567f,
    -0.681718f, -0.147621f, -0.716567f,
    -0.850651f,  0.000000f, -0.525731f,
    -0.688191f,  0.587785f, -0.425325f,
    -0.587785f,  0.425325f, -0.688191f,
    -0.425325f,  0.688191f, -0.587785f,
    -0.425325f, -0.688191f, -0.587785f,
    -0.587785f, -0.425325f, -0.688191f,
    -0.688191f, -0.587785f, -0.425325f
};
//---------------------------------------------------------------------------
unsigned char g_ColorTable[] =
{
    0,   0,   0,   15,  15,  15,  31,  31,  31,
    47,  47,  47,  63,  63,  63,  75,  75,  75,
    91,  91,  91,  107, 107, 107, 123, 123, 123,
    139, 139, 139, 155, 155, 155, 171, 171, 171,
    187, 187, 187, 203, 203, 203, 219, 219, 219,
    235, 235, 235, 15,  11,  7,   23,  15,  11,
    31,  23,  11,  39,  27,  15,  47,  35,  19,
    55,  43,  23,  63,  47,  23,  75,  55,  27,
    83,  59,  27,  91,  67,  31,  99,  75,  31,
    107, 83,  31,  115, 87,  31,  123, 95,  35,
    131, 103, 35,  143, 111, 35,  11,  11,  15,
    19,  19,  27,  27,  27,  39,  39,  39,  51,
    47,  47,  63,  55,  55,  75,  63,  63,  87,
    71,  71,  103, 79,  79,  115, 91,  91,  127,
    99,  99,  139, 107, 107, 151, 115, 115, 163,
    123, 123, 175, 131, 131, 187, 139, 139, 203,
    0,   0,   0,   7,   7,   0,   11,  11,  0,
    19,  19,  0,   27,  27,  0,   35,  35,  0,
    43,  43,  7,   47,  47,  7,   55,  55,  7,
    63,  63,  7,   71,  71,  7,   75,  75,  11,
    83,  83,  11,  91,  91,  11,  99,  99,  11,
    107, 107, 15,  7,   0,   0,   15,  0,   0,
    23,  0,   0,   31,  0,   0,   39,  0,   0,
    47,  0,   0,   55,  0,   0,   63,  0,   0,
    71,  0,   0,   79,  0,   0,   87,  0,   0,
    95,  0,   0,   103, 0,   0,   111, 0,   0,
    119, 0,   0,   127, 0,   0,   19,  19,  0,
    27,  27,  0,   35,  35,  0,   47,  43,  0,
    55,  47,  0,   67,  55,  0,   75,  59,  7,
    87,  67,  7,   95,  71,  7,   107, 75,  11,
    119, 83,  15,  131, 87,  19,  139, 91,  19,
    151, 95,  27,  163, 99,  31,  175, 103, 35,
    35,  19,  7,   47,  23,  11,  59,  31,  15,
    75,  35,  19,  87,  43,  23,  99,  47,  31,
    115, 55,  35,  127, 59,  43,  143, 67,  51,
    159, 79,  51,  175, 99,  47,  191, 119, 47,
    207, 143, 43,  223, 171, 39,  239, 203, 31,
    255, 243, 27,  11,  7,   0,   27,  19,  0,
    43,  35,  15,  55,  43,  19,  71,  51,  27,
    83,  55,  35,  99,  63,  43,  111, 71,  51,
    127, 83,  63,  139, 95,  71,  155, 107, 83,
    167, 123, 95,  183, 135, 107, 195, 147, 123,
    211, 163, 139, 227, 179, 151, 171, 139, 163,
    159, 127, 151, 147, 115, 135, 139, 103, 123,
    127, 91,  111, 119, 83,  99,  107, 75,  87,
    95,  63,  75,  87,  55,  67,  75,  47,  55,
    67,  39,  47,  55,  31,  35,  43,  23,  27,
    35,  19,  19,  23,  11,  11,  15,  7,   7,
    187, 115, 159, 175, 107, 143, 163, 95,  131,
    151, 87,  119, 139, 79,  107, 127, 75,  95,
    115, 67,  83,  107, 59,  75,  95,  51,  63,
    83,  43,  55,  71,  35,  43,  59,  31,  35,
    47,  23,  27,  35,  19,  19,  23,  11,  11,
    15,  7,   7,   219, 195, 187, 203, 179, 167,
    191, 163, 155, 175, 151, 139, 163, 135, 123,
    151, 123, 111, 135, 111, 95,  123, 99,  83,
    107, 87,  71,  95,  75,  59,  83,  63,  51,
    67,  51,  39,  55,  43,  31,  39,  31,  23,
    27,  19,  15,  15,  11,  7,   111, 131, 123,
    103, 123, 111, 95,  115, 103, 87,  107, 95,
    79,  99,  87,  71,  91,  79,  63,  83,  71,
    55,  75,  63,  47,  67,  55,  43,  59,  47,
    35,  51,  39,  31,  43,  31,  23,  35,  23,
    15,  27,  19,  11,  19,  11,  7,   11,  7,
    255, 243, 27,  239, 223, 23,  219, 203, 19,
    203, 183, 15,  187, 167, 15,  171, 151, 11,
    155, 131, 7,   139, 115, 7,   123, 99,  7,
    107, 83,  0,   91,  71,  0,   75,  55,  0,
    59,  43,  0,   43,  31,  0,   27,  15,  0,
    11,  7,   0,   0,   0,   255, 11,  11,  239,
    19,  19,  223, 27,  27,  207, 35,  35,  191,
    43,  43,  175, 47,  47,  159, 47,  47,  143,
    47,  47,  127, 47,  47,  111, 47,  47,  95,
    43,  43,  79,  35,  35,  63,  27,  27,  47,
    19,  19,  31,  11,  11,  15,  43,  0,   0,
    59,  0,   0,   75,  7,   0,   95,  7,   0,
    111, 15,  0,   127, 23,  7,   147, 31,  7,
    163, 39,  11,  183, 51,  15,  195, 75,  27,
    207, 99,  43,  219, 127, 59,  227, 151, 79,
    231, 171, 95,  239, 191, 119, 247, 211, 139,
    167, 123, 59,  183, 155, 55,  199, 195, 55,
    231, 227, 87,  127, 191, 255, 171, 231, 255,
    215, 255, 255, 103, 0,   0,   139, 0,   0,
    179, 0,   0,   215, 0,   0,   255, 0,   0,
    255, 243, 147, 255, 247, 199, 255, 255, 255,
    159, 91,  83
};
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
CSR_Mesh* csrShapeCreateBox(const CSR_VertexFormat* pVertexFormat,
                                  float             width,
                                  float             height,
                                  float             depth,
                                  unsigned          color,
                                  int               repeatTexOnEachFace)
{
    size_t      i;
    CSR_Vector3 vertices[8];
    CSR_Vector3 normals[6];
    CSR_Vector2 texCoords[24];
    CSR_Mesh*   pMesh;

    // calculate half values
    const float halfX = width  / 2.0;
    const float halfY = height / 2.0;
    const float halfZ = depth  / 2.0;

    // no vertex format?
    if (!pVertexFormat)
        return 0;

    // create a new mesh for the box
    pMesh = csrMeshCreate();

    // succeeded?
    if (!pMesh)
        return 0;

    // prepare the vertex buffer for each box edges
    pMesh->m_pVB   = (CSR_VertexBuffer*)csrMemoryAlloc(0, sizeof(CSR_VertexBuffer), 6);
    pMesh->m_Count = 6;

    // succeeded?
    if (!pMesh->m_pVB)
    {
        csrMeshRelease(pMesh);
        return 0;
    }

    // iterate through each edges
    for (i = 0; i < 6; ++i)
    {
        // set vertex format and calculate the stride
        pMesh->m_pVB[i].m_Format        = *pVertexFormat;
        pMesh->m_pVB[i].m_Format.m_Type = CSR_VT_TriangleStrip;
        csrVertexFormatCalculateStride(&pMesh->m_pVB[i].m_Format);

        // initialize the vertex buffer data
        pMesh->m_pVB[i].m_pData = 0;
        pMesh->m_pVB[i].m_Count = 0;
    }

    // iterate through vertices to create. Vertices are generated as follow:
    //     v2 *--------* v6
    //      / |      / |
    // v4 *--------* v8|
    //    |   |    |   |
    //    |v1 *----|---* v5
    //    | /      | /
    // v3 *--------* v7
    for (i = 0; i < 8; ++i)
    {
        // generate the 4 first vertices on the left, and 4 last on the right
        if (!(i / 4))
            vertices[i].m_X = -halfX;
        else
            vertices[i].m_X =  halfX;

        // generate 2 vertices on the front, then 2 vertices on the back
        if (!((i / 2) % 2))
            vertices[i].m_Z = -halfZ;
        else
            vertices[i].m_Z =  halfZ;

        // for each vertices, generates one on the top, and one on the bottom
        if (!(i % 2))
            vertices[i].m_Y = -halfY;
        else
            vertices[i].m_Y =  halfY;
    }

    // calculate normals
    normals[0].m_X = -1.0; normals[0].m_Y =  0.0; normals[0].m_Z =  0.0;
    normals[1].m_X =  1.0; normals[1].m_Y =  0.0; normals[1].m_Z =  0.0;
    normals[2].m_X =  0.0; normals[2].m_Y = -1.0; normals[2].m_Z =  0.0;
    normals[3].m_X =  0.0; normals[3].m_Y =  1.0; normals[3].m_Z =  0.0;
    normals[4].m_X =  0.0; normals[4].m_Y =  0.0; normals[4].m_Z = -1.0;
    normals[5].m_X =  0.0; normals[5].m_Y =  0.0; normals[5].m_Z =  1.0;

    // do repeat texture on each faces?
    if (repeatTexOnEachFace)
    {
        // calculate texture positions
        texCoords[0].m_X  = 0.0; texCoords[0].m_Y  = 0.0;
        texCoords[1].m_X  = 0.0; texCoords[1].m_Y  = 1.0;
        texCoords[2].m_X  = 1.0; texCoords[2].m_Y  = 0.0;
        texCoords[3].m_X  = 1.0; texCoords[3].m_Y  = 1.0;
        texCoords[4].m_X  = 0.0; texCoords[4].m_Y  = 0.0;
        texCoords[5].m_X  = 0.0; texCoords[5].m_Y  = 1.0;
        texCoords[6].m_X  = 1.0; texCoords[6].m_Y  = 0.0;
        texCoords[7].m_X  = 1.0; texCoords[7].m_Y  = 1.0;
        texCoords[8].m_X  = 0.0; texCoords[8].m_Y  = 0.0;
        texCoords[9].m_X  = 0.0; texCoords[9].m_Y  = 1.0;
        texCoords[10].m_X = 1.0; texCoords[10].m_Y = 0.0;
        texCoords[11].m_X = 1.0; texCoords[11].m_Y = 1.0;
        texCoords[12].m_X = 0.0; texCoords[12].m_Y = 0.0;
        texCoords[13].m_X = 0.0; texCoords[13].m_Y = 1.0;
        texCoords[14].m_X = 1.0; texCoords[14].m_Y = 0.0;
        texCoords[15].m_X = 1.0; texCoords[15].m_Y = 1.0;
        texCoords[16].m_X = 0.0; texCoords[16].m_Y = 0.0;
        texCoords[17].m_X = 0.0; texCoords[17].m_Y = 1.0;
        texCoords[18].m_X = 1.0; texCoords[18].m_Y = 0.0;
        texCoords[19].m_X = 1.0; texCoords[19].m_Y = 1.0;
        texCoords[20].m_X = 0.0; texCoords[20].m_Y = 0.0;
        texCoords[21].m_X = 0.0; texCoords[21].m_Y = 1.0;
        texCoords[22].m_X = 1.0; texCoords[22].m_Y = 0.0;
        texCoords[23].m_X = 1.0; texCoords[23].m_Y = 1.0;
    }
    else
    {
        // calculate texture offset
        const float texOffset = 1.0 / 3.0;

        // calculate texture positions. They are distributed as follow:
        // -------------------
        // |     |     |     |
        // |  1  |  2  |  3  |
        // |     |     |     |
        // |-----------------|
        // |     |     |     |
        // |  4  |  5  |  6  |
        // |     |     |     |
        // -------------------
        // |  This texture   |
        // |  area is not    |
        // |  used           |
        // -------------------
        texCoords[0].m_X  = 0.0;             texCoords[0].m_Y  = texOffset;
        texCoords[1].m_X  = 0.0;             texCoords[1].m_Y  = 0.0;
        texCoords[2].m_X  = texOffset;       texCoords[2].m_Y  = texOffset;
        texCoords[3].m_X  = texOffset;       texCoords[3].m_Y  = 0.0;
        texCoords[4].m_X  = texOffset;       texCoords[4].m_Y  = texOffset;
        texCoords[5].m_X  = texOffset;       texCoords[5].m_Y  = 0.0;
        texCoords[6].m_X  = texOffset * 2.0; texCoords[6].m_Y  = texOffset;
        texCoords[7].m_X  = texOffset * 2.0; texCoords[7].m_Y  = 0.0;
        texCoords[8].m_X  = texOffset * 2.0; texCoords[8].m_Y  = texOffset;
        texCoords[9].m_X  = texOffset * 2.0; texCoords[9].m_Y  = 0.0;
        texCoords[10].m_X = 1.0;             texCoords[10].m_Y = texOffset;
        texCoords[11].m_X = 1.0;             texCoords[11].m_Y = 0.0;
        texCoords[12].m_X = 0.0;             texCoords[12].m_Y = texOffset * 2.0;
        texCoords[13].m_X = 0.0;             texCoords[13].m_Y = texOffset;
        texCoords[14].m_X = texOffset;       texCoords[14].m_Y = texOffset * 2.0;
        texCoords[15].m_X = texOffset;       texCoords[15].m_Y = texOffset;
        texCoords[16].m_X = texOffset;       texCoords[16].m_Y = texOffset * 2.0;
        texCoords[17].m_X = texOffset;       texCoords[17].m_Y = texOffset;
        texCoords[18].m_X = texOffset * 2.0; texCoords[18].m_Y = texOffset * 2.0;
        texCoords[19].m_X = texOffset * 2.0; texCoords[19].m_Y = texOffset;
        texCoords[20].m_X = texOffset * 2.0; texCoords[20].m_Y = texOffset * 2.0;
        texCoords[21].m_X = texOffset * 2.0; texCoords[21].m_Y = texOffset;
        texCoords[22].m_X = 1.0;             texCoords[22].m_Y = texOffset * 2.0;
        texCoords[23].m_X = 1.0;             texCoords[23].m_Y = texOffset;
    }

    // create box edge 1
    csrVertexBufferAdd(&vertices[1], &normals[0], &texCoords[4], color, &pMesh->m_pVB[0]);
    csrVertexBufferAdd(&vertices[0], &normals[0], &texCoords[5], color, &pMesh->m_pVB[0]);
    csrVertexBufferAdd(&vertices[3], &normals[0], &texCoords[6], color, &pMesh->m_pVB[0]);
    csrVertexBufferAdd(&vertices[2], &normals[0], &texCoords[7], color, &pMesh->m_pVB[0]);

    // create box edge 2
    csrVertexBufferAdd(&vertices[3], &normals[5], &texCoords[8],  color, &pMesh->m_pVB[1]);
    csrVertexBufferAdd(&vertices[2], &normals[5], &texCoords[9],  color, &pMesh->m_pVB[1]);
    csrVertexBufferAdd(&vertices[7], &normals[5], &texCoords[10], color, &pMesh->m_pVB[1]);
    csrVertexBufferAdd(&vertices[6], &normals[5], &texCoords[11], color, &pMesh->m_pVB[1]);

    // create box edge 3
    csrVertexBufferAdd(&vertices[7], &normals[1], &texCoords[12], color, &pMesh->m_pVB[2]);
    csrVertexBufferAdd(&vertices[6], &normals[1], &texCoords[13], color, &pMesh->m_pVB[2]);
    csrVertexBufferAdd(&vertices[5], &normals[1], &texCoords[14], color, &pMesh->m_pVB[2]);
    csrVertexBufferAdd(&vertices[4], &normals[1], &texCoords[15], color, &pMesh->m_pVB[2]);

    // create box edge 4
    csrVertexBufferAdd(&vertices[5], &normals[4], &texCoords[16], color, &pMesh->m_pVB[3]);
    csrVertexBufferAdd(&vertices[4], &normals[4], &texCoords[17], color, &pMesh->m_pVB[3]);
    csrVertexBufferAdd(&vertices[1], &normals[4], &texCoords[18], color, &pMesh->m_pVB[3]);
    csrVertexBufferAdd(&vertices[0], &normals[4], &texCoords[19], color, &pMesh->m_pVB[3]);

    // create box edge 5
    csrVertexBufferAdd(&vertices[1], &normals[3], &texCoords[0], color, &pMesh->m_pVB[4]);
    csrVertexBufferAdd(&vertices[3], &normals[3], &texCoords[1], color, &pMesh->m_pVB[4]);
    csrVertexBufferAdd(&vertices[5], &normals[3], &texCoords[2], color, &pMesh->m_pVB[4]);
    csrVertexBufferAdd(&vertices[7], &normals[3], &texCoords[3], color, &pMesh->m_pVB[4]);

    // create box edge 6
    csrVertexBufferAdd(&vertices[2], &normals[2], &texCoords[20], color, &pMesh->m_pVB[5]);
    csrVertexBufferAdd(&vertices[0], &normals[2], &texCoords[21], color, &pMesh->m_pVB[5]);
    csrVertexBufferAdd(&vertices[6], &normals[2], &texCoords[22], color, &pMesh->m_pVB[5]);
    csrVertexBufferAdd(&vertices[4], &normals[2], &texCoords[23], color, &pMesh->m_pVB[5]);

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
// MDL model functions
//---------------------------------------------------------------------------
void csrMDLReadHeader(const CSR_Buffer* pBuffer, size_t* pOffset, CSR_MDLHeader* pHeader)
{
    // read header from file
    csrBufferRead(pBuffer, pOffset, sizeof(unsigned),               1, &pHeader->m_ID);
    csrBufferRead(pBuffer, pOffset, sizeof(unsigned),               1, &pHeader->m_Version);
    csrBufferRead(pBuffer, pOffset, sizeof(pHeader->m_Scale),       1, &pHeader->m_Scale);
    csrBufferRead(pBuffer, pOffset, sizeof(pHeader->m_Translate),   1, &pHeader->m_Translate);
    csrBufferRead(pBuffer, pOffset, sizeof(float),                  1, &pHeader->m_BoundingRadius);
    csrBufferRead(pBuffer, pOffset, sizeof(pHeader->m_EyePosition), 1, &pHeader->m_EyePosition);
    csrBufferRead(pBuffer, pOffset, sizeof(unsigned),               1, &pHeader->m_SkinCount);
    csrBufferRead(pBuffer, pOffset, sizeof(unsigned),               1, &pHeader->m_SkinWidth);
    csrBufferRead(pBuffer, pOffset, sizeof(unsigned),               1, &pHeader->m_SkinHeight);
    csrBufferRead(pBuffer, pOffset, sizeof(unsigned),               1, &pHeader->m_VertexCount);
    csrBufferRead(pBuffer, pOffset, sizeof(unsigned),               1, &pHeader->m_PolygonCount);
    csrBufferRead(pBuffer, pOffset, sizeof(unsigned),               1, &pHeader->m_FrameCount);
    csrBufferRead(pBuffer, pOffset, sizeof(unsigned),               1, &pHeader->m_SyncType);
    csrBufferRead(pBuffer, pOffset, sizeof(unsigned),               1, &pHeader->m_Flags);
    csrBufferRead(pBuffer, pOffset, sizeof(float),                  1, &pHeader->m_Size);
}
//---------------------------------------------------------------------------
void csrMDLReadSkin(const CSR_Buffer*    pBuffer,
                          size_t*        pOffset,
                    const CSR_MDLHeader* pHeader,
                          CSR_MDLSkin*   pSkin)
{
    // calculate texture size
    pSkin->m_TexLen = pHeader->m_SkinWidth * pHeader->m_SkinHeight;

    // read the skin group flag
    csrBufferRead(pBuffer, pOffset, sizeof(unsigned), 1, &pSkin->m_Group);

    pSkin->m_pTime = 0;

    // is a group of textures?
    if (!pSkin->m_Group)
    {
        pSkin->m_Count = 1;

        // create memory for texture
        pSkin->m_pData = (unsigned char*)malloc(pSkin->m_TexLen);

        // read texture from file
        csrBufferRead(pBuffer, pOffset, pSkin->m_TexLen, 1, pSkin->m_pData);

        return;
    }

    // read the skin count
    csrBufferRead(pBuffer, pOffset, sizeof(unsigned), 1, &pSkin->m_Count);

    // no skin to read?
    if (!pSkin->m_Count)
        return;

    // create memory for time table
    pSkin->m_pTime = (float*)malloc(pSkin->m_Count * sizeof(float));

    // read time table from file
    csrBufferRead(pBuffer, pOffset, pSkin->m_Count * sizeof(float), 1, pSkin->m_pTime);

    // create memory for texture
    pSkin->m_pData = (unsigned char*)malloc(pSkin->m_TexLen * pSkin->m_Count);

    // read texture from file
    csrBufferRead(pBuffer, pOffset, pSkin->m_TexLen * pSkin->m_Count, 1, pSkin->m_pData);
}
//---------------------------------------------------------------------------
void csrMDLReadTextureCoord(const CSR_Buffer*          pBuffer,
                                  size_t*              pOffset,
                                  CSR_MDLTextureCoord* pTexCoord)
{
    // read texture coordinates from file
    csrBufferRead(pBuffer, pOffset, sizeof(unsigned), 1, &pTexCoord->m_OnSeam);
    csrBufferRead(pBuffer, pOffset, sizeof(unsigned), 1, &pTexCoord->m_U);
    csrBufferRead(pBuffer, pOffset, sizeof(unsigned), 1, &pTexCoord->m_V);
}
//---------------------------------------------------------------------------
void csrMDLReadPolygon(const CSR_Buffer* pBuffer, size_t* pOffset, CSR_MDLPolygon* pPolygon)
{
    // read polygon from file
    csrBufferRead(pBuffer, pOffset, sizeof(unsigned),                1, &pPolygon->m_FacesFront);
    csrBufferRead(pBuffer, pOffset, sizeof(pPolygon->m_VertexIndex), 1, &pPolygon->m_VertexIndex);
}
//---------------------------------------------------------------------------
void csrMDLReadVertex(const CSR_Buffer* pBuffer, size_t* pOffset, CSR_MDLVertex* pVertex)
{
    // read vertex from file
    csrBufferRead(pBuffer, pOffset, sizeof(pVertex->m_Vertex), 1, &pVertex->m_Vertex);
    csrBufferRead(pBuffer, pOffset, sizeof(unsigned),          1, &pVertex->m_NormalIndex);
}
//---------------------------------------------------------------------------
void csrMDLReadFrame(const CSR_Buffer*    pBuffer,
                           size_t*        pOffset,
                     const CSR_MDLHeader* pHeader,
                           CSR_MDLFrame*  pFrame)
{
    unsigned i;

    // read frame bounding box
    csrMDLReadVertex(pBuffer, pOffset, &pFrame->m_BoundingBoxMin);
    csrMDLReadVertex(pBuffer, pOffset, &pFrame->m_BoundingBoxMax);

    // read frame name
    csrBufferRead(pBuffer, pOffset, sizeof(char), 16, &pFrame->m_Name);

    // create frame vertex buffer
    pFrame->m_pVertex = (CSR_MDLVertex*)malloc(sizeof(CSR_MDLVertex) * pHeader->m_VertexCount);

    // read frame vertices
    for (i = 0; i < pHeader->m_VertexCount; ++i)
        csrMDLReadVertex(pBuffer, pOffset, &pFrame->m_pVertex[i]);
}
//---------------------------------------------------------------------------
void csrMDLReadFrameGroup(const CSR_Buffer*        pBuffer,
                                size_t*            pOffset,
                          const CSR_MDLHeader*     pHeader,
                                CSR_MDLFrameGroup* pFrameGroup)
{
    // read the group type
    csrBufferRead(pBuffer, pOffset, sizeof(unsigned), 1, &pFrameGroup->m_Type);

    // is a single frame or a group of frames?
    if (!pFrameGroup->m_Type)
    {
        pFrameGroup->m_Count = 1;

        // create frame vertex buffer
        pFrameGroup->m_pFrame = (CSR_MDLFrame*)malloc(sizeof(CSR_MDLFrame) * pFrameGroup->m_Count);

        csrMDLReadFrame(pBuffer, pOffset, pHeader, &pFrameGroup->m_pFrame[0]);
        return;
    }

    // todo FIXME -cFeature -oJean: This code should be implemented
}
//---------------------------------------------------------------------------
void csrMDLUncompressTexture(const CSR_MDLSkin* pSkin, unsigned index, CSR_Texture* pTexture)
{
    unsigned offset;
    unsigned i;

    // calculate texture length and start offset
    pTexture->m_Length = pSkin->m_TexLen;
    offset             = pTexture->m_Length * index;

    // allocate memory for the texture pixels
    pTexture->m_pPixels = (unsigned char*)malloc(sizeof(unsigned char) * pTexture->m_Length * 3);

    // convert indexed 8 bits texture to RGB 24 bits
    for (i = 0; i < pTexture->m_Length; ++i)
    {
        pTexture->m_pPixels[(i * 3)]     = g_MDLColors[pSkin->m_pData[offset + i] * 3];
        pTexture->m_pPixels[(i * 3) + 1] = g_MDLColors[pSkin->m_pData[offset + i] * 3 + 1];
        pTexture->m_pPixels[(i * 3) + 2] = g_MDLColors[pSkin->m_pData[offset + i] * 3 + 2];
    }
}
//---------------------------------------------------------------------------
void csrMDLUncompressVertex(const CSR_MDLHeader* pHeader,
                            const CSR_MDLVertex* pVertex,
                                  CSR_Vector3*   pResult)
{
    unsigned i;
    float    vertex[3];

    // iterate through vertex coordinates
    for (i = 0; i < 3; ++i)
        // uncompress vertex using frame scale and translate values
        vertex[i] = (pHeader->m_Scale[i] * pVertex->m_Vertex[i]) + pHeader->m_Translate[i];

    // copy decompressed vertex to result
    pResult->m_X = vertex[0];
    pResult->m_Y = vertex[1];
    pResult->m_Z = vertex[2];
}
//---------------------------------------------------------------------------
int miniCreateMDLMesh(MINI_MDLHeader*       pHeader,
                      MINI_MDLFrameGroup*   pFrameGroups,
                      MINI_MDLSkin*         pSkin,
                      MINI_MDLTextureCoord* pTexCoord,
                      MINI_MDLPolygon*      pPolygon,
                      MINI_VertexFormat*    pVertexFormat,
                      unsigned              color,
                      MINI_MDLModel**       pMDLModel)
{
    unsigned int        index;
    unsigned int        vertexIndex;
    unsigned int        normalIndex;
    unsigned int        offset;
    unsigned int        vertexLength;
    int                 i;
    int                 j;
    int                 k;
    float               tu;
    float               tv;
    MINI_MDLFrameGroup* pSrcFrameGroup;
    MINI_MDLVertex*     pSrcVertex;
    MINI_Frame*         pMdlFrm;
    MINI_Mesh*          pMdlMesh;
    MINI_Vector3        vertex;

    // create MDL model
    *pMDLModel                    = (MINI_MDLModel*)    malloc(sizeof(MINI_MDLModel));
    (*pMDLModel)->m_pVertexFormat = (MINI_VertexFormat*)malloc(sizeof(MINI_VertexFormat));
    (*pMDLModel)->m_pFrame        = (MINI_Frame*)       malloc(sizeof(MINI_Frame) * pHeader->m_FrameCount);
    (*pMDLModel)->m_FrameCount    = pHeader->m_FrameCount;

    // calculate stride
    miniCalculateStride(pVertexFormat);

    // copy vertex format
    *((*pMDLModel)->m_pVertexFormat) = *pVertexFormat;

    vertexLength = sizeof(float) * pVertexFormat->m_Stride;

    // iterate through frames to create
    for (index = 0; index < pHeader->m_FrameCount; ++index)
    {
        // get source frame group from which meshes should be extracted
        pSrcFrameGroup = &pFrameGroups[index];

        // get current frame to populate
        pMdlFrm              = &(*pMDLModel)->m_pFrame[index];
        pMdlFrm->m_pMesh     = 0;
        pMdlFrm->m_MeshCount = 0;

        // iterate through meshes composing the frame
        for (i = 0; i < pSrcFrameGroup->m_Count; ++i)
        {
            // set mesh count
            ++pMdlFrm->m_MeshCount;

            // create new mesh
            if (!pMdlFrm->m_pMesh)
                pMdlFrm->m_pMesh = (MINI_Mesh*)malloc(sizeof(MINI_Mesh));
            else
                pMdlFrm->m_pMesh = (MINI_Mesh*)realloc(pMdlFrm->m_pMesh,
                                                       sizeof(MINI_Mesh) * pMdlFrm->m_MeshCount);

            // get current mesh to populate
            pMdlMesh = &pMdlFrm->m_pMesh[i];

            // populate newly created mesh
            pMdlMesh->m_pVertexBuffer = 0;
            pMdlMesh->m_VertexCount   = pHeader->m_PolygonCount * 3;

            // add new vertex
            if (!pMdlMesh->m_pVertexBuffer)
                pMdlMesh->m_pVertexBuffer = (float*)malloc(pMdlMesh->m_VertexCount * vertexLength);
            else
                pMdlMesh->m_pVertexBuffer = (float*)realloc(pMdlMesh->m_pVertexBuffer,
                                                            pMdlMesh->m_VertexCount * vertexLength);

            pMdlMesh->m_IsTriangleStrip = 0;

            vertexIndex = 0;

            // iterate through polygons to process
            for (j = 0; j < pHeader->m_PolygonCount; ++j)
                for (k = 0; k < 3; ++k)
                {
                    // get source vertex
                    pSrcVertex =
                            &pSrcFrameGroup->m_pFrame[i].m_pVertex[pPolygon[j].m_VertexIndex[k]];

                    // uncompress vertex
                    miniUncompressMDLVertex(pHeader, pSrcVertex, &vertex);

                    offset = vertexIndex;

                    // populate vertex
                    pMdlMesh->m_pVertexBuffer[offset]     = vertex.m_X;
                    pMdlMesh->m_pVertexBuffer[offset + 1] = vertex.m_Y;
                    pMdlMesh->m_pVertexBuffer[offset + 2] = vertex.m_Z;

                    offset += 3;

                    // do include normals?
                    if ((*pMDLModel)->m_pVertexFormat->m_UseNormals)
                    {
                        // calculate normal index in table
                        normalIndex = pSrcVertex->m_NormalIndex;

                        // extract normal
                        pMdlMesh->m_pVertexBuffer[offset]     = g_NormalTable[normalIndex];
                        pMdlMesh->m_pVertexBuffer[offset + 1] = g_NormalTable[normalIndex + 1];
                        pMdlMesh->m_pVertexBuffer[offset + 2] = g_NormalTable[normalIndex + 2];

                        offset += 3;
                    }

                    // do include texture coordinates?
                    if ((*pMDLModel)->m_pVertexFormat->m_UseTextures)
                    {
                        // get vertex texture coordinates. Be careful, here a pointer of
                        // type float should be read from memory, for that the conversion
                        // cannot be done from M_Precision
                        tu = pTexCoord[pPolygon[j].m_VertexIndex[k]].m_U;
                        tv = pTexCoord[pPolygon[j].m_VertexIndex[k]].m_V;

                        // is texture coordinate on the back face?
                        if (!pPolygon[j].m_FacesFront &&
                                pTexCoord[pPolygon[j].m_VertexIndex[k]].m_OnSeam)
                            // correct the texture coordinate to put it on the back face
                            tu += pHeader->m_SkinWidth * 0.5f;

                        // scale s and t to range from 0.0 to 1.0
                        tu = (tu + 0.5) / pHeader->m_SkinWidth;
                        tv = (tv + 0.5) / pHeader->m_SkinHeight;

                        // copy texture coordinates from source
                        pMdlMesh->m_pVertexBuffer[offset]     = tu;
                        pMdlMesh->m_pVertexBuffer[offset + 1] = tv;

                        offset += 2;
                    }

                    // do use colors?
                    if ((*pMDLModel)->m_pVertexFormat->m_UseColors)
                    {
                        pMdlMesh->m_pVertexBuffer[offset]     = (float)((color >> 24) & 0xFF) / 255.0f;
                        pMdlMesh->m_pVertexBuffer[offset + 1] = (float)((color >> 16) & 0xFF) / 255.0f;
                        pMdlMesh->m_pVertexBuffer[offset + 2] = (float)((color >> 8)  & 0xFF) / 255.0f;
                        pMdlMesh->m_pVertexBuffer[offset + 3] = (float)( color        & 0xFF) / 255.0f;
                    }

                    vertexIndex += pVertexFormat->m_Stride;
                }
        }
    }

    return 1;
}
//---------------------------------------------------------------------------
int miniLoadMDLModel(const unsigned char*     pName,
                           MINI_VertexFormat* pVertexFormat,
                           unsigned           color,
                           MINI_MDLModel**    pMDLModel,
                           MINI_Texture*      pTexture)
{
    FILE*                 pFile;
    MINI_MDLHeader*       pHeader;
    MINI_MDLSkin*         pSkin;
    MINI_MDLTextureCoord* pTexCoord;
    MINI_MDLPolygon*      pPolygon;
    MINI_MDLFrameGroup*   pFrameGroup;
    unsigned int          i;
    int                   result = 0;

    // open mdl file
    pFile = M_MINI_FILE_OPEN((const char*)pName, "rb");

    // succeeded?
    if (!pFile)
        return 0;

    // create mdl header
    pHeader = (MINI_MDLHeader*)malloc(sizeof(MINI_MDLHeader));

    // read file header
    miniReadMDLHeader(pFile, pHeader);

    // is mdl file and version correct?
    if ((pHeader->m_ID != M_MDL_ID) || ((float)pHeader->m_Version != M_MDL_Mesh_File_Version))
    {
        free(pHeader);
        return 0;
    }

    // read skins
    if (pHeader->m_SkinCount)
    {
        pSkin = (MINI_MDLSkin*)malloc(sizeof(MINI_MDLSkin) * pHeader->m_SkinCount);

        for (i = 0; i < pHeader->m_SkinCount; ++i)
            miniReadMDLSkin(pFile, pHeader, &pSkin[i]);
    }
    else
        pSkin = 0;

    // read texture coordinates
    if (pHeader->m_VertexCount)
    {
        pTexCoord = (MINI_MDLTextureCoord*)malloc(sizeof(MINI_MDLTextureCoord) * pHeader->m_VertexCount);

        for (i = 0; i < pHeader->m_VertexCount; ++i)
            miniReadMDLTextureCoord(pFile, &pTexCoord[i]);
    }
    else
        pTexCoord = 0;

    // read polygons
    if (pHeader->m_PolygonCount)
    {
        pPolygon = (MINI_MDLPolygon*)malloc(sizeof(MINI_MDLPolygon) * pHeader->m_PolygonCount);

        for (i = 0; i < pHeader->m_PolygonCount; ++i)
            miniReadMDLPolygon(pFile, &pPolygon[i]);
    }
    else
        pPolygon = 0;

    // read frames
    if (pHeader->m_FrameCount)
    {
        pFrameGroup = (MINI_MDLFrameGroup*)malloc(sizeof(MINI_MDLFrameGroup) * pHeader->m_FrameCount);

        for (i = 0; i < pHeader->m_FrameCount; ++i)
            miniReadMDLFrameGroup(pFile, pHeader, &pFrameGroup[i]);
    }
    else
        pFrameGroup = 0;

    // close MD2 file
    M_MINI_FILE_CLOSE(pFile);

    // create mesh from file content
    result = miniCreateMDLMesh(pHeader,
                               pFrameGroup,
                               pSkin,
                               pTexCoord,
                               pPolygon,
                               pVertexFormat,
                               color,
                               pMDLModel);

    // extract texture from model
    miniUncompressMDLTexture(pSkin, 0, pTexture);

    // set texture size
    pTexture->m_Width  = pHeader->m_SkinWidth;
    pTexture->m_Height = pHeader->m_SkinHeight;

    // delete frame vertices
    for (i = 0; i < pHeader->m_FrameCount; ++i)
        if (pFrameGroup[i].m_pFrame)
        {
            free(pFrameGroup[i].m_pFrame->m_pVertex);
            free(pFrameGroup[i].m_pFrame);
        }

    // delete skin time table
    if (pSkin->m_pTime)
        free(pSkin->m_pTime);

    // delete skin data
    if (pSkin->m_pData)
        free(pSkin->m_pData);

    // delete MD2 structures
    free(pHeader);
    free(pSkin);
    free(pTexCoord);
    free(pPolygon);
    free(pFrameGroup);

    return result;
}
//---------------------------------------------------------------------------
void miniReleaseMDLModel(MINI_MDLModel* pMDLModel)
{
    unsigned int i;
    unsigned int j;

    // no model to delete?
    if (!pMDLModel)
        return;

    // iterate through meshes and delete each mesh
    for (i = 0; i < pMDLModel->m_FrameCount; ++i)
    {
        if (!pMDLModel->m_pFrame[i].m_pMesh)
            continue;

        // iterate through mesh vertex buffers and delete each vertex buffer
        for (j = 0; j < pMDLModel->m_pFrame[i].m_MeshCount; ++j)
            if (pMDLModel->m_pFrame[i].m_pMesh[j].m_pVertexBuffer)
                free(pMDLModel->m_pFrame[i].m_pMesh[j].m_pVertexBuffer);

        // delete meshes, if exist
        free(pMDLModel->m_pFrame[i].m_pMesh);
    }

    // delete frame list, if exists
    if (pMDLModel->m_pFrame)
        free(pMDLModel->m_pFrame);

    // delete vertex format, if exists
    if (pMDLModel->m_pVertexFormat)
        free(pMDLModel->m_pVertexFormat);

    free(pMDLModel);
}
//---------------------------------------------------------------------------
