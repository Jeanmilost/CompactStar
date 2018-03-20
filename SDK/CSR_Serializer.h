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
} CSR_SceneFileHeader;

#ifdef __cplusplus
    extern "C"
    {
#endif
        //-------------------------------------------------------------------
        // Write functions
        //-------------------------------------------------------------------

        /**
        * Writes a header in the buffer
        *@param pID - header identifier, must always be a string containing 4 chars
        *@param dataSize - size of data the chunk will contain, in bytes
        *@param[in, out] pBuffer - buffer to write in
        */
        int csrSerializerWriteHeader(const char* pID, size_t dataSize, CSR_Buffer* pBuffer);

        /**
        * Writes a material inside a buffer
        *@param pMaterial - material to write
        *@param[in, out] pBuffer - buffer to write in
        *@return 1 on success, otherwise 0
        */
        int csrSerializerWriteMaterial(const CSR_Material* pMaterial, CSR_Buffer* pBuffer);

        /**
        * Writes a texture inside a buffer
        *@param pTexture - texture to write
        *@param[in, out] pBuffer - buffer to write in
        *@return 1 on success, otherwise 0
        */
        int csrSerializerWriteTexture(const CSR_Buffer* pTexture, CSR_Buffer* pBuffer);

        /**
        * Writes a bumpmap inside a buffer
        *@param pBumpMap - bumpmap texture to write
        *@param[in, out] pBuffer - buffer to write in
        *@return 1 on success, otherwise 0
        */
        int csrSerializerWriteBumpMap(const CSR_Buffer* pBumpMap, CSR_Buffer* pBuffer);

        /**
        * Writes a vertex format inside a buffer
        *@param pVF - vertex format to write
        *@param[in, out] pBuffer - buffer to write in
        *@return 1 on success, otherwise 0
        */
        int csrSerializerWriteVF(const CSR_VertexFormat* pVF, CSR_Buffer* pBuffer);

        /**
        * Writes a vertex culling inside a buffer
        *@param pVC - vertex culling to write
        *@param[in, out] pBuffer - buffer to write in
        *@return 1 on success, otherwise 0
        */
        int csrSerializerWriteVC(const CSR_VertexCulling* pVC, CSR_Buffer* pBuffer);

        /**
        * Writes a vertex buffer inside a buffer
        *@param pVB - vertex buffer to write
        *@param[in, out] pBuffer - buffer to write in
        *@return 1 on success, otherwise 0
        */
        int csrSerializerWriteVB(const CSR_VertexBuffer* pVB, CSR_Buffer* pBuffer);

        /**
        * Writes a mesh inside a buffer
        *@param pMesh - mesh to write
        *@param pTexture - texture belonging to mesh, if 0 no texture will be saved
        *@param pBumpMap - bumpmap texture belonging to mesh, if 0 no bumpmap texture will be saved
        *@param[in, out] pBuffer - buffer to write in
        *@return 1 on success, otherwise 0
        */
        int csrSerializerWriteMesh(const CSR_Mesh*   pMesh,
                                   const CSR_Buffer* pTexture,
                                   const CSR_Buffer* pBumpMap,
                                         CSR_Buffer* pBuffer);

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