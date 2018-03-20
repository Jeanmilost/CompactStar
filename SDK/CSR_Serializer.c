/****************************************************************************
 * ==> CSR_Serializer ------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides the functions required for the        *
 *               serialization of models and scenes                         *
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

#include "CSR_Serializer.h"

//---------------------------------------------------------------------------
// Global defines
//---------------------------------------------------------------------------
#define M_CSR_Signature_Material       "matl"
#define M_CSR_Signature_Texture        "txtr"
#define M_CSR_Signature_BumpMap        "bump"
#define M_CSR_Signature_Vertex_Format  "vtfo"
#define M_CSR_Signature_Vertex_Culling "vtcl"
#define M_CSR_Signature_Vertex_Buffer  "vbuf"
#define M_CSR_Signature_Mesh           "mesh"
//---------------------------------------------------------------------------
// Write functions
//---------------------------------------------------------------------------
int csrSerializerWriteHeader(const char* pID, size_t dataSize, CSR_Buffer* pBuffer)
{
    CSR_SceneFileHeader header;

    // validate the input
    if (!pID)
        return 0;

    // populate the header to write
    memcpy(header.m_ID, pID, 4);
    header.m_HeaderSize = sizeof(CSR_SceneFileHeader);
    header.m_ChunkSize  = header.m_HeaderSize + dataSize;

    // write the header
    if (!csrBufferWrite(pBuffer, &header, header.m_HeaderSize, 1))
        return 0;

    return 1;
}
//---------------------------------------------------------------------------
int csrSerializerWriteMaterial(const CSR_Material* pMaterial, CSR_Buffer* pBuffer)
{
    // validate the input
    if (!pMaterial || !pBuffer)
        return 0;

    // write the header
    if (!csrSerializerWriteHeader(M_CSR_Signature_Material, sizeof(CSR_Material), pBuffer))
        return 0;

    // write the data
    if (!csrBufferWrite(pBuffer, pMaterial, sizeof(CSR_Material), 1))
        return 0;

    return 1;
}
//---------------------------------------------------------------------------
int csrSerializerWriteTexture(const CSR_Buffer* pTexture, CSR_Buffer* pBuffer)
{
    // validate the input
    if (!pTexture || !pBuffer)
        return 0;

    // write the header
    if (!csrSerializerWriteHeader(M_CSR_Signature_Texture, pTexture->m_Length, pBuffer))
        return 0;

    // write the data
    if (!csrBufferWrite(pBuffer, pTexture->m_pData, pTexture->m_Length, 1))
        return 0;

    return 1;
}
//---------------------------------------------------------------------------
int csrSerializerWriteBumpMap(const CSR_Buffer* pBumpMap, CSR_Buffer* pBuffer)
{
    // validate the input
    if (!pBumpMap || !pBuffer)
        return 0;

    // write the header
    if (!csrSerializerWriteHeader(M_CSR_Signature_BumpMap, pBumpMap->m_Length, pBuffer))
        return 0;

    // write the data
    if (!csrBufferWrite(pBuffer, pBumpMap->m_pData, pBumpMap->m_Length, 1))
        return 0;

    return 1;
}
//---------------------------------------------------------------------------
int csrSerializerWriteVF(const CSR_VertexFormat* pVF, CSR_Buffer* pBuffer)
{
    // validate the input
    if (!pVF || !pBuffer)
        return 0;

    // write the header
    if (!csrSerializerWriteHeader(M_CSR_Signature_Vertex_Format, sizeof(CSR_VertexFormat), pBuffer))
        return 0;

    // write the data
    if (!csrBufferWrite(pBuffer, pVF, sizeof(CSR_VertexFormat), 1))
        return 0;

    return 1;
}
//---------------------------------------------------------------------------
int csrSerializerWriteVC(const CSR_VertexCulling* pVC, CSR_Buffer* pBuffer)
{
    // validate the input
    if (!pVC || !pBuffer)
        return 0;

    // write the header
    if (!csrSerializerWriteHeader(M_CSR_Signature_Vertex_Culling, sizeof(CSR_VertexCulling), pBuffer))
        return 0;

    // write the data
    if (!csrBufferWrite(pBuffer, pVC, sizeof(CSR_VertexCulling), 1))
        return 0;

    return 1;
}
//---------------------------------------------------------------------------
int csrSerializerWriteVB(const CSR_VertexBuffer* pVB, CSR_Buffer* pBuffer)
{
    unsigned    dataSize;
    CSR_Buffer* pDataBuffer;

    // validate the input
    if (!pVB || !pBuffer)
        return 0;

    // initialize the local data buffer
    pDataBuffer = csrBufferCreate();

    // write the vertex format
    if (!csrSerializerWriteVF(&pVB->m_Format, pDataBuffer))
    {
        csrBufferRelease(pDataBuffer);
        return 0;
    }

    // write the vertex culling
    if (!csrSerializerWriteVC(&pVB->m_Culling, pDataBuffer))
    {
        csrBufferRelease(pDataBuffer);
        return 0;
    }

    // write the vertex material
    if (!csrSerializerWriteMaterial(&pVB->m_Material, pDataBuffer))
    {
        csrBufferRelease(pDataBuffer);
        return 0;
    }

    // calculate the animation time data size
    dataSize = sizeof(double);

    // write the animation time data size
    if (!csrBufferWrite(pDataBuffer, &dataSize, sizeof(unsigned), 1))
    {
        csrBufferRelease(pDataBuffer);
        return 0;
    }

    // write the animation time
    if (!csrBufferWrite(pDataBuffer, &pVB->m_Time, sizeof(double), 1))
    {
        csrBufferRelease(pDataBuffer);
        return 0;
    }

    // calculate the vertex buffer data size
    dataSize = (unsigned)pVB->m_Count * sizeof(float);

    // write the vertex data size
    if (!csrBufferWrite(pDataBuffer, &dataSize, sizeof(unsigned), 1))
    {
        csrBufferRelease(pDataBuffer);
        return 0;
    }

    // write the vertex data content
    if (!csrBufferWrite(pDataBuffer, pVB->m_pData, sizeof(float), pVB->m_Count))
    {
        csrBufferRelease(pDataBuffer);
        return 0;
    }

    // write the vertex buffer header
    if (!csrSerializerWriteHeader(M_CSR_Signature_Vertex_Buffer, pDataBuffer->m_Length, pBuffer))
    {
        csrBufferRelease(pDataBuffer);
        return 0;
    }

    // write the vertex buffer data content
    if (!csrBufferWrite(pBuffer, pDataBuffer->m_pData, pDataBuffer->m_Length, 1))
    {
        csrBufferRelease(pDataBuffer);
        return 0;
    }

    // release the local data buffer
    csrBufferRelease(pDataBuffer);

    return 1;
}
//---------------------------------------------------------------------------
int csrSerializerWriteMesh(const CSR_Mesh*   pMesh,
                           const CSR_Buffer* pTexture,
                           const CSR_Buffer* pBumpMap,
                                 CSR_Buffer* pBuffer)
{
    size_t      i;
    unsigned    dataSize;
    CSR_Buffer* pDataBuffer;

    // validate the input
    if (!pMesh || !pBuffer)
        return 0;

    // initialize the local data buffer
    pDataBuffer = csrBufferCreate();

    // calculate the animation time data size
    dataSize = sizeof(double);

    // write all the vertex buffers the mesh contains
    for (i = 0; i < pMesh->m_Count; ++i)
        if (!csrSerializerWriteVB(&pMesh->m_pVB[i], pDataBuffer))
        {
            csrBufferRelease(pDataBuffer);
            return 0;
        }

    // write the animation time data size
    if (!csrBufferWrite(pDataBuffer, &dataSize, sizeof(unsigned), 1))
    {
        csrBufferRelease(pDataBuffer);
        return 0;
    }

    // write the animation time
    if (!csrBufferWrite(pDataBuffer, &pMesh->m_Time, sizeof(double), 1))
    {
        csrBufferRelease(pDataBuffer);
        return 0;
    }

    // write the mesh texture
    if (pTexture)
        if (!csrSerializerWriteTexture(pTexture, pBuffer))
        {
            csrBufferRelease(pDataBuffer);
            return 0;
        }

    // write the mesh bumpmap texture
    if (pBumpMap)
        if (!csrSerializerWriteBumpMap(pBumpMap, pBuffer))
        {
            csrBufferRelease(pDataBuffer);
            return 0;
        }

    // write the mesh header
    if (!csrSerializerWriteHeader(M_CSR_Signature_Mesh, pDataBuffer->m_Length, pBuffer))
    {
        csrBufferRelease(pDataBuffer);
        return 0;
    }

    // write mesh data content
    if (!csrBufferWrite(pBuffer, pDataBuffer->m_pData, pDataBuffer->m_Length, 1))
    {
        csrBufferRelease(pDataBuffer);
        return 0;
    }

    // release the local data buffer
    csrBufferRelease(pDataBuffer);

    return 1;
}
//---------------------------------------------------------------------------
