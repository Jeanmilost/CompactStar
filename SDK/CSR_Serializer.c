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
#define M_CSR_Signature_Model          "modl"
#define M_CSR_Signature_Model_Anim     "mani"
#define M_CSR_Signature_MDL            "mdlm"
//---------------------------------------------------------------------------
// Write functions
//---------------------------------------------------------------------------
int csrSerializerWriteHeader(const char* pID, size_t dataSize, unsigned options, CSR_Buffer* pBuffer)
{
    CSR_SceneFileHeader header;

    // validate the input
    if (!pID)
        return 0;

    // populate the header to write
    memcpy(header.m_ID, pID, 4);
    header.m_HeaderSize = sizeof(CSR_SceneFileHeader);
    header.m_ChunkSize  = header.m_HeaderSize + dataSize;
    header.m_Options    = options;

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
    if (!csrSerializerWriteHeader(M_CSR_Signature_Material, sizeof(CSR_Material), 0, pBuffer))
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
    if (!csrSerializerWriteHeader(M_CSR_Signature_Texture, pTexture->m_Length, 0, pBuffer))
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
    if (!csrSerializerWriteHeader(M_CSR_Signature_BumpMap, pBumpMap->m_Length, 0, pBuffer))
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
    if (!csrSerializerWriteHeader(M_CSR_Signature_Vertex_Format, sizeof(CSR_VertexFormat), 0, pBuffer))
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
    if (!csrSerializerWriteHeader(M_CSR_Signature_Vertex_Culling, sizeof(CSR_VertexCulling), 0, pBuffer))
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
    if (!csrSerializerWriteHeader(M_CSR_Signature_Vertex_Buffer, pDataBuffer->m_Length, 0, pBuffer))
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
int csrSerializerWriteMesh(const CSR_Mesh*         pMesh,
                                 CSR_Buffer*       pBuffer,
                           const CSR_fOnGetTexture fOnGetTexture,
                           const CSR_fOnGetBumpMap fOnGetBumpMap)
{
    size_t      i;
    unsigned    dataSize;
    CSR_Buffer* pDataBuffer;

    // validate the input
    if (!pMesh || !pBuffer)
        return 0;

    // initialize the local data buffer
    pDataBuffer = csrBufferCreate();

    // do write texture?
    if (fOnGetTexture)
    {
        // create a new buffer for the texture
        CSR_Buffer* pTextureBuffer = csrBufferCreate();

        // get the texture to write
        if (fOnGetTexture(pMesh, 0, pTextureBuffer) && pTextureBuffer->m_Length)
            // write the mesh texture
            if (!csrSerializerWriteTexture(pTextureBuffer, pDataBuffer))
            {
                csrBufferRelease(pTextureBuffer);
                csrBufferRelease(pDataBuffer);
                return 0;
            }

        // free the texture buffer
        csrBufferRelease(pTextureBuffer);
    }

    // do write bumpmap?
    if (fOnGetBumpMap)
    {
        // create a new buffer for the bumpmap
        CSR_Buffer* pBumpMapBuffer = csrBufferCreate();

        // get the bumpmap to write
        if (fOnGetBumpMap(pMesh, 0, pBumpMapBuffer) && pBumpMapBuffer->m_Length)
            // write the mesh bumpmap
            if (!csrSerializerWriteTexture(pBumpMapBuffer, pDataBuffer))
            {
                csrBufferRelease(pBumpMapBuffer);
                csrBufferRelease(pDataBuffer);
                return 0;
            }

        // free the bumpmap buffer
        csrBufferRelease(pBumpMapBuffer);
    }

    // write all the vertex buffers the mesh contains
    for (i = 0; i < pMesh->m_Count; ++i)
        if (!csrSerializerWriteVB(&pMesh->m_pVB[i], pDataBuffer))
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
    if (!csrBufferWrite(pDataBuffer, &pMesh->m_Time, sizeof(double), 1))
    {
        csrBufferRelease(pDataBuffer);
        return 0;
    }

    // write the mesh header
    if (!csrSerializerWriteHeader(M_CSR_Signature_Mesh, pDataBuffer->m_Length, 0, pBuffer))
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
int csrSerializerWriteModel(const CSR_Model*        pModel,
                                  CSR_Buffer*       pBuffer,
                            const CSR_fOnGetTexture fOnGetTexture,
                            const CSR_fOnGetBumpMap fOnGetBumpMap)
{
    size_t      i;
    unsigned    dataSize;
    CSR_Buffer* pDataBuffer;

    // validate the input
    if (!pModel || !pBuffer)
        return 0;

    // initialize the local data buffer
    pDataBuffer = csrBufferCreate();

    // do write texture?
    if (fOnGetTexture)
    {
        // create a new buffer for the texture
        CSR_Buffer* pTextureBuffer = csrBufferCreate();

        // get the texture to write
        if (fOnGetTexture(pModel, 0, pTextureBuffer) && pTextureBuffer->m_Length)
            // write the mesh texture
            if (!csrSerializerWriteTexture(pTextureBuffer, pDataBuffer))
            {
                csrBufferRelease(pTextureBuffer);
                csrBufferRelease(pDataBuffer);
                return 0;
            }

        // free the texture buffer
        csrBufferRelease(pTextureBuffer);
    }

    // do write bumpmap?
    if (fOnGetBumpMap)
    {
        // create a new buffer for the bumpmap
        CSR_Buffer* pBumpMapBuffer = csrBufferCreate();

        // get the bumpmap to write
        if (fOnGetBumpMap(pModel, 0, pBumpMapBuffer) && pBumpMapBuffer->m_Length)
            // write the mesh bumpmap
            if (!csrSerializerWriteTexture(pBumpMapBuffer, pDataBuffer))
            {
                csrBufferRelease(pBumpMapBuffer);
                csrBufferRelease(pDataBuffer);
                return 0;
            }

        // free the bumpmap buffer
        csrBufferRelease(pBumpMapBuffer);
    }

    // write all the meshes the model contains
    for (i = 0; i < pModel->m_MeshCount; ++i)
        if (!csrSerializerWriteMesh(&pModel->m_pMesh[i], pDataBuffer, fOnGetTexture, fOnGetBumpMap))
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
    if (!csrBufferWrite(pDataBuffer, &pModel->m_Time, sizeof(double), 1))
    {
        csrBufferRelease(pDataBuffer);
        return 0;
    }

    // write the model header
    if (!csrSerializerWriteHeader(M_CSR_Signature_Model, pDataBuffer->m_Length, 0, pBuffer))
    {
        csrBufferRelease(pDataBuffer);
        return 0;
    }

    // write model data content
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
int csrSerializerWriteModelAnimation(const CSR_ModelAnimation* pModelAnim,
                                           CSR_Buffer*         pBuffer)
{
    // validate the input
    if (!pModelAnim || !pBuffer)
        return 0;

    // write the header
    if (!csrSerializerWriteHeader(M_CSR_Signature_Model_Anim, sizeof(CSR_VertexCulling), 0, pBuffer))
        return 0;

    // write the data
    if (!csrBufferWrite(pBuffer, pModelAnim, sizeof(CSR_ModelAnimation), 1))
        return 0;

    return 1;
}
//---------------------------------------------------------------------------
int csrSerializerWriteMDL(const CSR_MDL*          pMDL,
                                CSR_Buffer*       pBuffer,
                          const CSR_fOnGetTexture fOnGetTexture,
                          const CSR_fOnGetBumpMap fOnGetBumpMap)
{
    size_t      i;
    CSR_Buffer* pDataBuffer;

    // validate the input
    if (!pMDL || !pBuffer)
        return 0;

    // initialize the local data buffer
    pDataBuffer = csrBufferCreate();

    for (i = 0; i < pMDL->m_TextureCount; ++i)
    {
        // do write texture?
        if (fOnGetTexture)
        {
            // create a new buffer for the texture
            CSR_Buffer* pTextureBuffer = csrBufferCreate();

            // get the texture to write
            if (fOnGetTexture(pMDL, i, pTextureBuffer) && pTextureBuffer->m_Length)
                // write the mesh texture
                if (!csrSerializerWriteTexture(pTextureBuffer, pDataBuffer))
                {
                    csrBufferRelease(pTextureBuffer);
                    csrBufferRelease(pDataBuffer);
                    return 0;
                }

            // free the texture buffer
            csrBufferRelease(pTextureBuffer);
        }

        // do write bumpmap?
        if (fOnGetBumpMap)
        {
            // create a new buffer for the bumpmap
            CSR_Buffer* pBumpMapBuffer = csrBufferCreate();

            // get the bumpmap to write
            if (fOnGetBumpMap(pMDL, i, pBumpMapBuffer) && pBumpMapBuffer->m_Length)
                // write the mesh bumpmap
                if (!csrSerializerWriteTexture(pBumpMapBuffer, pDataBuffer))
                {
                    csrBufferRelease(pBumpMapBuffer);
                    csrBufferRelease(pDataBuffer);
                    return 0;
                }

            // free the bumpmap buffer
            csrBufferRelease(pBumpMapBuffer);
        }
    }

    // write all the animations the MDL contains
    for (i = 0; i < pMDL->m_AnimationCount; ++i)
        if (!csrSerializerWriteModelAnimation(&pMDL->m_pAnimation[i], pDataBuffer))
        {
            csrBufferRelease(pDataBuffer);
            return 0;
        }

    // write all the models the MDL contains
    for (i = 0; i < pMDL->m_ModelCount; ++i)
        if (!csrSerializerWriteModel(&pMDL->m_pModel[i], pDataBuffer, fOnGetTexture, fOnGetBumpMap))
        {
            csrBufferRelease(pDataBuffer);
            return 0;
        }

    // write the MDL header
    if (!csrSerializerWriteHeader(M_CSR_Signature_MDL, pDataBuffer->m_Length, 0, pBuffer))
    {
        csrBufferRelease(pDataBuffer);
        return 0;
    }

    // write MDL data content
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
