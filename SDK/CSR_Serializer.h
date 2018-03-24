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

#ifndef CSR_SerializerH
#define CSR_SerializerH

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Geometry.h"
#include "CSR_Vertex.h"
#include "CSR_Model.h"
#include "CSR_Renderer.h"

//---------------------------------------------------------------------------
// Enumerators
//---------------------------------------------------------------------------

/**
* Global header options
*/
typedef enum
{
    CSR_HO_None = 0x0,
} CSR_EHeaderOptions;

/**
* Scene file header options
*/
typedef enum
{
    CSR_SO_None            = 0x0,
    CSR_SO_ModelType_Mesh  = 0x1,
    CSR_SO_ModelType_Model = 0x2,
    CSR_SO_ModelType_MDL   = 0x4,
    CSR_SO_DoGenerateAABB  = 0x8,
    CSR_SO_Transparent     = 0x10
} CSR_ESceneItemOptions;

/**
* Scene file data type
*/
typedef enum
{
    CSR_DT_Unknown = 0,
    CSR_DT_Vertices,
    CSR_DT_TimeStamp,
    CSR_DT_ShaderIndex,
    CSR_DT_TextureIndex,
    CSR_DT_BumpMapIndex
} CSR_EDataType;

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

// Write context prototype
typedef struct CSR_WriteContext CSR_WriteContext;

//---------------------------------------------------------------------------
// Structures
//---------------------------------------------------------------------------

/**
* Scene file header
*/
typedef struct
{
    char     m_ID[4];      // chunk identifier
    unsigned m_HeaderSize; // size of the header, in bytes
    unsigned m_ChunkSize;  // size of the chunk (i.e. header + content), in bytes
    unsigned m_Options;    // chunk options (depend of each chunk)
} CSR_SceneFileHeader;

//---------------------------------------------------------------------------
// Callbacks
//---------------------------------------------------------------------------

/**
* Called when a model should receive a texture index to save
*@param pModel - model for which the texture index should be get
*@param index - model texture index, in case the model contains several textures
*@param bumpMap - if 1, the needed texture should be a bump map, normal texture if 0
*@return texture index from a referenced texture list, if -1 no texture will be linked
*/
typedef int (*CSR_fOnGetTextureIndex)(const void* pModel, size_t index, int bumpMap);

/**
* Called when a model should receive a shader index to save
*@param pModel - model for which the shader index should be get
*@return shader index from a referenced shader list, if -1 no shader will be linked
*/
typedef int (*CSR_fOnGetShaderIndex)(const void* pModel);

//---------------------------------------------------------------------------
// Implementation
//---------------------------------------------------------------------------

/**
* Write context
*/
struct CSR_WriteContext
{
    CSR_fOnGetTextureIndex m_fOnGetTextureIndex;
    CSR_fOnGetShaderIndex  m_fOnGetShaderIndex;
};

