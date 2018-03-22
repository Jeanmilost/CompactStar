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

// std
#include <stdlib.h>

//---------------------------------------------------------------------------
// Global defines
//---------------------------------------------------------------------------
#define M_CSR_Signature_Data           "data"
#define M_CSR_Signature_Color          "colr"
#define M_CSR_Signature_Matrix         "mat4"
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
#define M_CSR_Signature_Matrix_Item    "mati"
#define M_CSR_Signature_Matrix_List    "matl"
#define M_CSR_Signature_Scene_Item     "scit"
#define M_CSR_Signature_Scene          "scne"
//---------------------------------------------------------------------------
// Write functions
//---------------------------------------------------------------------------
int csrSerializerWriteHeader(const char*       pID,
                                   size_t      dataSize,
                                   unsigned    options,
                                   CSR_Buffer* pBuffer)
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
int csrSerializerWriteData(const CSR_Buffer* pData, CSR_EDataType type, CSR_Buffer* pBuffer)
{
    // validate the input
    if (!pData || !pBuffer)
        return 0;

    // is data empty?
    if (!pData->m_pData || !pData->m_Length)
        return 1;

    // write the header
    if (!csrSerializerWriteHeader(M_CSR_Signature_Data,
                                  pData->m_Length,
                                  type,
                                  pBuffer))
        return 0;

    // write the data
    if (!csrBufferWrite(pBuffer, pData->m_pData, pData->m_Length, 1))
        return 0;

    return 1;
}
//---------------------------------------------------------------------------
int csrSerializerWriteColor(const CSR_Color* pColor, CSR_Buffer* pBuffer)
{
    // validate the input
    if (!pColor || !pBuffer)
        return 0;

    // write the header
    if (!csrSerializerWriteHeader(M_CSR_Signature_Color,
                                  sizeof(CSR_Color),
                                  CSR_HO_None,
                                  pBuffer))
        return 0;

    // write the data
    if (!csrBufferWrite(pBuffer, pColor, sizeof(CSR_Color), 1))
        return 0;

    return 1;
}
//---------------------------------------------------------------------------
int csrSerializerWriteMatrix(const CSR_Matrix4* pMatrix, CSR_Buffer* pBuffer)
{
    // validate the input
    if (!pMatrix || !pBuffer)
        return 0;

    // write the header
    if (!csrSerializerWriteHeader(M_CSR_Signature_Matrix,
                                  sizeof(CSR_Matrix4),
                                  CSR_HO_None,
                                  pBuffer))
        return 0;

    // write the data
    if (!csrBufferWrite(pBuffer, pMatrix, sizeof(CSR_Matrix4), 1))
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
    if (!csrSerializerWriteHeader(M_CSR_Signature_Material,
                                  sizeof(CSR_Material),
                                  CSR_HO_None,
                                  pBuffer))
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
    if (!csrSerializerWriteHeader(M_CSR_Signature_Texture,
                                  pTexture->m_Length,
                                  CSR_HO_None,
                                  pBuffer))
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
    if (!csrSerializerWriteHeader(M_CSR_Signature_BumpMap,
                                  pBumpMap->m_Length,
                                  CSR_HO_None,
                                  pBuffer))
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
    if (!csrSerializerWriteHeader(M_CSR_Signature_Vertex_Format,
                                  sizeof(CSR_VertexFormat),
                                  CSR_HO_None,
                                  pBuffer))
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
    if (!csrSerializerWriteHeader(M_CSR_Signature_Vertex_Culling,
                                  sizeof(CSR_VertexCulling),
                                  CSR_HO_None,
                                  pBuffer))
        return 0;

    // write the data
    if (!csrBufferWrite(pBuffer, pVC, sizeof(CSR_VertexCulling), 1))
        return 0;

    return 1;
}
//---------------------------------------------------------------------------
int csrSerializerWriteVB(const CSR_VertexBuffer* pVB, CSR_Buffer* pBuffer)
{
    CSR_Buffer* pChunkBuffer;
    CSR_Buffer* pDataBuffer;

    // validate the input
    if (!pVB || !pBuffer)
        return 0;

    // initialize the local chunk buffer
    pChunkBuffer = csrBufferCreate();

    // write the vertex format
    if (!csrSerializerWriteVF(&pVB->m_Format, pChunkBuffer))
    {
        csrBufferRelease(pChunkBuffer);
        return 0;
    }

    // write the vertex culling
    if (!csrSerializerWriteVC(&pVB->m_Culling, pChunkBuffer))
    {
        csrBufferRelease(pChunkBuffer);
        return 0;
    }

    // write the vertex material
    if (!csrSerializerWriteMaterial(&pVB->m_Material, pChunkBuffer))
    {
        csrBufferRelease(pChunkBuffer);
        return 0;
    }

    // initialize a buffer to write the standalone data
    pDataBuffer = csrBufferCreate();

    // prepare the buffer to write the animation time
    pDataBuffer->m_Length = sizeof(double);
    pDataBuffer->m_pData  = malloc(pDataBuffer->m_Length);

    // succeeded?
    if (!pDataBuffer->m_pData)
    {
        csrBufferRelease(pDataBuffer);
        csrBufferRelease(pChunkBuffer);
        return 0;
    }

    // copy the data to write
    memcpy(pDataBuffer->m_pData, &pVB->m_Time, pDataBuffer->m_Length);

    // write the animation time data
    if (!csrSerializerWriteData(pDataBuffer, CSR_DT_TimeStamp, pChunkBuffer))
    {
        csrBufferRelease(pDataBuffer);
        csrBufferRelease(pChunkBuffer);
        return 0;
    }

    // release the buffer content
    free(pDataBuffer->m_pData);

    // prepare the buffer to write the vertices data content
    pDataBuffer->m_Length = (unsigned)pVB->m_Count * sizeof(float);
    pDataBuffer->m_pData  = malloc(pDataBuffer->m_Length);

    // succeeded?
    if (!pDataBuffer->m_pData)
    {
        csrBufferRelease(pDataBuffer);
        csrBufferRelease(pChunkBuffer);
        return 0;
    }

    // copy the data to write
    memcpy(pDataBuffer->m_pData, pVB->m_pData, pDataBuffer->m_Length);

    // write the animation time data
    if (!csrSerializerWriteData(pDataBuffer, CSR_DT_Vertices, pChunkBuffer))
    {
        csrBufferRelease(pDataBuffer);
        csrBufferRelease(pChunkBuffer);
        return 0;
    }

    // release the buffer
    csrBufferRelease(pDataBuffer);

    // write the vertex buffer header
    if (!csrSerializerWriteHeader(M_CSR_Signature_Vertex_Buffer,
                                  pChunkBuffer->m_Length,
                                  CSR_HO_None,
                                  pBuffer))
    {
        csrBufferRelease(pChunkBuffer);
        return 0;
    }

    // write the vertex buffer data content
    if (!csrBufferWrite(pBuffer, pChunkBuffer->m_pData, pChunkBuffer->m_Length, 1))
    {
        csrBufferRelease(pChunkBuffer);
        return 0;
    }

    // release the local data buffer
    csrBufferRelease(pChunkBuffer);

    return 1;
}
//---------------------------------------------------------------------------
int csrSerializerWriteMesh(const CSR_Mesh*         pMesh,
                                 CSR_Buffer*       pBuffer,
                           const CSR_fOnGetTexture fOnGetTexture,
                           const CSR_fOnGetBumpMap fOnGetBumpMap)
{
    size_t      i;
    CSR_Buffer* pChunkBuffer;
    CSR_Buffer* pDataBuffer;

    // validate the input
    if (!pMesh || !pBuffer)
        return 0;

    // initialize the local chunk buffer
    pChunkBuffer = csrBufferCreate();

    // do write texture?
    if (fOnGetTexture)
    {
        // create a new buffer for the texture
        CSR_Buffer* pTextureBuffer = csrBufferCreate();

        // get the texture to write
        if (fOnGetTexture(pMesh, 0, pTextureBuffer) && pTextureBuffer->m_Length)
            // write the mesh texture
            if (!csrSerializerWriteTexture(pTextureBuffer, pChunkBuffer))
            {
                csrBufferRelease(pTextureBuffer);
                csrBufferRelease(pChunkBuffer);
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
            if (!csrSerializerWriteTexture(pBumpMapBuffer, pChunkBuffer))
            {
                csrBufferRelease(pBumpMapBuffer);
                csrBufferRelease(pChunkBuffer);
                return 0;
            }

        // free the bumpmap buffer
        csrBufferRelease(pBumpMapBuffer);
    }

    // write all the vertex buffers the mesh contains
    for (i = 0; i < pMesh->m_Count; ++i)
        if (!csrSerializerWriteVB(&pMesh->m_pVB[i], pChunkBuffer))
        {
            csrBufferRelease(pChunkBuffer);
            return 0;
        }

    // initialize a buffer to write the standalone data
    pDataBuffer = csrBufferCreate();

    // prepare the buffer to write the animation time
    pDataBuffer->m_Length = sizeof(double);
    pDataBuffer->m_pData  = malloc(pDataBuffer->m_Length);

    // succeeded?
    if (!pDataBuffer->m_pData)
    {
        csrBufferRelease(pDataBuffer);
        csrBufferRelease(pChunkBuffer);
        return 0;
    }

    // copy the data to write
    memcpy(pDataBuffer->m_pData, &pMesh->m_Time, pDataBuffer->m_Length);

    // write the animation time data
    if (!csrSerializerWriteData(pDataBuffer, CSR_DT_TimeStamp, pChunkBuffer))
    {
        csrBufferRelease(pDataBuffer);
        csrBufferRelease(pChunkBuffer);
        return 0;
    }

    // release the buffer
    csrBufferRelease(pDataBuffer);

    // write the mesh header
    if (!csrSerializerWriteHeader(M_CSR_Signature_Mesh,
                                  pChunkBuffer->m_Length,
                                  CSR_HO_None,
                                  pBuffer))
    {
        csrBufferRelease(pChunkBuffer);
        return 0;
    }

    // write mesh data content
    if (!csrBufferWrite(pBuffer, pChunkBuffer->m_pData, pChunkBuffer->m_Length, 1))
    {
        csrBufferRelease(pChunkBuffer);
        return 0;
    }

    // release the local data buffer
    csrBufferRelease(pChunkBuffer);

    return 1;
}
//---------------------------------------------------------------------------
int csrSerializerWriteModel(const CSR_Model*        pModel,
                                  CSR_Buffer*       pBuffer,
                            const CSR_fOnGetTexture fOnGetTexture,
                            const CSR_fOnGetBumpMap fOnGetBumpMap)
{
    size_t      i;
    CSR_Buffer* pChunkBuffer;
    CSR_Buffer* pDataBuffer;

    // validate the input
    if (!pModel || !pBuffer)
        return 0;

    // initialize the local chunk buffer
    pChunkBuffer = csrBufferCreate();

    // do write texture?
    if (fOnGetTexture)
    {
        // create a new buffer for the texture
        CSR_Buffer* pTextureBuffer = csrBufferCreate();

        // get the texture to write
        if (fOnGetTexture(pModel, 0, pTextureBuffer) && pTextureBuffer->m_Length)
            // write the mesh texture
            if (!csrSerializerWriteTexture(pTextureBuffer, pChunkBuffer))
            {
                csrBufferRelease(pTextureBuffer);
                csrBufferRelease(pChunkBuffer);
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
            if (!csrSerializerWriteTexture(pBumpMapBuffer, pChunkBuffer))
            {
                csrBufferRelease(pBumpMapBuffer);
                csrBufferRelease(pChunkBuffer);
                return 0;
            }

        // free the bumpmap buffer
        csrBufferRelease(pBumpMapBuffer);
    }

    // write all the meshes the model contains
    for (i = 0; i < pModel->m_MeshCount; ++i)
        if (!csrSerializerWriteMesh(&pModel->m_pMesh[i], pChunkBuffer, fOnGetTexture, fOnGetBumpMap))
        {
            csrBufferRelease(pChunkBuffer);
            return 0;
        }

    // initialize a buffer to write the standalone data
    pDataBuffer = csrBufferCreate();

    // prepare the buffer to write the animation time
    pDataBuffer->m_Length = sizeof(double);
    pDataBuffer->m_pData  = malloc(pDataBuffer->m_Length);

    // succeeded?
    if (!pDataBuffer->m_pData)
    {
        csrBufferRelease(pDataBuffer);
        csrBufferRelease(pChunkBuffer);
        return 0;
    }

    // copy the data to write
    memcpy(pDataBuffer->m_pData, &pModel->m_Time, pDataBuffer->m_Length);

    // write the animation time data
    if (!csrSerializerWriteData(pDataBuffer, CSR_DT_TimeStamp, pChunkBuffer))
    {
        csrBufferRelease(pDataBuffer);
        csrBufferRelease(pChunkBuffer);
        return 0;
    }

    // release the buffer
    csrBufferRelease(pDataBuffer);

    // write the model header
    if (!csrSerializerWriteHeader(M_CSR_Signature_Model,
                                  pChunkBuffer->m_Length,
                                  CSR_HO_None,
                                  pBuffer))
    {
        csrBufferRelease(pChunkBuffer);
        return 0;
    }

    // write model data content
    if (!csrBufferWrite(pBuffer, pChunkBuffer->m_pData, pChunkBuffer->m_Length, 1))
    {
        csrBufferRelease(pChunkBuffer);
        return 0;
    }

    // release the local data buffer
    csrBufferRelease(pChunkBuffer);

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
    if (!csrSerializerWriteHeader(M_CSR_Signature_Model_Anim,
                                  sizeof(CSR_ModelAnimation),
                                  CSR_HO_None,
                                  pBuffer))
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
    CSR_Buffer* pChunkBuffer;

    // validate the input
    if (!pMDL || !pBuffer)
        return 0;

    // initialize the local chunk buffer
    pChunkBuffer = csrBufferCreate();

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
                if (!csrSerializerWriteTexture(pTextureBuffer, pChunkBuffer))
                {
                    csrBufferRelease(pTextureBuffer);
                    csrBufferRelease(pChunkBuffer);
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
                if (!csrSerializerWriteTexture(pBumpMapBuffer, pChunkBuffer))
                {
                    csrBufferRelease(pBumpMapBuffer);
                    csrBufferRelease(pChunkBuffer);
                    return 0;
                }

            // free the bumpmap buffer
            csrBufferRelease(pBumpMapBuffer);
        }
    }

    // write all the animations the MDL contains
    for (i = 0; i < pMDL->m_AnimationCount; ++i)
        if (!csrSerializerWriteModelAnimation(&pMDL->m_pAnimation[i], pChunkBuffer))
        {
            csrBufferRelease(pChunkBuffer);
            return 0;
        }

    // write all the models the MDL contains
    for (i = 0; i < pMDL->m_ModelCount; ++i)
        if (!csrSerializerWriteModel(&pMDL->m_pModel[i], pChunkBuffer, fOnGetTexture, fOnGetBumpMap))
        {
            csrBufferRelease(pChunkBuffer);
            return 0;
        }

    // write the MDL header
    if (!csrSerializerWriteHeader(M_CSR_Signature_MDL,
                                  pChunkBuffer->m_Length,
                                  CSR_HO_None,
                                  pBuffer))
    {
        csrBufferRelease(pChunkBuffer);
        return 0;
    }

    // write MDL data content
    if (!csrBufferWrite(pBuffer, pChunkBuffer->m_pData, pChunkBuffer->m_Length, 1))
    {
        csrBufferRelease(pChunkBuffer);
        return 0;
    }

    // release the local data buffer
    csrBufferRelease(pChunkBuffer);

    return 1;
}
//---------------------------------------------------------------------------
int csrSerializerWriteMatrixItem(const CSR_MatrixItem* pMatrixItem, CSR_Buffer* pBuffer)
{
    // validate the input
    if (!pMatrixItem || !pBuffer)
        return 0;

    // write the header
    if (!csrSerializerWriteHeader(M_CSR_Signature_Matrix_Item,
                                  sizeof(CSR_Matrix4),
                                  CSR_HO_None,
                                  pBuffer))
        return 0;

    // write the data
    if (!csrBufferWrite(pBuffer, pMatrixItem->m_pMatrix, sizeof(CSR_Matrix4), 1))
        return 0;

    return 1;
}
//---------------------------------------------------------------------------
int csrSerializerWriteMatrixList(const CSR_MatrixList* pMatrixList, CSR_Buffer* pBuffer)
{
    size_t      i;
    CSR_Buffer* pChunkBuffer;

    // validate the input
    if (!pMatrixList || !pBuffer)
        return 0;

    // initialize the local chunk buffer
    pChunkBuffer = csrBufferCreate();

    // write all the matrix items the list contains
    for (i = 0; i < pMatrixList->m_Count; ++i)
        if (!csrSerializerWriteMatrixItem(&pMatrixList->m_pItem[i], pChunkBuffer))
        {
            csrBufferRelease(pChunkBuffer);
            return 0;
        }

    // write the matrix list header
    if (!csrSerializerWriteHeader(M_CSR_Signature_Matrix_List,
                                  pChunkBuffer->m_Length,
                                  CSR_HO_None,
                                  pBuffer))
    {
        csrBufferRelease(pChunkBuffer);
        return 0;
    }

    // write mesh data content
    if (!csrBufferWrite(pBuffer, pChunkBuffer->m_pData, pChunkBuffer->m_Length, 1))
    {
        csrBufferRelease(pChunkBuffer);
        return 0;
    }

    // release the local data buffer
    csrBufferRelease(pChunkBuffer);

    return 1;
}
//---------------------------------------------------------------------------
int csrSerializerWriteSceneItem(const CSR_SceneItem*    pSceneItem,
                                      int               transparent,
                                      CSR_Buffer*       pBuffer,
                                const CSR_fOnGetTexture fOnGetTexture,
                                const CSR_fOnGetBumpMap fOnGetBumpMap)
{
    CSR_Buffer*           pChunkBuffer;
    CSR_ESceneItemOptions options;

    // validate the input
    if (!pSceneItem || !pBuffer)
        return 0;

    // initialize the local chunk buffer
    pChunkBuffer = csrBufferCreate();

    // write the model
    switch (pSceneItem->m_Type)
    {
        case CSR_MT_Mesh:
            options = CSR_SO_ModelType_Mesh;

            // write the mesh
            if (!csrSerializerWriteMesh((CSR_Mesh*)pSceneItem->m_pModel,
                                                   pChunkBuffer,
                                                   fOnGetTexture,
                                                   fOnGetBumpMap))
            {
                csrBufferRelease(pChunkBuffer);
                return 0;
            }

            break;

        case CSR_MT_Model:
            options = CSR_SO_ModelType_Model;

            // write the model
            if (!csrSerializerWriteModel((CSR_Model*)pSceneItem->m_pModel,
                                                     pChunkBuffer,
                                                     fOnGetTexture,
                                                     fOnGetBumpMap))
            {
                csrBufferRelease(pChunkBuffer);
                return 0;
            }

            break;

        case CSR_MT_MDL:
            options = CSR_SO_ModelType_MDL;

            // write the MDL model
            if (!csrSerializerWriteMDL((CSR_MDL*)pSceneItem->m_pModel,
                                                 pChunkBuffer,
                                                 fOnGetTexture,
                                                 fOnGetBumpMap))
            {
                csrBufferRelease(pChunkBuffer);
                return 0;
            }

            break;

        default:
            csrBufferRelease(pChunkBuffer);
            return 0;
    }

    // write the scene item matrix list
    if (pSceneItem->m_pMatrixList)
        if (!csrSerializerWriteMatrixList(pSceneItem->m_pMatrixList, pChunkBuffer))
        {
            csrBufferRelease(pChunkBuffer);
            return 0;
        }

    // set the transparent option if needed
    if (transparent)
        options |= CSR_SO_Transparent;

    // set the do generate AABB tree if the scene item contains one
    if (pSceneItem->m_pAABBTree)
        options |= CSR_SO_DoGenerateAABB;

    // write the scene item header
    if (!csrSerializerWriteHeader(M_CSR_Signature_Scene_Item,
                                  pChunkBuffer->m_Length,
                                  options,
                                  pBuffer))
    {
        csrBufferRelease(pChunkBuffer);
        return 0;
    }

    // write scene item data content
    if (!csrBufferWrite(pBuffer, pChunkBuffer->m_pData, pChunkBuffer->m_Length, 1))
    {
        csrBufferRelease(pChunkBuffer);
        return 0;
    }

    // release the local data buffer
    csrBufferRelease(pChunkBuffer);

    return 1;
}
//---------------------------------------------------------------------------
int csrSerializerWriteScene(const CSR_Scene*        pScene,
                                  CSR_Buffer*       pBuffer,
                            const CSR_fOnGetTexture fOnGetTexture,
                            const CSR_fOnGetBumpMap fOnGetBumpMap)
{
    size_t             i;
    CSR_Buffer*        pChunkBuffer;
    CSR_EHeaderOptions options;

    // validate the input
    if (!pScene || !pBuffer)
        return 0;

    // initialize the local chunk buffer
    pChunkBuffer = csrBufferCreate();

    // write the scene color
    if (!csrSerializerWriteColor(&pScene->m_Color, pChunkBuffer))
    {
        csrBufferRelease(pChunkBuffer);
        return 0;
    }

    // write the scene matrix
    if (!csrSerializerWriteMatrix(&pScene->m_Matrix, pChunkBuffer))
    {
        csrBufferRelease(pChunkBuffer);
        return 0;
    }

    // write all the items the scene contains
    for (i = 0; i < pScene->m_ItemCount; ++i)
        if (!csrSerializerWriteSceneItem(&pScene->m_pItem[i],
                                          0,
                                          pChunkBuffer,
                                          fOnGetTexture,
                                          fOnGetBumpMap))
        {
            csrBufferRelease(pChunkBuffer);
            return 0;
        }

    // write all the transparent items the scene contains
    for (i = 0; i < pScene->m_TransparentItemCount; ++i)
        if (!csrSerializerWriteSceneItem(&pScene->m_pTransparentItem[i],
                                          1,
                                          pChunkBuffer,
                                          fOnGetTexture,
                                          fOnGetBumpMap))
        {
            csrBufferRelease(pChunkBuffer);
            return 0;
        }

    // write the scene header
    if (!csrSerializerWriteHeader(M_CSR_Signature_Scene,
                                  pChunkBuffer->m_Length,
                                  CSR_HO_None,
                                  pBuffer))
    {
        csrBufferRelease(pChunkBuffer);
        return 0;
    }

    // write scene data content
    if (!csrBufferWrite(pBuffer, pChunkBuffer->m_pData, pChunkBuffer->m_Length, 1))
    {
        csrBufferRelease(pChunkBuffer);
        return 0;
    }

    // release the local data buffer
    csrBufferRelease(pChunkBuffer);

    return 1;
}
//---------------------------------------------------------------------------