#ifdef __cplusplus
    extern "C"
    {
#endif
        //-------------------------------------------------------------------
        // Write functions
        //-------------------------------------------------------------------

        /**
        * Writes a header in the buffer
        *@param pContext - write context, containing the write options
        *@param pID - header identifier, must always be a string containing 4 chars
        *@param dataSize - size of data the chunk will contain, in bytes
        *@param options - header options
        *@param[in, out] pBuffer - buffer to write in
        */
        int csrSerializerWriteHeader(const CSR_WriteContext* pContext,
                                     const char*             pID,
                                           size_t            dataSize,
                                           unsigned          options,
                                           CSR_Buffer*       pBuffer);

        /**
        * Writes a data inside a buffer
        *@param pContext - write context, containing the write options
        *@param pData - data to write
        *@param type - data type
        *@param[in, out] pBuffer - buffer to write in
        *@return 1 on success, otherwise 0
        */
        int csrSerializerWriteData(const CSR_WriteContext* pContext,
                                   const CSR_Buffer*       pData,
                                         CSR_EDataType     type,
                                         CSR_Buffer*       pBuffer);

        /**
        * Writes a color inside a buffer
        *@param pContext - write context, containing the write options
        *@param pColor - color to write
        *@param[in, out] pBuffer - buffer to write in
        *@return 1 on success, otherwise 0
        */
        int csrSerializerWriteColor(const CSR_WriteContext* pContext,
                                    const CSR_Color*        pColor,
                                          CSR_Buffer*       pBuffer);

        /**
        * Writes a matrix inside a buffer
        *@param pContext - write context, containing the write options
        *@param pMatrix - matrix to write
        *@param[in, out] pBuffer - buffer to write in
        *@return 1 on success, otherwise 0
        */
        int csrSerializerWriteMatrix(const CSR_WriteContext* pContext,
                                     const CSR_Matrix4*      pMatrix,
                                           CSR_Buffer*       pBuffer);

        /**
        * Writes a material inside a buffer
        *@param pContext - write context, containing the write options
        *@param pMaterial - material to write
        *@param[in, out] pBuffer - buffer to write in
        *@return 1 on success, otherwise 0
        */
        int csrSerializerWriteMaterial(const CSR_WriteContext* pContext,
                                       const CSR_Material*     pMaterial,
                                             CSR_Buffer*       pBuffer);

        /**
        * Writes a texture inside a buffer
        *@param pContext - write context, containing the write options
        *@param pTexture - texture to write
        *@param[in, out] pBuffer - buffer to write in
        *@return 1 on success, otherwise 0
        */
        int csrSerializerWriteTexture(const CSR_WriteContext* pContext,
                                      const CSR_Buffer*       pTexture,
                                            CSR_Buffer*       pBuffer);

        /**
        * Writes a bump map inside a buffer
        *@param pContext - write context, containing the write options
        *@param pBumpMap - bump map texture to write
        *@param[in, out] pBuffer - buffer to write in
        *@return 1 on success, otherwise 0
        */
        int csrSerializerWriteBumpMap(const CSR_WriteContext* pContext,
                                      const CSR_Buffer*       pBumpMap,
                                            CSR_Buffer*       pBuffer);

        /**
        * Writes the dependencies (texture, shader, ...) a model requires
        *@param pContext - write context, containing the write options
        *@param pModel - model for which the links should be written
        *@param index - MDL texture index, always 0 for other model types
        *@param[in, out] pBuffer - buffer to write in
        *@return 1 on success, otherwise 0
        */
        int csrSerializerWriteModelDependencies(const CSR_WriteContext* pContext,
                                                const void*             pModel,
                                                      size_t            index,
                                                      CSR_Buffer*       pBuffer);

        /**
        * Writes a vertex format inside a buffer
        *@param pContext - write context, containing the write options
        *@param pVF - vertex format to write
        *@param[in, out] pBuffer - buffer to write in
        *@return 1 on success, otherwise 0
        */
        int csrSerializerWriteVF(const CSR_WriteContext* pContext,
                                 const CSR_VertexFormat* pVF,
                                       CSR_Buffer*       pBuffer);

        /**
        * Writes a vertex culling inside a buffer
        *@param pContext - write context, containing the write options
        *@param pVC - vertex culling to write
        *@param[in, out] pBuffer - buffer to write in
        *@return 1 on success, otherwise 0
        */
        int csrSerializerWriteVC(const CSR_WriteContext*  pContext,
                                 const CSR_VertexCulling* pVC,
                                       CSR_Buffer*        pBuffer);

        /**
        * Writes a vertex buffer inside a buffer
        *@param pContext - write context, containing the write options
        *@param pVB - vertex buffer to write
        *@param[in, out] pBuffer - buffer to write in
        *@return 1 on success, otherwise 0
        */
        int csrSerializerWriteVB(const CSR_WriteContext* pContext,
                                 const CSR_VertexBuffer* pVB,
                                       CSR_Buffer*       pBuffer);

        /**
        * Writes a mesh inside a buffer
        *@param pContext - write context, containing the write options
        *@param pMesh - mesh to write
        *@param[in, out] pBuffer - buffer to write in
        *@return 1 on success, otherwise 0
        */
        int csrSerializerWriteMesh(const CSR_WriteContext* pContext,
                                   const CSR_Mesh*         pMesh,
                                         CSR_Buffer*       pBuffer);

        /**
        * Writes a model inside a buffer
        *@param pContext - write context, containing the write options
        *@param pModel - model to write
        *@param[in, out] pBuffer - buffer to write in
        *@return 1 on success, otherwise 0
        */
        int csrSerializerWriteModel(const CSR_WriteContext* pContext,
                                    const CSR_Model*        pModel,
                                          CSR_Buffer*       pBuffer);

        /**
        * Writes a model animation inside a buffer
        *@param pContext - write context, containing the write options
        *@param pModelAnim - model animation to write
        *@param[in, out] pBuffer - buffer to write in
        *@return 1 on success, otherwise 0
        */
        int csrSerializerWriteModelAnimation(const CSR_WriteContext*   pContext,
                                             const CSR_ModelAnimation* pModelAnim,
                                                   CSR_Buffer*         pBuffer);

        /**
        * Writes a MDL model inside a buffer
        *@param pContext - write context, containing the write options
        *@param pMDL - MDL model to write
        *@param[in, out] pBuffer - buffer to write in
        *@return 1 on success, otherwise 0
        */
        int csrSerializerWriteMDL(const CSR_WriteContext* pContext,
                                  const CSR_MDL*          pMDL,
                                        CSR_Buffer*       pBuffer);

        /**
        * Writes a matrix item inside a buffer
        *@param pContext - write context, containing the write options
        *@param pMatrixItem - matrix item to write
        *@param[in, out] pBuffer - buffer to write in
        *@return 1 on success, otherwise 0
        */
        int csrSerializerWriteMatrixItem(const CSR_WriteContext* pContext,
                                         const CSR_MatrixItem*   pMatrixItem,
                                               CSR_Buffer*       pBuffer);

        /**
        * Writes a matrix list inside a buffer
        *@param pContext - write context, containing the write options
        *@param pMatrixList - matrix list to write
        *@param[in, out] pBuffer - buffer to write in
        *@return 1 on success, otherwise 0
        */
        int csrSerializerWriteMatrixList(const CSR_WriteContext* pContext,
                                         const CSR_MatrixList*   pMatrixList,
                                               CSR_Buffer*       pBuffer);

        /**
        * Writes a scene item inside a buffer
        *@param pContext - write context, containing the write options
        *@param pSceneItem - scene item to write
        *@param transparent - if 1, the scene item is transparent
        *@param[in, out] pBuffer - buffer to write in
        *@return 1 on success, otherwise 0
        */
        int csrSerializerWriteSceneItem(const CSR_WriteContext* pContext,
                                        const CSR_SceneItem*    pSceneItem,
                                              int               transparent,
                                              CSR_Buffer*       pBuffer);

        /**
        * Writes a scene inside a buffer
        *@param pContext - write context, containing the write options
        *@param pScene - scene to write
        *@param[in, out] pBuffer - buffer to write in
        *@return 1 on success, otherwise 0
        */
        int csrSerializerWriteScene(const CSR_WriteContext* pContext,
                                    const CSR_Scene*        pScene,
                                          CSR_Buffer*       pBuffer);

#ifdef __cplusplus
    }
#endif

//---------------------------------------------------------------------------
// Compiler
//---------------------------------------------------------------------------

// needed in mobile c compiler to link the .h file with the .c
#if defined(_OS_IOS_) || defined(_OS_ANDROID_) || defined(_OS_WINDOWS_)
    #include "CSR_Serializer.c"
#endif

#endif